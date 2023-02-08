
#include "Driver.h"

/************************************************************************
* 函数名称:DriverEntry
* 功能描述:初始化驱动程序，定位和申请硬件资源，创建内核对象
* 参数列表:
	  pDriverObject:从I/O管理器中传进来的驱动对象
	  pRegistryPath:驱动程序在注册表的中的路径
* 返回 值:返回初始化驱动状态
*************************************************************************/
#pragma PAGEDCODE
extern "C" NTSTATUS DriverEntry(
	IN PDRIVER_OBJECT pDriverObject,
	IN PUNICODE_STRING pRegistryPath)
{
	NTSTATUS status;
	KdPrint(("Enter DriverEntry\n"));

	//设置卸载函数
	pDriverObject->DriverUnload = HelloDDKUnload;

	//设置派遣函数
	for (int i = 0; i < arraysize(pDriverObject->MajorFunction); ++i)
	{
		pDriverObject->MajorFunction[i] = HelloDDKDispatchRoutin;
	}

	pDriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL] = HelloDDKDeviceIOControl;
	pDriverObject->MajorFunction[IRP_MJ_READ] = HelloDDKRead;

	//创建驱动设备对象
	status = CreateDevice(pDriverObject);

	KdPrint(("Leave DriverEntry\n"));
	return status;
}

/************************************************************************
* 函数名称:CreateDevice
* 功能描述:初始化设备对象
* 参数列表:
	  pDriverObject:从I/O管理器中传进来的驱动对象
* 返回 值:返回初始化状态
*************************************************************************/
#pragma PAGEDCODE
NTSTATUS CreateDevice(
	IN PDRIVER_OBJECT	pDriverObject)
{
	NTSTATUS status;
	PDEVICE_OBJECT pDevObj;
	PDEVICE_EXTENSION pDevExt;

	//创建设备名称
	UNICODE_STRING devName;
	RtlInitUnicodeString(&devName, L"\\Device\\MyDDKDevice");

	//创建设备
	status = IoCreateDevice(pDriverObject,
		sizeof(DEVICE_EXTENSION),
		&(UNICODE_STRING)devName,
		FILE_DEVICE_UNKNOWN,
		0, TRUE,
		&pDevObj);
	if (!NT_SUCCESS(status))
		return status;

	pDevObj->Flags |= DO_BUFFERED_IO;
	pDevExt = (PDEVICE_EXTENSION)pDevObj->DeviceExtension;
	pDevExt->pDevice = pDevObj;
	pDevExt->ustrDeviceName = devName;

	//申请模拟文件的缓冲区
	pDevExt->buffer = (PUCHAR)ExAllocatePool(PagedPool, MAX_FILE_LENGTH);
	//设置模拟文件大小
	pDevExt->file_length = 0;

	//创建符号链接
	UNICODE_STRING symLinkName;
	RtlInitUnicodeString(&symLinkName, L"\\??\\HelloDDK");
	pDevExt->ustrSymLinkName = symLinkName;
	status = IoCreateSymbolicLink(&symLinkName, &devName);
	if (!NT_SUCCESS(status))
	{
		IoDeleteDevice(pDevObj);
		return status;
	}
	return STATUS_SUCCESS;
}

/************************************************************************
* 函数名称:HelloDDKUnload
* 功能描述:负责驱动程序的卸载操作
* 参数列表:
	  pDriverObject:驱动对象
* 返回 值:返回状态
*************************************************************************/
#pragma PAGEDCODE
VOID HelloDDKUnload(IN PDRIVER_OBJECT pDriverObject)
{
	PDEVICE_OBJECT	pNextObj;
	KdPrint(("Enter DriverUnload\n"));
	pNextObj = pDriverObject->DeviceObject;
	while (pNextObj != NULL)
	{
		PDEVICE_EXTENSION pDevExt = (PDEVICE_EXTENSION)
			pNextObj->DeviceExtension;

		if (pDevExt->buffer)
		{
			ExFreePool(pDevExt->buffer);
			pDevExt->buffer = NULL;
		}

		//删除符号链接
		UNICODE_STRING pLinkName = pDevExt->ustrSymLinkName;
		IoDeleteSymbolicLink(&pLinkName);
		pNextObj = pNextObj->NextDevice;
		IoDeleteDevice(pDevExt->pDevice);
	}
}

