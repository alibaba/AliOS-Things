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
#ifndef _SYS_CONFIG_H_
#define _SYS_CONFIG_H_

#include "iot_import.h"
#include "iot_export.h"

#define CONFIG_MAGIC_STR        "Alink+"
#define CONFIG_VERSION          0x01
#define CONFIG_LEN              ((long)(&((device_config_t  *)0)->crc))

typedef struct device_config {
    char magic[8];
    char firmware_version[FIRMWARE_VERSION_MAXLEN];
    char ota_version[FIRMWARE_VERSION_MAXLEN];
    uint16_t crc;
} device_config_t;

extern device_config_t main_config;

int config_init(void);
void config_exit(void);
int config_factory();
int config_update(void);

int config_set_fw_version(char *buffer);
char *config_get_fw_version(void);

int config_set_ota_version(char *buffer);
char *config_get_ota_version(void);

#endif
