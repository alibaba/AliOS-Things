/*
*****************************************************************************
*					Mountain View Silicon Tech. Inc.
*	Copyright 2010, Mountain View Silicon Tech. Inc., ShangHai, China
*					All rights reserved.
*
* Filename:			rst.c
* Description:		rst driver.
* maintainer: lujiangang
* Change History:
******************************************************************************
*/
#ifndef __RST_H__
#define __RST_H__

#ifdef __cplusplus
extern "C" {
#endif//__cplusplus

/**
 * @Brief  codec system clock domain (registers) logic software reset
 * @param  None
 * @return None
 */
void RstCodecModule(void);

/**
 * @Brief  sd logic software reset
 * @param  None
 * @return None
 */
void RstSdModule(void);

/**
 * @Brief  decoder logic software reset
 * @param  None
 * @return None
 */
void RstDecModule(void);

/**
 * @Brief  usb logic software reset
 * @param  None
 * @return None
 */
void RstUsbModule(void);

/**
 * @Brief  saradc logic software reset
 * @param  None
 * @return None
 */
void RstSardcModule(void);

/**
 * @Brief  cts logic software reset
 * @param  None
 * @return None
 */
void RstCtsModule(void);

/**
 * @Brief  ir logic software reset
 * @param  None
 * @return None
 */
void RstIrModule(void);

/**
 * @Brief  fshc logic software reset
 * @param  None
 * @return None
 */
void RstFshcModule(void);

/**
 * @Brief  buart logic software reset
 * @param  None
 * @return None
 */
void RstBuartModule(void);

/**
 * @Brief  fuart logic software reset
 * @param  None
 * @return None
 */
void RstFuartModule(void);

/**
 * @Brief  timer0 logic software reset
 * @param  None
 * @return None
 */
void RstTimer0Module(void);

/**
 * @Brief  timer1 logic software reset
 * @param  None
 * @return None
 */
void RstTimer1Module(void);

/**
 * @Brief  spim logic software reset
 * @param  None
 * @return None
 */
void RstSpimModule(void);

/**
 * @Brief  drmc logic software reset
 * @param  None
 * @return None
 */
void RstSdramModule(void);

/**
 * @Brief  I2s logic software reset
 * @param  None
 * @return None
 */
void RstI2sModule(void);

/**
 * @Brief  spis logic software reset
 * @param  None
 * @return None
 */
void RstSpisModule(void);

/**
 * @Brief  phub logic software reset
 * @param  None
 * @return None
 */
void RstPhubModule(void);

/**
 * @Brief  pmem logic software reset
 * @param  None
 * @return None
 */
void RstPmemModule(void);

/**
 * @Brief  lcdc logic software reset
 * @param  None
 * @return None
 */
void RstLcdcModule(void);

/**
 * @Brief  dlut logic software reset
 * @param  None
 * @return None
 */
void RstDlutModule(void);

/**
 * @Brief  dmac logic software reset
 * @param  None
 * @return None
 */
void RstDmacModule(void);

/**
 * @Brief  12.288M generator logic software reset
 * @param  None
 * @return None
 */
void Rst12p288GeneratorModule(void);

/**
 * @Brief  codec mclk clock domain logic software reset
 * @param  None
 * @return None
 */
void RstMclk(void);

#ifdef  __cplusplus
}
#endif//__cplusplus

#endif

