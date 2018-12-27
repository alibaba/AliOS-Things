/**
 *******************************************************************************
 * @file    rst.h
 * @author  Lujiangang
 * @version V1.0.1
 * @date    27-May-2013
 * @brief   rst driver header file
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
 
/**
* @addtogroup Çý¶¯
* @{
* @defgroup RST RST
* @{
*/
 
#ifndef __RST_H__
#define __RST_H__

#ifdef __cplusplus
extern "C" {
#endif//__cplusplus

/**
 * @brief  codec system clock domain (registers) logic software reset
 * @param  None
 * @return None
 */
void RstCodecModule(void);

/**
 * @brief  sd logic software reset
 * @param  None
 * @return None
 */
void RstSdModule(void);

/**
 * @brief  decoder logic software reset
 * @param  None
 * @return None
 */
void RstDecModule(void);

/**
 * @brief  usb logic software reset
 * @param  None
 * @return None
 */
void RstUsbModule(void);

/**
 * @brief  saradc logic software reset
 * @param  None
 * @return None
 */
void RstSardcModule(void);

/**
 * @brief  cts logic software reset
 * @param  None
 * @return None
 */
void RstCtsModule(void);

/**
 * @brief  ir logic software reset
 * @param  None
 * @return None
 */
void RstIrModule(void);

/**
 * @brief  fshc logic software reset
 * @param  None
 * @return None
 */
void RstFshcModule(void);

/**
 * @brief  buart logic software reset
 * @param  None
 * @return None
 */
void RstBuartModule(void);

/**
 * @brief  fuart logic software reset
 * @param  None
 * @return None
 */
void RstFuartModule(void);

/**
 * @brief  timer0 logic software reset
 * @param  None
 * @return None
 */
void RstTimer0Module(void);

/**
 * @brief  timer1 logic software reset
 * @param  None
 * @return None
 */
void RstTimer1Module(void);

/**
 * @brief  spim logic software reset
 * @param  None
 * @return None
 */
void RstSpimModule(void);

/**
 * @brief  drmc logic software reset
 * @param  None
 * @return None
 */
void RstSdramModule(void);

/**
 * @brief  I2s logic software reset
 * @param  None
 * @return None
 */
void RstI2sModule(void);

/**
 * @brief  spis logic software reset
 * @param  None
 * @return None
 */
void RstSpisModule(void);

/**
 * @brief  phub logic software reset
 * @param  None
 * @return None
 */
void RstPhubModule(void);

/**
 * @brief  pmem logic software reset
 * @param  None
 * @return None
 */
void RstPmemModule(void);

/**
 * @brief  lcdc logic software reset
 * @param  None
 * @return None
 */
void RstLcdcModule(void);

/**
 * @brief  dlut logic software reset
 * @param  None
 * @return None
 */
void RstDlutModule(void);

/**
 * @brief  dmac logic software reset
 * @param  None
 * @return None
 */
void RstDmacModule(void);

/**
 * @brief  12.288M generator logic software reset
 * @param  None
 * @return None
 */
void Rst12p288GeneratorModule(void);

/**
 * @brief  codec mclk clock domain logic software reset
 * @param  None
 * @return None
 */
void RstMclk(void);

#ifdef  __cplusplus
}
#endif//__cplusplus

#endif

/**
 * @}
 * @}
 */
