/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

/*
 * be_jse_utils.h
 *
 * utils函数
 *
 */

#ifndef BE_JSE_UTILS_H
#define BE_JSE_UTILS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "be_jse_types.h"

#ifdef __cplusplus
extern "C" {
#endif

#define itoa(val, str, base) sprintf(str, "%d", val)
#define ftoa(val, str) sprintf(str, "%g", val)

#if defined(__GNUC__) && !defined(__clang__)
#if defined(LINK_TIME_OPTIMISATION) && !defined(SAVE_ON_FLASH) && \
    !defined(DEBUG)
#define ALWAYS_INLINE \
    __attribute__((gnu_inline)) __attribute__((always_inline)) inline
#else
#define ALWAYS_INLINE __attribute__((gnu_inline)) inline
#endif
#else
#define ALWAYS_INLINE
#endif

#if 1
/* #ifdef DEBUG */
#define be_assert(X) \
    if (!(X)) be_jse_assert_fail(__FILE__, __LINE__);
#else
#define be_assert(X) \
    do {             \
    } while (0)
#endif

#define NOT_USED(x) ((void)(x))

bool is_whitespace(char ch);
bool is_numeric(char ch);
bool is_hex_decimal(char ch);
bool is_alpha(char ch);

void be_jse_error(const char *message);
void be_jse_error_at(const char *message, be_jse_lexer_ctx_t *lex,
                     int tokenPos);
void be_jse_warn(const char *message);
void be_jse_warn_at(const char *message, be_jse_lexer_ctx_t *lex, int tokenPos);
void be_jse_assert_fail(const char *file, int line);

/* C99中函数的实现,如 strtol、snprintf、atol、atof、itoa、ftoa
   ESP8266中的libc部份支持C99 */

void itostr(be_jse_int_t val, char *str, unsigned int base);
void ftoa_bounded(be_jse_float_t val, char *str, size_t len);
int chtod(char ch);

be_jse_float_t string_to_float(const char *str);
long long string_to_int(const char *s);

#ifndef BE_JSE_SILENT
/* 仅仅用于调试 */
void be_jse_save_tmp_token(const char *token);
const char *be_jse_get_tmp_token(void);

#endif

/*
 * color def.
 * see
 * http://stackoverflow.com/questions/3585846/color-text-in-terminal-applications-in-unix
 */
#define COL_DEF "\x1B[0m"  /* white */
#define COL_RED "\x1B[31m" /* red */
#define COL_GRE "\x1B[32m" /* green */
#define COL_BLU "\x1B[34m" /* blue */
#define COL_YEL "\x1B[33m" /* yellow */
#define COL_WHE "\x1B[37m" /* white */
#define COL_CYN "\x1B[36m"
#define COL_MAG "\x1B[35m"

#define GLOBAL_LOG_TAG \
    "\e[65m" /* LOG TAG, use ESC characters, g(cli) ascii is 65 */

void be_jse_post_async(be_jse_symbol_t *func, be_jse_symbol_t **params,
                       int param_count);

#ifdef __cplusplus
}
#endif

#endif /* BE_JSE_UTILS_H */
