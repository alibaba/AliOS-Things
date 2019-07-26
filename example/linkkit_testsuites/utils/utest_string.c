/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#include "sdk-testsuites_internal.h"

CASE(UTILS_STRING, hexbuf_convert)
{
    unsigned char       hexbuf[] = { 0x12, 0x34, 0x56, 0x78, 0xcd, 0xef, 0xab, 0x90 };
    char                hexstr[20] = { 0 };

    LITE_hexbuf_convert(hexbuf, hexstr, sizeof(hexbuf), 0);
    log_info("hexstr = '%s'", hexstr);
    ASSERT_STR_EQ(hexstr, "12345678cdefab90");

    LITE_hexbuf_convert(hexbuf, hexstr, sizeof(hexbuf), 1);
    log_info("hexstr = '%s'", hexstr);
    ASSERT_STR_EQ(hexstr, "12345678CDEFAB90");
}

CASE(UTILS_STRING, hexstr_convert)
{
    unsigned char       hexbuf[] = { 0x12, 0x34, 0x56, 0x78, 0xcd, 0xef, 0xab, 0x90 };
    char                hexstr[20] = { 0 };
    unsigned char       outbuf[10] = { 0x0 };
    char                str_uppercase[] = {"1234567890abcdef"};

    LITE_hexbuf_convert(hexbuf, hexstr, sizeof(hexbuf), 1);
    LITE_hexstr_convert(hexstr, sizeof(hexbuf), outbuf, sizeof(outbuf));
    HEXDUMP_DEBUG(outbuf, sizeof(outbuf));

    ASSERT_EQ(outbuf[3], 0x78);

    memset(outbuf, 0, sizeof(outbuf));
    LITE_hexstr_convert(str_uppercase, strlen(str_uppercase), outbuf, sizeof(outbuf));
    HEXDUMP_DEBUG(outbuf, sizeof(outbuf));
    ASSERT_EQ(outbuf[7], 0xef);

    memset(outbuf, 0, sizeof(outbuf));
    LITE_hexstr_convert("1234567", strlen("1234567"), outbuf, sizeof(outbuf));
    ASSERT_EQ(outbuf[0], 0x00);

    memset(outbuf, 0, sizeof(outbuf));
    LITE_hexstr_convert("[]", strlen("[]"), outbuf, sizeof(outbuf));
    ASSERT_EQ(outbuf[0], 0x00);
}

SUITE(UTILS_STRING) = {
    ADD_CASE(UTILS_STRING, hexbuf_convert),
    ADD_CASE(UTILS_STRING, hexstr_convert),
    ADD_CASE_NULL
};

