
#include "HelloWDM.h"

/************************************************************************************************
* �������ƣ�DriverEntry
* ������������ʼ���������򣬶�λ������Ӳ����Դ�������ں˶���
* �����б�
		pDriverObject����I/O�������д���������������
		pRegistryPath������������ע����е�·��
* ����ֵ�����س�ʼ������״̬
*************************************************************************************************/
#pragma INITCODE
extern "C" NTSTATUS DriverEntry(IN PDRIVER_OBJECT pDriverObject, 
								IN PUNICODE_STRING pRegistryPath)
{
	KdPrint(("Enter DriverEntry\n"));
	// 1.�����豸
	pDriverObject->DriverExtension->AddDevice = HelloWDMAddDevice;

	// 2.�ַ�����
	pDriverObject->MajorFunction[IRP_MJ_PNP] = HelloWDMPnp;
	pDriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL] =
		pDriverObject->MajorFunction[IRP_MJ_CREATE] =
		pDriverObject->MajorFunction[IRP_MJ_READ] =
		pDriverObject->MajorFunction[IRP_MJ_WRITE] = HelloWDMDispatchRoutine;

	// 3.ж������
	pDriverObject->DriverUnload = HelloWDMUnload;

	KdPrint(("Leave DriverEntry\n"));
	return STATUS_SUCCESS;
}

/************************************************************************************************
* �������ƣ�HelloWDMAddDevice
* ����������������豸
* �����б�
		DriverObject����I/O�������д���������������
		PhysicalDeviceObject����I/O�������д������������豸
* ����ֵ������������豸״̬
*************************************************************************************************/
#pragma PAGEDCODE
NTSTATUS HelloWDMAddDevice(IN PDRIVER_OBJECT DriverObject, 
							IN PDEVICE_OBJECT PhysicalDeviceObject)
{
	PAGED_CODE();
	KdPrint(("Enter HelloWDMAddDevice\n"));

	NTSTATUS status;
	PDEVICE_OBJECT fdo;
	UNICODE_STRING devName;
	RtlInitUnicodeString(&devName, L"\\Device\\MyWDMDevice");
	// 1.�����豸
	status = IoCreateDevice(
		DriverObject,
		sizeof(DEVICE_EXTENSION),
		&(UNICODE_STRING)devName,
		FILE_DEVICE_UNKNOWN,
		0,
		FALSE,
		&fdo
	);
	if (!NT_SUCCESS(status))
	{
		return status;
	}
	PDEVICE_EXTENSION pdx = (PDEVICE_EXTENSION)fdo->DeviceExtension;
	pdx->fdo = fdo;
	// 2.��ӵ��豸ջ
	pdx->NextStackDevice = IoAttachDeviceToDeviceStack(fdo, PhysicalDeviceObject);
	UNICODE_STRING symLinkName;
	RtlInitUnicodeString(&symLinkName, L"\\Device\\MyWDMDevice");

	// 3.��ӷ�������
	pdx->ustrDeviceName = devName;
	pdx->ustrSymLinkName = symLinkName;
	status = IoCreateSymbolicLink(&(UNICODE_STRING)symLinkName, &(UNICODE_STRING)devName);
	if (!NT_SUCCESS(status))
	{
		IoDeleteSymbolicLink(&pdx->ustrSymLinkName);
		status = IoCreateSymbolicLink(&symLinkName, &devName);
		if (!NT_SUCCESS(status))
		{
			return status;
		}
	}

	fdo->Flags |= DO_BUFFERED_IO | DO_POWER_PAGABLE;
	fdo->Flags &= ~DO_DEVICE_INITIALIZING;

	KdPrint(("Leave HelloWDMAddDevice\n"));
	return STATUS_SUCCESS;
}

/************************************************************************************************
* �������ƣ�DefaultPnpHandler
* ������������PNP IRP����ȱʡ����
* �����б�
		pdx�������豸����
		Irp����IO�����
* ����ֵ������״̬
*************************************************************************************************/
#pragma PAGECODE
NTSTATUS DefaultPnpHandler(PDEVICE_EXTENSION pdx, PIRP Irp)
{
	PAGED_CODE();
	KdPrint(("Enter DefaultPnpHandler\n"));
	IoSkipCurrentIrpStackLocation(Irp);
	KdPrint(("Leave DefaultPnpHandler\n"));
	return IoCallDriver(pdx->NextStackDevice, Irp);
}

