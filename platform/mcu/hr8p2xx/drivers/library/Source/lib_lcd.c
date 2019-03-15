/***************************************************************
 *Copyright (C), 2015, Shanghai Eastsoft Microelectronics Co., Ltd
 ***************************************************************/

#include "lib_lcd.h"
#include "system_HR8P2xx.h"
//#include "Lib_Timer.h"

ErrorStatus LCD_Init(LCD_InitStruType * LCD_InitStruct,LCDSEL_TYPE SELx)
{
    if(LCD_InitStruct->LCD_PRS>63)
    {
        return ERROR;
    }
    else
    {

        LCDC->CON0.PRS   = LCD_InitStruct->LCD_PRS;
    }

    switch(SELx)
    {
        case Sel_0:
            if(LCD_InitStruct->LCD_SEG0>0xFFFFFFFF)
            {
                return ERROR;
            }
            else
            {
                LCDC->SEL0.SEG  = LCD_InitStruct->LCD_SEG0;

            }
            break;
        case Sel_1:
#ifdef HR8P287
            if(LCD_InitStruct->LCD_SEG1>0x0000FFFF)
#else
            if(LCD_InitStruct->LCD_SEG1>0x00FFFFFF)
#endif
            {
                return ERROR;
            }
            else
            {
                LCDC->SEL1.SEG  = LCD_InitStruct->LCD_SEG1;


            }
            break;
        case Sel_All:
#ifdef HR8P287
            if((LCD_InitStruct->LCD_SEG0>0xFFFFFFFF)||(LCD_InitStruct->LCD_SEG1>0x0000FFFF))
#else
            if((LCD_InitStruct->LCD_SEG0>0xFFFFFFFF)||(LCD_InitStruct->LCD_SEG1>0x00FFFFFF))
#endif
            {
                return ERROR;
            }
            else
            {
                LCDC->SEL0.SEG  = LCD_InitStruct->LCD_SEG0; 
                LCDC->SEL1.SEG  = LCD_InitStruct->LCD_SEG1; 

            }
        default: break;

    }

    LCDC->CON0.COMS = LCD_InitStruct->LCD_Coms;

    LCDC->CON0.FLIK  = LCD_InitStruct->LCD_LcdFlik;

    LCDC->CON0.WFS  = LCD_InitStruct->LCD_LcdWFS;

    LCDC->CON0.BIAS  = LCD_InitStruct->LCD_Bias;

    LCDC->CON0.RS  = LCD_InitStruct->LCD_RS;

    LCDC->CON0.BVS  = LCD_InitStruct->LCD_Grayscale;

    LCDC->CON0.BIAS_SEL=LCD_InitStruct->LCD_BIAS_SEL;

    LCDC->CON0.BIAS_PWMSEL=LCD_InitStruct->LCD_BIAS_PWMSEL;

    return SUCCESS;
}

void LCD_ITConfig(LCD_TYPE_IE LCD_IT,TYPE_FUNCEN NewState)
{
#ifdef HR8P287
    if (NewState != DISABLE)
    {
        LCDC->IE.Word |= (uint32_t)LCD_IT;
    }
    else
    {
        LCDC->IE.Word &= ~((uint32_t)LCD_IT);
    }
#else
    if (NewState != DISABLE)
    {
        LCDC->IF.Word |= (uint32_t)LCD_IT;
    }
    else
    {
        LCDC->IF.Word &= ~((uint32_t)LCD_IT);
    }
#endif
}

ITStatus LCD_GetRFITStatus(void)
{
    ITStatus bitstatus = RESET;

    if ((LCDC->IF.RFIF != (uint32_t)RESET))
    {

        bitstatus = SET;
    }
    else
    {

        bitstatus = RESET;
    }
    return  bitstatus;
}

ITStatus LCD_GetOFFITStatus(void)
{
    ITStatus bitstatus = RESET;

    if ((LCDC->IF.OFFIF != (uint32_t)RESET))
    {
        bitstatus = SET;
    }
    else
    {
        bitstatus = RESET;
    }
    return  bitstatus;
}

void LCD_GrayscaleConfig(LCD_TYPE_BVS LCD_Grayscale )
{
    LCDC->CON0.BVS  = LCD_Grayscale;
}

ErrorStatus LCD_FlickerTimeConfig(uint8_t On_Time,uint8_t Off_Time)
{

    if(On_Time>255 || Off_Time>255)
    {
        return ERROR;           
    }

    LCDC->FLKT.TON  = On_Time;

    LCDC->FLKT.TOFF   = Off_Time;
    return SUCCESS;
}

