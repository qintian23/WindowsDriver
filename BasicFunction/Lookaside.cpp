
#include "Lookaside.h"

#pragma INITCODE
VOID LookasideTest()
{
    //初始化Lookaside对象
    PAGED_LOOKASIDE_LIST pageList;
    ExInitializePagedLookasideList(&pageList, NULL, NULL, 0, sizeof(ZLCLOOKASIDESTRUCT), '1234', 0);

#define ARRAY_NUMBER 50
    PZLCLOOKASIDESTRUCT MyObjectArray[ARRAY_NUMBER];
    // 模拟频繁申请内存
    for (int i = 0; i < ARRAY_NUMBER; i++)
    {
        MyObjectArray[i] = (PZLCLOOKASIDESTRUCT)ExAllocateFromPagedLookasideList(&pageList);
    }

    // 模拟频繁回收内存
    for (int i = 0; i < ARRAY_NUMBER; i++)
    {
        ExFreeToPagedLookasideList(&pageList, MyObjectArray[i]);
        MyObjectArray[i] = NULL;
    }
    ExDeletePagedLookasideList(&pageList);
    //删除Lookaside对象

    return VOID();
}
