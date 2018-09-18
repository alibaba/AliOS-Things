/*
 * Copyright (C) 2018 Alibaba Group Holding Limited
 */

#ifndef __ID_KEY_3DES_H__
#define __ID_KEY_3DES_H__

#if (CHIP_CONST_TEST_KEY)
const char *g_3des_id2 = "0102030405060708090A0B0C";

const uint8_t g_3des_key[] = {
    0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
    0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x10
};
#else
const char *g_3des_id2 = "00AAAAAABBBBBB48B69CD200";

const uint8_t g_3des_key[] = {
    0xBA, 0xA4, 0xCE, 0xE3, 0x1F, 0xFB, 0xF7, 0xDC,
    0x1A, 0xD9, 0xD9, 0x58, 0xAB, 0x57, 0x7F, 0xFB,
};
#endif

#endif

