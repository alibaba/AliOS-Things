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
#ifndef _AWSS_H_
#define _AWSS_H_

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
int awss_start();

/**
 * @brief   stop wifi setup service
 *
 * @retval  -1 : failure
 * @retval  0 : sucess
 * @note
 *      if awss_stop is called before exit of awss_start, awss and notify will stop.
 */
int awss_stop();

/**
 * @brief   make sure user touches devic belong to themselves
 *
 * @retval  -1 : failure
 * @retval  0 : sucess
 */
int awss_config_press();

/**
 * @brief   report token to cloud after wifi setup success
 *
 * @retval  -1 : failure
 * @retval  0 : sucess
 */
int awss_report_cloud();

/**
 * @brief   report reset to cloud when device connects cloud.
 *
 * @retval  -1 : failure
 * @retval  0 : sucess
 * @note
 *      if device dosen't connect cloud, device will fails to send reset to cloud.
 */
int awss_report_reset();

#if defined(__cplusplus)  /* If this is a C++ compiler, use C linkage */
}
#endif

#endif
