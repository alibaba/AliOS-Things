#ifndef RX8130CE_H
#define RX8130CE_H

/**@defgroup rx8310ce_api
 * @{
 */


#ifdef __cplusplus
extern "C" {
#endif

/* ===== System include ===== */
#include "aos/hal/i2c.h"
#include "aos/hal/rtc.h"
// #include "rtc.h"

/* I2C settings for I2C library */
#ifndef RX8130CE_I2C
#define RX8130CE_I2C 1
#endif

/* RX8130CE I2C clock */
#ifndef RX8130CE_I2C_CLOCK
#define RX8130CE_I2C_CLOCK 100000
#endif

/* I2C slave address for RX8130CE */
#define RX8130CE_I2C_ADDR 0x32

/* Registers location */
#define RX8130CE_SECONDS 0x10
#define RX8130CE_MINUTES 0x11
#define RX8130CE_HOURS 0x12
#define RX8130CE_WEEK 0x13
#define RX8130CE_DAY 0x14
#define RX8130CE_MONTH 0x15
#define RX8130CE_YEAR 0x16
#define RX8130CE_MIN_ALARM 0x17
#define RX8130CE_HOUR_ALARM 0x18
#define RX8130CE_WEEK_DAY_ALARM 0x19
#define RX8130CE_TCNT0 0x1A
#define RX8130CE_TCNT1 0x1B
#define RX8130CE_EXT 0x1C
#define RX8130CE_FLAG 0x1D
#define RX8130CE_CR0 0x1E
#define RX8130CE_CR1 0x1F
#define RX8130CE_DOFFSET 0x30

/* Extension Register (1Ch) bit positions */
#define RX8130CE_EXT_BIT_TSEL (7 << 0)
#define RX8130CE_EXT_BIT_WADA (1 << 3)
#define RX8130CE_EXT_BIT_TE (1 << 4)
#define RX8130CE_EXT_BIT_USEL (1 << 5)
#define RX8130CE_EXT_BIT_FSEL (3 << 6)

/* Flag Register (1Dh) bit positions */
#define RX8130CE_FLAG_BIT_VLF (1 << 1)
#define RX8130CE_FLAG_BIT_AF (1 << 3)
#define RX8130CE_FLAG_BIT_TF (1 << 4)
#define RX8130CE_FLAG_BIT_UF (1 << 5)

/* Control 0 Register (1Еh) bit positions */
#define RX8130CE_CR0_BIT_TSTP (1 << 2)
#define RX8130CE_CR0_BIT_AIE (1 << 3)
#define RX8130CE_CR0_BIT_TIE (1 << 4)
#define RX8130CE_CR0_BIT_UIE (1 << 5)
#define RX8130CE_CR0_BIT_STOP (1 << 6)
#define RX8130CE_CR0_BIT_TEST (1 << 7)

/* Digital offset Register (1Еh) bit positions */
#define RX8130CE_DOFFSET_BIT_DTE (1 << 7)

/* ===== Dev Type Definition ===== */
/* RX8130CE Result enumeration */
typedef enum {
  RX8130CE_RESULT_OK = 0x00,         /*!< Everything OK */
  RX8130CE_RESULT_ERROR,             /*!< An error occurred */
  RX8130CE_RESULT_DEVICENOTCONNECTED /*!< Device is not connected */
} RX8130CE_RESULT_E;

/* RX8130CE Structure for date/time */
typedef struct {
  uint8_t seconds; /*!< Seconds parameter, from 00 to 59 */
  uint8_t minutes; /*!< Minutes parameter, from 00 to 59 */
  uint8_t hours;   /*!< Hours parameter, 24Hour mode, 00 to 23 */
  uint8_t week;    /*!< Day in a week, from 1 to 7 */
  uint8_t day;     /*!< Day in a month, 1 to 31 */
  uint8_t month;   /*!< Month in a year, 1 to 12 */
  uint8_t year;    /*!< Year parameter, 00 to 99, 00 is 2000 and 99 is 2099 */
} RX8130CE_TIME_T;

/**
 * @brief  Enumeration for SQW/OUT pin
 */
typedef enum {
  RX8130CE_OutputFrequency_1Hz =
      0x00, /*!< Set SQW/OUT pin to 1Hz output frequency */
  RX8130CE_OutputFrequency_4096Hz,  /*!< Set SQW/OUT pin to 4096Hz output
                                      frequency */
  RX8130CE_OutputFrequency_8192Hz,  /*!< Set SQW/OUT pin to 8192Hz output
                                      frequency */
  RX8130CE_OutputFrequency_32768Hz, /*!< Set SQW/OUT pin to 32768Hz output
                                      frequency */
  RX8130CE_OutputFrequency_High, /*!< Set SQW/OUT pin high. Because this pin is
                                   open-drain, you will need external pull up
                                   resistor */
  RX8130CE_OutputFrequency_Low   /*!< Set SQW/OUT pin low */
} RX8130CE_OutputFrequency_t;


/* ===== API Lists ===== */
/*********************************************************
 * @fun    rx8130ce_init
 * @breif  rx8130ce initialization
 * @param  none
 * @rtn    0 : on success, EIO : error
 *********************************************************/
int rx8130ce_init(void);

/*********************************************************
 * @fun    rx8130ce_set_time
 * @param[in]  buf      the pointer for rx8130ce time
 * @param[in]  len      the length of time
 * @rtn    0 : on success, EIO : error
 *********************************************************/
int rx8130ce_set_time(const void *buf, uint8_t len);

/*********************************************************
 * @fun    rx8130ce_get_time
 * @param[in]  buf      the pointer for rx8130ce time
 * @param[in]  len      the length of time
 * @rtn    0 : on success, EIO : error
 *********************************************************/
int rx8130ce_get_time(void *buf, uint8_t len);

#ifdef __cplusplus
}
#endif /* __cplusplus */

/**
 * @}
 */
#endif /* RX8130CE_H */
