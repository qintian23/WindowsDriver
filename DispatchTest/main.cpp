#include <windows.h>
#include <stdio.h>
//使用CTL_CODE必须加入winioctl.h
#include <winioctl.h>
#include "..\WDMBasicFunction\Ioctls.h"

#define  DEVICE_NAME	"test.dat"
#define BUFFER_SIZE	512

//假设该文件大于或等于BUFFER_SIZE
VOID CALLBACK MyFileIOCompletionRoutine(
	DWORD dwErrorCode,                // 对于此次操作返回的状态
	DWORD dwNumberOfBytesTransfered,  // 告诉已经操作了多少字节,也就是在IRP里的Infomation
	LPOVERLAPPED lpOverlapped         // 这个数据结构
)
{
	printf("IO operation end!\n");
}

int dispatchTest()
{
	HANDLE hDevice =
		CreateFile(L"\\\\.\\HelloDDK",
			GENERIC_READ | GENERIC_WRITE,
			0,		// share mode none
			NULL,	// no security
			OPEN_EXISTING,
			FILE_ATTRIBUTE_NORMAL,
			NULL);		// no template

	if (hDevice == INVALID_HANDLE_VALUE)
	{
		printf("Failed to obtain file handle to device: "
			"%s with Win32 error code: %d\n",
			"MyWDMDevice", GetLastError());
		return 1;
	}

	UCHAR InputBuffer[10];
	UCHAR OutputBuffer[10];
	//将输入缓冲区全部置成0XBB
	memset(InputBuffer, 0xBB, 10);
	DWORD dwOutput;
	//输入缓冲区作为输入，输出缓冲区作为输出

	BOOL bRet;
	bRet = DeviceIoControl(hDevice, IOCTL_TEST1, InputBuffer, 10, &OutputBuffer, 10, &dwOutput, NULL);
	if (bRet)
	{
		printf("Output buffer:%d bytes\n", dwOutput);
		for (int i = 0; i < (int)dwOutput; i++)
		{
			printf("%02X ", OutputBuffer[i]);
		}
		printf("\n");
	}

	bRet = DeviceIoControl(hDevice, IOCTL_TEST2, InputBuffer, 10, &OutputBuffer, 10, &dwOutput, NULL);
	if (bRet)
	{
		printf("Output buffer:%d bytes\n", dwOutput);
		for (int i = 0; i < (int)dwOutput; i++)
		{
			printf("%02X ", OutputBuffer[i]);
		}
		printf("\n");
	}

	bRet = DeviceIoControl(hDevice, IOCTL_TEST3, InputBuffer, 10, &OutputBuffer, 10, &dwOutput, NULL);
	if (bRet)
	{
		printf("Output buffer:%d bytes\n", dwOutput);
		for (int i = 0; i < (int)dwOutput; i++)
		{
			printf("%02X ", OutputBuffer[i]);
		}
		printf("\n");
	}

	UCHAR buffer[10];
	ULONG ulRead;
	memset(buffer, 0xBB, 10);
	bRet = ReadFile(hDevice, buffer, 10, &ulRead, NULL);
	if (bRet)
	{
		printf("Address 0X%08X\n", buffer);
		printf("(Read %d bytes):", ulRead);
		for (int i = 0; i < (int)ulRead; i++)
		{
			printf("%02X ", buffer[i]);
		}

		printf("\n");
	}


	CloseHandle(hDevice);
	return 0;
}

int asyncOperate1()
{
	HANDLE hDevice =
		CreateFile(L"test.dat",
			GENERIC_READ | GENERIC_WRITE,
			0,
			NULL,
			OPEN_EXISTING,
			FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED,//此处设置FILE_FLAG_OVERLAPPED
			NULL);

	if (hDevice == INVALID_HANDLE_VALUE)
	{
		printf("Read Error\n");
		return 1;
	}

	UCHAR buffer[BUFFER_SIZE];

	//初始化overlap使其内部全部为零
	//不用初始化事件!!
	OVERLAPPED overlap = { 0 };

	//这里没有设置OVERLAP参数，因此是异步操作
	ReadFileEx(hDevice, buffer, BUFFER_SIZE, &overlap, MyFileIOCompletionRoutine);

	//做一些其他操作，这些操作会与读设备并行执行

	//进入alterable
	SleepEx(0, TRUE);

	CloseHandle(hDevice);

	return 0;
}

int main()
{
	HANDLE hDevice =
		CreateFile(L"test.dat",
			GENERIC_READ | GENERIC_WRITE,
			0,
			NULL,
			OPEN_EXISTING,
			FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED,//此处设置FILE_FLAG_OVERLAPPED
			NULL);

	if (hDevice == INVALID_HANDLE_VALUE)
	{
		printf("Read Error\n");
		return 1;
	}

	UCHAR buffer[BUFFER_SIZE];

	//初始化overlap使其内部全部为零
	//不用初始化事件!!
	OVERLAPPED overlap = { 0 };

	//这里没有设置OVERLAP参数，因此是异步操作
	ReadFileEx(hDevice, buffer, BUFFER_SIZE, &overlap, MyFileIOCompletionRoutine);

	//做一些其他操作，这些操作会与读设备并行执行

	//进入alterable
	SleepEx(0, TRUE);

	CloseHandle(hDevice);

	return 0;
}