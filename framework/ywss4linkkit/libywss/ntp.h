/*
 * Copyright (c) 2014-2016 Alibaba Group. All rights reserved.
 *
 * Alibaba Group retains all right, title and interest (including all
 * intellectual property rights) in and to this computer program, which is
 * protected by applicable intellectual property laws.  Unless you have
 * obtained a separate written license from Alibaba Group., you are not
 * authorized to utilize all or a part of this computer program for any
 * purpose (including reproduction, distribution, modification, and
 * compilation into object code), and you must immediately destroy or
 * return to Alibaba Group all copies of this computer program.  If you
 * are licensed by Alibaba Group, your rights to utilize this computer
 * program are limited by the terms of that license.  To obtain a license,
 * please contact Alibaba Group.
 *
 * This computer program contains trade secrets owned by Alibaba Group.
 * and, unless unauthorized by Alibaba Group in writing, you agree to
 * maintain the confidentiality of this computer program and related
 * information and to not disclose this computer program and related
 * information to any other person or entity.
 *
 * THIS COMPUTER PROGRAM IS PROVIDED AS IS WITHOUT ANY WARRANTIES, AND
 * Alibaba Group EXPRESSLY DISCLAIMS ALL WARRANTIES, EXPRESS OR IMPLIED,
 * INCLUDING THE WARRANTIES OF MERCHANTIBILITY, FITNESS FOR A PARTICULAR
 * PURPOSE, TITLE, AND NONINFRINGEMENT.
 */
#ifndef NTP_H
#define NTP_H

#if defined(__cplusplus)  /* If this is a C++ compiler, use C linkage */
extern "C"
{
#endif


#define TOPIC_NTP                       "/ext/ntp/%s/%s/request"
#define TOPIC_NTP_REPLY                 "/ext/ntp/%s/%s/response"

#define NTP_TIME_STR_MAX_LEN            (20)

int linkkit_ntp_time_request(void (*)(const char *ntp_offset_time_ms));
int linkkit_ntp_time_reply(char *topic, int topic_len, void *payload, int payload_len, void *ctx);

#if defined(__cplusplus)  /* If this is a C++ compiler, use C linkage */
}
#endif

#endif
