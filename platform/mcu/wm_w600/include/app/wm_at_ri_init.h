/**
 * @file    wm_at_ri_init.h
 *
 * @brief   AT_RI task and interface resource initial Module
 *
 * @author  winnermicro
 *
 * Copyright (c) 2015 Winner Microelectronics Co., Ltd.
 */

#ifndef WM_AT_RI_H
#define WM_AT_RI_H

/**
 * @defgroup APP_APIs APP APIs
 * @brief APP APIs
 */

/**
 * @addtogroup APP_APIs
 * @{
 */

/**
 * @defgroup AT_RI_APIs AT_RI APIs
 * @brief AT_RI command APIs
 */

/**
 * @addtogroup AT_RI_APIs
 * @{
 */

/**
 * @brief          This function is used to initialize hostif task
 				   used by AT&RI Command
 *
 * @param          None
 *
 * @retval         0     success
 * @retval         other failed
 *
 * @note           None
 */
int tls_hostif_init(void);

/**
 * @brief          This function is used to initialize high speed SPI
 				   used by AT&RI Command
 *
 * @param          None
 *
 * @retval         0     success
 * @retval         other failed
 *
 * @note           None
 */
int tls_hspi_init(void);

/**
 * @brief          This function is used to initialize UART
 				   used by AT&RI Command
 *
 * @param          None
 *
 * @return         None
 *
 * @note           None
 */
void tls_uart_init(void);

/**
 * @}
 */

/**
 * @}
 */

#endif /* WM_AT_RI_H */

