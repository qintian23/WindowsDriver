#include "DRegedit.h"

#define MY_REG_SOFTWARE_KEY_NAME L"\\Registry\\Machine\\software\\Zhulc"

#pragma INITCODE
VOID CreateRegTest()
{
    // �������ĳע�����Ŀ
    UNICODE_STRING RegUnicodeString;
    HANDLE hRegister;

    // ��ʼ�� UNICODE_STRING �ַ���
    RtlInitUnicodeString(&RegUnicodeString, MY_REG_SOFTWARE_KEY_NAME);

    OBJECT_ATTRIBUTES objectAttributes;
    // ��ʼ�� objectAttributes
    InitializeObjectAttributes(&objectAttributes,
        &RegUnicodeString,
        OBJ_CASE_INSENSITIVE,
        NULL,
        NULL);
    ULONG ulResult;
    // ���������ע�����Ŀ
    NTSTATUS ntStatus = ZwCreateKey(&hRegister,
        KEY_ALL_ACCESS,
        &objectAttributes,
        0,
        NULL,
        REG_OPTION_NON_VOLATILE,
        &ulResult);
    if (NT_SUCCESS(ntStatus))
    {
        // �ж��Ǳ��´����������Ѿ�������
        if (ulResult == REG_CREATED_NEW_KEY)
        {
            KdPrint(("The register item is created\n"));
        }
        else if (ulResult == REG_OPENED_EXISTING_KEY)
        {
            KdPrint(("The register item has been created,and now is opened\n"));
        }
    }

    // (2)�������ĳע�����Ŀ������
    UNICODE_STRING subRegUnicodeString;
    HANDLE hSubRegister;

    // ��ʼ�� UNICODE_STRING �ַ���
    RtlInitUnicodeString(&subRegUnicodeString, L"SubItem");

    OBJECT_ATTRIBUTES subObjectAttributes;
    // ��ʼ��subObjectAttributes
    InitializeObjectAttributes(&subObjectAttributes,
        &subRegUnicodeString,
        OBJ_CASE_INSENSITIVE,
        hRegister,
        NULL);
    // �������ע�����Ŀ
    ntStatus = ZwCreateKey(&hSubRegister,
        KEY_ALL_ACCESS,
        &subObjectAttributes,
        0,
        NULL,
        REG_OPTION_NON_VOLATILE,
        &ulResult);
    if (NT_SUCCESS(ntStatus))
    {
        // �ж��Ǳ��´����������Ѿ�������
        if (ulResult == REG_CREATED_NEW_KEY)
        {
            KdPrint(("The sub register item is created\n"));
        }
        else if (ulResult == REG_OPENED_EXISTING_KEY)
        {
            KdPrint(("The sub register item has been created,and now is opened\n"));
        }
    }
    //�ر�ע�����
    ZwClose(hRegister);
    ZwClose(hSubRegister);
}

#pragma INITCODE
VOID OpenRegTest()
{
    UNICODE_STRING RegUnicodeString;
    HANDLE hRegister;

    // ��ʼ�� UNICODE_STRING �ַ���
    RtlInitUnicodeString(&RegUnicodeString, MY_REG_SOFTWARE_KEY_NAME);

    OBJECT_ATTRIBUTES objectAttributes;
    // ��ʼ�� objectAttributes
    InitializeObjectAttributes(&objectAttributes,
        &RegUnicodeString,
        OBJ_CASE_INSENSITIVE,
        NULL,
        NULL);
    // ��ע���
    NTSTATUS ntStatus = ZwOpenKey(&hRegister,
        KEY_ALL_ACCESS,
        &objectAttributes);
    if (NT_SUCCESS(ntStatus))
    {
        KdPrint(("Open register successfully\n"));
    }
    ZwClose(hRegister);
}

