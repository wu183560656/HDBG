#include "dbg.h"
#include <ntifs.h>
#include <hvm\include\sdk.h>
#include <intrin.h>

struct BREAK_CONTEXT
{
	ULONG64 rax, rcx, rdx, rbx, reserve, rbp, rsi, rdi;
	ULONG64 r8, r9, r10, r11, r12, r13, r14, r15;
};
struct BREAK_FRAME
{
	ULONG64 rip;
	ULONG64 cs;
	ULONG64 rflags;
	ULONG64 rsp;
	ULONG64 ss;
};
extern"C" void OnBreakPointCallbackA();
extern"C" void OnBreakPointCallback(BREAK_CONTEXT* pContext, BREAK_FRAME* pFrame)
{
	(pContext);
	(pFrame);
}

namespace dbg
{
	//所有DebugPoint列表
	static PVOID DebugPointList[0x10000 / 4] = { NULL };


	bool Initialize(DBGINIT_INFORMATION* pInformation)
	{
		if (
			!pInformation->ObpReferenceObjectByHandleWithTag_Address ||
			!pInformation->EPROCESS$DebugPort_Offset || !pInformation->ETHREAD$HideFromDebugger_Offset ||
			!pInformation->DbgkCreateThread_DebugPoint.Address || !pInformation->DbgkExitThread_DebugPoint.Address ||
			!pInformation->DbgkExitThread_DebugPoint.Address || !pInformation->DbgkExitThread_DebugPoint.Address ||
			!pInformation->DbgkMapViewOfSection_DebugPoint.Address || !pInformation->DbgkUnMapViewOfSection_DebugPoint.Address ||
			!pInformation->DbgkUnMapViewOfSection_DebugPoint.Address || !pInformation->DbgkUnMapViewOfSection_DebugPoint.Address ||
			!pInformation->DbgkExitProcess_DebugPoint.Address || !pInformation->DbgkpQueueMessage_DebugPoint.Address ||
			!pInformation->DbgkpQueueMessage_DebugPoint.Address || !pInformation->DbgkpQueueMessage_DebugPoint.Address ||
			!pInformation->DbgkForwardException_DebugPoint.Address || !pInformation->DbgkForwardException_DebugPoint.Address ||
			!pInformation->DbgkForwardException_HideFromDebugger.Address || !pInformation->DbgkForwardException_HideFromDebugger.Address
			)
		{
			return false;
		}
		HVM_BREAKPOINT_INFORMATION BreakPoint{ 0 };
		BreakPoint.Attribute.Execute = 1;
		BreakPoint.CbType = 1;
		BreakPoint.Jmp.Rip = (unsigned __int64)OnBreakPointCallbackA;
		//添加断点
		BreakPoint.VirtualAddress = pInformation->ObpReferenceObjectByHandleWithTag_Address;
		BreakPoint.Size = 1;
		HvmAddBreakPoint(&BreakPoint);
		BreakPoint.VirtualAddress = pInformation->DbgkCreateThread_DebugPoint.Address;
		BreakPoint.Size = (unsigned char)pInformation->DbgkCreateThread_DebugPoint.Size;
		HvmAddBreakPoint(&BreakPoint);
		BreakPoint.VirtualAddress = pInformation->DbgkExitThread_DebugPoint.Address;
		BreakPoint.Size = (unsigned char)pInformation->DbgkExitThread_DebugPoint.Size;
		HvmAddBreakPoint(&BreakPoint);
		BreakPoint.VirtualAddress = pInformation->DbgkMapViewOfSection_DebugPoint.Address;
		BreakPoint.Size = (unsigned char)pInformation->DbgkMapViewOfSection_DebugPoint.Size;
		HvmAddBreakPoint(&BreakPoint);
		BreakPoint.VirtualAddress = pInformation->DbgkUnMapViewOfSection_DebugPoint.Address;
		BreakPoint.Size = (unsigned char)pInformation->DbgkUnMapViewOfSection_DebugPoint.Size;
		HvmAddBreakPoint(&BreakPoint);
		BreakPoint.VirtualAddress = pInformation->DbgkExitProcess_DebugPoint.Address;
		BreakPoint.Size = (unsigned char)pInformation->DbgkExitProcess_DebugPoint.Size;
		HvmAddBreakPoint(&BreakPoint);
		BreakPoint.VirtualAddress = pInformation->DbgkpQueueMessage_DebugPoint.Address;
		BreakPoint.Size = (unsigned char)pInformation->DbgkpQueueMessage_DebugPoint.Size;
		HvmAddBreakPoint(&BreakPoint);
		BreakPoint.VirtualAddress = pInformation->DbgkForwardException_DebugPoint.Address;
		BreakPoint.Size = (unsigned char)pInformation->DbgkForwardException_DebugPoint.Size;
		HvmAddBreakPoint(&BreakPoint);
		BreakPoint.VirtualAddress = pInformation->DbgkForwardException_HideFromDebugger.Address;
		BreakPoint.Size = (unsigned char)pInformation->DbgkForwardException_HideFromDebugger.Size;
		HvmAddBreakPoint(&BreakPoint);


		return false;
	}
	void Uninitialize()
	{

	}
};
