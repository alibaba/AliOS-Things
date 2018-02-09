/*
 * Copyright (C) 2016 YunOS Project. All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
 
#ifndef YUNOS_BSP_TIMER_H
#define YUNOS_BSP_TIMER_H

#include <stdint.h>
#include <bsp/yunos_bsp_errno.h>
#include <time.h>


enum yunos_bsp_rtc_reset_action_e {
  YUNOS_BSP_RTC_RESET_ACTION_CONTINUOUS = 0, ///< RTC continous even after reset
  YUNOS_BSP_RTC_RESET_ACTION_CERTAINVALUE,   ///< Set specfic value after reset
  YUNOS_BSP_RTC_RESET_ACTION_UNCERTAINVALUE, ///< Set unspecfic value after reset
};

/**
* @brief rtc timer int mode
*
* @param :	Timer interrupt mode
**/
enum rtc_every {
    RTC_SEC_INT  = 0x0,    /* SEC INT */
    RTC_MIN_INT  = 0x1,    /* MIN INT */
    RTC_HOUR_INT = 0x2,    /* HOUR INT*/
    RTC_DAY_INT  = 0x3     /* DAY INT */
};

/**
 * @brief  stamp event
 *
 * @param :  Stamp sexternal event
 **/
enum stamp_event {
    EXTERNAL_EVENT_1 = 0x000,   	/** external event 1 */
    EXTERNAL_EVENT_2 = 0x001, 	/** external event 2 */
	EXTERNAL_EVENT_3 = 0x010,		/** external event 3 */
	EXTERNAL_EVENT_4 = 0x011,		/** external event 4 */
	EXTERNAL_EVENT_5 = 0x100,		/** external event 5 */
	EXTERNAL_EVENT_6 = 0x101,		/** external event 6 */
	EXTERNAL_EVENT_7 = 0x110,		/** external event 7 */
	EXTERNAL_EVENT_8 = 0x111,		/** external event 8 */
};

/**
 * @brief  stamp event polarity
 *
 * @param :  stamp event polarity
 **/
enum stamp_event_polarity {
	POSEDGE_EDGE = 0x00,
	NEGEDGE_EDGE = 0x01,
	DUAL_EDGE    = 0x10,
};


/**
 * @brief  stamp id
 *
 * @param :  stamp event polarity
 **/
enum stamp_num {
	TIMSTAMP0 = 0x0,
	TIMSTAMP1 = 0x1,
};

/**
 * @brief  stamp polar register
 *
 * @param :  stamp event polarity
 **/
enum stamp_reg {
	POS_REG = 0x0,
	NEG_REG = 0x1,
};

/**
 * @brief  stamp polar register
 *
 * @param :  stamp event polarity
 **/
enum rtc_enable {
	RTC_DISABLE = 0x0,
	RTC_ENABLE  = 0x1,
};

/**
 * error number related definitions: 0x0000AB00
 */
#define YUNOS_BSP_API_RTC_INIT        0x01
#define YUNOS_BSP_API_RTC_OPEN        0x02
#define YUNOS_BSP_API_RTC_CLOSE       0x03
#define YUNOS_BSP_API_RTC_RESETACTION 0x04
#define YUNOS_BSP_API_RTC_READTIME    0x05
#define YUNOS_BSP_API_RTC_SETTIME     0x06
#define YUNOS_BSP_API_RTC_GETTIMER    0x07
#define YUNOS_BSP_API_RTC_STARTTIMER  0x08
#define YUNOS_BSP_API_RTC_STOPTIMER   0x09

typedef void (*yunos_bsp_rtc_cb_t)(void *arg);

struct yunos_bsp_rtc_timer_info {
  struct timespec *tp;                      ///< the time to set
  yunos_bsp_rtc_cb_t callback;              ///< callback
  void *arg;                                ///< args of callback
};

