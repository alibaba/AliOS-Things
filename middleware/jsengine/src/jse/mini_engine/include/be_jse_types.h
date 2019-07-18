/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#ifndef BE_JSE_TYPES_H
#define BE_JSE_TYPES_H

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include "be_jse_global.h"

typedef enum {
    BE_SYM_NUM_MASK  = 8, /* 01000 ~ 01111 之间的都是数字, bit3==1 */
    BE_SYM_TYPE_MASK = 15,

    BE_SYM_NULL = 1, /* NULL */

    BE_SYM_STRING     = 2, /* 字符串 */
    BE_SYM_STRING_EXT = 3, /* 字符串扩展 */
    BE_SYM_ARRAY      = 4, /* 数组 */

    BE_SYM_OBJECT   = 5, /* 对象 */
    BE_SYM_FUNCTION = 6, /* 函数 */
    BE_SYM_INTEGER  = 8, /* 整型 */
    BE_SYM_FLOAT    = 9, /* 浮点型 */

    BE_SYM_NAME   = 16, /* 名称 */
    BE_SYM_NATIVE = 32, /* 本地扩展 */

    BE_SYM_RECURSING = 128,
    BE_SYM_FUNCTION_ARGVS =
        BE_SYM_FUNCTION | BE_SYM_NAME, /* 等同 BE_SYM_STRING */

    BE_SYM_ROOT = BE_SYM_OBJECT | BE_SYM_NATIVE,

} be_jse_symbol_type_e;

typedef enum {

    /* 变量类型 */
    BE_TOKEN_ID = 256,

    BE_TOKEN_INT,
    BE_TOKEN_FLOAT,
    BE_TOKEN_STR,

    /* 运算操作符 */

    /* 逻辑判断 */

    BE_TOKEN_OP_EQUAL,         /* == */
    BE_TOKEN_OP_STRICT_EQUAL,  /* === */
    BE_TOKEN_OP_NEQUAL,        /* != */
    BE_TOKEN_OP_STRICT_NEQUAL, /* !=== */
    BE_TOKEN_OP_LESS_EQUAL,    /* <= */

    /* 移位操作 */

    BE_TOKEN_OP_LEFT_SHIFT,
    BE_TOKEN_OP_LEFT_SHIFT_EQL,

    BE_TOKEN_OP_MORE_EQUAL,
    BE_TOKEN_OP_RIGHT_SHIFT,

    BE_TOKEN_OP_RIGHT_SHIFT_UNSIGN,
    BE_TOKEN_OP_RIGHT_SHIFT_EQUAL,

    /* += 或者 -= */

    BE_TOKEN_OP_PLUS_EQUAL,
    BE_TOKEN_OP_MINUS_EQUAL,

    /* *= 或者/= */
    BE_TOKEN_OP_MUL_EQUAL,
    BE_TOKEN_OP_DIV_EQUAL,

    /* ++ 或者 -- */

    BE_TOKEN_OP_PLUS_PLUS,
    BE_TOKEN_OP_MINUS_MINUS,

    /* 逻辑运算 */
    BE_TOKEN_OP_AND_EQUAL,
    BE_TOKEN_OP_LOGIC_AND,
    BE_TOKEN_OP_OR_EQUAL,
    BE_TOKEN_OP_LOGIC_OR,
    BE_TOKEN_OP_XOR_EQUAL,

    /* 关键字 */
    BE_TOKEN_KW_IF,
    BE_TOKEN_KW_ELSE,

    BE_TOKEN_KW_DO,
    BE_TOKEN_KW_WHILE,
    BE_TOKEN_KW_FOR,
    BE_TOKEN_KW_IN,

    BE_TOKEN_KW_BREAK,
    BE_TOKEN_KW_CONTINUE,

    BE_TOKEN_KW_SWITCH,
    BE_TOKEN_KW_CASE,
    BE_TOKEN_KW_DEFAULT,

    BE_TOKEN_KW_NEW,
    BE_TOKEN_KW_VAR,

    BE_TOKEN_KW_FUNCTION,
    BE_TOKEN_KW_RETURN,

    BE_TOKEN_KW_TRUE,
    BE_TOKEN_KW_FALSE,
    BE_TOKEN_KW_NULL,
    BE_TOKEN_KW_UNDEFINED,

    BE_TOKEN_END = 0,

} be_lexer_token_type_e;

typedef enum {
    EXEC_IDLE        = 0,
    EXEC_NORMAL      = 1,
    EXEC_BREAK       = 2,
    EXEC_CONTINUE    = 4,
    EXEC_INTERRUPTED = 8,
    EXEC_ERROR       = 16,
    EXEC_FOR_INIT    = 32,
    EXEC_IN_LOOP     = 64,
    EXEC_IN_SWITCH   = 128,
    EXEC_RUN_MASK    = EXEC_NORMAL | EXEC_BREAK | EXEC_CONTINUE,
    EXEC_ERROR_MASK  = EXEC_INTERRUPTED | EXEC_ERROR,
    EXEC_SAVE_RESTORE_MASK =
        EXEC_NORMAL | EXEC_IN_LOOP |
        EXEC_IN_SWITCH, /* the things JSP_SAVE/RESTORE_EXECUTE should keep
                           track of */
} be_execflag_e;

