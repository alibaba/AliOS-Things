/**
 * Copyright (C) 2016 The YunOS Project. All rights reserved.
 */

#include "ali_crypto.h"
#include "ali_crypto_test.h"

#define EN_GCOV_COVERAGE 1

#define TEST_KEY_SIZE (32)
#define TEST_DATA_SIZE (141)
#define TEST_ECB_SIZE (96)
#define TEST_ECB_SIZE_NOALIGN (97)
#define TEST_CBC_SIZE (96)
#define TEST_CBC_SIZE_NOALIGN (97)
#define TEST_CTR_SIZE (141)
#define TEST_CTS_SIZE (141)
#define TEST_XTS_SIZE (141)
#define TEST_CFB_SIZE (141)

static uint8_t _g_aes_key[TEST_KEY_SIZE] = {
    0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b,
    0x0c, 0x0d, 0x0e, 0x0f, 0x0f, 0x0f, 0x0f, 0x0e, 0x0d, 0x0c, 0x0b,
    0x0a, 0x09, 0x08, 0x07, 0x06, 0x05, 0x04, 0x03, 0x02, 0x01
};

static uint8_t _g_aes_iv[AES_IV_SIZE] = {
    0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
    0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
};

static uint8_t _g_test_data[TEST_DATA_SIZE] = {
    0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x01, 0x02, 0x03, 0x04,
    0x05, 0x06, 0x07, 0x08, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
    0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x01, 0x02, 0x03, 0x04,
    0x05, 0x06, 0x07, 0x08, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
    0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x01, 0x02, 0x03, 0x04,
    0x05, 0x06, 0x07, 0x08, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
    0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x01, 0x02, 0x03, 0x04,
    0x05, 0x06, 0x07, 0x08, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
    0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x01, 0x02, 0x03, 0x04,
    0x05, 0x06, 0x07, 0x08, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
    0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x0a, 0x0b, 0x0c, 0x0d,
    0x0e, 0x0f, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x13
};

/* openssl calculated results */
static uint8_t _g_ecb_128_enc[TEST_ECB_SIZE] = {
    0x55, 0x4c, 0x93, 0x50, 0x16, 0x35, 0xaa, 0xdb, 0xad, 0xf8, 0x8b, 0xc6,
    0xd3, 0x6e, 0x57, 0xf7, 0x55, 0x4c, 0x93, 0x50, 0x16, 0x35, 0xaa, 0xdb,
    0xad, 0xf8, 0x8b, 0xc6, 0xd3, 0x6e, 0x57, 0xf7, 0x55, 0x4c, 0x93, 0x50,
    0x16, 0x35, 0xaa, 0xdb, 0xad, 0xf8, 0x8b, 0xc6, 0xd3, 0x6e, 0x57, 0xf7,
    0x55, 0x4c, 0x93, 0x50, 0x16, 0x35, 0xaa, 0xdb, 0xad, 0xf8, 0x8b, 0xc6,
    0xd3, 0x6e, 0x57, 0xf7, 0x55, 0x4c, 0x93, 0x50, 0x16, 0x35, 0xaa, 0xdb,
    0xad, 0xf8, 0x8b, 0xc6, 0xd3, 0x6e, 0x57, 0xf7, 0x55, 0x4c, 0x93, 0x50,
    0x16, 0x35, 0xaa, 0xdb, 0xad, 0xf8, 0x8b, 0xc6, 0xd3, 0x6e, 0x57, 0xf7
};

static uint8_t _g_ecb_128_pkcs_enc[] = {
    0x55, 0x4c, 0x93, 0x50, 0x16, 0x35, 0xaa, 0xdb, 0xad, 0xf8, 0x8b, 0xc6,
    0xd3, 0x6e, 0x57, 0xf7, 0x55, 0x4c, 0x93, 0x50, 0x16, 0x35, 0xaa, 0xdb,
    0xad, 0xf8, 0x8b, 0xc6, 0xd3, 0x6e, 0x57, 0xf7, 0x55, 0x4c, 0x93, 0x50,
    0x16, 0x35, 0xaa, 0xdb, 0xad, 0xf8, 0x8b, 0xc6, 0xd3, 0x6e, 0x57, 0xf7,
    0x55, 0x4c, 0x93, 0x50, 0x16, 0x35, 0xaa, 0xdb, 0xad, 0xf8, 0x8b, 0xc6,
    0xd3, 0x6e, 0x57, 0xf7, 0x55, 0x4c, 0x93, 0x50, 0x16, 0x35, 0xaa, 0xdb,
    0xad, 0xf8, 0x8b, 0xc6, 0xd3, 0x6e, 0x57, 0xf7, 0x55, 0x4c, 0x93, 0x50,
    0x16, 0x35, 0xaa, 0xdb, 0xad, 0xf8, 0x8b, 0xc6, 0xd3, 0x6e, 0x57, 0xf7,
    0x2a, 0xef, 0x35, 0x61, 0x62, 0x58, 0x47, 0x06, 0xe2, 0x9f, 0x96, 0x94,
    0x66, 0x03, 0xdd, 0x66,
};

static uint8_t _g_ecb_192_enc[TEST_ECB_SIZE] = {
    0x7c, 0x73, 0x26, 0x78, 0x4d, 0x71, 0xaf, 0xaf, 0x2b, 0x1a, 0xae, 0x70,
    0xe3, 0x1f, 0x6f, 0x4a, 0x7c, 0x73, 0x26, 0x78, 0x4d, 0x71, 0xaf, 0xaf,
    0x2b, 0x1a, 0xae, 0x70, 0xe3, 0x1f, 0x6f, 0x4a, 0x7c, 0x73, 0x26, 0x78,
    0x4d, 0x71, 0xaf, 0xaf, 0x2b, 0x1a, 0xae, 0x70, 0xe3, 0x1f, 0x6f, 0x4a,
    0x7c, 0x73, 0x26, 0x78, 0x4d, 0x71, 0xaf, 0xaf, 0x2b, 0x1a, 0xae, 0x70,
    0xe3, 0x1f, 0x6f, 0x4a, 0x7c, 0x73, 0x26, 0x78, 0x4d, 0x71, 0xaf, 0xaf,
    0x2b, 0x1a, 0xae, 0x70, 0xe3, 0x1f, 0x6f, 0x4a, 0x7c, 0x73, 0x26, 0x78,
    0x4d, 0x71, 0xaf, 0xaf, 0x2b, 0x1a, 0xae, 0x70, 0xe3, 0x1f, 0x6f, 0x4a
};

static uint8_t _g_ecb_192_pkcs_enc[] = {
    0x7c, 0x73, 0x26, 0x78, 0x4d, 0x71, 0xaf, 0xaf, 0x2b, 0x1a, 0xae, 0x70,
    0xe3, 0x1f, 0x6f, 0x4a, 0x7c, 0x73, 0x26, 0x78, 0x4d, 0x71, 0xaf, 0xaf,
    0x2b, 0x1a, 0xae, 0x70, 0xe3, 0x1f, 0x6f, 0x4a, 0x7c, 0x73, 0x26, 0x78,
    0x4d, 0x71, 0xaf, 0xaf, 0x2b, 0x1a, 0xae, 0x70, 0xe3, 0x1f, 0x6f, 0x4a,
    0x7c, 0x73, 0x26, 0x78, 0x4d, 0x71, 0xaf, 0xaf, 0x2b, 0x1a, 0xae, 0x70,
    0xe3, 0x1f, 0x6f, 0x4a, 0x7c, 0x73, 0x26, 0x78, 0x4d, 0x71, 0xaf, 0xaf,
    0x2b, 0x1a, 0xae, 0x70, 0xe3, 0x1f, 0x6f, 0x4a, 0x7c, 0x73, 0x26, 0x78,
    0x4d, 0x71, 0xaf, 0xaf, 0x2b, 0x1a, 0xae, 0x70, 0xe3, 0x1f, 0x6f, 0x4a,
    0x35, 0x36, 0x60, 0xfa, 0x08, 0xb7, 0x4c, 0x8a, 0x1d, 0x82, 0x90, 0xd8,
    0x63, 0xbb, 0x23, 0xfc,
};

static uint8_t _g_ecb_256_enc[TEST_ECB_SIZE] = {
    0x34, 0xad, 0x17, 0xeb, 0x50, 0xfb, 0x58, 0xda, 0x3b, 0x60, 0x18, 0x18,
    0x0f, 0x70, 0x13, 0x53, 0x34, 0xad, 0x17, 0xeb, 0x50, 0xfb, 0x58, 0xda,
    0x3b, 0x60, 0x18, 0x18, 0x0f, 0x70, 0x13, 0x53, 0x34, 0xad, 0x17, 0xeb,
    0x50, 0xfb, 0x58, 0xda, 0x3b, 0x60, 0x18, 0x18, 0x0f, 0x70, 0x13, 0x53,
    0x34, 0xad, 0x17, 0xeb, 0x50, 0xfb, 0x58, 0xda, 0x3b, 0x60, 0x18, 0x18,
    0x0f, 0x70, 0x13, 0x53, 0x34, 0xad, 0x17, 0xeb, 0x50, 0xfb, 0x58, 0xda,
    0x3b, 0x60, 0x18, 0x18, 0x0f, 0x70, 0x13, 0x53, 0x34, 0xad, 0x17, 0xeb,
    0x50, 0xfb, 0x58, 0xda, 0x3b, 0x60, 0x18, 0x18, 0x0f, 0x70, 0x13, 0x53
};

static uint8_t _g_ecb_256_pkcs_enc[] = {
    0x34, 0xad, 0x17, 0xeb, 0x50, 0xfb, 0x58, 0xda, 0x3b, 0x60, 0x18, 0x18,
    0x0f, 0x70, 0x13, 0x53, 0x34, 0xad, 0x17, 0xeb, 0x50, 0xfb, 0x58, 0xda,
    0x3b, 0x60, 0x18, 0x18, 0x0f, 0x70, 0x13, 0x53, 0x34, 0xad, 0x17, 0xeb,
    0x50, 0xfb, 0x58, 0xda, 0x3b, 0x60, 0x18, 0x18, 0x0f, 0x70, 0x13, 0x53,
    0x34, 0xad, 0x17, 0xeb, 0x50, 0xfb, 0x58, 0xda, 0x3b, 0x60, 0x18, 0x18,
    0x0f, 0x70, 0x13, 0x53, 0x34, 0xad, 0x17, 0xeb, 0x50, 0xfb, 0x58, 0xda,
    0x3b, 0x60, 0x18, 0x18, 0x0f, 0x70, 0x13, 0x53, 0x34, 0xad, 0x17, 0xeb,
    0x50, 0xfb, 0x58, 0xda, 0x3b, 0x60, 0x18, 0x18, 0x0f, 0x70, 0x13, 0x53,
    0x76, 0x06, 0x15, 0x24, 0x41, 0x27, 0x3d, 0x64, 0x91, 0x0b, 0x87, 0x68,
    0xd7, 0x12, 0xbc, 0x67,
};

