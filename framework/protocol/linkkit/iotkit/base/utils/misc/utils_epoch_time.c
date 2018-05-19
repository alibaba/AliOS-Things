/*
 * Copyright (c) 2014-2016 Alibaba Group. All rights reserved.
 * License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#include <stdint.h>
#include "utils_epoch_time.h"
#include "lite-log.h"

#define HTTP_RESP_CONTENT_LEN   (64)
#define ALIYUN_NTP_SERVER       "ntp%d.aliyun.com"
#define ALIYUN_NTP_PORT         (123)

#define LI                      0
#define VN                      3
#define MODE                    3
#define STRATUM                 0
#define POLL                    4
#define PREC                   -6

#define JAN_1970                0x83aa7e80 /* 2208988800 1970 - 1900 in seconds */

/* How to multiply by 4294.967296 quickly (and not quite exactly)
 * without using floating point or greater than 32-bit integers.
 * If you want to fix the last 12 microseconds of error, add in
 * (2911*(x))>>28)
 */
#define NTPFRAC(x)              (4294 * (x) + ((1981 * (x)) >> 11))

/* The reverse of the above, needed if we want to set our microsecond
 * clock (via settimeofday) based on the incoming time in NTP format.
 * Basically exact.
 */
#define USEC(x)                 (((x) >> 12) - 759 * ((((x) >> 10) + 32768) >> 16))

/* Converts NTP delay and dispersion, apparently in seconds scaled
 * by 65536, to microseconds.  RFC1305 states this time is in seconds,
 * doesn't mention the scaling.
 * Should somehow be the same as 1000000 * x / 65536
 */
#define sec2u(x)                ((x) * 15.2587890625)

struct timeval_t {
    uint32_t tv_sec;
    uint32_t tv_usec;
};

struct ntptime_t {
    uint32_t coarse;
    uint32_t fine;
};

struct ntp_packet_t {
    int li;
    int vn;
    int mode;
    int stratum;
    int poll;
    int prec;
    int delay;
    int disp;
    int refid;
};

/**
 * implement of htonl and ntohl
 */
#define BigLittleSwap(A)        ((((uint32_t)(A) & 0xff000000) >> 24) | \
                                (((uint32_t)(A) & 0x00ff0000) >> 8) | \
                                (((uint32_t)(A) & 0x0000ff00) << 8) | \
                                (((uint32_t)(A) & 0x000000ff) << 24))

/* return 1 if big endian */
static int _check_endian(void)
{
    union {
        uint32_t i;
        uint8_t c[4];
    } u;

    u.i = 0x12345678;
    return (0x12 == u.c[0]);
}

static uint32_t _htonl(uint32_t h)
{
    return _check_endian() ? h : BigLittleSwap(h);
}

static uint32_t _ntohl(uint32_t n)
{
    return _check_endian() ? n : BigLittleSwap(n);
}

static int _get_packet(unsigned char *packet, int *len)
{
    uint32_t data[12];
    struct timeval_t now = {0, 0};

    if (*len < 48) {
        log_err("packet buf too short!\n");
        return -1;
    }

    memset(packet, 0, *len);

    data[0] = _htonl((LI << 30) | (VN << 27) | (MODE << 24) |
                     (STRATUM << 16) | (POLL << 8) | (PREC & 0xff));
    data[1] = _htonl(1 << 16);  /* Root Delay (seconds) */
    data[2] = _htonl(1 << 16);  /* Root Dispersion (seconds) */
    data[10] = _htonl(now.tv_sec + JAN_1970); /* Transmit Timestamp coarse */
    data[11] = _htonl(NTPFRAC(now.tv_usec));  /* Transmit Timestamp fine */

    memcpy(packet, data, 48);
    *len = 48;

    return 0;
}