/************************************************************************
* 函数名称:HelloDDKDispatchRoutin
* 功能描述:对读IRP进行处理
* 参数列表:
	  pDevObj:功能设备对象
	  pIrp:从IO请求包
* 返回 值:返回状态
*************************************************************************/
#pragma PAGEDCODE
NTSTATUS HelloDDKDispatchRoutin(IN PDEVICE_OBJECT pDevObj,
	IN PIRP pIrp)
{
	KdPrint(("Enter HelloDDKDispatchRoutin\n"));

	PIO_STACK_LOCATION stack = IoGetCurrentIrpStackLocation(pIrp);
	//建立一个字符串数组与IRP类型对应起来
	static char* irpname[] =
	{
		"IRP_MJ_CREATE",
		"IRP_MJ_CREATE_NAMED_PIPE",
		"IRP_MJ_CLOSE",
		"IRP_MJ_READ",
		"IRP_MJ_WRITE",
		"IRP_MJ_QUERY_INFORMATION",
		"IRP_MJ_SET_INFORMATION",
		"IRP_MJ_QUERY_EA",
		"IRP_MJ_SET_EA",
		"IRP_MJ_FLUSH_BUFFERS",
		"IRP_MJ_QUERY_VOLUME_INFORMATION",
		"IRP_MJ_SET_VOLUME_INFORMATION",
		"IRP_MJ_DIRECTORY_CONTROL",
		"IRP_MJ_FILE_SYSTEM_CONTROL",
		"IRP_MJ_DEVICE_CONTROL",
		"IRP_MJ_INTERNAL_DEVICE_CONTROL",
		"IRP_MJ_SHUTDOWN",
		"IRP_MJ_LOCK_CONTROL",
		"IRP_MJ_CLEANUP",
		"IRP_MJ_CREATE_MAILSLOT",
		"IRP_MJ_QUERY_SECURITY",
		"IRP_MJ_SET_SECURITY",
		"IRP_MJ_POWER",
		"IRP_MJ_SYSTEM_CONTROL",
		"IRP_MJ_DEVICE_CHANGE",
		"IRP_MJ_QUERY_QUOTA",
		"IRP_MJ_SET_QUOTA",
		"IRP_MJ_PNP",
	};

	UCHAR type = stack->MajorFunction;
	if (type >= arraysize(irpname))
		KdPrint((" - Unknown IRP, major type %X\n", type));
	else
		KdPrint(("\t%s\n", irpname[type]));


	//对一般IRP的简单操作，后面会介绍对IRP更复杂的操作
	NTSTATUS status = STATUS_SUCCESS;
	// 完成IRP
	pIrp->IoStatus.Status = status;
	pIrp->IoStatus.Information = 0;	// bytes xfered
	IoCompleteRequest(pIrp, IO_NO_INCREMENT);

	KdPrint(("Leave HelloDDKDispatchRoutin\n"));

	return status;
}

