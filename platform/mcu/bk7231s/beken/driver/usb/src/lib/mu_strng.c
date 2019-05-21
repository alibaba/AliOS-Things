/******************************************************************
*                                                                *
*        Copyright Mentor Graphics Corporation 2004              *
*                                                                *
*                All Rights Reserved.                            *
*                                                                *
*    THIS WORK CONTAINS TRADE SECRET AND PROPRIETARY INFORMATION *
*  WHICH IS THE PROPERTY OF MENTOR GRAPHICS CORPORATION OR ITS   *
*  LICENSORS AND IS SUBJECT TO LICENSE TERMS.                    *
*                                                                *
******************************************************************/

/*
 * Micro-string library
 * $Revision: 1.2 $
 */

#include "mu_strng.h"

static const char MGC_CapOffset = 'a' - 'A';

static int8_t MGC_CharCompare(char c1, char c2, uint8_t bCaseSensitive)
{
    char cc1 = c1;
    char cc2 = c2;

    if(!bCaseSensitive)
    {
        if((c1 >= 'a') && (c1 <= 'z'))
        {
            cc1 -= MGC_CapOffset;
        }
        if((c2 >= 'a') && (c2 <= 'z'))
        {
            cc2 -= MGC_CapOffset;
        }
    }
    return (cc1 - cc2);
}

/*
* implementation
*/
int8_t MUSB_StringCompare(const char *pString1,
                          const char *pString2, uint8_t bCaseSensitive)
{
    int8_t bResult = 0;
    const char *p1 = pString1;
    const char *p2 = pString2;

    /* sanity */
    if(!pString1 || !pString2)
    {
        return 1;
    }

    /* compare characters until mismatch or at least one string's end */
    while((0 == bResult) && *p1 && *p2)
    {
        bResult = MGC_CharCompare(*p1, *p2, bCaseSensitive);
        p1++;
        p2++;
    }
    if(0 == bResult)
    {
        /* strings matched until one's end; indicate different-sized strings */
        if(*p1 && !*p2)
        {
            /* first string is longer */
            bResult = 1;
        }
        else if(!*p1 && *p2)
        {
            /* second string is longer */
            bResult = -1;
        }
    }

    return bResult;
}

/*
* implementation
*/
uint8_t MUSB_StringConcat(char *pResult,
                          uint16_t wResultLength, const char *pConcat)
{
    const char *pAppend = pConcat;
    uint16_t wIndex = 0;
    uint8_t bResult = FALSE;

    /* sanity */
    if(!pResult || !pConcat)
    {
        return bResult;
    }

    while(pResult[wIndex] && (wIndex < wResultLength))
    {
        wIndex++;
    }
    if(wIndex < wResultLength)
    {
        while(*pAppend && (wIndex < wResultLength))
        {
            pResult[wIndex++] = *pAppend;
            pAppend++;
        }
        if(wIndex < wResultLength)
        {
            pResult[wIndex] = (char)0;
            bResult = TRUE;
        }
        else
        {
            pResult[wIndex - 1] = (char)0;
        }
    }

    return bResult;
}

/*
* implementation
*/
const char *MUSB_StringFind(const char *pString,
                            const char *pPattern, uint8_t bCaseSensitive)
{
    const char *pResult = NULL;
    const char *pSearch = pString;
    const char *pFind = pPattern;
    int32_t dwStringLength = MUSB_StringLength(pString);
    uint16_t wPatternLength = MUSB_StringLength(pPattern);
    uint16_t wFoundLength = 0;

    /* sanity */
    if(!pString || !pPattern)
    {
        return NULL;
    }

    /* find first candidate and prune if length impossible */
    while((dwStringLength >= (int32_t)wPatternLength) &&
            (wFoundLength < wPatternLength))
    {
        wFoundLength = 0;
        while((dwStringLength >= (int32_t)wPatternLength) &&
                (0 != MGC_CharCompare(*pSearch, *pFind, bCaseSensitive)))
        {
            pSearch++;
            dwStringLength--;
        }
        pResult = pSearch;
        if(dwStringLength >= (int32_t)wPatternLength)
        {
            while((wFoundLength < wPatternLength) &&
                    (0 == MGC_CharCompare(pSearch[wFoundLength], pPattern[wFoundLength], bCaseSensitive)))
            {
                wFoundLength++;
            }
        }
        dwStringLength--;
    }

    return (wFoundLength == wPatternLength) ? pResult : NULL;
}

