/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <string.h>
#include "amp_utils.h"
#include "amp_defines.h"
#include "aos_system.h"

#define MOD_STR "AMP_UTILS"

static AOS_SYSTEM_VERSION aos_version = {0};

unsigned char hex2num(unsigned char ch)
{
    if (ch >= 'a') {
        return ch - 'a' + 10;
    } else if (ch >= 'A') {
        return ch - 'A' + 10;
    }

    return ch - '0';
}

char itoch(int val)
{
    if (val < 10) {
        return (char)('0' + val);
    }

    return (char)('a' + val - 10);
}

void num2hex(unsigned char ch, unsigned char *hex)
{
    hex[0] = itoch(ch / 16);
    hex[1] = itoch(ch % 16);
}

/*****************************************************************************
 *Function:    end_with
 *Description: check if str1 end with str2
 *Input:       str1 and str2
 *Output:      if str1 end with str2,return 1 else return 0
 *****************************************************************************/
int end_with(char *str1, char *str2)
{
    if (!str1 || !str2) {
        return 0;
    }

    int len1 = strlen(str1);
    int len2 = strlen(str2);

    if (len1 >= len2) {
        if (strcmp(str1 + len1 - len2, str2) == 0) {
            return 1;
        }
    }

    return 0;
}

/*****************************************************************************
 *Function:    hexdump
 *Description: print buff with hex style
 *Input:       title: a string, buff: the dest buff for dump,len:the buffer len
 *Output:      none
 *****************************************************************************/
void amp_dump(const char *title, const void *buff, const int len)
{
    int i, j, written;
    unsigned char ascii[16 + 1] = {0};
    // char header[64]             = {0};
    unsigned char *buf          = (unsigned char *)buff;

    written = 0;

    amp_debug(MOD_STR, "%s : ", title);
    for (i = 0; i < len; ++i) {
        if (i % 16 == 0) {
            amp_debug(MOD_STR, "| %08X: ", (unsigned int)(i + (long)buff));
            written += 8;
        }

        amp_debug(MOD_STR, "%02X", buf[i]);
        written += 2;

        if (i % 2 == 1) {
            amp_debug(MOD_STR, " ");
            written += 1;
        }
        sprintf((char *)ascii + i % 16, "%c",
                ((buf[i] >= ' ' && buf[i] <= '~') ? buf[i] : '.'));

        if (((i + 1) % 16 == 0) || (i == len - 1)) {
            for (j = 0; j < 48 - written; ++j) {
                amp_debug(MOD_STR, " ");
            }

            amp_debug(MOD_STR, " %s", ascii);
            amp_debug(MOD_STR, "");

            written = 0;
            memset(ascii, 0, sizeof(ascii));
        }
    }
    amp_debug(MOD_STR, "%s",
              "+"
              "-----------------------"
              "-----------------------"
              "-----------------------");

    return;
}


/**
 * 设置用户JS脚本版本号
 *
*/
void aos_userjs_version_set(char *version)
{
    if (!version)
        return;
    snprintf(aos_version.userjs, sizeof(aos_version.userjs), "%s", version);
    // amp_debug(MOD_STR, "version %s", aos_version.userjs);
}

/**
 * 获取用户的JS脚本版本号
 *
*/
const char *aos_userjs_version_get(void)
{
    // amp_debug(MOD_STR, "version %s", aos_version.userjs);
    return aos_version.userjs;
}

/**
 * 获取用户的JS脚本版本号
 *
*/
const char *aos_app_version_get(void)
{
    const char *amp_version_fmt = "amp-v%s";
    aos_snprintf(aos_version.app, 64, amp_version_fmt, AMP_VERSION_NUMBER);
    // amp_debug(MOD_STR, "version %s", aos_version.app);
    return aos_version.app;
}

/**
 * 获取系统kernel软件版本号，在模组设备上即模组软件版本号
 *
*/
const char *aos_kernel_version_get(void)
{
    memset(aos_version.kernel, 0, AOS_VERSION_LENGTH);
    memcpy(aos_version.kernel, AMP_MODULE_SOFTWARE, strlen(AMP_MODULE_SOFTWARE));
    // amp_debug(MOD_STR, "version %s", aos_version.kernel);
    return aos_version.kernel;
}


/**
 * 获取系统软件版本号，包括JS轻应用软件版本和模组软件版本
 * 用于模组软件不能单独升级、只能和应用软件一起打包升级的场景
 *
*/
const char *aos_system_version_get(void)
{
    const char *amp_version_fmt = "amp-v%s-%s";
    memset(aos_version.system, 0, AOS_VERSION_LENGTH * 2);
    aos_snprintf(aos_version.system, AMP_VERSION_LENGTH, amp_version_fmt, \
                 AMP_VERSION_NUMBER, AMP_MODULE_SOFTWARE);
    // amp_debug(MOD_STR, "version %s", aos_version.system);
    return aos_version.system;
}

/**
 * 系统软件编译时间
 *
*/
const char *aos_system_build_time(void)
{
    const char *amp_version_fmt = "%s, %s";
    memset(aos_version.build_time, 0, 128);

    aos_snprintf(aos_version.build_time, AMP_VERSION_LENGTH, amp_version_fmt, \
                 __DATE__, __TIME__);
    // amp_debug(MOD_STR, "version %s", aos_version.build_time);
    return aos_version.build_time;
}

/**
 * 模组硬件版本
 *
*/
const char *aos_hardware_version_get(void)
{
    memset(aos_version.module_hardware, 0, AOS_VERSION_LENGTH);
    memcpy(aos_version.module_hardware, AMP_MODULE_HARDWARE, strlen(AMP_MODULE_HARDWARE));
    // amp_debug(MOD_STR, "version %s", aos_version.module_hardware);
    return aos_version.module_hardware;
}