static void _rfc1305_parse_timeval(unsigned char *read_buf, struct timeval_t *tv)
{
/* straight out of RFC-1305 Appendix A */
    struct ntp_packet_t ntp_packet;
    struct ntptime_t xmttime;
#ifdef NTP_DEBUG
    struct ntptime_t reftime, orgtime, rectime;
#endif
    memset(&ntp_packet, 0, sizeof(struct ntp_packet_t));

#define Data(i) _ntohl(((unsigned int *)read_buf)[i])
    ntp_packet.li      = Data(0) >> 30 & 0x03;
    ntp_packet.vn      = Data(0) >> 27 & 0x07;
    ntp_packet.mode    = Data(0) >> 24 & 0x07;
    ntp_packet.stratum = Data(0) >> 16 & 0xff;
    ntp_packet.poll    = Data(0) >>  8 & 0xff;
    ntp_packet.prec    = Data(0)       & 0xff;
    if (ntp_packet.prec & 0x80) ntp_packet.prec |= 0xffffff00;
    ntp_packet.delay   = Data(1);
    ntp_packet.disp    = Data(2);
    ntp_packet.refid   = Data(3);

#ifdef NTP_DEBUG
    reftime.coarse = Data(4);
    reftime.fine   = Data(5);
    orgtime.coarse = Data(6);
    orgtime.fine   = Data(7);
    rectime.coarse = Data(8);
    rectime.fine   = Data(9);
#endif
    xmttime.coarse = Data(10);
    xmttime.fine   = Data(11);
#undef Data

#ifdef NTP_DEBUG
    log_debug("LI=%d  VN=%d  Mode=%d  Stratum=%d  Poll=%d  Precision=%d\n",
              ntp_packet.li, ntp_packet.vn, ntp_packet.mode,
              ntp_packet.stratum, ntp_packet.poll, ntp_packet.prec);
    log_debug("Delay=%.1f  Dispersion=%.1f  Refid=%u.%u.%u.%u\n",
              sec2u(ntp_packet.delay), sec2u(ntp_packet.disp),
              ntp_packet.refid >> 24 & 0xff, ntp_packet.refid >> 16 & 0xff,
              ntp_packet.refid >> 8 & 0xff, ntp_packet.refid & 0xff);
    log_debug("Reference %u.%.6u\n", reftime.coarse - JAN_1970, USEC(reftime.fine));
    log_debug("Originate %u.%.6u\n", orgtime.coarse - JAN_1970, USEC(orgtime.fine));
    log_debug("Receive   %u.%.6u\n", rectime.coarse - JAN_1970, USEC(rectime.fine));
    log_debug("Transmit  %u.%.6u\n", xmttime.coarse - JAN_1970, USEC(xmttime.fine));
#endif

    tv->tv_sec = xmttime.coarse - JAN_1970;
    tv->tv_usec = USEC(xmttime.fine);
}

static uint64_t _get_timestamp_from_ntp(const char *host)
{
    intptr_t fd;  /* socket */
    int ret = -1;
    uint64_t retval = 0;
    struct timeval_t tv;
    unsigned char write_buf[48] = {0};
    int write_len = sizeof(write_buf);
    unsigned char read_buf[1500] = {0};

    fd = HAL_UDP_create(0, 0);
    if (fd < 0) {
        log_err("udp create error!");
        return 0;
    }

    ret = HAL_UDP_connect(fd, host, ALIYUN_NTP_PORT);
    log_info("fd = HAL_UDP_connect('%s':%d) = %d", host, ALIYUN_NTP_PORT, ret);

    ret = _get_packet(write_buf, &write_len);
    if (ret < 0) {
        log_err("get_packet error!");
        goto do_exit;
    }

    ret = HAL_UDP_send(fd, write_buf, write_len, 0);
    if (ret < 0) {
        log_err("udp write error!");
        goto do_exit;
    }

    ret = HAL_UDP_recv(fd, read_buf, sizeof(read_buf), 3000);
    if (ret < 0) {
        log_err("udp read error!");
        goto do_exit;
    }
    _rfc1305_parse_timeval(read_buf, &tv);
    retval = ((uint64_t)tv.tv_sec) * 1000 + tv.tv_usec / 1000;

do_exit:
    HAL_UDP_close(fd);
    return retval;
}

uint64_t utils_get_epoch_time_from_ntp(char copy[], int len)
{
    char ntp_server[20] = {0};
    int ntp_server_index = 1;
    uint64_t time_in_ms = 0;

    for (ntp_server_index = 1; ntp_server_index <= 7; ntp_server_index ++) {
        HAL_Snprintf(ntp_server, 20, ALIYUN_NTP_SERVER, ntp_server_index);
        time_in_ms = _get_timestamp_from_ntp(ntp_server);
        if (time_in_ms > 0) {
            HAL_Snprintf(copy, len, "%" PRIu64, time_in_ms);
            break;
        }
    }

    return time_in_ms;
}

void demo_epoch_time(void)
{
    char            buf[64];
    uint64_t        ret = 0;

    memset(buf, 0, sizeof(buf));
    ret = utils_get_epoch_time_from_ntp(buf, sizeof(buf));

    log_info("ret = utils_get_epoch_time_from_ntp() = %" PRIu64, ret);
    log_info("buf = '%s'", buf);
}

