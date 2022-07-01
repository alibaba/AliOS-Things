/**
 * \file            lwesp_parser.h
 * \brief           Parser of AT responses
 */

/*
 * Copyright (c) 2022 Tilen MAJERLE
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without restriction,
 * including without limitation the rights to use, copy, modify, merge,
 * publish, distribute, sublicense, and/or sell copies of the Software,
 * and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE
 * AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 *
 * This file is part of LwESP - Lightweight ESP-AT parser library.
 *
 * Author:          Tilen MAJERLE <tilen@majerle.eu>
 * Version:         v1.1.2-dev
 */
#ifndef LWESP_HDR_PARSER_H
#define LWESP_HDR_PARSER_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include "lwesp/lwesp.h"

int32_t     lwespi_parse_number(const char** str);
uint8_t     lwespi_parse_string(const char** src, char* dst, size_t dst_len, uint8_t trim);
uint8_t     lwespi_parse_ip(const char** src, lwesp_ip_t* ip);
uint8_t     lwespi_parse_mac(const char** src, lwesp_mac_t* mac);

lwespr_t    lwespi_parse_cipstatus_cipstate(const char* str);
lwespr_t    lwespi_parse_ipd(const char* str);
lwespr_t    lwespi_parse_ciprecvdata(const char* str);
lwespr_t    lwespi_parse_ciprecvlen(const char* str);

uint8_t     lwespi_parse_cwlap(const char* str, lwesp_msg_t* msg);
uint8_t     lwespi_parse_cwjap(const char* str, lwesp_msg_t* msg);
uint8_t     lwespi_parse_cwlif(const char* str, lwesp_msg_t* msg);
uint8_t     lwespi_parse_cipdomain(const char* src, lwesp_msg_t* msg);
uint8_t     lwespi_parse_cipsntptime(const char* str, lwesp_msg_t* msg);
uint8_t     lwespi_parse_ping_time(const char* str, lwesp_msg_t* msg);
uint8_t     lwespi_parse_hostname(const char* str, lwesp_msg_t* msg);
uint8_t     lwespi_parse_link_conn(const char* str);

uint8_t     lwespi_parse_at_sdk_version(const char* str, lwesp_sw_version_t* version_out);

uint8_t     lwespi_parse_ap_conn_disconn_sta(const char* str, uint8_t is_conn);
uint8_t     lwespi_parse_ap_ip_sta(const char* str);
uint8_t     lwespi_parse_cwsap(const char* str, lwesp_msg_t* msg);

uint8_t     lwespi_parse_cwdhcp(const char* str);

uint8_t     lwespi_parse_webserver(const char* str);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* LWESP_HDR_PARSER_H */
