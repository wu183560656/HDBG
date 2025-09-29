#include <ntifs.h>
#include <hvm\hvm_lib\loader.h>

static PMDL _HvmHeapPMdl = NULL;
static PVOID _HvmHeapBaseAddress = NULL;
static HANDLE _hSerialFileHandle = NULL;
static USHORT _uSerialPort = 0;
//查找并锁定有效串口
static unsigned short FindAndLockSerialPort()
{
    const struct {
        const wchar_t* FileName;
        USHORT BasePort;
    }SerialList[] = {
        {L"\\Device\\Serial0",0x3F8},
        {L"\\Device\\Serial1",0x2F8},
        {L"\\Device\\Serial2",0x3E8},
        {L"\\Device\\Serial3",0x2E8},
    };

    if (!_uSerialPort)
    {
        NTSTATUS status;
        UNICODE_STRING PathName;
        OBJECT_ATTRIBUTES oa{};
        IO_STATUS_BLOCK iostatus;
        //尝试填写输出串口
        for (int i = 0; i < sizeof(SerialList) / sizeof(*SerialList); i++)
        {
            RtlInitUnicodeString(&PathName, SerialList[i].FileName);
            InitializeObjectAttributes(&oa, &PathName, OBJ_CASE_INSENSITIVE, NULL, NULL);
            status = ZwCreateFile(&_hSerialFileHandle, GENERIC_WRITE | GENERIC_READ, &oa, &iostatus, NULL, 0, FILE_SHARE_WRITE | FILE_SHARE_READ, FILE_OPEN, FILE_SYNCHRONOUS_IO_NONALERT, NULL, 0);
            if (NT_SUCCESS(status))
            {
                _uSerialPort = SerialList[i].BasePort;
                break;
            }
        }
    }
    return _uSerialPort;
}
//释放串口
static void UnLockSerialPort()
{
    if (_hSerialFileHandle)
    {
        ZwClose(_hSerialFileHandle);
        _hSerialFileHandle = NULL;
        _uSerialPort = 0;
    }
}

static BOOLEAN ExecuteOnEachProcessor(BOOLEAN(*Function)(PVOID), PVOID Context)
{
    BOOLEAN Result = TRUE;
    PROCESSOR_NUMBER processorNumber;
    GROUP_AFFINITY affinity{};
    GROUP_AFFINITY oldAffinity;
    ULONG CpuCount = KeQueryActiveProcessorCountEx(0);
    for (ULONG cpu_index = 0; cpu_index < CpuCount && Result; cpu_index++)
    {
        KeGetProcessorNumberFromIndex(cpu_index, &processorNumber);
        affinity.Group = processorNumber.Group;
        affinity.Mask = 1ULL << processorNumber.Number;
        affinity.Reserved[0] = affinity.Reserved[1] = affinity.Reserved[2] = 0;
        KeSetSystemGroupAffinityThread(&affinity, &oldAffinity);
        Result = Function(Context);
        KeRevertToUserGroupAffinityThread(&oldAffinity);
    }
    return Result;
}

NTSTATUS LoadHVM(PVOID ImageBase, SIZE_T ImageSize)
{
    NTSTATUS Status = -1;
    do
    {
        //申请Kernel需要的堆内存
        //每个核需要10MB
        USHORT ActiveProcessorCount = (USHORT)KeQueryActiveProcessorCountEx(ALL_PROCESSOR_GROUPS);
        ULONG64 HvmHeapSize = 0x100000ULL * 10 * ActiveProcessorCount;
        PHYSICAL_ADDRESS LowAddress{};
        PHYSICAL_ADDRESS HighAddress{};
        PHYSICAL_ADDRESS SkipBytes{};
        LowAddress.QuadPart = 0;
        HighAddress.QuadPart = -1;
        SkipBytes.QuadPart = 0x200000ULL;
        _HvmHeapPMdl = MmAllocatePagesForMdlEx(LowAddress, HighAddress, SkipBytes, HvmHeapSize, MmCached, MM_ALLOCATE_FULLY_REQUIRED | MM_ALLOCATE_REQUIRE_CONTIGUOUS_CHUNKS);
        if (_HvmHeapPMdl)
        {
            _HvmHeapBaseAddress = MmMapLockedPagesSpecifyCache(_HvmHeapPMdl, KernelMode, MmNotMapped, NULL, FALSE, 0);
            if (!_HvmHeapBaseAddress)
            {
                MmFreePagesFromMdl(_HvmHeapPMdl);
                _HvmHeapPMdl = NULL;
            }
        }
        if (!_HvmHeapPMdl || !_HvmHeapBaseAddress)
        {
            Status = STATUS_MEMORY_NOT_ALLOCATED;
            break;
        }
        //初始化Kernel
        if (!HvmInitialize(ImageBase, (UINT32)ImageSize, _HvmHeapBaseAddress, HvmHeapSize
            , [](void* VirtualAddress)->unsigned __int64 {
                ULONG64 Result = MmGetPhysicalAddress(VirtualAddress).QuadPart;
                if (!Result)
                {
                    Result = (ULONG64)-1;
                }
                return Result;
            }
            , [](unsigned __int64 PhysicalAddress)->void* {
                PHYSICAL_ADDRESS PhysicalAddressValue{};
                PhysicalAddressValue.QuadPart = PhysicalAddress;
                PVOID Result = MmGetVirtualForPhysical(PhysicalAddressValue);
                return Result;
            }
            , [](const char* text)->void {
                DbgPrintEx(0, 0, text);
            }
            , FindAndLockSerialPort()))
        {
            break;
        }
        //让每个处理器都进入虚拟化
        if (!ExecuteOnEachProcessor([](PVOID)->BOOLEAN {
            return HvmStartTheProcessor();
            }, NULL)) {
            break;
        }
        //初始化调试器
        Status = STATUS_SUCCESS;
    } while (false);
    return Status;
}
void UnLoadHVM()
{
    //所有核心停止虚拟化
    ExecuteOnEachProcessor([](PVOID)->BOOLEAN {
        return HvmStopTheProcessor();
        }, NULL);
    //释放堆内存
    if (_HvmHeapPMdl)
    {
        if (_HvmHeapBaseAddress)
        {
            MmUnmapLockedPages(_HvmHeapBaseAddress, _HvmHeapPMdl);
            _HvmHeapBaseAddress = NULL;
        }
        MmFreePagesFromMdl(_HvmHeapPMdl);
        _HvmHeapPMdl = NULL;
    }
    UnLockSerialPort();
}