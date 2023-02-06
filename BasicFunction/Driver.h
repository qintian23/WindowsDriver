
#pragma once

#ifdef  __cplusplus
extern "C"
{
#endif
#include <ntddk.h>
#ifdef __cplusplus
}
#endif

typedef struct _DEVICE_EXTENSION
{
	PDEVICE_OBJECT pDevice;
	UNICODE_STRING ustrDeviceName;	//设备名称
	UNICODE_STRING ustrSymLinkName;	//符号链接名
}DEVICE_EXTENSION, * PDEVICE_EXTENSION;

#define PAGEDCODE code_seg("PAGE");
#define LOCKEDCODE code_seg();
#define INITCODE code_seg("INIT");

#define PAGEDDATA data_seg("PAGE");
#define LOCKEDDATA data_seg();
#define INITDATA data_seg("INIT");

#define arraysize(p) (sizeof(p)/sizeof(p)[0])

NTSTATUS HelloWDMDispatchRoutine(IN PDEVICE_OBJECT fdo, IN PIRP Irp);

void HelloWDMUnload(IN PDRIVER_OBJECT DriverObject);

NTSTATUS CreateDevice(IN PDRIVER_OBJECT DriverObject);