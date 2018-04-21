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


#ifndef __LITE_UTILS_H__
#define __LITE_UTILS_H__

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <stddef.h>
#if defined(_PLATFORM_IS_LINUX_)
    #include <assert.h>
#endif

#include "lite-list.h"
#include "lite-log.h"

#ifndef UTILS_printf
#define UTILS_printf                printf
#endif

#define LITE_TRUE                   (1)
#define LITE_FALSE                  (0)

#ifndef container_of
#define container_of(ptr, type, member)  \
    ((type *) ((char *) (ptr) - offsetof(type, member)))
#endif

#define LITE_MINIMUM(a, b)          (((a) <= (b)) ? (a) : (b))
#define LITE_MAXIMUM(a, b)          (((a) >= (b)) ? (a) : (b))
#define LITE_isdigit(c)             (((c) <= '9' && (c) >= '0') ? (LITE_TRUE) : (LITE_FALSE))

#if defined(_PLATFORM_IS_LINUX_)
#define LITE_ASSERT(expr)           assert(expr)
#else
#define LITE_ASSERT(expr) \
    do { \
        if (!(expr)) { \
            UTILS_printf("### %s | %s(%d): ASSERT FAILED ###: %s is FALSE\r\n", \
                         __FILE__, __func__, __LINE__, #expr); \
        } \
    } while(0)
#endif

#define MEM_MAGIC                       (0x1234)

#define LITE_calloc(num, size, ...)     LITE_malloc_internal(__func__, __LINE__, (num * size), ##__VA_ARGS__)
#define LITE_malloc(size, ...)          LITE_malloc_internal(__func__, __LINE__, size, ##__VA_ARGS__)
#define LITE_realloc(ptr, size, ...)    LITE_realloc_internal(__func__, __LINE__, ptr, size, ##__VA_ARGS__)
#define LITE_free(ptr)              \
    do { \
        if(!ptr) { \
            log_err("%s == NULL! LITE_free(%s) aborted.", #ptr, #ptr); \
            break; \
        } \
        \
        LITE_free_internal((void *)ptr); \
        ptr = NULL; \
    } while(0)

void       *LITE_malloc_internal(const char *f, const int l, int size, ...);
void       *LITE_realloc_internal(const char *f, const int l, void *ptr, int size, ...);
void        LITE_free_internal(void *ptr);
void       *LITE_malloc_routine(int size, ...);
void        LITE_free_routine(void *ptr);
void       *LITE_calloc_routine(size_t n, size_t s, ...);

char       *LITE_strdup(const char *src, ...);
char       *LITE_format_string(const unsigned int len, const char *fmt, ...);
char       *LITE_format_nstring(const int len, const char *fmt, ...);
void        LITE_hexbuf_convert(unsigned char *digest, char *out, int buflen, int uppercase);
void        LITE_hexstr_convert(char *hexstr, uint8_t *out_buf, int len);
void        LITE_replace_substr(char orig[], char key[], char swap[]);

void        LITE_dump_malloc_free_stats(int level);
void        LITE_track_malloc_callstack(int state);

char           *LITE_json_value_of(char *key, char *src, ...);
list_head_t    *LITE_json_keys_of(char *src, char *prefix, ...);

char           *LITE_json_value_of_ext(char *key, char *src, ...);
char           *LITE_json_value_of_ext2(char *key, char *src, int src_len, int *value_len);

list_head_t    *LITE_json_keys_of_ext(char *src, char *prefix, ...);

int             get_json_item_size(char *src, int src_len);

void            LITE_json_keys_release(list_head_t *keylist);

typedef struct _json_key_t {
    char           *key;
    list_head_t     list;
} json_key_t;

#define foreach_json_keys_in(src, iter_key, keylist, pos)   \
    for(keylist = (void *)LITE_json_keys_of((char *)src, ""), \
        pos = (void *)list_first_entry((list_head_t *)keylist, json_key_t, list), \
        iter_key = ((json_key_t *)pos)->key; \
        (iter_key = ((json_key_t *)pos)->key); \
        pos = list_next_entry((json_key_t *)pos, list, json_key_t))

int demo_string_utils(void);
int demo_json_parser(void);
int demo_json_token(void);
int demo_json_array_token(void);
int demo_json_array_parse(void);

#endif  /* __LITE_UTILS_H__ */
