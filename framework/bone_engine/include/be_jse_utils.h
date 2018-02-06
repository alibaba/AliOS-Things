/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

/*
 * be_jse_utils.h
 *
 * utils函数
 *
 */

#ifndef BE_JSE_UTILS_H_
#define BE_JSE_UTILS_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "be_jse_types.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef alloca
#define alloca(x) __builtin_alloca(x)
#endif

#if defined(ESP8266)

#define USE_FLASH_MEMORY
#define IN_FLASH_MEMORY   __attribute__((section(".irom.literal"))) __attribute__((aligned(4)))

#define READ_FLASH_UINT8(ptr) ({ uint32_t __p = (uint32_t)(char*)(ptr); volatile uint32_t __d = *(uint32_t*)(__p & (uint32_t)~3); ((uint8_t*)&__d)[__p & 3]; })
#define READ_FLASH_UINT16(ptr) (READ_FLASH_UINT8(ptr) | (READ_FLASH_UINT8(((char*)ptr)+1)<<8) )

#define CALLED_FROM_INTERRUPT __attribute__((section(".iram1.text")))

#define itoa(val,str,base) os_sprintf(str,"%d",val)
#define ftoa(val,str)      os_sprintf(str,"%f",val)

#define printf os_printf

#else
#define CALLED_FROM_INTERRUPT
#define itoa(val,str,base) sprintf(str,"%d",val)
#define ftoa(val,str)      sprintf(str,"%g",val)
#endif

#if defined(__GNUC__) && !defined(__clang__)
#if defined(LINK_TIME_OPTIMISATION) && !defined(SAVE_ON_FLASH) && !defined(DEBUG)
#define ALWAYS_INLINE __attribute__ ((gnu_inline)) __attribute__((always_inline)) inline
#else
#define ALWAYS_INLINE __attribute__ ((gnu_inline)) inline
#endif
#else
#define ALWAYS_INLINE
#endif


#if 1
//#ifdef DEBUG
#define be_assert(X) if (!(X)) be_jse_assert_fail(__FILE__,__LINE__);
#else
#define be_assert(X) do { } while(0)
#endif

#define NOT_USED(x) ( (void)(x) )

bool is_whitespace(char ch);
bool is_numeric(char ch);
bool is_hex_decimal(char ch);
bool is_alpha(char ch);

// ------------
void be_jse_error(const char *message);
void be_jse_error_at(const char *message, be_jse_lex_ctx_t *lex, int tokenPos);
void be_jse_warn(const char *message);
void be_jse_warn_at(const char *message, be_jse_lex_ctx_t *lex, int tokenPos);
void be_jse_assert_fail(const char *file, int line);

// C99中函数的实现,如 strtol、snprintf、atol、atof、itoa、ftoa
// ESP8266中的libc部份支持C99

void itostr(be_jse_int_t val,char *str,unsigned int base);
void ftoa_bounded(be_jse_float_t val,char *str, size_t len);
int  chtod(char ch);

be_jse_float_t string_to_float(const char *str);
long long string_to_int(const char *s);


#ifndef BE_JSE_SILENT
// 仅仅用于调试
void be_jse_save_tmp_token(const char* token);
const char* be_jse_get_tmp_token();

#endif
#ifdef __cplusplus
}
#endif

#endif /* BE_JSE_UTILS_H_ */
