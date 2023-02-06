#pragma once

#include "Driver.h"

typedef struct _ZLCLINKLISTSTRUCT
{
    ULONG number;
    LIST_ENTRY ListEntry;
}ZLCLINKLISTSTRUCT, * PZLCLINKLISTSTRUCT;

VOID LinkListTest();

