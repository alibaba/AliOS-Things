/*
 * Copyright (C) 2015-2021 Alibaba Group Holding Limited
 */
#ifndef __WIFIDPD_HAAS1000_H__
#define __WIFIDPD_HAAS1000_H__

#ifdef __cplusplus
extern "C" {
#endif

int wifidpd_open(void);
int wifidpd_read(unsigned short reg, unsigned short *val);
int wifidpd_write(unsigned short reg, unsigned short val);

#ifdef __cplusplus
}
#endif

#endif