static uint8_t _g_cbc_128_enc[TEST_ECB_SIZE] = {
    0xe4, 0x57, 0x7c, 0x54, 0x84, 0x50, 0x31, 0x40, 0x6b, 0x6d, 0x76, 0x85,
    0x17, 0xc2, 0x2c, 0x74, 0x9d, 0x24, 0xd4, 0xd8, 0x4e, 0x51, 0x44, 0xc1,
    0x41, 0xcb, 0x62, 0xb5, 0x56, 0x40, 0xd6, 0xe9, 0xc3, 0x36, 0x2e, 0x15,
    0xaf, 0xf1, 0xac, 0xab, 0x09, 0x00, 0x51, 0x7f, 0x1f, 0x15, 0x13, 0x2e,
    0xd3, 0x33, 0x62, 0xa3, 0x7e, 0x28, 0xfd, 0x62, 0x5e, 0xfa, 0x22, 0xad,
    0x7c, 0xd0, 0x03, 0xe4, 0x9f, 0x8b, 0x7f, 0xbf, 0x05, 0xbd, 0x77, 0x23,
    0x75, 0x9d, 0xae, 0x1f, 0x02, 0x9e, 0x35, 0xc3, 0xe0, 0xc3, 0x7b, 0xea,
    0x14, 0xa0, 0x6e, 0x69, 0x1a, 0x2d, 0x66, 0x5a, 0x34, 0x19, 0x49, 0x16,
};

static uint8_t _g_cbc_128_pkcs_enc[] = {
    0xe4, 0x57, 0x7c, 0x54, 0x84, 0x50, 0x31, 0x40, 0x6b, 0x6d, 0x76, 0x85,
    0x17, 0xc2, 0x2c, 0x74, 0x9d, 0x24, 0xd4, 0xd8, 0x4e, 0x51, 0x44, 0xc1,
    0x41, 0xcb, 0x62, 0xb5, 0x56, 0x40, 0xd6, 0xe9, 0xc3, 0x36, 0x2e, 0x15,
    0xaf, 0xf1, 0xac, 0xab, 0x09, 0x00, 0x51, 0x7f, 0x1f, 0x15, 0x13, 0x2e,
    0xd3, 0x33, 0x62, 0xa3, 0x7e, 0x28, 0xfd, 0x62, 0x5e, 0xfa, 0x22, 0xad,
    0x7c, 0xd0, 0x03, 0xe4, 0x9f, 0x8b, 0x7f, 0xbf, 0x05, 0xbd, 0x77, 0x23,
    0x75, 0x9d, 0xae, 0x1f, 0x02, 0x9e, 0x35, 0xc3, 0xe0, 0xc3, 0x7b, 0xea,
    0x14, 0xa0, 0x6e, 0x69, 0x1a, 0x2d, 0x66, 0x5a, 0x34, 0x19, 0x49, 0x16,
    0xcd, 0x55, 0x69, 0x17, 0xfa, 0x5a, 0x85, 0x95, 0x95, 0xa6, 0x29, 0x02,
    0xd7, 0x70, 0x18, 0x1d,
};

static uint8_t _g_cbc_192_enc[] = {
    0xf1, 0xcf, 0x7b, 0x19, 0xf7, 0xd3, 0x70, 0x0b, 0x8e, 0xa2, 0xb7, 0x98,
    0x24, 0x29, 0xad, 0x9f, 0xcf, 0x3f, 0xce, 0x8f, 0x2f, 0xa1, 0x56, 0x1b,
    0xb9, 0x7c, 0xd9, 0x48, 0x1f, 0x51, 0x89, 0x62, 0xa7, 0xb2, 0xeb, 0x04,
    0xff, 0xd1, 0x64, 0x55, 0x1c, 0x5d, 0x52, 0x79, 0x16, 0x6a, 0x5e, 0xfb,
    0xae, 0x76, 0x26, 0xd4, 0x04, 0xfb, 0x04, 0x81, 0x40, 0x9f, 0x75, 0xb7,
    0x9f, 0xaa, 0xba, 0xcb, 0xc1, 0xcd, 0xb9, 0x53, 0x97, 0x3d, 0xc2, 0xba,
    0xd3, 0x85, 0xd0, 0x6d, 0x19, 0x30, 0xcf, 0x78, 0x41, 0x72, 0xee, 0xce,
    0x53, 0x5e, 0xf1, 0x67, 0x4a, 0x04, 0x2d, 0x28, 0x53, 0x42, 0x78, 0xa7,
};

static uint8_t _g_cbc_192_pkcs_enc[] = {
    0xf1, 0xcf, 0x7b, 0x19, 0xf7, 0xd3, 0x70, 0x0b, 0x8e, 0xa2, 0xb7, 0x98,
    0x24, 0x29, 0xad, 0x9f, 0xcf, 0x3f, 0xce, 0x8f, 0x2f, 0xa1, 0x56, 0x1b,
    0xb9, 0x7c, 0xd9, 0x48, 0x1f, 0x51, 0x89, 0x62, 0xa7, 0xb2, 0xeb, 0x04,
    0xff, 0xd1, 0x64, 0x55, 0x1c, 0x5d, 0x52, 0x79, 0x16, 0x6a, 0x5e, 0xfb,
    0xae, 0x76, 0x26, 0xd4, 0x04, 0xfb, 0x04, 0x81, 0x40, 0x9f, 0x75, 0xb7,
    0x9f, 0xaa, 0xba, 0xcb, 0xc1, 0xcd, 0xb9, 0x53, 0x97, 0x3d, 0xc2, 0xba,
    0xd3, 0x85, 0xd0, 0x6d, 0x19, 0x30, 0xcf, 0x78, 0x41, 0x72, 0xee, 0xce,
    0x53, 0x5e, 0xf1, 0x67, 0x4a, 0x04, 0x2d, 0x28, 0x53, 0x42, 0x78, 0xa7,
    0x18, 0x6e, 0x66, 0x9b, 0x7e, 0x4c, 0xb6, 0xf0, 0xa0, 0x00, 0x29, 0x17,
    0x25, 0x72, 0x45, 0x5e,
};

static uint8_t _g_cbc_256_enc[] = {
    0x96, 0xd9, 0x7d, 0x67, 0x72, 0xf8, 0x08, 0x87, 0x87, 0x90, 0xc7, 0x0b,
    0x0d, 0xef, 0xf5, 0xa9, 0x96, 0x55, 0xab, 0x84, 0xee, 0x71, 0x13, 0x10,
    0xe0, 0x22, 0xd1, 0x6a, 0xa3, 0x59, 0x3c, 0x48, 0xbe, 0xd1, 0x72, 0xda,
    0x9e, 0x07, 0x03, 0x58, 0xf0, 0xa9, 0xd3, 0xb4, 0x8f, 0xc0, 0x3f, 0x4a,
    0x81, 0x0b, 0x4a, 0xe0, 0xf4, 0xfc, 0x78, 0x6e, 0x85, 0x05, 0x73, 0x1a,
    0x02, 0xd4, 0x9c, 0xb0, 0xe6, 0x39, 0x7a, 0x79, 0xd8, 0x5d, 0x07, 0xba,
    0xbb, 0x6d, 0x05, 0x1a, 0x40, 0xfd, 0x86, 0xb7, 0x49, 0x07, 0xa4, 0xd5,
    0xe7, 0x1a, 0xe5, 0xcb, 0x9c, 0x57, 0xf8, 0xd5, 0x0c, 0x5e, 0xa2, 0x9b,
};

static uint8_t _g_cbc_256_pkcs_enc[] = {
    0x96, 0xd9, 0x7d, 0x67, 0x72, 0xf8, 0x08, 0x87, 0x87, 0x90, 0xc7, 0x0b,
    0x0d, 0xef, 0xf5, 0xa9, 0x96, 0x55, 0xab, 0x84, 0xee, 0x71, 0x13, 0x10,
    0xe0, 0x22, 0xd1, 0x6a, 0xa3, 0x59, 0x3c, 0x48, 0xbe, 0xd1, 0x72, 0xda,
    0x9e, 0x07, 0x03, 0x58, 0xf0, 0xa9, 0xd3, 0xb4, 0x8f, 0xc0, 0x3f, 0x4a,
    0x81, 0x0b, 0x4a, 0xe0, 0xf4, 0xfc, 0x78, 0x6e, 0x85, 0x05, 0x73, 0x1a,
    0x02, 0xd4, 0x9c, 0xb0, 0xe6, 0x39, 0x7a, 0x79, 0xd8, 0x5d, 0x07, 0xba,
    0xbb, 0x6d, 0x05, 0x1a, 0x40, 0xfd, 0x86, 0xb7, 0x49, 0x07, 0xa4, 0xd5,
    0xe7, 0x1a, 0xe5, 0xcb, 0x9c, 0x57, 0xf8, 0xd5, 0x0c, 0x5e, 0xa2, 0x9b,
    0x23, 0xe5, 0xb1, 0xc2, 0x7e, 0xd8, 0xd4, 0x3f, 0x25, 0x90, 0x29, 0x3b,
    0x8a, 0x9a, 0xb2, 0xee,
};

static uint8_t _g_cfb8_128_enc[] = {
    0x72, 0xa1, 0xd4, 0x70, 0x9b, 0xf9, 0x27, 0xc9, 0x33, 0x5f, 0x98, 0x03,
    0x35, 0xa5, 0x13, 0x63, 0xb4, 0x74, 0x30, 0x07, 0xe7, 0x27, 0xd1, 0x19,
    0xa9, 0x67, 0x48, 0x23, 0x70, 0x1c, 0xce, 0x1c, 0x97, 0x7f, 0xa0, 0x5f,
    0x4a, 0xe8, 0x68, 0x29, 0xe8, 0x41, 0x65, 0xc9, 0xab, 0x24, 0xba, 0x3d,
    0x87, 0x20, 0x88, 0x8f, 0x45, 0x7f, 0x9f, 0x8a, 0xf3, 0x44, 0x5e, 0x44,
    0xb7, 0x42, 0xf0, 0x91, 0x91, 0x11, 0x29, 0xf9, 0x80, 0x64, 0x3e, 0xfe,
    0xeb, 0x5d, 0x5b, 0x71, 0xd1, 0x07, 0x3f, 0xe5, 0x9c, 0x79, 0xd6, 0xda,
    0x2c, 0x48, 0xfc, 0xba, 0xf9, 0x4d, 0x56, 0x29, 0xb8, 0xd5, 0x26, 0x8b,
    0xd5, 0xfd, 0xd5, 0x48, 0xcb, 0xdb, 0x75, 0x01, 0xce, 0x2c, 0x53, 0x6b,
    0xe1, 0x6b, 0xd7, 0x37, 0x5f, 0x5d, 0xb9, 0xa5, 0xcc, 0x0a, 0x26, 0x51,
    0x1a, 0x81, 0xa0, 0x2d, 0x96, 0xd0, 0x2f, 0xb4, 0x19, 0xcb, 0x57, 0x13,
    0x26, 0x33, 0xd9, 0x40, 0x7f, 0xcd, 0x49, 0xf8, 0xde,
};

