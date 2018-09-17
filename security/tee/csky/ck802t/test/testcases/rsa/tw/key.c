/**
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <rsa_test.h>

uint8_t modulus[] = {
    0xdf, 0x83, 0xe4, 0x76, 0x2d, 0x00, 0x61, 0xf6, 0xd0, 0x8d, 0x4a, 0x04,
    0x66, 0xb1, 0xd5, 0x55, 0xef, 0x71, 0xb5, 0xa5, 0x4e, 0x69, 0x44, 0xd3,
    0x4f, 0xb8, 0x3d, 0xec, 0xb1, 0x1d, 0x5f, 0x82, 0x6a, 0x48, 0x21, 0x00,
    0x7f, 0xd7, 0xd5, 0xf6, 0x82, 0x35, 0xc2, 0xa6, 0x67, 0xa3, 0x53, 0x2d,
    0x3a, 0x83, 0x9a, 0xba, 0x60, 0xc2, 0x11, 0x22, 0xc2, 0x35, 0x83, 0xe9,
    0x10, 0xa1, 0xb4, 0xa6, 0x74, 0x57, 0x99, 0xd3, 0xa8, 0x6a, 0x21, 0x83,
    0x76, 0xc1, 0x67, 0xde, 0xd8, 0xec, 0xdf, 0xf7, 0xc0, 0x1b, 0xf6, 0xfa,
    0x14, 0xa4, 0x0a, 0xec, 0xd1, 0xee, 0xc0, 0x76, 0x4c, 0xcd, 0x4a, 0x0a,
    0x5c, 0x96, 0xf2, 0xc9, 0xa4, 0x67, 0x03, 0x97, 0x2e, 0x17, 0xcd, 0xa9,
    0x27, 0x9d, 0xa6, 0x35, 0x5f, 0x7d, 0xb1, 0x6b, 0x68, 0x0e, 0x99, 0xc7,
    0xdd, 0x5d, 0x6f, 0x15, 0xce, 0x8e, 0x85, 0x33,
};
uint8_t publicExponent[] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x01,
};
uint8_t privateExponent[] = {
    0xc6, 0x15, 0x3d, 0x02, 0xfe, 0x1e, 0xb8, 0xb2, 0xe3, 0x60, 0x53, 0x98,
    0x52, 0xea, 0x87, 0x06, 0x01, 0x8d, 0xe4, 0x4c, 0xfb, 0x90, 0x8f, 0x4e,
    0x35, 0xf8, 0x31, 0xe8, 0xf1, 0x8d, 0xf6, 0x76, 0xbd, 0x79, 0xee, 0xc5,
    0x62, 0x87, 0x05, 0x37, 0xd1, 0x6d, 0x93, 0x73, 0xa5, 0xa5, 0x38, 0xb1,
    0x7c, 0x89, 0xe5, 0x36, 0x07, 0x49, 0xf5, 0xa5, 0xb8, 0x37, 0x75, 0x0f,
    0xb7, 0x8d, 0x97, 0x69, 0xc4, 0xd4, 0x8a, 0xb7, 0xfe, 0x74, 0x48, 0x45,
    0x58, 0x47, 0x29, 0xa3, 0x0b, 0xa7, 0xdc, 0x55, 0x98, 0x18, 0x8c, 0xd4,
    0x52, 0xf5, 0xc9, 0xe8, 0x40, 0xce, 0x97, 0x46, 0x14, 0x1f, 0x62, 0x94,
    0xc3, 0x21, 0x1e, 0x5d, 0x49, 0x59, 0x31, 0xeb, 0xc4, 0x95, 0xf9, 0x33,
    0x70, 0xa7, 0x90, 0xc3, 0x9e, 0x98, 0x58, 0xa4, 0x00, 0xa4, 0x0f, 0xf3,
    0x51, 0x80, 0xc6, 0x14, 0xfb, 0xd5, 0x5b, 0x01,
};
