/**
 * Copyright (c) 2017, Realtek Semiconductor Corporation. All rights reserved.
 */

#ifndef _BT_TYPES_H_
#define _BT_TYPES_H_

#ifdef __cplusplus
extern "C" {
#endif

/**
 * bt_types.h
 *
 * \name    BT_BYTE_ORDER
 * \brief   BT buffer/array byte-order utility macros.
 * \anchor  BT_BYTE_ORDER
 */
/**
 * \ingroup     BTTYPES
 */
/** Calculate integer bit count of b'1 */
#define INT_BIT_COUNT(integer, count)   {               \
        count = 0;                                      \
        while (integer)                                 \
        {                                               \
            count++;                                    \
            integer &= integer - 1;                     \
        }                                               \
    }

/** Stream skip len */
#define STREAM_SKIP_LEN(s, len)     {                   \
        s += len;                                       \
    }

/** Stream to array */
#define STREAM_TO_ARRAY(a, s, len)  {                   \
        uint32_t ii;                                    \
        for (ii = 0; ii < len; ii++)                    \
        {                                               \
            *((uint8_t *)(a) + ii) = *s++;              \
        }                                               \
    }

/** Array to stream */
#define ARRAY_TO_STREAM(s, a, len)  {                   \
        uint32_t ii;                                    \
        for (ii = 0; ii < len; ii++)                    \
        {                                               \
            *s++ = *((uint8_t *)(a) + ii);              \
        }                                               \
    }

/** Little Endian stream to uint8 */
#define LE_STREAM_TO_UINT8(u8, s)   {                   \
        u8  = (uint8_t)(*s);                            \
        s  += 1;                                        \
    }

/** Little Endian stream to uint16 */
#define LE_STREAM_TO_UINT16(u16, s) {                   \
        u16  = ((uint16_t)(*(s + 0)) << 0) +            \
               ((uint16_t)(*(s + 1)) << 8);             \
        s   += 2;                                       \
    }

/** Little Endian stream to uint24 */
#define LE_STREAM_TO_UINT24(u24, s) {                   \
        u24  = ((uint32_t)(*(s + 0)) <<  0) +           \
               ((uint32_t)(*(s + 1)) <<  8) +           \
               ((uint32_t)(*(s + 2)) << 16);            \
        s   += 3;                                       \
    }

/** Little Endian stream to uint32 */
#define LE_STREAM_TO_UINT32(u32, s) {                   \
        u32  = ((uint32_t)(*(s + 0)) <<  0) +           \
               ((uint32_t)(*(s + 1)) <<  8) +           \
               ((uint32_t)(*(s + 2)) << 16) +           \
               ((uint32_t)(*(s + 3)) << 24);            \
        s   += 4;                                       \
    }

/** Little Endian uint8 to stream */
#define LE_UINT8_TO_STREAM(s, u8)   {                   \
        *s++ = (uint8_t)(u8);                           \
    }

/** Little Endian uint16 to stream */
#define LE_UINT16_TO_STREAM(s, u16) {                   \
        *s++ = (uint8_t)((u16) >> 0);                   \
        *s++ = (uint8_t)((u16) >> 8);                   \
    }

/** Little Endian uint24 to stream */
#define LE_UINT24_TO_STREAM(s, u24) {                   \
        *s++ = (uint8_t)((u24) >>  0);                  \
        *s++ = (uint8_t)((u24) >>  8);                  \
        *s++ = (uint8_t)((u24) >> 16);                  \
    }

/** Little Endian uint32 to stream */
#define LE_UINT32_TO_STREAM(s, u32) {                   \
        *s++ = (uint8_t)((u32) >>  0);                  \
        *s++ = (uint8_t)((u32) >>  8);                  \
        *s++ = (uint8_t)((u32) >> 16);                  \
        *s++ = (uint8_t)((u32) >> 24);                  \
    }

/** Little Endian array to uint8 */
#define LE_ARRAY_TO_UINT8(u8, a)    {                   \
        u8  = (uint8_t)(*(a + 0));                      \
    }

/** Little Endian array to uint16 */
#define LE_ARRAY_TO_UINT16(u16, a)  {                   \
        u16 = ((uint16_t)(*(a + 0)) << 0) +             \
              ((uint16_t)(*(a + 1)) << 8);              \
    }

/** Little Endian array to uint24 */
#define LE_ARRAY_TO_UINT24(u24, a)  {                   \
        u24 = ((uint32_t)(*(a + 0)) <<  0) +            \
              ((uint32_t)(*(a + 1)) <<  8) +            \
              ((uint32_t)(*(a + 2)) << 16);             \
    }

/** Little Endian array to uint32 */
#define LE_ARRAY_TO_UINT32(u32, a) {                    \
        u32 = ((uint32_t)(*(a + 0)) <<  0) +            \
              ((uint32_t)(*(a + 1)) <<  8) +            \
              ((uint32_t)(*(a + 2)) << 16) +            \
              ((uint32_t)(*(a + 3)) << 24);             \
    }

/** Little Endian uint8 to array */
#define LE_UINT8_TO_ARRAY(a, u8)    {                   \
        *((uint8_t *)(a) + 0) = (uint8_t)(u8);          \
    }

/** Little Endian uint16 to array */
#define LE_UINT16_TO_ARRAY(a, u16)  {                   \
        *((uint8_t *)(a) + 0) = (uint8_t)((u16) >> 0);  \
        *((uint8_t *)(a) + 1) = (uint8_t)((u16) >> 8);  \
    }

/** Little Endian uint24 to array */
#define LE_UINT24_TO_ARRAY(a, u24) {                    \
        *((uint8_t *)(a) + 0) = (uint8_t)((u24) >>  0); \
        *((uint8_t *)(a) + 1) = (uint8_t)((u24) >>  8); \
        *((uint8_t *)(a) + 2) = (uint8_t)((u24) >> 16); \
    }