static uint8_t _g_cfb128_128_enc[] = {
    0x72, 0x29, 0xbe, 0x3a, 0x0a, 0x61, 0x7e, 0xae, 0xca, 0xfa, 0xda, 0xaa,
    0x08, 0x65, 0x50, 0x1e, 0xcb, 0xd3, 0xe6, 0x3e, 0xf5, 0x07, 0xe9, 0x27,
    0x7b, 0x27, 0x21, 0x26, 0xcf, 0x65, 0xdb, 0xf8, 0x48, 0x45, 0xd8, 0x44,
    0xee, 0x4e, 0xed, 0xb3, 0x79, 0xd5, 0x5e, 0xc8, 0x22, 0x27, 0xee, 0x1e,
    0xeb, 0x9a, 0x96, 0x6a, 0x53, 0xc8, 0xed, 0x1e, 0x49, 0xd2, 0x61, 0x9f,
    0x2e, 0x10, 0xd5, 0xfa, 0x7d, 0x66, 0xba, 0xa2, 0x85, 0xdc, 0xe3, 0xab,
    0x1d, 0x46, 0xb6, 0xb5, 0x0f, 0x1f, 0xa3, 0xa2, 0x70, 0x77, 0x24, 0x87,
    0x37, 0xc0, 0x89, 0xab, 0x1a, 0x58, 0x48, 0x02, 0xd5, 0x0e, 0x13, 0x7a,
    0x37, 0x8b, 0x93, 0x9f, 0x2e, 0x36, 0xf4, 0x8a, 0x5a, 0xe4, 0xea, 0x61,
    0xe0, 0xc2, 0xc3, 0x14, 0x30, 0xc0, 0x02, 0x0f, 0x53, 0xd9, 0x37, 0x81,
    0x56, 0x5a, 0xcd, 0x2f, 0x5a, 0x0b, 0xdc, 0x18, 0x20, 0x49, 0x60, 0xb6,
    0x7a, 0x8f, 0x9f, 0xf2, 0x33, 0x5a, 0x51, 0x78, 0xf4,
};

static uint8_t _g_cfb8_192_enc[] = {
    0xa3, 0x7a, 0x97, 0x12, 0x4f, 0x26, 0x9d, 0xca, 0x14, 0xbf, 0x07, 0x1d,
    0x06, 0xfe, 0x36, 0x81, 0x19, 0x66, 0x6c, 0xa6, 0x4e, 0xf5, 0x44, 0x3e,
    0x3d, 0x4f, 0xa8, 0xc9, 0xf0, 0xd3, 0xf4, 0x8b, 0xa4, 0x34, 0x36, 0x61,
    0xb5, 0xbf, 0x70, 0x3d, 0x47, 0x9a, 0x59, 0x94, 0xff, 0x49, 0xd1, 0x12,
    0x72, 0xe6, 0x4a, 0x1d, 0x27, 0xdf, 0xf9, 0x53, 0xb6, 0xde, 0x65, 0xd2,
    0x1f, 0x6e, 0x0a, 0x28, 0x1e, 0xc8, 0xf1, 0xca, 0x35, 0xbc, 0x62, 0xe9,
    0x13, 0x14, 0x69, 0x87, 0x3c, 0x20, 0xa0, 0xac, 0x9b, 0xe1, 0x2c, 0x4b,
    0x86, 0x39, 0xec, 0x4a, 0xe6, 0x13, 0x5f, 0xbe, 0x14, 0x0b, 0xc4, 0xdf,
    0x7f, 0xe8, 0x77, 0xa9, 0xe5, 0x48, 0x53, 0x95, 0xf8, 0xd8, 0x15, 0x18,
    0xb6, 0xa9, 0x58, 0x56, 0x41, 0xdb, 0x65, 0x40, 0xa3, 0xf4, 0xd0, 0x96,
    0x1e, 0x3e, 0x2f, 0xce, 0x4e, 0xbe, 0xca, 0x26, 0x66, 0x37, 0x0d, 0xf6,
    0x6d, 0xff, 0x30, 0x32, 0x5e, 0x25, 0x26, 0xe8, 0xf8,
};

static uint8_t _g_cfb128_192_enc[] = {
    0xa3, 0x5b, 0xe4, 0x76, 0xa0, 0x0a, 0x89, 0x55, 0xad, 0xfd, 0xb9, 0x92,
    0x27, 0xaa, 0x7b, 0x8d, 0x1a, 0x0a, 0x24, 0x25, 0xb9, 0xd2, 0x65, 0x7e,
    0x09, 0x66, 0xe4, 0xb5, 0xfe, 0xad, 0xef, 0x06, 0x99, 0x60, 0x80, 0x87,
    0xfa, 0xe1, 0xa9, 0xd7, 0xc6, 0xd5, 0xeb, 0x0f, 0xb0, 0x14, 0xb7, 0xa1,
    0xb1, 0xb3, 0xc1, 0xf7, 0x5f, 0x46, 0x0c, 0x07, 0xc8, 0xed, 0x20, 0xe1,
    0xa3, 0xd3, 0x7e, 0xf3, 0xbd, 0x67, 0xb4, 0xa0, 0x0c, 0x82, 0xfd, 0x7c,
    0x92, 0x30, 0x7c, 0xec, 0x64, 0x15, 0xbd, 0x5e, 0xb6, 0x43, 0x1b, 0x77,
    0x11, 0x2c, 0x38, 0x07, 0xc8, 0x2d, 0x37, 0x6f, 0xaf, 0xd2, 0xc3, 0xbc,
    0xf8, 0x7d, 0xde, 0x60, 0xba, 0x6b, 0xd2, 0x49, 0x5e, 0x06, 0x05, 0x01,
    0x12, 0xdc, 0xdd, 0x4d, 0xe3, 0x83, 0x19, 0x66, 0x58, 0x30, 0x8f, 0x7e,
    0x39, 0xdf, 0xfc, 0x02, 0xa6, 0xba, 0x2b, 0x65, 0xfc, 0xf6, 0xf8, 0x21,
    0xe7, 0x0e, 0xea, 0xc2, 0x1c, 0xa2, 0xb5, 0x25, 0xcd,

};

static uint8_t _g_cfb8_256_enc[] = {
    0xd9, 0x0c, 0xe6, 0x45, 0xbc, 0x1b, 0x90, 0x8a, 0x86, 0x00, 0x93, 0x94,
    0xd8, 0xaa, 0x53, 0xd8, 0x00, 0xf5, 0xe1, 0xbb, 0xd3, 0x97, 0x6f, 0x29,
    0x1b, 0x94, 0x6e, 0x75, 0x75, 0xec, 0x53, 0x44, 0x33, 0x61, 0x8e, 0x4c,
    0x4a, 0xe6, 0x10, 0xb9, 0xd7, 0x3c, 0xfe, 0x69, 0xf2, 0xf7, 0x65, 0xf1,
    0x56, 0x18, 0x2a, 0xe3, 0x6d, 0xd7, 0x61, 0x02, 0xe9, 0x89, 0x0e, 0xd8,
    0x74, 0x6a, 0xe6, 0xdc, 0x23, 0xb4, 0xd9, 0x05, 0x59, 0x42, 0x7a, 0x82,
    0x56, 0xbd, 0xc2, 0xa0, 0xea, 0x6a, 0x0b, 0xb3, 0xf6, 0x68, 0x86, 0x4f,
    0x0d, 0x0f, 0xa7, 0x8f, 0x17, 0x7f, 0x97, 0x99, 0xd9, 0xe5, 0xb0, 0xd6,
    0xf1, 0x1b, 0x61, 0xf6, 0x89, 0x98, 0xda, 0x09, 0xc5, 0x67, 0xbd, 0xe2,
    0x14, 0x2a, 0xf3, 0x5b, 0x76, 0xf4, 0x99, 0x9c, 0xc0, 0xa0, 0xcb, 0x70,
    0xaf, 0x24, 0xab, 0xd6, 0x68, 0x02, 0x67, 0x95, 0xb8, 0xf4, 0xa6, 0xcb,
    0x7a, 0x2f, 0x1e, 0x50, 0x18, 0x8f, 0x4a, 0xe5, 0x72,
};

static uint8_t _g_cfb128_256_enc[] = {
    0xd9, 0x14, 0xdd, 0x2c, 0x76, 0x15, 0xfd, 0xae, 0x3c, 0x15, 0x4d, 0x0d,
    0x5d, 0xd5, 0x0e, 0x30, 0x9c, 0x4f, 0x27, 0xf5, 0xc7, 0x87, 0x16, 0x7c,
    0x5f, 0x18, 0x48, 0x0d, 0xd2, 0x70, 0xbc, 0x1e, 0x91, 0xcb, 0x08, 0xb2,
    0x84, 0x01, 0x4a, 0x22, 0xcd, 0xbd, 0xfd, 0xd6, 0x2b, 0xda, 0x07, 0xb9,
    0xdb, 0x8a, 0xf9, 0xdf, 0xaa, 0xa2, 0x5d, 0x54, 0xbc, 0x7e, 0x7d, 0x2d,
    0x3e, 0x8b, 0x3b, 0x5f, 0xb5, 0x18, 0xff, 0xcc, 0x32, 0xbc, 0x54, 0x0e,
    0xdf, 0x9b, 0x0a, 0xc4, 0xcb, 0xfc, 0xb5, 0xed, 0x05, 0x61, 0x48, 0x1c,
    0x24, 0x7a, 0xda, 0xac, 0xa7, 0xfb, 0x1b, 0xf5, 0xfd, 0x7a, 0x04, 0x43,
    0x4f, 0xe0, 0xff, 0xb4, 0xae, 0x3c, 0x77, 0xe5, 0x7e, 0x13, 0xb9, 0xbf,
    0x2c, 0xd5, 0xed, 0x32, 0xe0, 0x38, 0x21, 0xf7, 0xbc, 0xcd, 0x80, 0xbd,
    0x74, 0xa4, 0x8f, 0xc8, 0xb0, 0x08, 0x9a, 0xc7, 0xfc, 0xd2, 0x14, 0xfa,
    0x93, 0xd4, 0x59, 0x9f, 0x3c, 0x98, 0x65, 0xe3, 0x37,
};

static uint8_t _g_ctr_128_enc[] = {
    0x72, 0x29, 0xbe, 0x3a, 0x0a, 0x61, 0x7e, 0xae, 0xca, 0xfa, 0xda, 0xaa,
    0x08, 0x65, 0x50, 0x1e, 0x35, 0x4d, 0x37, 0x50, 0x3e, 0x77, 0xc5, 0x45,
    0xee, 0xbd, 0xbe, 0x7b, 0x5e, 0x90, 0xb3, 0xe6, 0x1d, 0x2c, 0xb4, 0xd2,
    0x16, 0x18, 0xc2, 0x70, 0x84, 0x5d, 0x5a, 0x75, 0xff, 0x7f, 0x23, 0xc4,
    0xd7, 0xec, 0x7f, 0xfc, 0xb3, 0x3f, 0x5e, 0x9b, 0x38, 0x7a, 0x94, 0x21,
    0xa6, 0x32, 0x08, 0x77, 0xb2, 0x60, 0x78, 0xc0, 0x8b, 0x79, 0xd4, 0x88,
    0x82, 0x5f, 0xa2, 0xf9, 0x1d, 0xf8, 0x75, 0xe6, 0x69, 0x41, 0xd1, 0x76,
    0x46, 0x95, 0xae, 0xc4, 0xec, 0x4f, 0x13, 0x1e, 0x56, 0xfb, 0x99, 0xa7,
    0x72, 0x89, 0x2e, 0xcf, 0xf4, 0x16, 0x0b, 0x02, 0xf4, 0x8b, 0x7b, 0xed,
    0xf5, 0x38, 0xe9, 0xd6, 0xb9, 0x8a, 0xe3, 0xd9, 0x90, 0xde, 0xdb, 0xe2,
    0x55, 0xa1, 0x51, 0x19, 0xca, 0x5e, 0xc1, 0x78, 0x8a, 0xf5, 0x7b, 0xab,
    0xcb, 0xd1, 0x3b, 0xb9, 0xba, 0x4d, 0xc5, 0xea, 0x55,
};

