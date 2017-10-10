/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef COTA_H
#define COTA_H

#include "../service.h"

int cota_init(void);
int cota_exit(void);
void alink_attrfilter(char *cJsonStr);
#endif
