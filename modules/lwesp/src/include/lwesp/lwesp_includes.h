/**
 * \file            lwesp_includes.h
 * \brief           All main includes
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
#ifndef LWESP_HDR_INCLUDES_H
#define LWESP_HDR_INCLUDES_H

#include "lwesp/lwesp_opt.h"
#include "lwesp/lwesp_typedefs.h"
#include "lwesp/lwesp_buff.h"
#include "lwesp/lwesp_input.h"
#include "lwesp/lwesp_evt.h"
#include "lwesp/lwesp_debug.h"
#include "lwesp/lwesp_utils.h"
#include "lwesp/lwesp_pbuf.h"
#include "lwesp/lwesp_conn.h"
#include "lwesp/lwesp_server.h"
#include "system/lwesp_sys.h"

#if LWESP_CFG_MODE_STATION || __DOXYGEN__
#include "lwesp/lwesp_sta.h"
#endif /* LWESP_CFG_MODE_STATION || __DOXYGEN__ */
#if LWESP_CFG_MODE_ACCESS_POINT || __DOXYGEN__
#include "lwesp/lwesp_ap.h"
#endif /* LWESP_CFG_MODE_ACCESS_POINT || __DOXYGEN__ */
#if LWESP_CFG_NETCONN || __DOXYGEN__
#include "lwesp/lwesp_netconn.h"
#endif /* LWESP_CFG_NETCONN || __DOXYGEN__ */
#if LWESP_CFG_PING || __DOXYGEN__
#include "lwesp/lwesp_ping.h"
#endif /* LWESP_CFG_PING || __DOXYGEN__ */
#if LWESP_CFG_WPS || __DOXYGEN__
#include "lwesp/lwesp_wps.h"
#endif /* LWESP_CFG_WPS || __DOXYGEN__ */
#if LWESP_CFG_SNTP || __DOXYGEN__
#include "lwesp/lwesp_sntp.h"
#endif /* LWESP_CFG_SNTP || __DOXYGEN__ */
#if LWESP_CFG_HOSTNAME || __DOXYGEN__
#include "lwesp/lwesp_hostname.h"
#endif /* LWESP_CFG_HOSTNAME || __DOXYGEN__ */
#if LWESP_CFG_DNS || __DOXYGEN__
#include "lwesp/lwesp_dns.h"
#endif /* LWESP_CFG_DNS || __DOXYGEN__ */
#if LWESP_CFG_SMART || __DOXYGEN__
#include "lwesp/lwesp_smart.h"
#endif /* LWESP_CFG_SMART || __DOXYGEN__ */
#include "lwesp/lwesp_dhcp.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */



#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* LWESP_HDR_INCLUDES_H */
