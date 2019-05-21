#ifndef __TEMP_DETECT_PUB_H__
#define __TEMP_DETECT_PUB_H__


//#define TMP_DETECT_DEBUG
#ifdef TMP_DETECT_DEBUG
#define TMP_DETECT_PRT      os_printf
#define TMP_DETECT_WARN     warning_prf
#define TMP_DETECT_FATAL    fatal_prf
#else
#define TMP_DETECT_PRT      null_prf
#define TMP_DETECT_WARN     null_prf
#define TMP_DETECT_FATAL    null_prf
#endif

#if (CFG_SOC_NAME == SOC_BK7221U)
#define ADC_TEMP_SENSER_CHANNEL                     8
#else
#define ADC_TEMP_SENSER_CHANNEL                     7
#endif
#define ADC_TEMP_BUFFER_SIZE                        5

#if CFG_USE_TEMPERATURE_DETECT
#define ADC_TMEP_DETECT_INTVAL                      (15)  // 15s  how many second
#define ADC_TMEP_DETECT_INTVAL_INIT                 (1)   // 1s
#define ADC_TMEP_XTAL_INIT                          (60)  // 60s
#define ADC_TMEP_DETECT_INTVAL_CHANGE               (120) // 2 mins

#define ADC_TMEP_DIST_INTIAL_VAL                    (0)
#if (CFG_SOC_NAME == SOC_BK7231)
#define ADC_TMEP_LSB_PER_10DEGREE                   (24)
#else
#define ADC_TMEP_LSB_PER_10DEGREE                   (22)// 7231:24,7231U:22,
#endif
#define ADC_TMEP_10DEGREE_PER_DBPWR                 (1) // 7231:1,7231U:1,
#define ADC_TEMP_VAL_MIN                            (50)
#define ADC_TEMP_VAL_MAX                            (500)

#define ADC_XTAL_DIST_INTIAL_VAL                    (70)

/*******************************************************************************
* Function Declarations
*******************************************************************************/
void temp_detect_change_configuration(UINT32 intval, UINT32 thre, UINT32 dist);
UINT32 temp_detect_init(UINT32 init_val);
UINT32 temp_detect_uninit(void);
void temp_detect_pause_timer(void);
void temp_detect_restart_detect(void);
UINT32 temp_get_detect_time(void);
UINT32 temp_detect_is_opened_saradc(void) ;
UINT32 temp_detect_is_init(void);

#endif

UINT32 temp_single_get_current_temperature(UINT32 *temp_value);

#endif