#pragma PAGEDCODE
NTSTATUS HelloDDKDeviceIOControl(IN PDEVICE_OBJECT pDevObj,
	IN PIRP pIrp)
{
	NTSTATUS status = STATUS_SUCCESS;
	KdPrint(("Enter HelloDDKDeviceIOControl\n"));

	//得到当前堆栈
	PIO_STACK_LOCATION stack = IoGetCurrentIrpStackLocation(pIrp);
	//得到输入缓冲区大小
	ULONG cbin = stack->Parameters.DeviceIoControl.InputBufferLength;
	//得到输出缓冲区大小
	ULONG cbout = stack->Parameters.DeviceIoControl.OutputBufferLength;
	//得到IOCTL码
	ULONG code = stack->Parameters.DeviceIoControl.IoControlCode;

	ULONG info = 0;

	switch (code)
	{						// process request
	case IOCTL_TEST1:
	{
		KdPrint(("IOCTL_TEST1\n"));
		//缓冲区方式IOCTL
		//显示输入缓冲区数据
		UCHAR* InputBuffer = (UCHAR*)pIrp->AssociatedIrp.SystemBuffer;
		for (ULONG i = 0; i < cbin; i++)
		{
			KdPrint(("%X\n", InputBuffer[i]));
		}

		//操作输出缓冲区
		UCHAR* OutputBuffer = (UCHAR*)pIrp->AssociatedIrp.SystemBuffer;
		memset(OutputBuffer, 0xAA, cbout);
		//设置实际操作输出缓冲区长度
		info = cbout;
		break;
	}
	case IOCTL_TEST2:
	{
		KdPrint(("IOCTL_TEST2\n"));
		//缓冲区方式IOCTL
		//显示输入缓冲区数据

		//缓冲区方式IOCTL
		//显示输入缓冲区数据
		UCHAR* InputBuffer = (UCHAR*)pIrp->AssociatedIrp.SystemBuffer;
		for (ULONG i = 0; i < cbin; i++)
		{
			KdPrint(("%X\n", InputBuffer[i]));
		}

		//pIrp->MdlAddress为DeviceIoControl输出缓冲区地址相同
		KdPrint(("User Address:0X%08X\n", MmGetMdlVirtualAddress(pIrp->MdlAddress)));

		UCHAR* OutputBuffer = (UCHAR*)MmGetSystemAddressForMdlSafe(pIrp->MdlAddress, NormalPagePriority);
		//InputBuffer被映射到内核模式下的内存地址，必定在0X80000000-0XFFFFFFFF之间
		memset(OutputBuffer, 0xAA, cbout);
		//设置实际操作输出缓冲区长度
		info = cbout;
		break;
	}
	case IOCTL_TEST3:
	{
		KdPrint(("IOCTL_TEST3\n"));
		//缓冲区方式IOCTL

		//缓冲区方式IOCTL
		//显示输入缓冲区数据
		UCHAR* UserInputBuffer = (UCHAR*)stack->Parameters.DeviceIoControl.Type3InputBuffer;
		KdPrint(("UserInputBuffer:0X%0X\n", UserInputBuffer));

		//得到用户模式地址
		PVOID UserOutputBuffer = pIrp->UserBuffer;

		KdPrint(("UserOutputBuffer:0X%0X\n", UserOutputBuffer));

		__try
		{
			KdPrint(("Enter __try block\n"));

			//判断指针是否可读
			ProbeForRead(UserInputBuffer, cbin, 4);
			//显示输入缓冲区内容
			for (ULONG i = 0; i < cbin; i++)
			{
				KdPrint(("%X\n", UserInputBuffer[i]));
			}

			//判断指针是否可写
			ProbeForWrite(UserOutputBuffer, cbout, 4);

			//操作输出缓冲区
			memset(UserOutputBuffer, 0xAA, cbout);

			//由于在上面引发异常，所以以后语句不会被执行!
			info = cbout;

			KdPrint(("Leave __try block\n"));
		}
		__except (EXCEPTION_EXECUTE_HANDLER)
		{
			KdPrint(("Catch the exception\n"));
			KdPrint(("The program will keep going\n"));
			status = STATUS_UNSUCCESSFUL;
		}

		info = cbout;
		break;
	}

	default:
		status = STATUS_INVALID_VARIANT;
	}

	// 完成IRP
	pIrp->IoStatus.Status = status;
	pIrp->IoStatus.Information = info;	// bytes xfered
	IoCompleteRequest(pIrp, IO_NO_INCREMENT);

	KdPrint(("Leave HelloDDKDeviceIOControl\n"));

	return status;
}

