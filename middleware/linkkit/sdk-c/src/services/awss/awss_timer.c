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

#include <stdlib.h>
#include "os.h"
#include "awss_timer.h"

#if defined(__cplusplus)  /* If this is a C++ compiler, use C linkage */
extern "C"
{
#endif

int awss_stop_timer(void *timer)
{
    if (timer == NULL)
        return 0;
    HAL_Timer_Stop(timer);
    HAL_Timer_Delete(timer);
    return 0;
}
#if 0
int awss_start_timer(void **timer, const char *name, void *func, void *user_data, int ms)
{
    if (timer == NULL)
        return -1;
    *timer = HAL_Timer_Create(name, (void (*)(void *))func, user_data);
    if (*timer == NULL)
        return -1;
    HAL_Timer_Stop(*timer);
    HAL_Timer_Start(*timer, ms);
    return 0;
}
#endif
#if defined(__cplusplus)  /* If this is a C++ compiler, use C linkage */
}
#endif
