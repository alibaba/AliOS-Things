/**
 *****************************************************************************
 * @file:			lcd_seg.h
 * @author			Ingrid Chen
 * @version			V1.0.0
 * @data			02-July-2013
 * @Brief			LCD Seg driver header file
 * @maintainer Sean
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
 * XXXX XXXX | XXXX X18	17	16	|	15	14	13	12	11	10	9	8	|	7	6	5	4	3	2	1	0  	//ch
 * XXXX XXXX | XXXX XC6	C5	C4	|	C3	C2	C1	C0	B31	B30	B29	B28	|	B27	B26	B25	B24 B23	B22	B21	B20 //gpio
 * XXXX XXXX | XXXX X1	1	1	|	1	1	1	1	1	1	1	1	|	1	1	1	1	1	1	1	1  	//bit
 * XXXX XXXX |		BYTE1		|				BYTE2				|				BYTE3  				//uint8_t
 *
 * LcdSegMask:	The lowest effective bit corresponding to the pin is COM[0],
 *				The highest effective bit corresponding to the pin is COM[N-1]
 * LcdComMask:	The lowest effective bit corresponding to the pin is SEG[0],
 *				The highest effective bit corresponding to the pin SEG[N-1]
 *
 * LcdBias:		0 -> 1/3 bias
 *				1 -> 1/4 bias
 *
 * LcdDispData: User must alloc (Com+Seg)*Com byte size memory space, and variable is equal to the first address poniter of this memory space
 */
#ifndef __LCD_SEG_H__
#define __LCD_SEG_H__

#ifdef __cplusplus
extern "C" {
#endif//__cplusplus

typedef struct _ST_LCD_SEG_CTRL
{
	uint32_t	LcdSegMask: 19;		/**<Specify the SEG port to use which Lcd analog Channel*/
	uint32_t	LcdComMask: 19;		/**<Specify the COM port to use which Lcd analog Channel*/
	uint32_t	LcdBias: 1; 			/**<Select bias voltage type*/
	uint32_t	: 0;
	uint8_t*   LcdDispData;		/**<pointer of Display data buffer*/
} ST_LCD_SEG_CTRL;

/**
 * @Brief	LCD Seg Initialize
 * @Param	lcdsegctrl refer to stuct ST_LCD_SEG_CTRL define
 * @Param       Voltage   LCD top voltage selection
 *     @arg  3'b111: VDD33 * 1
 *     @arg  3'b110: VDD33 * 32/33
 *     @arg  3'b101: VDD33 * 31/33
 *     @arg  3'b100: VDD33 * 30/33
 *     @arg  3'b011: VDD33 * 29/33
 *     @arg  3'b010: VDD33 * 28/33
 *     @arg  3'b001: VDD33 * 27/33
 *     @arg  3'b000: VDD33 * 26/33
 * @Return	None
 * @Note@ 	Config IO reuse, bias voltage & display buffer
 */
void LcdsegInit(ST_LCD_SEG_CTRL* lcdsegctrl,uint8_t Voltage);
/**
 * @Brief	LCD Seg DeInitialize
 * @Param	None
 * @Return	None
 * @Note@
 */
void LcdsegDeinit(void);

/**
 * @Brief	LCD Seg prepare display data
 * @Param	Buf
 * @Return	None
 * @Note@
 */
void LcdsegSetData(uint8_t* Buf);

/**
 * @Brief	LCD Seg refresh display data at interval time
 * @Param	None
 * @Return	None
 * @Note@
 */
void LcdsegSetRefMem(void);

/**
 * @Brief	LCD CTRL Reg[0]~Reg[18] Init
 * @Param	None
 * @Return	None
 * @Note@
 */
void LcdCtrlRegInit(void);

#ifdef  __cplusplus
}
#endif//__cplusplus

#endif
