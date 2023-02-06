
#include "LinkList.h"

#pragma INITCODE
VOID LinkListTest()
{
    LIST_ENTRY linkListHead;
    // 初始化链表
    InitializeListHead(&linkListHead);

    PZLCLINKLISTSTRUCT pData;
    ULONG i = 0;
    // 在链表中插入10个元素
    KdPrint(("Begin insert to link list"));
    for (i = 0; i < 10; i++)
    {
        pData = (PZLCLINKLISTSTRUCT)ExAllocatePool(PagedPool, sizeof(ZLCLINKLISTSTRUCT));
        pData->number = i;
        InsertHeadList(&linkListHead, &pData->ListEntry);
    }

    // 从链表中取出，并显示
    KdPrint(("Begin remove from link list \n"));
    while (!IsListEmpty(&linkListHead))
    {
        PLIST_ENTRY pEntry = RemoveTailList(&linkListHead);
        pData = CONTAINING_RECORD(pEntry,
            ZLCLINKLISTSTRUCT,
            ListEntry);
        KdPrint(("%d \n", pData->number));
        ExFreePool(pData);
    }

	return VOID();
}
