/************************************************************************************************
* 文件名称：HelloWDM.h
* 作   者：朱柳承
* 完成日期：2023-1-7
*************************************************************************************************/
#pragma once

#ifdef __cplusplus
extern "C"
{
#endif
#include <wdm.h>
#ifdef __cplusplus
}
#endif // __cplusplus


typedef struct _DEVICE_EXTENSION
{
	PDEVICE_OBJECT fdo;
	PDEVICE_OBJECT NextStackDevice;
	UNICODE_STRING ustrDeviceName; // 设备名
	UNICODE_STRING ustrSymLinkName; // 符号连接名
}DEVICE_EXTENSION, *PDEVICE_EXTENSION;

#define PAGEDCODE code_seg("PAGE")
#define LOCKDCODE code_seg()
#define INITCODE code_seg("INIT")

#define PAGEDDATA data_seg("PAGE")
#define LOCKDDATA data_seg()
#define INITDATA data_seg("INIT")

#define arraysize(p) (sizeof(p)/sizeof((p)[0]))

NTSTATUS HelloWDMAddDevice(IN PDRIVER_OBJECT DriverObject, IN PDEVICE_OBJECT PhysicalDeviceObject);

NTSTATUS HelloWDMPnp(IN PDEVICE_OBJECT fdo, IN PIRP Irp);

NTSTATUS HelloWDMDispatchRoutine(IN PDEVICE_OBJECT fdo, IN PIRP Irp);

void HelloWDMUnload(IN PDRIVER_OBJECT DriverObject);

extern "C"
NTSTATUS DriverEntry(IN PDRIVER_OBJECT DriverObject, IN PUNICODE_STRING RegistryPath);

