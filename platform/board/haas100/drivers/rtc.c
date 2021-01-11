/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#include "aos/hal/rtc.h"
#include "rx8130ce.h"


/**
 * This function will initialize the on board CPU real time clock
 *
 *
 * @param[in]  rtc  rtc device
 *
 * @return  0 : on success, EIO : if an error occurred with any step
 */
int32_t hal_rtc_init(rtc_dev_t *rtc)
{
    int ret = 0;

    ret = rx8130ce_init();
    if(ret)
    {
        printf("board rtc init fail\r\n");
        return -1;
    }

	return 0;
}

/**
 * This function will return the value of time read from the on board CPU real time clock.
 *
 * @param[in]   rtc   rtc device
 * @param[out]  time  pointer to a time structure
 *
 * @return  0 : on success, EIO : if an error occurred with any step
 */
int32_t hal_rtc_get_time(rtc_dev_t *rtc, rtc_time_t *time)
{
    int ret = 0;

    ret = rx8130ce_get_time(time, sizeof(rtc_time_t));

    if(ret)
    {
        printf("board rtc get time fail\r\n");
        return -1;
    }

	return 0;
}

/**
 * This function will set MCU RTC time to a new value.
 *
 * @param[in]   rtc   rtc device
 * @param[out]  time  pointer to a time structure
 *
 * @return  0 : on success, EIO : if an error occurred with any step
 */
int32_t hal_rtc_set_time(rtc_dev_t *rtc, const rtc_time_t *time)
{
    int ret = 0;

    ret = rx8130ce_set_time(time, sizeof(rtc_time_t));

    if(ret)
    {
        printf("board rtc set time fail\r\n");
        return -1;
    }

	return 0;
}

/**
 * De-initialises an RTC interface, Turns off an RTC hardware interface
 *
 * @param[in]  RTC  the interface which should be de-initialised
 *
 * @return  0 : on success, EIO : if an error occurred with any step
 */
int32_t hal_rtc_finalize(rtc_dev_t *rtc)
{
	return 0;
}

