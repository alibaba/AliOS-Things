/**
 ******************************************************************************
 * @file    oled.h
 * @author  William Xu
 * @version V1.0.0
 * @date    9-Apr-2018
 * @brief   OLED driver VGM128064 functions header files
 ******************************************************************************
 *
 * Copyright (c) 2009-2018 MXCHIP Co.,Ltd.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 ******************************************************************************
 */

#ifndef __OLED_H_
#define __OLED_H_

#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

/** \addtogroup drivers */
/** @{*/

/** \addtogroup oled_Screen */
/** @{*/

/******************************************************************************
 *                                 Constants
 ******************************************************************************/

/* for 8*16 char can only display 4 rows, 16 chars each row. */
#define OLED_DISPLAY_ROW_1    0    /**< Line 1, yellow. */
#define OLED_DISPLAY_ROW_2    2    /**< Line 2, blue. */
#define OLED_DISPLAY_ROW_3    4    /**< Line 3, blue. */
#define OLED_DISPLAY_ROW_4    6    /**< Line 4, blue. */

#define OLED_DISPLAY_COLUMN_START    0    /**< column from left pos 0  */

#define OLED_DISPLAY_MAX_CHAR_PER_ROW    16   /**< max 16 chars each row */

/******************************************************************************
 *                             Function Declarations
 ******************************************************************************/

void OLED_WR_Byte(uint8_t dat, uint8_t cmd);   
void OLED_WR_Bytes(uint8_t *dat, uint8_t len, uint8_t cmd);   
void OLED_Display_On(void);
void OLED_Display_Off(void);  

void OLED_DrawPoint(uint8_t x,uint8_t y,uint8_t t);
void OLED_Fill(uint8_t x1,uint8_t y1,uint8_t x2,uint8_t y2,uint8_t dot);
void OLED_ShowChar(uint8_t x,uint8_t y,uint8_t chr);
void OLED_FillAll(void);
void OLED_ShowNum(uint8_t x,uint8_t y,uint32_t num,uint8_t len,uint8_t size);

void OLED_Set_Pos(unsigned char x, unsigned char y);
void OLED_ShowChinese(uint8_t x,uint8_t y,uint8_t no);
void OLED_DrawBMP(unsigned char x0, unsigned char y0,unsigned char x1, unsigned char y1,unsigned char BMP[]);



/**
 * @brief Initialize OLED device.
 *
 * @return none
 */
void OLED_Init(void);


/**
 * @brief Clear up all data shown on OLED
 *
 * @return none
 */
void OLED_Clear(void);


/**
 * @brief show string in OLED specified place
 *
 * @param x: Position the X axis of the string to display
 * @param y: Position the Y axis of the string to display
 * @param p: String to be displayed on OLED
 * 
 * @return none
 */
void OLED_ShowString(uint8_t x,uint8_t y, const char *p);

/** @}*/
/** @}*/

#ifdef __cplusplus
}
#endif

#endif  
	 



