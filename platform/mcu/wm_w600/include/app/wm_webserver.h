/**
 * @file    wm_webserver.h
 *
 * @brief   WEB SERVER
 *
 * @author  winnermicro
 *
 * Copyright (c) 2015 Winner Microelectronics Co., Ltd.
 */

#ifndef __WEBSERVER_H__
#define __WEBSERVER_H__

/**
 * @defgroup APP_APIs APP APIs
 * @brief APP APIs
 */

/**
 * @addtogroup APP_APIs
 * @{
 */

/**
 * @defgroup WEB_APIs WEB APIs
 * @brief WEB server APIs
 */

/**
 * @addtogroup WEB_APIs
 * @{
 */

/**
 * @brief          This function is used to start WEB SERVER service
 *
 * @param[in]      None
 *
 * @return         None
 *
 * @note           None
 */
void tls_webserver_init(void);

/**
 * @}
 */

/**
 * @}
 */

#endif /*__WEBSERVER_H__*/

