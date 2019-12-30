/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#ifndef _AIS_OS_H_
#define _AIS_OS_H_

#include <stdint.h>

#include "aos/kernel.h"

/**
 * get platform's os info(AliOS Things or non-AliOS Things), and vendor's info.
 * @param[in-out]     info       Device's OS info and chip's info with the format".
 * @param[in-out]     p_len      Length of the OS info and vendor's info.
 *
 * @return  0 on success, negative error on failure.
 */

int os_get_version_chip_info(char* info, uint8_t* p_len);

#endif
