/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <aos/errno.h>
#include "devmgr_common.h"
#include "service.h"

#define IEEE_ADDR_STRING_FMT    "%02x%02x%02x%02x%02x%02x%02x%02x"

int bytes_2_hexstr(const uint8_t *buf, int buf_len, char *hexstr,
                   int hex_buf_len)
{
    int len = 0;
    int index = 0;

    if (hex_buf_len <= buf_len * 2) {
        return SERVICE_RESULT_ERR;
    }

    while (index < buf_len) {
        len += snprintf(hexstr + len, hex_buf_len - len, "%02x", *(buf + index));
        index++;
    }

    *(hexstr + len) = '\0';

    return SERVICE_RESULT_OK;
}


int hexstr_2_bytes(const char *hexstr, uint8_t *buf, int *buf_len)
{
    int len = 0;
    int index = 0;
    unsigned int arr = 0;

    if (strlen(hexstr) / 2 > *buf_len + 1) {
        return SERVICE_RESULT_ERR;
    }

    while (len < strlen(hexstr)) {
        if (1 != sscanf(hexstr, "%02x", &arr )) {
            return SERVICE_RESULT_ERR;
        }

        *(buf + index) = (uint8_t) * (&arr);
        index++;
        len += 2;
    }

    *buf_len = index;
    return SERVICE_RESULT_OK;
}


int get_ieeeaddr_by_string(char *ieeeaddr_string,
                           uint8_t extaddr[IEEE_ADDR_BYTES])
{
    int i;
    unsigned int arr[8] = {0};

    if (NULL == ieeeaddr_string) {
        return SERVICE_RESULT_ERR;
    }

    if (IEEE_ADDR_BYTES != sscanf(ieeeaddr_string, IEEE_ADDR_STRING_FMT,
                                  &arr[0], &arr[1], &arr[2], &arr[3],
                                  &arr[4], &arr[5], &arr[6], &arr[7])) {
        return SERVICE_RESULT_ERR;
    }

    for (i = 0; i < 8; i++) {
        extaddr[i] = (uint8_t)arr[i];
    }

    return SERVICE_RESULT_OK;
}


char *get_ieeeaddr_string_by_extaddr(uint8_t extaddr[IEEE_ADDR_BYTES],
                                     char *ieeeaddr_buf, int buf_len)
{
    int len = 0;
    len = snprintf(ieeeaddr_buf, buf_len, IEEE_ADDR_STRING_FMT, extaddr[0],
                   extaddr[1], extaddr[2],
                   extaddr[3], extaddr[4], extaddr[5], extaddr[6], extaddr[7]);

    if (len == buf_len) {
        return NULL;
    }

    return ieeeaddr_buf;
}


