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

#include <unistd.h>

#include "sntp/sntp.h"
#include "sys/socket.h"
#include "netdb.h"
#include "sys/time.h"
#include "string.h"
#include "stdlib.h"
#include "errno.h"

#include "ulog/ulog.h"

#define TAG            "sntp"

#define SNTP_DEBUG(format, ...)
#define SNTP_INFO(format, ...)     LOGI(TAG, format, ##__VA_ARGS__)
#define SNTP_ERROR(format, ...)    LOGE(TAG, format, ##__VA_ARGS__)
#define SNTP_WRANING(format, ...)  LOGW(TAG, format, ##__VA_ARGS__)

/* SNTP protocol defines */
#define SNTP_MSG_LEN                48
#define SNTP_LI_NO_WARNING          0x00
#define SNTP_VERSION                3 /* NTP Version */
#define SNTP_MODE_MASK              0x07
#define SNTP_MODE_CLIENT            0x03
#define SNTP_MODE_SERVER            0x04
#define SNTP_MODE_BROADCAST         0x05
#define SNTP_STRATUM_KOD            0x00
/* number of seconds between 1900 and 1970 */
#define DIFF_SEC_1900_1970         (2208988800UL)
/* number of seconds between 1970 and Feb 7, 2036 (6:28:16 UTC) (MSB=0) */
#define DIFF_SEC_1970_2036         (2085978496UL)

typedef struct
{
  uint8_t li_vn_mode;	   // Eight bits. li, vn, and mode.
						   // li.	Two bits.	Leap indicator.
						   // vn.	Three bits. Version number of the protocol.
						   // mode. Three bits. Client will pick mode 3 for client.

  uint8_t stratum;		   // Eight bits. Stratum level of the local clock.
  uint8_t poll; 		   // Eight bits. Maximum interval between successive messages.
  uint8_t precision;	   // Eight bits. Precision of the local clock.

  uint32_t rootDelay;	   // 32 bits. Total round trip delay time.
  uint32_t rootDispersion; // 32 bits. Max error aloud from primary clock source.
  uint32_t refId;		   // 32 bits. Reference clock identifier.

  uint32_t refTm_s; 	   // 32 bits. Reference time-stamp seconds.
  uint32_t refTm_f; 	   // 32 bits. Reference time-stamp fraction of a second.

  uint32_t origTm_s;	   // 32 bits. Originate time-stamp seconds.
  uint32_t origTm_f;	   // 32 bits. Originate time-stamp fraction of a second.

  uint32_t rxTm_s;		   // 32 bits. Received time-stamp seconds.
  uint32_t rxTm_f;		   // 32 bits. Received time-stamp fraction of a second.

  uint32_t txTm_s;		   // 32 bits and the most important field the client cares about. Transmit time-stamp seconds.
  uint32_t txTm_f;		   // 32 bits. Transmit time-stamp fraction of a second.
} ntp_packet;			   // Total: 384 bits or 48 bytes.

static sntp_time g_time;

static struct timezone g_timezone = { 0, 0 };

static int g_last_query_status = -1;

// Sets time zone.
void sntp_set_timezone(const struct timezone *tz) {
	if (tz) {
		g_timezone = *tz;
	} else {
		g_timezone.tz_minuteswest = 0;
		g_timezone.tz_dsttime = 0;
	}
}

#if SNTP_SUPPORT_MULTIPLE_SERVERS
typedef struct {
  struct in_addr addr;
} sntp_server_info;

static sntp_server_info sntp_servers[SNTP_MAX_SERVERS];

static const struct in_addr* sntp_get_server_addr(uint8_t idx)
{
	if (idx < SNTP_MAX_SERVERS) {
		return &sntp_servers[idx].addr;
	}
	return INADDR_ANY;
}

static int sntp_set_server_addr(uint8_t idx, struct in_addr *server_addr)
{
	if (idx >= SNTP_MAX_SERVERS)
		return -1;
	sntp_servers[idx].addr.s_addr = server_addr->s_addr;
	return 0;
}

/**
  * @brief Set the remote ntp server address by name.
  * @note
  * @param idx: numdns the index of the NTP server to set must be < SNTP_MAX_SERVERS.
  *        arg: servers name that want to set. such as: "cn.ntp.org.cn" "118.24.4.66"
  * @retval 0:success -1:fail
  */
int sntp_set_server(uint8_t idx, char *server)
{
	struct hostent *host;

	if (idx >= SNTP_MAX_SERVERS)
		return -1;

	host = gethostbyname(server);
	if (!host) {
		SNTP_ERROR("invalid address parameter '%s'",server);
		return -1;
	}

	sntp_set_server_addr(idx, (struct in_addr *)host->h_addr);
	return 0;
}
#endif

