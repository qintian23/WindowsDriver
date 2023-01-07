#include <ntifs.h>

VOID UnloadDriver(PDRIVER_OBJECT pDriverObject)
{
	KdPrint(("Unload Driver success. This is my driver test program!!! Hahaha :)"));
}

NTSTATUS DriverEntry(PDRIVER_OBJECT pDriverObject, PUNICODE_STRING pRegistryPath)
{
	KdPrint(("DriverPath:%wZ", pRegistryPath));
	KdPrint(("Hello myDriver,  This is my driver test program!!! Hahaha :)"));
	pDriverObject->DriverUnload = UnloadDriver;
	return STATUS_SUCCESS;
}