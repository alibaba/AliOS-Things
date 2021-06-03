/**
 * ant_typedef.h
 *
 * 基本类型定义接口
 *
 */
#ifndef __ANT_TYPEDEF_H__
#define __ANT_TYPEDEF_H__
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

#define ANT_SUCCESS        0
#define ANT_FAIL           -1

typedef char               ant_s8;
typedef char               ant_char;
typedef short              ant_s16;
typedef int                ant_s32;
typedef unsigned char      ant_u8;
typedef unsigned short     ant_u16;
typedef unsigned int       ant_u32;
typedef unsigned int       ant_size;
typedef signed long long   ant_s64;
typedef unsigned long long ant_u64;
typedef unsigned long      ant_ptr;

typedef enum {
    ANT_FALSE = 0,
    ANT_TRUE  = 1
} ant_bool;

#ifdef __cplusplus
} // extern "C"
#endif

#endif

