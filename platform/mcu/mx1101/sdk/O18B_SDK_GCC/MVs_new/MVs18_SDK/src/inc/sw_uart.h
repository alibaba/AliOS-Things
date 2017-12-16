/**
 *******************************************************************************
 * @file    sw_uart.h
 * @author  Yancy
 * @version V1.0.0
 * @date    10-10-2013
 * @brief   sw uart driver header file
 * @maintainer: Halley
 * @使用注意事项：
 * 1) 如果想将SWUART的输出重定向到标准输出端口(使用DBG打印)，需要调用函数
 *    EnableSwUartAsFuart(TRUE)进行使能。
 * 2) 必须在Cache初始化之后才能正常发送数据。在标准SDK中，由于Cache初始化包含在
 *    了OSStartKernel中，所以在OSStartKernel之后才能正常发送数据或打印信息。
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


#ifndef __SW_UART_H__
#define __SW_UART_H__

#ifdef __cplusplus
extern "C" {
#endif//__cplusplus


#include "type.h"
#include "gpio.h"

/* GPIO bank selection */
#define  SWUART_GPIO_PORT_A    GPIO_A_IN
#define  SWUART_GPIO_PORT_B    GPIO_B_IN
#define  SWUART_GPIO_PORT_C    GPIO_C_IN

/**
 * @brief  Redirect SWUART to standard output(for example, DBG)
 * @param  EnableFlag: 0: not redirect, 1: redirect
 * @return None.
 */
void EnableSwUartAsFuart(bool EnableFlag);

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
 *     @arg  115200
 *     @arg  57600
 *     @arg  38400
 * @return None.
 */
void SwUartTxInit(uint8_t PortIndex, uint8_t PinIndex, uint32_t BaudRate);

/**
 * @brief  Deinit uart TX to default gpio.
 * @param  PortIndex: select which gpio bank to use
 *     @arg  SWUART_GPIO_PORT_A
 *     @arg  SWUART_GPIO_PORT_B
 *     @arg  SWUART_GPIO_PORT_C
 * @param  PinIndex:  0 ~ 31, select which gpio io to deinit.
 * @return None.
 */
void SwUartTxDeinit(uint8_t PortIndex, uint8_t PinIndex);


/**
 * @brief  Send data from buffer
 * @param  Buf: Buffer address
 * @param  BufLen: Length of bytes to be send
 * @return None.
 * @note   This function can only be used after OSStartKernel() is called
 */
void SwUartSend(uint8_t* Buf, uint32_t BufLen);


/**
 * @Brief	make sw uart baudrate automatic  adaptation
 * @Param	PreFq System Frequency before changed
 * @Param	CurFq System Frequency after changed
 */
void SWUartBuadRateAutoAdap(char PreFq, char CurFq);

#ifdef  __cplusplus
}
#endif//__cplusplus

#endif	//__SW_UART_H__
