/**
 *****************************************************************************
 * @file     audio_path.h
 * @author   Ingrid
 * @version  V1.0.0
 * @date     25-June-2013
 * @maintainer: YunWang
 * @brief    declare phub module driver interface
 *****************************************************************************
 * @attention
 *
 * <h2><center>&copy; COPYRIGHT 2013 MVSilicon </center></h2>
 */


#ifndef __AUDIO_PATH_H__
#define	__AUDIO_PATH_H__

#ifdef __cplusplus
extern "C" {
#endif//__cplusplus

/**
 * typical path for application
 */
typedef enum _PHUB_PATH_SEL
{
    /**<single-in,single-out*/
    PCMFIFO_TO_IISOUT = 0x40,
    PCMFIFO_MIX_DACOUT = 0x114,
    IISIN_TO_DACOUT = 0x212,		                /**<i2s in via mix to dac out*/
    ADCIN_TO_PMEM = 0x080,		                    /**<adc in not via mix to dac out*/
    BEEP_TO_IISOUT = 0x008,
    BEEP_TO_DACOUT = 0x110,
    ADCIN_MIX_DACOUT = 0x311,

    /**<single-in,multi-out*/
    PCMFIFO_TO_IISOUT_DACOUT = 0x01C,		        /**<pcm data via mix to dac out and i2s out*/
    BEEP_TO_IIS_DACOUT = 0x018,

    /**<multi-in,single-out*/
    PCMFIFO_IISIN_MIX_DACOUT = 0x216,
    PCMFIFO_IISIN_MIX_PMEM = 0x226,		            /**<mixed recording*/
    PCMFIFO_ADCIN_MIX_PMEM = 0x325,			        /**<mixed recording*/
    PCMFIFO_ADCIN_MIX_IISOUT = 0x00D,			    /**<mixed play*/
    PCMFIFO_ADCIN_MIX_DACOUT = 0x315,               /**<mixed play*/

    /**<multi-in,multi-out*/
    ADCIN2PMEM_PCMFIFO2IISOUT = 0x2C0,
    ADCIN2PMEM_PCMFIFO2DACOUT = 0x194,		        /**<recording and play*/
    ADCIN2PMEM_PCMFIFOBEEP2_IISOUT = 0x08C,
	ADCIN2PMEM_PCMFIFO2DACOUTI2SOUT = 0x09C,
    ADCIN2PMEM_IISIN2DACOUT = 0x292,                /**<recording and play*/
    PCMFIFO_ADCIN_MIX_TO_DACOUT_PMEM = 0x335,	      /**<mixed recording and mixed play*/
    PCMFIFO_IISIN_MIX_TO_DACOUT_PMEM = 0x236,	      /**<mixed recording and mixed play*/
    ADCIN2PMEM_IISIN2DACOUT_PCMFIFO2IISOUT = 0x2D2,	/**<AEC application*/
    IISIN2DACOUT_ADCIN2IISOUT = 0x212,	            /**<conversstion application, like bluetooth, config is the same as IISIN_TO_DACOUT, due to adc module can connect i2s directly*/
    ALL_PATH_CLOSE = 0x0
} PHUB_PATH_SEL;


/**
 * @brief  codec data path config
 * @Param  PhubPath -> codec data path.
 * @Return TRUE AND FALSE.
 */
bool PhubPathSel(PHUB_PATH_SEL PhubPath);

/**
 * @brief  codec data path close.
 * @Param  PhubPath -> codec data path.
 * @Return TRUE AND FALSE.
 */
void PhubPathClose(PHUB_PATH_SEL PhubPath);

/**
 * @brief  ofifo in gain config.
 * @Param  LGain ofifo left channel gain.
 * @param  RGain ofifo right channel gain.
 * @Return None
 */
void PhubOfifoInGainSet(uint16_t LGain, uint16_t RGain);

/**
 * @brief  ADC & I2S in gain config.
 * @Param  LGain left channel gain.
 * @param  RGain right channel gain.
 * @Return None
 */
void PhubAdcI2sInGainSet(uint16_t LGain, uint16_t RGain);


#ifdef  __cplusplus
}
#endif//__cplusplus

#endif	//__PHUB_H__

