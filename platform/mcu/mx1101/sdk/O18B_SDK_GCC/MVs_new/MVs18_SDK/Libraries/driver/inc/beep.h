/**
 *****************************************************************************
 * @file     beep.c
 * @author   Richard
 * @version  V1.0.0
 * @date     19-June-2013
 * @maintainer Sam
 * @brief    beep module driver interface
 * change log:
 *			 Modify by Sam -20140507
 *****************************************************************************
 * @attention
 *
 * <h2><center>&copy; COPYRIGHT 2013 MVSilicon </center></h2>
 */


#ifndef __BEEP_H__
#define	__BEEP_H__

#ifdef __cplusplus
extern "C" {
#endif//__cplusplus

#define BEEP_IS_BUSY               (1)      /**<last beep is not finished*/
#define BEEP_IS_NORMAL             (0)      /**<beep module start work*/

/**
 * @brief  generate sine wave,can adjust beep freq and vol gain.
 * @param  BeepFreq [100Hz, SampleRate/2)
 * @param  VolGain [0~4096)
 * @param  SampleRate nine sample rate
 * @param  OnTime range:[1/BeepFreq, (2^14-1)/BeepFreq] unit:ms 0-continuous mode, >0 signle mode
 * @param  FallTime: [vol_gain/(64*fs) ~ 16*vol_gain/fs] unit:ms
 * @return 1-BEEP_IS_BUSY, 0-BEEP_IS_NORMAL
 */
bool BeepStart(uint16_t BeepFreq, uint16_t VolGain, uint16_t SampleRate, uint16_t OnTime, uint16_t FallTime);

/**
 * @brief  in beep continuous mode,need call this function to exit from continuous mode
 * @param  NONE
 * @return NONE
 */
void BeepStop(void);


#ifdef __cplusplus
}
#endif//__cplusplus

#endif	//__BEEP_H__