static void sntp_init_request_packet(ntp_packet *packet)
{
	struct timeval local_time;

	memset(packet, 0, sizeof(ntp_packet));
	gettimeofday(&local_time, NULL);
	packet->li_vn_mode = SNTP_LI_NO_WARNING | SNTP_VERSION << 3 | SNTP_MODE_CLIENT;
	packet->txTm_s = htonl(local_time.tv_sec);
	packet->txTm_f = htonl(local_time.tv_usec);
}

static int sntp_process_time(ntp_packet *packet, struct timeval *time)
{
	struct timeval local_time;
	uint32_t t0_s, t1_s, t2_s, t3_s, real_time_s;
	uint32_t t0_us, t1_us, t2_us, t3_us, real_time_us;
	int deta1, deta2, transmit_time;

	if (((packet->li_vn_mode & SNTP_MODE_MASK) == SNTP_MODE_SERVER) ||
		((packet->li_vn_mode & SNTP_MODE_MASK) == SNTP_MODE_BROADCAST)) {
		gettimeofday(&local_time, NULL);

		t0_s = ntohl(packet->origTm_s);
		t0_us = ntohl(packet->origTm_f);

		t3_s = local_time.tv_sec;
		t3_us = local_time.tv_usec;

		t1_s =  ntohl(packet->rxTm_s);
		t1_us = ntohl(packet->rxTm_f) / 4295;

		t2_s =  ntohl(packet->txTm_s);
		t2_us = ntohl(packet->txTm_f) / 4295;

		deta1 = (t3_s - t0_s) * 1000000 + (t3_us - t0_us); /* us */
		deta2 = (t2_s - t1_s) * 1000000 + (t2_us - t1_us); /* us */

		transmit_time = (deta1 - deta2) / 2; /* us */

		real_time_s = t2_s + (t2_us + transmit_time) / 1000000;
		real_time_us = (t2_us + transmit_time) % 1000000;

		int is_1900_based = ((real_time_s & 0x80000000) != 0);
		real_time_s = is_1900_based ? (real_time_s - DIFF_SEC_1900_1970) : (real_time_s + DIFF_SEC_1970_2036);
		time->tv_sec = real_time_s + (g_timezone.tz_minuteswest + g_timezone.tz_dsttime * 60) * 60;
		time->tv_usec = real_time_us;
	} else {
		SNTP_ERROR("sntp_request: not response frame code");
		return -1;
	}
	return 0;
}

static int sntp_query_server(sntp_arg *arg, struct timeval *ntp_time)
{
	ntp_packet packet;
	int sockfd = -1;
	struct sockaddr_in server_addr;
	struct hostent *host;
	char *server_name = NULL;
	int ret = 0;
	struct timeval timeout_val;
#if SNTP_SUPPORT_MULTIPLE_SERVERS
	uint8_t idx = 0;
#endif

	memset(&server_addr, 0, sizeof(server_addr));

	/* choose the right server */
	do {
		if (arg && arg->server_name) {
			server_name = arg->server_name;
			break;
		}
#if SNTP_SUPPORT_MULTIPLE_SERVERS
		while(idx < SNTP_MAX_SERVERS && sntp_get_server_addr(idx)->s_addr == INADDR_ANY)
			idx++;
		if (idx < SNTP_MAX_SERVERS) {
			server_name = NULL;
			server_addr.sin_addr.s_addr = sntp_get_server_addr(idx)->s_addr;
			break;
		}
#endif
		server_name = SNTP_SERVER_ADDRESS;
	} while(0);

	if (server_name) {
		host = gethostbyname(server_name);
		if (!host) {
			SNTP_ERROR("invalid address parameter '%s'",server_name);
			ret = -1;
			goto exit;
		}
		server_addr.sin_addr.s_addr  = (*(struct in_addr *)host->h_addr).s_addr;
	}

	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(SNTP_PORT);

	sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (sockfd < 0) {
		SNTP_ERROR("socket create err!");
		ret = -1;
		goto exit;
	}

	int val = 1;
	if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &val, sizeof(int)) != 0) {
		SNTP_ERROR("setsockopt(SO_REUSEADDR) failed");
		ret = -1;
		goto exit;
	}

	int timeout_ms = arg && arg->recv_timeout > 0 ? arg->recv_timeout : SNTP_RECV_TIMEOUT;
	timeout_val.tv_sec = timeout_ms / 1000;
	timeout_val.tv_usec = timeout_ms % 1000 * 1000;
	if (setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &timeout_val, sizeof(struct timeval)) != 0) {
		SNTP_ERROR("setsockopt(SO_RCVTIMEO) failed %d %d", arg->recv_timeout, errno);
		ret = -1;
		goto exit;
	}

