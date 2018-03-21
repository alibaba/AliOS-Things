/**
******************************************************************************
* @file    paltform_rng.c 
* @author  William Xu
* @version V1.0.0
* @date    05-May-2014
* @brief   This file provide RNG driver functions.
******************************************************************************
*
*  The MIT License
*  Copyright (c) 2014 MXCHIP Inc.
*
*  Permission is hereby granted, free of charge, to any person obtaining a copy 
*  of this software and associated documentation files (the "Software"), to deal
*  in the Software without restriction, including without limitation the rights 
*  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
*  copies of the Software, and to permit persons to whom the Software is furnished
*  to do so, subject to the following conditions:
*
*  The above copyright notice and this permission notice shall be included in
*  all copies or substantial portions of the Software.
*
*  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR 
*  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
*  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE 
*  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
*  WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR 
*  IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
******************************************************************************
*/ 


//#include "MICOPlatform.h"
//#include "MICORTOS.h"

#include "board.h"
#include "platform_peripheral.h"
#include "platform_logging.h"


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
        *pWord = Rand3(OSC8M_Get());
    }

    if(remainByteCount){
        tempRDM = Rand3(OSC8M_Get());
        memcpy(pByte, &tempRDM, (size_t)remainByteCount);
    }
    
    return kNoErr;
}
