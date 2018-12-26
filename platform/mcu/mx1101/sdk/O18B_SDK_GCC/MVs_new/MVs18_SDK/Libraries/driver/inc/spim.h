/**
  *****************************************************************************
  * @file:			spim.h
  * @author			Ingrid Chen
  * @maintainer 	Lilu
  * @version		V1.0.0
  * @data			18-June-2013
  * @Brief			SPI Master driver header file.
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

#ifndef __SPIM_H__
#define __SPIM_H__

#ifdef __cplusplus
extern "C" {
#endif//__cplusplus

#define	SPIM_CLK_DIV_24M	0x0			/**< SPI master clock Div 24MHz*/
#define	SPIM_CLK_DIV_12M	0x1			/**< SPI master clock Div 12MHz*/
#define	SPIM_CLK_DIV_6M		0x2			/**< SPI master clock Div 6MHz*/
#define	SPIM_CLK_DIV_3M		0x3			/**< SPI master clock Div 3MHz*/
#define	SPIM_CLK_DIV_1M5	0x4			/**< SPI master clock Div 1.5MHz*/
#define	SPIM_CLK_DIV_750K	0x5			/**< SPI master clock Div 750KHz*/
#define	SPIM_CLK_DIV_325K	0x6			/**< SPI master clock Div 325KHz*/

#define	NONE_FLASH  0
#define	FLASH_GD	1
#define	FLASH_SST	2

/**
 * err code define
 */
typedef enum _SPI_MASTER_ERR_CODE
{
    ERR_SPIM_TIME_OUT = -255,			/**<function execute time out*/
    ERR_SPIM_DATALEN_OUT_OF_RANGE,		/**<data len is out of range < 0*/
    SPIM_NONE_ERR = 0,
} SPI_MASTER_ERR_CODE;


/**
 * @Brief	SPI master initial mode and clock div
 * @Param	Mode
 *			0 - CPOL = 0 & CPHA = 0, 1 - CPOL = 0 & CPHA = 1,
 *			2 - CPOL = 1 & CPHA = 0, 3 - CPOL = 1, CPHA = 1
 * @Param	ClkDiv 	valid value:(0 ~ 11)
 * 			spi clock = system clock / (2 EXP (ClkDivExp)).
 * @Return	None
 * @note@	for example, bClkDivExp = 3, then spi clock = system clock / 8.
 */
void SpiMasterInit(uint8_t Mode, uint8_t ClkDiv);

/**
 * @Brief	SPI master module send 1 byte
 * @Param	val  data for sending
 * @Return	None
 */
void SpiMasterSendByte(uint8_t val);

/**
 * @Brief	SPI master module receive 1 byte
 * @Param	None
 * @Return	received data
 */
uint8_t SpiMasterRecvByte(void);

/**
 * @Brief	SPI master module send N bytes
 * @Param	SendBuf point of send data buffer
 * @Param	Length max 65536 byte
 * @Return	error code
 * @Note@
 */
SPI_MASTER_ERR_CODE SpiMasterSendData(uint8_t* SendBuf, uint32_t Length);

/**
 * @Brief	SPI master module receive N bytes
 * @Param	RecvBuf point of receive data buffer
 * @Param	Length max 65536 byte
 * @Return	error code
 * @Note@
 */
SPI_MASTER_ERR_CODE SpiMasterRecvData(uint8_t* RecvBuf, uint32_t Length);

/**
 * @Brief	SPI master module start sending or receiving data
 *				SpiMasterStartData(...), SpiMasterGetDmaDone(), SpiMasterIntClr()    These three functions are used together. 
 * @Param	Buf point of send or receive data buffer
 * @Param	Length max 65536 byte
 * @Param	Direction 0:send   1:receive
 * @Return	None
 * @Note@
 */
void SpiMasterStartData(uint8_t* Buf, uint32_t Length, uint8_t Dir);
#define SpiMasterSendBytes(Buf, Length)   SpiMasterStartData(Buf, Length, 0)
#define SpiMasterRecvBytes(Buf, Length)   SpiMasterStartData(Buf, Length, 1)

/**
 * @Brief	Obtain the state of sending or receiving data
 *				SpiMasterStartData(...), SpiMasterGetDmaDone(), SpiMasterIntClr()    These three functions are used together. 
 * @Param	None
 * @Return	The state of DMA
 * @Note@
 */
bool SpiMasterGetDmaDone(void);

/**
 * @Brief	Clear interrupt bit after DMA done
 *				SpiMasterStartData(...), SpiMasterGetDmaDone(), SpiMasterIntClr()    These three functions are used together. 
 * @Param	None
 * @Return	None
 * @Note@
 */
void SpiMasterIntClr(void);

#ifdef __cplusplus
}
#endif//__cplusplus

#endif
