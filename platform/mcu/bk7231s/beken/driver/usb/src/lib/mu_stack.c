/******************************************************************
*                                                                *
*         Copyright Mentor Graphics Corporation 2005             *
*                                                                *
*                All Rights Reserved.                            *
*                                                                *
*    THIS WORK CONTAINS TRADE SECRET AND PROPRIETARY INFORMATION *
*  WHICH IS THE PROPERTY OF MENTOR GRAPHICS CORPORATION OR ITS   *
*  LICENSORS AND IS SUBJECT TO LICENSE TERMS.                    *
*                                                                *
******************************************************************/

/*
 * Stack (LIFO data structure) library
 * $Revision: 1.5 $
 */

#include "mu_stack.h"
#include "mu_mem.h"

/* Call this (portable) or initialize struct (non-portable) */
MUSB_Stack *MUSB_StackInit(MUSB_Stack *pStack,
                           uint_fast16_t wItemSize,
                           uint_fast16_t wStaticItemCount,
                           void *pStaticBuffer)
{
    MUSB_Array *pArray;
    uint8_t bAlloc = FALSE;
    MUSB_Stack *pResult = pStack;
    if(!pStack)
    {
        bAlloc = TRUE;
        pResult = (MUSB_Stack *)MUSB_MemAlloc(sizeof(MUSB_Stack));
    }
    if(pResult)
    {
        pResult->wNextTopIndex = 0;
        pArray = MUSB_ArrayInit(&(pResult->Array), wItemSize,
                                wStaticItemCount, pStaticBuffer);
        if(!pArray)
        {
            if(bAlloc)
            {
                MUSB_MemFree(pResult);
            }
            pResult = NULL;
        }
    }
    return pResult;
}

uint8_t MUSB_StackClear(MUSB_Stack *pStack)
{
    if(pStack)
    {
        pStack->wNextTopIndex = 0;
        MUSB_ArrayClear(&(pStack->Array));
    }
    return FALSE;
}

uint_fast16_t MUSB_StackSize(MUSB_Stack *pStack)
{
    if(pStack)
    {
        return MUSB_ArrayLength(&(pStack->Array));
    }
    return 0;
}

void *MUSB_StackTop(MUSB_Stack *pStack)
{
    if(pStack && pStack->wNextTopIndex)
    {
        return MUSB_ArrayFetch(&(pStack->Array), pStack->wNextTopIndex - 1);
    }
    return NULL;
}

uint8_t MUSB_StackPop(MUSB_Stack *pStack, void *pItem)
{
    void *pResult = MUSB_StackTop(pStack);
    if(pResult && pItem)
    {
        MUSB_MemCopy(pItem, pResult, pStack->Array.wItemSize);
        pStack->wNextTopIndex--;
        return TRUE;
    }
    return FALSE;
}

uint8_t MUSB_StackPush(MUSB_Stack *pStack, const void *pItem)
{
    uint_fast16_t wIndex, wCount;
    uint8_t *pLocation;
    uint8_t bOk = TRUE;

    if(pStack && pItem)
    {
        wIndex = pStack->wNextTopIndex;
        wCount = MUSB_ArrayLength(&(pStack->Array));
        if(wCount > wIndex)
        {
            if(wIndex >= pStack->Array.wStaticItemCount)
            {
                pLocation = (uint8_t *)pStack->Array.pDynamicBuffer;
                pLocation += pStack->Array.wItemSize *
                             (wIndex - pStack->Array.wStaticItemCount);
            }
            else
            {
                pLocation = (uint8_t *)pStack->Array.pStaticBuffer;
                pLocation += pStack->Array.wItemSize * wIndex;
            }
            MUSB_MemCopy(pLocation, pItem, pStack->Array.wItemSize);
        }
        else
        {
            bOk = MUSB_ArrayAppend(&(pStack->Array), pItem);
            if(bOk)
            {
                pStack->wNextTopIndex++;
            }
        }
        return bOk;
    }
    return FALSE;
}

