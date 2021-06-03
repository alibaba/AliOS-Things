/*
 * Copyright (C) 2017 XRADIO TECHNOLOGY CO., LTD. All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *    1. Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *    2. Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the
 *       distribution.
 *    3. Neither the name of XRADIO TECHNOLOGY CO., LTD. nor the names of
 *       its contributors may be used to endorse or promote products derived
 *       from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 *  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef _NET_SNTP_H_
#define _NET_SNTP_H_

#include <stdint.h>
#include "lwip/sockets.h"

#ifdef __cplusplus
extern "C" {
#endif

/** @defgroup sntp_api sntp
 * @{
 */

#define SNTP_PORT                     123
#define SNTP_SUPPORT_MULTIPLE_SERVERS 1
#if SNTP_SUPPORT_MULTIPLE_SERVERS
#define SNTP_MAX_SERVERS              3
#endif
#ifndef SNTP_RECV_TIMEOUT
#define SNTP_RECV_TIMEOUT             3000 /* ms */
#endif
#define SNTP_RETRY_TIMEOUT            SNTP_RECV_TIMEOUT
#define SNTP_RETRY_TIMES              3

#define SNTP_SERVER_ADDRESS          "pool.ntp.org"

typedef struct {
	char *server_name;    /* remote server name, if this is not NULL, this will be preferred. */
	int recv_timeout;     /* the receive timeout from ntp server */
	uint8_t retry_times;  /* the retry times when receiver timeout */
} sntp_arg;

typedef struct {
        uint8_t sec;       /**< Seconds after the minute   - [0,59]  */
        uint8_t min;       /**< Minutes after the hour     - [0,59]  */
        uint8_t hour;      /**< Hours after the midnight   - [0,23]  */
        uint8_t day;       /**< Day of the month           - [1,31]  */
        uint8_t mon;       /**< Months                     - [1,12]  */
        uint8_t week;      /**< Days in a week             - [0,6]   */
        uint8_t year;      /**< Years                      - [0,127] */
} sntp_time;

int sntp_request(void *arg);
sntp_time *sntp_obtain_time(void);

int sntp_get_time(sntp_arg *arg, struct timeval *ntp_time);
#if SNTP_SUPPORT_MULTIPLE_SERVERS
int sntp_set_server(uint8_t idx, char *server_name);
#endif

/*
* check if last sntp query is OK
* @param: None
* @retval: 0 success -1 fail
*/
int sntp_last_query_status(void);

/*
* set system calender time directly
* @param: current time in millisecond
* @retval: 0 success -1 fail
*/
int sntp_set_time_direct(long long now_ms);

#ifdef USE_RECORD_CALENDAR_TIME
/*
* update calendar time from file
* @param: none
* @retval: none
*/
void update_calendar_time_from_file(void);

/*
* record calendar time to file
* @param: current time in millisecond
* @retval: none
*/
void record_calendar_time(long long ms);
#endif

#ifdef __cplusplus
}
#endif

#endif /* _NET_SNTP_H_ */

