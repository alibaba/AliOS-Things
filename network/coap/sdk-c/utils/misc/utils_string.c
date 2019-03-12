/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */
#ifdef UTILS_STRING

#include "iotx_utils_internal.h"
#include "string_utils.h"
#include "iotx_utils_internal.h"

#define UTILS_STRING_MALLOC(size) LITE_malloc(size, MEM_MAGIC, "utils.string")
#define UTILS_STRING_FREE(ptr)    LITE_free(ptr)

char *LITE_strdup(const char *src, ...)
{
    int             len = 0;
    char           *dst = NULL;
    int             magic = 0;
    char           *module_name = NULL;

    if (!src) {
        return NULL;
    }
    len = strlen(src) + 1;
    if (len > 1024) {
        utils_err("Too long string to duplicate, abort! len = %d", len);
        return NULL;
    }

#if WITH_MEM_STATS_PER_MODULE
    va_list         ap;
    va_start(ap, src);
    magic = va_arg(ap, int);
    if (MEM_MAGIC == magic) {
        module_name = va_arg(ap, char *);
    }
    va_end(ap);
#endif

    dst = (char *)LITE_malloc(sizeof(char) * len, magic, module_name);

    if (!dst) {
        return NULL;
    }
    strncpy(dst, src, len);

    return dst;
}

void LITE_hexbuf_convert(unsigned char *digest, char *out, int in_len, int uppercase)
{
    static char    *zEncode[] = {"0123456789abcdef", "0123456789ABCDEF"};
    int             j = 0;
    int             i = 0;
    int             idx = uppercase ? 1 : 0;

    for (i = 0; i < in_len; i ++) {
        int         a = digest[i];

        out[j++] = zEncode[idx][(a >> 4) & 0xf];
        out[j++] = zEncode[idx][a & 0xf];
    }
}

static uint8_t _hexval_of_char(char hex)
{
    if (LITE_isdigit(hex)) {
        return (hex - '0');
    }
    if (hex >= 'a' && hex <= 'f') {
        return (hex - 'a' + 10);
    }
    if (hex >= 'A' && hex <= 'F') {
        return (hex - 'A' + 10);
    }

    return 0;
}

void LITE_hexstr_convert(char *input, int input_len, unsigned char *output, int output_len)
{
    int             i = 0;
    uint8_t         ch0, ch1;

    if (input_len % 2 != 0) {
        utils_err("hexstr length (%d) is not even", input_len);
        return;
    }

    while (i < input_len / 2 && i < output_len) {
        ch0 = _hexval_of_char((char)input[2 * i]);
        ch1 = _hexval_of_char((char)input[2 * i + 1]);
        output[i] = (ch0 << 4 | ch1);
        i++;
    }
}

int LITE_get_randstr(_OU_ char *random, _IN_ int length)
{
    int index = 0;

    if (random == NULL || length <= 0) {
        utils_err("Invalid Parameter");
        return FAIL_RETURN;
    }

    HAL_Srandom(HAL_UptimeMs());

    for (index = 0; index < length; index++) {
        switch (HAL_Random(3)) {
            case 0: {
                random[index] = 'A' + HAL_Random(26);
            }
            break;
            case 1: {
                random[index]  = 'a' + HAL_Random(26);
            }
            break;
            case 2: {
                random[index] = '0' + HAL_Random(10);
            }
            break;
            default: {
                utils_err("Should Not Execute Here");
                return FAIL_RETURN;
            }
        }
    }

    return SUCCESS_RETURN;
}

int8_t utils_hb2hex(uint8_t hb)
{
    hb = hb & 0xF;
    return (int8_t)(hb < 10 ? '0' + hb : hb - 10 + 'a');
}

#if WITH_STRING_UTILS_EXT
char *LITE_format_string(const char *fmt, ...)
{
#define TEMP_STRING_MAXLEN      (512)

    va_list         ap;
    char           *tmp = NULL;
    char           *dst;
    int             rc = -1;

    va_start(ap, fmt);
    tmp = UTILS_STRING_MALLOC(TEMP_STRING_MAXLEN);
    LITE_ASSERT(tmp);
    memset(tmp, 0, TEMP_STRING_MAXLEN);
    rc = HAL_Vsnprintf(tmp, TEMP_STRING_MAXLEN, fmt, ap);
    va_end(ap);
    LITE_ASSERT(rc < 1024);

    dst = LITE_strdup(tmp);
    UTILS_STRING_FREE(tmp);

    return dst;

#undef TEMP_STRING_MAXLEN
}

char *LITE_format_nstring(const int len, const char *fmt, ...)
{
    char           *tmp = NULL;
    char           *dst;
    char           *module_name = NULL;
    int             magic = 0;
    va_list         ap;

    va_start(ap, fmt);
    magic = va_arg(ap, int);
    if (MEM_MAGIC == magic) {
        module_name = va_arg(ap, char *);
    }
    if (!module_name) {
        va_start(ap, fmt);
    }

    tmp = LITE_malloc(len + 2, magic, module_name);
    if (NULL == tmp) {
        va_end(ap);
        return NULL;
    }
    memset(tmp, 0, len + 2);
    UTILS_vsnprintf(tmp, len + 1, fmt, ap);
    va_end(ap);

    dst = LITE_malloc(len + 1, magic, module_name);
    if (!dst) {
        LITE_free(tmp);
        return NULL;
    }
    LITE_snprintf(dst, (len + 1), "%s", tmp);
    LITE_free(tmp);
    return dst;

}

void LITE_replace_substr(char originalString[], char key[], char swap[])
{
    int         lengthOfOriginalString, lengthOfKey, lengthOfSwap, lengthOfRemain, i, j, flag;
    char        tmp[512] = {0};

    lengthOfOriginalString = strlen(originalString);
    lengthOfKey = strlen(key);
    lengthOfSwap = strlen(swap);

    if (lengthOfOriginalString >= 512 || (lengthOfOriginalString - lengthOfKey + lengthOfSwap) >= 512) {
        return;
    }

    for (i = 0; i <= lengthOfOriginalString - lengthOfKey; i++) {
        flag = 1;
        for (j  = 0; j < lengthOfKey; j++) {
            if (originalString[i + j] != key[j]) {
                flag = 0;
                break;
            }
        }

        if (flag) {
            memcpy(tmp, originalString, lengthOfOriginalString + 1);
            memcpy(&tmp[i], swap, lengthOfSwap + 1);

            lengthOfRemain = strlen(&originalString[i  + lengthOfKey]);
            if (lengthOfRemain >= (512 - i - lengthOfSwap)) {
                return;
            }

            memcpy(tmp + i + lengthOfSwap, originalString + i + lengthOfKey, lengthOfRemain + 1);
            memcpy(originalString, tmp, strlen(tmp) + 1);
            i += lengthOfSwap - 1;
            lengthOfOriginalString = strlen(originalString);
        }
    }
}

#endif  /* #if WITH_STRING_UTILS_EXT */
#endif