NTSTATUS HelloDDKRead(IN PDEVICE_OBJECT pDevObj,
	IN PIRP pIrp)
{
	KdPrint(("Enter HelloDDKRead\n"));

	PDEVICE_EXTENSION pDevExt = (PDEVICE_EXTENSION)pDevObj->DeviceExtension;
	NTSTATUS status = STATUS_SUCCESS;

	PIO_STACK_LOCATION stack = IoGetCurrentIrpStackLocation(pIrp);

	ULONG ulReadLength = stack->Parameters.Read.Length;
	KdPrint(("ulReadLength:%d\n", ulReadLength));

	ULONG mdl_length = MmGetMdlByteCount(pIrp->MdlAddress);
	PVOID mdl_address = MmGetMdlVirtualAddress(pIrp->MdlAddress);
	ULONG mdl_offset = MmGetMdlByteOffset(pIrp->MdlAddress);

	KdPrint(("mdl_address:0X%08X\n", mdl_address));
	KdPrint(("mdl_length:%d\n", mdl_length));
	KdPrint(("mdl_offset:%d\n", mdl_offset));

	if (mdl_length != ulReadLength)
	{
		//MDL的长度应该和读长度相等，否则该操作应该设为不成功
		pIrp->IoStatus.Information = 0;
		status = STATUS_UNSUCCESSFUL;
	}
	else
	{
		//用MmGetSystemAddressForMdlSafe得到MDL在内核模式下的映射
		PVOID kernel_address = MmGetSystemAddressForMdlSafe(pIrp->MdlAddress, NormalPagePriority);
		KdPrint(("kernel_address:0X%08X\n", kernel_address));
		memset(kernel_address, 0XAA, ulReadLength);
		pIrp->IoStatus.Information = ulReadLength;	// bytes xfered
	}

	pIrp->IoStatus.Status = status;

	IoCompleteRequest(pIrp, IO_NO_INCREMENT);
	KdPrint(("Leave HelloDDKRead\n"));

	return status;
}

/*
NTSTATUS HelloDDKRead(IN PDEVICE_OBJECT pDevObj,
	IN PIRP pIrp)
{
	KdPrint(("Enter HelloDDKRead\n"));

	PDEVICE_EXTENSION pDevExt = (PDEVICE_EXTENSION)pDevObj->DeviceExtension;
	NTSTATUS status = STATUS_SUCCESS;

	//得到当前堆栈
	PIO_STACK_LOCATION stack = IoGetCurrentIrpStackLocation(pIrp);
	//得到读的长度
	ULONG ulReadLength = stack->Parameters.Read.Length;
	//得到读的偏移量
	ULONG ulReadOffset = (ULONG)stack->Parameters.Read.ByteOffset.QuadPart;
	//得到用户模式地址
	PVOID user_address = pIrp->UserBuffer;

	KdPrint(("user_address:0X%0X\n", user_address));

	__try
	{
		KdPrint(("Enter __try block\n"));
		//判断空指针是否可写，显然会导致异常
		ProbeForWrite(user_address, ulReadLength, 4);

		memset(user_address, 0xAA, ulReadLength);

		//由于在上面引发异常，所以以后语句不会被执行!
		KdPrint(("Leave __try block\n"));
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		KdPrint(("Catch the exception\n"));
		KdPrint(("The program will keep going\n"));
		status = STATUS_UNSUCCESSFUL;
	}

	pIrp->IoStatus.Status = status;
	pIrp->IoStatus.Information = ulReadLength;	// bytes xfered
	IoCompleteRequest(pIrp, IO_NO_INCREMENT);
	KdPrint(("Leave HelloDDKRead\n"));

	return status;
}
*/