ErrorStatus  LCD_PixelWriteByte(LCD_TYPE_PIXEL LCD_DD, LCD_DD_BYTE nByte ,uint8_t LCD_data)
{
#ifdef HR8P287
    if(LCD_DD>11||nByte >3)
#else
    if(LCD_DD>13||nByte >3)
#endif
    {
        return ERROR;
    }
    switch (LCD_DD)
    {
        case LCD_Pixel_LCDD0:
            LCDC->D0.LCDD0=(LCDC->D0.LCDD0&(~(0X000000FF<<(nByte*8))))|(((uint32_t)LCD_data)<<(nByte*8));
            break;
        case LCD_Pixel_LCDD1:
            LCDC->D1.LCDD1=(LCDC->D1.LCDD1&(~(0X000000FF<<(nByte*8))))|(((uint32_t)LCD_data)<<(nByte*8));
            break;
        case LCD_Pixel_LCDD2:
            LCDC->D2.LCDD2=(LCDC->D2.LCDD2&(~(0X000000FF<<(nByte*8))))|(((uint32_t)LCD_data)<<(nByte*8));
            break;
        case LCD_Pixel_LCDD3:
            LCDC->D3.LCDD3=(LCDC->D3.LCDD3&(~(0X000000FF<<(nByte*8))))|(((uint32_t)LCD_data)<<(nByte*8));
            break;
        case LCD_Pixel_LCDD4:
            LCDC->D4.LCDD4=(LCDC->D4.LCDD4&(~(0X000000FF<<(nByte*8))))|(((uint32_t)LCD_data)<<(nByte*8));
            break;
        case LCD_Pixel_LCDD5:
            LCDC->D5.LCDD5=(LCDC->D5.LCDD5&(~(0X000000FF<<(nByte*8))))|(((uint32_t)LCD_data)<<(nByte*8));
            break;      
        case LCD_Pixel_LCDD6:
            LCDC->D6.LCDD6=(LCDC->D6.LCDD6&(~(0X000000FF<<(nByte*8))))|(((uint32_t)LCD_data)<<(nByte*8));
            break;
        case LCD_Pixel_LCDD7:
            LCDC->D7.LCDD7=(LCDC->D7.LCDD7&(~(0X000000FF<<(nByte*8))))|(((uint32_t)LCD_data)<<(nByte*8));
            break;
        case LCD_Pixel_LCDD8:
            LCDC->D8.LCDD8=(LCDC->D8.LCDD8&(~(0X000000FF<<(nByte*8))))|(((uint32_t)LCD_data)<<(nByte*8));
            break;
        case LCD_Pixel_LCDD9:
            LCDC->D9.LCDD9=(LCDC->D9.LCDD9&(~(0X000000FF<<(nByte*8))))|(((uint32_t)LCD_data)<<(nByte*8));
            break;
        case LCD_Pixel_LCDD10:
            LCDC->D10.LCDD10=(LCDC->D10.LCDD10&(~(0X000000FF<<(nByte*8))))|(((uint32_t)LCD_data)<<(nByte*8));
            break;
        case LCD_Pixel_LCDD11:
            LCDC->D11.LCDD11=(LCDC->D11.LCDD11&(~(0X000000FF<<(nByte*8))))|(((uint32_t)LCD_data)<<(nByte*8));
            break;
#ifdef HR8P296
        case LCD_Pixel_LCDD12:
            LCDC->D12.LCDD12=(LCDC->D12.LCDD12&(~(0X000000FF<<(nByte*8))))|(((uint32_t)LCD_data)<<(nByte*8));
            break;      
        case LCD_Pixel_LCDD13:
            LCDC->D13.LCDD13=(LCDC->D13.LCDD13&(~(0X000000FF<<(nByte*8))))|(((uint32_t)LCD_data)<<(nByte*8));
            break;
#endif 
        default:
            return  ERROR;          
    }
    return SUCCESS;
}

