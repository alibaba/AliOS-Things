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
 * Bit-manipulation library
 * $Revision: 1.2 $
 */

#include "mu_bits.h"

/*
* implementation
*/
int MUSB_BitsGet(const uint8_t *pBuffer, uint8_t bOffset, uint8_t bCount,
                 uint32_t *pdwResult)
{
    uint8_t bTopByte;
    int8_t bByte;
    uint8_t bTopBits;
    uint32_t dwTemp = 0;
    uint8_t bTopBit = bOffset + bCount;
    int iError = 0;

    if(pBuffer && pdwResult)
    {
        if((bTopBit <= 32) && (bOffset < 8))
        {
            /* gather/mask, avoiding *, / or large left shifts */
            bTopByte = (bTopBit - 1) >> 3;
            bTopBits = 8 - (((bTopByte + 1) << 3) - bTopBit);
            bByte = (int8_t)bTopByte;
            if(bTopBits)
            {
                dwTemp = pBuffer[bByte--] & ~(0xff << bTopBits);
            }
            else
            {
                dwTemp = pBuffer[bByte--];
            }
            for(; bByte >= 0; bByte--)
            {
                dwTemp <<= 8;
                dwTemp |= pBuffer[bByte];
            }
            /* shift & return */
            *pdwResult = dwTemp >> bOffset;
        }
        else
        {
            iError = -2;
        }
    }
    else
    {
        iError = -1;
    }
    return (iError);
}

/*
* implementation
*/
int MUSB_BitsSet(uint8_t *pBuffer, uint8_t bOffset, uint8_t bCount,
                 uint32_t dwValue)
{
    uint8_t bTopByte;
    uint8_t bByte;
    uint8_t bMask;
    uint8_t bTopBits;
    uint32_t dwTemp;
    uint8_t bTopBit = bOffset + bCount;
    int iError = 0;

    if(pBuffer)
    {
        if((bTopBit > 32) || (bOffset > 7) || !bCount)
        {
            iError = -2;
        }
        else
        {
            bTopByte = (bTopBit - 1) >> 3;
            bTopBits = 8 - (((bTopByte + 1) << 3) - bTopBit);
            /* shift into position */
            dwTemp = dwValue << bOffset;

            /* compute mask of bits to set */
            bMask = 0xff << bOffset;
            /* see if we are within one byte */
            if(!bTopByte)
            {
                /* special case */
                bMask &= ~(0xff << bTopBits);
            }
            /* clear only the bits we will set */
            *pBuffer &= ~bMask;
            /* set new bits */
            *pBuffer |= (uint8_t)(dwTemp & bMask);

            for(bByte = 1; bByte < bTopByte + 1; bByte++)
            {
                dwTemp >>= 8;
                if(bByte == bTopByte)
                {
                    /* compute mask */
                    bMask = ~(0xff << bTopBits);
                    /* clear only the bits we will set */
                    pBuffer[bByte] &= ~bMask;
                    /* set new bits */
                    pBuffer[bByte] |= (dwTemp & bMask);
                }
                else
                {
                    /* in the middle; set all bits */
                    pBuffer[bByte] = (uint8_t)(dwTemp & 0xff);
                }
            }
        }
    }
    else
    {
        iError = -1;
    }
    return (iError);
}
