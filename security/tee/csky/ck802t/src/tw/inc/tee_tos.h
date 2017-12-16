/**
 * Copyright (C) 2016 The YunOS Project. All rights reserved.
 **/

#ifndef _TEE_TOS_H_
#define _TEE_TOS_H_

#include "tee_types.h"
#include <stdarg.h>

#define TEE_SUCCESS                         (0x00000000)
#define TEE_ERROR_GENERIC                   (0xFFFF0000)
#define TEE_ERROR_ACCESS_DENIED             (0xFFFF0001)
#define TEE_ERROR_CANCEL                    (0xFFFF0002)
#define TEE_ERROR_ACCESS_CONFLICT           (0xFFFF0003)
#define TEE_ERROR_EXCESS_DATA               (0xFFFF0004)
#define TEE_ERROR_BAD_FORMAT                (0xFFFF0005)
#define TEE_ERROR_BAD_PARAMETERS            (0xFFFF0006)
#define TEE_ERROR_BAD_STATE                 (0xFFFF0007)
#define TEE_ERROR_ITEM_NOT_FOUND            (0xFFFF0008)
#define TEE_ERROR_NOT_IMPLEMENTED           (0xFFFF0009)
#define TEE_ERROR_NOT_SUPPORTED             (0xFFFF000A)
#define TEE_ERROR_NO_DATA                   (0xFFFF000B)
#define TEE_ERROR_OUT_OF_MEMORY             (0xFFFF000C)
#define TEE_ERROR_BUSY                      (0xFFFF000D)
#define TEE_ERROR_COMMUNICATION             (0xFFFF000E)
#define TEE_ERROR_SECURITY                  (0xFFFF000F)
#define TEE_ERROR_SHORT_BUFFER              (0xFFFF0010)
#define TEE_PENDING                         (0xFFFF2000)
#define TEE_ERROR_TIMEOUT                   (0xFFFF3001)
#define TEE_ERROR_OVERFLOW                  (0xFFFF300F)
#define TEE_ERROR_TARGET_DEAD               (0xFFFF3024)
#define TEE_ERROR_STORAGE_NO_SPACE          (0xFFFF3041)
#define TEE_ERROR_MAC_INVALID               (0xFFFF3071)
#define TEE_ERROR_SIGNATURE_INVALID         (0xFFFF3072)
#define TEE_ERROR_TIME_NOT_SET              (0xFFFF5000)
#define TEE_ERROR_TIME_NEEDS_RESET          (0xFFFF5001)

#define TEE_PARAM_TYPE_NONE             (0)
#define TEE_PARAM_TYPE_VALUE_INPUT      (1)
#define TEE_PARAM_TYPE_VALUE_OUTPUT     (2)
#define TEE_PARAM_TYPE_VALUE_INOUT      (3)
#define TEE_PARAM_TYPE_MEMREF_INPUT     (5)
#define TEE_PARAM_TYPE_MEMREF_OUTPUT    (6)
#define TEE_PARAM_TYPE_MEMREF_INOUT     (7)

#define TEE_ORIGIN_API                  (0x00000001)
#define TEE_ORIGIN_COMMS                (0x00000002)
#define TEE_ORIGIN_TEE                  (0x00000003)
#define TEE_ORIGIN_TRUSTED_APP          (0x00000004)

typedef uint32_t TEE_Result;

typedef struct
{
    uint32_t timeLow;
    uint16_t timeMid;
    uint16_t timeHiAndVersion;
    uint8_t clockSeqAndNode[8];
} TEE_UUID;

typedef struct
{
    uint32_t login;
    TEE_UUID uuid;
} TEE_Identity;

#define TEE_PARAM_TYPES(t0, t1, t2, t3) \
        ((t0) | ((t1) << 4) | ((t2) << 8) | ((t3) << 12))
#define TEE_PARAM_TYPE_GET(t, i) (((t) >> (i*4)) & 0xF)
typedef union
{
    struct
    {
        void *buffer;
        uint32_t size;
    } memref;
    struct
    {
        uint32_t a, b;
    } value;
} TEE_Param;


/* strings */
extern void *memcpy(void *dest, const void *src, size_t n);
extern void *memmove(void *dest, const void *src, size_t n);
extern void *memset(void *s, int c, size_t n);
extern int memcmp(const void *s1, const void *s2, size_t n);
extern char *strcpy(char *dest, const char *src);
extern char *strncpy(char *dest, const char *src, size_t n);
extern char *strcat(char *dest, const char *src);
extern char *strncat(char *dest, const char *src, size_t n);
extern int strcmp(const char *s1, const char *s2);
extern int strncmp(const char *s1, const char *s2, size_t n);
extern size_t strlen(const char *s);
extern size_t strnlen(const char *s, size_t maxlen);

/* pool */
extern void *malloc(size_t size);
extern void *realloc(void *ptr, size_t size);
extern void *memalign(size_t size, size_t alignment);
extern void free(void *ptr);

/* print */
extern void uart_print_uint8(unsigned char data);
extern void uart_print_uint32(unsigned int data);
extern void uart_print_string(char *data);
#ifdef CONFIG_DBG
extern int printf(const char *format, ...);
extern int sprintf(char *s, const char *format, ...);
extern int vsprintf(char *s, const char *format, va_list arg);
extern int snprintf(char *s, size_t maxlen, const char *format, ...);
extern int vsnprintf(char *s, size_t maxlen, const char *format, va_list arg);
#endif

#endif /* _TEE_TOS_H_ */