static uint8_t _g_ctr_192_enc[] = {
    0xa3, 0x5b, 0xe4, 0x76, 0xa0, 0x0a, 0x89, 0x55, 0xad, 0xfd, 0xb9, 0x92,
    0x27, 0xaa, 0x7b, 0x8d, 0xf5, 0x81, 0xce, 0xc4, 0xe4, 0x43, 0x14, 0x39,
    0x4d, 0x57, 0x6a, 0x7d, 0x4a, 0x7c, 0xd8, 0x04, 0xa4, 0xcc, 0xe5, 0x29,
    0xd8, 0x40, 0x56, 0x46, 0xc8, 0x68, 0x53, 0xd1, 0xa0, 0xd1, 0xad, 0x1d,
    0x78, 0x4d, 0xa9, 0x49, 0x0e, 0x7b, 0x02, 0x57, 0x99, 0x34, 0x96, 0x7b,
    0xe8, 0xfe, 0x26, 0xdf, 0x3a, 0xb9, 0xa4, 0x8b, 0x6f, 0x1f, 0x07, 0x25,
    0xd2, 0x70, 0x7d, 0x02, 0xa5, 0xee, 0x23, 0x8f, 0x49, 0xcb, 0x34, 0x95,
    0x11, 0x83, 0x1c, 0x82, 0x35, 0x2b, 0xf2, 0xb4, 0x76, 0x78, 0xdc, 0xa7,
    0xea, 0x43, 0x9e, 0xad, 0x81, 0xa7, 0x67, 0x87, 0x46, 0xa7, 0xa6, 0xfb,
    0xf1, 0x95, 0x04, 0xa6, 0x44, 0x78, 0xdc, 0x3c, 0x87, 0x29, 0xc7, 0xfe,
    0x52, 0x6b, 0x05, 0x22, 0xbd, 0x71, 0x7b, 0x58, 0xb1, 0xc9, 0x34, 0x4b,
    0xc3, 0xba, 0x8f, 0x9f, 0xd6, 0xed, 0xf2, 0x49, 0x61,
};

static uint8_t _g_ctr_256_enc[] = {
    0xd9, 0x14, 0xdd, 0x2c, 0x76, 0x15, 0xfd, 0xae, 0x3c, 0x15, 0x4d, 0x0d,
    0x5d, 0xd5, 0x0e, 0x30, 0xd4, 0x5d, 0x0a, 0xa7, 0xa8, 0xb5, 0x8d, 0xbf,
    0xd7, 0x84, 0xd6, 0x0d, 0x13, 0x11, 0x86, 0x9d, 0x43, 0x80, 0xcf, 0x5f,
    0x65, 0xe9, 0x8a, 0x36, 0x2e, 0x72, 0x37, 0xf2, 0xaa, 0x2a, 0xbe, 0xf5,
    0xeb, 0x4d, 0xcb, 0x2a, 0x8b, 0xf7, 0x7e, 0x19, 0xea, 0x27, 0x4e, 0xb6,
    0x10, 0xdf, 0x99, 0x28, 0xbf, 0x4f, 0x07, 0xd0, 0xf6, 0x74, 0x14, 0x42,
    0xa6, 0x5a, 0x22, 0x33, 0xfc, 0xe2, 0x41, 0x01, 0x34, 0xd4, 0x08, 0x2d,
    0x16, 0x34, 0xd8, 0x7a, 0xf4, 0x08, 0xfb, 0x2f, 0xe0, 0xe1, 0xb8, 0xf1,
    0x9d, 0x26, 0x4b, 0xac, 0x9d, 0x66, 0x59, 0x2a, 0x0e, 0xd6, 0x8d, 0xd5,
    0x38, 0x9d, 0x51, 0xd9, 0xaa, 0x44, 0x6d, 0xda, 0xd2, 0xf5, 0x3e, 0xb9,
    0xc5, 0x23, 0x20, 0xf6, 0xb1, 0x8a, 0x4e, 0x21, 0x06, 0x1f, 0x14, 0x04,
    0xde, 0x74, 0xaf, 0x05, 0x2d, 0x76, 0xca, 0x69, 0xed,
};

static int _aes_ecb_encrypt_decrypt(uint32_t idx, bool is_enc, size_t key_len,
                                    uint8_t *src, uint8_t *dst, void *aes_ctx)
{
    ali_crypto_result result;
    size_t            dst_size;

#if EN_GCOV_COVERAGE
    /* for gcov coverage */
    result = ali_aes_init(AES_ECB, is_enc, NULL, NULL, key_len, NULL, aes_ctx);
    if (result == ALI_CRYPTO_SUCCESS) {
        return -1;
    }

    /* for gcov coverage */
    result = ali_aes_init(AES_ECB, is_enc, _g_aes_key, NULL, 0, NULL, aes_ctx);
    if (result == ALI_CRYPTO_SUCCESS) {
        return -1;
    }

    /* for gcov coverage */
    result = ali_aes_init((aes_type_t)-1, is_enc, _g_aes_key, NULL, key_len,
                          NULL, aes_ctx);
    if (result == ALI_CRYPTO_SUCCESS) {
        return -1;
    }
#endif

    result =
      ali_aes_init(AES_ECB, is_enc, _g_aes_key, NULL, key_len, NULL, aes_ctx);
    if (result != ALI_CRYPTO_SUCCESS) {
        PRINT_RET(-1, "aes_ecb init fail(%08x)\n", result);
    }


    if ((idx & 0x1) != 0x0) {
#if EN_GCOV_COVERAGE
        /* for gcov coverage */
        result = ali_aes_process(src, dst, 16, NULL);
        if (result == ALI_CRYPTO_SUCCESS) {
            return -1;
        }
        /* for gcov coverage */
        result = ali_aes_process(NULL, dst, 16, aes_ctx);
        if (result == ALI_CRYPTO_SUCCESS) {
            return -1;
        }
        /* for gcov coverage */
        result = ali_aes_process(src, dst, 17, aes_ctx);
        if (result == ALI_CRYPTO_SUCCESS) {
            return -1;
        }
#endif

        result = ali_aes_process(src, dst, 16, aes_ctx);
        if (result != ALI_CRYPTO_SUCCESS) {
            PRINT_RET(-1, "aes_ecb process 1th fail(%08x)", result);
        }
        result = ali_aes_process(src + 16, dst + 16, 16, aes_ctx);
        if (result != ALI_CRYPTO_SUCCESS) {
            PRINT_RET(-1, "aes_ecb process 2th fail(%08x)", result);
        }
        result = ali_aes_process(src + 32, dst + 32, 16, aes_ctx);
        if (result != ALI_CRYPTO_SUCCESS) {
            PRINT_RET(-1, "aes_ecb process 3th fail(%08x)", result);
        }

        dst_size = TEST_ECB_SIZE - 48;

#if EN_GCOV_COVERAGE
        /* for gcov coverage */
        result = ali_aes_finish(src + 48, TEST_ECB_SIZE - 48, dst + 48,
                                &dst_size, SYM_NOPAD, NULL);
        if (result == ALI_CRYPTO_SUCCESS) {
            return -1;
        }
#endif

        result = ali_aes_finish(src + 48, TEST_ECB_SIZE - 48, dst + 48,
                                &dst_size, SYM_NOPAD, aes_ctx);
        if (result != ALI_CRYPTO_SUCCESS) {
            PRINT_RET(-1, "finish fail(%08x) size(%d)\n", result, dst_size);
        }
    } else {
        dst_size = TEST_ECB_SIZE;
        result   = ali_aes_finish(src, TEST_ECB_SIZE, dst, &dst_size, SYM_NOPAD,
                                aes_ctx);
        if (result != ALI_CRYPTO_SUCCESS || dst_size != TEST_ECB_SIZE) {
            PRINT_RET(-1, "finish fail(%08x) size(%d)\n", result, dst_size);
        }
    }

    return 0;
}

static int _aes_cbc_encrypt_decrypt(uint32_t idx, bool is_enc, size_t key_len,
                                    uint8_t *src, uint8_t *dst, void *aes_ctx)
{
    ali_crypto_result result;
    size_t            dst_size;

#if EN_GCOV_COVERAGE
    /* for gcov coverage */
    result =
      ali_aes_init(AES_CBC, is_enc, _g_aes_key, NULL, key_len, NULL, aes_ctx);
    if (result == ALI_CRYPTO_SUCCESS) {
        return -1;
    }
#endif

    result = ali_aes_init(AES_CBC, is_enc, _g_aes_key, NULL, key_len, _g_aes_iv,
                          aes_ctx);
    if (result != ALI_CRYPTO_SUCCESS) {
        PRINT_RET(-1, "aes_cbc init fail(%08x)\n", result);
    }

    if ((idx & 0x1) != 0x0) {
        CRYPT_INF("process finish\n");
#if EN_GCOV_COVERAGE
        /* for gcov coverage */
        result = ali_aes_process(src, dst, 17, aes_ctx);
        if (result == ALI_CRYPTO_SUCCESS) {
            return -1;
        }
#endif
        result = ali_aes_process(src, dst, 16, aes_ctx);
        if (result != ALI_CRYPTO_SUCCESS) {
            PRINT_RET(-1, "aes_cbc process 1th fail(%08x)", result);
        }
        result = ali_aes_process(src + 16, dst + 16, 16, aes_ctx);
        if (result != ALI_CRYPTO_SUCCESS) {
            PRINT_RET(-1, "aes_cbc process 2th fail(%08x)", result);
        }
        result = ali_aes_process(src + 32, dst + 32, 16, aes_ctx);
        if (result != ALI_CRYPTO_SUCCESS) {
            PRINT_RET(-1, "aes_cbc process 3th fail(%08x)", result);
        }

        dst_size = TEST_CBC_SIZE - 48;
        result   = ali_aes_finish(src + 48, TEST_CBC_SIZE - 48, dst + 48,
                                &dst_size, SYM_NOPAD, aes_ctx);
        if (result != ALI_CRYPTO_SUCCESS) {
            PRINT_RET(-1, "aes_cbc finish fail(%08x) size(%d)\n", result,
                      dst_size);
        }
    } else {
        CRYPT_INF("finish no process\n");
        dst_size = TEST_CBC_SIZE;
        result   = ali_aes_finish(src, TEST_CBC_SIZE, dst, &dst_size, SYM_NOPAD,
                                aes_ctx);
        if (result != ALI_CRYPTO_SUCCESS || dst_size != TEST_CBC_SIZE) {
            PRINT_RET(-1, "aes_cbc finish fail(%08x) size(%d)\n", result,
                      dst_size);
        }
    }

    return 0;
}

