/**
 ******************************************************************************
 * @file    platform_rng.c
 * @author  William Xu
 * @version V1.0.0
 * @date    05-May-2014
 * @brief   This file provide RNG driver functions.
 ******************************************************************************
 *  UNPUBLISHED PROPRIETARY SOURCE CODE
 *  Copyright (c) 2016 MXCHIP Inc.
 *
 *  The contents of this file may not be disclosed to third parties, copied or
 *  duplicated in any form, in whole or in part, without the prior written
 *  permission of MXCHIP Corporation.
 ******************************************************************************
 */

#include "platform_peripheral.h"
#include "platform.h"

/******************************************************
 *                   Macros
 ******************************************************/


/******************************************************
 *                   Enumerations
 ******************************************************/

/******************************************************
 *                 Type Definitions
 ******************************************************/

 /******************************************************
 *                    Structures
 ******************************************************/


/******************************************************
 *                     Variables
 ******************************************************/

/******************************************************
 *               Function Declarations
 ******************************************************/

OSStatus platform_random_number_read( void *inBuffer, int inByteCount )
{
    // PLATFORM_TO_DO
     // PLATFORM_TO_DO
    int idx;
    uint32_t *pWord = inBuffer;
    uint32_t tempRDM;
    uint8_t *pByte = NULL;
    int inWordCount;
    int remainByteCount;

    inWordCount = inByteCount/4;
    remainByteCount = inByteCount%4;
    pByte = (uint8_t *)pWord+inWordCount*4;

    for(idx = 0; idx<inWordCount; idx++, pWord++){
        srand(mico_rtos_get_time());
        *pWord = rand();
    }

    if(remainByteCount){
        srand(mico_rtos_get_time());
        tempRDM = rand();
        memcpy(pByte, &tempRDM, (size_t)remainByteCount);
    }
    
    return kNoErr;
}
