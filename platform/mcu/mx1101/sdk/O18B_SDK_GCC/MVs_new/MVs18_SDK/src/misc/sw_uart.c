/**
 *******************************************************************************
 * @file    sw_uart.c
 * @author  Yancy
 * @version V1.0.0
 * @date    10-10-2013
 * @maintainer: Halley
 * @brief   software uart driver. When hardware uart pins is occupied as other
 *          functions, this software uart can output debug info. This software
 *          uart only has TX function.
 * Changelog
 *          SW baud rate automatic adaptation -Robert 20140318
 *******************************************************************************
 * @attention
 *
 * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
 * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
 * TIME. AS A RESULT, MVSILICON SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
 * INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
 * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
 * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
 *
 * <h2><center>&copy; COPYRIGHT 2013 MVSilicon </center></h2>
 */

#include "type.h"
#include "gpio.h"


uint32_t OutRegAddr, OutRegBitMsk, DelayLoops;

/**
 * @brief  Init specified IO as software uart's TX.
 *         The BaudRate is valid in DPLL 96M,if you use RC48M its true value is an half of BaudRate.
 *         Any other divided frequency is the same with last example.
 * @param  PortIndex: select which gpio bank to use
 *     @arg  SWUART_GPIO_PORT_A
 *     @arg  SWUART_GPIO_PORT_B
 *     @arg  SWUART_GPIO_PORT_C
 * @param  PinIndex:  0 ~ 31, select which gpio io to use.
 *         for example, if PortIndex = SWUART_GPIO_PORT_A, PinIndex = 10,
 *         GPIO_A10 is used as software uart's TX.
 * @param  BaudRate, can be 115200, 57600 or 38400
  *    @arg  115200
 *     @arg  57600
 *     @arg  38400
 * @return None.
 */
void SwUartTxInit(uint8_t PortIndex, uint8_t PinIndex, uint32_t BaudRate)
{
	OutRegAddr = 0x40002000 + (PortIndex + 1) * 4;//Output Register Address
	OutRegBitMsk  = 1 << PinIndex;//Output Register Bit Mask

	GpioSetRegBits(PortIndex + 1, OutRegBitMsk);//Must output high as default!
	GpioClrRegBits(PortIndex + 2, OutRegBitMsk);//Input disable
	GpioSetRegBits(PortIndex + 3, OutRegBitMsk);//Output enable

	if(BaudRate == 115200)
	{
		DelayLoops = 270;
	}
	else if(BaudRate == 57600)
	{
		DelayLoops = 548;
	}
	else if(BaudRate == 38400)
	{
		DelayLoops = 825;
	}
}

 /**
 * @brief  Deinit uart TX to default gpio.
 * @param  PortIndex: select which gpio bank to use
 *     @arg  SWUART_GPIO_PORT_A
 *     @arg  SWUART_GPIO_PORT_B
 *     @arg  SWUART_GPIO_PORT_C
 * @param  PinIndex:  0 ~ 31, select which gpio io to deinit.
 * @return None.
 */
void SwUartTxDeinit(uint8_t PortIndex, uint8_t PinIndex)
{
	OutRegAddr = 0x40002000 + (PortIndex + 1) * 4;//Output Register Address
	OutRegBitMsk  = 1 << PinIndex;//Output Register Bit Mask

	GpioClrRegBits(PortIndex + 1, OutRegBitMsk);//OUTPUT = 0;
	GpioSetRegBits(PortIndex + 2, OutRegBitMsk);//IE = 1
	GpioClrRegBits(PortIndex + 3, OutRegBitMsk);//OE = 0
}

/**
 * @Brief	make sw uart baudrate automatic  adaptation
 * @Param	PreFq System Frequency before changed
 * @Param	CurFq System Frequency after changed
 */
void SWUartBuadRateAutoAdap(char PreFq, char CurFq)
{
	if(PreFq > CurFq)
    {        
//        if(ClkSysClkSelGet())
        {
            DelayLoops = PreFq * 96 * DelayLoops / (CurFq * 80);
        }
//        else
//        {
//            DelayLoops = PreFq * 1 * DelayLoops / (CurFq * 1);
//        }
    }
    else
	{
//        if(ClkSysClkSelGet())
        {
            DelayLoops = (PreFq * 80 * DelayLoops) / (CurFq * 96);
        }
//        else
//        {
//            DelayLoops /= (CurFq * 1) / (PreFq * 1);
//        }
    }
}


/**
 * @brief  Delay to keep tx's level for some time
 * @param  void
 * @return None.
 */
__attribute__((section("CACHE.4KBMEM")))
__asm void SwUartDelay(void)
{
	IMPORT DelayLoops
	
	PUSH {R0}
	LDR R0, = DelayLoops
	LDR R0, [R0]
DELAY_LOOP
	SUBS R0, #1
	BNE DELAY_LOOP
	
	POP {R0}
	BX  LR
    ALIGN
}

/**
* @brief  Send 1 byte
* @param  c: byte to be send
* @return None
*/
__attribute__((section("CACHE.4KBMEM")))
__asm void SwUartSendByte(uint8_t c)
{
    IMPORT OutRegAddr
    IMPORT OutRegBitMsk
    
;-------------------------------------------------------
;save relative registrers
;-------------------------------------------------------    
    PUSH {R1-R5, LR}
    ;save interrupt configuration
    MRS R1, FAULTMASK
    ;disable all interrupts
    CPSID   F

;-------------------------------------------------------
;Load OutRegAddr and OutRegBitMsk
;-------------------------------------------------------
    LDR R2, =OutRegAddr
    LDR R2, [R2]
    LDR R3, [R2]
    LDR R4, =OutRegBitMsk
    LDR R4, [R4]

;-------------------------------------------------------
;send start bit
;-------------------------------------------------------
    BIC R3, R3, R4
    STR R3, [R2] ;output zero
    BL SwUartDelay

;-------------------------------------------------------  
;send 8 bit data
;-------------------------------------------------------
    MOV R5, #8
DATA_LOOP
    TST R0, #1
    ITE EQ
    BICEQ R3, R3, R4
    ORRNE R3, R3, R4
    STR R3, [R2];output 0/1 according to data
    BL SwUartDelay
 
    LSR R0, #1
    SUBS R5, #1
    BNE DATA_LOOP 
    
;-------------------------------------------------------
;send stop bit
;-------------------------------------------------------
    ORR R3, R3, R4
    STR R3, [R2];output stop bit
    BL SwUartDelay

;-------------------------------------------------------
;restore relative registers
;-------------------------------------------------------
    MSR FAULTMASK, R1   
    POP {R1-R5, PC}
	ALIGN
}

/**
 * @brief  Send data from buffer
 * @param  Buf: Buffer address
 * @param  BufLen: Length of bytes to be send
 * @return None
 */
void SwUartSend(uint8_t* Buf, uint32_t BufLen)
{
	while(BufLen--)
	{
		SwUartSendByte(*Buf++);
	}
}



