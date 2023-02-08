
#pragma once

#ifdef __cplusplus
extern "C"
{
#endif
#include <ntddk.h>
#ifdef  __cplusplus
}
#endif // __cplusplus
#include "Ioctls.h"

#define PAGEDCODE code_seg("PAGE")
#define LOCKEDCODE code_seg()
#define INITCODE code_seg("INIT")

#define PAGEDDATA data_seg("PAGE")
#define LOCKEDDATA data_seg()
#define INITDATA data_seg("INIT")

#define arraysize(p) (sizeof(p)/sizeof((p)[0]))

#define MAX_FILE_LENGTH 1024

typedef struct _DEVICE_EXTENSION {
	PDEVICE_OBJECT pDevice;
	UNICODE_STRING ustrDeviceName;	//设备名称
	UNICODE_STRING ustrSymLinkName;	//符号链接名

	PUCHAR buffer; // 缓冲区
	ULONG file_length; // 模拟的文件长度，必须小于MAX_FILE_LENGTH
} DEVICE_EXTENSION, * PDEVICE_EXTENSION;


NTSTATUS CreateDevice(IN PDRIVER_OBJECT pDriverObject);

VOID HelloDDKUnload(IN PDRIVER_OBJECT pDriverObject);

NTSTATUS HelloDDKDispatchRoutin(IN PDEVICE_OBJECT pDevObj, IN PIRP pIrp);

NTSTATUS HelloDDKDeviceIOControl(IN PDEVICE_OBJECT pDevObj, IN PIRP pIrp);

NTSTATUS HelloDDKRead(IN PDEVICE_OBJECT pDevObj, IN PIRP pIrp);

NTSTATUS HelloDDWrite(IN PDEVICE_OBJECT pDevObj, IN PIRP pIrp);

NTSTATUS HelloDDKQueryInfomation(IN PDEVICE_OBJECT pDevObj, IN PIRP pIrp);