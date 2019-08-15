/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#include <stdio.h>

#include "jse_common.h"

unsigned char hex2num(unsigned char ch)
{
    if (ch >= 'a') {
        return ch - 'a' + 10;
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
void hexdump(const char *title, const void *buff, const int len)
{
    int i, j, written;
    unsigned char ascii[16 + 1] = {0};
    char header[64]             = {0};
    unsigned char *buf          = (unsigned char *)buff;

    written = 0;

    printf("%s : \n\r", title);
    for (i = 0; i < len; ++i) {
        if (i % 16 == 0) {
            printf("| %08X: ", (unsigned int)(i + (long)buff));
            written += 8;
        }

        printf("%02X", buf[i]);
        written += 2;

        if (i % 2 == 1) {
            printf(" ");
            written += 1;
        }
        sprintf((char *)ascii + i % 16, "%c",
                ((buf[i] >= ' ' && buf[i] <= '~') ? buf[i] : '.'));

        if (((i + 1) % 16 == 0) || (i == len - 1)) {
            for (j = 0; j < 48 - written; ++j) {
                printf(" ");
            }

            printf(" %s", ascii);
            printf("\r\n");

            written = 0;
            memset(ascii, 0, sizeof(ascii));
        }
    }
    printf("%s\r\n",
           "+"
           "-----------------------"
           "-----------------------"
           "-----------------------");

    return;
}
