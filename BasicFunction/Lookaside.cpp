
#include "Lookaside.h"

#pragma INITCODE
VOID LookasideTest()
{
    //��ʼ��Lookaside����
    PAGED_LOOKASIDE_LIST pageList;
    ExInitializePagedLookasideList(&pageList, NULL, NULL, 0, sizeof(ZLCLOOKASIDESTRUCT), '1234', 0);

#define ARRAY_NUMBER 50
    PZLCLOOKASIDESTRUCT MyObjectArray[ARRAY_NUMBER];
    // ģ��Ƶ�������ڴ�
    for (int i = 0; i < ARRAY_NUMBER; i++)
    {
        MyObjectArray[i] = (PZLCLOOKASIDESTRUCT)ExAllocateFromPagedLookasideList(&pageList);
    }

    // ģ��Ƶ�������ڴ�
    for (int i = 0; i < ARRAY_NUMBER; i++)
    {
        ExFreeToPagedLookasideList(&pageList, MyObjectArray[i]);
        MyObjectArray[i] = NULL;
    }
    ExDeletePagedLookasideList(&pageList);
    //ɾ��Lookaside����

    return VOID();
}
