#include <ntifs.h>

VOID UnloadDriver(PDRIVER_OBJECT pDriverObject)
{
	KdPrint(("Unload Driver success. This is my driver!"));
}

NTSTATUS DriverEntry(PDRIVER_OBJECT pDriverObject, PUNICODE_STRING pRegistryPath)
{
	KdPrint(("DriverPath:%wZ. This is my driver!", pRegistryPath));
	KdPrint(("Hello my Driver. This is my driver!"));

	pDriverObject->DriverUnload = UnloadDriver;
	return STATUS_SUCCESS;
}