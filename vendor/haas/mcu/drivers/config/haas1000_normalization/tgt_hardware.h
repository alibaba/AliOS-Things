/*
 * Copyright (C) 2015-2021 Alibaba Group Holding Limited
 */
#ifndef __TGT_HARDWARE__
#define __TGT_HARDWARE__

#ifdef __cplusplus
extern "C" {
#endif

#include "hal_iomux.h"
#include "hal_gpio.h"
#include "hal_key.h"
#include "hal_aud.h"

//config hwardware codec iir.
#define EQ_HW_DAC_IIR_LIST_NUM              1
#define EQ_HW_ADC_IIR_LIST_NUM              1
#define EQ_HW_IIR_LIST_NUM                  1
#define EQ_SW_IIR_LIST_NUM                  8
#define EQ_HW_FIR_LIST_NUM                  3
#ifdef AUDIO_PROCESS_CFG_DIFF_FOR_ODM
#define DRC_CFG_NUM                         8
#define DRC_2_CFG_NUM                       8
#endif

//pwl
#define CFG_HW_PLW_NUM (0)
extern const struct HAL_IOMUX_PIN_FUNCTION_MAP cfg_hw_pinmux_pwl[CFG_HW_PLW_NUM];

#ifdef CFG_HW_ALI_MODULE
//adckey define
#define CFG_HW_ADCKEY_NUMBER 3
#else
#define CFG_HW_ADCKEY_NUMBER 2
#endif

#define CFG_HW_ADCKEY_BASE 0
#define CFG_HW_ADCKEY_ADC_MAXVOLT 1000
#define CFG_HW_ADCKEY_ADC_MINVOLT 0
#define CFG_HW_ADCKEY_ADC_KEYVOLT_BASE -1
extern const uint16_t CFG_HW_ADCKEY_MAP_TABLE[CFG_HW_ADCKEY_NUMBER];
extern const uint16_t CFG_HW_ADCKEY_VOL_TABLE[CFG_HW_ADCKEY_NUMBER];

#define BTA_AV_CO_SBC_MAX_BITPOOL  31

//gpiokey define
#ifdef IS_MULTI_AI_ENABLED
//#ifdef CFG_HW_ALI_CONTRA
//#define CFG_HW_GPIOKEY_NUM (4)
//#elif defined(CFG_HW_ALI_SUPER_MARIE)
#define CFG_HW_GPIOKEY_NUM (5)
//#else
//#define CFG_HW_GPIOKEY_NUM (2)
//#endif

#else

//#ifdef CFG_HW_ALI_CONTRA
//#define CFG_HW_GPIOKEY_NUM (2)
//#elif defined(CFG_HW_ALI_SUPER_MARIE)
#define CFG_HW_GPIOKEY_NUM (3)
//#else
//#define CFG_HW_GPIOKEY_NUM (0)
//#endif
#endif
extern const struct HAL_KEY_GPIOKEY_CFG_T cfg_hw_gpio_key_cfg[CFG_HW_GPIOKEY_NUM];

// ANC function key
#define ANC_FUNCTION_KEY                    HAL_KEY_CODE_PWR

// ANC coefficient curve number
#define ANC_COEF_NUM                        (1)

//#define ANC_TALK_THROUGH

#ifdef ANC_TALK_THROUGH
#define ANC_COEF_LIST_NUM                   (ANC_COEF_NUM + 1)
#else
#define ANC_COEF_LIST_NUM                   (ANC_COEF_NUM)
#endif

#define ANC_FF_MIC_CH_L                     AUD_CHANNEL_MAP_CH0
#define ANC_FF_MIC_CH_R                     AUD_CHANNEL_MAP_CH1
#define ANC_FB_MIC_CH_L                     AUD_CHANNEL_MAP_CH2
#define ANC_FB_MIC_CH_R                     AUD_CHANNEL_MAP_CH3

// audio codec
#define CFG_HW_AUD_INPUT_PATH_NUM           3
#define CFG_HW_AUD_INPUT_PATH_MAINMIC_DEV   (AUD_CHANNEL_MAP_CH0 | AUD_CHANNEL_MAP_CH1 | AUD_CHANNEL_MAP_CH2 | AUD_VMIC_MAP_VMIC1)
#define CFG_HW_AUD_INPUT_PATH_LINEIN_DEV    (AUD_CHANNEL_MAP_CH0 | AUD_CHANNEL_MAP_CH1)
#define CFG_HW_AUD_INPUT_PATH_ASRMIC_DEV    (AUD_CHANNEL_MAP_CH0 | AUD_VMIC_MAP_VMIC1)
extern const struct AUD_IO_PATH_CFG_T cfg_audio_input_path_cfg[CFG_HW_AUD_INPUT_PATH_NUM];

#define CFG_HW_AUD_OUTPUT_PATH_SPEAKER_DEV  (AUD_CHANNEL_MAP_CH1)

#define CFG_HW_AUD_SIDETONE_MIC_DEV         (AUD_CHANNEL_MAP_CH0)
#define CFG_HW_AUD_SIDETONE_GAIN_DBVAL      (-20)

//bt config
extern const char *BT_LOCAL_NAME;
extern const char *BLE_DEFAULT_NAME;
extern uint8_t ble_addr[6];
extern uint8_t bt_addr[6];
extern uint8_t wifi_addr[6];
#define CODEC_SADC_VOL (9)  // index of codec_adc_vol

#ifdef ALIOS_THINGS_RELEASE
/* alios require 100 steps for vol adjusting */
extern const struct CODEC_DAC_VOL_T codec_dac_vol[TGT_VOLUME_LEVEL_QTY + 100 - 16];
/* alios require aec adc analog gain 6 db */
#define ANALOG_ADC_A_GAIN_DB (6)
#else
extern const struct CODEC_DAC_VOL_T codec_dac_vol[TGT_VOLUME_LEVEL_QTY];
#endif

//range -12~+12
#define CFG_HW_AUD_EQ_NUM_BANDS (8)
extern const int8_t cfg_aud_eq_sbc_band_settings[CFG_HW_AUD_EQ_NUM_BANDS];
#define CFG_AUD_EQ_IIR_NUM_BANDS (4)

//battery info
#define APP_BATTERY_MIN_MV (3200)
#define APP_BATTERY_PD_MV   (3100)

#define APP_BATTERY_MAX_MV (4200)

extern const struct HAL_IOMUX_PIN_FUNCTION_MAP app_battery_ext_charger_enable_cfg;
extern const struct HAL_IOMUX_PIN_FUNCTION_MAP app_battery_ext_charger_detecter_cfg;
extern const struct HAL_IOMUX_PIN_FUNCTION_MAP app_battery_ext_charger_indicator_cfg;


#if defined(CFG_HW_ALI_XIAOQIANG)
#define CFG_HW_AUD_OUTPUT_POP_SWITCH HAL_IOMUX_PIN_P0_0
#elif defined(CFG_HW_ALI_CONTRA) || defined(CFG_HW_ALI_SUPER_MARIE)
#define CFG_HW_AUD_OUTPUT_POP_SWITCH HAL_IOMUX_PIN_P1_0
#else
#define CFG_HW_AUD_OUTPUT_POP_SWITCH HAL_IOMUX_PIN_P0_0
#endif

#ifdef __cplusplus
}
#endif

#endif
