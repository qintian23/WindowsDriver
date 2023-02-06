
#include "DFile.h"

#pragma INITCODE
VOID CreateFileTest()
{
    OBJECT_ATTRIBUTES objectAttributes;
    IO_STATUS_BLOCK iostatus;
    HANDLE hfile;
    UNICODE_STRING logFileUnicodeString;

    // ��ʼ�� UNICODE_STRING �ַ���
    RtlInitUnicodeString(&logFileUnicodeString, L"\\??\\C:\\1.log");
    // ����д�� "\\Device\\HarddiskVolume1\\1.LOG"

    // ��ʼ�� objectAttributes
    InitializeObjectAttributes(&objectAttributes,
        &logFileUnicodeString,
        OBJ_CASE_INSENSITIVE,
        NULL,
        NULL);
    // �����ļ�
    NTSTATUS ntStatus = ZwCreateFile(&hfile,
        GENERIC_WRITE,
        &objectAttributes,
        &iostatus,
        NULL,
        FILE_ATTRIBUTE_NORMAL,
        FILE_SHARE_READ,
        FILE_OPEN_IF,
        FILE_SYNCHRONOUS_IO_NONALERT,
        NULL,
        0);

    if (NT_SUCCESS(ntStatus))
    {
        KdPrint(("Create file succussfully!\n"));
    }
    else
    {
        KdPrint(("Create file  unsuccessfully!\n"));
    }

    //�ļ�����
    //.......

    //�ر��ļ����
    ZwClose(hfile);
}

#pragma INITCODE
VOID OpenFileTest2()
{
    OBJECT_ATTRIBUTES objectAttributes;
    IO_STATUS_BLOCK iostatus;
    HANDLE hfile;
    UNICODE_STRING logFileUnicodeString;

    // ��ʼ�� UNICODE_STRING �ַ���
    RtlInitUnicodeString(&logFileUnicodeString, L"\\??\\C:\\1.log");
    //����д�� "\\Device\\HarddiskVolume1\\1.LOG"

    // ��ʼ�� objectAttributes
    InitializeObjectAttributes(&objectAttributes,
        &logFileUnicodeString,
        OBJ_CASE_INSENSITIVE,
        NULL,
        NULL);

    // �����ļ�
    NTSTATUS ntStatus = ZwOpenFile(&hfile,
        GENERIC_ALL,
        &objectAttributes,
        &iostatus,
        FILE_SHARE_READ | FILE_SHARE_WRITE,
        FILE_SYNCHRONOUS_IO_NONALERT);

    if (NT_SUCCESS(ntStatus))
    {
        KdPrint(("Create file succussfully!\n"));
    }
    else
    {
        KdPrint(("Create file  unsuccessfully!\n"));
    }

    //�ļ�����
    //.......

    //�ر��ļ����
    ZwClose(hfile);
}

#pragma INITCODE
VOID OpenFileTest1()
{
    OBJECT_ATTRIBUTES objectAttributes;
    IO_STATUS_BLOCK iostatus;
    HANDLE hfile;
    UNICODE_STRING logFileUnicodeString;

    //��ʼ��UNICODE_STRING�ַ���
    RtlInitUnicodeString(&logFileUnicodeString,
        L"\\??\\C:\\1.log");
    //����д�� "\\Device\\HarddiskVolume1\\1.LOG"

    //��ʼ��objectAttributes
    InitializeObjectAttributes(&objectAttributes,
        &logFileUnicodeString,
        OBJ_CASE_INSENSITIVE,//�Դ�Сд���� 
        NULL,
        NULL);

    //�����ļ�
    NTSTATUS ntStatus = ZwCreateFile(&hfile,
        GENERIC_READ,
        &objectAttributes,
        &iostatus,
        NULL,
        FILE_ATTRIBUTE_NORMAL,
        FILE_SHARE_WRITE,
        FILE_OPEN,//���ļ��򿪣�����������򷵻ش��� 
        FILE_SYNCHRONOUS_IO_NONALERT,
        NULL,
        0);
    if (NT_SUCCESS(ntStatus))
    {
        KdPrint(("Open file succussfully!\n"));
    }
    else
    {
        KdPrint(("Open file  unsuccessfully!\n"));
    }

    //�ļ�����
    //.......

    //�ر��ļ����
    ZwClose(hfile);
}

#pragma INITCODE
VOID FileAttributeTest()
{
    OBJECT_ATTRIBUTES objectAttributes;
    IO_STATUS_BLOCK iostatus;
    HANDLE hfile;
    UNICODE_STRING logFileUnicodeString;

    // ��ʼ�� UNICODE_STRING �ַ���
    RtlInitUnicodeString(&logFileUnicodeString, L"\\??\\C:\\1.log");
    //����д�� "\\Device\\HarddiskVolume1\\1.LOG"

    // ��ʼ�� objectAttributes
    InitializeObjectAttributes(&objectAttributes,
        &logFileUnicodeString,
        OBJ_CASE_INSENSITIVE,
        NULL,
        NULL);

    // �����ļ�
    NTSTATUS ntStatus = ZwCreateFile(&hfile,
        GENERIC_READ,
        &objectAttributes,
        &iostatus,
        NULL,
        FILE_ATTRIBUTE_NORMAL,
        0,
        FILE_OPEN,
        FILE_SYNCHRONOUS_IO_NONALERT,
        NULL,
        0);
    if (NT_SUCCESS(ntStatus))
    {
        KdPrint(("open file successfully.\n"));
    }

    FILE_STANDARD_INFORMATION fsi;
    // ��ȡ�ļ�����
    ntStatus = ZwQueryInformationFile(hfile,
        &iostatus,
        &fsi,
        sizeof(FILE_STANDARD_INFORMATION),
        FileStandardInformation);
    if (NT_SUCCESS(ntStatus))
    {
        KdPrint(("file length:%u\n", fsi.EndOfFile.QuadPart));
    }

    // �޸ĵ�ǰ�ļ�ָ��
    FILE_POSITION_INFORMATION fpi;
    fpi.CurrentByteOffset.QuadPart = 100i64;
    ntStatus = ZwSetInformationFile(hfile,
        &iostatus,
        &fpi,
        sizeof(FILE_POSITION_INFORMATION),
        FilePositionInformation);
    if (NT_SUCCESS(ntStatus))
    {
        KdPrint(("update the file pointer successfully.\n"));
    }

    // �ر��ļ����
    ZwClose(hfile);
}

