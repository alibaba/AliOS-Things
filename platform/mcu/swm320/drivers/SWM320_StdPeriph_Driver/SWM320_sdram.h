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

#ifndef __SWM320_SDRAM_H__
#define __SWM320_SDRAM_H__

typedef struct {
    uint8_t DataWidth;

    uint8_t CellSize;
    uint8_t CellBank;
    uint8_t CellWidth;
} SDRAM_InitStructure;

#define SDRAM_CELLSIZE_16Mb     3
#define SDRAM_CELLSIZE_64Mb     0
#define SDRAM_CELLSIZE_128Mb    1
#define SDRAM_CELLSIZE_256Mb    2

#define SDRAM_CELLBANK_2        0
#define SDRAM_CELLBANK_4        1

void SDRAM_Init(SDRAM_InitStructure * initStruct);

#endif //__SWM320_SDRAM_H__
