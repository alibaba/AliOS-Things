/**
  ******************************************************************************
  * @file    hx8347g.h
  * @author  MCD Application Team
  * @brief   This file contains all the functions prototypes for the hx8347g.c
  *          driver.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2016 STMicroelectronics</center></h2>
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */ 

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __HX8347G_H
#define __HX8347G_H

#ifdef __cplusplus
 extern "C" {
#endif 

/* Includes ------------------------------------------------------------------*/
#include "../Common/lcd.h"

/** @addtogroup BSP
  * @{
  */ 

/** @addtogroup Components
  * @{
  */ 
  
/** @addtogroup hx8347g
  * @{
  */

/** @defgroup HX8347G_Exported_Types
  * @{
  */
   
/**
  * @}
  */ 

/** @defgroup HX8347G_Exported_Constants
  * @{
  */
/** 
  * @brief  HX8347G ID  
  */  
#define  HX8347G_ID    0x0075
   
/** 
  * @brief  HX8347G Size  
  */  
#define  HX8347G_LCD_PIXEL_WIDTH    ((uint16_t)320)
#define  HX8347G_LCD_PIXEL_HEIGHT   ((uint16_t)240)
   
/** 
  * @brief  HX8347G Registers  
  */ 
#define LCD_REG_0             0x00
#define LCD_REG_1             0x01
#define LCD_REG_2             0x02
#define LCD_REG_3             0x03
#define LCD_REG_4             0x04
#define LCD_REG_5             0x05
#define LCD_REG_6             0x06
#define LCD_REG_7             0x07
#define LCD_REG_8             0x08
#define LCD_REG_9             0x09
#define LCD_REG_10            0x0A
#define LCD_REG_11            0x0B
#define LCD_REG_12            0x0C
#define LCD_REG_13            0x0D
#define LCD_REG_14            0x0E
#define LCD_REG_15            0x0F
#define LCD_REG_16            0x10
#define LCD_REG_17            0x11
#define LCD_REG_18            0x12
#define LCD_REG_19            0x13
#define LCD_REG_20            0x14
#define LCD_REG_21            0x15
#define LCD_REG_22            0x16
#define LCD_REG_23            0x17
#define LCD_REG_24            0x18
#define LCD_REG_25            0x19
#define LCD_REG_26            0x1A
#define LCD_REG_27            0x1B
#define LCD_REG_28            0x1C
#define LCD_REG_29            0x1D
#define LCD_REG_30            0x1E
#define LCD_REG_31            0x1F
#define LCD_REG_32            0x20
#define LCD_REG_33            0x21
#define LCD_REG_34            0x22
#define LCD_REG_35            0x23
#define LCD_REG_36            0x24
#define LCD_REG_37            0x25
#define LCD_REG_38            0x26
#define LCD_REG_39            0x27
#define LCD_REG_40            0x28
#define LCD_REG_41            0x29
#define LCD_REG_42            0x2A
#define LCD_REG_43            0x2B
#define LCD_REG_44            0x2C
#define LCD_REG_45            0x2D
#define LCD_REG_46            0x2E
#define LCD_REG_47            0x2F
#define LCD_REG_48            0x30
#define LCD_REG_49            0x31
#define LCD_REG_50            0x32
#define LCD_REG_51            0x33
#define LCD_REG_52            0x34
#define LCD_REG_53            0x35
#define LCD_REG_54            0x36
#define LCD_REG_55            0x37
#define LCD_REG_56            0x38
#define LCD_REG_57            0x39
#define LCD_REG_58            0x3A
#define LCD_REG_59            0x3B
#define LCD_REG_60            0x3C
#define LCD_REG_61            0x3D
#define LCD_REG_62            0x3E
#define LCD_REG_63            0x3F
#define LCD_REG_64            0x40
#define LCD_REG_65            0x41
#define LCD_REG_66            0x42
#define LCD_REG_67            0x43
#define LCD_REG_68            0x44
#define LCD_REG_69            0x45
#define LCD_REG_70            0x46
#define LCD_REG_71            0x47
#define LCD_REG_72            0x48
#define LCD_REG_73            0x49
#define LCD_REG_74            0x4A
#define LCD_REG_75            0x4B
#define LCD_REG_76            0x4C
#define LCD_REG_77            0x4D
#define LCD_REG_78            0x4E
#define LCD_REG_79            0x4F
#define LCD_REG_80            0x50
#define LCD_REG_81            0x51
#define LCD_REG_82            0x52
#define LCD_REG_83            0x53
#define LCD_REG_84            0x54
#define LCD_REG_85            0x55
#define LCD_REG_86            0x56
#define LCD_REG_87            0x57
#define LCD_REG_88            0x58
#define LCD_REG_89            0x59
#define LCD_REG_90            0x5A
#define LCD_REG_91            0x5B
#define LCD_REG_92            0x5C
#define LCD_REG_93            0x5D
#define LCD_REG_94            0x5E
#define LCD_REG_95            0x5F
#define LCD_REG_96            0x60
#define LCD_REG_97            0x61
#define LCD_REG_98            0x62
#define LCD_REG_99            0x63
#define LCD_REG_104           0x68
#define LCD_REG_105           0x69
#define LCD_REG_112           0x70
#define LCD_REG_113           0x71
#define LCD_REG_132           0x84
#define LCD_REG_133           0x85
#define LCD_REG_195           0xC3
#define LCD_REG_197           0xC5
#define LCD_REG_199           0xC7
#define LCD_REG_203           0xCB
#define LCD_REG_204           0xCC
#define LCD_REG_205           0xCD
#define LCD_REG_206           0xCE
#define LCD_REG_207           0xCF
#define LCD_REG_208           0xD0
#define LCD_REG_209           0xD1
#define LCD_REG_210           0xD2
#define LCD_REG_211           0xD3
#define LCD_REG_232           0xE8
#define LCD_REG_233           0xE9
#define LCD_REG_234           0xEA
#define LCD_REG_235           0xEB
#define LCD_REG_236           0xEC
#define LCD_REG_237           0xED
#define LCD_REG_241           0xF1
#define LCD_REG_242           0xF2
#define LCD_REG_255           0xFF

/**
  * @}
  */
  
/** @defgroup HX8347G_Exported_Functions
  * @{
  */ 
void     hx8347g_Init(void);
uint16_t hx8347g_ReadID(void);
void     hx8347g_WriteReg(uint8_t LCDReg, uint16_t LCDRegValue);
uint16_t hx8347g_ReadReg(uint8_t LCDReg);

void     hx8347g_DisplayOn(void);
void     hx8347g_DisplayOff(void);
void     hx8347g_SetCursor(uint16_t Xpos, uint16_t Ypos);
void     hx8347g_WritePixel(uint16_t Xpos, uint16_t Ypos, uint16_t RGBCode);
uint16_t hx8347g_ReadPixel(uint16_t Xpos, uint16_t Ypos);

void     hx8347g_DrawHLine(uint16_t RGBCode, uint16_t Xpos, uint16_t Ypos, uint16_t Length);
void     hx8347g_DrawVLine(uint16_t RGBCode, uint16_t Xpos, uint16_t Ypos, uint16_t Length);
void     hx8347g_DrawBitmap(uint16_t Xpos, uint16_t Ypos, uint8_t *pbmp);

void     hx8347g_SetDisplayWindow(uint16_t Xpos, uint16_t Ypos, uint16_t Width, uint16_t Height);


uint16_t hx8347g_GetLcdPixelWidth(void);
uint16_t hx8347g_GetLcdPixelHeight(void);

/* LCD driver structure */
extern LCD_DrvTypeDef   hx8347g_drv;

/* LCD IO functions */
void     LCD_IO_Init(void);
void     LCD_IO_WriteMultipleData(uint8_t *pData, uint32_t Size);
void     LCD_IO_WriteReg(uint8_t Reg);
uint16_t LCD_IO_ReadData(uint16_t Reg);
void     LCD_Delay (uint32_t delay);
/**
  * @}
  */ 
      
#ifdef __cplusplus
}
#endif

#endif /* __HX8347G_H */

/**
  * @}
  */ 

/**
  * @}
  */ 

/**
  * @}
  */
  
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