#pragma INITCODE
VOID WriteFileTest()
{
    OBJECT_ATTRIBUTES objectAttributes;
    IO_STATUS_BLOCK iostatus;
    HANDLE hfile;
    UNICODE_STRING logFileUnicodeString;

    // ��ʼ�� UNICODE_STRING �ַ���
    RtlInitUnicodeString(&logFileUnicodeString, L"\\??\\C:\\1.log");
    //����д�� "\\Device\\HarddiskVolume1\\1.LOG"

    // ��ʼ�� objectAttributes
    InitializeObjectAttributes(&objectAttributes,
        &logFileUnicodeString,
        OBJ_CASE_INSENSITIVE,
        NULL,
        NULL);

    // �����ļ�
    NTSTATUS ntStatus = ZwCreateFile(&hfile,
        GENERIC_WRITE,
        &objectAttributes,
        &iostatus,
        NULL,
        FILE_ATTRIBUTE_NORMAL,
        FILE_SHARE_WRITE,
        FILE_OPEN_IF, //��ʹ���ڸ��ļ���Ҳ���� 
        FILE_SYNCHRONOUS_IO_NONALERT,
        NULL,
        0);
#define BUFFER_SIZE 1024
    PUCHAR pBuffer = (PUCHAR)ExAllocatePool(PagedPool, BUFFER_SIZE);
    // ����Ҫ��������
    RtlFillMemory(pBuffer, BUFFER_SIZE, 0xAA);

    KdPrint(("The program will write %d bytes\n", BUFFER_SIZE));
    // д�ļ�
    ZwWriteFile(hfile, NULL, NULL, NULL, &iostatus, pBuffer, BUFFER_SIZE, NULL, NULL);
    KdPrint(("The program really wrote %d bytes\n", iostatus.Information));

    // ����Ҫ��������
    RtlFillMemory(pBuffer, BUFFER_SIZE, 0xBB);

    KdPrint(("The program will append %d bytes\n", BUFFER_SIZE));
    // ׷������
    LARGE_INTEGER number;
    number.QuadPart = 1024i64; // �����ļ�ָ��
    // ���ļ����и���д
    ZwWriteFile(hfile, NULL, NULL, NULL, &iostatus, pBuffer, BUFFER_SIZE, &number, NULL);
    KdPrint(("The program really appended %d bytes\n", iostatus.Information));

    // �ر��ļ����
    ZwClose(hfile);

    ExFreePool(pBuffer);
}

#pragma INITCODE
VOID ReadFileTest()
{
    OBJECT_ATTRIBUTES objectAttributes;
    IO_STATUS_BLOCK iostatus;
    HANDLE hfile;
    UNICODE_STRING logFileUnicodeString;

    //��ʼ��UNICODE_STRING�ַ���
    RtlInitUnicodeString(&logFileUnicodeString, L"\\??\\C:\\1.log");
    //����д�� "\\Device\\HarddiskVolume1\\1.LOG"

    // ��ʼ�� objectAttributes
    InitializeObjectAttributes(&objectAttributes,
        &logFileUnicodeString,
        OBJ_CASE_INSENSITIVE,
        NULL,
        NULL);
    // �����ļ�
    NTSTATUS ntStatus = ZwCreateFile(&hfile,
        GENERIC_READ,
        &objectAttributes,
        &iostatus,
        NULL,
        FILE_ATTRIBUTE_NORMAL,
        FILE_SHARE_READ,
        FILE_OPEN,
        FILE_SYNCHRONOUS_IO_NONALERT,
        NULL,
        0);

    if (!NT_SUCCESS(ntStatus))
    {
        KdPrint(("The file is not exist!\n"));
        return;
    }

    FILE_STANDARD_INFORMATION fsi;
    // ��ȡ�ļ�����
    ntStatus = ZwQueryInformationFile(hfile,
        &iostatus,
        &fsi,
        sizeof(FILE_STANDARD_INFORMATION),
        FileStandardInformation);

    KdPrint(("The program want to read %d bytes\n", fsi.EndOfFile.QuadPart));

    // Ϊ��ȡ���ļ����仺����
    PUCHAR pBuffer = (PUCHAR)ExAllocatePool(PagedPool,
        (LONG)fsi.EndOfFile.QuadPart);

    // ��ȡ�ļ�
    ZwReadFile(hfile,
        NULL,
        NULL,
        NULL,
        &iostatus,
        pBuffer,
        (LONG)fsi.EndOfFile.QuadPart,
        NULL,
        NULL);
    KdPrint(("The program really read %d bytes\n", iostatus.Information));
    // �ر��ļ����
    ZwClose(hfile);

    // �ͷŻ�����
    ExFreePool(pBuffer);
}

#pragma INITCODE
VOID FileTest()
{
    //�����ļ�ʵ��
//	CreateFileTest();

    //���ļ�ʵ��
//	OpenFileTest1();
    OpenFileTest2();

    //	FileAttributeTest();

        //д�ļ���׷���ļ�ʵ��
    //	WriteFileTest();

    //	ReadFileTest();

}