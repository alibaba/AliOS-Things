/**
  *****************************************************************************
  * @file:			dma.h
  * @author			Ingrid Chen
  * @version		V1.0.0
  * @data			06-June-2013
  * @maintainer       Halley
  * @Brief			DMA module driver header file.
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
* @defgroup DMA DMA
* @{
*/
  
#ifndef __DMA_H__
#define __DMA_H__

#ifdef __cplusplus
extern "C" {
#endif//__cplusplus

/*
**********************************************************
*					TYPE DEFINE
**********************************************************
*/
/**
 * DMA Transfer Mode
 */
typedef enum _DMA_XFER_MODE
{
    DMA_XFER_MODE_NONE,
    DMA_XFER_MODE_BASIC,
    DMA_XFER_MODE_PP,
    DMA_XFER_MODE_SHUT
} DMA_XFER_MODE;

/**
 * DMA Transfer Channel
 */
typedef enum _DMA_XFER_CHANNEL
{
    DMA_XFER_ALTCH0,
    DMA_XFER_PRICH0,
    DMA_XFER_PRICH1
} DMA_XFER_CHANNEL;

/**
 * DMA Address increment
 */
typedef enum _DMA_ADDR_INC
{
    DMA_ADDR_INC_NO,
    DMA_ADDR_INC_BYTE,
    DMA_ADDR_INC_WORD,
    DMA_ADDR_INC_DWORD
} DMA_ADDR_INC;

/**
 * DMA Address Width
 */
typedef enum _DMA_ADDR_WIDTH
{
    DMA_ADDR_WIDTH_BYTE,
    DMA_ADDR_WIDTH_WORD,
    DMA_ADDR_WIDTH_DWORD
} DMA_ADDR_WIDTH;

/*
**********************************************************
*					GLOBAL FUNCTION PROTOTYPE
**********************************************************
*/
/**
 * @brief	通过DMA通道CH0在XMEM/VMEM/PMEM/SDRAM内部或两两传输数据
 * @param	SrcAddr
 * @param	DstAddr
 * @param	Length
 * @return	None
 * @note@ 	Transfer Length is max to 65536 bytes
 */
void DmaMemoryCopy(uint32_t SrcAddr, uint32_t DstAddr, uint32_t Length);

/**
 * @brief	Get DMA Primary CH0 interrupt status
 * @param	None
 * @return	Interrupt flag
 * @note@
 */
bool DmaPriCh0GetInt(void);

/**
 * @brief	Clear DMA Primary CH0 interrupt status
 * @param	None
 * @return	None
 * @note@
 */
void DmaPriCh0IntClr(void);


/**
 * @brief	Set DMA channel transfer mode
 * @param	Channel refer to DMA_XFER_CHANNEL
 * @param	Mode refer to DMA_XFER_MODE
 * @return	None
 * @note@
 */
void DmaSetChannelMode(uint8_t Channel, uint8_t Mode);

/**
 * @brief	Close DMA Primary CH0
 * @param	None
 * @return	None
 * @note@
 */
void DmaPriCh0Close(void);

/**
 * @brief	DMA CH0 request 
 * @param	None
 * @return	None
 * @note@
 */
void DmaCh0SwReq(void);

/**
 * @brief	Get DMA Primary CH1 interrupt status
 * @param	None
 * @return	Interrupt flag
 * @note@
 */
bool DmaPriCh1GetInt(void);

/**
 * @brief	Clear DMA Primary CH1 interrupt status
 * @param	None
 * @return	None
 * @note@
 */
void DmaPriCh1IntClr(void);

/**
 * @brief	Close DMA Primary CH1
 * @param	None
 * @return	None
 * @note@
 */
void DmaPriCh1Close(void);

/**
 * @brief	设置DMA传输的参数
 * @param	请结合几个参数的数据结构理解
 * @return	None
 * @note@
 */
void DmaSetParameter(uint8_t Channel,
                     uint32_t SrcAddr, DMA_ADDR_INC SrcAddrNCR, DMA_ADDR_WIDTH SrcDataWIDTH,
                     uint32_t DstAddr, DMA_ADDR_INC DStAddrINCR, DMA_ADDR_WIDTH DstDataWIDTH,
                     uint8_t BurstSize, uint32_t XferSize);

bool DmaCopyDone(void);

/**
 * @brief	Copy data to LCD Module
 * @param	SrcAddr
 * @param	XferSize
 * @return	None
 * @note@ 	Source Start Address must 4 byte align
 *			Transfer Size is max to 262143
 */
void DmaPriCh1ShuttleXferSet(uint32_t SrcAddr, uint32_t XferSize);

#ifdef __cplusplus
}
#endif//__cplusplus

#endif

/**
 * @}
 * @}
 */
