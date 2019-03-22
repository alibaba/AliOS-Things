/******************************************************************************************************************************************
* @attention
*
* THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS WITH CODING INFORMATION
* REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME. AS A RESULT, SYNWIT SHALL NOT BE HELD LIABLE
* FOR ANY DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE CONTENT
* OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING INFORMATION CONTAINED HEREIN IN CONN-
* -ECTION WITH THEIR PRODUCTS.
*
* COPYRIGHT 2012 Synwit Technology
*******************************************************************************************************************************************/

#ifndef __SWM320_CRC_H__
#define __SWM320_CRC_H__


#define CRC32_IN32  0
#define CRC32_IN16  2
#define CRC32_IN8   4
#define CRC16_IN16  3
#define CRC16_IN8   5


void CRC_Init(CRC_TypeDef * CRCx, uint32_t mode, uint32_t out_not, uint32_t out_rev, uint32_t ini_val);

static __INLINE void CRC_Write(uint32_t data)
{
    CRC->DATAIN = data;
}

static __INLINE uint32_t CRC_Result(void)
{
    return CRC->RESULT;
}

#endif //__SWM320_CRC_H__
