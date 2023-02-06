#pragma once

#include "Driver.h"

typedef struct _ZLCLOOKASIDESTRUCT
{
    CHAR buffer[64];
}ZLCLOOKASIDESTRUCT, * PZLCLOOKASIDESTRUCT;


VOID LookasideTest();