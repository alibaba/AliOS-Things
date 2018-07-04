/*
 *  Copyright (C) 2015 YunOS Project. All rights reserved.
 */

#include <stdio.h>
#include <stdarg.h>

void pal_log(const char *fmt, ...)
{
    va_list args;

    va_start(args, fmt);
    vprintf(fmt, args);
    va_end(args);
}

