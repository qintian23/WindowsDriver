#include <windows.h>
#include <stdio.h>
//ʹ��CTL_CODE�������winioctl.h
#include <winioctl.h>
#include "..\WDMBasicFunction\Ioctls.h"

#define  DEVICE_NAME	"test.dat"
#define BUFFER_SIZE	512

//������ļ����ڻ����BUFFER_SIZE
VOID CALLBACK MyFileIOCompletionRoutine(
	DWORD dwErrorCode,                // ���ڴ˴β������ص�״̬
	DWORD dwNumberOfBytesTransfered,  // �����Ѿ������˶����ֽ�,Ҳ������IRP���Infomation
	LPOVERLAPPED lpOverlapped         // ������ݽṹ
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
	//�����뻺����ȫ���ó�0XBB
	memset(InputBuffer, 0xBB, 10);
	DWORD dwOutput;
	//���뻺������Ϊ���룬�����������Ϊ���

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
			FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED,//�˴�����FILE_FLAG_OVERLAPPED
			NULL);

	if (hDevice == INVALID_HANDLE_VALUE)
	{
		printf("Read Error\n");
		return 1;
	}

	UCHAR buffer[BUFFER_SIZE];

	//��ʼ��overlapʹ���ڲ�ȫ��Ϊ��
	//���ó�ʼ���¼�!!
	OVERLAPPED overlap = { 0 };

	//����û������OVERLAP������������첽����
	ReadFileEx(hDevice, buffer, BUFFER_SIZE, &overlap, MyFileIOCompletionRoutine);

	//��һЩ������������Щ����������豸����ִ��

	//����alterable
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
			FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED,//�˴�����FILE_FLAG_OVERLAPPED
			NULL);

	if (hDevice == INVALID_HANDLE_VALUE)
	{
		printf("Read Error\n");
		return 1;
	}

	UCHAR buffer[BUFFER_SIZE];

	//��ʼ��overlapʹ���ڲ�ȫ��Ϊ��
	//���ó�ʼ���¼�!!
	OVERLAPPED overlap = { 0 };

	//����û������OVERLAP������������첽����
	ReadFileEx(hDevice, buffer, BUFFER_SIZE, &overlap, MyFileIOCompletionRoutine);

	//��һЩ������������Щ����������豸����ִ��

	//����alterable
	SleepEx(0, TRUE);

	CloseHandle(hDevice);

	return 0;
}