#pragma INITCODE
VOID DeleteItemRegTest()
{
    UNICODE_STRING RegUnicodeString;
    HANDLE hRegister;

#define MY_REG_SOFTWARE_KEY_NAME1 L"\\Registry\\Machine\\Software\\Zhulc\\SubItem"
    // ��ʼ��UNICODE_STRING�ַ���
    RtlInitUnicodeString(&RegUnicodeString, MY_REG_SOFTWARE_KEY_NAME1);

    OBJECT_ATTRIBUTES objectAttributes;
    // ��ʼ�� objectAttributes
    InitializeObjectAttributes(&objectAttributes,
        &RegUnicodeString,
        OBJ_CASE_INSENSITIVE,
        NULL,
        NULL);
    // ��ע���
    NTSTATUS ntStatus = ZwOpenKey(&hRegister,
        KEY_ALL_ACCESS,
        &objectAttributes);

    if (NT_SUCCESS(ntStatus))
    {
        KdPrint(("Open register successfully\n"));
    }

    ntStatus = ZwDeleteKey(hRegister);
    if (NT_SUCCESS(ntStatus))
    {
        KdPrint(("Delete the item successfully\n"));
    }
    else if (ntStatus == STATUS_ACCESS_DENIED)
    {
        KdPrint(("STATUS_ACCESS_DENIED\n"));
    }
    else if (ntStatus == STATUS_INVALID_HANDLE)
    {
        KdPrint(("STATUS_INVALID_HANDLE\n"));
    }
    else
    {
        KdPrint(("Maybe the item has sub item to delete\n"));
    }

    ZwClose(hRegister);
}

#pragma INITCODE
VOID SetRegTest()
{
    UNICODE_STRING RegUnicodeString;
    HANDLE hRegister;

    // ��ʼ�� UNICODE_STRING �ַ���
    RtlInitUnicodeString(&RegUnicodeString, MY_REG_SOFTWARE_KEY_NAME);

    OBJECT_ATTRIBUTES objectAttributes;
    // ��ʼ�� objectAttributes
    InitializeObjectAttributes(&objectAttributes,
        &RegUnicodeString,
        OBJ_CASE_INSENSITIVE,
        NULL,
        NULL);
    // ��ע���
    NTSTATUS ntStatus = ZwOpenKey(&hRegister,
        KEY_ALL_ACCESS,
        &objectAttributes);
    if (NT_SUCCESS(ntStatus))
    {
        KdPrint(("Open register successfully\n"));
    }

    UNICODE_STRING ValueName;
    // ��ʼ�� ValueName
    RtlInitUnicodeString(&ValueName, L"REG_DWORD value");

    // ���� REG_DWORD �Ӽ�
    ULONG ulValue = 1000;
    ZwSetValueKey(hRegister,
        &ValueName,
        0,
        REG_DWORD,
        &ulValue,
        sizeof(ulValue));
    // ��ʼ��ValueName
    RtlInitUnicodeString(&ValueName, L"REG_SZ value");
    WCHAR* strValue = L"hello world";

    // ���� REG_SZ �Ӽ�
    ZwSetValueKey(hRegister,
        &ValueName,
        0,
        REG_SZ,
        strValue,
        wcslen(strValue) * 2 + 2);

    // ��ʼ�� ValueName
    RtlInitUnicodeString(&ValueName, L"REG_BINARY value");

    UCHAR buffer[10];
    RtlFillMemory(buffer, sizeof(buffer), 0xFF);

    // ���� REG_MULTI_SZ �Ӽ�
    ZwSetValueKey(hRegister,
        &ValueName,
        0,
        REG_BINARY,
        buffer,
        sizeof(buffer));

    // �ر�ע�����
    ZwClose(hRegister);
}

