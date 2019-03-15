/**
  *****************************************************************************
  * @file:			spis.h
  * @author			Ingrid Chen
  * @maintainer		lilu
  * @version		V1.0.0
  * @data			18-June-2013
  * @Brief			SPI Slave driver header file.
  * @note			For sdio and spi can't access memory simultaneously
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
* @addtogroup 驱动
* @{
* @defgroup SPIS SPIS
* @{
*/

#ifndef __SPIS_H__
#define __SPIS_H__

#ifdef __cplusplus
extern "C" {
#endif//__cplusplus


/**
 * @brief	SPIS初始化
 * @param	Mode
 *			0 - CPOL = 0 & CPHA = 0, 1 - CPOL = 0 & CPHA = 1,
 *			2 - CPOL = 1 & CPHA = 0, 3 - CPOL = 1 & CPHA = 1
 * @param	StartAddr	SPIS缓冲区起始地址，从PMEM开始的相对地址
 * @param	RXDepth		接收缓冲区深度（最大深度可到32767字节）
 * @param	TXDepth		发送缓冲区深度（最大深度可到32767字节）（StartAddr+RXDepth即为发送缓冲区首地址）
 * @return	无
 * @note@ 	由于公用PMEM总线spis和uart不能同时开启
 */
bool SpiSlaveInit(uint8_t Mode, uint32_t StartAddr, uint32_t RxDepth, uint32_t TxDepth);

/**
 * @brief	SPIS关闭，主要为释放PMEM总线
 * @param	无
 * @return	无
 * @note@
 */
void SpiSlaveClose(void);

/**
 * @brief	SPIS发送数据
 * @param	Buf 		发送数据buf的首地址
 * @param	BufLen 		发送数据的长度（字节单位）
 * @param	TimeOut		超时时间
 * @return	实际发送的数据长度（字节单位）
 */
uint32_t SpiSlaveSend(uint8_t* Buf, uint32_t BufLen, uint32_t TimeOut);

/**
 * @brief	SPIS接收数据
 * @param	Buf 		接收数据buf的首地址
 * @param	BufLen 		接收数据长度（字节单位）
 * @param	TimeOut 	超时时间
 * @return	实际接收的数据长度
 */
uint32_t SpiSlaveReceive(uint8_t* Buf, uint32_t BufLen, uint32_t TimeOut);

/**
 * @brief	SPIS清除中断标志
 * @param	无
 * @return	无
 * @note@
 */
void SpiSlaveIntClr(void);

/**
 * @brief  获取接收缓冲区中剩余数据长度（字节单位）
 * @param  无
 * @return 返回接收缓冲区剩余数据长度（字节单位）
 */
uint32_t SpiSlaveRxDataLenGet(void);

/**
 * @brief  清空接收缓冲区
 * @param  无
 * @return 无
 */
void SpisClrRxBuff(void);
#ifdef  __cplusplus
}
#endif//__cplusplus

#endif

/**
 * @}
 * @}
 */
