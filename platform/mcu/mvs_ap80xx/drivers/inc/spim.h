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

/**
* @addtogroup 驱动
* @{
* @defgroup SPIM SPIM
* @{
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
 * 错误码
 */
typedef enum _SPI_MASTER_ERR_CODE
{
    ERR_SPIM_TIME_OUT = -255,			/**<function execute time out*/
    ERR_SPIM_DATALEN_OUT_OF_RANGE,		/**<data len is out of range < 0*/
    SPIM_NONE_ERR = 0,
} SPI_MASTER_ERR_CODE;


/**
 * @brief	SPIM初始化
 * @param	Mode
 *			0 - CPOL = 0 & CPHA = 0, 1 - CPOL = 0 & CPHA = 1,
 *			2 - CPOL = 1 & CPHA = 0, 3 - CPOL = 1 & CPHA = 1,
 * @param	ClkDiv 	有效值:(0 ~ 11)
 * 			分频系数对应SPIM的频率：0 - 24M、1 - 12M、2 - 6M、3 - 3M、4 - 1.5M....
 * @return	无
 * @note@	对系统频率进行分频、降频会影响SPIM输出频率
 */
void SpiMasterInit(uint8_t Mode, uint8_t ClkDiv);

/**
 * @brief	SPIM发送1字节数据
 * @param	val  将要发送的1字节数据值
 * @return	无
 */
void SpiMasterSendByte(uint8_t val);

/**
 * @brief	SPIM接收1字节数据
 * @param	无
 * @return	接收到的1字节数据
 */
uint8_t SpiMasterRecvByte(void);

/**
 * @brief	SPIM发送数据
 * @param	SendBuf	发送数据Buf的首地址
 * @param	Length 	发送数据的长度（内部使用DMA搬移数据，最大长度65535字节）
 * @return	错误码
 * @note@
 */
SPI_MASTER_ERR_CODE SpiMasterSendData(uint8_t* SendBuf, uint32_t Length);

/**
 * @brief	SPIM接收数据
 * @param	RecvBuf	接收数据Buf的首地址
 * @param	Length	接收数据的长度（内部使用DMA搬移数据，最大长度65535字节）
 * @return	错误码
 * @note@
 */
SPI_MASTER_ERR_CODE SpiMasterRecvData(uint8_t* RecvBuf, uint32_t Length);

/**
 * @brief	SPIM开始发送、接收数据
 *				SpiMasterStartData(...), SpiMasterGetDmaDone(), SpiMasterIntClr()三个函数组合使用构成SPIM收发数据的非阻塞接口
 * @param	Buf			数据Buf的首地址
 * @param	Length		数据的长度（内部使用DMA搬移数据，最大长度65535字节）
 * @param	Direction	传输方向 0:发送   1:接收
 * @return	无
 * @note@
 */
void SpiMasterStartData(uint8_t* Buf, uint32_t Length, uint8_t Dir);
#define SpiMasterSendBytes(Buf, Length)   SpiMasterStartData(Buf, Length, 0)
#define SpiMasterRecvBytes(Buf, Length)   SpiMasterStartData(Buf, Length, 1)

/**
 * @brief	获取SPIM收发数据的完成状态
 *				SpiMasterStartData(...), SpiMasterGetDmaDone(), SpiMasterIntClr()三个函数组合使用构成SPIM收发数据的非阻塞接口 
 * @param	无
 * @return	返回数据传输是否完成
 * @note@
 */
bool SpiMasterGetDmaDone(void);

/**
 * @brief	清除SPIM完成标志位
 *				SpiMasterStartData(...), SpiMasterGetDmaDone(), SpiMasterIntClr()三个函数组合使用构成SPIM收发数据的非阻塞接口 
 * @param	无
 * @return	无
 * @note@
 */
void SpiMasterIntClr(void);

#ifdef __cplusplus
}
#endif//__cplusplus

#endif

/**
 * @}
 * @}
 */
