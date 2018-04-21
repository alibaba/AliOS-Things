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


#include "dev_conf.h"
#include "lite-log.h"
//#include "iot_import_awss.h"

device_config_t main_config;
#define CRC16_SEED   0xFFFF
#define POLY16       0x1021

static int is_init = 0;
static void config_dump(void);

static unsigned short utils_crc16(unsigned char *buf, unsigned int length)
{
    int i;
    unsigned short shift = CRC16_SEED, data = 0, val = 0;

    for (i = 0; i < length; i++) {
        if ((i % 8) == 0) {
            data = (*buf++) << 8;
        }
        val = shift ^ data;
        shift = shift << 1;
        data = data << 1;
        if (val & 0x8000) {
            shift = shift ^ POLY16;
        }
    }
    return shift;
}


int config_init(void)
{
    int ret = 0;

    if (!is_init) {
        memset(&main_config, 0, sizeof(main_config));
        if (!HAL_Config_Read((char *)&main_config, sizeof(main_config))
            && !strncmp(main_config.magic, CONFIG_MAGIC_STR,
                        strlen(CONFIG_MAGIC_STR))
            && main_config.crc == utils_crc16((uint8_t *) &main_config,
                                              CONFIG_LEN)) {
            log_info("config init ok");
        } else {
            log_warning("config init fail, reset...");
            ret = config_factory();
        }

        config_dump();
        is_init = 1;
    }

    return ret;
}

void config_exit(void)
{
    is_init = 0;
}

void config_dump(void)
{
    log_debug("~~~~~dump device config~~~~~");
    log_debug("magic: %s", main_config.magic);
    log_debug("firmware_version: %s", main_config.firmware_version);
    log_debug("crc: %d", main_config.crc);
    log_debug("~~~~~~~~~~~~~~~~~~~~~~~~~~");
}

int config_update(void)
{
    main_config.crc = utils_crc16((uint8_t *) &main_config, CONFIG_LEN);
    return HAL_Config_Write((const char *)&main_config, sizeof(main_config));
}

int config_factory()
{
    memset(&main_config, 0, sizeof(main_config));

    strcpy(main_config.magic, CONFIG_MAGIC_STR);
    HAL_GetFirmwareVesion(main_config.firmware_version);
    main_config.crc = utils_crc16((uint8_t *) &main_config, CONFIG_LEN);
    return config_update();
}

char *config_get_fw_version(void)
{
    if (!is_init) {
        config_init();
    }
    return main_config.firmware_version;
}