ErrorStatus  LCD_PixelWriteHalfWord(LCD_TYPE_PIXEL LCD_DD, LCD_DD_HALFWORD nHalfWord , uint16_t LCD_data)
{
#ifdef HR8P287
    if( LCD_DD>11 || nHalfWord >1)
#else
    if( LCD_DD>13 || nHalfWord >1)
#endif
    {
        return ERROR;
    }
    switch (LCD_DD)
    {
        case LCD_Pixel_LCDD0:
            LCDC->D0.LCDD0=(LCDC->D0.LCDD0&(~(0X0000FFFF<<(nHalfWord*16))))|(((uint32_t)LCD_data)<<(nHalfWord*16));
            break;
        case LCD_Pixel_LCDD1:
            LCDC->D1.LCDD1=(LCDC->D1.LCDD1&(~(0X0000FFFF<<(nHalfWord*16))))|(((uint32_t)LCD_data)<<(nHalfWord*16));
            break;
        case LCD_Pixel_LCDD2:
            LCDC->D2.LCDD2=(LCDC->D2.LCDD2&(~(0X0000FFFF<<(nHalfWord*16))))|(((uint32_t)LCD_data)<<(nHalfWord*16));
            break;
        case LCD_Pixel_LCDD3:
            LCDC->D3.LCDD3=(LCDC->D3.LCDD3&(~(0X0000FFFF<<(nHalfWord*16))))|(((uint32_t)LCD_data)<<(nHalfWord*16));
            break;
        case LCD_Pixel_LCDD4:
            LCDC->D4.LCDD4=(LCDC->D4.LCDD4&(~(0X0000FFFF<<(nHalfWord*16))))|(((uint32_t)LCD_data)<<(nHalfWord*16));
            break;
        case LCD_Pixel_LCDD5:
            LCDC->D5.LCDD5=(LCDC->D5.LCDD5&(~(0X0000FFFF<<(nHalfWord*16))))|(((uint32_t)LCD_data)<<(nHalfWord*16));
            break;      
        case LCD_Pixel_LCDD6:
            LCDC->D6.LCDD6=(LCDC->D6.LCDD6&(~(0X0000FFFF<<(nHalfWord*16))))|(((uint32_t)LCD_data)<<(nHalfWord*16));
            break;
        case LCD_Pixel_LCDD7:
            LCDC->D7.LCDD7=(LCDC->D7.LCDD7&(~(0X0000FFFF<<(nHalfWord*16))))|(((uint32_t)LCD_data)<<(nHalfWord*16));
            break;
        case LCD_Pixel_LCDD8:
            LCDC->D8.LCDD8=(LCDC->D8.LCDD8&(~(0X0000FFFF<<(nHalfWord*16))))|(((uint32_t)LCD_data)<<(nHalfWord*16));
            break;
        case LCD_Pixel_LCDD9:
            LCDC->D9.LCDD9=(LCDC->D9.LCDD9&(~(0X0000FFFF<<(nHalfWord*16))))|(((uint32_t)LCD_data)<<(nHalfWord*16));
            break;
        case LCD_Pixel_LCDD10:
            LCDC->D10.LCDD10=(LCDC->D10.LCDD10&(~(0X0000FFFF<<(nHalfWord*16))))|(((uint32_t)LCD_data)<<(nHalfWord*16));
            break;
        case LCD_Pixel_LCDD11:
            LCDC->D11.LCDD11=(LCDC->D11.LCDD11&(~(0X0000FFFF<<(nHalfWord*16))))|(((uint32_t)LCD_data)<<(nHalfWord*16));
            break;
#ifdef HR8P296
        case LCD_Pixel_LCDD12:
            LCDC->D12.LCDD12=(LCDC->D12.LCDD12&(~(0X0000FFFF<<(nHalfWord*16))))|(((uint32_t)LCD_data)<<(nHalfWord*16));
            break;
        case LCD_Pixel_LCDD13:
            LCDC->D13.LCDD13=(LCDC->D13.LCDD13&(~(0X0000FFFF<<(nHalfWord*16))))|(((uint32_t)LCD_data)<<(nHalfWord*16));
            break;
#endif           
        default: 
            return  ERROR;
    }
    return SUCCESS;
}

ErrorStatus  LCD_PixelWriteWord(LCD_TYPE_PIXEL LCD_DD,uint32_t LCD_data)
{
#ifdef HR8P287
    if( LCD_DD>11 )
#else
    if( LCD_DD>13 )
#endif
    {
        return ERROR;
    }
    switch (LCD_DD)
    {
        case LCD_Pixel_LCDD0:
            LCDC->D0.LCDD0=(uint32_t)LCD_data;
            break;
        case LCD_Pixel_LCDD1:
            LCDC->D1.LCDD1=(uint32_t)LCD_data;
            break;
        case LCD_Pixel_LCDD2:
            LCDC->D2.LCDD2=(uint32_t)LCD_data;
            break;
        case LCD_Pixel_LCDD3:
            LCDC->D3.LCDD3=(uint32_t)LCD_data;
            break;
        case LCD_Pixel_LCDD4:
            LCDC->D4.LCDD4=(uint32_t)LCD_data;
            break;
        case LCD_Pixel_LCDD5:
            LCDC->D5.LCDD5=(uint32_t)LCD_data;
            break;      
        case LCD_Pixel_LCDD6:
            LCDC->D6.LCDD6=(uint32_t)LCD_data;
            break;
        case LCD_Pixel_LCDD7:
            LCDC->D7.LCDD7=(uint32_t)LCD_data;
            break;
        case LCD_Pixel_LCDD8:
            LCDC->D8.LCDD8=(uint32_t)LCD_data;
            break;
        case LCD_Pixel_LCDD9:
            LCDC->D9.LCDD9=(uint32_t)LCD_data;
            break;
        case LCD_Pixel_LCDD10:
            LCDC->D10.LCDD10=(uint32_t)LCD_data;
            break;
        case LCD_Pixel_LCDD11:
            LCDC->D11.LCDD11=(uint32_t)LCD_data;
            break;
#ifdef HR8P296
        case LCD_Pixel_LCDD12:
            LCDC->D12.LCDD12=(uint32_t)LCD_data;
            break;
        case LCD_Pixel_LCDD13:
            LCDC->D13.LCDD13=(uint32_t)LCD_data;
            break;
#endif
        default:
            return  ERROR;
    }
    return SUCCESS;
}

/*************************END OF FILE**********************/