/************************************************************************************************
* �������ƣ�HandleRemoveDevice
* ������������IRP_MN_REMOVE_DEVICE_IRP���д���
* �����б�
		pdx�������豸����
		Irp����IO�����
* ����ֵ������״̬
*************************************************************************************************/
#pragma PAGECODE
NTSTATUS HandleRemoveDevice(PDEVICE_EXTENSION pdx, IN PIRP Irp)
{
	PAGED_CODE();
	KdPrint(("Enter HandleRemoveDevice\n"));

	Irp->IoStatus.Status = STATUS_SUCCESS;
	NTSTATUS status = DefaultPnpHandler(pdx, Irp);
	IoDeleteSymbolicLink(&(UNICODE_STRING)pdx->ustrSymLinkName);

	// ����IoDeleteDevice() ��fdo���豸ջ���ѿ�
	if (pdx->NextStackDevice)
		IoDeleteDevice(pdx->NextStackDevice);

	// ɾ��fdo
	IoDeleteDevice(pdx->fdo);
	KdPrint(("Leave HandleRemoveDevice\n"));
	return status;
}

/************************************************************************************************
* �������ƣ�HelloWDMDispatchRoutine
* ������������ȱʡIRP���д���
* �����б�
		fdo�������豸����
		Irp����IO�����
* ����ֵ������״̬
*************************************************************************************************/
#pragma PAGECODE
NTSTATUS HelloWDMDispatchRoutine(IN PDEVICE_OBJECT fdo, IN PIRP Irp)
{
	PAGED_CODE();
	KdPrint(("Enter HelloWDMDispatchRoutine\n"));

	Irp->IoStatus.Status = STATUS_SUCCESS;
	Irp->IoStatus.Information = 0; // no bytes xfered
	IoCompleteRequest(Irp, IO_NO_INCREMENT);

	KdPrint(("Leave HelloWDMDispatchRoutine\n"));
	return STATUS_SUCCESS;
}

/************************************************************************************************
* �������ƣ�HelloWDMPnp
* �����������Լ��弴��IRP���д���
* �����б�
		fdo�������豸����
		Irp����IO�����
* ����ֵ������״̬
*************************************************************************************************/
#pragma PAGECODE
NTSTATUS HelloWDMPnp(IN PDEVICE_OBJECT fdo, IN PIRP Irp)
{
	PAGED_CODE();

	KdPrint(("Enter HelloWDMPnp\n"));
	NTSTATUS status = STATUS_SUCCESS;
	PDEVICE_EXTENSION pdx = (PDEVICE_EXTENSION)fdo->DeviceExtension;
	PIO_STACK_LOCATION stack = IoGetCurrentIrpStackLocation(Irp);
	static NTSTATUS (*fcntab[])(PDEVICE_EXTENSION pdx, PIRP Irp) =
	{
		DefaultPnpHandler,	// IRP_MN_START_DEVICE
		DefaultPnpHandler,	// IRP_MN_QUERY_REMOVE_DEVICE
		HandleRemoveDevice, // IRP_MN_REMOVE_DEVICE
		DefaultPnpHandler,	// IRP_MN_CANCEL_REMOVE_DEVICE
		DefaultPnpHandler,	// IRP_MN_STOP_DEVICE
		DefaultPnpHandler,	// IRP_MN_QUERY_STOP_DEVICE
		DefaultPnpHandler,	// IRP_MN_CANCEL_STOP_DEVICE
		DefaultPnpHandler,	// IRP_MN_QUERY_DEVICE_RELATIONS
		DefaultPnpHandler,	// IRP_MN_QUERY_INTERFACE
		DefaultPnpHandler,	// IRP_MN_QUERY_CAPABILITIES
		DefaultPnpHandler,	// IRP_MN_QUERY_RESOURCES
		DefaultPnpHandler,	// IRP_MN_QUERY_RESOURCES_REQUIREMENTS
		DefaultPnpHandler,	// IRP_MN_QUERY_DEVICE_EXIT
		DefaultPnpHandler,	// IRP_MN_FILTER_RESOURCE_REQUIREMENTS
		DefaultPnpHandler,	// 
		DefaultPnpHandler,	// IRP_MN_READ_CONFIG
		DefaultPnpHandler,	// IRP_MN_WRITE_CONFIG
		DefaultPnpHandler,	// IRP_MN_EJECT
		DefaultPnpHandler,	// IRP_MN_SET_LOCK
		DefaultPnpHandler,	// IRP_MN_QUERY_ID
		DefaultPnpHandler,	// IRP_MN_QUERY_PNP_DEVICE_STATE
		DefaultPnpHandler,	// IRP_MN_QUERY_BUS_INFORMATION
		DefaultPnpHandler,	// IRP_MN_DEVICE_USAGE_NOTIFICATION
		DefaultPnpHandler,	// IRP_MN_SURPRISE_REMOVAL
	};

	KdPrint(("Leave HelloWDMPnp\n"));
	return STATUS_SUCCESS;
}


/************************************************************************************************
* �������ƣ�HelloWDMUnload
* �����������������������ж�ز���
* �����б�
		DriverObject����������
* ����ֵ����
*************************************************************************************************/
#pragma PAGEDCODE
void HelloWDMUnload(IN PDRIVER_OBJECT DriverObject)
{
	PAGED_CODE();
	KdPrint(("Enter HellooWDMUnload\n"));
	KdPrint(("Leave HellooWDMUnload\n"));
}

