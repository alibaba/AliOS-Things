/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#ifndef __SYSTEMDIAG_H__
#define __SYSTEMDIAG_H__

#include "../menu.h"

extern MENU_TYP systemDiag;

int systemDiag_init(void);
int systemDiag_uninit(void);

#endif