typedef unsigned short be_jse_node_t;
typedef long be_jse_int_t;
typedef bool be_jse_bool_t;
typedef unsigned long be_jse_uint_t;
#ifdef USE_FLOATS
typedef float be_jse_float_t;
#else
typedef double be_jse_float_t;
#endif
typedef void (*be_jse_callback)(be_jse_node_t id);

typedef union {
    char str[BE_JSE_SYMBOL_DATA_STRING_LEN];
    be_jse_int_t integer;
    be_jse_float_t floating;
    be_jse_callback callback; /* native扩展函数 */
} PACKED_FLAGS be_symbol_data_t;

/*
    JSE编译时指定一个固定大小的be_jse_symbol_t
   类型数组用于存所JS运行中所有的对象 be_jse_node_t 类型数据:
   记录对应var在数组中的位置, 从1开始对应数组下标+1
*/
/*
TBD: locks/refs 都为unsigned char ? 结构体减少一个Byte
*/
typedef struct {
#ifdef LINUXHOST
    be_jse_node_t node_id;
#endif
    unsigned char locks;
    unsigned short refs;        /* 引用计数,用于垃圾回收 */
    be_jse_symbol_type_e flags; /* 变量类型 */

    /* data若是存放字符串, next_sibling/prev_sibling/first_child都在使用,
       长度增加sizeof(be_jse_entry_id_t)*3 */
    be_symbol_data_t data;

    /* 平行节点 */
    be_jse_node_t next_sibling;
    be_jse_node_t prev_sibling;

    /* 保存为Array/Ojbect时, first_child/last_child记录有名对象成员
       在多成员情况下, last_child记录最后一个成员,
       每个成员的prev_sibling指向上一个成员,next_sibling指向下一个成员 */

    /* 保存为BE_SYM_FUNCTION时,与上类似,
       first_child/last_child记录函数参数/CODE(JS函数内容)/SCOPE */

    /* 记录BE_SYM_NAME有名对象的值的节点号 */
    be_jse_node_t first_child;

    /* 记录字符串BE_SYM_STRING_EXT的节点号,类似链表 */
    be_jse_node_t last_child;
} PACKED_FLAGS be_jse_symbol_t;

typedef struct _be_jse_lexer_ctx_t {
    int tk; /* token类型 == be_lexer_token_type_e */
    int token_start;
    int token_end;
    int token_last_end;
    char token[MAX_TOKEN_LENGTH];
    int token_len; /* 当前token长度 */

    char curr_char;
    char next_char;

    /* 从字符串中分析 */
    const char *src;
    int start_pos;
    int end_pos;
    int curr_pos;
} be_jse_lexer_ctx_t;

typedef enum {
    PARSE_NOSKIP_ALL = 0,
    PARSE_NOSKIP_A   = 1,
    PARSE_NOSKIP_B   = 2,
    PARSE_NOSKIP_C   = 4,
    PARSE_NOSKIP_D   = 8,
} be_jse_parse_skip_flag_e;

typedef struct {
    be_jse_node_t root; /* 所有符号表的根起点 */
    be_jse_node_t zeroInt;
    be_jse_node_t oneInt;
    /* 内嵌的string/object/array/int/math/json对象 */
    be_jse_node_t stringClass;
    be_jse_node_t objectClass;
    be_jse_node_t arrayClass;
#ifdef USE_INTEGER
    be_jse_node_t intClass;
#endif

#ifdef USE_MATH
    be_jse_node_t mathClass;
#endif

#ifdef USE_JSON
    be_jse_node_t jsonClass;
#endif

#ifdef USE_ALINK
    /* alink协议扩展 */
    be_jse_node_t alinkClass;
#endif

#ifdef USE_PROCESS
    /* process扩展 */
    be_jse_node_t processClass;
#endif

#ifdef USE_GPIO
    /* gpio扩展 */
    be_jse_node_t gpioClass;
#endif

#ifdef USE_UART
    /* uart扩展 */
    be_jse_node_t uartClass;
#endif

#ifdef USE_MODULES
    be_jse_node_t moduleClass;
#endif

} be_jse_executor_ctx_t;

typedef struct {
    be_jse_executor_ctx_t *executor;
    be_jse_lexer_ctx_t *lex;

    be_jse_node_t scopes[BE_JSE_PARSE_MAX_SCOPES];
    int scopeCount;

    be_execflag_e execute;
} be_jse_vm_ctx_t;

#endif /* BE_JSE_TYPES_H */
