/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#ifndef __AWSS_H__
#define __AWSS_H__

#include <stdint.h>

#if defined(__cplusplus)  /* If this is a C++ compiler, use C linkage */
extern "C"
{
#endif

/**
 * @brief   start wifi setup service
 *
 * @retval  -1 : wifi setup fail
 * @retval  0 : sucess
 * @note: awss_report_cloud must been called to enable wifi setup service
 */
int awss_start(void);

/**
 * @brief   stop wifi setup service
 *
 * @retval  -1 : failure
 * @retval  0 : sucess
 * @note
 *      if awss_stop is called before exit of awss_start, awss and notify will stop.
 */
int awss_stop(void);

/**
 * @brief   make sure user touches device belong to themselves
 *
 * @retval  -1 : failure
 * @retval  0 : sucess
 * @note: AWSS doesn't parse awss packet until user touches deivce use this api.
 */
int awss_config_press(void);

/**
 * @brief   get the awss config press status in realtime.
 *
 * @retval  1 : user has touched device
 * @retval  0 : user don't touch device
 */
uint8_t awss_get_config_press(void);
void awss_set_config_press(uint8_t press);

/**
 * @brief   report token to cloud after wifi setup success
 *
 * @retval  -1 : failure
 * @retval  0 : sucess
 */
int awss_report_cloud(void);

#if defined(__cplusplus)  /* If this is a C++ compiler, use C linkage */
}
#endif

#endif
