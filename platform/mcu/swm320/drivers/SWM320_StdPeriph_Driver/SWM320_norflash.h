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

#ifndef __SWM320_NORFLASH_H__
#define __SWM320_NORFLASH_H__

typedef struct {
    uint8_t DataWidth;

    uint8_t WELowPulseTime;
    uint8_t OEPreValidTime;

    uint8_t OperFinishIEn;
    uint8_t OperTimeoutIEn;
} NORFL_InitStructure;



void NORFL_Init(NORFL_InitStructure * initStruct);
uint32_t NORFL_ChipErase(void);
uint32_t NORFL_SectorErase(uint32_t addr);
uint32_t NORFL_Write(uint32_t addr, uint32_t data);
uint32_t NORFL_Read(uint32_t addr);
uint16_t NORFL_ReadID(uint32_t id_addr);


/*
#define NORFL_Read8(addr)           *((volatile uint8_t  *)(NORFLM_BASE + addr))
#define NORFL_Read16(addr)          *((volatile uint16_t *)(NORFLM_BASE + addr))
*/
#define NORFL_Read32(addr)          *((volatile uint32_t *)(NORFLM_BASE + addr))



#define NORFL_CMD_READ              0
#define NORFL_CMD_RESET             1
#define NORFL_CMD_AUTO_SELECT       2
#define NORFL_CMD_PROGRAM           3
#define NORFL_CMD_CHIP_ERASE        4
#define NORFL_CMD_SECTOR_ERASE      5

#endif // __SWM320_NORFLASH_H__
