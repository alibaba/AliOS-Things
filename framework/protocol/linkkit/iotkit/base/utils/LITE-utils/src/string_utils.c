/*
 * Copyright (c) 2014-2016 Alibaba Group. All rights reserved.
 * License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */


#include "lite-utils_internal.h"
#include "string_utils.h"

char *LITE_format_string(const unsigned int len, const char *fmt, ...)
{
#define TEMP_STRING_MAXLEN      (512)

    char           *tmp = NULL;
    char           *module_name = NULL;
    unsigned int    n;
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

    n = (len != 0) ? len : TEMP_STRING_MAXLEN;
    tmp = LITE_malloc(n + 1, magic, module_name);

    LITE_ASSERT(tmp);
    if (NULL == tmp) {
        return NULL;
    }
    memset(tmp, 0, n + 1);
    UTILS_vsnprintf(tmp, n, fmt, ap);
    va_end(ap);

    return tmp;

#undef TEMP_STRING_MAXLEN
}

char *LITE_format_nstring(const int len, const char *fmt, ...)
{
    char           *tmp = NULL;
    char           *dst;
    char           *module_name = NULL;
    int             rc = -1;
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
        return NULL;
    }
    memset(tmp, 0, len + 2);
    rc = UTILS_vsnprintf(tmp, len + 1, fmt, ap);
    va_end(ap);
    LITE_ASSERT(tmp);
    LITE_ASSERT(rc < 1024);

    dst = LITE_malloc(len + 1, magic, module_name);
    LITE_snprintf(dst, (len + 1), "%s", tmp);
    LITE_free(tmp);
    return dst;

}

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
        log_err("Too long string to duplicate, abort! len = %d", len);
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

void LITE_hexstr_convert(char *hexstr, uint8_t *out_buf, int in_len)
{
    int             i = 0;
    uint8_t         ch0, ch1;

    if (in_len % 2 != 0) {
        log_err("hexstr length (%d) is not even", in_len);
        return;
    }

    while (i < in_len / 2) {
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

