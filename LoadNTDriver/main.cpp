#include <Windows.h>
#include <winsvc.h>
#include <conio.h>
#include <stdio.h>

#define DRIVER_NAME "HelloDDK"
#define DRIVER_PATH "HelloWDM\\HelloDDK.sys"

// װ��NT��������
BOOL LoadNTDriver(char* lpszDriverName, char* lpszDriverPath)
{
	char szDriverImagePath[256];
	// �õ�����������·��
	GetFullPathName(lpszDriverPath, 256, szDriverImagePath, NULL);

	BOOL bRet = FALSE;

	SC_HANDLE hServiceMgr = NULL; // SCM�������ľ��
	SC_HANDLE hServiceDDK = NULL; // NT��������ķ�����

	// �򿪷�����ƹ�����
	hServiceMgr = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);

	if (hServiceMgr == NULL)
	{
		// OpenSCManager
		printf("OpenSCManager() Faild %d \n", GetLastError());
		bRet = FALSE;
		goto BeforeLeave;
	}
	else
	{
		// OpenSCMager�ɹ�
		printf("OpenSCManager() ok ! \n");
	}

	// ������������Ӧ�ķ���
	hServiceDDK = CreateService(hServiceMgr,
		lpszDriverName, // �����������ע����е�����
		lpszDriverName, // ע�����������
		SERVICE_ALL_ACCESS, // ������������ķ���Ȩ��
		SERVICE_KERNEL_DRIVER, // ��ʾ���صķ�������������
		SERVICE_DEMAND_START, // ע������������ START ֵ
		SERVICE_ERROR_IGNORE, // ע������������ ErrorControl ֵ
		szDriverImagePath, // ע������������ ImagePath ֵ
		NULL,
		NULL,
		NULL,
		NULL,
		NULL);

	DWORD dwRtn;
	// �жϷ����Ƿ�ʧ��
	if (hServiceDDK == NULL)
	{
		dwRtn = GetLastError();
		if (dwRtn != ERROR_IO_PENDING && dwRtn != ERROR_SERVICE_EXISTS)
		{
			// ��������ԭ�򴴽�����ʧ��
			printf("CrateService() Faild %d ! \n", dwRtn);
			bRet = FALSE;
			goto BeforeLeave;
		}
		else
		{
			// ���񴴽�ʧ�ܣ������ڷ����Ѿ�������
			printf("CrateService() Faild Service is ERROR_IO_PENDING or ERROR_SERVICE_EXISTS! \n");
		}

		// ���������Ѿ����أ�ֻ��Ҫ��
		hServiceDDK = OpenService(hServiceMgr, lpszDriverName, SERVICE_ALL_ACCESS);
		if (hServiceDDK == NULL)
		{
			// ����򿪷���Ҳʧ�ܣ�����ζ����
			dwRtn = GetLastError();
			bRet = FALSE;
			goto BeforeLeave;
		}
		else
		{
			printf("openService() ok !\n");
		}
	}
	else
	{
		printf("CreateService() ok \n");
	}

	// �����������
	bRet = StartService(hServiceDDK, NULL, NULL);
	if (!bRet)
	{
		DWORD dwRtn = GetLastError();
		if (dwRtn != ERROR_IO_PENDING && dwRtn != ERROR_SERVICE_ALREADY_RUNNING)
		{
			printf("StartService() Faild %d ! \n", dwRtn);
			bRet = FALSE;
			goto BeforeLeave;
		}
		else
		{
			if (dwRtn == ERROR_IO_PENDING)
			{
				// �豸����ס
				printf("StartService() Faild Error_IO_PENDING ! \n");
				bRet = FALSE;
				goto BeforeLeave;
			}
			else
			{
				// �����Ѿ�����
				printf("StartService() Faild ERROR_SERVICE_ALREADY_RUNNING ! \n");
				bRet = FALSE;
				goto BeforeLeave;
			}
		}
	}
	bRet = TRUE;
	// �뿪ǰ�رվ��
