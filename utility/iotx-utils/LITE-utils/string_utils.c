/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include "lite-utils_internal.h"
#include "string_utils.h"

char *LITE_format_string(const char *fmt, ...)
{
    va_list         ap;
    char           tmp[1024];
    char           *dst;
    int             rc = -1;

    va_start(ap, fmt);
    rc = vsprintf(tmp, fmt, ap);
    va_end(ap);
    assert(tmp);
    assert(rc < 1024);

    dst = LITE_strdup(tmp);
    //free(tmp);

    return dst;
}

char *LITE_format_nstring(const int len, const char *fmt, ...)
{
    va_list         ap;
    char           tmp[1024];
    char           *dst;
    int             rc = -1;

    va_start(ap, fmt);
    rc = vsprintf(tmp, fmt, ap);
    va_end(ap);
    assert(tmp);
    assert(rc < 1024);

    dst = LITE_malloc(len + 1);
    LITE_snprintf(dst, (len + 1), "%s", tmp);
    //free(tmp);

    return dst;
}

char *LITE_strdup(const char *src)
{
    int             len = 0;
    char           *dst = NULL;

    if (!src) {
        return NULL;
    }
    len = strlen(src) + 1;
    if (len > 1024) {
        log_err("Too long string to duplicate, abort! len = %d", len);
        return NULL;
    }

    dst = (char *)LITE_malloc(sizeof(char) * len);
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

void LITE_hexstr_convert(char *hexstr, uint8_t *out_buf, int in_len)
{
    int             i = 0;
    uint8_t         ch0, ch1;

    if (in_len % 2 != 0) {
        log_err("hexstr length (%d) is not even", in_len);
        return;
    }

    while (i < in_len) {
        ch0 = _hexval_of_char((char)hexstr[2 * i]);
        ch1 = _hexval_of_char((char)hexstr[2 * i + 1]);
        out_buf[i] = (ch0 << 4 | ch1);
        i++;
    }
}

void LITE_replace_substr(char originalString[], char key[], char swap[])
{
    int         lengthOfOriginalString, lengthOfKey, lengthOfSwap, i, j, flag;
    char        tmp[512];

    lengthOfOriginalString = strlen(originalString);
    lengthOfKey = strlen(key);
    lengthOfSwap = strlen(swap);

    for (i = 0; i <= lengthOfOriginalString - lengthOfKey; i++) {
        flag = 1;
        for (j  = 0; j < lengthOfKey; j++) {
            if (originalString[i + j] != key[j]) {
                flag = 0;
                break;
            }
        }

        if (flag) {
            strcpy(tmp, originalString);
            strcpy(&tmp[i], swap);
            strcpy(&tmp[i + lengthOfSwap], &originalString[i  + lengthOfKey]);
            strcpy(originalString, tmp);
            i += lengthOfSwap - 1;
            lengthOfOriginalString = strlen(originalString);
        }
    }
}
