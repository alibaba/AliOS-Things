/*
* Copyright (c) 2008-2017 Allwinner Technology Co. Ltd.
* All rights reserved.
*
* File : StringContainer.h
* Description :
* History :
*   Author  : Khan <chengkan@allwinnertech.com>
*   Date    : 2017/3/2
*/

#ifndef STRINGCONTAINER_H
#define STRINGCONTAINER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "BaseUtils.h"

typedef struct _StringCtn StringCtn;
struct _StringCtn
{
    char* mString;
    kbool (*setTo8)(void* hself, const char*, size_t);
    kbool (*setTo16)(void* hself, const cdx_uint16*, size_t);
    void  (*clear)(void* hself);
    const char* (*string)(void* hself);
};

StringCtn* GenerateStringContainer(void);
kbool EraseStringContainer(void*);
#endif
