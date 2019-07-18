/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#ifndef BE_JSE_SYMBOL_H
#define BE_JSE_SYMBOL_H

#include "be_jse_types.h"
#include "be_jse_utils.h"

#ifdef __cplusplus
extern "C" {
#endif

#define SYM_TABLE_UNUSED_REF 0xFFFF

void be_jse_symbol_table_init(void);
void be_jse_symbol_table_deinit(void);

/* symbol node ops */
void free_symbol_node(be_jse_symbol_t *node);
void add_symbol_node(be_jse_symbol_t *parent, be_jse_symbol_t *namedChild);
be_jse_symbol_t *get_root_node(void);

/* symbol new ops */
be_jse_symbol_t *new_symbol(be_jse_symbol_type_e type);
be_jse_symbol_t *new_str_symbol(const char *str);
be_jse_symbol_t *new_int_symbol(be_jse_int_t value);
be_jse_symbol_t *new_bool_symbol(bool value);
be_jse_symbol_t *new_float_symbol(be_jse_float_t value);
be_jse_symbol_t *new_func_code_symbol(be_jse_lexer_ctx_t *lex, int charFrom,
                                      int charTo);
be_jse_symbol_t *new_named_symbol(be_jse_symbol_t *var,
                                  be_jse_symbol_t *valueOrZero);
#ifdef USE_JSON
be_jse_symbol_t *new_json_symbol(char *json_str, size_t json_str_len);
#endif
/* symbol lock ops 在ROM空间有冗余情况下可以采用inline方式 */
be_jse_symbol_t *symbol_lock(be_jse_node_t id);
be_jse_symbol_t *symbol_relock(be_jse_symbol_t *s);
be_jse_node_t symbol_unlock(be_jse_symbol_t *s);
be_jse_symbol_t *unlock_symbol_value(be_jse_symbol_t *v);

/* symbol value ops */
void *get_symbol_first_ptr(void);
be_jse_int_t get_symbol_int(be_jse_symbol_t *s);
be_jse_bool_t get_symbol_bool(be_jse_symbol_t *s);
be_jse_symbol_t *get_symbol_value(be_jse_symbol_t *s);
be_jse_bool_t get_symbol_value_bool(be_jse_symbol_t *s);
be_jse_float_t get_symbol_double(be_jse_symbol_t *s);

/* 取符号的值项（key/value）并强制类型转换成int */
be_jse_int_t get_symbol_value_int(be_jse_symbol_t *s);

/*symbol string ops*/
void symbol_to_str(be_jse_symbol_t *v, char *str, size_t len);
int symbol_str_len(be_jse_symbol_t *v);

bool symbol_str_equal(be_jse_symbol_t *var, const char *str);
int symbol_str_cmp(
    be_jse_symbol_t *va, be_jse_symbol_t *vb, int starta, int startb,
    bool equalAtEndOfString); /* /< Compare 2 strings, starting from the given
                                 /< character positions */

void symbol_str_append(be_jse_symbol_t *var, const char *str);
void symbol_str_value_append(be_jse_symbol_t *var, be_jse_symbol_t *str,
                             int stridx, int maxLength);

be_jse_symbol_t *symbol_maths_op(be_jse_symbol_t *a, be_jse_symbol_t *b,
                                 int op);
be_jse_symbol_t *symbol_value_maths_op(be_jse_symbol_t *a, be_jse_symbol_t *b,
                                       int op);
be_jse_symbol_t *symbol_maths_op_error(int op, const char *datatype);

/* Symbol ops */
be_jse_symbol_t *symbol_cp(be_jse_symbol_t *src);
be_jse_symbol_t *symbol_name_cp(be_jse_symbol_t *src, bool linkChildren,
                                bool keepAsName);

be_jse_symbol_t *add_symbol_node_name(be_jse_symbol_t *parent,
                                      be_jse_symbol_t *child, const char *name);
be_jse_symbol_t *set_symbol_node_name(be_jse_symbol_t *name,
                                      be_jse_symbol_t *src);
be_jse_symbol_t *lookup_named_child_symbol(be_jse_node_t parentref,
                                           const char *name,
                                           bool createIfNotFound);
be_jse_symbol_t *lookup_child_symbol(be_jse_node_t parentref,
                                     be_jse_symbol_t *childName,
                                     bool addIfNotFound);
void remove_child_symbol(be_jse_symbol_t *parent, be_jse_symbol_t *child);

/* Array ops */
be_jse_int_t get_symbol_array_length(be_jse_symbol_t *v);
be_jse_symbol_t *get_symbol_array_item(
    be_jse_symbol_t *arr, int index); /* /< Get an item at the specified index
                                         /< in the array (and lock it) */
be_jse_symbol_t *get_symbol_array_index(
    be_jse_symbol_t *arr,
    be_jse_symbol_t *value); /* /< Get the index of the value in the array */
be_jse_int_t symbol_array_push(be_jse_symbol_t *arr, be_jse_symbol_t *value);
be_jse_symbol_t *symbol_array_pop(be_jse_symbol_t *arr);

be_jse_symbol_t *str_to_symbol(be_jse_symbol_t *var, bool unlockVar);
int get_symbol_ref_count(be_jse_symbol_t *toCount, be_jse_symbol_t *var);

#ifdef USE_JSON
void symbol_to_json(be_jse_symbol_t *var, be_jse_symbol_t *result);
#endif

/* ALWAYS_INLINE 减少调用开销
   ALWAYS_INLINE 定义的函数代码小但调用频繁 */

/* 符号表引用计数 */
static ALWAYS_INLINE be_jse_symbol_t *INC_SYMBL_REF(be_jse_symbol_t *s)
{
    be_assert(s);
    s->refs++;
    return s;
}
static ALWAYS_INLINE void DEC_SYMBL_REF(be_jse_symbol_t *s)
{
    be_assert(s && s->refs > 0);
    s->refs--;
    if (s->locks == 0 && s->refs == 0) free_symbol_node(s);
}
static ALWAYS_INLINE be_jse_node_t INC_SYMBL_REF_BY_ID(be_jse_node_t id)
{
    be_jse_symbol_t *s;
    be_assert(id);
    s = symbol_lock(id);

    INC_SYMBL_REF(s);

    symbol_unlock(s);
    return id;
}
static ALWAYS_INLINE be_jse_node_t DEC_SYMBL_REF_BY_ID(be_jse_node_t ref)
{
    be_jse_symbol_t *s;
    be_assert(ref);
    s = symbol_lock(ref);
    DEC_SYMBL_REF(s);
    symbol_unlock(s);
    return 0;
}

be_jse_node_t get_symbol_node_id(be_jse_symbol_t *s);

static ALWAYS_INLINE bool symbol_is_root(const be_jse_symbol_t *v)
{
    return v && (v->flags & BE_SYM_TYPE_MASK) == BE_SYM_ROOT;
}

static ALWAYS_INLINE bool symbol_is_int(const be_jse_symbol_t *s)
{
    return s && (s->flags & BE_SYM_TYPE_MASK) == BE_SYM_INTEGER;
}
static ALWAYS_INLINE bool symbol_is_float(const be_jse_symbol_t *s)
{
    return s && (s->flags & BE_SYM_TYPE_MASK) == BE_SYM_FLOAT;
}
static ALWAYS_INLINE bool symbol_is_string(const be_jse_symbol_t *s)
{
    return s && (s->flags & BE_SYM_TYPE_MASK) == BE_SYM_STRING;
}
static ALWAYS_INLINE bool symbol_is_string_ext(const be_jse_symbol_t *s)
{
    return s && (s->flags & BE_SYM_TYPE_MASK) == BE_SYM_STRING_EXT;
}
static ALWAYS_INLINE bool symbol_is_number(const be_jse_symbol_t *s)
{
    return s && (s->flags & BE_SYM_NUM_MASK) != 0;
}
static ALWAYS_INLINE bool symbol_is_function(const be_jse_symbol_t *s)
{
    return s && (s->flags & BE_SYM_TYPE_MASK) == BE_SYM_FUNCTION;
}
static ALWAYS_INLINE bool symbol_is_function_argv(const be_jse_symbol_t *s)
{
    return s && (s->flags & BE_SYM_FUNCTION_ARGVS) == BE_SYM_FUNCTION_ARGVS;
}
static ALWAYS_INLINE bool symbol_is_object(const be_jse_symbol_t *s)
{
    return s && (s->flags & BE_SYM_TYPE_MASK) == BE_SYM_OBJECT;
}
static ALWAYS_INLINE bool symbol_is_array(const be_jse_symbol_t *s)
{
    return s && (s->flags & BE_SYM_TYPE_MASK) == BE_SYM_ARRAY;
}
static ALWAYS_INLINE bool symbol_is_native(const be_jse_symbol_t *s)
{
    return s && (s->flags & BE_SYM_NATIVE) != 0;
}
static ALWAYS_INLINE bool symbol_is_undefined(const be_jse_symbol_t *s)
{
    return !s;
}
static ALWAYS_INLINE bool symbol_is_null(const be_jse_symbol_t *s)
{
    return s && (s->flags & BE_SYM_TYPE_MASK) == BE_SYM_NULL;
}
static ALWAYS_INLINE bool symbol_is_basic(const be_jse_symbol_t *s)
{
    return symbol_is_number(s) ||
           symbol_is_string(s); /* /< Is this *not* an array/object/etc */
}
static ALWAYS_INLINE bool symbol_is_name(const be_jse_symbol_t *s)
{
    return s && (s->flags & BE_SYM_NAME) != 0;
}
static ALWAYS_INLINE bool symbol_has_characterdata(const be_jse_symbol_t *s)
{
    return symbol_is_string(s) || symbol_is_string_ext(s) ||
           symbol_is_function_argv(s);
}
static ALWAYS_INLINE bool symbol_has_child(const be_jse_symbol_t *s)
{
    return symbol_is_function(s) || symbol_is_object(s) || symbol_is_array(s);
}

/* This is the number of characters a be_jse_symbol_t can contain, NOT string
   length */
static ALWAYS_INLINE int symbol_get_max_char_len(const be_jse_symbol_t *s)
{
    if (symbol_is_string_ext(s)) return BE_JSE_SYMBOL_DATA_STRING_MAX_LEN;
    be_assert(symbol_has_characterdata(s));
    return BE_JSE_SYMBOL_DATA_STRING_LEN;
}

static ALWAYS_INLINE bool get_bool_symbol_and_unlock(be_jse_symbol_t *v)
{
    bool b = get_symbol_bool(v);
    symbol_unlock(v);
    return b;
}

#ifdef DUMP_SYMBL_USAGE

/* 获取当前使用了多少symbol table */
int be_jse_get_memory_usage(void);
/* 获取当前使用symbol table的最大值 */
int be_jse_get_memory_max_usage(void);
/* 查看当前正在使用的BeVars, 用于debug分析内存相关问题 */
void be_jse_show_symbol_table_used(void);
/* 获取当前symbol table的数目 */
int be_jse_get_memory_count(void);

#endif

#ifdef TRACE_JSE_INFO
void trace_symbol_info(be_jse_node_t node, int indent);
void dump_symbol_node_id(int id);
#endif

#ifdef __cplusplus
}
#endif

#endif /* BE_JSE_SYMBOL_H */