#if SNTP_SUPPORT_MULTIPLE_SERVERS
send_request:
#endif
	SNTP_DEBUG("server name:%s", server_name ? server_name : "NULL");
	SNTP_DEBUG("server ip:%s", inet_ntoa(server_addr.sin_addr));

	int retry_times = arg ? arg->retry_times : SNTP_RETRY_TIMES;

	do {
		sntp_init_request_packet(&packet);

		ret = sendto(sockfd, &packet, sizeof(packet), 0, (struct sockaddr *)&server_addr, sizeof(server_addr));
		if (ret <= 0) {
			SNTP_ERROR("send err,ret=%d,errno:%d", ret, errno);
			ret = -1;
			continue;
		}

		ret = recvfrom(sockfd, &packet, sizeof(packet), 0, NULL, NULL);
		if (ret != SNTP_MSG_LEN) {
			if (errno != EAGAIN)
			    SNTP_ERROR("recv err,ret=%d,errno:%d", ret, errno);
			ret = -1;
			continue;
		} else {
			if ((ret = sntp_process_time(&packet, ntp_time)) != 0)
				ret = -1;
			else
				goto exit;
		}
	} while (--retry_times && retry_times > 0);

#if SNTP_SUPPORT_MULTIPLE_SERVERS
	/* try and get the next ntp server */
	if (server_name == NULL) {
		for (idx++; idx < SNTP_MAX_SERVERS; idx++) {
			if (sntp_get_server_addr(idx)->s_addr != INADDR_ANY) {
				server_name = NULL;
				server_addr.sin_addr.s_addr = sntp_get_server_addr(idx)->s_addr;
				goto send_request;
			}
		}
	}
#endif

exit:
	if (sockfd >= 0)
		close(sockfd);
	if (ret)
		return -1;

	return 0;
}

#define MIN_SNTP_SET_INTERVAL_MS 5000
static long long m_sntp_set_time = 0;

/**
  * @brief Get time from the remote server.
  * @note This a blocking interface.
  * @param ntp_time: Pointer to the struct timeval.
  *        arg: The pointer of sntp module parameter
  * @retval 0:success -1:fail
  */
int sntp_get_time(sntp_arg *arg, struct timeval *ntp_time)
{
    int ret = 0;

	ret = sntp_query_server(arg, ntp_time);
    if (ret == 0) {
    	if (m_sntp_set_time == 0 ||
    		aos_now_ms() - m_sntp_set_time > MIN_SNTP_SET_INTERVAL_MS) {
    		aos_calendar_time_set(ntp_time->tv_sec * (uint64_t)1000 + (uint64_t)(ntp_time->tv_usec / 1000));
    	    m_sntp_set_time = aos_now_ms();
        }
    }

    g_last_query_status = ret;

    return ret;
}

/**
 * Send an SNTP request via sockets. This interface has been deprecated.
 * This is a very minimal implementation that does not fully conform
 * to the SNTPv4 RFC, especially regarding server load and error procesing.
 */
int sntp_request(void *arg)
{
	struct timeval ntp_time;
	struct tm *gt;
	int ret;

	ret = sntp_query_server(NULL, &ntp_time);
	if (ret != 0)
		return -1;

	gt = gmtime(&ntp_time.tv_sec);
	if (gt==NULL) {
		return -1;
	}

	g_time.year = gt->tm_year % 100;
	g_time.mon = gt->tm_mon + 1;
	g_time.day = gt->tm_mday;
	g_time.week = gt->tm_wday;
	g_time.hour = gt->tm_hour + 8;
	g_time.min = gt->tm_min;
	g_time.sec = gt->tm_sec;

	return 0;
}

/**
 * obtain time
 * This interface has been deprecated.
 */
sntp_time *sntp_obtain_time(void)
{
	return &g_time;
}

/*
* check if last sntp query is OK
* @param: None
* @retval: 0 success -1 fail
*/
int sntp_last_query_status(void)
{
	return g_last_query_status;
}

/*
* set system calender time directly
* @param: current time in millisecond
* @retval: 0 success -1 fail
*/
int sntp_set_time_direct(long long now_ms)
{
	aos_calendar_time_set(now_ms);

	return 0;
}