/** Little Endian uint32 to array */
#define LE_UINT32_TO_ARRAY(a, u32) {                    \
        *((uint8_t *)(a) + 0) = (uint8_t)((u32) >>  0); \
        *((uint8_t *)(a) + 1) = (uint8_t)((u32) >>  8); \
        *((uint8_t *)(a) + 2) = (uint8_t)((u32) >> 16); \
        *((uint8_t *)(a) + 3) = (uint8_t)((u32) >> 24); \
    }

/** Big Endian stream to uint8 */
#define BE_STREAM_TO_UINT8(u8, s)   {                   \
        u8   = (uint8_t)(*(s + 0));                     \
        s   += 1;                                       \
    }

/** Big Endian stream to uint16 */
#define BE_STREAM_TO_UINT16(u16, s) {                   \
        u16  = ((uint16_t)(*(s + 0)) << 8) +            \
               ((uint16_t)(*(s + 1)) << 0);             \
        s   += 2;                                       \
    }

/** Big Endian stream to uint24 */
#define BE_STREAM_TO_UINT24(u24, s) {                   \
        u24  = ((uint32_t)(*(s + 0)) << 16) +           \
               ((uint32_t)(*(s + 1)) <<  8) +           \
               ((uint32_t)(*(s + 2)) <<  0);            \
        s   += 3;                                       \
    }

/** Big Endian stream to uint32 */
#define BE_STREAM_TO_UINT32(u32, s) {                   \
        u32  = ((uint32_t)(*(s + 0)) << 24) +           \
               ((uint32_t)(*(s + 1)) << 16) +           \
               ((uint32_t)(*(s + 2)) <<  8) +           \
               ((uint32_t)(*(s + 3)) <<  0);            \
        s   += 4;                                       \
    }

/** Big Endian uint8 to stream */
#define BE_UINT8_TO_STREAM(s, u8)   {                   \
        *s++ = (uint8_t)(u8);                           \
    }

/** Big Endian uint16 to stream */
#define BE_UINT16_TO_STREAM(s, u16) {                   \
        *s++ = (uint8_t)((u16) >> 8);                   \
        *s++ = (uint8_t)((u16) >> 0);                   \
    }

/** Big Endian uint24 to stream */
#define BE_UINT24_TO_STREAM(s, u24) {                   \
        *s++ = (uint8_t)((u24) >> 16);                  \
        *s++ = (uint8_t)((u24) >>  8);                  \
        *s++ = (uint8_t)((u24) >>  0);                  \
    }

/** Big Endian uint32 to stream */
#define BE_UINT32_TO_STREAM(s, u32) {                   \
        *s++ = (uint8_t)((u32) >> 24);                  \
        *s++ = (uint8_t)((u32) >> 16);                  \
        *s++ = (uint8_t)((u32) >>  8);                  \
        *s++ = (uint8_t)((u32) >>  0);                  \
    }

/** Big Endian array to uint8 */
#define BE_ARRAY_TO_UINT8(u8, a)    {                   \
        u8  = (uint8_t)(*(a + 0));                      \
    }

/** Big Endian array to uint16 */
#define BE_ARRAY_TO_UINT16(u16, a)  {                   \
        u16 = ((uint16_t)(*(a + 0)) << 8) +             \
              ((uint16_t)(*(a + 1)) << 0);              \
    }

/** Big Endian array to uint24 */
#define BE_ARRAY_TO_UINT24(u24, a)  {                   \
        u24 = ((uint32_t)(*(a + 0)) << 16) +            \
              ((uint32_t)(*(a + 1)) <<  8) +            \
              ((uint32_t)(*(a + 2)) <<  0);             \
    }

/** Big Endian array to uint32 */
#define BE_ARRAY_TO_UINT32(u32, a)  {                   \
        u32 = ((uint32_t)(*(a + 0)) << 24) +            \
              ((uint32_t)(*(a + 1)) << 16) +            \
              ((uint32_t)(*(a + 2)) <<  8) +            \
              ((uint32_t)(*(a + 3)) <<  0);             \
    }

/** Big Endian uint8 to array */
#define BE_UINT8_TO_ARRAY(a, u8)    {                   \
        *((uint8_t *)(a) + 0) = (uint8_t)(u8);          \
    }

/** Big Endian uint16 to array */
#define BE_UINT16_TO_ARRAY(a, u16)  {                   \
        *((uint8_t *)(a) + 0) = (uint8_t)((u16) >> 8);  \
        *((uint8_t *)(a) + 1) = (uint8_t)((u16) >> 0);  \
    }

/** Big Endian uint24 to array */
#define BE_UINT24_TO_ARRAY(a, u24)  {                   \
        *((uint8_t *)(a) + 0) = (uint8_t)((u24) >> 16); \
        *((uint8_t *)(a) + 1) = (uint8_t)((u24) >>  8); \
        *((uint8_t *)(a) + 2) = (uint8_t)((u24) >>  0); \
    }

/** Big Endian uint32 to array */
#define BE_UINT32_TO_ARRAY(a, u32)  {                   \
        *((uint8_t *)(a) + 0) = (uint8_t)((u32) >> 24); \
        *((uint8_t *)(a) + 1) = (uint8_t)((u32) >> 16); \
        *((uint8_t *)(a) + 2) = (uint8_t)((u32) >>  8); \
        *((uint8_t *)(a) + 3) = (uint8_t)((u32) >>  0); \
    }

#ifdef __cplusplus
}
#endif

#endif /* _BT_TYPES_H_ */
