#include <ntifs.h>

static VOID DriverUnload(struct _DRIVER_OBJECT* DriverObject)
{
    UNREFERENCED_PARAMETER(DriverObject);
    return;
}

extern"C" NTSTATUS DriverEntry(PDRIVER_OBJECT DriverObject, PUNICODE_STRING RegistryPath)
{
    DriverObject->DriverUnload = DriverUnload;
    UNREFERENCED_PARAMETER(RegistryPath);

    return STATUS_SUCCESS;
}