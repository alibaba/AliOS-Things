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

#include "SWM320.h"
#include "SWM320_lcd.h"

#include <string.h>

void LCD_Init(LCD_TypeDef * LCDx, LCD_InitStructure * initStruct)
{
    switch((uint32_t)LCDx)
    {
    case ((uint32_t)LCD):
        SYS->CLKEN |= (0x01 << SYS_CLKEN_LCD_Pos);
        break;
    }

    if(initStruct->Interface == LCD_INTERFACE_RGB)
    {
        LCDx->START = (0 << LCD_START_MPUEN_Pos);

        if(initStruct->Dir == LCD_DIR_LANDSCAPE)
        {
            LCDx->CR0 = ((initStruct->HnPixel - 1) << LCD_CR0_HPIX_Pos) |
                        ((initStruct->VnPixel - 1) << LCD_CR0_VPIX_Pos) |
                        (initStruct->ClkAlways << LCD_CR0_DCLK_Pos) |
                        (initStruct->HsyncWidth << LCD_CR0_HLOW_Pos);

            LCDx->CR1 = (initStruct->Dir << LCD_CR1_DIRV_Pos) |
                        ((initStruct->Hfp - 1) << LCD_CR1_HFP_Pos)  |
                        ((initStruct->Hbp - 1) << LCD_CR1_HBP_Pos)  |
                        ((initStruct->Vfp - 1) << LCD_CR1_VFP_Pos)  |
                        ((initStruct->Vbp - 1) << LCD_CR1_VBP_Pos)  |
                        (initStruct->ClkDiv << LCD_CR1_DCLKDIV_Pos) |
                        (initStruct->SamplEdge << LCD_CR1_DCLKINV_Pos);
        }
        else
        {
            LCDx->CR0 = ((initStruct->HnPixel - 1) << LCD_CR0_VPIX_Pos) |
                        ((initStruct->VnPixel - 1) << LCD_CR0_HPIX_Pos) |
                        (initStruct->ClkAlways << LCD_CR0_DCLK_Pos) |
                        (initStruct->HsyncWidth << LCD_CR0_HLOW_Pos);

            LCDx->CR1 = (initStruct->Dir << LCD_CR1_DIRV_Pos) |
                        ((initStruct->Hfp - 1) << LCD_CR1_VFP_Pos)  |
                        ((initStruct->Hbp - 1) << LCD_CR1_VBP_Pos)  |
                        ((initStruct->Vfp - 1) << LCD_CR1_HFP_Pos)  |
                        ((initStruct->Vbp - 1) << LCD_CR1_HBP_Pos)  |
                        (initStruct->ClkDiv << LCD_CR1_DCLKDIV_Pos) |
                        (initStruct->SamplEdge << LCD_CR1_DCLKINV_Pos);
        }
    }
    else if(initStruct->Interface == LCD_INTERFACE_I80)
    {
        LCDx->START = (1 << LCD_START_MPUEN_Pos);

        LCDx->CR1 = (1 << LCD_CR1_I80_Pos) |
                    (initStruct->T_CSf_WRf << LCD_CR1_TAS_Pos) |
                    (initStruct->T_WRnHold << LCD_CR1_TPWLW_Pos) |
                    (initStruct->T_WRr_CSr << LCD_CR1_TAH_Pos) |
                    (initStruct->T_CSr_CSf << LCD_CR1_TTAIL_Pos);
    }

    LCDx->IE = 1;
    LCDx->IF = 1;   //Çå³ý±êÖ¾
    if(initStruct->IntEOTEn) LCD_INTEn(LCDx);
    else                     LCD_INTDis(LCDx);

    switch((uint32_t)LCDx)
    {
    case ((uint32_t)LCD):
        if(initStruct->IntEOTEn)
        {
            NVIC_EnableIRQ(LCD_IRQn);
        }
        else
        {
            NVIC_DisableIRQ(LCD_IRQn);
        }
        break;
    }
}

void LCD_Start(LCD_TypeDef * LCDx)
{
    LCDx->START |= (1 << LCD_START_GO_Pos);
}

uint32_t LCD_IsBusy(LCD_TypeDef * LCDx)
{
    return (LCDx->START & LCD_START_GO_Msk) ? 1 : 0;
}

void LCD_I80_WriteReg(LCD_TypeDef * LCDx, uint16_t reg, uint16_t val[], uint16_t cnt)
{
    LCD->SRCADDR = (uint32_t)val;
    LCD->CR0 &= ~LCD_CR0_DLEN_Msk;
    LCD->CR0 |= ((cnt-1) << LCD_CR0_DLEN_Pos);

    LCD->CR1 |=  (1 << LCD_CR1_CMD_Pos);
    LCD->CR1 &= ~LCD_CR1_REG_Msk;
    LCD->CR1 |= (reg << LCD_CR1_REG_Pos);

    LCD_Start(LCDx);
    while(LCD_IsBusy(LCDx));
}

void LCD_I80_WriteOneReg(LCD_TypeDef * LCDx, uint16_t reg, uint16_t val)
{
    uint16_t buf[1] __attribute__((aligned(4)));

    buf[0] = val;

    LCD_I80_WriteReg(LCDx, reg, buf, 1);
}

void LCD_I80_WriteData(LCD_TypeDef * LCDx, uint16_t val[], uint16_t cnt)
{
    LCD->SRCADDR = (uint32_t)val;
    LCD->CR0 &= ~LCD_CR0_DLEN_Msk;
    LCD->CR0 |= ((cnt-1) << LCD_CR0_DLEN_Pos);

    LCD->CR1 &= ~(1 << LCD_CR1_CMD_Pos);

    LCD_Start(LCDx);
    while(LCD_IsBusy(LCDx));
}

void LCD_I80_WriteOneData(LCD_TypeDef * LCDx, uint16_t val)
{
    uint16_t buf[1] __attribute__((aligned(4)));

    buf[0] = val;

    LCD_I80_WriteData(LCDx, buf, 2);
}

void LCD_INTEn(LCD_TypeDef * LCDx)
{
    LCDx->IM = 0;
}

void LCD_INTDis(LCD_TypeDef * LCDx)
{
    LCDx->IM = 1;
}

void LCD_INTClr(LCD_TypeDef * LCDx)
{
    LCDx->IF = 1;
}

uint32_t LCD_INTStat(LCD_TypeDef * LCDx)
{
    return (LCDx->IF & 0x01) ? 1 : 0;
}
