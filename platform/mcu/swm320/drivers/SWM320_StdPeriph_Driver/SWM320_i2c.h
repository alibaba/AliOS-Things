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

#ifndef __SWM320_I2C_H__
#define __SWM320_I2C_H__

typedef struct {
    uint8_t  Master;
    uint8_t  Addr7b;

    uint32_t MstClk;
    uint8_t  MstIEn;

    uint16_t SlvAddr;
    uint8_t  SlvRxEndIEn;
    uint8_t  SlvTxEndIEn;
    uint8_t  SlvSTADetIEn;
    uint8_t  SlvSTODetIEn;
    uint8_t  SlvRdReqIEn;
    uint8_t  SlvWrReqIEn;
} I2C_InitStructure;


void I2C_Init(I2C_TypeDef * I2Cx, I2C_InitStructure * initStruct);

void I2C_Open(I2C_TypeDef * I2Cx);
void I2C_Close(I2C_TypeDef * I2Cx);

#endif //__SWM320_I2C_H__
