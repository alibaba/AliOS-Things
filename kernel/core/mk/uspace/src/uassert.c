/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <utask.h>

void uassert_fail(const char *expr, const char *file, int line)
{
    printf("%s:%d assert failed, \"%s\" \r\n", file, line, expr);

    krhino_uprocess_exit();
}

