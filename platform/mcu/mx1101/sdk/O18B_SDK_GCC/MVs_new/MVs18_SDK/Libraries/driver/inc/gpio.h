/******************************************************************************
 * @file    gpio.h
 * @author  YunWang
 * @version V1.0.0
 * @date    4-May-2014
 * @brief   gpio register description and function declare
 * @TypicalAppication
 *   Gpio can be configured as general io pin also be reused for other pin.
 *   They are configured as general io pin using the following functions:
 *   1.Digital output:  
		--Disable input function: Clr GPIO_A_IE, GPIO_B_IE or GPIO_C_IE
		--Enable Output function: Set GPIO_A_OE, GPIO_B_OE or GPIO_C_OE
		--Strong or week output:  Set or clr GPIO_A_OUTDS, GPIO_B_OUTDS or GPIO_C_OUTDS
		--Pull up or down function: Set or clr {PU,PD} 
		--strong or week pull: Set or clr GPIO_A_DS, GPIO_B_DS or GPIO_C_DS
		--Output High level or Low level: Set or clr GPIO_A_OUT, GPIO_B_OUT or GPIO_C_OUT
		
	 2.Digital input:
		--Disable output function: Clr GPIO_A_OE, GPIO_B_OE or GPIO_C_OE
		--Enable input function: Set GPIO_A_IE, GPIO_B_IE or GPIO_C_IE
		--Pull up or down function: Set or clr {PU,PD} 
		--strong or week pull: Set or clr GPIO_A_DS, GPIO_B_DS or GPIO_C_DS
		--Read input status
	 
	 3.Analog input:
		--Clr PU: Clr GPIO_A_PU, GPIO_B_PU or GPIO_C_PU
		--Set PD: Set GPIO_A_PD, GPIO_B_PD or GPIO_C_PD
		--Clr OE: Clr GPIO_A_OE, GPIO_B_OE or GPIO_C_OE
		--Clr IE: Clr GPIO_A_IE, GPIO_B_IE or GPIO_C_IE
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; COPYRIGHT 2013 MVSilicon </center></h2>
 * 
 */
#ifndef __GPIO_H__
#define	__GPIO_H__