#pragma INITCODE
VOID QueryRegTest()
{
    UNICODE_STRING  RegUnicodeString;
    HANDLE hRegister;

    // ��ʼ�� UNICODE_STRING �ַ���
    RtlInitUnicodeString(&RegUnicodeString, MY_REG_SOFTWARE_KEY_NAME);

    OBJECT_ATTRIBUTES objectAttributes;
    // ��ʼ�� objectAttributes
    InitializeObjectAttributes(&objectAttributes,
        &RegUnicodeString,
        OBJ_CASE_INSENSITIVE,
        NULL,
        NULL);
    // ��ע���
    NTSTATUS ntStatus = ZwOpenKey(&hRegister,
        KEY_ALL_ACCESS,
        &objectAttributes);

    if (NT_SUCCESS(ntStatus))
    {
        KdPrint(("Open register successfully\n"));
    }

    UNICODE_STRING ValueName;
    // ��ʼ��ValueName
    RtlInitUnicodeString(&ValueName, L"REG_DWORD value");

    // ��ȡ REG_DWORD �Ӽ�
    ULONG ulSize;
    ntStatus = ZwQueryValueKey(hRegister,
        &ValueName,
        KeyValuePartialInformation,
        NULL,
        0,
        &ulSize);
    if (ntStatus == STATUS_OBJECT_NAME_NOT_FOUND || ulSize == 0)
    {
        ZwClose(hRegister);
        KdPrint(("The item is not exist\n"));
        return;
    }
    PKEY_VALUE_PARTIAL_INFORMATION pvpi = (PKEY_VALUE_PARTIAL_INFORMATION)ExAllocatePool(PagedPool, ulSize);

    ntStatus = ZwQueryValueKey(hRegister,
        &ValueName,
        KeyValuePartialInformation,
        pvpi,
        ulSize,
        &ulSize);
    if (!NT_SUCCESS(ntStatus))
    {
        ZwClose(hRegister);
        KdPrint(("Read regsiter error\n"));
        return;
    }
    // �ж��Ƿ�ΪREG_DWORD����
    if (pvpi->Type == REG_DWORD && pvpi->DataLength == sizeof(ULONG))
    {
        PULONG pulValue = (PULONG)pvpi->Data;
        KdPrint(("The value:%d\n", *pulValue));
    }

    ExFreePool(pvpi);

    // ��ʼ�� ValueName
    RtlInitUnicodeString(&ValueName, L"REG_SZ value");
    // ��ȡ REG_SZ �Ӽ�
    ntStatus = ZwQueryValueKey(hRegister,
        &ValueName,
        KeyValuePartialInformation,
        NULL,
        0,
        &ulSize);
    if (ntStatus == STATUS_OBJECT_NAME_NOT_FOUND || ulSize == 0)
    {
        ZwClose(hRegister);
        KdPrint(("The item is not exist\n"));
        return;
    }

    pvpi = (PKEY_VALUE_PARTIAL_INFORMATION)ExAllocatePool(PagedPool, ulSize);

    ntStatus = ZwQueryValueKey(hRegister,
        &ValueName,
        KeyValuePartialInformation,
        pvpi,
        ulSize,
        &ulSize);
    if (!NT_SUCCESS(ntStatus))
    {
        ZwClose(hRegister);
        KdPrint(("Read regsiter error\n"));
        return;
    }

    // �ж��Ƿ�ΪREG_SZ����   
    if (pvpi->Type == REG_SZ)
    {
        KdPrint(("The value:%S\n", pvpi->Data));
    }
    ZwClose(hRegister);
}

