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

#ifndef __SWM320_SPI_H__
#define __SWM320_SPI_H__

typedef struct {
    uint8_t  FrameFormat;
    uint8_t  SampleEdge;
    uint8_t  IdleLevel;
    uint8_t  WordSize;
    uint8_t  Master;
    uint8_t  clkDiv;

    uint8_t  RXHFullIEn;
    uint8_t  TXEmptyIEn;
    uint8_t  TXCompleteIEn;
} SPI_InitStructure;

#define SPI_FORMAT_SPI          0
#define SPI_FORMAT_TI_SSI       1

#define SPI_FIRST_EDGE          0
#define SPI_SECOND_EDGE         1

#define SPI_LOW_LEVEL           0
#define SPI_HIGH_LEVEL          1

#define SPI_CLKDIV_4            0
#define SPI_CLKDIV_8            1
#define SPI_CLKDIV_16           2
#define SPI_CLKDIV_32           3
#define SPI_CLKDIV_64           4
#define SPI_CLKDIV_128          5
#define SPI_CLKDIV_256          6
#define SPI_CLKDIV_512          7

void SPI_Init(SPI_TypeDef * SPIx, SPI_InitStructure * initStruct);
void SPI_Open(SPI_TypeDef * SPIx);
void SPI_Close(SPI_TypeDef * SPIx);

uint32_t SPI_Read(SPI_TypeDef * SPIx);
void SPI_Write(SPI_TypeDef * SPIx, uint32_t data);
void SPI_WriteWithWait(SPI_TypeDef * SPIx, uint32_t data);
uint32_t SPI_ReadWrite(SPI_TypeDef * SPIx, uint32_t data);

uint32_t SPI_IsRXEmpty(SPI_TypeDef * SPIx);
uint32_t SPI_IsTXFull(SPI_TypeDef * SPIx);
uint32_t SPI_IsTXEmpty(SPI_TypeDef * SPIx);

void SPI_INTRXHalfFullEn(SPI_TypeDef * SPIx);
void SPI_INTRXHalfFullDis(SPI_TypeDef * SPIx);
uint32_t SPI_INTRXHalfFullStat(SPI_TypeDef * SPIx);
void SPI_INTRXFullEn(SPI_TypeDef * SPIx);
void SPI_INTRXFullDis(SPI_TypeDef * SPIx);
uint32_t SPI_INTRXFullStat(SPI_TypeDef * SPIx);
void SPI_INTRXOverflowEn(SPI_TypeDef * SPIx);
void SPI_INTRXOverflowDis(SPI_TypeDef * SPIx);
void SPI_INTRXOverflowClr(SPI_TypeDef * SPIx);
uint32_t SPI_INTRXOverflowStat(SPI_TypeDef * SPIx);

void SPI_INTTXHalfFullEn(SPI_TypeDef * SPIx);
void SPI_INTTXHalfFullDis(SPI_TypeDef * SPIx);
uint32_t SPI_INTTXHalfFullStat(SPI_TypeDef * SPIx);
void SPI_INTTXEmptyEn(SPI_TypeDef * SPIx);
void SPI_INTTXEmptyDis(SPI_TypeDef * SPIx);
uint32_t SPI_INTTXEmptyStat(SPI_TypeDef * SPIx);
void SPI_INTTXCompleteEn(SPI_TypeDef * SPIx);
void SPI_INTTXCompleteDis(SPI_TypeDef * SPIx);
void SPI_INTTXCompleteClr(SPI_TypeDef * SPIx);
uint32_t SPI_INTTXCompleteStat(SPI_TypeDef * SPIx);


#endif //__SWM320_SPI_H__
