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

#ifndef __SWM320_LCD_H__
#define __SWM320_LCD_H__

typedef struct {
    uint8_t  Interface;

    uint8_t  Dir;
    uint16_t HnPixel;
    uint16_t VnPixel;
    uint8_t  Hfp;
    uint8_t  Hbp;
    uint8_t  Vfp;
    uint8_t  Vbp;
    uint8_t  ClkDiv;
    uint8_t  SamplEdge;
    uint8_t  ClkAlways;
    uint8_t  HsyncWidth;
    uint8_t  T_CSf_WRf;
    uint8_t  T_WRnHold;
    uint8_t  T_WRr_CSr;
    uint8_t  T_CSr_CSf;

    uint8_t  IntEOTEn;
} LCD_InitStructure;


#define LCD_INTERFACE_RGB   0
#define LCD_INTERFACE_I80   1
#define LCD_INTERFACE_M68   2

#define LCD_DIR_LANDSCAPE   0
#define LCD_DIR_PORTRAIT    1

#define LCD_SAMPLEDGE_RISE  0
#define LCD_SAMPLEDGE_FALL  1

#define LCD_HSYNC_1DOTCLK   0
#define LCD_HSYNC_2DOTCLK   1
#define LCD_HSYNC_3DOTCLK   2
#define LCD_HSYNC_4DOTCLK   3

#define LCD_CLKDIV_2        0
#define LCD_CLKDIV_4        1
#define LCD_CLKDIV_6        2
#define LCD_CLKDIV_8        3
#define LCD_CLKDIV_10       4
#define LCD_CLKDIV_12       5
#define LCD_CLKDIV_14       6
#define LCD_CLKDIV_16       7
#define LCD_CLKDIV_18       8
#define LCD_CLKDIV_20       9
#define LCD_CLKDIV_22       10
#define LCD_CLKDIV_24       11
#define LCD_CLKDIV_26       12
#define LCD_CLKDIV_28       13
#define LCD_CLKDIV_30       14
#define LCD_CLKDIV_32       15
#define LCD_CLKDIV_34       16
#define LCD_CLKDIV_36       17
#define LCD_CLKDIV_38       18
#define LCD_CLKDIV_40       19
#define LCD_CLKDIV_42       20
#define LCD_CLKDIV_44       21
#define LCD_CLKDIV_46       22
#define LCD_CLKDIV_48       23
#define LCD_CLKDIV_50       24
#define LCD_CLKDIV_52       25
#define LCD_CLKDIV_54       26
#define LCD_CLKDIV_56       27
#define LCD_CLKDIV_58       28
#define LCD_CLKDIV_60       29
#define LCD_CLKDIV_62       30
#define LCD_CLKDIV_64       31


void LCD_Init(LCD_TypeDef * LCDx, LCD_InitStructure * initStruct);
void LCD_Start(LCD_TypeDef * LCDx);
uint32_t LCD_IsBusy(LCD_TypeDef * LCDx);

void LCD_I80_WriteReg(LCD_TypeDef * LCDx, uint16_t reg, uint16_t val[], uint16_t cnt);
void LCD_I80_WriteOneReg(LCD_TypeDef * LCDx, uint16_t reg, uint16_t val);
void LCD_I80_WriteData(LCD_TypeDef * LCDx, uint16_t data[], uint16_t cnt);
void LCD_I80_WriteOneData(LCD_TypeDef * LCDx, uint16_t val);

void LCD_INTEn(LCD_TypeDef * LCDx);
void LCD_INTDis(LCD_TypeDef * LCDx);
void LCD_INTClr(LCD_TypeDef * LCDx);
uint32_t LCD_INTStat(LCD_TypeDef * LCDx);


#endif //__SWM320_LCD_H__
