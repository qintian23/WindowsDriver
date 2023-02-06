#include "DString.h"

#pragma INITCODE
VOID StringInitTest()
{
    // (1)用 RtlInitAnsiString 初始化字符串
    ANSI_STRING AnsiString1;
    CHAR* string1 = "hello";
    // 初始化 ANSI_STRING 字符串
    RtlInitAnsiString(&AnsiString1, string1);
    KdPrint(("AnsiString1:%z\n", &AnsiString1)); // 打印hello

    string1[0] = 'H';
    string1[1] = 'E';
    string1[2] = 'L';
    string1[3] = 'L';
    string1[4] = 'O';
    // 改变string1，AnsiString1同样会导致变化
    KdPrint(("AnsiString1:%z\n", &AnsiString1)); // 打印HELLO

    // (2)程序员自己初始化字符串
#define BUFFER_SIZE 1024
    UNICODE_STRING UnicodeString1 = { 0 };
    // 设置缓存区大小
    UnicodeString1.MaximumLength = BUFFER_SIZE;
    // 分配内存
    UnicodeString1.Buffer = (PWSTR)ExAllocatePool(PagedPool, BUFFER_SIZE);
    WCHAR* wideString = L"hello";

    // 设置字符串长度，因为是宽字符，所以是字符长度的2倍
    UnicodeString1.Length = 2 * wcslen(wideString);

    // 保证缓存区足够大，否则程序终止
    ASSERT(UnicodeString1.MaximumLength >= UnicodeString1.Length);
    // 内存拷贝
    RtlCopyMemory(UnicodeString1.Buffer, wideString, UnicodeString1.Length);
    // 设置字符长度
    UnicodeString1.Length = 2 * wcslen(wideString);

    KdPrint(("UnicodeString:%wZ\n", &UnicodeString1));

    // 清理内存
    ExFreePool(UnicodeString1.Buffer);
    UnicodeString1.Buffer = NULL;
    UnicodeString1.Length = UnicodeString1.MaximumLength = 0;
}

#pragma INITCODE
VOID StringCopyTest()
{
    // 初始化 UnicodeString1
    UNICODE_STRING UnicodeString1;
    RtlInitUnicodeString(&UnicodeString1, L"Hello Wrold");

    // 初始化UnicodeString2
    UNICODE_STRING UnicodeString2 = { 0 };
    UnicodeString2.Buffer = (PWSTR)ExAllocatePool(PagedPool, BUFFER_SIZE);
    UnicodeString2.MaximumLength = BUFFER_SIZE;

    // 将初始化 UnicodeString2 拷贝到 UnicodeString1
    RtlCopyUnicodeString(&UnicodeString2, &UnicodeString1);

    // 分别显示 UnicodeString1 和 UnicodeString2
    KdPrint(("UnicodeString1:%wz\n", &UnicodeString1));
    KdPrint(("UnicodeString2:%wz\n", &UnicodeString2));

    // 销毁 UnicodeString2
    // 注意 !!UnicodeString1 不用销毁
    RtlFreeUnicodeString(&UnicodeString2);
}

#pragma INITCODE
VOID StringCompareTest()
{
    // 初始化 UnicodeString1
    UNICODE_STRING UnicodeString1;
    RtlInitUnicodeString(&UnicodeString1, L"Hello world");

    // 初始化 UnicodeString2
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
    // 初始化 UnicodeString1
    UNICODE_STRING UnicodeString1;
    RtlInitUnicodeString(&UnicodeString1, L"Hello World");

    // 变化前
    KdPrint(("UnicodeString1:%wz\n", &UnicodeString1));

    // 变大写
    RtlUpcaseUnicodeString(&UnicodeString1, &UnicodeString1, FALSE);

    // 变化后
    KdPrint(("UnicodeString1:%wZ\n", &UnicodeString1));
}

#pragma INITCODE
VOID StringToIntegerTest()
{
    // (1)字符串转换成数字
    // 初始化 UnicodeString1
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

    // (2)数字转换成字符串
    // 初始化 UnicodeString2
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

    //销毁UnicodeString2
    //注意!!UnicodeString1不用销毁
    RtlFreeUnicodeString(&Unicodestring2);
}

#pragma INITCODE
VOID StringConverTest()
{
    //(1)将UNICODE_STRING字符串转换成ANSI_STRING字符串
    //初始化UnicodeString1
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

    // 销毁 AnsiString1
    RtlFreeAnsiString(&AnsiString1);

    // (2)将ANSI_STRING字符串转换成UNICODE_STRING字符串
    // 初始化AnsiString2
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

    // 销毁 UnicodeString2
    RtlFreeUnicodeString(&UnicodeString2);
}

VOID StringTest()
{
    //字符串初始化实验
//	StringInitTest();

    //字符串拷贝实验
//	StringCopyTest();

    //字符串比较实验
//	StringCompareTest();

    //字符串变大写实验
//	StringToUpperTest();

    //字符串与整型相互转化实验
//	StringToIntegerTest();

    //ANSI_STRING字符串与UNICODE_STRING字符串相互转换实验
    StringConverTest();

}