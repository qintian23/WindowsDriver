
#include "HelloWDM.h"

/************************************************************************************************
* 函数名称：DriverEntry
* 功能描述：初始化驱动程序，定位和申请硬件资源，创建内核对象
* 参数列表：
		pDriverObject：从I/O管理器中传进来的驱动对象
		pRegistryPath：驱动程序在注册表中的路径
* 返回值：返回初始化驱动状态
*************************************************************************************************/
#pragma INITCODE
extern "C" NTSTATUS DriverEntry(IN PDRIVER_OBJECT pDriverObject, 
								IN PUNICODE_STRING pRegistryPath)
{
	KdPrint(("Enter DriverEntry\n"));
	// 1.创建设备
	pDriverObject->DriverExtension->AddDevice = HelloWDMAddDevice;

	// 2.分发例程
	pDriverObject->MajorFunction[IRP_MJ_PNP] = HelloWDMPnp;
	pDriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL] =
		pDriverObject->MajorFunction[IRP_MJ_CREATE] =
		pDriverObject->MajorFunction[IRP_MJ_READ] =
		pDriverObject->MajorFunction[IRP_MJ_WRITE] = HelloWDMDispatchRoutine;

	// 3.卸载例程
	pDriverObject->DriverUnload = HelloWDMUnload;

	KdPrint(("Leave DriverEntry\n"));
	return STATUS_SUCCESS;
}

/************************************************************************************************
* 函数名称：HelloWDMAddDevice
* 功能描述：添加新设备
* 参数列表：
		DriverObject：从I/O管理器中传进来的驱动对象
		PhysicalDeviceObject：从I/O管理器中传进来的物理设备
* 返回值：返回添加新设备状态
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
	// 1.创建设备
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
	// 2.添加到设备栈
	pdx->NextStackDevice = IoAttachDeviceToDeviceStack(fdo, PhysicalDeviceObject);
	UNICODE_STRING symLinkName;
	RtlInitUnicodeString(&symLinkName, L"\\Device\\MyWDMDevice");

	// 3.添加符号连接
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
* 函数名称：DefaultPnpHandler
* 功能描述：对PNP IRP进行缺省处理
* 参数列表：
		pdx：功能设备对象
		Irp：从IO请求包
* 返回值：返回状态
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
* 函数名称：HandleRemoveDevice
* 功能描述：对IRP_MN_REMOVE_DEVICE_IRP进行处理
* 参数列表：
		pdx：功能设备对象
		Irp：从IO请求包
* 返回值：返回状态
*************************************************************************************************/
#pragma PAGECODE
NTSTATUS HandleRemoveDevice(PDEVICE_EXTENSION pdx, IN PIRP Irp)
{
	PAGED_CODE();
	KdPrint(("Enter HandleRemoveDevice\n"));

	Irp->IoStatus.Status = STATUS_SUCCESS;
	NTSTATUS status = DefaultPnpHandler(pdx, Irp);
	IoDeleteSymbolicLink(&(UNICODE_STRING)pdx->ustrSymLinkName);

	// 调用IoDeleteDevice() 把fdo从设备栈中脱开
	if (pdx->NextStackDevice)
		IoDeleteDevice(pdx->NextStackDevice);

	// 删除fdo
	IoDeleteDevice(pdx->fdo);
	KdPrint(("Leave HandleRemoveDevice\n"));
	return status;
}

/************************************************************************************************
* 函数名称：HelloWDMDispatchRoutine
* 功能描述：对缺省IRP进行处理
* 参数列表：
		fdo：功能设备对象
		Irp：从IO请求包
* 返回值：返回状态
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
* 函数名称：HelloWDMPnp
* 功能描述：对即插即用IRP进行处理
* 参数列表：
		fdo：功能设备对象
		Irp：从IO请求包
* 返回值：返回状态
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
* 函数名称：HelloWDMUnload
* 功能描述：负责驱动程序的卸载操作
* 参数列表：
		DriverObject：驱动对象
* 返回值：空
*************************************************************************************************/
#pragma PAGEDCODE
void HelloWDMUnload(IN PDRIVER_OBJECT DriverObject)
{
	PAGED_CODE();
	KdPrint(("Enter HellooWDMUnload\n"));
	KdPrint(("Leave HellooWDMUnload\n"));
}

