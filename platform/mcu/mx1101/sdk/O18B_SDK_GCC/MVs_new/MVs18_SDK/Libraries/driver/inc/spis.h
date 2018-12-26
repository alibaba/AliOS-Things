/**
  *****************************************************************************
  * @file:			spis.h
  * @author			Ingrid Chen
  * @version		V1.0.0
  * @data			18-June-2013
  * @Brief			SPI Slave driver header file.
  * @note			For sdio and spi can't access memory simultaneously
  * @maintainer lilu
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

#ifndef __SPIS_H__
#define __SPIS_H__

#ifdef __cplusplus
extern "C" {
#endif//__cplusplus


/**
 * @Brief	SPI slave initial mode and config pmem size
 * @Param	Mode
 *			0 - CPOL = 0 & CPHA = 0, 1 - CPOL = 0 & CPHA = 1,
 *			2 - CPOL = 1 & CPHA = 0, 3 - CPOL = 1 & CPHA = 1
 * @Param	StartAddr
 * @Param	RXDepth
 * @Param	TXDepth
 * @Return	None
 * @Note@ 	FirstAddr and RXDepth/TXDepth must 4bytes align.spi_mode and uart_mode cannot be set enable at the same time
 */
bool SpiSlaveInit(uint8_t Mode, uint32_t StartAddr, uint32_t RxDepth, uint32_t TxDepth);

/**
 * @Brief	SPI slave module close
 * @Param	None
 * @Return	None
 * @Note@
 */
void SpiSlaveClose(void);

/**
 * @Brief	SPI slave module send data
 * @Param	Buf 		source date address point
 * @Param	BufLen 		source data length
 * @Param	TimeOut		send fail timeout value
 * @Return	The amount of data acctually send
 */
uint32_t SpiSlaveSend(uint8_t* Buf, uint32_t BufLen, uint32_t TimeOut);

/**
 * @Brief	SPI slave module receive data
 * @Param	Buf 		recieve date address point
 * @Param	BufLen 		recieve data length
 * @Param	TimeOut 	recieve fail timeout value
 * @Return	The amount of data acctually recevied
 */
uint32_t SpiSlaveReceive(uint8_t* Buf, uint32_t BufLen, uint32_t TimeOut);

/**
 * @Brief	SPI slave module clear interrupt flag
 * @Param	None
 * @Return	None
 * @Note@
 */
void SpiSlaveIntClr(void);

/**
 * @brief  the remaining length of data in memory, unit:bytes
 * @param  None
 * @return the remaining length of data
 */
uint32_t SpiSlaveRxDataLenGet(void);

/**
 * @brief  Clear Rx Buffer
 * @param  None
 * @return None
 */
void SpisClrRxBuff(void);
#ifdef  __cplusplus
}
#endif//__cplusplus

#endif