/*
* implementation
*/
uint8_t MUSB_Stringize(char *pResult, uint16_t wResultLength,
                       uint32_t dwNumber, uint8_t bBase, uint8_t bJustification)
{
    uint8_t bDigit;
    uint16_t wIndex;
    uint16_t wCount;
    uint16_t wZeroCount = 0;
    uint8_t bOk = TRUE;
    uint32_t dwValue = dwNumber;

    if(!pResult || !wResultLength || (wResultLength <= bJustification))
    {
        bOk = FALSE;
    }
    switch(bBase)
    {
    case 2:
    case 8:
    case 10:
    case 16:
        break;
    default:
        bOk = FALSE;
    }
    if(bOk)
    {
        if(!dwNumber)
        {
            /* trivial case */
            for(wIndex = 0; wIndex < MUSB_MAX(bJustification, 1); wIndex++)
            {
                pResult[wIndex] = '0';
            }
            pResult[wIndex] = (char)0;
        }
        else
        {
            /* count digits */
            wCount = 0;
            while(dwValue)
            {
                dwValue /= bBase;
                wCount++;
            }

            /* justify now if needed */
            if(bJustification && (bJustification >= wCount))
            {
                wZeroCount = bJustification - wCount;
                for(wIndex = 0; wIndex < wZeroCount; wIndex++)
                {
                    pResult[wIndex] = '0';
                }
            }

            /* fail if overflow would result */
            if((wCount + wZeroCount) >= wResultLength)
            {
                bOk = FALSE;
            }
            else
            {
                /* do the real job */
                wIndex = wCount + wZeroCount;
                pResult[wIndex] = (char)0;
                dwValue = dwNumber;
                while(dwValue)
                {
                    bDigit = (uint8_t)(dwValue % bBase);
                    if(bDigit > 9)
                    {
                        pResult[--wIndex] = 'A' + bDigit - 10;
                    }
                    else
                    {
                        pResult[--wIndex] = '0' + bDigit;
                    }
                    dwValue /= bBase;
                }
            }
        }
    }

    return bOk;
}

/*
* implementation
*/
uint16_t MUSB_StringLength(const char *pString)
{
    uint16_t wResult = 0;
    const char *p = pString;

    if(p)
    {
        while(*p && (wResult < 0xffff))
        {
            p++;
            wResult++;
        }
    }
    return wResult;
}

static uint8_t MGC_GetDigitValue(const char bDigit, uint8_t *pbResult, uint8_t bBase)
{
    uint8_t bResult = TRUE;
    uint8_t bValue = bDigit - '0';

    if(bDigit < '0')
    {
        bResult = FALSE;
    }
    if(16 == bBase)
    {
        if(bDigit >= 'a')
        {
            bValue = bDigit - 'a' + 10;
        }
        else if(bDigit >= 'A')
        {
            bValue = bDigit - 'A' + 10;
        }
    }

    if(bValue >= bBase)
    {
        bResult = FALSE;
    }

    *pbResult = bValue;
    return bResult;
}

/*
* implementation
*/
int32_t MUSB_StringParse(const char *pString, const char **pEnd, uint8_t bBase)
{
    const char *pTest;
    uint8_t bOk;
    uint8_t bDigit;
    uint8_t bIsNegative = FALSE;
    const char *pDigit = pString;
    int32_t dwResult = 0;
    uint8_t bRealBase = bBase;

    /* sanity */
    if(!pString)
    {
        return 0;
    }

    if(pEnd)
    {
        *pEnd = pString;
    }
    if('-' == *pDigit)
    {
        bIsNegative = TRUE;
        pDigit++;
    }

    if(!bRealBase)
    {
        pTest = MUSB_StringFind(pDigit, "0x", FALSE);
        if(pTest == pDigit)
        {
            bRealBase = 16;
            pDigit += 2;
        }
        else if('0' == *pDigit)
        {
            bRealBase = 8;
            pDigit++;
        }
        else if('%' == *pDigit)
        {
            bRealBase = 2;
            pDigit++;
        }
        else
        {
            bRealBase = 10;
        }
    }

    bOk = MGC_GetDigitValue(*pDigit, &bDigit, bRealBase);
    while(bOk)
    {
        dwResult *= bRealBase;
        dwResult += bDigit;
        bOk = MGC_GetDigitValue(*++pDigit, &bDigit, bRealBase);
    }
    if(pEnd)
    {
        *pEnd = pDigit;
    }

    return bIsNegative ? (0 - dwResult) : dwResult;
}
