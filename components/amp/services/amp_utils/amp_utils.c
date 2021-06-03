/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <string.h>

#include "amp_defines.h"
#include "aos_system.h"

#define MOD_STR "AMP_UTILS"

unsigned char hex2num(unsigned char ch)
{
    if (ch >= 'a') {
        return ch - 'a' + 10;
    }
    else if (ch >= 'A') {
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
void amp_hexdump(const char *title, const void *buff, const int len)
{
    int i, j, written;
    unsigned char ascii[16 + 1] = {0};
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

int amp_version_get(char *version)
{
    const char *amp_version_fmt = "amp-v%s-g%s-%s-%s";

    aos_snprintf(version, AMP_VERSION_LENGTH, amp_version_fmt, \
                                  AMP_VERSION_NUMBER, AMP_GIT_COMMIT, AMP_MODULE_HARDWARE, \
                                  AMP_MODULE_SOFTWARE);
    return 0;
}

int amp_app_version_get(char *version)
{
    const char *amp_version_fmt = "amp-v%s";
    aos_snprintf(version, 64, amp_version_fmt, AMP_VERSION_NUMBER);
    return 0;
}
