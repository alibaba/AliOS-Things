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
 * @Brief	通过DMA通道CH0在XMEM/VMEM/PMEM/SDRAM内部或两两传输数据
 * @Param	SrcAddr
 * @Param	DstAddr
 * @Param	Length
 * @Return	None
 * @Note@ 	Transfer Length is max to 65536 bytes
 */
void DmaMemoryCopy(uint32_t SrcAddr, uint32_t DstAddr, uint32_t Length);

/**
 * @Brief	Get DMA Primary CH0 interrupt status
 * @Param	None
 * @Return	Interrupt flag
 * @Note@
 */
bool DmaPriCh0GetInt(void);

/**
 * @Brief	Clear DMA Primary CH0 interrupt status
 * @Param	None
 * @Return	None
 * @Note@
 */
void DmaPriCh0IntClr(void);


/**
 * @Brief	Set DMA channel transfer mode
 * @Param	Channel refer to DMA_XFER_CHANNEL
 * @Param	Mode refer to DMA_XFER_MODE
 * @Return	None
 * @Note@
 */
void DmaSetChannelMode(uint8_t Channel, uint8_t Mode);

/**
 * @Brief	Close DMA Primary CH0
 * @Param	None
 * @Return	None
 * @Note@
 */
void DmaPriCh0Close(void);

/**
 * @Brief	DMA CH0 request 
 * @Param	None
 * @Return	None
 * @Note@
 */
void DmaCh0SwReq(void);

/**
 * @Brief	Get DMA Primary CH1 interrupt status
 * @Param	None
 * @Return	Interrupt flag
 * @Note@
 */
bool DmaPriCh1GetInt(void);

/**
 * @Brief	Clear DMA Primary CH1 interrupt status
 * @Param	None
 * @Return	None
 * @Note@
 */
void DmaPriCh1IntClr(void);

/**
 * @Brief	Close DMA Primary CH1
 * @Param	None
 * @Return	None
 * @Note@
 */
void DmaPriCh1Close(void);

/**
 * @Brief	设置DMA传输的参数
 * @Param	请结合几个参数的数据结构理解
 * @Return	None
 * @Note@
 */
void DmaSetParameter(uint8_t Channel,
                     uint32_t SrcAddr, DMA_ADDR_INC SrcAddrNCR, DMA_ADDR_WIDTH SrcDataWIDTH,
                     uint32_t DstAddr, DMA_ADDR_INC DStAddrINCR, DMA_ADDR_WIDTH DstDataWIDTH,
                     uint8_t BurstSize, uint32_t XferSize);

bool DmaCopyDone(void);

/**
 * @Brief	Copy data to LCD Module
 * @Param	SrcAddr
 * @Param	XferSize
 * @Return	None
 * @Note@ 	Source Start Address must 4 byte align
 *			Transfer Size is max to 262143
 */
void DmaPriCh1ShuttleXferSet(uint32_t SrcAddr, uint32_t XferSize);

#ifdef __cplusplus
}
#endif//__cplusplus

#endif