#ifdef __cplusplus
#define EXTERN extern "C"
extern "C"
{
#else
#define EXTERN extern
#endif
/**
 * Get timestamp date and time.
 *
 * @param[in]	stamp_num	The stamp num to read.
 * @param[in]	stamp_reg	The polarity of register to read.
 * @param[in]	rtctime		The point of time.
 * @return		none
 */
int yunos_bsp_rtc_read_stamp(enum stamp_num stamp_num,
							 enum stamp_reg stamp_reg,
							 struct tm *rtctime);

/**
 * Config the stamp.
 *
 * @param[in]	stamp_num	The number stamp to set.
 * @param[in]	stamp_pol	Varible of stamp polarity.
 * @param[in]	stamp_event	Varible of stamp event.
 * @return		Zero on success; a negated errno value on failure.
 */
int yunos_bsp_rtc_set_stamp(enum stamp_num stamp_num,
							enum stamp_event_polarity stamp_pol,
							enum stamp_event stamp_event);

/**
 * Enable or disable timestap.
 *
 * @param[in]	stamp_num	The number stamp to set.
 * @param[in]	enable	FLAG of enable or disable.
 * @return		Zero on success; a negated errno value on failure.
 */
int yunos_bsp_rtc_set_stamp_enable(enum stamp_num stamp_num,
								   enum rtc_enable enable);

/**
 * Init the rtc, and provide the rtc device count.
 * @param[out]  count   Output pointer which neet the device count.
 * @param[in]   force   true: init even if already iniatlized; otherwise false
 * @return      Zero on success; a negated errno value on failure.
 */
int yunos_bsp_rtc_init(uint8_t *count, bool force);

/**
 * Open the device with id.
 * @param[in]   id      The device index. (Should be less than the device count)
 * @return      Zero on success; a negated errno value on failure.
 */
int yunos_bsp_rtc_open(uint8_t id);

/**
 * Close the device with id.
 * @param[in]   id      The device index. (Should be less than the device count)
 * @return      Zero on success; a negated errno value on failure.
 */
int yunos_bsp_rtc_close(uint8_t id);

/**
 * Get the rtc power up init time with the device id.
 * @param[in]   id      The device index. (Should be less than the device count)
 * @param[in]   action  Output pointer which need the reset action
 * @param[in]   rtctime Output pointer which neet the rtc time.
 * @return      Zero on success; a negated errno value on failure.
 * @note        Only significant when action=YUNOS_BSP_RTC_RESET_ACTION_CERTAINVALUE
 */
int yunos_bsp_rtc_resetaction(uint8_t id,
                              enum yunos_bsp_rtc_reset_action_e *action,
                              struct tm *rtctime);

/**
 * Read the rtc time with the device id.
 * @param[in]   id      The device index. (Should be less than the device count)
 * @param[out]      rtctime Output pointer which neet the rtc time.
 * @return      Zero on success; a negated errno value on failure.
 */
int yunos_bsp_rtc_readtime(uint8_t id, struct tm *rtctime);

/**
 * Set the rtc time with the device id.
 * @param[in]   id      The device index. (Should be less than the device count)
 * @param[in]   rtctime The rtc time.
 * @return      Zero on success; a negated errno value on failure.
 */
int yunos_bsp_rtc_settime(uint8_t id, const struct tm *rtctime);


/**
 * Get the rtc timer id with the device id.
 * @param[in]   id          The device index. (Should be less than the device count)
 * @param[in]   timer_id    Output pointer which neet the timer id.
 * @return      Zero on success; a negated errno value on failure.
 */
int yunos_bsp_rtc_gettimer(uint8_t id, uint8_t *timer_id);


/**
 * Start the rtc timer, call the callback(parm) with indicated timespec.
 * @param[in]   id          The device index. (Should be less than the device count)
 * @param[in]   timer_id    The timer id.
 * @param[in]   timer_info  The pointer that contains timeout actions.
 * @return      Zero on success; a negated errno value on failure.
 */
int yunos_bsp_rtc_starttimer(uint8_t id, uint8_t timer_id,
                             const struct yunos_bsp_rtc_timer_info* timer_info);


/**
 * Stop the rtc timer.
 * @param[in]   id         The device index. (Should be less than the device count)
 * @param[in]   timer_id   The timer id.
 * @return      Zero on success; a negated errno value on failure.
 */
int yunos_bsp_rtc_stoptimer(uint8_t id, uint8_t timer_id);

/**
 * Set alarm and register callback function.
 * @param[in]   rtctime    The time of alarm.
 * @param[in]   callback   Callback function for register.
 * @param[in]   arg        Argument of Callback.
 * @return      Zero on success; a negated errno value on failure.
 */
int yunos_bsp_rtc_startalarm(struct tm *rtctime,yunos_bsp_rtc_cb_t callback,void *arg );

/**
 * Stop alarm.
 * @return      Zero on success; a negated errno value on failure.
 */
int yunos_bsp_rtc_stopalarm(void);

/**
 * Read alarm time.
 * @param[in]   rtctime Point to result of time.
 * @return Standard errno
 */
int yunos_bsp_rtc_get_alarm(struct tm *rtctime);

#undef EXTERN
#ifdef __cplusplus
}
#endif


#endif /* YUNOS_BSP_TIMER_H */
