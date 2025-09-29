#include <ntifs.h>

#include "hvm_loader.h"
#include "dbg.h"


static VOID DriverUnload(struct _DRIVER_OBJECT* DriverObject)
{
    UNREFERENCED_PARAMETER(DriverObject);
    return;
}

extern"C" NTSTATUS DriverEntry(PDRIVER_OBJECT DriverObject, PUNICODE_STRING RegistryPath)
{
    UNREFERENCED_PARAMETER(RegistryPath);
    if (DriverObject)
    {
        DriverObject->DriverUnload = DriverUnload;
    }
    NTSTATUS Status = LoadHVM(DriverObject->DriverStart, DriverObject->DriverSize);
    if (!NT_SUCCESS(Status))
    {
        return Status;
    }
    return Status;
}