static int _aes_ctr_encrypt_decrypt(uint32_t idx, bool is_enc, size_t key_len,
                                    uint8_t *src, uint8_t *dst, void *aes_ctx)
{
    ali_crypto_result result;
    size_t            dst_size;

#if EN_GCOV_COVERAGE
    /* for gcov coverage */
    result =
      ali_aes_init(AES_CTR, is_enc, _g_aes_key, NULL, key_len, NULL, aes_ctx);
    if (result == ALI_CRYPTO_SUCCESS) {
        return -1;
    }
#endif

    result = ali_aes_init(AES_CTR, is_enc, _g_aes_key, NULL, key_len, _g_aes_iv,
                          aes_ctx);
    if (result != ALI_CRYPTO_SUCCESS) {
        PRINT_RET(-1, "aes_ctr init fail(%08x)\n", result);
    }
    if (is_enc) {
        if ((idx & 0x1) != 0x0) {
            result = ali_aes_process(src, dst, 13, aes_ctx);
            if (result != ALI_CRYPTO_SUCCESS) {
                PRINT_RET(-1, "aes_ctr process 1th fail(%08x)", result);
            }
            result = ali_aes_process(src + 13, dst + 13, 27, aes_ctx);
            if (result != ALI_CRYPTO_SUCCESS) {
                PRINT_RET(-1, "aes_ctr process 2th fail(%08x)", result);
            }
            result = ali_aes_process(src + 40, dst + 40, 8, aes_ctx);
            if (result != ALI_CRYPTO_SUCCESS) {
                PRINT_RET(-1, "aes_ctr process 3th fail(%08x)", result);
            }

            dst_size = TEST_CTR_SIZE - 48;
            result   = ali_aes_finish(src + 48, TEST_CTR_SIZE - 48, dst + 48,
                                    &dst_size, SYM_NOPAD, aes_ctx);
            if (result != ALI_CRYPTO_SUCCESS) {
                PRINT_RET(-1, "aes_ctr finish fail(%08x)", result);
            }
        } else {
            dst_size = TEST_CTR_SIZE;
            result   = ali_aes_finish(src, TEST_CTR_SIZE, dst, &dst_size,
                                    SYM_NOPAD, aes_ctx);
            if (result != ALI_CRYPTO_SUCCESS || dst_size != TEST_CTR_SIZE) {
                PRINT_RET(-1, "aes_ctr finish fail(%08x)", result);
            }
        }
        else
        {
            if ((idx & 0x1) != 0x0) {
                result = ali_aes_process(src, dst, 8, aes_ctx);
                if (result != ALI_CRYPTO_SUCCESS) {
                    PRINT_RET(-1, "aes_ctr process 1th fail(%08x)", result);
                }
                result = ali_aes_process(src + 8, dst + 8, 30, aes_ctx);
                if (result != ALI_CRYPTO_SUCCESS) {
                    PRINT_RET(-1, "aes_ctr process 2th fail(%08x)", result);
                }
                result = ali_aes_process(src + 38, dst + 38, 10, aes_ctx);
                if (result != ALI_CRYPTO_SUCCESS) {
                    PRINT_RET(-1, "aes_ctr process 3th fail(%08x)", result);
                }

                dst_size = TEST_CTR_SIZE - 48;
                result = ali_aes_finish(src + 48, TEST_CTR_SIZE - 48, dst + 48,
                                        &dst_size, SYM_NOPAD, aes_ctx);
                if (result != ALI_CRYPTO_SUCCESS) {
                    PRINT_RET(-1, "aes_ctr finish fail(%08x)", result);
                }
            } else {
                dst_size = TEST_CTR_SIZE;
                result   = ali_aes_finish(src, TEST_CTR_SIZE, dst, &dst_size,
                                        SYM_NOPAD, aes_ctx);
                if (result != ALI_CRYPTO_SUCCESS || dst_size != TEST_CTR_SIZE) {
                    PRINT_RET(-1, "aes_ctr finish fail(%08x)", result);
                }
            }
            return 0;
        }

        static int _aes_ecb_pkcs_encrypt_decrypt(
          uint32_t idx, bool is_enc, size_t key_len, uint8_t *src,
          size_t in_len, uint8_t *dst, size_t *dst_len, void *aes_ctx)
        {
            ali_crypto_result result;
            size_t            dst_size;

#if EN_GCOV_COVERAGE
            /* for gcov coverage */
            result =
              ali_aes_init(AES_ECB, is_enc, NULL, NULL, key_len, NULL, aes_ctx);
            if (result == ALI_CRYPTO_SUCCESS) {
                return -1;
            }

            /* for gcov coverage */
            result =
              ali_aes_init(AES_ECB, is_enc, _g_aes_key, NULL, 0, NULL, aes_ctx);
            if (result == ALI_CRYPTO_SUCCESS) {
                return -1;
            }

            /* for gcov coverage */
            result = ali_aes_init((aes_type_t)-1, is_enc, _g_aes_key, NULL,
                                  key_len, NULL, aes_ctx);
            if (result == ALI_CRYPTO_SUCCESS) {
                return -1;
            }
#endif

            result = ali_aes_init(AES_ECB, is_enc, _g_aes_key, NULL, key_len,
                                  NULL, aes_ctx);
            if (result != ALI_CRYPTO_SUCCESS) {
                PRINT_RET(-1, "aes_ecb init fail(%08x)\n", result);
            }


            if ((idx & 0x1) != 0x0) {
                CRYPT_INF("process finish\n");
#if EN_GCOV_COVERAGE
                /* for gcov coverage */
                result = ali_aes_process(src, dst, 16, NULL);
                if (result == ALI_CRYPTO_SUCCESS) {
                    return -1;
                }
                /* for gcov coverage */
                result = ali_aes_process(NULL, dst, 16, aes_ctx);
                if (result == ALI_CRYPTO_SUCCESS) {
                    return -1;
                }
                /* for gcov coverage */
                result = ali_aes_process(src, dst, 17, aes_ctx);
                if (result == ALI_CRYPTO_SUCCESS) {
                    return -1;
                }
#endif

                result = ali_aes_process(src, dst, 16, aes_ctx);
                if (result != ALI_CRYPTO_SUCCESS) {
                    PRINT_RET(-1, "aes_ecb process 1th fail(%08x)", result);
                }
                result = ali_aes_process(src + 16, dst + 16, 16, aes_ctx);
                if (result != ALI_CRYPTO_SUCCESS) {
                    PRINT_RET(-1, "aes_ecb process 2th fail(%08x)", result);
                }
                result = ali_aes_process(src + 32, dst + 32, 16, aes_ctx);
                if (result != ALI_CRYPTO_SUCCESS) {
                    PRINT_RET(-1, "aes_ecb process 3th fail(%08x)", result);
                }

                dst_size = *dst_len - 48;

#if EN_GCOV_COVERAGE
                /* for gcov coverage */
                result = ali_aes_finish(src + 48, in_len - 48, dst + 48,
                                        &dst_size, SYM_PKCS5_PAD, NULL);
                if (result == ALI_CRYPTO_SUCCESS) {
                    return -1;
                }
#endif

                result = ali_aes_finish(src + 48, in_len - 48, dst + 48,
                                        &dst_size, SYM_PKCS5_PAD, aes_ctx);
                if (result != ALI_CRYPTO_SUCCESS) {
                    PRINT_RET(-1, "finish fail(%08x) size(%d)\n", result,
                              dst_size);
                }

                *dst_len = 48 + dst_size;
            } else {
                CRYPT_INF("finish not process\n");
                dst_size = *dst_len;
                result   = ali_aes_finish(src, in_len, dst, &dst_size,
                                        SYM_PKCS5_PAD, aes_ctx);

                if (result != ALI_CRYPTO_SUCCESS) {
                    PRINT_RET(-1, "finish fail(%08x) size(%d)\n", result,
                              dst_size);
                }
                *dst_len = dst_size;
            }

            return 0;
        }

        static int _aes_cbc_pkcs_encrypt_decrypt(
          uint32_t idx, bool is_enc, size_t key_len, uint8_t *src,
          size_t in_len, uint8_t *dst, size_t *dst_len, void *aes_ctx)
        {
            ali_crypto_result result;
            size_t            dst_size;

#if EN_GCOV_COVERAGE
            /* for gcov coverage */
            result = ali_aes_init(AES_CBC, is_enc, _g_aes_key, NULL, key_len,
                                  NULL, aes_ctx);
            if (result == ALI_CRYPTO_SUCCESS) {
                return -1;
            }
#endif

            result = ali_aes_init(AES_CBC, is_enc, _g_aes_key, NULL, key_len,
                                  _g_aes_iv, aes_ctx);
            if (result != ALI_CRYPTO_SUCCESS) {
                PRINT_RET(-1, "aes_cbc init fail(%08x)\n", result);
            }

            if ((idx & 0x1) != 0x0) {
                CRYPT_INF("process finis\n");
#if EN_GCOV_COVERAGE
                /* for gcov coverage */
                result = ali_aes_process(src, dst, 17, aes_ctx);
                if (result == ALI_CRYPTO_SUCCESS) {
                    return -1;
                }
#endif
                result = ali_aes_process(src, dst, 16, aes_ctx);
                if (result != ALI_CRYPTO_SUCCESS) {
                    PRINT_RET(-1, "aes_cbc process 1th fail(%08x)", result);
                }
                result = ali_aes_process(src + 16, dst + 16, 16, aes_ctx);
                if (result != ALI_CRYPTO_SUCCESS) {
                    PRINT_RET(-1, "aes_cbc process 2th fail(%08x)", result);
                }
                result = ali_aes_process(src + 32, dst + 32, 16, aes_ctx);
                if (result != ALI_CRYPTO_SUCCESS) {
                    PRINT_RET(-1, "aes_cbc process 3th fail(%08x)", result);
                }

                dst_size = *dst_len - 48;
                result   = ali_aes_finish(src + 48, in_len - 48, dst + 48,
                                        &dst_size, SYM_PKCS5_PAD, aes_ctx);
                if (result != ALI_CRYPTO_SUCCESS) {
                    PRINT_RET(-1, "aes_cbc finish fail(%08x) size(%d)\n",
                              result, dst_size);
                }

                *dst_len = 48 + dst_size;
            } else {
                CRYPT_INF("finish no process\n");
                dst_size = *dst_len;
                result   = ali_aes_finish(src, in_len, dst, &dst_size,
                                        SYM_PKCS5_PAD, aes_ctx);
                if (result != ALI_CRYPTO_SUCCESS) {
                    PRINT_RET(-1, "aes_cbc finish fail(%08x) size(%d)\n",
                              result, dst_size);
                }
                *dst_len = dst_size;
            }

            return 0;
        }

#if 0
        static int _aes_cts_encrypt_decrypt(uint32_t idx, bool is_enc,
                                            size_t key_len, uint8_t *src, uint8_t *dst, void *aes_ctx) {
            ali_crypto_result result;
            size_t dst_size;

            result = ali_aes_init(AES_CTS, is_enc,
                                  _g_aes_key, NULL, key_len, _g_aes_iv, aes_ctx);
            if (result != ALI_CRYPTO_SUCCESS) {
                CRYPT_ERR("aes_cts init fail(%08x)\n", result);
                return -1;
            }

            if ((idx & 0x1) != 0x0) {
                result = ali_aes_process(src, dst, 16, aes_ctx);
                if (result != ALI_CRYPTO_SUCCESS) {
                    CRYPT_ERR("aes_cts process 1th fail(%08x)", result);
                    return -1;
                }
                result = ali_aes_process(src + 16, dst + 16, 32, aes_ctx);
                if (result != ALI_CRYPTO_SUCCESS) {
                    CRYPT_ERR("aes_cts process 2th fail(%08x)", result);
                    return -1;
                }
                result = ali_aes_process(src + 48, dst + 48, 16, aes_ctx);
                if (result != ALI_CRYPTO_SUCCESS) {
                    CRYPT_ERR("aes_cts process 3th fail(%08x)", result);
                    return -1;
                }

                dst_size = TEST_CTS_SIZE - 64;
                result = ali_aes_finish(
                             src + 64, TEST_CTS_SIZE - 64,
                             dst + 64, &dst_size, SYM_NOPAD, aes_ctx);
                if (result != ALI_CRYPTO_SUCCESS) {
                    CRYPT_ERR("aes_cts finish fail(%08x)", result);
                    return -1;
                }
            } else {
                dst_size = TEST_CTS_SIZE;
                result = ali_aes_finish(
                             src, TEST_CTS_SIZE,
                             dst, &dst_size, SYM_NOPAD, aes_ctx);
                if (result != ALI_CRYPTO_SUCCESS || dst_size != TEST_CTS_SIZE) {
                    CRYPT_ERR("aes_cts finish fail(%08x)", result);
                    return -1;
                }
            }

            return 0;
        }

        static int _aes_xts_encrypt_decrypt(uint32_t idx, bool is_enc,
                                            size_t key_len, uint8_t *src, uint8_t *dst, void *aes_ctx) {
            ali_crypto_result result;
            size_t dst_size;

            result = ali_aes_init(AES_XTS, is_enc,
                                  _g_aes_key, _g_aes_key, key_len, _g_aes_iv, aes_ctx);
            if (result != ALI_CRYPTO_SUCCESS) {
                CRYPT_ERR("aes_xts init fail(%08x)\n", result);
                return -1;
            }

            if ((idx & 0x1) != 0x0) {
                result = ali_aes_process(src, dst, 16, aes_ctx);
                if (result != ALI_CRYPTO_SUCCESS) {
                    CRYPT_ERR("aes_xts process 1th fail(%08x)", result);
                    return -1;
                }
                result = ali_aes_process(src + 16, dst + 16, 32, aes_ctx);
                if (result != ALI_CRYPTO_SUCCESS) {
                    CRYPT_ERR("aes_xts process 2th fail(%08x)", result);
                    return -1;
                }
                result = ali_aes_process(src + 48, dst + 48, 16, aes_ctx);
                if (result != ALI_CRYPTO_SUCCESS) {
                    CRYPT_ERR("aes_xts process 3th fail(%08x)", result);
                    return -1;
                }

                dst_size = TEST_XTS_SIZE - 64;
                result = ali_aes_finish(
                             src + 64, TEST_XTS_SIZE - 64,
                             dst + 64, &dst_size, SYM_NOPAD, aes_ctx);
                if (result != ALI_CRYPTO_SUCCESS) {
                    CRYPT_ERR("aes_xts finish fail(%08x)", result);
                    return -1;
                }
            } else {
                dst_size = TEST_XTS_SIZE;
                result = ali_aes_finish(
                             src, TEST_XTS_SIZE,
                             dst, &dst_size, SYM_NOPAD, aes_ctx);
                if (result != ALI_CRYPTO_SUCCESS || dst_size != TEST_XTS_SIZE) {
                    CRYPT_ERR("aes_xts finish fail(%08x)", result);
                    return -1;
                }
            }

            return 0;
        }
#endif

#if 1
        static int _aes_cfb_encrypt_decrypt(
          uint32_t idx, aes_type_t type, bool is_enc, size_t key_len,
          uint8_t *src, uint8_t *dst, void *aes_ctx)
        {
            ali_crypto_result result;
            size_t            dst_size;

#if EN_GCOV_COVERAGE
            /* for gcov coverage */
            result = ali_aes_init(type, is_enc, _g_aes_key, NULL, key_len, NULL,
                                  aes_ctx);
            if (result == ALI_CRYPTO_SUCCESS) {
                return -1;
            }
#endif

            result = ali_aes_init(type, is_enc, _g_aes_key, NULL, key_len,
                                  _g_aes_iv, aes_ctx);
            if (result != ALI_CRYPTO_SUCCESS) {
                PRINT_RET(-1, "aes_cfb init fail(%08x)\n", result);
            }

            if ((idx & 0x1) != 0x0) {
                result = ali_aes_process(src, dst, 13, aes_ctx);
                if (result != ALI_CRYPTO_SUCCESS) {
                    PRINT_RET(-1, "aes_cfb process 1th fail(%08x)", result);
                }
                result = ali_aes_process(src + 13, dst + 13, 27, aes_ctx);
                if (result != ALI_CRYPTO_SUCCESS) {
                    PRINT_RET(-1, "aes_cfb process 2th fail(%08x)", result);
                }
                result = ali_aes_process(src + 40, dst + 40, 8, aes_ctx);
                if (result != ALI_CRYPTO_SUCCESS) {
                    PRINT_RET(-1, "aes_cfb process 3th fail(%08x)", result);
                }

                dst_size = TEST_CFB_SIZE - 48;
                result = ali_aes_finish(src + 48, TEST_CTR_SIZE - 48, dst + 48,
                                        &dst_size, SYM_NOPAD, aes_ctx);
                if (result != ALI_CRYPTO_SUCCESS) {
                    PRINT_RET(-1, "aes_cfb finish fail(%08x)", result);
                }
            } else {
                dst_size = TEST_CFB_SIZE;
                result   = ali_aes_finish(src, TEST_CTR_SIZE, dst, &dst_size,
                                        SYM_NOPAD, aes_ctx);
                if (result != ALI_CRYPTO_SUCCESS || dst_size != TEST_CTR_SIZE) {
                    PRINT_RET(-1, "aes_cfb finish fail(%08x)", result);
                }
            }

            return 0;
        }
#endif

        static int _ali_crypto_aes_ecb_test(void)
        {
            int               ret;
            ali_crypto_result result;
            size_t            i, key_len;
            void *            aes_ctx = NULL;
            size_t            aes_ctx_size;
            aes_type_t        type = AES_ECB;
            uint8_t *         ecb_enc_exp;
            size_t            ecb_enc_exp_size;
            uint8_t           plaintext[TEST_ECB_SIZE_NOALIGN];
            uint8_t           ciphertext[TEST_ECB_SIZE_NOALIGN + 16];
            size_t            out_len, dec_len;

#if EN_GCOV_COVERAGE
            /* for gcov coverage */
            result = ali_aes_init(AES_ECB, true, NULL, NULL, 16, NULL, NULL);
            if (result == ALI_CRYPTO_SUCCESS) {
                return -1;
            }

            /* for gcov coverage */
            result = ali_aes_reset(NULL);
            if (result == ALI_CRYPTO_SUCCESS) {
                return -1;
            }

            /* for gcov coverage */
            result = ali_aes_copy_context(NULL, NULL);
            if (result == ALI_CRYPTO_SUCCESS) {
                return -1;
            }
#endif

            for (i = 0; i < 3; i++) {
                if (i == 0) {
                    key_len     = 16;
                    ecb_enc_exp = _g_ecb_128_enc;
                } else if (i == 1) {
                    key_len     = 24;
                    ecb_enc_exp = _g_ecb_192_enc;
                } else {
                    key_len     = 32;
                    ecb_enc_exp = _g_ecb_256_enc;
                }

                result = ali_aes_get_ctx_size(type, &aes_ctx_size);
                if (result != ALI_CRYPTO_SUCCESS) {
                    PRINT_RET(-1, "get ctx size fail(%08x)\n", result);
                }

                aes_ctx = CRYPT_MALLOC(aes_ctx_size);
                if (aes_ctx == NULL) {
                    GO_RET(-1, "kmalloc(%d) fail\n", (int)aes_ctx_size);
                }

                /* encrypt no pad */
                CRYPT_MEMSET(aes_ctx, 0, aes_ctx_size);
                ret = _aes_ecb_encrypt_decrypt(i, true, key_len, _g_test_data,
                                               ciphertext, aes_ctx);
                if (ret < 0) {
                    GO_RET(-1, "ecb encrypt fail!\n");
                }

                /* decrypt no pad */
                CRYPT_MEMSET(aes_ctx, 0, aes_ctx_size);
                ret = _aes_ecb_encrypt_decrypt(i, false, key_len, ciphertext,
                                               plaintext, aes_ctx);
                if (ret < 0) {
                    GO_RET(-1, "ecb decrypt fail!\n");
                }

                /* check result no pad */
                if (CRYPT_MEMCMP(ciphertext, ecb_enc_exp, TEST_ECB_SIZE) ||
                    CRYPT_MEMCMP(plaintext, _g_test_data, TEST_ECB_SIZE)) {
                    ali_crypto_print_data("plaintext", plaintext,
                                          TEST_ECB_SIZE);
                    ali_crypto_print_data("ciphertext", ciphertext,
                                          TEST_ECB_SIZE);
                    GO_RET(-1, "AES_ECB_%d test fail!\n", (int)key_len << 3);
                } else {
                    CRYPT_INF("AES_ECB_%d test success!\n", (int)key_len << 3);
                }

                if (i == 0) {
                    ecb_enc_exp      = _g_ecb_128_pkcs_enc;
                    ecb_enc_exp_size = sizeof(_g_ecb_128_pkcs_enc);
                } else if (i == 1) {
                    ecb_enc_exp      = _g_ecb_192_pkcs_enc;
                    ecb_enc_exp_size = sizeof(_g_ecb_192_pkcs_enc);
                } else {
                    ecb_enc_exp      = _g_ecb_256_pkcs_enc;
                    ecb_enc_exp_size = sizeof(_g_ecb_256_pkcs_enc);
                }
                /* encrypt pkcs */
                CRYPT_MEMSET(aes_ctx, 0, aes_ctx_size);
                CRYPT_MEMSET(ciphertext, 0, TEST_ECB_SIZE + 16);
                CRYPT_MEMSET(plaintext, 0, TEST_ECB_SIZE);
                out_len = TEST_ECB_SIZE + 16;
                ret     = _aes_ecb_pkcs_encrypt_decrypt(
                  i, true, key_len, _g_test_data, TEST_ECB_SIZE, ciphertext,
                  &out_len, aes_ctx);
                if (ret < 0) {
                    GO_RET(-1, "ecb encrypt fail!\n");
                }

                /* decrypt pkcs */
                CRYPT_MEMSET(aes_ctx, 0, aes_ctx_size);
                dec_len = TEST_ECB_SIZE;
                ret     = _aes_ecb_pkcs_encrypt_decrypt(
                  i, false, key_len, ciphertext, out_len, plaintext, &dec_len,
                  aes_ctx);
                if (ret < 0) {
                    GO_RET(-1, "ecb decrypt fail!\n");
                }

                /* check result pkcs */
                if (CRYPT_MEMCMP(ciphertext, ecb_enc_exp, ecb_enc_exp_size) ||
                    CRYPT_MEMCMP(plaintext, _g_test_data, TEST_ECB_SIZE)) {
                    ali_crypto_print_data("plaintext", plaintext,
                                          TEST_ECB_SIZE);
                    ali_crypto_print_data("ciphertext", ciphertext,
                                          ecb_enc_exp_size);
                    GO_RET(-1, "AES_ECB_%d pkcs test fail!\n",
                           (int)key_len << 3);
                } else {
                    CRYPT_INF("AES_ECB_%d pkcs test success!\n",
                              (int)key_len << 3);
                }

                /* encrypt pkcs no align*/
                CRYPT_MEMSET(aes_ctx, 0, aes_ctx_size);
                CRYPT_MEMSET(ciphertext, 0, TEST_ECB_SIZE_NOALIGN + 16);
                CRYPT_MEMSET(plaintext, 0, TEST_ECB_SIZE_NOALIGN);
                out_len = TEST_ECB_SIZE_NOALIGN + 16;
                ret     = _aes_ecb_pkcs_encrypt_decrypt(
                  i, true, key_len, _g_test_data, TEST_ECB_SIZE_NOALIGN,
                  ciphertext, &out_len, aes_ctx);
                if (ret < 0) {
                    GO_RET(-1, "ecb encrypt fail!\n");
                }

                /* decrypt pkcs no align */
                CRYPT_MEMSET(aes_ctx, 0, aes_ctx_size);
                dec_len = TEST_ECB_SIZE_NOALIGN;
                ret     = _aes_ecb_pkcs_encrypt_decrypt(
                  i, false, key_len, ciphertext, out_len, plaintext, &dec_len,
                  aes_ctx);
                if (ret < 0) {
                    GO_RET(-1, "ecb decrypt fail!\n");
                }

                /* check result no align */
                if (CRYPT_MEMCMP(plaintext, _g_test_data,
                                 TEST_ECB_SIZE_NOALIGN)) {
                    ali_crypto_print_data("plaintext", plaintext,
                                          TEST_ECB_SIZE_NOALIGN);
                    ali_crypto_print_data("ciphertext", ciphertext, out_len);
                    GO_RET(-1, "AES_ECB_%d pkcs test fail!\n",
                           (int)key_len << 3);
                } else {
                    CRYPT_INF("AES_ECB_%d pkcs test success!\n",
                              (int)key_len << 3);
                }
#if EN_GCOV_COVERAGE
                /* for gcov coverage */
                result = ali_aes_copy_context(aes_ctx, aes_ctx);
                if (result == ALI_CRYPTO_SUCCESS) {
                    GO_RET(-1, "ali_aes_copy_context not expect!\n");
                }
#endif

                result = ali_aes_reset(aes_ctx);
                if (result != ALI_CRYPTO_SUCCESS) {
                    GO_RET(-1, "ali_aes_reset fail %d!\n", result);
                }
                CRYPT_FREE(aes_ctx);
                aes_ctx = NULL;
            }

            return 0;

        _OUT:
            if (aes_ctx) {
                CRYPT_FREE(aes_ctx);
            }

            return -1;
        }

        static int _ali_crypto_aes_cbc_test(void)
        {
            int               ret;
            ali_crypto_result result;
            size_t            i, key_len;
            void *            aes_ctx = NULL;
            size_t            aes_ctx_size;
            aes_type_t        type = AES_CBC;
            uint8_t *         cbc_enc_exp;
            size_t            cbc_enc_exp_size;
            uint8_t           plaintext[TEST_CBC_SIZE_NOALIGN];
            uint8_t           ciphertext[TEST_CBC_SIZE_NOALIGN + 16];
            size_t            out_len, dec_len;

            for (i = 0; i < 3; i++) {
                if (i == 0) {
                    key_len     = 16;
                    cbc_enc_exp = _g_cbc_128_enc;
                } else if (i == 1) {
                    key_len     = 24;
                    cbc_enc_exp = _g_cbc_192_enc;
                } else {
                    key_len     = 32;
                    cbc_enc_exp = _g_cbc_256_enc;
                }

                result = ali_aes_get_ctx_size(type, &aes_ctx_size);
                if (result != ALI_CRYPTO_SUCCESS) {
                    GO_RET(-1, "get ctx size fail(%08x)\n", result);
                }

                aes_ctx = CRYPT_MALLOC(aes_ctx_size);
                if (aes_ctx == NULL) {
                    GO_RET(-1, "malloc(%d) fail\n", (int)aes_ctx_size);
                }

                /* encrypt nopad */
                CRYPT_MEMSET(aes_ctx, 0, aes_ctx_size);
                ret = _aes_cbc_encrypt_decrypt(i, true, key_len, _g_test_data,
                                               ciphertext, aes_ctx);
                if (ret < 0) {
                    GO_RET(-1, "cbc encrypt fail!\n");
                }

                /* decrypt no pad */
                CRYPT_MEMSET(aes_ctx, 0, aes_ctx_size);
                ret = _aes_cbc_encrypt_decrypt(i, false, key_len, ciphertext,
                                               plaintext, aes_ctx);
                if (ret < 0) {
                    GO_RET(-1, "cbc decrypt fail!\n");
                }

                /* check result no pad */
                if (CRYPT_MEMCMP(ciphertext, cbc_enc_exp, TEST_CBC_SIZE) ||
                    CRYPT_MEMCMP(plaintext, _g_test_data, TEST_CBC_SIZE)) {
                    ali_crypto_print_data("plaintext", plaintext,
                                          TEST_CBC_SIZE);
                    ali_crypto_print_data("ciphertext", ciphertext,
                                          TEST_CBC_SIZE);
                    GO_RET(-1, "AES_CBC_%d test fail!\n", (int)key_len << 3);
                } else {
                    CRYPT_INF("AES_CBC_%d test success!\n", (int)key_len << 3);
                }

                if (i == 0) {
                    cbc_enc_exp      = _g_cbc_128_pkcs_enc;
                    cbc_enc_exp_size = sizeof(_g_cbc_128_pkcs_enc);
                } else if (i == 1) {
                    cbc_enc_exp      = _g_cbc_192_pkcs_enc;
                    cbc_enc_exp_size = sizeof(_g_cbc_192_pkcs_enc);
                } else {
                    cbc_enc_exp      = _g_cbc_256_pkcs_enc;
                    cbc_enc_exp_size = sizeof(_g_cbc_256_pkcs_enc);
                }
                /* encrypt pkcs */
                CRYPT_MEMSET(aes_ctx, 0, aes_ctx_size);
                CRYPT_MEMSET(ciphertext, 0, TEST_CBC_SIZE + 16);
                CRYPT_MEMSET(plaintext, 0, TEST_CBC_SIZE);
                out_len = TEST_CBC_SIZE + 16;
                ret     = _aes_cbc_pkcs_encrypt_decrypt(
                  i, true, key_len, _g_test_data, TEST_CBC_SIZE, ciphertext,
                  &out_len, aes_ctx);
                if (ret < 0) {
                    GO_RET(-1, "cbc encrypt fail!\n");
                }

                /* decrypt pkcs */
                CRYPT_MEMSET(aes_ctx, 0, aes_ctx_size);
                dec_len = TEST_CBC_SIZE;
                ret     = _aes_cbc_pkcs_encrypt_decrypt(
                  i, false, key_len, ciphertext, out_len, plaintext, &dec_len,
                  aes_ctx);
                if (ret < 0) {
                    GO_RET(-1, "cbc decrypt fail!\n");
                }

                /* check result pkcs */
                if (CRYPT_MEMCMP(ciphertext, cbc_enc_exp, cbc_enc_exp_size) ||
                    CRYPT_MEMCMP(plaintext, _g_test_data, TEST_CBC_SIZE)) {
                    ali_crypto_print_data("plaintext", plaintext,
                                          TEST_CBC_SIZE);
                    ali_crypto_print_data("ciphertext", ciphertext,
                                          cbc_enc_exp_size);
                    GO_RET(-1, "AES_CBC_%d test fail!\n", (int)key_len << 3);
                } else {
                    CRYPT_INF("AES_CBC_%d test success!\n", (int)key_len << 3);
                }

                /* encrypt pkcs no align */
                CRYPT_MEMSET(aes_ctx, 0, aes_ctx_size);
                CRYPT_MEMSET(ciphertext, 0, TEST_CBC_SIZE_NOALIGN + 16);
                CRYPT_MEMSET(plaintext, 0, TEST_CBC_SIZE_NOALIGN);
                out_len = TEST_CBC_SIZE_NOALIGN + 16;
                ret     = _aes_cbc_pkcs_encrypt_decrypt(
                  i, true, key_len, _g_test_data, TEST_CBC_SIZE_NOALIGN,
                  ciphertext, &out_len, aes_ctx);
                if (ret < 0) {
                    GO_RET(-1, "cbc encrypt fail!\n");
                }

                /* decrypt pkcs no align */
                CRYPT_MEMSET(aes_ctx, 0, aes_ctx_size);
                dec_len = TEST_CBC_SIZE_NOALIGN;
                ret     = _aes_cbc_pkcs_encrypt_decrypt(
                  i, false, key_len, ciphertext, out_len, plaintext, &dec_len,
                  aes_ctx);
                if (ret < 0) {
                    GO_RET(-1, "cbc decrypt fail!\n");
                }

                /* check result pkcs no align */
                if (CRYPT_MEMCMP(plaintext, _g_test_data,
                                 TEST_CBC_SIZE_NOALIGN)) {
                    ali_crypto_print_data("plaintext", plaintext,
                                          TEST_CBC_SIZE_NOALIGN);
                    GO_RET(-1, "AES_CBC_%d test fail!\n", (int)key_len << 3);
                } else {
                    CRYPT_INF("AES_CBC_%d test success!\n", (int)key_len << 3);
                }

                CRYPT_FREE(aes_ctx);
                aes_ctx = NULL;
            }

            return 0;

        _OUT:
            if (aes_ctx) {
                CRYPT_FREE(aes_ctx);
            }

            return -1;
        }

        static int _ali_crypto_aes_ctr_test(void)
        {
            int               ret;
            ali_crypto_result result;
            size_t            i, key_len;
            void *            aes_ctx = NULL;
            size_t            aes_ctx_size;
            aes_type_t        type = AES_CTR;
            uint8_t *         ctr_enc_exp;
            uint8_t           plaintext[TEST_CTR_SIZE];
            uint8_t           ciphertext[TEST_CTR_SIZE];

            for (i = 0; i < 3; i++) {
                if (i == 0) {
                    key_len     = 16;
                    ctr_enc_exp = _g_ctr_128_enc;
                } else if (i == 1) {
                    key_len     = 24;
                    ctr_enc_exp = _g_ctr_192_enc;
                } else {
                    key_len     = 32;
                    ctr_enc_exp = _g_ctr_256_enc;
                }

                result = ali_aes_get_ctx_size(type, &aes_ctx_size);
                if (result != ALI_CRYPTO_SUCCESS) {
                    GO_RET(-1, "get ctx size fail(%08x)\n", result);
                }

                aes_ctx = CRYPT_MALLOC(aes_ctx_size);
                if (aes_ctx == NULL) {
                    GO_RET(-1, "malloc(%d) fail\n", (int)aes_ctx_size);
                }

                /* encrypt */
                CRYPT_MEMSET(aes_ctx, 0, aes_ctx_size);
                ret = _aes_ctr_encrypt_decrypt(i, true, key_len, _g_test_data,
                                               ciphertext, aes_ctx);
                if (ret < 0) {
                    GO_RET(-1, "ctr encrypt fail!\n");
                }

                /* decrypt */
                CRYPT_MEMSET(aes_ctx, 0, aes_ctx_size);
                ret = _aes_ctr_encrypt_decrypt(i, false, key_len, ciphertext,
                                               plaintext, aes_ctx);
                if (ret < 0) {
                    GO_RET(-1, "ctr decrypt fail!\n");
                }

                /* check result */
                if (CRYPT_MEMCMP(plaintext, _g_test_data, TEST_CTR_SIZE) ||
                    CRYPT_MEMCMP(ciphertext, ctr_enc_exp, TEST_CTR_SIZE)) {
                    ali_crypto_print_data("plaintext", plaintext,
                                          TEST_CTR_SIZE);
                    ali_crypto_print_data("ciphertext", ciphertext,
                                          TEST_CTR_SIZE);
                    GO_RET(-1, "AES_CTR_%d test fail!\n", (int)key_len << 3);
                } else {
                    CRYPT_INF("AES_CTR_%d test success!\n", (int)key_len << 3);
                }

                CRYPT_FREE(aes_ctx);
                aes_ctx = NULL;
            }

            return 0;

        _OUT:
            if (aes_ctx) {
                CRYPT_FREE(aes_ctx);
            }

            return -1;
        }

#if 1
        static int _ali_crypto_aes_cfb_test(void)
        {
            int               ret;
            ali_crypto_result result;
            size_t            i, key_len;
            void *            aes_ctx = NULL;
            size_t            aes_ctx_size;
            aes_type_t        type;
            uint8_t *         cfb_enc_exp;
            uint8_t           plaintext[TEST_CFB_SIZE];
            uint8_t           ciphertext[TEST_CFB_SIZE];

            for (type = AES_CFB8; type <= AES_CFB128; type++) {
                for (i = 0; i < 3; i++) {
                    if (i == 0) {
                        key_len = 16;
                        if (type == AES_CFB8) {
                            cfb_enc_exp = _g_cfb8_128_enc;
                        } else if (type == AES_CFB128) {
                            cfb_enc_exp = _g_cfb128_128_enc;
                        } else {
                            GO_RET(-1, "invalid cfb type 0x%08x\n", type);
                        }
                    } else if (i == 1) {
                        key_len = 24;
                        if (type == AES_CFB8) {
                            cfb_enc_exp = _g_cfb8_192_enc;
                        } else if (type == AES_CFB128) {
                            cfb_enc_exp = _g_cfb128_192_enc;
                        } else {
                            GO_RET(-1, "invalid cfb type 0x%08x\n", type);
                        }
                    } else {
                        key_len = 32;
                        if (type == AES_CFB8) {
                            cfb_enc_exp = _g_cfb8_256_enc;
                        } else if (type == AES_CFB128) {
                            cfb_enc_exp = _g_cfb128_256_enc;
                        } else {
                            GO_RET(-1, "invalid cfb key type 0x%08x\n", i);
                        }
                    }

                    result = ali_aes_get_ctx_size(type, &aes_ctx_size);
                    if (result != ALI_CRYPTO_SUCCESS) {
                        GO_RET(-1, "get ctx size fail(%08x)\n", result);
                    }

                    aes_ctx = CRYPT_MALLOC(aes_ctx_size);
                    if (aes_ctx == NULL) {
                        GO_RET(-1, "malloc(%d) fail\n", (int)aes_ctx_size);
                    }

                    /* encrypt */
                    CRYPT_MEMSET(aes_ctx, 0, aes_ctx_size);
                    ret = _aes_cfb_encrypt_decrypt(i, type, true, key_len,
                                                   _g_test_data, ciphertext,
                                                   aes_ctx);
                    if (ret < 0) {
                        GO_RET(-1, "ctr encrypt fail!\n");
                    }

                    /* decrypt */
                    CRYPT_MEMSET(aes_ctx, 0, aes_ctx_size);
                    ret = _aes_cfb_encrypt_decrypt(
                      i, type, false, key_len, ciphertext, plaintext, aes_ctx);
                    if (ret < 0) {
                        GO_RET(-1, "ctr decrypt fail!\n");
                    }

                    /* check result */
                    if (CRYPT_MEMCMP(plaintext, _g_test_data, TEST_CFB_SIZE) ||
                        CRYPT_MEMCMP(ciphertext, cfb_enc_exp, TEST_CFB_SIZE)) {
                        ali_crypto_print_data("plaintext", plaintext,
                                              TEST_CFB_SIZE);
                        ali_crypto_print_data("ciphertext", ciphertext,
                                              TEST_CFB_SIZE);
                        GO_RET(-1, "AES_CFB_%d test fail!\n",
                               (int)key_len << 3);
                    } else {
                        CRYPT_INF("AES_CFB_%d test success!\n",
                                  (int)key_len << 3);
                    }

                    CRYPT_FREE(aes_ctx);
                    aes_ctx = NULL;
                }
            }

            return 0;

        _OUT:
            if (aes_ctx) {
                CRYPT_FREE(aes_ctx);
            }

            return -1;
        }
#endif

#if 0
        static int _ali_crypto_aes_cts_test(void) {
            int ret;
            ali_crypto_result result;
            size_t i, key_len;
            void *aes_ctx = NULL;
            size_t aes_ctx_size;
            aes_type_t type = AES_CTS;
            uint8_t plaintext[TEST_CTS_SIZE];
            uint8_t ciphertext[TEST_CTS_SIZE];

            for (i = 0; i < 3; i++) {
                if (i == 0) {
                    key_len = 16;
                } else if (i == 1) {
                    key_len = 24;
                } else {
                    key_len = 32;
                }

                result = ali_aes_get_ctx_size(type, &aes_ctx_size);
                if (result != ALI_CRYPTO_SUCCESS) {
                    CRYPT_ERR("get ctx size fail(%08x)\n", result);
                    goto _OUT;
                }

                aes_ctx = CRYPT_MALLOC(aes_ctx_size);
                if (aes_ctx == NULL) {
                    CRYPT_ERR("kmalloc(%d) fail\n", aes_ctx_size);
                    goto _OUT;
                }

                /* encrypt */
                CRYPT_MEMSET(aes_ctx, 0, aes_ctx_size);
                ret = _aes_cts_encrypt_decrypt(i, true,
                                               key_len, _g_test_data, ciphertext, aes_ctx);
                if (ret < 0) {
                    CRYPT_ERR("cts encrypt fail!\n");
                    goto _OUT;
                }

                /* decrypt */
                CRYPT_MEMSET(aes_ctx, 0, aes_ctx_size);
                ret = _aes_cts_encrypt_decrypt(i, false,
                                               key_len, ciphertext, plaintext, aes_ctx);
                if (ret < 0) {
                    CRYPT_ERR("cts decrypt fail!\n");
                    goto _OUT;
                }

                /* check result */
                if (CRYPT_MEMCMP(plaintext, _g_test_data, TEST_CTS_SIZE)) {
                    CRYPT_INF("AES_CTS_%d test fail!\n", key_len << 3);
                    ali_crypto_print_data("plaintext", plaintext, TEST_CTS_SIZE);
                    ali_crypto_print_data("ciphertext", ciphertext, TEST_CTS_SIZE);
                    goto _OUT;
                } else {
                    CRYPT_INF("AES_CTS_%d test success!\n", key_len << 3);
                }

                CRYPT_FREE(aes_ctx);
                aes_ctx = NULL;
            }

            return 0;

_OUT:
            if (aes_ctx) {
                CRYPT_FREE(aes_ctx);
            }

            return -1;
        }

        static int _ali_crypto_aes_xts_test(void) {
            int ret;
            ali_crypto_result result;
            size_t i, key_len;
            void *aes_ctx = NULL;
            size_t aes_ctx_size;
            aes_type_t type = AES_XTS;
            uint8_t plaintext[TEST_XTS_SIZE];
            uint8_t ciphertext[TEST_XTS_SIZE];

            for (i = 0; i < 3; i++) {
                if (i == 0) {
                    key_len = 16;
                } else if (i == 1) {
                    key_len = 24;
                } else {
                    key_len = 32;
                }

                result = ali_aes_get_ctx_size(type, &aes_ctx_size);
                if (result != ALI_CRYPTO_SUCCESS) {
                    CRYPT_ERR("get ctx size fail(%08x)\n", result);
                    goto _OUT;
                }

                aes_ctx = CRYPT_MALLOC(aes_ctx_size);
                if (aes_ctx == NULL) {
                    CRYPT_ERR("kmalloc(%d) fail\n", aes_ctx_size);
                    goto _OUT;
                }

                /* encrypt */
                CRYPT_MEMSET(aes_ctx, 0, aes_ctx_size);
                ret = _aes_xts_encrypt_decrypt(i, true,
                                               key_len, _g_test_data, ciphertext, aes_ctx);
                if (ret < 0) {
                    CRYPT_ERR("xts encrypt fail!\n");
                    goto _OUT;
                }

                /* decrypt */
                CRYPT_MEMSET(aes_ctx, 0, aes_ctx_size);
                ret = _aes_xts_encrypt_decrypt(i, false,
                                               key_len, ciphertext, plaintext, aes_ctx);
                if (ret < 0) {
                    CRYPT_ERR("xts decrypt fail!\n");
                    goto _OUT;
                }

                /* check result */
                if (CRYPT_MEMCMP(plaintext, _g_test_data, TEST_XTS_SIZE)) {
                    CRYPT_INF("AES_XTS_%d test fail!\n", key_len << 3);
                    ali_crypto_print_data("plaintext", plaintext, TEST_XTS_SIZE);
                    ali_crypto_print_data("ciphertext", ciphertext, TEST_XTS_SIZE);
                    goto _OUT;
                } else {
                    CRYPT_INF("AES_XTS_%d test success!\n", key_len << 3);
                }

                CRYPT_FREE(aes_ctx);
                aes_ctx = NULL;
            }

            return 0;

_OUT:
            if (aes_ctx) {
                CRYPT_FREE(aes_ctx);
            }

            return -1;
        }
#endif

        int ali_crypto_aes_test(void)
        {
            int ret;

#if EN_GCOV_COVERAGE
            /* for gcov coverage */
            ret = ali_aes_get_ctx_size(AES_ECB, NULL);
            if (ret == ALI_CRYPTO_SUCCESS) {
                return ALI_CRYPTO_ERROR;
            }
#endif

            CRYPT_INF("Test aes ecb:\n");
            ret = _ali_crypto_aes_ecb_test();
            if (ret < 0) {
                return ret;
            }

            CRYPT_INF("Test aes cbc:\n");
            ret = _ali_crypto_aes_cbc_test();
            if (ret < 0) {
                return ret;
            }

            CRYPT_INF("Test aes ctr:\n");
            ret = _ali_crypto_aes_ctr_test();
            if (ret < 0) {
                return ret;
            }

            CRYPT_INF("Test aes cfb:\n");
            ret = _ali_crypto_aes_cfb_test();
            if (ret < 0) {
                return ret;
            }

#if 0
            CRYPT_INF("Test aes cts:\n");
            ret = _ali_crypto_aes_cts_test();
            if (ret < 0) {
                return ret;
            }

            CRYPT_INF("Test aes xts:\n");
            ret = _ali_crypto_aes_xts_test();
            if (ret < 0) {
                return ret;
            }
#endif

            return 0;
        }
