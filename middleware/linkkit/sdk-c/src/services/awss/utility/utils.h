/*
 * Copyright (c) 2014-2015 Alibaba Group. All rights reserved.
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
#ifndef _UTILS_H_
#define _UTILS_H_

#if defined(__cplusplus) /* If this is a C++ compiler, use C linkage */
extern "C" {
#endif

#include "os.h"
#include "iotx_utils.h"

/**
 * @brief string to hex
 *
 * @param[in] str: input hex string
 * @param[in] str_len: length of input hex string
 * @param[out] out: output hex byte stream
 * @param[in/out] output_len: [in] for output buffer size, [out] for
 *                 output hex byte len
 * @Note None.
 *
 * @retval return num of hex bytes converted, 0 means error.
 */
#define utils_str_to_hex    LITE_hexstr_convert


/**
 * @brief hex to string
 *
 * @param[in] buf: input hex byte stream
 * @param[in] buf_len: input stream length in byte
 * @param[out] str: encoded hex string
 * @param[in/out] str_len: [in] for str buffer size, [out] for
 *                  encoded string length
 * @Note output str buffer is NULL-terminated(if str_buf_len is longer enough)
 *
 * @retval return length of str converted, 0 means error.
 */
#define utils_hex_to_str(buf, buf_len, str, str_buf_len) LITE_hexbuf_convert(buf, str, buf_len, 1)

#if defined(__cplusplus) /* If this is a C++ compiler, use C linkage */
}
#endif

#endif
