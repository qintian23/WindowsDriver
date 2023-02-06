#include "DString.h"

#pragma INITCODE
VOID StringInitTest()
{
    // (1)�� RtlInitAnsiString ��ʼ���ַ���
    ANSI_STRING AnsiString1;
    CHAR* string1 = "hello";
    // ��ʼ�� ANSI_STRING �ַ���
    RtlInitAnsiString(&AnsiString1, string1);
    KdPrint(("AnsiString1:%z\n", &AnsiString1)); // ��ӡhello

    string1[0] = 'H';
    string1[1] = 'E';
    string1[2] = 'L';
    string1[3] = 'L';
    string1[4] = 'O';
    // �ı�string1��AnsiString1ͬ���ᵼ�±仯
    KdPrint(("AnsiString1:%z\n", &AnsiString1)); // ��ӡHELLO

    // (2)����Ա�Լ���ʼ���ַ���
#define BUFFER_SIZE 1024
    UNICODE_STRING UnicodeString1 = { 0 };
    // ���û�������С
    UnicodeString1.MaximumLength = BUFFER_SIZE;
    // �����ڴ�
    UnicodeString1.Buffer = (PWSTR)ExAllocatePool(PagedPool, BUFFER_SIZE);
    WCHAR* wideString = L"hello";

    // �����ַ������ȣ���Ϊ�ǿ��ַ����������ַ����ȵ�2��
    UnicodeString1.Length = 2 * wcslen(wideString);

    // ��֤�������㹻�󣬷��������ֹ
    ASSERT(UnicodeString1.MaximumLength >= UnicodeString1.Length);
    // �ڴ濽��
    RtlCopyMemory(UnicodeString1.Buffer, wideString, UnicodeString1.Length);
    // �����ַ�����
    UnicodeString1.Length = 2 * wcslen(wideString);

    KdPrint(("UnicodeString:%wZ\n", &UnicodeString1));

    // �����ڴ�
    ExFreePool(UnicodeString1.Buffer);
    UnicodeString1.Buffer = NULL;
    UnicodeString1.Length = UnicodeString1.MaximumLength = 0;
}

#pragma INITCODE
VOID StringCopyTest()
{
    // ��ʼ�� UnicodeString1
    UNICODE_STRING UnicodeString1;
    RtlInitUnicodeString(&UnicodeString1, L"Hello Wrold");

    // ��ʼ��UnicodeString2
    UNICODE_STRING UnicodeString2 = { 0 };
    UnicodeString2.Buffer = (PWSTR)ExAllocatePool(PagedPool, BUFFER_SIZE);
    UnicodeString2.MaximumLength = BUFFER_SIZE;

    // ����ʼ�� UnicodeString2 ������ UnicodeString1
    RtlCopyUnicodeString(&UnicodeString2, &UnicodeString1);

    // �ֱ���ʾ UnicodeString1 �� UnicodeString2
    KdPrint(("UnicodeString1:%wz\n", &UnicodeString1));
    KdPrint(("UnicodeString2:%wz\n", &UnicodeString2));

    // ���� UnicodeString2
    // ע�� !!UnicodeString1 ��������
    RtlFreeUnicodeString(&UnicodeString2);
}

#pragma INITCODE
VOID StringCompareTest()
{
    // ��ʼ�� UnicodeString1
    UNICODE_STRING UnicodeString1;
    RtlInitUnicodeString(&UnicodeString1, L"Hello world");

    // ��ʼ�� UnicodeString2
    UNICODE_STRING UnicodeString2;
    RtlInitUnicodeString(&UnicodeString1, L"Hello");

    if (RtlEqualUnicodeString(&UnicodeString1, &UnicodeString2, TRUE))
    {
        KdPrint(("UnicodeString1 and UnicodeString2 are equal\n"));
    }
    else
    {
        KdPrint(("UnicodeString1 and UnicodeString2 are NOT equal\n"));
    }
}