#pragma INITCODE
VOID EnumerateSubItemRegTest()
{
    UNICODE_STRING RegUnicodeString;
    HANDLE hRegister;

    // ��ʼ��UNICODE_STRING�ַ���
    RtlInitUnicodeString(&RegUnicodeString, MY_REG_SOFTWARE_KEY_NAME);

    OBJECT_ATTRIBUTES objectAttributes;
    // ��ʼ�� objectAttributes
    InitializeObjectAttributes(&objectAttributes,
        &RegUnicodeString,
        OBJ_CASE_INSENSITIVE,
        NULL,
        NULL);
    // ��ע���
    NTSTATUS ntStatus = ZwOpenKey(&hRegister,
        KEY_ALL_ACCESS,
        &objectAttributes);
    if (NT_SUCCESS(ntStatus))
    {
        KdPrint(("Open register successfully\n"));
    }

    ULONG ulSize;
    // ��һ�ε���ZwQueryKeyΪ�˻�ȡKEY_FULL_INFORMATION���ݵĳ���
    ZwQueryKey(hRegister,
        KeyFullInformation,
        NULL,
        0,
        &ulSize);
    PKEY_FULL_INFORMATION pfi = (PKEY_FULL_INFORMATION)ExAllocatePool(PagedPool, ulSize);

    // �ڶ��ε���ZwQueryKeyΪ�˻�ȡKEY_FULL_INFORMATION���ݵ�����
    ZwQueryKey(hRegister,
        KeyFullInformation,
        pfi,
        ulSize,
        &ulSize);

    for (ULONG i = 0; i < pfi->SubKeys; i++)
    {
        // ��һ�ε���ZwEnumerateKeyΪ�˻�ȡKEY_BASIC_INFORMATION���ݵĳ���
        ZwEnumerateKey(hRegister,
            i,
            KeyBasicInformation,
            NULL,
            0,
            &ulSize);
        PKEY_BASIC_INFORMATION pbi = (PKEY_BASIC_INFORMATION)ExAllocatePool(PagedPool, ulSize);

        // �ڶ��ε���ZwEnumerateKeyΪ�˻�ȡKEY_BASIC_INFORMATION���ݵ�����
        ZwEnumerateKey(hRegister,
            i,
            KeyBasicInformation,
            pbi,
            ulSize,
            &ulSize);

        UNICODE_STRING uniKeyName;
        uniKeyName.Length =
            uniKeyName.MaximumLength = (USHORT)pbi->NameLength;

        uniKeyName.Buffer = pbi->Name;

        KdPrint(("The %d sub item name:%wZ\n", i, &uniKeyName));

        ExFreePool(pbi);
    }
    ExFreePool(pfi);
    ZwClose(hRegister);
}

#pragma INITCODE
VOID EnumerateSubValueRegTest()
{
    UNICODE_STRING RegUnicodeString;
    HANDLE hRegister;

    // ��ʼ��UNICODE_STRING�ַ���
    RtlInitUnicodeString(&RegUnicodeString, MY_REG_SOFTWARE_KEY_NAME);

    OBJECT_ATTRIBUTES objectAttributes;
    // ��ʼ�� objectAttributes
    InitializeObjectAttributes(&objectAttributes,
        &RegUnicodeString,
        OBJ_CASE_INSENSITIVE,
        NULL,
        NULL);
    // ��ע���
    NTSTATUS ntStatus = ZwOpenKey(&hRegister,
        KEY_ALL_ACCESS,
        &objectAttributes);
    if (NT_SUCCESS(ntStatus))
    {
        KdPrint(("Open register successfully\n"));
    }
    ULONG ulSize;
    ZwQueryKey(hRegister,
        KeyFullInformation,
        NULL,
        0,
        &ulSize);
    PKEY_FULL_INFORMATION pfi = (PKEY_FULL_INFORMATION)ExAllocatePool(PagedPool, ulSize);

    ZwQueryKey(hRegister,
        KeyFullInformation,
        pfi,
        ulSize,
        &ulSize);
    for (ULONG i = 0; i < pfi->Values; i++)
    {
        ZwEnumerateValueKey(hRegister,
            i,
            KeyValueBasicInformation,
            NULL,
            0,
            &ulSize);
        PKEY_VALUE_BASIC_INFORMATION pvbi = (PKEY_VALUE_BASIC_INFORMATION)ExAllocatePool(PagedPool, ulSize);

        ZwEnumerateValueKey(hRegister,
            i,
            KeyValueBasicInformation,
            pvbi,
            ulSize,
            &ulSize);

        UNICODE_STRING uniKeyName;
        uniKeyName.Length =
            uniKeyName.MaximumLength = (USHORT)pvbi->NameLength;

        uniKeyName.Buffer = pvbi->Name;

        KdPrint(("The %d sub value name:%wZ\n", i, &uniKeyName));

        if (pvbi->Type == REG_SZ)
        {
            KdPrint(("The sub value type:REG_SZ\n"));
        }
        else if (pvbi->Type == REG_MULTI_SZ)
        {
            KdPrint(("The sub value type:REG_MULTI_SZ\n"));
        }
        else if (pvbi->Type == REG_DWORD)
        {
            KdPrint(("The sub value type:REG_DWORD\n"));
        }
        else if (pvbi->Type == REG_BINARY)
        {
            KdPrint(("The sub value type:REG_BINARY\n"));
        }
        ExFreePool(pvbi);
    }
    ExFreePool(pfi);
    ZwClose(hRegister);
}

