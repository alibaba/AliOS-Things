/**
 * @file    wm_ntp.h
 *
 * @brief   ntp module
 *
 * @author  dave
 *
 * Copyright (c) 2015 Winner Microelectronics Co., Ltd.
 */

#ifndef WM_NTP_H
#define WM_NTP_H

#include "wm_type_def.h"

/**
 * @defgroup APP_APIs APP APIs
 * @brief APP APIs
 */

/**
 * @addtogroup APP_APIs
 * @{
 */

/**
 * @defgroup NTP_APIs NTP APIs
 * @brief NTP APIs
 */

/**
 * @addtogroup NTP_APIs
 * @{
 */

/**
 * @brief          This function is used to get network time.
 *
 * @param          None
 *
 * @retval         time value
 *
 * @note           None
 */
u32 tls_ntp_client(void);

/**
 * @brief          This function is used to set ntp server ipaddr.
 *
 * @param[in]      *ipaddr      xxx.xxx.xxx.xxx
 * @param[in]      server_no    max num is four
 *
 * @retval         WM_SUCCESS     success
 * @retval         WM_FAILED failed
 *
 * @note           None
 */
int tls_ntp_set_server(char *ipaddr, int server_no);

/**
 * @}
 */

/**
 * @}
 */

#endif /* WM_NTP_H */

