/**
  *****************************************************************************
  * @file:			lcd.h
  * @author			Ingrid Chen
  * @version		V1.0.0
  * @data			06-June-2013
  * @maintainer Sean
  * @Brief			LCDC module driver header file.
  ******************************************************************************
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, MVSILICON SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
  * INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2013 MVSilicon </center></h2>
  */

/**
* @addtogroup Çý¶¯
* @{
* @defgroup LCD LCD
* @{
*/

#ifndef __LCD_H__
#define __LCD_H__

#ifdef __cplusplus
extern "C" {
#endif//__cplusplus

/**
 * LCDC Clock Divider
 */
#define LCD_CLK_DIV4_24M	1		/**< LCDC clk divider coefficient lcdc_clk = 24M @96M sysclk*/
#define LCD_CLK_DIV8_12M	2		/**< LCDC clk divider coefficient lcdc_clk = 12M @96M sysclk*/
#define LCD_CLK_DIV16_6M	3		/**< LCDC clk divider coefficient lcdc_clk = 6M @96M sysclk*/
#define LCD_CLK_DIV32_3M	4		/**< LCDC clk divider coefficient lcdc_clk = 3M @96M sysclk*/
#define LCD_CLK_DIV64_1M5	5		/**< LCDC clk divider coefficient lcdc_clk = 1.5M @96M sysclk*/
#define LCD_CLK_DIV128_0M75	6		/**< LCDC clk divider coefficient lcdc_clk = 0.75M @96M sysclk*/

/**
 * LCDC select interface
 */
#define LCD_SEL_8080		0		/**< LCDC interface protocol 8080*/
#define LCD_SEL_6800		1		/**< LCDC interface protocol 6800*/

/**
 * LCDC endian
 */
#define LCD_BIG_ENDIAN		0		/**< LCDC row data(32bits) byte endian in data buffer(16*32)  
[31:24] - byte3, [23:16] - byte2, [15:8] - byte1, [7:0] - byte0*/
#define LCD_LITTLE_ENDIAN	1		/**< LCDC row data(32bits) byte endian in data buffer(16*32)  
[31:24] - byte2, [23:16] - byte3, [15:8] - byte0, [7:0] - byte1*/

#define LCD_DIR_READ		0
#define LCD_DIR_WRITE		1


/**
 * LCDC 8080 CTRL BIT
 */
typedef enum _LCD_8080_CTRL
{
    LCD_8080_RS,
    LCD_8080_CS,
    LCD_8080_RD,
    LCD_8080_WR
} LCD_8080_CTRL;

/**
 * LCDC 6800 CTRL BIT
 */
typedef enum _LCD_6800_CTRL
{
    LCD_6800_RS,
    LCD_6800_CS,
    LCD_6800_EN,
    LCD_6800_RW
} LCD_6800_CTRL;

/**
 * @brief	Config LCD Controller parameter
 * @param	ClkDiv -> Clock Divider
 * @param	SelIf -> Select suupport 8080 interface	or 6800 interface
 * @param	Endian -> Two-byte-endian
 * @return	None
 * @note
 */
void LcdcConfig(uint8_t ClkDiv, uint8_t SelIf, uint8_t Endian);

/**
 * @brief	Set LCD Controller direction
 * @param	dir	0:LCD_DIR_READ
 *				1:LCD_DIR_WRITE
 * @return	None
 * @note 	If bypass user can directly control 4 cmd pin
 */
void LcdcSetDir(uint8_t dir);

/**
 * @brief	Direct control command bit high
 * @param	IoSel ->	3	2	1	0
				6800	RW	EN	CS	RS
				8080	WR	RD	CS	RS
 * @return	None
 * @note
 */
void LcdcSetCtrlIoBit(uint8_t IoSel);

/**
 * @brief	Direct control command bit low
 * @param	IoSel ->	3	2	1	0
				6800	RW	EN	CS	RS
				8080	WR	RD	CS	RS
 * @return	None
 * @note
 */
void LcdcClrCtrlIoBit(uint8_t IoSel);

/**
 * @brief	Write byte data to LCD driver data bus
 * @param	Val
 * @return	None
 * @note
 */
void LcdcWriteByte(uint8_t Val);

/**
 * @brief	Read byte data from LCD driver data bus
 * @param	None
 * @return	Val
 * @note
 */
uint8_t LcdcReadByte(void);

/**
 * @brief	Write multi byte data to LCD driver by controller
 * @param	SrcAddr
 * @param	XferSize 1~262143 uint8_t
 * @return	None
 * @note
 */
void LcdcWriteData(uint32_t SrcAddr, uint32_t XferSize);

/**
 * @brief	Read multi byte data from LCD driver by controller
 * @param	DstAddr
 * @param	XferSize 1~16 uint32_t
 * @return	None
 * @note
 */
void LcdcReadData(uint32_t DstAddr, uint32_t XferSize);


#ifdef  __cplusplus
}
#endif//__cplusplus

#endif

/**
 * @}
 * @}
 */
