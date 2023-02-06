
#include "LinkList.h"

#pragma INITCODE
VOID LinkListTest()
{
    LIST_ENTRY linkListHead;
    // ��ʼ������
    InitializeListHead(&linkListHead);

    PZLCLINKLISTSTRUCT pData;
    ULONG i = 0;
    // �������в���10��Ԫ��
    KdPrint(("Begin insert to link list"));
    for (i = 0; i < 10; i++)
    {
        pData = (PZLCLINKLISTSTRUCT)ExAllocatePool(PagedPool, sizeof(ZLCLINKLISTSTRUCT));
        pData->number = i;
        InsertHeadList(&linkListHead, &pData->ListEntry);
    }

    // ��������ȡ��������ʾ
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