#pragma INITCODE
VOID StringUpcaseTest()
{
    // ��ʼ�� UnicodeString1
    UNICODE_STRING UnicodeString1;
    RtlInitUnicodeString(&UnicodeString1, L"Hello World");

    // �仯ǰ
    KdPrint(("UnicodeString1:%wz\n", &UnicodeString1));

    // ���д
    RtlUpcaseUnicodeString(&UnicodeString1, &UnicodeString1, FALSE);

    // �仯��
    KdPrint(("UnicodeString1:%wZ\n", &UnicodeString1));
}

#pragma INITCODE
VOID StringToIntegerTest()
{
    // (1)�ַ���ת��������
    // ��ʼ�� UnicodeString1
    UNICODE_STRING Unicodestring1;
    RtlInitUnicodeString(&Unicodestring1, L"-100");

    ULONG lNumber;
    NTSTATUS nStatus = RtlUnicodeStringToInteger(&Unicodestring1, 10, &lNumber);
    if (NT_SUCCESS(nStatus))
    {
        KdPrint(("Conver to integer succussfully!\n"));
        KdPrint(("Result:%d\n", lNumber));
    }
    else
    {
        KdPrint(("Conver to integer unsuccessfully!\n"));
    }

    // (2)����ת�����ַ���
    // ��ʼ�� UnicodeString2
    UNICODE_STRING Unicodestring2 = { 0 };
    Unicodestring2.Buffer = (PWSTR)ExAllocatePool(PagedPool, BUFFER_SIZE);
    Unicodestring2.MaximumLength = BUFFER_SIZE;
    nStatus = RtlIntegerToUnicodeString(200, 10, &Unicodestring2);

    if (NT_SUCCESS(nStatus))
    {
        KdPrint(("Conver to string succussfully!\n"));
        KdPrint(("Result:%wZ\n", &Unicodestring2));
    }
    else
    {
        KdPrint(("Conver to string unsuccessfully!\n"));
    }

    //����UnicodeString2
    //ע��!!UnicodeString1��������
    RtlFreeUnicodeString(&Unicodestring2);
}

#pragma INITCODE
VOID StringConverTest()
{
    //(1)��UNICODE_STRING�ַ���ת����ANSI_STRING�ַ���
    //��ʼ��UnicodeString1
    UNICODE_STRING UnicodeString1;
    RtlInitUnicodeString(&UnicodeString1, L"Hello World");

    ANSI_STRING AnsiString1;
    NTSTATUS nStatus = RtlUnicodeStringToAnsiString(&AnsiString1, &UnicodeString1, TRUE);

    if (NT_SUCCESS(nStatus))
    {
        KdPrint(("Conver succussfully!\n"));
        KdPrint(("Result:%Z\n", &AnsiString1));
    }
    else
    {
        KdPrint(("Conver unsuccessfully!\n"));
    }

    // ���� AnsiString1
    RtlFreeAnsiString(&AnsiString1);

    // (2)��ANSI_STRING�ַ���ת����UNICODE_STRING�ַ���
    // ��ʼ��AnsiString2
    ANSI_STRING AnsiString2;
    RtlInitAnsiString(&AnsiString2, "Hello World");

    UNICODE_STRING UnicodeString2;
    nStatus = RtlAnsiStringToUnicodeString(&UnicodeString2, &AnsiString2, TRUE);

    if (NT_SUCCESS(nStatus))
    {
        KdPrint(("Conver succussfully!\n"));
        KdPrint(("Result:%wZ\n", &UnicodeString2));
    }
    else
    {
        KdPrint(("Conver unsuccessfully!\n"));
    }

    // ���� UnicodeString2
    RtlFreeUnicodeString(&UnicodeString2);
}

VOID StringTest()
{
    //�ַ�����ʼ��ʵ��
//	StringInitTest();

    //�ַ�������ʵ��
//	StringCopyTest();

    //�ַ����Ƚ�ʵ��
//	StringCompareTest();

    //�ַ������дʵ��
//	StringToUpperTest();

    //�ַ����������໥ת��ʵ��
//	StringToIntegerTest();

    //ANSI_STRING�ַ�����UNICODE_STRING�ַ����໥ת��ʵ��
    StringConverTest();

}