BeforeLeave:
	if (hServiceDDK)
	{
		CloseServiceHandle(hServiceDDK);
	}
	if (hServiceMgr)
	{
		CloseServiceHandle(hServiceMgr);
	}
	return bRet;
}

// ж����������
BOOL UnloadNTDriver(char* szSvrName)
{
	BOOL bRet = FALSE;
	SC_HANDLE hServiceMgr = NULL; // SCM �������ľ��
	SC_HANDLE hServiceDDK = NULL; // NT��������ķ�����
	SERVICE_STATUS SvrSta;

	// ��SCM������
	hServiceMgr = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
	if (hServiceMgr == NULL)
	{
		// ����SCM������ʧ��
		printf("OpenSCManager() Faild %d ! \n", GetLastError());
		bRet = FALSE;
		goto BeforeLeave;
	}
	else
	{
		// ����SCM������ʧ�ܳɹ�
		printf("OpenSCManager() ok ! \n");
	}
	// ����������Ӧ�ķ���
	hServiceDDK = OpenService(hServiceMgr, szSvrName, SERVICE_ALL_ACCESS);

	if (hServiceDDK == NULL)
	{
		// ����������Ӧ�ķ���ʧ��
		printf("OpenService() Faild %d ! \n", GetLastError());
		bRet = FALSE;
		goto BeforeLeave;
	}
	else
	{
		printf("OpenService() ok ! \n");
	}
	// ֹͣ�����������ֹͣʧ�ܣ�ֻ�������������ܣ��ڶ�̬����
	if (!ControlService(hServiceDDK, SERVICE_CONTROL_STOP, &SvrSta))
	{
		printf("ControlService() Faild %d !\n", GetLastError());
	}
	else
	{
		// ����������Ӧ��ʧ��
		printf("ControlService() ok ! \n");
	}
	// ��̬ж����������
	if (!DeleteService(hServiceDDK))
	{
		// ж��ʧ��
		printf("DeleteService() Faild %d !\n", GetLastError());
	}
	else
	{
		// ж�سɹ�
		printf("DelServer: eleteService() ok ! \n");
	}
	bRet = TRUE;

BeforeLeave:
	// �뿪ǰ�رմ򿪵ľ��
	if (hServiceDDK)
	{
		CloseServiceHandle(hServiceDDK);
	}
	if (hServiceMgr)
	{
		CloseServiceHandle(hServiceMgr);
	}
	return bRet;
}

void TestDriver()
{
	// ������������
	HANDLE hDevice = CreateFile("\\\\.\\HelloDDK",
		GENERIC_WRITE | GENERIC_READ,
		0,
		NULL,
		OPEN_EXISTING,
		0,
		NULL);
	if (hDevice != INVALID_HANDLE_VALUE)
	{
		printf("Create Device ok ! \n");
	}
	else
	{
		printf("Create Device faild %d ! \n", GetLastError());
	}
	CloseHandle(hDevice);
}

char* GetCharPtr(const char* target)
{
	char* result = const_cast<char*>(target); 
	return result;
}

int main(int argc, char* argv[])
{
	char* driver_name = GetCharPtr(DRIVER_NAME);
	char* driver_path = GetCharPtr(DRIVER_NAME);
	// ��������
	BOOL bRet = LoadNTDriver(driver_name, driver_path);
	if (!bRet)
	{
		printf("LOADNTDriver error\n");
		return 0;
	}

	// ���سɹ�
	printf("press any to create device ! \n");
	char tmpKey = getch();

	TestDriver();

	// ��ʱ�������ͨ��ע����������鿴�������ӵ������֤
	printf("press any to unload th driver ! \n");
	tmpKey = getch();

	// ж������
	UnloadNTDriver(driver_name);
	if (!bRet)
	{
		printf("UnloadNTDriver error \n");
		return 0;
	}

	return 0;
}