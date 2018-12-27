/**
 *****************************************************************************
 * @file     I2S.h
 * @author   Ingrid
 * @version  V2.1.0
 * @date     06-May-2014
 * @maintainer: YunWang
 * @brief    declare i2s module driver interface
 *****************************************************************************
 * @attention
 *
 * <h2><center>&copy; COPYRIGHT 2013 MVSilicon </center></h2>
 */

/**
* @addtogroup Çý¶¯
* @{
* @defgroup I2S I2S
* @{
*/

#ifndef __I2S_H__
#define __I2S_H__

#ifdef  __cplusplus
extern "C" {
#endif//__cplusplus

#include "type.h"

#define I2S_DATA_FROM_ADC                       1
#define I2S_DATA_FROM_DEC                       0

/**
 * i2s format definition
 */
typedef enum _I2S_DATA_FORMAT
{
	I2S_FORMAT_RIGHT = 0,
	I2S_FORMAT_LEFT,
	I2S_FORMAT_I2S,
	I2S_FORMAT_DSP
} I2S_DATA_FORMAT;

#define I2S_DSP_MODEB       (0)  /**<MSB is available on 1st BCLK rising edge after LRC rising edge*/
#define I2S_DSP_MODEA       (1)  /**<MSB is available on 2nd BCLK rising edge after LRC rising edge*/

#define CLASSD_STA_MODE		(0)  /**<support sta classd*/
#define CLASSD_NTP_MODE		(1)  /**<support ntp classd*/


/**
 * @brief  Set i2s sample rate in different mode
 * @param  SampleRate 8/11.025/12/16/22.05/24/32/44.1/48(KHz)
 * @param  Mode    0 = Normal Mode, 
 *                 1 = USB Mode, 
 *                 2 = EXTCLK_NORMAL_MODE, 
 *                 3 = EXTCLK_USB_MODE
 * @return NONE
 */
void I2sSampleRateSet(uint32_t SampleRate, uint8_t Mode);

/**
 * @brief  Set i2s module into master mode
 * @param  SrcDatFrom 1 = i2s pcm data from adc, 0 = i2s pcm data from phub
 * @param  ClkMode    0 = Normal Mode, 1 = USB Mode, 2 = EXTCLK_NORMAL_MODE, 
 *                    3 = EXTCLK_USB_MODE
 * @param  I2sFormat right/left/i2s/dsp
 * @return NONE
 */
void I2sSetMasterMode(uint8_t SrcDatFrom, uint8_t ClkMode, uint8_t I2sFormat);

/**
 * @brief  Set i2s module into slave mode
 * @param  SrcDatFrom 1 = i2s pcm data from adc, 0 = i2s pcm data from phub
 * @param  ClkMode    0 = Normal Mode, 1 = USB Mode, 2 = EXTCLK_NORMAL_MODE, 
 *                    3 = EXTCLK_USB_MODE
 * @param  I2sFormat right/left/i2s/dsp
 * @return NONE
 */
void I2sSetSlaveMode(uint8_t SrcDatFrom, uint8_t ClkMode, uint8_t I2sFormat);

/**
 * @brief  Set bluetooth into i2s master mode,fixed 8000Hz sample rate
 * @param  SrcDatFrom 1 = i2s pcm data from adc, 0 = i2s pcm data from phub
 * @param  ClkMode    0 = Normal Mode, 1 = USB Mode, 2 = EXTCLK_NORMAL_MODE, 
 *                    3 = EXTCLK_USB_MODE
 * @param  DspModeSel 0 = mode B 1 = mode A
 * @return NONE
 */
void I2sBluetoothMasterModeSet(uint8_t SrcDatFrom, uint8_t ClkMode, uint8_t DspModeSel);

/**
 * @brief  set bluetooth into i2s slave mode,fixed 8000Hz sample rate
 * @param  SrcDatFrom 1 = i2s pcm data from adc, 0 = i2s pcm data from phub
 * @param  ClkMode    0 = Normal Mode, 1 = USB Mode, 2 = EXTCLK_NORMAL_MODE, 
 *                    3 = EXTCLK_USB_MODE
 * @param  DspModeSel 0 = mode B 1 = mode A
 * @return NONE
 */
void I2sBluetoothSlaveModeSet(uint8_t SrcDatFrom, uint8_t ClkMode, uint8_t DspModeSel);

/**
 * @brief  Set classD sample rate for different classD chip, only support normal mode
 * @param  SampleRate 8/11.025/12/16/22.05/24/32/44.1/48(kHz)
 * @param  ClkMode    0 = Normal Mode, 1 = USB Mode, 2 = EXTCLK_NORMAL_MODE, 
 *                    3 = EXTCLK_USB_MODE
 * @param  ClassDSel 0--support sta classD, 1--support ntp classD
 * @return NONE
 */
void I2sClassdSampleRateSet(uint32_t SampleRate, uint8_t ClkMode, uint8_t ClassDSel);
/**
 * @brief  Set sample rate for different STA classD chip, only support normal mode
 * @param  SampleRate 8/11.025/12/16/22.05/24/32/44.1/48(kHz)
 * @param  ClkMode    0 = Normal Mode, 1 = USB Mode, 2 = EXTCLK_NORMAL_MODE, 
 *                    3 = EXTCLK_USB_MODE
 * @param  ReSample 0--not resample, 1--do resample
 * @return NONE
 */
void I2sClassdSTASampleRateSet(uint32_t SampleRate,uint8_t ClkMode,  bool ReSample);

/**
 * @brief  Support extern classD,it works in i2s slave mode and normal mode
 * @param  SrcDatFrom 1 = i2s pcm data from adc, 0 = i2s pcm data from phub
 * @param  I2sFormat right/left/i2s/dsp
 * @return NONE
 */
void I2sClassdSlaveModeSet(uint8_t SrcDatFrom, uint8_t I2sFormat);

/**
 * @brief  Support extern classD,it works in i2s master mode and normal mode
 * @param  SrcDatFrom 1 = i2s pcm data from adc, 0 = i2s pcm data from phub
 * @param  I2sFormat right/left/i2s/dsp
 * @return NONE
 */
void I2sClassdMasterModeSet(uint8_t SrcDatFrom, uint8_t I2sFormat);

/**
 * @brief  Assign pmem for i2s in
 * @param  PmemAddr assign start addr of i2s write pmem, base addr offset relative
 * @param  PmemDepth assign depth of pmem
 * @return None
 */
bool I2sInToPmemConfig(uint16_t PmemAddr, uint16_t PmemDepth);

/**
 * @brief  i2s write pmem disable
 * @param  NONE
 * @return NONE
 */
void I2sWritePmemDis(void);

/**
 * @brief  i2s write pmem enable
 * @param  NONE
 * @return NONE
 */
void I2sWritePmemEn(void);

/**
 * @brief  the remaining length of data in memory, unit:samples
 * @param  None
 * @return the remaining length of data
 */
uint32_t I2sPmemPcmRemainLenGet(void);

/**
 * @brief  read I2S data from PMEM
 * @param  Buffer  read pcm data to this buffer, and all pcm data fill in WAV format
 * @param  NumSamplesRequired  read data in samples
 * @param  PcmDataMode 0x00: DstBuffer[]= 0 0 ... 0(half)0 0 ... 0(half)
 *                           fill zero into buf
 *                     0x01: DstBuffer[]= L L ... L 
 *                           only read left ch pcm data into buf
 *                     0x02: DstBuffer[]= R R ... R
 *                           only read right ch pcm data into buf
 *                     0x03: DstBuffer[]= L R L R ... L R
 *                           read both ch pcm data into buf
 *                     0x04: DstBuffer[]= L L ... L(half)R R ... R(half)
 *                           read Left ch pcm data into top half buf, read Left ch pcm data into bottom half buf
 * @return The actual read data length
 */
uint32_t I2sPcmDataRead(uint8_t* Buffer, uint32_t NumSamplesRequired, uint32_t PcmDataMode);

/**
 * @brief  I2s write pmem point get
 * @param  None
 * @return I2s write pmem point
 */
uint16_t I2SCurrWritePointGet(void);

#ifdef  __cplusplus
}
#endif//__cplusplus

#endif//__BSP_I2S_H__


/**
 * @}
 * @}
 */