#pragma INITCODE
VOID RtlRegTest()
{
    // ��������Ŀ
    NTSTATUS ntStatus = RtlCreateRegistryKey(RTL_REGISTRY_SERVICES, L"HelloDDK\\Zhulc");
    if (NT_SUCCESS(ntStatus))
    {
        KdPrint(("Create the item successfully\n"));
    }

    // ���ĳ���Ƿ����
    ntStatus = RtlCheckRegistryKey(RTL_REGISTRY_SERVICES, L"HelloDDK\\Zhulc");
    if (NT_SUCCESS(ntStatus))
    {
        KdPrint(("The item is exist\n"));
    }

    // д�� REG_DWORD ������
    ULONG value1 = 100;
    ntStatus = RtlWriteRegistryValue(RTL_REGISTRY_SERVICES,
        L"HelloDDK\\Zhulc",
        L"DWORD_Value",
        REG_DWORD,
        &value1,
        sizeof(value1));
    if (NT_SUCCESS(ntStatus))
    {
        KdPrint(("Write the DWORD value succuessfully\n"));
    }

    PWCHAR szString = L"Hello DDK";
    ntStatus = RtlWriteRegistryValue(RTL_REGISTRY_SERVICES,
        L"HelloDDK\\Zhulc",
        L"SZ_Value",
        REG_SZ,
        szString,
        wcslen(szString) * 2 + 2);
    if (NT_SUCCESS(ntStatus))
    {
        KdPrint(("Write the REG_SZ value succuessfully\n"));
    }

    RTL_QUERY_REGISTRY_TABLE paramTable[2];
    RtlZeroMemory(paramTable, sizeof(paramTable));

    ULONG defaultData = 0;
    ULONG uQueryValue;
    paramTable[0].Flags = RTL_QUERY_REGISTRY_DIRECT;
    paramTable[0].Name = L"DWORD_Value";
    paramTable[0].EntryContext = &uQueryValue;
    paramTable[0].DefaultType = REG_DWORD;
    paramTable[0].DefaultData = &defaultData;
    paramTable[0].DefaultLength = sizeof(ULONG);

    //��ѯ REG_DWORD ������
    ntStatus = RtlQueryRegistryValues(RTL_REGISTRY_SERVICES,
        L"HelloDDK\\Zhulc",
        paramTable,
        NULL,
        NULL);
    if (NT_SUCCESS(ntStatus))
    {
        KdPrint(("Query the item successfully\n"));
        KdPrint(("The item is :%d\n", uQueryValue));
    }

    // ɾ���Ӽ�
    ntStatus = RtlDeleteRegistryValue(RTL_REGISTRY_SERVICES,
        L"HelloDDK\\Zhulc",
        L"DWORD_Value");
    if (NT_SUCCESS(ntStatus))
    {
        KdPrint(("delete the value successfully\n"));
    }
}

#pragma INITCODE
VOID RegTest()
{
    CreateRegTest();
    // 	OpenRegTest();
    // 	SetRegTest();
    //	QueryRegTest();
    //	EnumerateSubItemRegTest();
    //	EnumerateSubValueRegTest();
    DeleteItemRegTest();
    //	RtlRegTest();
}