#ifdef __cplusplus
extern "C" {
#endif//__cplusplus

#include "type.h"


// Bank A
#define		GPIO_A_IN							(0x00)	/**<GPIOA input data register macro*/
#define		GPIO_A_OUT							(0x01)	/**<GPIOA output data register macro*/
#define		GPIO_A_IE							(0x02)	/**<GPIOA input enable register macro.0: disable, 1: enable*/
#define		GPIO_A_OE							(0x03)	/**<GPIOA output enable register macro.0: disable, 1: enable*/
#define		GPIO_A_DS							(0x04)	/**<GPIOA pull-up/pull-down drive strength(when GPIOA input or output) register macro. 0: weak pull(20uA), 1: strong pull(70uA)*/
#define		GPIO_A_OUTDS						(0x05)	/**<GPIOA digital ouput driving strength select register macro.OUTDS=0 : 8mA,OUTDS=1 : 24mA*/
#define		GPIO_A_PU							(0x06)	/**<GPIOA pull up register macro*/
#define		GPIO_A_PD							(0x07)	/**<GPIOA pull down register macro.{pu,pd} 2'b00: pull-up, 2'b01: AIN, 2'b10: no pull-up, no pull-down, 2'b11: pull-down*/
#define		GPIO_A_PULLDOWN1					(0x08)	/**<GPIOA pull down current register macro.0: 0mA pull-down current, 1: 1.7mA pull-down current*/
#define		GPIO_A_PULLDOWN2					(0x09)	/**<GPIOA pull down current register macro.0: 0mA pull-down current, 1: 2.4mA pull-down current*/

#define     GPIOA0          					((uint32_t)1 << 0)	/**<GPIOA bit0 macro*/
#define     GPIOA1          					((uint32_t)1 << 1)	/**<GPIOA bit1 macro*/
#define     GPIOA2          					((uint32_t)1 << 2)	/**<GPIOA bit2 macro*/
#define     GPIOA3          					((uint32_t)1 << 3)	/**<GPIOA bit3 macro*/
#define     GPIOA4          					((uint32_t)1 << 4)	/**<GPIOA bit4 macro*/
#define     GPIOA5         	 					((uint32_t)1 << 5)	/**<GPIOA bit5 macro*/
#define     GPIOA6          					((uint32_t)1 << 6)	/**<GPIOA bit6 macro*/
#define     GPIOA7          					((uint32_t)1 << 7)	/**<GPIOA bit7 macro*/
#define     GPIOA8          					((uint32_t)1 << 8)	/**<GPIOA bit8 macro*/
#define     GPIOA9          					((uint32_t)1 << 9)	/**<GPIOA bit9 macro*/
#define     GPIOA10         					((uint32_t)1 << 10)	/**<GPIOA bit10 macro*/
#define     GPIOA11         					((uint32_t)1 << 11)	/**<GPIOA bit11 macro*/
#define     GPIOA12         					((uint32_t)1 << 12)	/**<GPIOA bit12 macro*/
#define     GPIOA13         					((uint32_t)1 << 13)	/**<GPIOA bit13 macro*/
#define     GPIOA14         					((uint32_t)1 << 14)	/**<GPIOA bit14 macro*/
#define     GPIOA15         					((uint32_t)1 << 15)	/**<GPIOA bit15 macro*/
#define     GPIOA16         					((uint32_t)1 << 16)	/**<GPIOA bit16 macro*/
#define     GPIOA17         					((uint32_t)1 << 17)	/**<GPIOA bit17 macro*/
#define     GPIOA18         					((uint32_t)1 << 18)	/**<GPIOA bit18 macro*/
#define     GPIOA19         					((uint32_t)1 << 19)	/**<GPIOA bit19 macro*/
#define     GPIOA20         					((uint32_t)1 << 20)	/**<GPIOA bit20 macro*/
#define     GPIOA21         					((uint32_t)1 << 21)	/**<GPIOA bit21 macro*/
#define     GPIOA22         					((uint32_t)1 << 22)	/**<GPIOA bit22 macro*/
#define     GPIOA23         					((uint32_t)1 << 23)	/**<GPIOA bit23 macro*/
#define     GPIOA24         					((uint32_t)1 << 24)	/**<GPIOA bit24 macro*/
#define     GPIOA25         					((uint32_t)1 << 25)	/**<GPIOA bit25 macro*/
#define     GPIOA26         					((uint32_t)1 << 26)	/**<GPIOA bit26 macro*/
#define     GPIOA27         					((uint32_t)1 << 27)	/**<GPIOA bit27 macro*/
#define     GPIOA28         					((uint32_t)1 << 28)	/**<GPIOA bit28 macro*/
#define     GPIOA29        						((uint32_t)1 << 29)	/**<GPIOA bit29 macro*/
#define     GPIOA30        						((uint32_t)1 << 30)	/**<GPIOA bit30 macro*/
#define     GPIOA31         					((uint32_t)1 << 31)	/**<GPIOA bit31 macro*/

// Bank B
#define		GPIO_B_IN							(0x0A)	/**<GPIOB input data register macro*/
#define		GPIO_B_OUT							(0x0B)	/**<GPIOB output data register macro*/
#define		GPIO_B_IE	 						(0x0C)	/**<GPIOB input enable register macro.0: disable, 1: enable*/
#define		GPIO_B_OE							(0x0D)	/**<GPIOB output enable register macro.0: disable, 1: enable*/
#define		GPIO_B_DS							(0x0E)	/**<GPIOB pull-up/pull-down drive strength(when GPIOB input or output) register macro. 0: weak pull(20uA), 1: strong pull(70uA)*/
#define		GPIO_B_OUTDS						(0x0F)	/**<GPIOB digital ouput driving strength select register macro.OUTDS=0 : 8mA,OUTDS=1 : 24mA*/
#define		GPIO_B_PU							(0x10)	/**<GPIOB pull up register macro*/
#define		GPIO_B_PD							(0x11)	/**<GPIOB pull down register macro.{pu,pd} 2'b00: pull-up, 2'b01: AIN, 2'b10: no pull-up, no pull-down, 2'b11: pull-down*/
#define		GPIO_B_PULLDOWN1					(0x12)	/**<GPIOB pull down current register macro.0: 0mA pull-down current, 1: 1.7mA pull-down current*/
#define		GPIO_B_PULLDOWN2					(0x13)	/**<GPIOB pull down current register macro.0: 0mA pull-down current, 1: 2.4mA pull-down current*/

#define     GPIOB0          					((uint32_t)1 << 0)	/**<GPIOB bit0 macro*/
#define     GPIOB1          					((uint32_t)1 << 1)	/**<GPIOB bit1 macro*/
#define     GPIOB2          					((uint32_t)1 << 2)	/**<GPIOB bit2 macro*/
#define     GPIOB3          					((uint32_t)1 << 3)	/**<GPIOB bit3 macro*/
#define     GPIOB4          					((uint32_t)1 << 4)	/**<GPIOB bit4 macro*/
#define     GPIOB5         	 					((uint32_t)1 << 5)	/**<GPIOB bit5 macro*/
#define     GPIOB6          					((uint32_t)1 << 6)	/**<GPIOB bit6 macro*/
#define     GPIOB7          					((uint32_t)1 << 7)	/**<GPIOB bit7 macro*/
#define     GPIOB8          					((uint32_t)1 << 8)	/**<GPIOB bit8 macro*/
#define     GPIOB9          					((uint32_t)1 << 9)	/**<GPIOB bit9 macro*/
#define     GPIOB10         					((uint32_t)1 << 10)	/**<GPIOB bit10 macro*/
#define     GPIOB11         					((uint32_t)1 << 11)	/**<GPIOB bit11 macro*/
#define     GPIOB12         					((uint32_t)1 << 12)	/**<GPIOB bit12 macro*/
#define     GPIOB13         					((uint32_t)1 << 13)	/**<GPIOB bit13 macro*/
#define     GPIOB14         					((uint32_t)1 << 14)	/**<GPIOB bit14 macro*/
#define     GPIOB15         					((uint32_t)1 << 15)	/**<GPIOB bit15 macro*/
#define     GPIOB16         					((uint32_t)1 << 16)	/**<GPIOB bit16 macro*/
#define     GPIOB17         					((uint32_t)1 << 17)	/**<GPIOB bit17 macro*/
#define     GPIOB18         					((uint32_t)1 << 18)	/**<GPIOB bit18 macro*/
#define     GPIOB19         					((uint32_t)1 << 19)	/**<GPIOB bit19 macro*/
#define     GPIOB20         					((uint32_t)1 << 20)	/**<GPIOB bit20 macro*/
#define     GPIOB21         					((uint32_t)1 << 21)	/**<GPIOB bit21 macro*/
#define     GPIOB22         					((uint32_t)1 << 22)	/**<GPIOB bit22 macro*/
#define     GPIOB23         					((uint32_t)1 << 23)	/**<GPIOB bit23 macro*/
#define     GPIOB24         					((uint32_t)1 << 24)	/**<GPIOB bit24 macro*/
#define     GPIOB25         					((uint32_t)1 << 25)	/**<GPIOB bit25 macro*/
#define     GPIOB26         					((uint32_t)1 << 26)	/**<GPIOB bit26 macro*/
#define     GPIOB27         					((uint32_t)1 << 27)	/**<GPIOB bit27 macro*/
#define     GPIOB28         					((uint32_t)1 << 28)	/**<GPIOB bit28 macro*/
#define     GPIOB29        						((uint32_t)1 << 29)	/**<GPIOB bit29 macro*/
#define     GPIOB30        						((uint32_t)1 << 30)	/**<GPIOB bit30 macro*/
#define     GPIOB31         					((uint32_t)1 << 31)	/**<GPIOB bit31 macro*/

// Bank C
#define		GPIO_C_IN							(0x14)	/**<GPIOC input data register macro*/
#define		GPIO_C_OUT							(0x15)	/**<GPIOC output data register macro*/
#define		GPIO_C_IE							(0x16)	/**<GPIOC input enable register macro.0: disable, 1: enable*/
#define		GPIO_C_OE							(0x17)	/**<GPIOC output enable register macro.0: disable, 1: enable*/
#define		GPIO_C_DS							(0x18)	/**<GPIOC pull-up/pull-down drive strength(when GPIOC input or output) register macro. 0: weak pull(20uA), 1: strong pull(70uA)*/
#define		GPIO_C_OUTDS						(0x19)	/**<GPIOC digital ouput driving strength select register macro.OUTDS=0 : 8mA,OUTDS=1 : 24mA*/
#define		GPIO_C_PU							(0x1A)	/**<GPIOC pull up register macro*/
#define		GPIO_C_PD							(0x1B)	/**<GPIOC pull down register macro.{pu,pd} 2'b00: pull-up, 2'b01: AIN, 2'b10: no pull-up, no pull-down, 2'b11: pull-down*/
#define		GPIO_C_PULLDOWN1					(0x1C)	/**<GPIOC pull down current register macro.0: 0mA pull-down current, 1: 1.7mA pull-down current*/
#define		GPIO_C_PULLDOWN2					(0x1D)	/**<GPIOC pull down current register macro.0: 0mA pull-down current, 1: 2.4mA pull-down current*/

#define     GPIOC0          					((uint32_t)1 << 0)	/**<GPIOC bit0 macro*/
#define     GPIOC1          					((uint32_t)1 << 1)	/**<GPIOC bit1 macro*/
#define     GPIOC2          					((uint32_t)1 << 2)	/**<GPIOC bit2 macro*/
#define     GPIOC3          					((uint32_t)1 << 3)	/**<GPIOC bit3 macro*/
#define     GPIOC4          					((uint32_t)1 << 4)	/**<GPIOC bit4 macro*/
#define     GPIOC5         	 					((uint32_t)1 << 5)	/**<GPIOC bit5 macro*/
#define     GPIOC6          					((uint32_t)1 << 6)	/**<GPIOC bit6 macro*/
#define     GPIOC7          					((uint32_t)1 << 7)	/**<GPIOC bit7 macro*/
#define     GPIOC8          					((uint32_t)1 << 8)	/**<GPIOC bit8 macro*/
#define     GPIOC9          					((uint32_t)1 << 9)	/**<GPIOC bit9 macro*/
#define     GPIOC10         					((uint32_t)1 << 10)	/**<GPIOC bit10 macro*/
#define     GPIOC11         					((uint32_t)1 << 11)	/**<GPIOC bit11 macro*/
#define     GPIOC12         					((uint32_t)1 << 12)	/**<GPIOC bit12 macro*/
#define     GPIOC13         					((uint32_t)1 << 13)	/**<GPIOC bit13 macro*/
#define     GPIOC14         					((uint32_t)1 << 14)	/**<GPIOC bit14 macro*/


//INT
#define		GPIO_A_INT							(0x1F)	/**<GPIOA interrupt inte register macro, using it with functions(GpioIntEn, GpioIntDis, GpioIntClr, GpioIntFlagGet)as the first para*/
#define		GPIO_B_INT							(0x23)	/**<GPIOA interrupt inte register macro, using it with functions(GpioIntEn, GpioIntDis, GpioIntClr, GpioIntFlagGet)as the first para*/
#define		GPIO_C_INT							(0x27)	/**<GPIOA interrupt inte register macro, using it with functions(GpioIntEn, GpioIntDis, GpioIntClr, GpioIntFlagGet)as the first para*/

#define     GPIO_NEG_EDGE_TRIGGER				(0)	/**<GPIO interrupt mode negedge register macro, you can use it with function GpioIntEn as the third para*/
#define     GPIO_POS_EDGE_TRIGGER				(1)	/**<GPIO interrupt mode posedge register macro, you can use it with function GpioIntEn as the third para*/



#define     RESTORE_TO_GENERAL_IO     			   			(0xFF)  /**<this mcaro will be used as a para value to restore the multiplex io to general io*/


/**
 *  define A0 pull down current
 */
typedef enum _ICS_VAL
{
    ICS_0MA0 = 0,	/**<pulldown current:0.0mA*/
    ICS_1MA7,		/**<pulldown current:1.7mA*/
    ICS_2MA4,		/**<pulldown current:2.4mA*/
    ICS_4MA1		/**<pulldown current:4.1mA*/
} ICS_VAL;

/**
 *  define A0 work as funtion
 */
typedef enum _A0_MODE_AF
{
    A0_FOR_GPIO = 0,	/**<normal gpio*/
    A0_FOR_ICS,			/**<pulldown current*/
    A0_FOR_CHARGER,		/**<led charger*/
    A0_FOR_PWM			/**<pwm*/

} A0_MODE_AF;


/**
 * @brief  config the pins used for Spi Master,or restore them to general IO
 * @param  ModeSel: config Spi Master pins multiplex relation,valid para 0,1,2,3,0xFF
 *			 0: spim0
 *			    spim_mosi a[2]
 *			    spim_clk  a[3]
 *			    spim_miso a[4]
 *			 1: spim1
 *			    spim_mosi a[21]
 *			    spim_clk  a[20]
 *			    spim_miso a[19]
 *			 2: spim2
 *			    spim_mosi b[5]
 *			    spim_clk  b[4]
 *			    spim_miso b[3]
 *			 3: spim3
 *			    spim_mosi b[20]
 *			    spim_clk  b[21]
 *			    spim_miso b[22]
 *           0xFF: restore the pins used for Spi Master to general IO
 * @return None
 */
void GpioSpimIoConfig(uint8_t ModeSel);

/**
 * @brief  config the pins used for Fuart Rx,or restore them to general IO
 * @param  ModeSel: config Fuart Rx pins multiplex relation,valid para 0,1,2,0xFF
 *			0: a[1]
 *			1: b[6]
 *			2: c[4]
 *          0xFF: restore the pins used for Fuart Rx to general IO
 * @return None
 */
void GpioFuartRxIoConfig(uint8_t ModeSel);

/**
 * @brief  config the pins used for Fuart Tx,or restore them to general IO
 * @param  ModeSel: config Fuart Tx pins multiplex relation,valid para 0,1,2,0xFF
 *			0: a[0]
 *			1: b[7]
 *			2: c[3]
 *          0xFF: restore the pins used for Fuart Tx to general IO
 * @return None
 */
void GpioFuartTxIoConfig(uint8_t ModeSel);

/**
 * @brief  config the pins used for Buart Rx,or restore them to general IO
 * @param  ModeSel: config Buart Rx pins multiplex relation,valid para 0,1,2,3,0xFF
 *			0: a[13]
 *			1: a[24]
 *			2: b[8]
 *			3: b[29]
 *          0xFF: restore the pins used for Buart Rx to general IO
 * @return None
 */
void GpioBuartRxIoConfig(uint8_t ModeSel);

/**
 * @brief  config the pins used for Buart Tx,or restore them to general IO
 * @param  ModeSel: config Buart Tx pins multiplex relation,valid para 0,1,2,3,0xFF
 *			0: a[16]
 *			1: a[25]
 *			2: b[9]
 *			3: b[28]
 *          0xFF: restore the pins used for Buart Tx to general IO
 * @return None
 */
void GpioBuartTxIoConfig(uint8_t ModeSel);


/**
 * @brief  config the pins used for Buart Cts,or restore them to general IO
 * @param  ModeSel: config Buart Cts pins multiplex relation,valid para 0,1,2,0xFF
 *			0: a[11]
 *			1: b[30]
 *			2: c[0]
 *          0xFF: restore the pins used for buart cts to general IO
 * @return None
 */
void GpioBuartCtsIoConfig(uint8_t ModeSel);

/**
 * @brief  config the pins used for Buart Rts,or restore them to general IO
 * @param  ModeSel: config Buart Rts pins multiplex relation,valid para 0,1,2,0xFF
 *		0: a[12]
 *		1: b[31]
 *		2: c[1]
 *      0xFF: restore the pins used for Buart Rts to general IO
 * @return None
 */
void GpioBuartRtsIoConfig(uint8_t ModeSel);

/**
 * @brief  config the pins used for sd,or restore them to general IO
 * @param  ModeSel: config IR pins multiplex relation,valid para 0,1,2,3,0xFF
 *		0: sd0
 *		    sd_cmd(io)     a[2]
 *		    sd_clk( o)     a[3]
 *		    sd_dat(io)     a[4]
 *		 1: sd1
 *		    sd_cmd(io)     a[21]
 *		    sd_clk( o)     a[20]
 *		    sd_dat(io)     a[19]
 *		 2: sd2
 *		    sd_cmd(io)     b[5]
 *		    sd_clk( o)     b[4]
 *		    sd_dat(io)     b[3]
 *		 3: sd3
 *		    sd_cmd(io)     b[20]
 *		    sd_clk( o)     b[21]
 *		    sd_dat(io)     b[22]
 *      0xFF: restore the pins used for Sd to general IO
 * @return None
 */
void GpioSdIoConfig(uint8_t ModeSel);

/**
 * @brief  config the pins used for Lcd,or restore them to general IO
 * @param  ModeSel: config Lcd pins multiplex relation,valid para 0,0xFF
 *         0:  	 config the pins used for Lcd
 *         0xFF: restore the pins used for Lcd to general IO
 * @return None
 */
void GpioLcdIoConfig(uint8_t ModeSel);

/**
 * @brief  config the pins used for Pwc,or restore them to general IO
 * @param  ModeSel: config Pwc pins multiplex relation,valid para 0,1,0xFF
 *		0: a[10]
 *		1: b[7]
 *      0xFF: restore the pins used for Pwc to general IO
 * @return None
 */
void GpioPwcIoConfig(uint8_t ModeSel);

/**
 * @brief  config the pins used for Ir,or restore them to general IO
 * @param  ModeSel: config IR pins multiplex relation,valid para 0,1,2,0xFF
 *		 0: a[10]
 *		 1: b[7]
 *		 2: c[2]
 *       0xFF: restore the pins used for Ir to general IO
 * @return NOne
 */
void GpioIrIoConfig(uint8_t ModeSel);

/**
 * @brief  config the pins used for flash,or restore them to general IO
 * @param  ModeSel: config Pcm pins multiplex relation,valid para 0,1,0xFF
 *		 0:    4-bits mode
 *		       fsh_hold(io)    a[13]
 *		       fsh_sck(o)      a[14]
 *		       fsh_si(io)      a[15]
 *		       fsh_wp(io)      a[16]
 *		       fsh_so(io)      a[17]
 *		       fsh_cs(o)       a[18]
 *		 1:    1-bit mode
 *		       fsh_sck(o)      a[14]
 *		       fsh_si(io)      a[15]
 *		       fsh_so(io)      a[17]
 *		       fsh_cs(o)       a[18]
 *       0xFF: restore the pins used for flash to general IO
 * @return None
 */
void GpioFshcIoConfig(uint8_t ModeSel);

/**
 * @brief  config the pins used for Sdram
 * @param  ModeSel: config Sdram pins multiplex relation,valid para 0,0xFF
 *         0:  	 config the pins used for Sdram
 *         0xFF: restore the pins used for Sdram to general IO
 * @return None
 */
void GpioSdramIoConfig(uint8_t ModeSel);

/**
 * @brief  config the pins used for Clk32 output,or restore them to general IO
 * @param  ModeSel: config Clk32 output pins multiplex relation,valid para 0,1,2,0xFF
 *		0: clk_32k_out    a[11]
 *		1: clk_32k_out    b[30]
 *		2: clk_32k_out    c[0]
 *      0xFF: restore the pins used for Clk32 output to general IO
 * @return None
 */
void GpioClk32kIoConfig(uint8_t ModeSel);

/**
 * @brief  config the pins used for Clk12M6M output,or restore them to general IO
 * @param  ModeSel: config Clk12M6M output pins multiplex relation,valid para 0,1,2,0xFF
 *		0: clk_12m_16m_out    a[12]
 *		1: clk_12m_16m_out    b[31]
 *		2: clk_12m_16m_out    c[1]
 *     0xFF:restore the pins used for Clk12M6M output to general IO
 * @return None
 */
void GpioClk12m6mIoConfig(uint8_t ModeSel);

/**
 * @brief  config the pins used for Pcm Sync,or restore them to general IO
 * @param  ModeSel: config Pcm Sync pins multiplex relation,valid para 0,1,2,0xFF
 *		 0: pcm0
 *		    pcm_sync(io)   b[3]
 *		    pcm_clk(io)    b[4]
 *		    pcm_din(i)     b[6]
 *		    pcm_do(o)      b[5]
 *		 1: pcm1
 *		    pcm_sync(io)   b[24]
 *		    pcm_clk(io)    b[25]
 *		    pcm_din(i)     b[27]
 *		    pcm_do(o)      b[26]
 *		 2: pcm2
 *		    pcm_sync(io)   c[9]
 *		    pcm_clk(io)    c[10]
 *		    pcm_din(i)     c[12]
 *		    pcm_do(o)      c[11]
 *     0xFF:restore the pins used for Pcm Sync to general IO
 * @return None
 */
void GpioPcmSyncIoConfig(uint8_t ModeSel);


/**
 * @brief  config the pins used for I2S,or restore them to general IO
 * @param  ModeSel: config I2S pins multiplex relation,valid para 0,1,2,0xFF
 *		 0:
 *		    i2s_lrck(io)   b[3]
 *		    i2s_bclk(io)   b[4]
 *		    i2s_din(i)     b[6]
 *		    i2s_do(o)      b[5]
 *		 1:
 *		    i2s_lrck(io)   b[24]
 *		    i2s_bclk(io)   b[25]
 *		    i2s_din(i)     b[27]
 *		    i2s_do(o)      b[26]
 *		 2:
 *		    i2s_lrck(io)   c[9]
 *		    i2s_bclk(io)   c[10]
 *		    i2s_din(i)     c[12]
 *		    i2s_do(o)      c[11]
 *     0xFF:restore the pins used for Pcm Sync to general IO
 * @return None
 */
void GpioI2sIoConfig(uint8_t ModeSel);

/**
 * @brief  config the pins used for Mclk,or restore them to general IO
 * @param  ModeSel: config pins used for Mclk or restore the pins used for Mclk to general IO.valid para 0,1,2,3,0xFF
 *		0: i2s_mclk(o)    b[2]
 *		1: i2s_mclk(o)    c[8]
 *		2: i2s_mclk(i)    b[2]
 *		3: i2s_mclk(i)    c[8]
 *      0xFF:restore the pins used for Mclk to general IO
 * @return None
 */
void GpioMclkIoConfig(uint8_t ModeSel);

/**
 * @brief  enable or disable the sw function
 * @param  ModeSel:
 *      0 and other value except 0xFF:	enable sw funciton,sw will use GPIOC[12] GPIOC[13] GPIOC[14]
 *      0xFF:restore the pins used for sw to general IO
 * @return None
 */
void GpioSwIoConfig(uint8_t ModeSel);

 /**
 * @brief  enable or disable the swo(SW viewer) function
 * @param  ModeSel:
 *      0 and other value except 0xFF:	enable sw viewer funciton,sw will use GPIOC[12]
 *      0xFF:restore the pins used for swo to general IO
 * @return None
 */
void GpioSwoIoConfig(uint8_t ModeSel);

/**
 * @brief  config the pins used for Spi Slave
 * @param  ModeSel: config Spi Slave pins multiplex relation,valid para 0,0xFF
 *         0:  	 config the pins used for Spi Slave
 *         0xFF: restore the pins used for Spi Slave to general IO
 * @return None
 */
void GpioSpisIoConfig(uint8_t ModeSel);

/**
 * @brief  enable or disable CTS(capacitor touch sensor) compare signal output,CTS_CMPOUT uses GPIOC[0]
 * @param  ModeSel: config CTS(capacitor touch sensor) compare signal output,valid para 0,0xFF
 *         0:  	 enable
 *         0xFF: disable
 * @return None
 */
void GpioCtsCmpoutIoConfig(uint8_t ModeSel);

/**
 * @brief  for both sdio and spi, vmem can always be accessed simultaneously
 * @param  sel: sdio or spi can occupies 12MHz xmem/pmem channel and another cannot
 *		   0: sdio occupies the 12MHz xmem/pmem channel, spi can't access xmem/pmem
 *         1: spi occupies the 12MHz xmem/pmem channel, sdio can't access xmem/pmem
 * @return None
 */
void GpioSdSpiSel(uint8_t Sel);

/**
 * @brief  Set gpio register value
 * @param  RegIndex: the gpio register macro
 * @param  Val: gpio register value which will be setted
 * @return None
 */
void GpioSetReg(uint8_t RegIndex, uint32_t Val);

/**
 * @brief  Get gpio register value
 * @param  RegIndex: the gpio register macro
 * @return gpio register value
 */
uint32_t GpioGetReg(uint8_t RegIndex);

/**
 * @brief  Set gpio register one bit to 1
 * @param  RegIndex: the gpio register macro
 * @param  GpioIndex: the bit which will be setted 1.valid value GPIOA0~GPIOA31,GPIOB0~GPIOB31,GPIOC0~GPIOC14
 * @return None
 */
void GpioSetRegOneBit(uint8_t	RegIndex, uint32_t GpioIndex);

/**
 * @brief  Clear GPIO register one bit to 0
 * @param  RegIndex: the gpio register macro
 * @param  GpioIndex: the bit which will be setted 0.valid value GPIOA0~GPIOA31,GPIOB0~GPIOB31,GPIOC0~GPIOC14
 * @return None
 */
void GpioClrRegOneBit(uint8_t	RegIndex, uint32_t GpioIndex);

/**
 * @brief  Set gpio register bits to 1
 * @param  RegIndex: the gpio register macro
 * @param  mask: the bits which will be setted 0.valid value GPIOA0~GPIOA31,GPIOB0~GPIOB31,GPIOC0~GPIOC14
 * @Note@  this functions will disable interrupt for a very short time,then enable interrupt
 * @return None
 */
void GpioSetRegBits(uint8_t RegIndex, uint32_t mask);

/**
 * @brief  Clear gpio register bits to 0
 * @param  RegIndex: the gpio register macro
 * @param  mask: the bits which will be setted 0.valid value GPIOA0~GPIOA31,GPIOB0~GPIOB31,GPIOC0~GPIOC14
 * @Note@  this functions will disable interrupt for a very short time,then enable interrupt
 * @return None
 */
void GpioClrRegBits(uint8_t RegIndex, uint32_t mask);

/**
 * @brief  Enable one gpio pin interrupt
 * @param  RegIndex: the gpio interrupt register macro.valid value GPIO_A_INT,GPIO_B_INT,GPIO_C_INT
 * @param  GpioIndex: the pin which will be setted interrupt enable.valid value GPIOA0~GPIOA31,GPIOB0~GPIOB31,GPIOC0~GPIOC14
 * @param  EdgeSel: set the interrupt mode,negdege trigger or posdege trigger. valid value GPIO_NEG_EDGE_TRIGGER,GPIO_POS_EDGE_TRIGGER
 * @return None
 */
void GpioIntEn(uint8_t RegIndex, uint32_t GpioIndex, uint8_t EdgeSel);

/**
 * @brief  Disable one gpio pin interrupt
 * @param  RegIndex: the gpio interrupt register macro.valid value GPIO_A_INT,GPIO_B_INT,GPIO_C_INT
 * @param  GpioIndex: the pin which will be setted interrupt disable.valid value GPIOA0~GPIOA31,GPIOB0~GPIOB31,GPIOC0~GPIOC14
 * @return None
 */
void GpioIntDis(uint8_t RegIndex, uint32_t GpioIndex);

/**
 * @brief  clear one gpio pin interrupt	flag
 * @param  RegIndex: the gpio interrupt register macro.valid value GPIO_A_INT,GPIO_B_INT,GPIO_C_INT
 * @param  GpioIndex: the gpio pin interrupt flag will clear.valid value GPIOA0~GPIOA31,GPIOB0~GPIOB31,GPIOC0~GPIOC14
 * @return None
 */
void GpioIntClr(uint8_t RegIndex, uint32_t GpioIndex);

/**
 * @brief  get one gpio port interrupt flag
 * @param  RegIndex: the gpio interrupt register macro.valid value GPIO_A_INT,GPIO_B_INT,GPIO_C_INT
 * @return one gpio port interrupt flag
 */
uint32_t GpioIntFlagGet(uint8_t RegIndex);

/**
 * @Brief	Set PULLDOWN1 & PULLDOWN 2 to A0 pin
 * @Param	IcsVal	ICS_1MA7,
 *					ICS_2MA4,
 *					ICS_4MA1
 * @Return	None
 * @Note@
 */
void GpioA0SetIcs(ICS_VAL IcsVal);

/**
 * @Brief	Set GpioA0 work at function
 * @Param	IcsVal	A0_FOR_GPIO
 * 					A0_FOR_ICS
 * 					A0_FOR_CHARGER
 * 					A0_FOR_PWM
 * @Return	None
 * @Note@ 	When A0 need to be used as GPIO(non-PULLDOWN), set A0_FOR_GPIO mode
 *			When A0 need to be used as Pull-down-current, call GpioA0SetIcs API
 *			When A0 need to be used as Led charger, call ChargerSetMode API
 *			When A0 need to be used as PWM current input, set A0_FOR_PWM mode first.
 */
void GpioA0SetMode(A0_MODE_AF A0ModeAf);


/**
 * @Brief	Set GPIO reset Source
 * @Param	Reset source,
 *			GPIO_RSTSRC_SYSREST,gpio reset by SYSRESET and PORRESET,default
 *			GPIO_RSTSRC_PORREST,gpio reset by PORRESET only
 * @Return	None
 * @Note@
 */
#define GPIO_RSTSRC_SYSREST        (0)
#define GPIO_RSTSRC_PORREST        (1)
void GpioPorSysReset(bool RestSrc);


#define SET_C11_ANALOG_IN() GpioClrRegBits(GPIO_C_PU, GPIOC11),\
							GpioSetRegBits(GPIO_C_PD, GPIOC11),\
							GpioClrRegBits(GPIO_C_IE, GPIOC11),\
							GpioClrRegBits(GPIO_C_OE, GPIOC11)
							
#define SET_C12_ANALOG_IN() GpioClrRegBits(GPIO_C_PU, GPIOC12),\
							GpioSetRegBits(GPIO_C_PD, GPIOC12),\
							GpioClrRegBits(GPIO_C_IE, GPIOC12),\
							GpioClrRegBits(GPIO_C_OE, GPIOC12)

#define SET_C13_ANALOG_IN() GpioClrRegBits(GPIO_C_PU, GPIOC13),\
							GpioSetRegBits(GPIO_C_PD, GPIOC13),\
							GpioClrRegBits(GPIO_C_IE, GPIOC13),\
							GpioClrRegBits(GPIO_C_OE, GPIOC13)
					
#define SET_C14_ANALOG_IN() GpioClrRegBits(GPIO_C_PU, GPIOC14),\
							GpioSetRegBits(GPIO_C_PD, GPIOC14),\
							GpioClrRegBits(GPIO_C_IE, GPIOC14),\
							GpioClrRegBits(GPIO_C_OE, GPIOC14)


typedef enum _USB_PIN_INDEX
{
    GPIO_A24A25 = 1,//PHY1
    GPIO_A22A23 = 2,//PHY2

} USB_PIN_INDEX;

typedef enum _USB_PIN_FEATURE
{
    USB_HOST = 0,
    USB_DEVICE,
    
} USB_PIN_FEATURE;

 /**
 * @brief  config io used for usb
 * @param  pin_index: select phy index
 *     @arg  GPIO_A24A25
 *     @arg  GPIO_A22A23
 * @param  pin_feature:  config phy feature
 *     @arg  USB_HOST
 *     @arg  USB_DEVICE
 * @return None
 */
void GpioConfigUsbPin(USB_PIN_INDEX pin_index, USB_PIN_FEATURE pin_feature);

typedef enum _GEN_PIN_INDEX
{
    GPIO_A22 = 22,
    GPIO_A23,
    GPIO_A24,
    GPIO_A25,

} GEN_PIN_INDEX;

typedef enum _GEN_PIN_FEATURE
{
    DIGITAL_IN = 0,
    DIGITAL_OUT,
    
} GEN_PIN_FEATURE;

 /**
 * @brief  config io used for gpio
 * @param  pin_index: select io index
 *     @arg  GPIO_A22
 *     @arg  GPIO_A23
 *     @arg  GPIO_A24
 *     @arg  GPIO_A25
 * @param  pin_feature:  config io feature
 *     @arg  DIGITAL_IN
 *     @arg  DIGITAL_OUT
 * @return None
 */
void GpioConfigGeneralPin(GEN_PIN_INDEX pin_index, GEN_PIN_FEATURE pin_feature);
#ifdef  __cplusplus
}
#endif//__cplusplus

#endif
