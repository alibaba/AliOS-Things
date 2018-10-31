/*******************************************************************************
  File Name:
    nm_common.c

  Summary:
    This module contains common APIs implementations.

  Description:
    This module contains common APIs implementations.
 *******************************************************************************/

//DOM-IGNORE-BEGIN
/*******************************************************************************
Copyright (c) 2015 released Microchip Technology Inc. All rights reserved.

Microchip licenses to you the right to use, modify, copy and distribute
Software only when embedded on a Microchip microcontroller or digital signal
controller that is integrated into your product or third party product
(pursuant to the sublicense terms in the accompanying license agreement).

You should refer to the license agreement accompanying this Software for
additional information regarding your rights and obligations.

SOFTWARE AND DOCUMENTATION ARE PROVIDED AS IS WITHOUT WARRANTY OF ANY KIND,
EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION, ANY WARRANTY OF
MERCHANTABILITY, TITLE, NON-INFRINGEMENT AND FITNESS FOR A PARTICULAR PURPOSE.
IN NO EVENT SHALL MICROCHIP OR ITS LICENSORS BE LIABLE OR OBLIGATED UNDER
CONTRACT, NEGLIGENCE, STRICT LIABILITY, CONTRIBUTION, BREACH OF WARRANTY, OR
OTHER LEGAL EQUITABLE THEORY ANY DIRECT OR INDIRECT DAMAGES OR EXPENSES
INCLUDING BUT NOT LIMITED TO ANY INCIDENTAL, SPECIAL, INDIRECT, PUNITIVE OR
CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA, COST OF PROCUREMENT OF
SUBSTITUTE GOODS, TECHNOLOGY, SERVICES, OR ANY CLAIMS BY THIRD PARTIES
(INCLUDING BUT NOT LIMITED TO ANY DEFENSE THEREOF), OR OTHER SIMILAR COSTS.
 *******************************************************************************/

#include "common/include/nm_common.h"

void m2m_memcpy(uint8* pDst,uint8* pSrc,uint32 sz)
{
	if(sz == 0) return;
	do
	{
		*pDst = *pSrc;
		pDst++;
		pSrc++;
	}while(--sz);
}
uint8 m2m_checksum(uint8* buf, int sz)
{
	uint8 cs = 0;
	while(--sz)
	{
		cs ^= *buf;
		buf++;
	}

	return cs;
}

void m2m_memset(uint8* pBuf,uint8 val,uint32 sz)
{
	if(sz == 0) return;
	do
	{
		*pBuf = val;
		pBuf++;
	}while(--sz);
}

uint16 m2m_strlen(uint8 *pcStr)
{
	uint16	u16StrLen = 0;
	while(*pcStr)
	{
		u16StrLen ++;
		pcStr++;
	}
	return u16StrLen;
}

uint8 m2m_strncmp(uint8 *pcS1, uint8 *pcS2, uint16 u16Len)
{
    for ( ; u16Len > 0; pcS1++, pcS2++, --u16Len)
	if (*pcS1 != *pcS2)
	    return ((*(uint8 *)pcS1 < *(uint8 *)pcS2) ? -1 : +1);
	else if (*pcS1 == '\0')
	    return 0;
    return 0;
}

/* Finds the occurance of pcStr in pcIn.
If pcStr is part of pcIn it returns a valid pointer to the start of pcStr within pcIn.
Otherwise a NULL Pointer is returned.
*/
uint8 * m2m_strstr(uint8 *pcIn, uint8 *pcStr)
{
    uint8 u8c;
    uint16 u16StrLen;

    u8c = *pcStr++;
    if (!u8c)
        return (uint8 *) pcIn;	// Trivial empty string case

    u16StrLen = m2m_strlen(pcStr);
    do {
        uint8 u8Sc;

        do {
            u8Sc = *pcIn++;
            if (!u8Sc)
                return (uint8 *) 0;
        } while (u8Sc != u8c);
    } while (m2m_strncmp(pcIn, pcStr, u16StrLen) != 0);

    return (uint8 *) (pcIn - 1);
}

sint8 m2m_memcmp(uint8 *pu8Buff1,uint8 *pu8Buff2 ,uint32 u32Size)
{
	uint32	i;
	sint8		s8Result = 0;
	for(i	 = 0 ; i < u32Size ; i++)
	{
		if(pu8Buff1[i] != pu8Buff2[i])
		{
			s8Result = 1;
			break;
		}
	}
	return s8Result;
}

/*!
 *	@fn			nm_sleep
 *	@brief		Sleep in units of mSec
 *	@param[IN]	u32TimeMsec
 *				Time in milliseconds
 */
void nm_sleep(uint32 u32TimeMsec)
{
    WDRV_TIME_DELAY(u32TimeMsec);
}

/*!
 *	@fn		nm_reset
 *	@brief	Reset NMC1500 SoC by setting CHIP_EN and RESET_N signals low,
 *			 CHIP_EN high then RESET_N high
 */
void nm_reset(void)
{
    WDRV_STUB_GPIO_ChipDisable();  
    WDRV_STUB_GPIO_ModuleReset();
    nm_sleep(100);
    WDRV_STUB_GPIO_ChipEnable();
    nm_sleep(100);
    WDRV_STUB_GPIO_ModuleUnreset();
    nm_sleep(100);
}

//DOM-IGNORE-END
