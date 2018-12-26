/******************************************************************************
 * @file    get_bootup_info.h
 * @author  Robert
 * @version V1.0.0
 * @date    2014-09-10
 * @maintainer lujiangang
 * @brief   get information from boot header 
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; COPYRIGHT 2013 MVSilicon </center></h2>
 */
#ifndef __GET_BOOTUP_INFO_H__
#define __GET_BOOTUP_INFO_H__

#ifdef __cplusplus
extern "C" {
#endif//__cplusplus

/**
 * @brief  Get POR update disable state.0:enable,1:disable
 * @param  None
 * @return POR update disable state
 */
extern bool PorUpdateDisable(void);


/**
 * @brief  External Crystal configure.0:32.768k,1~30:1M~30M
 * @param  None
 * @return External Crystal configure
 */
extern uint8_t ExCrystalConfig(void);


/**
 * @brief  Update device configure
 * @param  None
 * @return Update device configure
 */
extern uint8_t UpdateDevConfig(void);

/**
 * @brief  Get remap tpye.0:OTP only,1:OTP+FLASH,2:OTP+SDRAM,3:FLASH ONLY
 * @param  None
 * @return Remap type
 */
extern uint8_t GetRemapType(void);

/**
 * @brief  Get chip type.0x12 discribes O18.
 * @param  None
 * @return Chip type
 */
extern uint8_t GetChipType(void);

/**
 * @brief  Get patch version
 * @param  None
 * @return Patch version
 */
extern uint8_t GetPatchVersion(void);
/**
 * @brief  Get Boot version
 * @param  None
 * @return Boot version
 */
extern uint8_t GetBootVersion(void);
/**
 * @brief  Get Boot extend version
 * @param  None
 * @return Boot extend version
 */
extern uint8_t GetBootVersionEx(void);
/**
 * @brief  Get Sdk version
 * @param  None
 * @return Sdk version
 */
extern uint32_t GetSdkVer(void);
/**
 * @brief  Get wakeup source name
 * @param  WakeupSrc wakeup source index 
 * @return wakeup source name
 */
extern const char* GetWakeupSrcName(unsigned int WakeupSrc);
/**
 * @brief  get code size
 * @param  None 
 * @return code size in byte version
 */
extern long GetCodeSize(void);
/**
 * @brief  get code encryption information
 * @param  None 
 * @return code engryption information printf
 */
extern const char* GetCodeEncryptedInfo(void);

/**
 * @brief  Get current spi flash clock frequency
 * @param  None
 * @return spi clk frequency
 */
extern uint32_t ClkGetFshcClkFreq(void);

/**
 * @brief  Set external 32KHz oscillator calibration
 * @param  None
 * @return None
 */
extern void Osc32kExtCapCalibrate(void);

#ifdef  __cplusplus
}
#endif//__cplusplus

#endif

