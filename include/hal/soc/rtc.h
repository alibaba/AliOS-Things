/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef HAL_RTC_H
#define HAL_RTC_H

/*
 * RTC time
 */
typedef struct {
    uint8_t sec;
    uint8_t min;
    uint8_t hr;
    uint8_t weekday;
    uint8_t date;
    uint8_t month;
    uint8_t year;
} rtc_time_t;

typedef struct {
    uint8_t port; /* rtc port */
    void   *priv; /* priv data */
} rtc_dev_t;


/**
 * This function will initialize the on board CPU real time clock
 *
 *
 * @param[in]  rtc  rtc device
 *
 * @return  0 : on success, EIO : if an error occurred with any step
 */
void hal_rtc_init(rtc_dev_t *rtc);

/**
 * This function will return the value of time read from the on board CPU real time clock.
 *
 * @param[in]   rtc   rtc device
 * @param[out]  time  pointer to a time structure
 *
 * @return  0 : on success, EIO : if an error occurred with any step
 */
int32_t hal_rtc_get_time(rtc_dev_t *rtc, rtc_time_t *time);

/**
 * This function will set MCU RTC time to a new value.
 *
 * @param[in]   rtc   rtc device
 * @param[out]  time  pointer to a time structure
 *
 * @return  0 : on success, EIO : if an error occurred with any step
 */
int32_t hal_rtc_set_time(rtc_dev_t *rtc, rtc_time_t *time);

#endif /* HAL_RTC_H */

