
/*
 * Auto-Generated file, DO NOT EDIT!
 *
 * Command line:
 * /Users/ethan.lcz1/Desktop/git/aos33/components/py_engine/engine/lib/lv_bindings/gen/gen_mpy.py -M lodepng -MD /Users/ethan.lcz1/Desktop/git/aos33/components/py_engine/adapter/haas/build-HaaSEDUk1/lv_png.c.json -E /Users/ethan.lcz1/Desktop/git/aos33/components/py_engine/adapter/haas/build-HaaSEDUk1/lv_png.c.pp /Users/ethan.lcz1/Desktop/git/aos33/components/py_engine/engine/lib/lv_bindings/driver/png/lodepng/lodepng.h
 *
 * Preprocessing command:
 * Preprocessing was disabled.
 *
 * Generating Objects: 
 */

/*
 * Mpy includes
 */

#if MICROPY_PY_LVGL

#include <stdlib.h>
#include <string.h>
#include "py/obj.h"
#include "py/objint.h"
#include "py/objstr.h"
#include "py/runtime.h"
#include "py/binary.h"
#include "py/objarray.h"
#include "py/objtype.h"
#include "py/objexcept.h"

/*
 * lodepng includes
 */

#include "/Users/ethan.lcz1/Desktop/git/aos33/components/py_engine/engine/lib/lv_bindings/driver/png/lodepng/lodepng.h"


/*
 * Helper functions
 */

#ifndef GENMPY_UNUSED
#ifdef __GNUC__
#define GENMPY_UNUSED __attribute__ ((unused))
#else
#define GENMPY_UNUSED
#endif // __GNUC__
#endif // GENMPY_UNUSED

// Custom function mp object

typedef mp_obj_t (*mp_fun_ptr_var_t)(size_t n, const mp_obj_t *, void *ptr);

typedef struct mp_lv_obj_fun_builtin_var_t {
    mp_obj_base_t base;
    mp_uint_t n_args;
    mp_fun_ptr_var_t mp_fun;
    void *lv_fun;
} mp_lv_obj_fun_builtin_var_t;

STATIC mp_obj_t lv_fun_builtin_var_call(mp_obj_t self_in, size_t n_args, size_t n_kw, const mp_obj_t *args);
STATIC mp_int_t mp_func_get_buffer(mp_obj_t self_in, mp_buffer_info_t *bufinfo, mp_uint_t flags);

GENMPY_UNUSED STATIC const mp_obj_type_t mp_lv_type_fun_builtin_var = {
    { &mp_type_type },
    .flags = MP_TYPE_FLAG_BINDS_SELF | MP_TYPE_FLAG_BUILTIN_FUN,
    .name = MP_QSTR_function,
    .call = lv_fun_builtin_var_call,
    .unary_op = mp_generic_unary_op,
    .buffer_p = { .get_buffer = mp_func_get_buffer }
};

GENMPY_UNUSED STATIC const mp_obj_type_t mp_lv_type_fun_builtin_static_var = {
    { &mp_type_type },
    .flags = MP_TYPE_FLAG_BUILTIN_FUN,
    .name = MP_QSTR_function,
    .call = lv_fun_builtin_var_call,
    .unary_op = mp_generic_unary_op,
    .buffer_p = { .get_buffer = mp_func_get_buffer }
};

STATIC mp_obj_t lv_fun_builtin_var_call(mp_obj_t self_in, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    assert(MP_OBJ_IS_TYPE(self_in, &mp_lv_type_fun_builtin_var) ||
           MP_OBJ_IS_TYPE(self_in, &mp_lv_type_fun_builtin_static_var));
    mp_lv_obj_fun_builtin_var_t *self = MP_OBJ_TO_PTR(self_in);
    mp_arg_check_num(n_args, n_kw, self->n_args, self->n_args, false);
    return self->mp_fun(n_args, args, self->lv_fun);
}

STATIC mp_int_t mp_func_get_buffer(mp_obj_t self_in, mp_buffer_info_t *bufinfo, mp_uint_t flags) {
    (void)flags;
    assert(MP_OBJ_IS_TYPE(self_in, &mp_lv_type_fun_builtin_var) ||
           MP_OBJ_IS_TYPE(self_in, &mp_lv_type_fun_builtin_static_var));
    mp_lv_obj_fun_builtin_var_t *self = MP_OBJ_TO_PTR(self_in);

    bufinfo->buf = &self->lv_fun;
    bufinfo->len = sizeof(self->lv_fun);
    bufinfo->typecode = BYTEARRAY_TYPECODE;
    return 0;
}

#define MP_DEFINE_CONST_LV_FUN_OBJ_VAR(obj_name, n_args, mp_fun, lv_fun) \
    const mp_lv_obj_fun_builtin_var_t obj_name = \
        {{&mp_lv_type_fun_builtin_var}, n_args, mp_fun, lv_fun}

#define MP_DEFINE_CONST_LV_FUN_OBJ_STATIC_VAR(obj_name, n_args, mp_fun, lv_fun) \
    const mp_lv_obj_fun_builtin_var_t obj_name = \
        {{&mp_lv_type_fun_builtin_static_var}, n_args, mp_fun, lv_fun}

// Casting

typedef struct mp_lv_struct_t
{
    mp_obj_base_t base;
    void *data;
} mp_lv_struct_t;

STATIC const mp_lv_struct_t mp_lv_null_obj;

#ifdef LV_OBJ_T
STATIC mp_int_t mp_lv_obj_get_buffer(mp_obj_t self_in, mp_buffer_info_t *bufinfo, mp_uint_t flags);
#else
STATIC mp_int_t mp_lv_obj_get_buffer(mp_obj_t self_in, mp_buffer_info_t *bufinfo, mp_uint_t flags){ return 0; }
#endif

STATIC mp_int_t mp_blob_get_buffer(mp_obj_t self_in, mp_buffer_info_t *bufinfo, mp_uint_t flags);

STATIC mp_obj_t get_native_obj(mp_obj_t mp_obj)
{
    if (!MP_OBJ_IS_OBJ(mp_obj)) return mp_obj;
    const mp_obj_type_t *native_type = ((mp_obj_base_t*)mp_obj)->type;
    if (native_type == NULL)
        return NULL;
    if (native_type->parent == NULL ||
        (native_type->buffer_p.get_buffer == mp_blob_get_buffer) ||
        (native_type->buffer_p.get_buffer == mp_lv_obj_get_buffer))
       return mp_obj;
    while (native_type->parent) native_type = native_type->parent;
    return mp_obj_cast_to_native_base(mp_obj, MP_OBJ_FROM_PTR(native_type));
}

STATIC mp_obj_t dict_to_struct(mp_obj_t dict, const mp_obj_type_t *type);

STATIC mp_obj_t make_new_lv_struct(
    const mp_obj_type_t *type,
    size_t n_args,
    size_t n_kw,
    const mp_obj_t *args);

STATIC mp_obj_t cast(mp_obj_t mp_obj, const mp_obj_type_t *mp_type)
{
    mp_obj_t res = NULL;
    if (mp_obj == mp_const_none && mp_type->make_new == &make_new_lv_struct) {
        res = MP_OBJ_FROM_PTR(&mp_lv_null_obj);
    } else if (MP_OBJ_IS_OBJ(mp_obj)) {
        res = get_native_obj(mp_obj);
        if (res){
            const mp_obj_type_t *res_type = ((mp_obj_base_t*)res)->type;
            if (res_type != mp_type){
                if (res_type == &mp_type_dict &&
                    mp_type->make_new == &make_new_lv_struct)
                        res = dict_to_struct(res, mp_type);
                else res = NULL;
            }
        }
    }
    if (res == NULL) nlr_raise(
        mp_obj_new_exception_msg_varg(
            &mp_type_SyntaxError, MP_ERROR_TEXT("Can't convert %s to %s!"), mp_obj_get_type_str(mp_obj), qstr_str(mp_type->name)));
    return res;
}

// object handling
// This section is enabled only when objects are supported 

#ifdef LV_OBJ_T

typedef struct mp_lv_obj_t {
    mp_obj_base_t base;
    LV_OBJ_T *lv_obj;
    LV_OBJ_T *callbacks;
} mp_lv_obj_t;

STATIC inline LV_OBJ_T *mp_to_lv(mp_obj_t mp_obj)
{
    if (mp_obj == NULL || mp_obj == mp_const_none) return NULL;
    mp_obj_t native_obj = get_native_obj(mp_obj);
    if (mp_obj_get_type(native_obj)->buffer_p.get_buffer != mp_lv_obj_get_buffer)
        return NULL;
    mp_lv_obj_t *mp_lv_obj = MP_OBJ_TO_PTR(native_obj);
    if (mp_lv_obj->lv_obj == NULL) {
        nlr_raise(
            mp_obj_new_exception_msg(
                &mp_type_LvReferenceError, MP_ERROR_TEXT("Referenced object was deleted!")));
    }
    return mp_lv_obj->lv_obj;
}

STATIC inline LV_OBJ_T *mp_get_callbacks(mp_obj_t mp_obj)
{
    if (mp_obj == NULL || mp_obj == mp_const_none) return NULL;
    mp_lv_obj_t *mp_lv_obj = MP_OBJ_TO_PTR(get_native_obj(mp_obj));
    if (mp_lv_obj == NULL)
        nlr_raise(
            mp_obj_new_exception_msg(
                &mp_type_SyntaxError, MP_ERROR_TEXT("'user_data' argument must be either a dict or None!")));
    if (!mp_lv_obj->callbacks) mp_lv_obj->callbacks = mp_obj_new_dict(0);
    return mp_lv_obj->callbacks;
}

STATIC inline const mp_obj_type_t *get_BaseObj_type();

STATIC void mp_lv_delete_cb(lv_event_t * e)
{
    LV_OBJ_T *lv_obj = e->target;
    if (lv_obj){
        mp_lv_obj_t *self = lv_obj->user_data;
        if (self) {
            self->lv_obj = NULL;
        }
    }
}

STATIC inline mp_obj_t lv_to_mp(LV_OBJ_T *lv_obj)
{
    if (lv_obj == NULL) return mp_const_none;
    mp_lv_obj_t *self = (mp_lv_obj_t*)lv_obj->user_data;
    if (!self) 
    {
        // Find the object type
        const mp_obj_type_t *mp_obj_type = get_BaseObj_type();
        const lv_obj_class_t *lv_obj_class = lv_obj_get_class(lv_obj);
        const mp_lv_obj_type_t **iter = &mp_lv_obj_types[0];
        for (; *iter; iter++) {
            if ((*iter)->lv_obj_class == lv_obj_class) {
                mp_obj_type = &(*iter)->mp_obj_type;
                break;
            }
        }

        // Create the MP object
        self = m_new_obj(mp_lv_obj_t);
        *self = (mp_lv_obj_t){
            .base = {mp_obj_type},
            .lv_obj = lv_obj,
            .callbacks = NULL,
        };

        // Register the Python object in user_data
        lv_obj->user_data = self;
        
        // Register a "Delete" event callback
        lv_obj_add_event_cb(lv_obj, mp_lv_delete_cb, LV_EVENT_DELETE, NULL);
    }
    return MP_OBJ_FROM_PTR(self);
}

STATIC void* mp_to_ptr(mp_obj_t self_in);

STATIC mp_obj_t cast_obj_type(const mp_obj_type_t* type, mp_obj_t obj)
{
    mp_lv_obj_t *self = m_new_obj(mp_lv_obj_t);
    *self = (mp_lv_obj_t){
        .base = {type},
        .lv_obj = mp_to_ptr(obj),
        .callbacks = NULL,
    };
    if (!self->lv_obj) return mp_const_none;
    return MP_OBJ_FROM_PTR(self);
}

STATIC mp_obj_t cast_obj(mp_obj_t type_obj, mp_obj_t obj)
{
    return cast_obj_type((const mp_obj_type_t *)type_obj, obj);
}

STATIC mp_obj_t make_new(
    const mp_lv_obj_fun_builtin_var_t *lv_obj_var,
    const mp_obj_type_t *type,
    size_t n_args,
    size_t n_kw,
    const mp_obj_t *args)
{
    mp_obj_t lv_obj;
    if (n_args == 0 && n_kw == 0) // allow no args, and pass NULL as parent in such case
    {
        const mp_obj_t no_args[] = {mp_const_none};
        lv_obj = mp_call_function_n_kw(MP_OBJ_FROM_PTR(lv_obj_var), 1, 0, no_args);
    }
    else
    {
        lv_obj = mp_call_function_n_kw(MP_OBJ_FROM_PTR(lv_obj_var), n_args, n_kw, args);
    }

    if (!lv_obj) return mp_const_none;

    mp_lv_obj_t *self = MP_OBJ_TO_PTR(lv_obj);
    if (self->base.type != type)
        return cast_obj_type(type, lv_obj);
    return lv_obj;
}

STATIC MP_DEFINE_CONST_FUN_OBJ_2(cast_obj_obj, cast_obj);
STATIC MP_DEFINE_CONST_CLASSMETHOD_OBJ(cast_obj_class_method, MP_ROM_PTR(&cast_obj_obj));

STATIC mp_int_t mp_lv_obj_get_buffer(mp_obj_t self_in, mp_buffer_info_t *bufinfo, mp_uint_t flags) {
    (void)flags;
    mp_lv_obj_t *self = MP_OBJ_TO_PTR(self_in);

    bufinfo->buf = &self->lv_obj;
    bufinfo->len = sizeof(self->lv_obj);
    bufinfo->typecode = BYTEARRAY_TYPECODE;
    return 0;
}

STATIC mp_obj_t mp_lv_obj_binary_op(mp_binary_op_t op, mp_obj_t lhs_in, mp_obj_t rhs_in)
{
    mp_lv_obj_t *lhs = MP_OBJ_TO_PTR(lhs_in);
    mp_lv_obj_t *rhs = MP_OBJ_TO_PTR(rhs_in);
    switch (op)
    {
        case MP_BINARY_OP_EQUAL:
            return mp_obj_new_bool(lhs->lv_obj == rhs->lv_obj);
        case MP_BINARY_OP_NOT_EQUAL:
            return mp_obj_new_bool(lhs->lv_obj != rhs->lv_obj);
        default:
            return MP_OBJ_NULL;
    }
}

#else // LV_OBJ_T

typedef struct mp_lv_obj_type_t {
    mp_obj_type_t mp_obj_type;
} mp_lv_obj_type_t;

#endif

STATIC inline mp_obj_t convert_to_bool(bool b)
{
    return b? mp_const_true: mp_const_false;
}

STATIC inline mp_obj_t convert_to_str(const char *str)
{
    return str? mp_obj_new_str(str, strlen(str)): mp_const_none;
}

STATIC inline const char *convert_from_str(mp_obj_t str)
{
    if (str == NULL || str == mp_const_none)
        return NULL;

    if (MP_OBJ_IS_TYPE(str, &mp_type_bytearray) ||
        MP_OBJ_IS_TYPE(str, &mp_type_memoryview)) {
            mp_buffer_info_t buffer_info;
            if (mp_get_buffer(str, &buffer_info, MP_BUFFER_READ)) {
                return buffer_info.buf;
            }
    }

    return mp_obj_str_get_str(str);
}

// struct handling

STATIC mp_lv_struct_t *mp_to_lv_struct(mp_obj_t mp_obj)
{
    if (mp_obj == NULL || mp_obj == mp_const_none) return NULL;
    mp_obj_t native_obj = get_native_obj(mp_obj);
    if ( (!MP_OBJ_IS_OBJ(native_obj)) || (mp_obj_get_type(native_obj)->make_new != &make_new_lv_struct) ) nlr_raise(
            mp_obj_new_exception_msg(
                &mp_type_SyntaxError, MP_ERROR_TEXT("Expected Struct object!")));
    mp_lv_struct_t *mp_lv_struct = MP_OBJ_TO_PTR(native_obj);
    return mp_lv_struct;
}

STATIC inline size_t get_lv_struct_size(const mp_obj_type_t *type)
{
    mp_obj_t size_obj = mp_obj_dict_get(type->locals_dict, MP_OBJ_NEW_QSTR(MP_QSTR___SIZE__));
    return (size_t)mp_obj_get_int(size_obj);
}

STATIC mp_obj_t make_new_lv_struct(
    const mp_obj_type_t *type,
    size_t n_args,
    size_t n_kw,
    const mp_obj_t *args)
{
    if ((!MP_OBJ_IS_TYPE(type, &mp_type_type)) || type->make_new != &make_new_lv_struct)
        nlr_raise(
            mp_obj_new_exception_msg(
                &mp_type_SyntaxError, MP_ERROR_TEXT("Argument is not a struct type!")));
    size_t size = get_lv_struct_size(type);
    mp_arg_check_num(n_args, n_kw, 0, 1, false);
    mp_lv_struct_t *self = m_new_obj(mp_lv_struct_t);
    mp_lv_struct_t *other = (n_args > 0) && (!mp_obj_is_int(args[0])) ? mp_to_lv_struct(cast(args[0], type)): NULL;
    size_t count = (n_args > 0) && (mp_obj_is_int(args[0]))? mp_obj_get_int(args[0]): 1;
    *self = (mp_lv_struct_t){
        .base = {type}, 
        .data = (other && other->data == NULL)? NULL: m_malloc(size * count)
    };
    if (self->data) {
        if (other) {
            memcpy(self->data, other->data, size * count);
        } else {
            memset(self->data, 0, size * count);
        }
    }
    return MP_OBJ_FROM_PTR(self);
}

STATIC mp_obj_t lv_struct_binary_op(mp_binary_op_t op, mp_obj_t lhs_in, mp_obj_t rhs_in)
{
    mp_lv_struct_t *lhs = MP_OBJ_TO_PTR(lhs_in);
    mp_lv_struct_t *rhs = MP_OBJ_TO_PTR(rhs_in);
    switch (op)
    {
        case MP_BINARY_OP_EQUAL:
            return mp_obj_new_bool(lhs->data == rhs->data);
        case MP_BINARY_OP_NOT_EQUAL:
            return mp_obj_new_bool(lhs->data != rhs->data);
        default:
            return MP_OBJ_NULL;
    }
}

STATIC mp_obj_t lv_struct_subscr(mp_obj_t self_in, mp_obj_t index, mp_obj_t value)
{
    mp_lv_struct_t *self = mp_to_lv_struct(self_in);

    if ((!self) || (!self->data))
        return NULL;
    if (!mp_obj_is_int(index)) {
        nlr_raise(
            mp_obj_new_exception_msg(
                &mp_type_SyntaxError, MP_ERROR_TEXT("Subscript index must be an integer!")));
    }

    const mp_obj_type_t *type = mp_obj_get_type(self_in);
    size_t element_size = get_lv_struct_size(type);
    size_t element_index = mp_obj_get_int(index);
    void *element_addr = (byte*)self->data + element_size*element_index;

    if (value == MP_OBJ_NULL) {
        memset(element_addr, 0, element_size);
        return self_in;
    }

    mp_lv_struct_t *element_at_index = m_new_obj(mp_lv_struct_t);
    *element_at_index = (mp_lv_struct_t){
        .base = {type},
        .data = element_addr
    };

    if (value != MP_OBJ_SENTINEL){
        mp_lv_struct_t *other = mp_to_lv_struct(cast(value, type));
        if ((!other) || (!other->data))
            return NULL;
        memcpy(element_at_index->data, other->data, element_size);
    }

    return MP_OBJ_FROM_PTR(element_at_index);
}

STATIC void *copy_buffer(const void *buffer, size_t size)
{
    void *new_buffer = m_malloc(size);
    memcpy(new_buffer, buffer, size);
    return new_buffer;
}

// Reference an existing lv struct (or part of it)

STATIC mp_obj_t lv_to_mp_struct(const mp_obj_type_t *type, void *lv_struct)
{
    if (lv_struct == NULL) return mp_const_none;
    mp_lv_struct_t *self = m_new_obj(mp_lv_struct_t);
    *self = (mp_lv_struct_t){
        .base = {type},
        .data = lv_struct
    };
    return MP_OBJ_FROM_PTR(self);
}

STATIC void call_parent_methods(mp_obj_t obj, qstr attr, mp_obj_t *dest)
{
    const mp_obj_type_t *type = mp_obj_get_type(obj);
    while (type->locals_dict != NULL) {
        // generic method lookup
        // this is a lookup in the object (ie not class or type)
        assert(type->locals_dict->base.type == &mp_type_dict); // MicroPython restriction, for now
        mp_map_t *locals_map = &type->locals_dict->map;
        mp_map_elem_t *elem = mp_map_lookup(locals_map, MP_OBJ_NEW_QSTR(attr), MP_MAP_LOOKUP);
        if (elem != NULL) {
            mp_convert_member_lookup(obj, type, elem->value, dest);
            break;
        }
        if (type->parent == NULL) {
            break;
        }
        // search parents
        type = type->parent;
    }
}

// Convert dict to struct

STATIC mp_obj_t dict_to_struct(mp_obj_t dict, const mp_obj_type_t *type)
{
    mp_obj_t mp_struct = make_new_lv_struct(type, 0, 0, NULL);
    mp_obj_t native_dict = cast(dict, &mp_type_dict);
    mp_map_t *map = mp_obj_dict_get_map(native_dict);
    if (map == NULL) return mp_const_none;
    for (uint i = 0; i < map->alloc; i++) {
        mp_obj_t key = map->table[i].key;
        mp_obj_t value = map->table[i].value;
        if (key != MP_OBJ_NULL) {
            mp_obj_t dest[] = {MP_OBJ_SENTINEL, value};
            type->attr(mp_struct, mp_obj_str_get_qstr(key), dest);
            if (dest[0]) nlr_raise(
                mp_obj_new_exception_msg_varg(
                    &mp_type_SyntaxError, MP_ERROR_TEXT("Cannot set field %s on struct %s!"), qstr_str(mp_obj_str_get_qstr(key)), qstr_str(type->name)));
        }
    }
    return mp_struct;
}

// Convert mp object to ptr

STATIC void* mp_to_ptr(mp_obj_t self_in)
{
    mp_buffer_info_t buffer_info;
    if (self_in == NULL || self_in == mp_const_none)
        return NULL;

//    if (MP_OBJ_IS_INT(self_in))
//        return (void*)mp_obj_get_int(self_in);

    // If an object is user instance, take it as is so it could be used as user_data
    if (mp_obj_is_instance_type(mp_obj_get_type(self_in))){
        return MP_OBJ_TO_PTR(self_in);
    }

    if (!mp_get_buffer(self_in, &buffer_info, MP_BUFFER_READ)) {
        // No buffer protocol - this is not a Struct or a Blob, it's some other mp object.
        // We only allow setting dict directly, since it's useful to setting user_data for passing data to C.
        // On other cases throw an exception, to avoid a crash later
        if (MP_OBJ_IS_TYPE(self_in, &mp_type_dict))
            return MP_OBJ_TO_PTR(self_in);
        else nlr_raise(
                mp_obj_new_exception_msg_varg(
                    &mp_type_SyntaxError, MP_ERROR_TEXT("Cannot convert '%s' to pointer!"), mp_obj_get_type_str(self_in)));
    }

    if (MP_OBJ_IS_STR_OR_BYTES(self_in) || 
        MP_OBJ_IS_TYPE(self_in, &mp_type_bytearray) ||
        MP_OBJ_IS_TYPE(self_in, &mp_type_memoryview))
            return buffer_info.buf;
    else
    {
        void *result;
        if (buffer_info.len != sizeof(result) || buffer_info.typecode != BYTEARRAY_TYPECODE){
            nlr_raise(
                mp_obj_new_exception_msg_varg(
                    &mp_type_SyntaxError, MP_ERROR_TEXT("Cannot convert %s to pointer! (buffer does not represent a pointer)"), mp_obj_get_type_str(self_in)));
        }
        memcpy(&result, buffer_info.buf, sizeof(result));
        return result;
    }
}

// Blob is a wrapper for void* 

STATIC void mp_blob_print(const mp_print_t *print,
    mp_obj_t self_in,
    mp_print_kind_t kind)
{
    mp_printf(print, "Blob");
}

STATIC mp_int_t mp_blob_get_buffer(mp_obj_t self_in, mp_buffer_info_t *bufinfo, mp_uint_t flags) {
    (void)flags;
    mp_lv_struct_t *self = MP_OBJ_TO_PTR(self_in);

    bufinfo->buf = &self->data;
    bufinfo->len = sizeof(self->data);
    bufinfo->typecode = BYTEARRAY_TYPECODE;
    return 0;
}

STATIC const mp_obj_fun_builtin_var_t mp_lv_dereference_obj;

// Sometimes (but not always!) Blob represents a Micropython object.
// In such cases it's safe to cast the Blob back to the Micropython object
// cast argument is the underlying object type, and it's optional.

STATIC mp_obj_t mp_blob_cast(size_t argc, const mp_obj_t *argv)
{
    mp_obj_t self = argv[0];
    void *ptr = mp_to_ptr(self);
    if (argc == 1) return MP_OBJ_FROM_PTR(ptr);
    mp_obj_t type = argv[1];
    if (!MP_OBJ_IS_TYPE(type, &mp_type_type))
        nlr_raise(
            mp_obj_new_exception_msg(
                &mp_type_SyntaxError, MP_ERROR_TEXT("Cast argument must be a type!")));
    return cast(MP_OBJ_FROM_PTR(ptr), type);
}

STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(mp_blob_cast_obj, 1, 2, mp_blob_cast);

STATIC const mp_rom_map_elem_t mp_blob_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR___dereference__), MP_ROM_PTR(&mp_lv_dereference_obj) },
    { MP_ROM_QSTR(MP_QSTR___cast__), MP_ROM_PTR(&mp_blob_cast_obj) },
};

STATIC MP_DEFINE_CONST_DICT(mp_blob_locals_dict, mp_blob_locals_dict_table);

STATIC const mp_obj_type_t mp_blob_type = {
    { &mp_type_type },
    .name = MP_QSTR_Blob,
    .print = mp_blob_print,
    //.make_new = make_new_blob,
    .locals_dict = (mp_obj_dict_t*)&mp_blob_locals_dict,
    .buffer_p = { .get_buffer = mp_blob_get_buffer }
};

STATIC const mp_lv_struct_t mp_lv_null_obj = { {&mp_blob_type}, NULL };

STATIC inline mp_obj_t ptr_to_mp(void *data)
{
    return lv_to_mp_struct(&mp_blob_type, data);
}

// Cast pointer to struct

STATIC mp_obj_t mp_lv_cast(mp_obj_t type_obj, mp_obj_t ptr_obj)
{
    mp_lv_struct_t *self = m_new_obj(mp_lv_struct_t);
    *self = (mp_lv_struct_t){
        .base = {(const mp_obj_type_t*)type_obj}, 
        .data = mp_to_ptr(ptr_obj)
    };
    return MP_OBJ_FROM_PTR(self);
}

// Cast instance. Can be used in ISR when memory allocation is prohibited

STATIC inline mp_obj_t mp_lv_cast_instance(mp_obj_t self_in, mp_obj_t ptr_obj)
{
    mp_lv_struct_t *self = MP_OBJ_TO_PTR(self_in);
    self->data = mp_to_ptr(ptr_obj);
    return self_in;
}

STATIC MP_DEFINE_CONST_FUN_OBJ_2(mp_lv_cast_obj, mp_lv_cast);
STATIC MP_DEFINE_CONST_CLASSMETHOD_OBJ(mp_lv_cast_class_method, MP_ROM_PTR(&mp_lv_cast_obj));

STATIC MP_DEFINE_CONST_FUN_OBJ_2(mp_lv_cast_instance_obj, mp_lv_cast_instance);

// Dereference a struct/blob. This allows access to the raw data the struct holds

STATIC mp_obj_t mp_lv_dereference(size_t argc, const mp_obj_t *argv)
{
    mp_obj_t self_in = argv[0];
    mp_obj_t size_in = argc > 1? argv[1]: mp_const_none;
    mp_lv_struct_t *self = MP_OBJ_TO_PTR(self_in);
    size_t size = 0;
    if (size_in == mp_const_none){
        const mp_obj_type_t *type = self->base.type;
        size = get_lv_struct_size(type);
    } else {
        size = (size_t)mp_obj_get_int(size_in);
    }
    mp_obj_array_t *view = MP_OBJ_TO_PTR(mp_obj_new_memoryview(BYTEARRAY_TYPECODE,
        size, self->data));
    view->typecode |= 0x80; // used to indicate writable buffer
    return MP_OBJ_FROM_PTR(view);
}

STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(mp_lv_dereference_obj, 1, 2, mp_lv_dereference);

// Callback function handling
// Callback is either a callable object or a pointer. If it's a callable object, set user_data to the callback.
// Multiple callbacks are kept per object/struct using a dict that associate callback name with callback object
// In case of an lv_obj_t, user_data is mp_lv_obj_t which contains a member "callbacks" for that dict.
// In case of a struct, user_data is a pointer to that dict directly

STATIC mp_obj_t get_callback_dict_from_user_data(void *user_data)
{
    if (user_data){
        mp_obj_t obj = MP_OBJ_FROM_PTR(user_data);
#ifdef LV_OBJ_T
        return 
            MP_OBJ_IS_TYPE(obj, &mp_type_dict)? obj: // Handle the case of dict for a struct
            mp_get_callbacks(obj); // Handle the case of mp_lv_obj_t for an lv_obj_t
#else
        return obj;
#endif
    }
    return NULL;
}

STATIC void *mp_lv_callback(mp_obj_t mp_callback, void *lv_callback, qstr callback_name, void **user_data_ptr)
{
    if (lv_callback && mp_obj_is_callable(mp_callback)){
        if (user_data_ptr){
            // user_data is either a dict of callbacks in case of struct, or a pointer to mp_lv_obj_t in case of lv_obj_t
            if (! (*user_data_ptr) ) *user_data_ptr = MP_OBJ_TO_PTR(mp_obj_new_dict(0)); // if it's NULL - it's a dict for a struct
            mp_obj_t callbacks = get_callback_dict_from_user_data(*user_data_ptr);
            mp_obj_dict_store(callbacks, MP_OBJ_NEW_QSTR(callback_name), mp_callback);
        }
        return lv_callback;
    } else {
        return mp_to_ptr(mp_callback);
    }
}

// Function pointers wrapper

STATIC mp_obj_t mp_lv_funcptr(const mp_lv_obj_fun_builtin_var_t *mp_fun, void *lv_fun, void *lv_callback, qstr func_name, void *user_data)
{
    if (lv_fun == NULL)
        return mp_const_none;
    if (lv_fun == lv_callback) {
        mp_obj_t callbacks = get_callback_dict_from_user_data(user_data);
        if (callbacks)
            return mp_obj_dict_get(callbacks, MP_OBJ_NEW_QSTR(func_name));
    }
    mp_lv_obj_fun_builtin_var_t *funcptr = m_new_obj(mp_lv_obj_fun_builtin_var_t);
    *funcptr = *mp_fun;
    funcptr->lv_fun = lv_fun;
    return MP_OBJ_FROM_PTR(funcptr);
}

// Missing implementation for 64bit integer conversion

STATIC unsigned long long mp_obj_get_ull(mp_obj_t obj)
{
    if (mp_obj_is_small_int(obj))
        return MP_OBJ_SMALL_INT_VALUE(obj);

    unsigned long long val = 0;
    bool big_endian = !(__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__);
    mp_obj_int_to_bytes_impl(obj, big_endian, sizeof(val), (byte*)&val);
    return val;
}


// Array of natives

typedef struct mp_lv_array_t
{
    mp_lv_struct_t base;
    size_t element_size;
    bool is_signed;
} mp_lv_array_t;

STATIC void mp_lv_array_print(const mp_print_t *print,
    mp_obj_t self_in,
    mp_print_kind_t kind)
{
    mp_lv_array_t *self = MP_OBJ_TO_PTR(self_in);
    size_t element_size = self->element_size;
    bool is_signed = self->is_signed;
    mp_printf(print, "C Array (%sint%d[])", is_signed? "": "u", element_size*8);
}

STATIC mp_obj_t lv_array_subscr(mp_obj_t self_in, mp_obj_t index, mp_obj_t value)
{
    mp_lv_array_t *self = MP_OBJ_TO_PTR(self_in);

    if ((!self) || (!self->base.data))
        return NULL;
    if (!mp_obj_is_int(index)) {
        nlr_raise(
            mp_obj_new_exception_msg(
                &mp_type_SyntaxError, MP_ERROR_TEXT("Subscript index must be an integer!")));
    }

    size_t element_size = self->element_size;
    size_t element_index = mp_obj_get_int(index);
    void *element_addr = (byte*)self->base.data + element_size*element_index;
    bool is_signed = self->is_signed;
    union {
        long long val;
        unsigned long long uval;
    } element;
    memset(&element, 0, sizeof(element));

    if (value == MP_OBJ_NULL){
        memset(element_addr, 0, element_size);
    }
    else if (value == MP_OBJ_SENTINEL){
        memcpy(&element, element_addr, element_size);
        return is_signed? mp_obj_new_int_from_ll(element.val): mp_obj_new_int_from_ull(element.uval);
    } else {
        if (!mp_obj_is_int(value)) {
            nlr_raise(
                mp_obj_new_exception_msg_varg(
                    &mp_type_SyntaxError, MP_ERROR_TEXT("Value '%s' must be an integer!"), mp_obj_get_type_str(value)));
        }
        element.uval = mp_obj_get_ull(value);
        memcpy(element_addr, &element, element_size);
    }

    return self_in;
}

STATIC const mp_rom_map_elem_t mp_base_struct_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR___cast__), MP_ROM_PTR(&mp_lv_cast_class_method) },
    { MP_ROM_QSTR(MP_QSTR___cast_instance__), MP_ROM_PTR(&mp_lv_cast_instance_obj) },
    { MP_ROM_QSTR(MP_QSTR___dereference__), MP_ROM_PTR(&mp_lv_dereference_obj) },
};

STATIC MP_DEFINE_CONST_DICT(mp_base_struct_locals_dict, mp_base_struct_locals_dict_table);

STATIC const mp_obj_type_t mp_lv_base_struct_type = {
    { &mp_type_type },
    .name = MP_QSTR_Struct,
    .binary_op = lv_struct_binary_op,
    .subscr = lv_struct_subscr,
    .locals_dict = (mp_obj_dict_t*)&mp_base_struct_locals_dict,
    .buffer_p = { .get_buffer = mp_blob_get_buffer }
};

STATIC const mp_obj_type_t mp_lv_array_type = {
    { &mp_type_type },
    .name = MP_QSTR_C_Array,
    .print = mp_lv_array_print,
    .make_new = NULL, // TODO: provide constructor
    .binary_op = lv_struct_binary_op,
    .subscr = lv_array_subscr,
    .buffer_p = { .get_buffer = mp_blob_get_buffer },
    .locals_dict = (mp_obj_dict_t*)&mp_base_struct_locals_dict,
};

GENMPY_UNUSED STATIC mp_obj_t mp_array_from_ptr(void *lv_arr, size_t element_size, bool is_signed)
{
    mp_lv_array_t *self = m_new_obj(mp_lv_array_t);
    *self = (mp_lv_array_t){
        { {&mp_lv_array_type}, lv_arr },
        element_size,
        is_signed
    };
    return MP_OBJ_FROM_PTR(self);
}

GENMPY_UNUSED STATIC void *mp_array_to_ptr(mp_obj_t *mp_arr, size_t element_size, GENMPY_UNUSED bool is_signed)
{
    mp_obj_t mp_len = mp_obj_len_maybe(mp_arr);
    if (mp_len == MP_OBJ_NULL) return mp_to_ptr(mp_arr);
    mp_int_t len = mp_obj_get_int(mp_len);
    void *lv_arr = m_malloc(len * element_size);
    byte *element_addr = (byte*)lv_arr;
    mp_obj_t iter = mp_getiter(mp_arr, NULL);
    mp_obj_t item;
    while ((item = mp_iternext(iter)) != MP_OBJ_STOP_ITERATION) {
        union {
            long long val;
            unsigned long long uval;
        } element;
        if (!mp_obj_is_int(item)) {
            nlr_raise(
                mp_obj_new_exception_msg_varg(
                    &mp_type_SyntaxError, MP_ERROR_TEXT("Value '%s' must be an integer!"), mp_obj_get_type_str(item)));
        }
        element.uval = mp_obj_get_ull(item);
        memcpy(element_addr, &element, element_size);
        element_addr += element_size;
    }
    return lv_arr;
}

#define MP_ARRAY_CONVERTOR(name, size, is_signed) GENMPY_UNUSED STATIC mp_obj_t mp_array_from_ ## name(void *lv_arr){    return mp_array_from_ptr(lv_arr, size, is_signed);}GENMPY_UNUSED STATIC void *mp_array_to_ ## name(mp_obj_t mp_arr){    return mp_array_to_ptr(mp_arr, size, is_signed);}

MP_ARRAY_CONVERTOR(u8ptr, 1, false)
MP_ARRAY_CONVERTOR(i8ptr, 1, true)
MP_ARRAY_CONVERTOR(u16ptr, 2, false)
MP_ARRAY_CONVERTOR(i16ptr, 2, true)
MP_ARRAY_CONVERTOR(u32ptr, 4, false)
MP_ARRAY_CONVERTOR(i32ptr, 4, true)
MP_ARRAY_CONVERTOR(u64ptr, 8, false)
MP_ARRAY_CONVERTOR(i64ptr, 8, true)



/*
 * LVGL string constants
 */


/*
 * Helper Structs
 */
        

typedef union {
    void*           ptr_val;
    const char*     str_val;
    int             int_val;
    unsigned int    uint_val;
} C_Pointer;


/*
 * Struct C_Pointer
 */

STATIC inline const mp_obj_type_t *get_mp_C_Pointer_type();

STATIC inline C_Pointer* mp_write_ptr_C_Pointer(mp_obj_t self_in)
{
    mp_lv_struct_t *self = MP_OBJ_TO_PTR(cast(self_in, get_mp_C_Pointer_type()));
    return (C_Pointer*)self->data;
}

#define mp_write_C_Pointer(struct_obj) *mp_write_ptr_C_Pointer(struct_obj)

STATIC inline mp_obj_t mp_read_ptr_C_Pointer(void *field)
{
    return lv_to_mp_struct(get_mp_C_Pointer_type(), field);
}

#define mp_read_C_Pointer(field) mp_read_ptr_C_Pointer(copy_buffer(&field, sizeof(C_Pointer)))
#define mp_read_byref_C_Pointer(field) mp_read_ptr_C_Pointer(&field)

STATIC void mp_C_Pointer_attr(mp_obj_t self_in, qstr attr, mp_obj_t *dest)
{
    mp_lv_struct_t *self = MP_OBJ_TO_PTR(self_in);
    C_Pointer *data = (C_Pointer*)self->data;

    if (dest[0] == MP_OBJ_NULL) {
        // load attribute
        switch(attr)
        {
            case MP_QSTR_ptr_val: dest[0] = ptr_to_mp((void*)data->ptr_val); break; // converting from void *;
            case MP_QSTR_str_val: dest[0] = convert_to_str((void*)data->str_val); break; // converting from char *;
            case MP_QSTR_int_val: dest[0] = mp_obj_new_int(data->int_val); break; // converting from int;
            case MP_QSTR_uint_val: dest[0] = mp_obj_new_int_from_uint(data->uint_val); break; // converting from unsigned int;
            default: call_parent_methods(self_in, attr, dest); // fallback to locals_dict lookup
        }
    } else {
        if (dest[1])
        {
            // store attribute
            switch(attr)
            {
                case MP_QSTR_ptr_val: data->ptr_val = (void*)mp_to_ptr(dest[1]); break; // converting to void *;
                case MP_QSTR_str_val: data->str_val = (void*)(char*)convert_from_str(dest[1]); break; // converting to char *;
                case MP_QSTR_int_val: data->int_val = (int)mp_obj_get_int(dest[1]); break; // converting to int;
                case MP_QSTR_uint_val: data->uint_val = (unsigned int)mp_obj_get_int(dest[1]); break; // converting to unsigned int;
                default: return;
            }

            dest[0] = MP_OBJ_NULL; // indicate success
        }
    }
}

STATIC void mp_C_Pointer_print(const mp_print_t *print,
    mp_obj_t self_in,
    mp_print_kind_t kind)
{
    mp_printf(print, "struct C_Pointer");
}

STATIC const mp_obj_dict_t mp_C_Pointer_locals_dict;

STATIC const mp_obj_type_t mp_C_Pointer_type = {
    { &mp_type_type },
    .name = MP_QSTR_C_Pointer,
    .print = mp_C_Pointer_print,
    .make_new = make_new_lv_struct,
    .binary_op = lv_struct_binary_op,
    .subscr = lv_struct_subscr,
    .attr = mp_C_Pointer_attr,
    .locals_dict = (mp_obj_dict_t*)&mp_C_Pointer_locals_dict,
    .buffer_p = { .get_buffer = mp_blob_get_buffer },
    .parent = &mp_lv_base_struct_type
};

STATIC inline const mp_obj_type_t *get_mp_C_Pointer_type()
{
    return &mp_C_Pointer_type;
}
    

/*
 * lodepng LCT object definitions
 */
    

STATIC const mp_rom_map_elem_t LCT_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_GREY), MP_ROM_PTR(MP_ROM_INT(LCT_GREY)) },
    { MP_ROM_QSTR(MP_QSTR_RGB), MP_ROM_PTR(MP_ROM_INT(LCT_RGB)) },
    { MP_ROM_QSTR(MP_QSTR_PALETTE), MP_ROM_PTR(MP_ROM_INT(LCT_PALETTE)) },
    { MP_ROM_QSTR(MP_QSTR_GREY_ALPHA), MP_ROM_PTR(MP_ROM_INT(LCT_GREY_ALPHA)) },
    { MP_ROM_QSTR(MP_QSTR_RGBA), MP_ROM_PTR(MP_ROM_INT(LCT_RGBA)) },
    { MP_ROM_QSTR(MP_QSTR_MAX_OCTET_VALUE), MP_ROM_PTR(MP_ROM_INT(LCT_MAX_OCTET_VALUE)) }
};

STATIC MP_DEFINE_CONST_DICT(LCT_locals_dict, LCT_locals_dict_table);

STATIC void LCT_print(const mp_print_t *print,
    mp_obj_t self_in,
    mp_print_kind_t kind)
{
    mp_printf(print, "lodepng LCT");
}



STATIC const mp_lv_obj_type_t mp_lv_LCT_type = {
    {
        { &mp_type_type },
        .name = MP_QSTR_LCT,
        .print = LCT_print,
        
        
        .attr = call_parent_methods,
        .locals_dict = (mp_obj_dict_t*)&LCT_locals_dict,
        
        .parent = NULL
    },
#ifdef LV_OBJ_T
    NULL
#endif
};
    
#define funcptr_custom_zlib NULL


/*
 * lodepng extension definition for:
 * unsigned custom_zlib(unsigned char **, size_t *, const unsigned char *, size_t, const LodePNGDecompressSettings *)
 */
 
STATIC mp_obj_t mp_funcptr_custom_zlib(size_t mp_n_args, const mp_obj_t *mp_args, void *lv_func_ptr)
{
    unsigned char **arg0 = mp_to_ptr(mp_args[0]);
    size_t *arg1 = mp_array_to_u32ptr(mp_args[1]);
    const unsigned char *arg2 = (const unsigned char *)mp_array_to_u8ptr(mp_args[2]);
    size_t arg3 = (size_t)mp_obj_get_int(mp_args[3]);
    const LodePNGDecompressSettings *arg4 = (const LodePNGDecompressSettings *)mp_to_ptr(mp_args[4]);
    unsigned _res = ((unsigned (*)(unsigned char **, size_t *, const unsigned char *, size_t, const LodePNGDecompressSettings *))lv_func_ptr)(arg0, arg1, arg2, arg3, arg4);
    return mp_obj_new_int_from_uint(_res);
}

 

STATIC MP_DEFINE_CONST_LV_FUN_OBJ_STATIC_VAR(mp_funcptr_custom_zlib_obj, 5, mp_funcptr_custom_zlib, funcptr_custom_zlib);
    
STATIC inline mp_obj_t mp_lv_funcptr_custom_zlib(void *func){ return mp_lv_funcptr(&mp_funcptr_custom_zlib_obj, func, NULL, MP_QSTR_, NULL); }


/*
 * Function NOT generated:
 * Missing 'user_data' as a field of the first parameter of the callback function 'LodePNGDecompressSettings_custom_zlib_callback'
 * unsigned (*custom_zlib)(unsigned char **, size_t *, const unsigned char *, size_t, const LodePNGDecompressSettings *)
 */
    

/*
 * Function NOT generated:
 * Missing 'user_data' as a field of the first parameter of the callback function 'LodePNGDecompressSettings_custom_inflate_callback'
 * unsigned (*custom_inflate)(unsigned char **, size_t *, const unsigned char *, size_t, const LodePNGDecompressSettings *)
 */
    

/*
 * Struct LodePNGDecompressSettings
 */

STATIC inline const mp_obj_type_t *get_mp_LodePNGDecompressSettings_type();

STATIC inline struct LodePNGDecompressSettings* mp_write_ptr_LodePNGDecompressSettings(mp_obj_t self_in)
{
    mp_lv_struct_t *self = MP_OBJ_TO_PTR(cast(self_in, get_mp_LodePNGDecompressSettings_type()));
    return (struct LodePNGDecompressSettings*)self->data;
}

#define mp_write_LodePNGDecompressSettings(struct_obj) *mp_write_ptr_LodePNGDecompressSettings(struct_obj)

STATIC inline mp_obj_t mp_read_ptr_LodePNGDecompressSettings(void *field)
{
    return lv_to_mp_struct(get_mp_LodePNGDecompressSettings_type(), field);
}

#define mp_read_LodePNGDecompressSettings(field) mp_read_ptr_LodePNGDecompressSettings(copy_buffer(&field, sizeof(struct LodePNGDecompressSettings)))
#define mp_read_byref_LodePNGDecompressSettings(field) mp_read_ptr_LodePNGDecompressSettings(&field)

STATIC void mp_LodePNGDecompressSettings_attr(mp_obj_t self_in, qstr attr, mp_obj_t *dest)
{
    mp_lv_struct_t *self = MP_OBJ_TO_PTR(self_in);
    struct LodePNGDecompressSettings *data = (struct LodePNGDecompressSettings*)self->data;

    if (dest[0] == MP_OBJ_NULL) {
        // load attribute
        switch(attr)
        {
            case MP_QSTR_ignore_adler32: dest[0] = mp_obj_new_int_from_uint(data->ignore_adler32); break; // converting from unsigned;
            case MP_QSTR_ignore_nlen: dest[0] = mp_obj_new_int_from_uint(data->ignore_nlen); break; // converting from unsigned;
            case MP_QSTR_max_output_size: dest[0] = mp_obj_new_int_from_uint(data->max_output_size); break; // converting from size_t;
            case MP_QSTR_custom_zlib: dest[0] = mp_lv_funcptr(&mp_funcptr_custom_zlib_obj, (void*)data->custom_zlib, NULL ,MP_QSTR_LodePNGDecompressSettings_custom_zlib, NULL); break; // converting from callback unsigned (*)(unsigned char **, size_t *, unsigned char *, size_t, LodePNGDecompressSettings *);
            case MP_QSTR_custom_inflate: dest[0] = mp_lv_funcptr(&mp_funcptr_custom_zlib_obj, (void*)data->custom_inflate, NULL ,MP_QSTR_LodePNGDecompressSettings_custom_inflate, NULL); break; // converting from callback unsigned (*)(unsigned char **, size_t *, unsigned char *, size_t, LodePNGDecompressSettings *);
            case MP_QSTR_custom_context: dest[0] = ptr_to_mp((void*)data->custom_context); break; // converting from void *;
            default: call_parent_methods(self_in, attr, dest); // fallback to locals_dict lookup
        }
    } else {
        if (dest[1])
        {
            // store attribute
            switch(attr)
            {
                case MP_QSTR_ignore_adler32: data->ignore_adler32 = (unsigned)mp_obj_get_int(dest[1]); break; // converting to unsigned;
                case MP_QSTR_ignore_nlen: data->ignore_nlen = (unsigned)mp_obj_get_int(dest[1]); break; // converting to unsigned;
                case MP_QSTR_max_output_size: data->max_output_size = (size_t)mp_obj_get_int(dest[1]); break; // converting to size_t;
                case MP_QSTR_custom_zlib: data->custom_zlib = (void*)mp_lv_callback(dest[1], NULL ,MP_QSTR_LodePNGDecompressSettings_custom_zlib, NULL); break; // converting to callback unsigned (*)(unsigned char **, size_t *, unsigned char *, size_t, LodePNGDecompressSettings *);
                case MP_QSTR_custom_inflate: data->custom_inflate = (void*)mp_lv_callback(dest[1], NULL ,MP_QSTR_LodePNGDecompressSettings_custom_inflate, NULL); break; // converting to callback unsigned (*)(unsigned char **, size_t *, unsigned char *, size_t, LodePNGDecompressSettings *);
                case MP_QSTR_custom_context: data->custom_context = (void*)mp_to_ptr(dest[1]); break; // converting to void *;
                default: return;
            }

            dest[0] = MP_OBJ_NULL; // indicate success
        }
    }
}

STATIC void mp_LodePNGDecompressSettings_print(const mp_print_t *print,
    mp_obj_t self_in,
    mp_print_kind_t kind)
{
    mp_printf(print, "struct LodePNGDecompressSettings");
}

STATIC const mp_obj_dict_t mp_LodePNGDecompressSettings_locals_dict;

STATIC const mp_obj_type_t mp_LodePNGDecompressSettings_type = {
    { &mp_type_type },
    .name = MP_QSTR_LodePNGDecompressSettings,
    .print = mp_LodePNGDecompressSettings_print,
    .make_new = make_new_lv_struct,
    .binary_op = lv_struct_binary_op,
    .subscr = lv_struct_subscr,
    .attr = mp_LodePNGDecompressSettings_attr,
    .locals_dict = (mp_obj_dict_t*)&mp_LodePNGDecompressSettings_locals_dict,
    .buffer_p = { .get_buffer = mp_blob_get_buffer },
    .parent = &mp_lv_base_struct_type
};

STATIC inline const mp_obj_type_t *get_mp_LodePNGDecompressSettings_type()
{
    return &mp_LodePNGDecompressSettings_type;
}
    

/*
 * lodepng lodepng_default_decompress_settings global definitions
 */

STATIC const mp_lv_struct_t mp_lodepng_default_decompress_settings = {
    { &mp_LodePNGDecompressSettings_type },
    (LodePNGDecompressSettings*)&lodepng_default_decompress_settings
};
    

/*
 * Struct LodePNGColorMode
 */

STATIC inline const mp_obj_type_t *get_mp_LodePNGColorMode_type();

STATIC inline LodePNGColorMode* mp_write_ptr_LodePNGColorMode(mp_obj_t self_in)
{
    mp_lv_struct_t *self = MP_OBJ_TO_PTR(cast(self_in, get_mp_LodePNGColorMode_type()));
    return (LodePNGColorMode*)self->data;
}

#define mp_write_LodePNGColorMode(struct_obj) *mp_write_ptr_LodePNGColorMode(struct_obj)

STATIC inline mp_obj_t mp_read_ptr_LodePNGColorMode(void *field)
{
    return lv_to_mp_struct(get_mp_LodePNGColorMode_type(), field);
}

#define mp_read_LodePNGColorMode(field) mp_read_ptr_LodePNGColorMode(copy_buffer(&field, sizeof(LodePNGColorMode)))
#define mp_read_byref_LodePNGColorMode(field) mp_read_ptr_LodePNGColorMode(&field)

STATIC void mp_LodePNGColorMode_attr(mp_obj_t self_in, qstr attr, mp_obj_t *dest)
{
    mp_lv_struct_t *self = MP_OBJ_TO_PTR(self_in);
    LodePNGColorMode *data = (LodePNGColorMode*)self->data;

    if (dest[0] == MP_OBJ_NULL) {
        // load attribute
        switch(attr)
        {
            case MP_QSTR_colortype: dest[0] = mp_obj_new_int(data->colortype); break; // converting from LodePNGColorType;
            case MP_QSTR_bitdepth: dest[0] = mp_obj_new_int_from_uint(data->bitdepth); break; // converting from unsigned;
            case MP_QSTR_palette: dest[0] = mp_array_from_u8ptr((void*)data->palette); break; // converting from unsigned char *;
            case MP_QSTR_palettesize: dest[0] = mp_obj_new_int_from_uint(data->palettesize); break; // converting from size_t;
            case MP_QSTR_key_defined: dest[0] = mp_obj_new_int_from_uint(data->key_defined); break; // converting from unsigned;
            case MP_QSTR_key_r: dest[0] = mp_obj_new_int_from_uint(data->key_r); break; // converting from unsigned;
            case MP_QSTR_key_g: dest[0] = mp_obj_new_int_from_uint(data->key_g); break; // converting from unsigned;
            case MP_QSTR_key_b: dest[0] = mp_obj_new_int_from_uint(data->key_b); break; // converting from unsigned;
            default: call_parent_methods(self_in, attr, dest); // fallback to locals_dict lookup
        }
    } else {
        if (dest[1])
        {
            // store attribute
            switch(attr)
            {
                case MP_QSTR_colortype: data->colortype = (int)mp_obj_get_int(dest[1]); break; // converting to LodePNGColorType;
                case MP_QSTR_bitdepth: data->bitdepth = (unsigned)mp_obj_get_int(dest[1]); break; // converting to unsigned;
                case MP_QSTR_palette: data->palette = (void*)mp_array_to_u8ptr(dest[1]); break; // converting to unsigned char *;
                case MP_QSTR_palettesize: data->palettesize = (size_t)mp_obj_get_int(dest[1]); break; // converting to size_t;
                case MP_QSTR_key_defined: data->key_defined = (unsigned)mp_obj_get_int(dest[1]); break; // converting to unsigned;
                case MP_QSTR_key_r: data->key_r = (unsigned)mp_obj_get_int(dest[1]); break; // converting to unsigned;
                case MP_QSTR_key_g: data->key_g = (unsigned)mp_obj_get_int(dest[1]); break; // converting to unsigned;
                case MP_QSTR_key_b: data->key_b = (unsigned)mp_obj_get_int(dest[1]); break; // converting to unsigned;
                default: return;
            }

            dest[0] = MP_OBJ_NULL; // indicate success
        }
    }
}

STATIC void mp_LodePNGColorMode_print(const mp_print_t *print,
    mp_obj_t self_in,
    mp_print_kind_t kind)
{
    mp_printf(print, "struct LodePNGColorMode");
}

STATIC const mp_obj_dict_t mp_LodePNGColorMode_locals_dict;

STATIC const mp_obj_type_t mp_LodePNGColorMode_type = {
    { &mp_type_type },
    .name = MP_QSTR_LodePNGColorMode,
    .print = mp_LodePNGColorMode_print,
    .make_new = make_new_lv_struct,
    .binary_op = lv_struct_binary_op,
    .subscr = lv_struct_subscr,
    .attr = mp_LodePNGColorMode_attr,
    .locals_dict = (mp_obj_dict_t*)&mp_LodePNGColorMode_locals_dict,
    .buffer_p = { .get_buffer = mp_blob_get_buffer },
    .parent = &mp_lv_base_struct_type
};

STATIC inline const mp_obj_type_t *get_mp_LodePNGColorMode_type()
{
    return &mp_LodePNGColorMode_type;
}
    

/*
 * Struct LodePNGTime
 */

STATIC inline const mp_obj_type_t *get_mp_LodePNGTime_type();

STATIC inline LodePNGTime* mp_write_ptr_LodePNGTime(mp_obj_t self_in)
{
    mp_lv_struct_t *self = MP_OBJ_TO_PTR(cast(self_in, get_mp_LodePNGTime_type()));
    return (LodePNGTime*)self->data;
}

#define mp_write_LodePNGTime(struct_obj) *mp_write_ptr_LodePNGTime(struct_obj)

STATIC inline mp_obj_t mp_read_ptr_LodePNGTime(void *field)
{
    return lv_to_mp_struct(get_mp_LodePNGTime_type(), field);
}

#define mp_read_LodePNGTime(field) mp_read_ptr_LodePNGTime(copy_buffer(&field, sizeof(LodePNGTime)))
#define mp_read_byref_LodePNGTime(field) mp_read_ptr_LodePNGTime(&field)

STATIC void mp_LodePNGTime_attr(mp_obj_t self_in, qstr attr, mp_obj_t *dest)
{
    mp_lv_struct_t *self = MP_OBJ_TO_PTR(self_in);
    LodePNGTime *data = (LodePNGTime*)self->data;

    if (dest[0] == MP_OBJ_NULL) {
        // load attribute
        switch(attr)
        {
            case MP_QSTR_year: dest[0] = mp_obj_new_int_from_uint(data->year); break; // converting from unsigned;
            case MP_QSTR_month: dest[0] = mp_obj_new_int_from_uint(data->month); break; // converting from unsigned;
            case MP_QSTR_day: dest[0] = mp_obj_new_int_from_uint(data->day); break; // converting from unsigned;
            case MP_QSTR_hour: dest[0] = mp_obj_new_int_from_uint(data->hour); break; // converting from unsigned;
            case MP_QSTR_minute: dest[0] = mp_obj_new_int_from_uint(data->minute); break; // converting from unsigned;
            case MP_QSTR_second: dest[0] = mp_obj_new_int_from_uint(data->second); break; // converting from unsigned;
            default: call_parent_methods(self_in, attr, dest); // fallback to locals_dict lookup
        }
    } else {
        if (dest[1])
        {
            // store attribute
            switch(attr)
            {
                case MP_QSTR_year: data->year = (unsigned)mp_obj_get_int(dest[1]); break; // converting to unsigned;
                case MP_QSTR_month: data->month = (unsigned)mp_obj_get_int(dest[1]); break; // converting to unsigned;
                case MP_QSTR_day: data->day = (unsigned)mp_obj_get_int(dest[1]); break; // converting to unsigned;
                case MP_QSTR_hour: data->hour = (unsigned)mp_obj_get_int(dest[1]); break; // converting to unsigned;
                case MP_QSTR_minute: data->minute = (unsigned)mp_obj_get_int(dest[1]); break; // converting to unsigned;
                case MP_QSTR_second: data->second = (unsigned)mp_obj_get_int(dest[1]); break; // converting to unsigned;
                default: return;
            }

            dest[0] = MP_OBJ_NULL; // indicate success
        }
    }
}

STATIC void mp_LodePNGTime_print(const mp_print_t *print,
    mp_obj_t self_in,
    mp_print_kind_t kind)
{
    mp_printf(print, "struct LodePNGTime");
}

STATIC const mp_obj_dict_t mp_LodePNGTime_locals_dict;

STATIC const mp_obj_type_t mp_LodePNGTime_type = {
    { &mp_type_type },
    .name = MP_QSTR_LodePNGTime,
    .print = mp_LodePNGTime_print,
    .make_new = make_new_lv_struct,
    .binary_op = lv_struct_binary_op,
    .subscr = lv_struct_subscr,
    .attr = mp_LodePNGTime_attr,
    .locals_dict = (mp_obj_dict_t*)&mp_LodePNGTime_locals_dict,
    .buffer_p = { .get_buffer = mp_blob_get_buffer },
    .parent = &mp_lv_base_struct_type
};

STATIC inline const mp_obj_type_t *get_mp_LodePNGTime_type()
{
    return &mp_LodePNGTime_type;
}
    

/*
 * Array convertors for unsigned char *[3]
 */

GENMPY_UNUSED STATIC unsigned char * *mp_arr_to_unsigned_char_ptr__3__(mp_obj_t mp_arr)
{
    mp_obj_t mp_len = mp_obj_len_maybe(mp_arr);
    if (mp_len == MP_OBJ_NULL) return mp_to_ptr(mp_arr);
    mp_int_t len = mp_obj_get_int(mp_len);
    //TODO check dim!
    unsigned char * *lv_arr = (unsigned char **)m_malloc(len * sizeof(unsigned char *));
    mp_obj_t iter = mp_getiter(mp_arr, NULL);
    mp_obj_t item;
    size_t i = 0;
    while ((item = mp_iternext(iter)) != MP_OBJ_STOP_ITERATION) {
        lv_arr[i++] = mp_array_to_u8ptr(item);
    }
    return (unsigned char * *)lv_arr;
}

GENMPY_UNUSED STATIC mp_obj_t mp_arr_from_unsigned_char_ptr__3__(unsigned char * *arr)
{
    mp_obj_t obj_arr[3];
    for (size_t i=0; i<3; i++){
        obj_arr[i] = mp_array_from_u8ptr(arr[i]);
    }
    return mp_obj_new_list(3, obj_arr); // TODO: return custom iterable object!
}


/*
 * Array convertors for size_t [3]
 */

GENMPY_UNUSED STATIC size_t *mp_arr_to_size_t___3__(mp_obj_t mp_arr)
{
    mp_obj_t mp_len = mp_obj_len_maybe(mp_arr);
    if (mp_len == MP_OBJ_NULL) return mp_to_ptr(mp_arr);
    mp_int_t len = mp_obj_get_int(mp_len);
    //TODO check dim!
    size_t *lv_arr = (size_t*)m_malloc(len * sizeof(size_t));
    mp_obj_t iter = mp_getiter(mp_arr, NULL);
    mp_obj_t item;
    size_t i = 0;
    while ((item = mp_iternext(iter)) != MP_OBJ_STOP_ITERATION) {
        lv_arr[i++] = (size_t)mp_obj_get_int(item);
    }
    return (size_t *)lv_arr;
}

GENMPY_UNUSED STATIC mp_obj_t mp_arr_from_size_t___3__(size_t *arr)
{
    mp_obj_t obj_arr[3];
    for (size_t i=0; i<3; i++){
        obj_arr[i] = mp_obj_new_int_from_uint(arr[i]);
    }
    return mp_obj_new_list(3, obj_arr); // TODO: return custom iterable object!
}


/*
 * Struct LodePNGInfo
 */

STATIC inline const mp_obj_type_t *get_mp_LodePNGInfo_type();

STATIC inline LodePNGInfo* mp_write_ptr_LodePNGInfo(mp_obj_t self_in)
{
    mp_lv_struct_t *self = MP_OBJ_TO_PTR(cast(self_in, get_mp_LodePNGInfo_type()));
    return (LodePNGInfo*)self->data;
}

#define mp_write_LodePNGInfo(struct_obj) *mp_write_ptr_LodePNGInfo(struct_obj)

STATIC inline mp_obj_t mp_read_ptr_LodePNGInfo(void *field)
{
    return lv_to_mp_struct(get_mp_LodePNGInfo_type(), field);
}

#define mp_read_LodePNGInfo(field) mp_read_ptr_LodePNGInfo(copy_buffer(&field, sizeof(LodePNGInfo)))
#define mp_read_byref_LodePNGInfo(field) mp_read_ptr_LodePNGInfo(&field)

STATIC void mp_LodePNGInfo_attr(mp_obj_t self_in, qstr attr, mp_obj_t *dest)
{
    mp_lv_struct_t *self = MP_OBJ_TO_PTR(self_in);
    LodePNGInfo *data = (LodePNGInfo*)self->data;

    if (dest[0] == MP_OBJ_NULL) {
        // load attribute
        switch(attr)
        {
            case MP_QSTR_compression_method: dest[0] = mp_obj_new_int_from_uint(data->compression_method); break; // converting from unsigned;
            case MP_QSTR_filter_method: dest[0] = mp_obj_new_int_from_uint(data->filter_method); break; // converting from unsigned;
            case MP_QSTR_interlace_method: dest[0] = mp_obj_new_int_from_uint(data->interlace_method); break; // converting from unsigned;
            case MP_QSTR_color: dest[0] = mp_read_byref_LodePNGColorMode(data->color); break; // converting from LodePNGColorMode;
            case MP_QSTR_background_defined: dest[0] = mp_obj_new_int_from_uint(data->background_defined); break; // converting from unsigned;
            case MP_QSTR_background_r: dest[0] = mp_obj_new_int_from_uint(data->background_r); break; // converting from unsigned;
            case MP_QSTR_background_g: dest[0] = mp_obj_new_int_from_uint(data->background_g); break; // converting from unsigned;
            case MP_QSTR_background_b: dest[0] = mp_obj_new_int_from_uint(data->background_b); break; // converting from unsigned;
            case MP_QSTR_text_num: dest[0] = mp_obj_new_int_from_uint(data->text_num); break; // converting from size_t;
            case MP_QSTR_text_keys: dest[0] = mp_read_ptr_C_Pointer((void*)data->text_keys); break; // converting from char **;
            case MP_QSTR_text_strings: dest[0] = mp_read_ptr_C_Pointer((void*)data->text_strings); break; // converting from char **;
            case MP_QSTR_itext_num: dest[0] = mp_obj_new_int_from_uint(data->itext_num); break; // converting from size_t;
            case MP_QSTR_itext_keys: dest[0] = mp_read_ptr_C_Pointer((void*)data->itext_keys); break; // converting from char **;
            case MP_QSTR_itext_langtags: dest[0] = mp_read_ptr_C_Pointer((void*)data->itext_langtags); break; // converting from char **;
            case MP_QSTR_itext_transkeys: dest[0] = mp_read_ptr_C_Pointer((void*)data->itext_transkeys); break; // converting from char **;
            case MP_QSTR_itext_strings: dest[0] = mp_read_ptr_C_Pointer((void*)data->itext_strings); break; // converting from char **;
            case MP_QSTR_time_defined: dest[0] = mp_obj_new_int_from_uint(data->time_defined); break; // converting from unsigned;
            case MP_QSTR_time: dest[0] = mp_read_byref_LodePNGTime(data->time); break; // converting from LodePNGTime;
            case MP_QSTR_phys_defined: dest[0] = mp_obj_new_int_from_uint(data->phys_defined); break; // converting from unsigned;
            case MP_QSTR_phys_x: dest[0] = mp_obj_new_int_from_uint(data->phys_x); break; // converting from unsigned;
            case MP_QSTR_phys_y: dest[0] = mp_obj_new_int_from_uint(data->phys_y); break; // converting from unsigned;
            case MP_QSTR_phys_unit: dest[0] = mp_obj_new_int_from_uint(data->phys_unit); break; // converting from unsigned;
            case MP_QSTR_gama_defined: dest[0] = mp_obj_new_int_from_uint(data->gama_defined); break; // converting from unsigned;
            case MP_QSTR_gama_gamma: dest[0] = mp_obj_new_int_from_uint(data->gama_gamma); break; // converting from unsigned;
            case MP_QSTR_chrm_defined: dest[0] = mp_obj_new_int_from_uint(data->chrm_defined); break; // converting from unsigned;
            case MP_QSTR_chrm_white_x: dest[0] = mp_obj_new_int_from_uint(data->chrm_white_x); break; // converting from unsigned;
            case MP_QSTR_chrm_white_y: dest[0] = mp_obj_new_int_from_uint(data->chrm_white_y); break; // converting from unsigned;
            case MP_QSTR_chrm_red_x: dest[0] = mp_obj_new_int_from_uint(data->chrm_red_x); break; // converting from unsigned;
            case MP_QSTR_chrm_red_y: dest[0] = mp_obj_new_int_from_uint(data->chrm_red_y); break; // converting from unsigned;
            case MP_QSTR_chrm_green_x: dest[0] = mp_obj_new_int_from_uint(data->chrm_green_x); break; // converting from unsigned;
            case MP_QSTR_chrm_green_y: dest[0] = mp_obj_new_int_from_uint(data->chrm_green_y); break; // converting from unsigned;
            case MP_QSTR_chrm_blue_x: dest[0] = mp_obj_new_int_from_uint(data->chrm_blue_x); break; // converting from unsigned;
            case MP_QSTR_chrm_blue_y: dest[0] = mp_obj_new_int_from_uint(data->chrm_blue_y); break; // converting from unsigned;
            case MP_QSTR_srgb_defined: dest[0] = mp_obj_new_int_from_uint(data->srgb_defined); break; // converting from unsigned;
            case MP_QSTR_srgb_intent: dest[0] = mp_obj_new_int_from_uint(data->srgb_intent); break; // converting from unsigned;
            case MP_QSTR_iccp_defined: dest[0] = mp_obj_new_int_from_uint(data->iccp_defined); break; // converting from unsigned;
            case MP_QSTR_iccp_name: dest[0] = convert_to_str((void*)data->iccp_name); break; // converting from char *;
            case MP_QSTR_iccp_profile: dest[0] = mp_array_from_u8ptr((void*)data->iccp_profile); break; // converting from unsigned char *;
            case MP_QSTR_iccp_profile_size: dest[0] = mp_obj_new_int_from_uint(data->iccp_profile_size); break; // converting from unsigned;
            case MP_QSTR_unknown_chunks_data: dest[0] = mp_arr_from_unsigned_char_ptr__3__(data->unknown_chunks_data); break; // converting from unsigned char *[3];
            case MP_QSTR_unknown_chunks_size: dest[0] = mp_arr_from_size_t___3__(data->unknown_chunks_size); break; // converting from size_t [3];
            default: call_parent_methods(self_in, attr, dest); // fallback to locals_dict lookup
        }
    } else {
        if (dest[1])
        {
            // store attribute
            switch(attr)
            {
                case MP_QSTR_compression_method: data->compression_method = (unsigned)mp_obj_get_int(dest[1]); break; // converting to unsigned;
                case MP_QSTR_filter_method: data->filter_method = (unsigned)mp_obj_get_int(dest[1]); break; // converting to unsigned;
                case MP_QSTR_interlace_method: data->interlace_method = (unsigned)mp_obj_get_int(dest[1]); break; // converting to unsigned;
                case MP_QSTR_color: data->color = mp_write_LodePNGColorMode(dest[1]); break; // converting to LodePNGColorMode;
                case MP_QSTR_background_defined: data->background_defined = (unsigned)mp_obj_get_int(dest[1]); break; // converting to unsigned;
                case MP_QSTR_background_r: data->background_r = (unsigned)mp_obj_get_int(dest[1]); break; // converting to unsigned;
                case MP_QSTR_background_g: data->background_g = (unsigned)mp_obj_get_int(dest[1]); break; // converting to unsigned;
                case MP_QSTR_background_b: data->background_b = (unsigned)mp_obj_get_int(dest[1]); break; // converting to unsigned;
                case MP_QSTR_text_num: data->text_num = (size_t)mp_obj_get_int(dest[1]); break; // converting to size_t;
                case MP_QSTR_text_keys: data->text_keys = (void*)mp_write_ptr_C_Pointer(dest[1]); break; // converting to char **;
                case MP_QSTR_text_strings: data->text_strings = (void*)mp_write_ptr_C_Pointer(dest[1]); break; // converting to char **;
                case MP_QSTR_itext_num: data->itext_num = (size_t)mp_obj_get_int(dest[1]); break; // converting to size_t;
                case MP_QSTR_itext_keys: data->itext_keys = (void*)mp_write_ptr_C_Pointer(dest[1]); break; // converting to char **;
                case MP_QSTR_itext_langtags: data->itext_langtags = (void*)mp_write_ptr_C_Pointer(dest[1]); break; // converting to char **;
                case MP_QSTR_itext_transkeys: data->itext_transkeys = (void*)mp_write_ptr_C_Pointer(dest[1]); break; // converting to char **;
                case MP_QSTR_itext_strings: data->itext_strings = (void*)mp_write_ptr_C_Pointer(dest[1]); break; // converting to char **;
                case MP_QSTR_time_defined: data->time_defined = (unsigned)mp_obj_get_int(dest[1]); break; // converting to unsigned;
                case MP_QSTR_time: data->time = mp_write_LodePNGTime(dest[1]); break; // converting to LodePNGTime;
                case MP_QSTR_phys_defined: data->phys_defined = (unsigned)mp_obj_get_int(dest[1]); break; // converting to unsigned;
                case MP_QSTR_phys_x: data->phys_x = (unsigned)mp_obj_get_int(dest[1]); break; // converting to unsigned;
                case MP_QSTR_phys_y: data->phys_y = (unsigned)mp_obj_get_int(dest[1]); break; // converting to unsigned;
                case MP_QSTR_phys_unit: data->phys_unit = (unsigned)mp_obj_get_int(dest[1]); break; // converting to unsigned;
                case MP_QSTR_gama_defined: data->gama_defined = (unsigned)mp_obj_get_int(dest[1]); break; // converting to unsigned;
                case MP_QSTR_gama_gamma: data->gama_gamma = (unsigned)mp_obj_get_int(dest[1]); break; // converting to unsigned;
                case MP_QSTR_chrm_defined: data->chrm_defined = (unsigned)mp_obj_get_int(dest[1]); break; // converting to unsigned;
                case MP_QSTR_chrm_white_x: data->chrm_white_x = (unsigned)mp_obj_get_int(dest[1]); break; // converting to unsigned;
                case MP_QSTR_chrm_white_y: data->chrm_white_y = (unsigned)mp_obj_get_int(dest[1]); break; // converting to unsigned;
                case MP_QSTR_chrm_red_x: data->chrm_red_x = (unsigned)mp_obj_get_int(dest[1]); break; // converting to unsigned;
                case MP_QSTR_chrm_red_y: data->chrm_red_y = (unsigned)mp_obj_get_int(dest[1]); break; // converting to unsigned;
                case MP_QSTR_chrm_green_x: data->chrm_green_x = (unsigned)mp_obj_get_int(dest[1]); break; // converting to unsigned;
                case MP_QSTR_chrm_green_y: data->chrm_green_y = (unsigned)mp_obj_get_int(dest[1]); break; // converting to unsigned;
                case MP_QSTR_chrm_blue_x: data->chrm_blue_x = (unsigned)mp_obj_get_int(dest[1]); break; // converting to unsigned;
                case MP_QSTR_chrm_blue_y: data->chrm_blue_y = (unsigned)mp_obj_get_int(dest[1]); break; // converting to unsigned;
                case MP_QSTR_srgb_defined: data->srgb_defined = (unsigned)mp_obj_get_int(dest[1]); break; // converting to unsigned;
                case MP_QSTR_srgb_intent: data->srgb_intent = (unsigned)mp_obj_get_int(dest[1]); break; // converting to unsigned;
                case MP_QSTR_iccp_defined: data->iccp_defined = (unsigned)mp_obj_get_int(dest[1]); break; // converting to unsigned;
                case MP_QSTR_iccp_name: data->iccp_name = (void*)(char*)convert_from_str(dest[1]); break; // converting to char *;
                case MP_QSTR_iccp_profile: data->iccp_profile = (void*)mp_array_to_u8ptr(dest[1]); break; // converting to unsigned char *;
                case MP_QSTR_iccp_profile_size: data->iccp_profile_size = (unsigned)mp_obj_get_int(dest[1]); break; // converting to unsigned;
                case MP_QSTR_unknown_chunks_data: memcpy((void*)&data->unknown_chunks_data, mp_arr_to_unsigned_char_ptr__3__(dest[1]), sizeof(unsigned char *)*3); break; // converting to unsigned char *[3];
                case MP_QSTR_unknown_chunks_size: memcpy((void*)&data->unknown_chunks_size, mp_arr_to_size_t___3__(dest[1]), sizeof(size_t)*3); break; // converting to size_t [3];
                default: return;
            }

            dest[0] = MP_OBJ_NULL; // indicate success
        }
    }
}

STATIC void mp_LodePNGInfo_print(const mp_print_t *print,
    mp_obj_t self_in,
    mp_print_kind_t kind)
{
    mp_printf(print, "struct LodePNGInfo");
}

STATIC const mp_obj_dict_t mp_LodePNGInfo_locals_dict;

STATIC const mp_obj_type_t mp_LodePNGInfo_type = {
    { &mp_type_type },
    .name = MP_QSTR_LodePNGInfo,
    .print = mp_LodePNGInfo_print,
    .make_new = make_new_lv_struct,
    .binary_op = lv_struct_binary_op,
    .subscr = lv_struct_subscr,
    .attr = mp_LodePNGInfo_attr,
    .locals_dict = (mp_obj_dict_t*)&mp_LodePNGInfo_locals_dict,
    .buffer_p = { .get_buffer = mp_blob_get_buffer },
    .parent = &mp_lv_base_struct_type
};

STATIC inline const mp_obj_type_t *get_mp_LodePNGInfo_type()
{
    return &mp_LodePNGInfo_type;
}
    

/*
 * Struct LodePNGDecoderSettings
 */

STATIC inline const mp_obj_type_t *get_mp_LodePNGDecoderSettings_type();

STATIC inline LodePNGDecoderSettings* mp_write_ptr_LodePNGDecoderSettings(mp_obj_t self_in)
{
    mp_lv_struct_t *self = MP_OBJ_TO_PTR(cast(self_in, get_mp_LodePNGDecoderSettings_type()));
    return (LodePNGDecoderSettings*)self->data;
}

#define mp_write_LodePNGDecoderSettings(struct_obj) *mp_write_ptr_LodePNGDecoderSettings(struct_obj)

STATIC inline mp_obj_t mp_read_ptr_LodePNGDecoderSettings(void *field)
{
    return lv_to_mp_struct(get_mp_LodePNGDecoderSettings_type(), field);
}

#define mp_read_LodePNGDecoderSettings(field) mp_read_ptr_LodePNGDecoderSettings(copy_buffer(&field, sizeof(LodePNGDecoderSettings)))
#define mp_read_byref_LodePNGDecoderSettings(field) mp_read_ptr_LodePNGDecoderSettings(&field)

STATIC void mp_LodePNGDecoderSettings_attr(mp_obj_t self_in, qstr attr, mp_obj_t *dest)
{
    mp_lv_struct_t *self = MP_OBJ_TO_PTR(self_in);
    LodePNGDecoderSettings *data = (LodePNGDecoderSettings*)self->data;

    if (dest[0] == MP_OBJ_NULL) {
        // load attribute
        switch(attr)
        {
            case MP_QSTR_zlibsettings: dest[0] = mp_read_byref_LodePNGDecompressSettings(data->zlibsettings); break; // converting from LodePNGDecompressSettings;
            case MP_QSTR_ignore_crc: dest[0] = mp_obj_new_int_from_uint(data->ignore_crc); break; // converting from unsigned;
            case MP_QSTR_ignore_critical: dest[0] = mp_obj_new_int_from_uint(data->ignore_critical); break; // converting from unsigned;
            case MP_QSTR_ignore_end: dest[0] = mp_obj_new_int_from_uint(data->ignore_end); break; // converting from unsigned;
            case MP_QSTR_color_convert: dest[0] = mp_obj_new_int_from_uint(data->color_convert); break; // converting from unsigned;
            case MP_QSTR_read_text_chunks: dest[0] = mp_obj_new_int_from_uint(data->read_text_chunks); break; // converting from unsigned;
            case MP_QSTR_remember_unknown_chunks: dest[0] = mp_obj_new_int_from_uint(data->remember_unknown_chunks); break; // converting from unsigned;
            case MP_QSTR_max_text_size: dest[0] = mp_obj_new_int_from_uint(data->max_text_size); break; // converting from size_t;
            case MP_QSTR_max_icc_size: dest[0] = mp_obj_new_int_from_uint(data->max_icc_size); break; // converting from size_t;
            default: call_parent_methods(self_in, attr, dest); // fallback to locals_dict lookup
        }
    } else {
        if (dest[1])
        {
            // store attribute
            switch(attr)
            {
                case MP_QSTR_zlibsettings: data->zlibsettings = mp_write_LodePNGDecompressSettings(dest[1]); break; // converting to LodePNGDecompressSettings;
                case MP_QSTR_ignore_crc: data->ignore_crc = (unsigned)mp_obj_get_int(dest[1]); break; // converting to unsigned;
                case MP_QSTR_ignore_critical: data->ignore_critical = (unsigned)mp_obj_get_int(dest[1]); break; // converting to unsigned;
                case MP_QSTR_ignore_end: data->ignore_end = (unsigned)mp_obj_get_int(dest[1]); break; // converting to unsigned;
                case MP_QSTR_color_convert: data->color_convert = (unsigned)mp_obj_get_int(dest[1]); break; // converting to unsigned;
                case MP_QSTR_read_text_chunks: data->read_text_chunks = (unsigned)mp_obj_get_int(dest[1]); break; // converting to unsigned;
                case MP_QSTR_remember_unknown_chunks: data->remember_unknown_chunks = (unsigned)mp_obj_get_int(dest[1]); break; // converting to unsigned;
                case MP_QSTR_max_text_size: data->max_text_size = (size_t)mp_obj_get_int(dest[1]); break; // converting to size_t;
                case MP_QSTR_max_icc_size: data->max_icc_size = (size_t)mp_obj_get_int(dest[1]); break; // converting to size_t;
                default: return;
            }

            dest[0] = MP_OBJ_NULL; // indicate success
        }
    }
}

STATIC void mp_LodePNGDecoderSettings_print(const mp_print_t *print,
    mp_obj_t self_in,
    mp_print_kind_t kind)
{
    mp_printf(print, "struct LodePNGDecoderSettings");
}

STATIC const mp_obj_dict_t mp_LodePNGDecoderSettings_locals_dict;

STATIC const mp_obj_type_t mp_LodePNGDecoderSettings_type = {
    { &mp_type_type },
    .name = MP_QSTR_LodePNGDecoderSettings,
    .print = mp_LodePNGDecoderSettings_print,
    .make_new = make_new_lv_struct,
    .binary_op = lv_struct_binary_op,
    .subscr = lv_struct_subscr,
    .attr = mp_LodePNGDecoderSettings_attr,
    .locals_dict = (mp_obj_dict_t*)&mp_LodePNGDecoderSettings_locals_dict,
    .buffer_p = { .get_buffer = mp_blob_get_buffer },
    .parent = &mp_lv_base_struct_type
};

STATIC inline const mp_obj_type_t *get_mp_LodePNGDecoderSettings_type()
{
    return &mp_LodePNGDecoderSettings_type;
}
    

/*
 * Struct LodePNGState
 */

STATIC inline const mp_obj_type_t *get_mp_LodePNGState_type();

STATIC inline LodePNGState* mp_write_ptr_LodePNGState(mp_obj_t self_in)
{
    mp_lv_struct_t *self = MP_OBJ_TO_PTR(cast(self_in, get_mp_LodePNGState_type()));
    return (LodePNGState*)self->data;
}

#define mp_write_LodePNGState(struct_obj) *mp_write_ptr_LodePNGState(struct_obj)

STATIC inline mp_obj_t mp_read_ptr_LodePNGState(void *field)
{
    return lv_to_mp_struct(get_mp_LodePNGState_type(), field);
}

#define mp_read_LodePNGState(field) mp_read_ptr_LodePNGState(copy_buffer(&field, sizeof(LodePNGState)))
#define mp_read_byref_LodePNGState(field) mp_read_ptr_LodePNGState(&field)

STATIC void mp_LodePNGState_attr(mp_obj_t self_in, qstr attr, mp_obj_t *dest)
{
    mp_lv_struct_t *self = MP_OBJ_TO_PTR(self_in);
    LodePNGState *data = (LodePNGState*)self->data;

    if (dest[0] == MP_OBJ_NULL) {
        // load attribute
        switch(attr)
        {
            case MP_QSTR_decoder: dest[0] = mp_read_byref_LodePNGDecoderSettings(data->decoder); break; // converting from LodePNGDecoderSettings;
            case MP_QSTR_info_raw: dest[0] = mp_read_byref_LodePNGColorMode(data->info_raw); break; // converting from LodePNGColorMode;
            case MP_QSTR_info_png: dest[0] = mp_read_byref_LodePNGInfo(data->info_png); break; // converting from LodePNGInfo;
            case MP_QSTR_error: dest[0] = mp_obj_new_int_from_uint(data->error); break; // converting from unsigned;
            default: call_parent_methods(self_in, attr, dest); // fallback to locals_dict lookup
        }
    } else {
        if (dest[1])
        {
            // store attribute
            switch(attr)
            {
                case MP_QSTR_decoder: data->decoder = mp_write_LodePNGDecoderSettings(dest[1]); break; // converting to LodePNGDecoderSettings;
                case MP_QSTR_info_raw: data->info_raw = mp_write_LodePNGColorMode(dest[1]); break; // converting to LodePNGColorMode;
                case MP_QSTR_info_png: data->info_png = mp_write_LodePNGInfo(dest[1]); break; // converting to LodePNGInfo;
                case MP_QSTR_error: data->error = (unsigned)mp_obj_get_int(dest[1]); break; // converting to unsigned;
                default: return;
            }

            dest[0] = MP_OBJ_NULL; // indicate success
        }
    }
}

STATIC void mp_LodePNGState_print(const mp_print_t *print,
    mp_obj_t self_in,
    mp_print_kind_t kind)
{
    mp_printf(print, "struct LodePNGState");
}

STATIC const mp_obj_dict_t mp_LodePNGState_locals_dict;

STATIC const mp_obj_type_t mp_LodePNGState_type = {
    { &mp_type_type },
    .name = MP_QSTR_LodePNGState,
    .print = mp_LodePNGState_print,
    .make_new = make_new_lv_struct,
    .binary_op = lv_struct_binary_op,
    .subscr = lv_struct_subscr,
    .attr = mp_LodePNGState_attr,
    .locals_dict = (mp_obj_dict_t*)&mp_LodePNGState_locals_dict,
    .buffer_p = { .get_buffer = mp_blob_get_buffer },
    .parent = &mp_lv_base_struct_type
};

STATIC inline const mp_obj_type_t *get_mp_LodePNGState_type()
{
    return &mp_LodePNGState_type;
}
    

/*
 * Array convertors for char [5]
 */

GENMPY_UNUSED STATIC char *mp_arr_to_char___5__(mp_obj_t mp_arr)
{
    mp_obj_t mp_len = mp_obj_len_maybe(mp_arr);
    if (mp_len == MP_OBJ_NULL) return mp_to_ptr(mp_arr);
    mp_int_t len = mp_obj_get_int(mp_len);
    //TODO check dim!
    char *lv_arr = (char*)m_malloc(len * sizeof(char));
    mp_obj_t iter = mp_getiter(mp_arr, NULL);
    mp_obj_t item;
    size_t i = 0;
    while ((item = mp_iternext(iter)) != MP_OBJ_STOP_ITERATION) {
        lv_arr[i++] = (char)mp_obj_get_int(item);
    }
    return (char *)lv_arr;
}

GENMPY_UNUSED STATIC mp_obj_t mp_arr_from_char___5__(char *arr)
{
    mp_obj_t obj_arr[5];
    for (size_t i=0; i<5; i++){
        obj_arr[i] = mp_obj_new_int(arr[i]);
    }
    return mp_obj_new_list(5, obj_arr); // TODO: return custom iterable object!
}


STATIC const mp_rom_map_elem_t mp_C_Pointer_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR___SIZE__), MP_ROM_PTR(MP_ROM_INT(sizeof(C_Pointer))) },
    
};

STATIC MP_DEFINE_CONST_DICT(mp_C_Pointer_locals_dict, mp_C_Pointer_locals_dict_table);
        

STATIC const mp_rom_map_elem_t mp_LodePNGDecompressSettings_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR___SIZE__), MP_ROM_PTR(MP_ROM_INT(sizeof(struct LodePNGDecompressSettings))) },
    
};

STATIC MP_DEFINE_CONST_DICT(mp_LodePNGDecompressSettings_locals_dict, mp_LodePNGDecompressSettings_locals_dict_table);
        

STATIC const mp_rom_map_elem_t mp_LodePNGColorMode_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR___SIZE__), MP_ROM_PTR(MP_ROM_INT(sizeof(LodePNGColorMode))) },
    
};

STATIC MP_DEFINE_CONST_DICT(mp_LodePNGColorMode_locals_dict, mp_LodePNGColorMode_locals_dict_table);
        

STATIC const mp_rom_map_elem_t mp_LodePNGInfo_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR___SIZE__), MP_ROM_PTR(MP_ROM_INT(sizeof(LodePNGInfo))) },
    
};

STATIC MP_DEFINE_CONST_DICT(mp_LodePNGInfo_locals_dict, mp_LodePNGInfo_locals_dict_table);
        

STATIC const mp_rom_map_elem_t mp_LodePNGTime_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR___SIZE__), MP_ROM_PTR(MP_ROM_INT(sizeof(LodePNGTime))) },
    
};

STATIC MP_DEFINE_CONST_DICT(mp_LodePNGTime_locals_dict, mp_LodePNGTime_locals_dict_table);
        

STATIC const mp_rom_map_elem_t mp_LodePNGDecoderSettings_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR___SIZE__), MP_ROM_PTR(MP_ROM_INT(sizeof(LodePNGDecoderSettings))) },
    
};

STATIC MP_DEFINE_CONST_DICT(mp_LodePNGDecoderSettings_locals_dict, mp_LodePNGDecoderSettings_locals_dict_table);
        

STATIC const mp_rom_map_elem_t mp_LodePNGState_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR___SIZE__), MP_ROM_PTR(MP_ROM_INT(sizeof(LodePNGState))) },
    
};

STATIC MP_DEFINE_CONST_DICT(mp_LodePNGState_locals_dict, mp_LodePNGState_locals_dict_table);
        

/* 
 *
 * Global Module Functions
 *
 */


/*
 * lodepng extension definition for:
 * unsigned lodepng_decode_memory(unsigned char **out, unsigned *w, unsigned *h, const unsigned char *in, size_t insize, LodePNGColorType colortype, unsigned bitdepth)
 */
 
STATIC mp_obj_t mp_lodepng_decode_memory(size_t mp_n_args, const mp_obj_t *mp_args, void *lv_func_ptr)
{
    unsigned char **out = mp_to_ptr(mp_args[0]);
    unsigned *w = mp_array_to_u32ptr(mp_args[1]);
    unsigned *h = mp_array_to_u32ptr(mp_args[2]);
    const unsigned char *in = (const unsigned char *)mp_array_to_u8ptr(mp_args[3]);
    size_t insize = (size_t)mp_obj_get_int(mp_args[4]);
    LodePNGColorType colortype = (int)mp_obj_get_int(mp_args[5]);
    unsigned bitdepth = (unsigned)mp_obj_get_int(mp_args[6]);
    unsigned _res = ((unsigned (*)(unsigned char **, unsigned *, unsigned *, const unsigned char *, size_t, LodePNGColorType, unsigned))lv_func_ptr)(out, w, h, in, insize, colortype, bitdepth);
    return mp_obj_new_int_from_uint(_res);
}

 

STATIC MP_DEFINE_CONST_LV_FUN_OBJ_STATIC_VAR(mp_lodepng_decode_memory_obj, 7, mp_lodepng_decode_memory, lodepng_decode_memory);
    

/*
 * lodepng extension definition for:
 * unsigned lodepng_decode32(unsigned char **out, unsigned *w, unsigned *h, const unsigned char *in, size_t insize)
 */
 
STATIC mp_obj_t mp_lodepng_decode32(size_t mp_n_args, const mp_obj_t *mp_args, void *lv_func_ptr)
{
    unsigned char **out = mp_to_ptr(mp_args[0]);
    unsigned *w = mp_array_to_u32ptr(mp_args[1]);
    unsigned *h = mp_array_to_u32ptr(mp_args[2]);
    const unsigned char *in = (const unsigned char *)mp_array_to_u8ptr(mp_args[3]);
    size_t insize = (size_t)mp_obj_get_int(mp_args[4]);
    unsigned _res = ((unsigned (*)(unsigned char **, unsigned *, unsigned *, const unsigned char *, size_t))lv_func_ptr)(out, w, h, in, insize);
    return mp_obj_new_int_from_uint(_res);
}

 

STATIC MP_DEFINE_CONST_LV_FUN_OBJ_STATIC_VAR(mp_lodepng_decode32_obj, 5, mp_lodepng_decode32, lodepng_decode32);
    
/* Reusing lodepng_decode32 for lodepng_decode24 */

STATIC MP_DEFINE_CONST_LV_FUN_OBJ_STATIC_VAR(mp_lodepng_decode24_obj, 5, mp_lodepng_decode32, lodepng_decode24);
    

/*
 * lodepng extension definition for:
 * unsigned lodepng_decode_file(unsigned char **out, unsigned *w, unsigned *h, const char *filename, LodePNGColorType colortype, unsigned bitdepth)
 */
 
STATIC mp_obj_t mp_lodepng_decode_file(size_t mp_n_args, const mp_obj_t *mp_args, void *lv_func_ptr)
{
    unsigned char **out = mp_to_ptr(mp_args[0]);
    unsigned *w = mp_array_to_u32ptr(mp_args[1]);
    unsigned *h = mp_array_to_u32ptr(mp_args[2]);
    const char *filename = (const char *)(char*)convert_from_str(mp_args[3]);
    LodePNGColorType colortype = (int)mp_obj_get_int(mp_args[4]);
    unsigned bitdepth = (unsigned)mp_obj_get_int(mp_args[5]);
    unsigned _res = ((unsigned (*)(unsigned char **, unsigned *, unsigned *, const char *, LodePNGColorType, unsigned))lv_func_ptr)(out, w, h, filename, colortype, bitdepth);
    return mp_obj_new_int_from_uint(_res);
}

 

STATIC MP_DEFINE_CONST_LV_FUN_OBJ_STATIC_VAR(mp_lodepng_decode_file_obj, 6, mp_lodepng_decode_file, lodepng_decode_file);
    

/*
 * lodepng extension definition for:
 * unsigned lodepng_decode32_file(unsigned char **out, unsigned *w, unsigned *h, const char *filename)
 */
 
STATIC mp_obj_t mp_lodepng_decode32_file(size_t mp_n_args, const mp_obj_t *mp_args, void *lv_func_ptr)
{
    unsigned char **out = mp_to_ptr(mp_args[0]);
    unsigned *w = mp_array_to_u32ptr(mp_args[1]);
    unsigned *h = mp_array_to_u32ptr(mp_args[2]);
    const char *filename = (const char *)(char*)convert_from_str(mp_args[3]);
    unsigned _res = ((unsigned (*)(unsigned char **, unsigned *, unsigned *, const char *))lv_func_ptr)(out, w, h, filename);
    return mp_obj_new_int_from_uint(_res);
}

 

STATIC MP_DEFINE_CONST_LV_FUN_OBJ_STATIC_VAR(mp_lodepng_decode32_file_obj, 4, mp_lodepng_decode32_file, lodepng_decode32_file);
    
/* Reusing lodepng_decode32_file for lodepng_decode24_file */

STATIC MP_DEFINE_CONST_LV_FUN_OBJ_STATIC_VAR(mp_lodepng_decode24_file_obj, 4, mp_lodepng_decode32_file, lodepng_decode24_file);
    

/*
 * lodepng extension definition for:
 * const char *lodepng_error_text(unsigned code)
 */
 
STATIC mp_obj_t mp_lodepng_error_text(size_t mp_n_args, const mp_obj_t *mp_args, void *lv_func_ptr)
{
    unsigned code = (unsigned)mp_obj_get_int(mp_args[0]);
    const char * _res = ((const char *(*)(unsigned))lv_func_ptr)(code);
    return convert_to_str((void*)_res);
}

 

STATIC MP_DEFINE_CONST_LV_FUN_OBJ_STATIC_VAR(mp_lodepng_error_text_obj, 1, mp_lodepng_error_text, lodepng_error_text);
    

/*
 * lodepng extension definition for:
 * void lodepng_decompress_settings_init(LodePNGDecompressSettings *settings)
 */
 
STATIC mp_obj_t mp_lodepng_decompress_settings_init(size_t mp_n_args, const mp_obj_t *mp_args, void *lv_func_ptr)
{
    LodePNGDecompressSettings *settings = mp_write_ptr_LodePNGDecompressSettings(mp_args[0]);
    ((void (*)(LodePNGDecompressSettings *))lv_func_ptr)(settings);
    return mp_const_none;
}

 

STATIC MP_DEFINE_CONST_LV_FUN_OBJ_STATIC_VAR(mp_lodepng_decompress_settings_init_obj, 1, mp_lodepng_decompress_settings_init, lodepng_decompress_settings_init);
    

/*
 * lodepng extension definition for:
 * void lodepng_color_mode_init(LodePNGColorMode *info)
 */
 
STATIC mp_obj_t mp_lodepng_color_mode_init(size_t mp_n_args, const mp_obj_t *mp_args, void *lv_func_ptr)
{
    LodePNGColorMode *info = mp_write_ptr_LodePNGColorMode(mp_args[0]);
    ((void (*)(LodePNGColorMode *))lv_func_ptr)(info);
    return mp_const_none;
}

 

STATIC MP_DEFINE_CONST_LV_FUN_OBJ_STATIC_VAR(mp_lodepng_color_mode_init_obj, 1, mp_lodepng_color_mode_init, lodepng_color_mode_init);
    
/* Reusing lodepng_color_mode_init for lodepng_color_mode_cleanup */

STATIC MP_DEFINE_CONST_LV_FUN_OBJ_STATIC_VAR(mp_lodepng_color_mode_cleanup_obj, 1, mp_lodepng_color_mode_init, lodepng_color_mode_cleanup);
    

/*
 * lodepng extension definition for:
 * unsigned lodepng_color_mode_copy(LodePNGColorMode *dest, const LodePNGColorMode *source)
 */
 
STATIC mp_obj_t mp_lodepng_color_mode_copy(size_t mp_n_args, const mp_obj_t *mp_args, void *lv_func_ptr)
{
    LodePNGColorMode *dest = mp_write_ptr_LodePNGColorMode(mp_args[0]);
    const LodePNGColorMode *source = (const LodePNGColorMode *)mp_write_ptr_LodePNGColorMode(mp_args[1]);
    unsigned _res = ((unsigned (*)(LodePNGColorMode *, const LodePNGColorMode *))lv_func_ptr)(dest, source);
    return mp_obj_new_int_from_uint(_res);
}

 

STATIC MP_DEFINE_CONST_LV_FUN_OBJ_STATIC_VAR(mp_lodepng_color_mode_copy_obj, 2, mp_lodepng_color_mode_copy, lodepng_color_mode_copy);
    

/*
 * lodepng extension definition for:
 * LodePNGColorMode lodepng_color_mode_make(LodePNGColorType colortype, unsigned bitdepth)
 */
 
STATIC mp_obj_t mp_lodepng_color_mode_make(size_t mp_n_args, const mp_obj_t *mp_args, void *lv_func_ptr)
{
    LodePNGColorType colortype = (int)mp_obj_get_int(mp_args[0]);
    unsigned bitdepth = (unsigned)mp_obj_get_int(mp_args[1]);
    LodePNGColorMode _res = ((LodePNGColorMode (*)(LodePNGColorType, unsigned))lv_func_ptr)(colortype, bitdepth);
    return mp_read_LodePNGColorMode(_res);
}

 

STATIC MP_DEFINE_CONST_LV_FUN_OBJ_STATIC_VAR(mp_lodepng_color_mode_make_obj, 2, mp_lodepng_color_mode_make, lodepng_color_mode_make);
    
/* Reusing lodepng_color_mode_init for lodepng_palette_clear */

STATIC MP_DEFINE_CONST_LV_FUN_OBJ_STATIC_VAR(mp_lodepng_palette_clear_obj, 1, mp_lodepng_color_mode_init, lodepng_palette_clear);
    

/*
 * lodepng extension definition for:
 * unsigned lodepng_palette_add(LodePNGColorMode *info, unsigned char r, unsigned char g, unsigned char b, unsigned char a)
 */
 
STATIC mp_obj_t mp_lodepng_palette_add(size_t mp_n_args, const mp_obj_t *mp_args, void *lv_func_ptr)
{
    LodePNGColorMode *info = mp_write_ptr_LodePNGColorMode(mp_args[0]);
    unsigned char r = (unsigned char)mp_obj_get_int(mp_args[1]);
    unsigned char g = (unsigned char)mp_obj_get_int(mp_args[2]);
    unsigned char b = (unsigned char)mp_obj_get_int(mp_args[3]);
    unsigned char a = (unsigned char)mp_obj_get_int(mp_args[4]);
    unsigned _res = ((unsigned (*)(LodePNGColorMode *, unsigned char, unsigned char, unsigned char, unsigned char))lv_func_ptr)(info, r, g, b, a);
    return mp_obj_new_int_from_uint(_res);
}

 

STATIC MP_DEFINE_CONST_LV_FUN_OBJ_STATIC_VAR(mp_lodepng_palette_add_obj, 5, mp_lodepng_palette_add, lodepng_palette_add);
    

/*
 * lodepng extension definition for:
 * unsigned lodepng_get_bpp(const LodePNGColorMode *info)
 */
 
STATIC mp_obj_t mp_lodepng_get_bpp(size_t mp_n_args, const mp_obj_t *mp_args, void *lv_func_ptr)
{
    const LodePNGColorMode *info = (const LodePNGColorMode *)mp_write_ptr_LodePNGColorMode(mp_args[0]);
    unsigned _res = ((unsigned (*)(const LodePNGColorMode *))lv_func_ptr)(info);
    return mp_obj_new_int_from_uint(_res);
}

 

STATIC MP_DEFINE_CONST_LV_FUN_OBJ_STATIC_VAR(mp_lodepng_get_bpp_obj, 1, mp_lodepng_get_bpp, lodepng_get_bpp);
    
/* Reusing lodepng_get_bpp for lodepng_get_channels */

STATIC MP_DEFINE_CONST_LV_FUN_OBJ_STATIC_VAR(mp_lodepng_get_channels_obj, 1, mp_lodepng_get_bpp, lodepng_get_channels);
    
/* Reusing lodepng_get_bpp for lodepng_is_greyscale_type */

STATIC MP_DEFINE_CONST_LV_FUN_OBJ_STATIC_VAR(mp_lodepng_is_greyscale_type_obj, 1, mp_lodepng_get_bpp, lodepng_is_greyscale_type);
    
/* Reusing lodepng_get_bpp for lodepng_is_alpha_type */

STATIC MP_DEFINE_CONST_LV_FUN_OBJ_STATIC_VAR(mp_lodepng_is_alpha_type_obj, 1, mp_lodepng_get_bpp, lodepng_is_alpha_type);
    
/* Reusing lodepng_get_bpp for lodepng_is_palette_type */

STATIC MP_DEFINE_CONST_LV_FUN_OBJ_STATIC_VAR(mp_lodepng_is_palette_type_obj, 1, mp_lodepng_get_bpp, lodepng_is_palette_type);
    
/* Reusing lodepng_get_bpp for lodepng_has_palette_alpha */

STATIC MP_DEFINE_CONST_LV_FUN_OBJ_STATIC_VAR(mp_lodepng_has_palette_alpha_obj, 1, mp_lodepng_get_bpp, lodepng_has_palette_alpha);
    
/* Reusing lodepng_get_bpp for lodepng_can_have_alpha */

STATIC MP_DEFINE_CONST_LV_FUN_OBJ_STATIC_VAR(mp_lodepng_can_have_alpha_obj, 1, mp_lodepng_get_bpp, lodepng_can_have_alpha);
    

/*
 * lodepng extension definition for:
 * size_t lodepng_get_raw_size(unsigned w, unsigned h, const LodePNGColorMode *color)
 */
 
STATIC mp_obj_t mp_lodepng_get_raw_size(size_t mp_n_args, const mp_obj_t *mp_args, void *lv_func_ptr)
{
    unsigned w = (unsigned)mp_obj_get_int(mp_args[0]);
    unsigned h = (unsigned)mp_obj_get_int(mp_args[1]);
    const LodePNGColorMode *color = (const LodePNGColorMode *)mp_write_ptr_LodePNGColorMode(mp_args[2]);
    size_t _res = ((size_t (*)(unsigned, unsigned, const LodePNGColorMode *))lv_func_ptr)(w, h, color);
    return mp_obj_new_int_from_uint(_res);
}

 

STATIC MP_DEFINE_CONST_LV_FUN_OBJ_STATIC_VAR(mp_lodepng_get_raw_size_obj, 3, mp_lodepng_get_raw_size, lodepng_get_raw_size);
    

/*
 * lodepng extension definition for:
 * void lodepng_info_init(LodePNGInfo *info)
 */
 
STATIC mp_obj_t mp_lodepng_info_init(size_t mp_n_args, const mp_obj_t *mp_args, void *lv_func_ptr)
{
    LodePNGInfo *info = mp_write_ptr_LodePNGInfo(mp_args[0]);
    ((void (*)(LodePNGInfo *))lv_func_ptr)(info);
    return mp_const_none;
}

 

STATIC MP_DEFINE_CONST_LV_FUN_OBJ_STATIC_VAR(mp_lodepng_info_init_obj, 1, mp_lodepng_info_init, lodepng_info_init);
    
/* Reusing lodepng_info_init for lodepng_info_cleanup */

STATIC MP_DEFINE_CONST_LV_FUN_OBJ_STATIC_VAR(mp_lodepng_info_cleanup_obj, 1, mp_lodepng_info_init, lodepng_info_cleanup);
    

/*
 * lodepng extension definition for:
 * unsigned lodepng_info_copy(LodePNGInfo *dest, const LodePNGInfo *source)
 */
 
STATIC mp_obj_t mp_lodepng_info_copy(size_t mp_n_args, const mp_obj_t *mp_args, void *lv_func_ptr)
{
    LodePNGInfo *dest = mp_write_ptr_LodePNGInfo(mp_args[0]);
    const LodePNGInfo *source = (const LodePNGInfo *)mp_write_ptr_LodePNGInfo(mp_args[1]);
    unsigned _res = ((unsigned (*)(LodePNGInfo *, const LodePNGInfo *))lv_func_ptr)(dest, source);
    return mp_obj_new_int_from_uint(_res);
}

 

STATIC MP_DEFINE_CONST_LV_FUN_OBJ_STATIC_VAR(mp_lodepng_info_copy_obj, 2, mp_lodepng_info_copy, lodepng_info_copy);
    

/*
 * lodepng extension definition for:
 * unsigned lodepng_add_text(LodePNGInfo *info, const char *key, const char *str)
 */
 
STATIC mp_obj_t mp_lodepng_add_text(size_t mp_n_args, const mp_obj_t *mp_args, void *lv_func_ptr)
{
    LodePNGInfo *info = mp_write_ptr_LodePNGInfo(mp_args[0]);
    const char *key = (const char *)(char*)convert_from_str(mp_args[1]);
    const char *str = (const char *)(char*)convert_from_str(mp_args[2]);
    unsigned _res = ((unsigned (*)(LodePNGInfo *, const char *, const char *))lv_func_ptr)(info, key, str);
    return mp_obj_new_int_from_uint(_res);
}

 

STATIC MP_DEFINE_CONST_LV_FUN_OBJ_STATIC_VAR(mp_lodepng_add_text_obj, 3, mp_lodepng_add_text, lodepng_add_text);
    
/* Reusing lodepng_info_init for lodepng_clear_text */

STATIC MP_DEFINE_CONST_LV_FUN_OBJ_STATIC_VAR(mp_lodepng_clear_text_obj, 1, mp_lodepng_info_init, lodepng_clear_text);
    

/*
 * lodepng extension definition for:
 * unsigned lodepng_add_itext(LodePNGInfo *info, const char *key, const char *langtag, const char *transkey, const char *str)
 */
 
STATIC mp_obj_t mp_lodepng_add_itext(size_t mp_n_args, const mp_obj_t *mp_args, void *lv_func_ptr)
{
    LodePNGInfo *info = mp_write_ptr_LodePNGInfo(mp_args[0]);
    const char *key = (const char *)(char*)convert_from_str(mp_args[1]);
    const char *langtag = (const char *)(char*)convert_from_str(mp_args[2]);
    const char *transkey = (const char *)(char*)convert_from_str(mp_args[3]);
    const char *str = (const char *)(char*)convert_from_str(mp_args[4]);
    unsigned _res = ((unsigned (*)(LodePNGInfo *, const char *, const char *, const char *, const char *))lv_func_ptr)(info, key, langtag, transkey, str);
    return mp_obj_new_int_from_uint(_res);
}

 

STATIC MP_DEFINE_CONST_LV_FUN_OBJ_STATIC_VAR(mp_lodepng_add_itext_obj, 5, mp_lodepng_add_itext, lodepng_add_itext);
    
/* Reusing lodepng_info_init for lodepng_clear_itext */

STATIC MP_DEFINE_CONST_LV_FUN_OBJ_STATIC_VAR(mp_lodepng_clear_itext_obj, 1, mp_lodepng_info_init, lodepng_clear_itext);
    

/*
 * lodepng extension definition for:
 * unsigned lodepng_set_icc(LodePNGInfo *info, const char *name, const unsigned char *profile, unsigned profile_size)
 */
 
STATIC mp_obj_t mp_lodepng_set_icc(size_t mp_n_args, const mp_obj_t *mp_args, void *lv_func_ptr)
{
    LodePNGInfo *info = mp_write_ptr_LodePNGInfo(mp_args[0]);
    const char *name = (const char *)(char*)convert_from_str(mp_args[1]);
    const unsigned char *profile = (const unsigned char *)mp_array_to_u8ptr(mp_args[2]);
    unsigned profile_size = (unsigned)mp_obj_get_int(mp_args[3]);
    unsigned _res = ((unsigned (*)(LodePNGInfo *, const char *, const unsigned char *, unsigned))lv_func_ptr)(info, name, profile, profile_size);
    return mp_obj_new_int_from_uint(_res);
}

 

STATIC MP_DEFINE_CONST_LV_FUN_OBJ_STATIC_VAR(mp_lodepng_set_icc_obj, 4, mp_lodepng_set_icc, lodepng_set_icc);
    
/* Reusing lodepng_info_init for lodepng_clear_icc */

STATIC MP_DEFINE_CONST_LV_FUN_OBJ_STATIC_VAR(mp_lodepng_clear_icc_obj, 1, mp_lodepng_info_init, lodepng_clear_icc);
    

/*
 * lodepng extension definition for:
 * unsigned lodepng_convert(unsigned char *out, const unsigned char *in, const LodePNGColorMode *mode_out, const LodePNGColorMode *mode_in, unsigned w, unsigned h)
 */
 
STATIC mp_obj_t mp_lodepng_convert(size_t mp_n_args, const mp_obj_t *mp_args, void *lv_func_ptr)
{
    unsigned char *out = mp_array_to_u8ptr(mp_args[0]);
    const unsigned char *in = (const unsigned char *)mp_array_to_u8ptr(mp_args[1]);
    const LodePNGColorMode *mode_out = (const LodePNGColorMode *)mp_write_ptr_LodePNGColorMode(mp_args[2]);
    const LodePNGColorMode *mode_in = (const LodePNGColorMode *)mp_write_ptr_LodePNGColorMode(mp_args[3]);
    unsigned w = (unsigned)mp_obj_get_int(mp_args[4]);
    unsigned h = (unsigned)mp_obj_get_int(mp_args[5]);
    unsigned _res = ((unsigned (*)(unsigned char *, const unsigned char *, const LodePNGColorMode *, const LodePNGColorMode *, unsigned, unsigned))lv_func_ptr)(out, in, mode_out, mode_in, w, h);
    return mp_obj_new_int_from_uint(_res);
}

 

STATIC MP_DEFINE_CONST_LV_FUN_OBJ_STATIC_VAR(mp_lodepng_convert_obj, 6, mp_lodepng_convert, lodepng_convert);
    

/*
 * lodepng extension definition for:
 * void lodepng_decoder_settings_init(LodePNGDecoderSettings *settings)
 */
 
STATIC mp_obj_t mp_lodepng_decoder_settings_init(size_t mp_n_args, const mp_obj_t *mp_args, void *lv_func_ptr)
{
    LodePNGDecoderSettings *settings = mp_write_ptr_LodePNGDecoderSettings(mp_args[0]);
    ((void (*)(LodePNGDecoderSettings *))lv_func_ptr)(settings);
    return mp_const_none;
}

 

STATIC MP_DEFINE_CONST_LV_FUN_OBJ_STATIC_VAR(mp_lodepng_decoder_settings_init_obj, 1, mp_lodepng_decoder_settings_init, lodepng_decoder_settings_init);
    

/*
 * lodepng extension definition for:
 * void lodepng_state_init(LodePNGState *state)
 */
 
STATIC mp_obj_t mp_lodepng_state_init(size_t mp_n_args, const mp_obj_t *mp_args, void *lv_func_ptr)
{
    LodePNGState *state = mp_write_ptr_LodePNGState(mp_args[0]);
    ((void (*)(LodePNGState *))lv_func_ptr)(state);
    return mp_const_none;
}

 

STATIC MP_DEFINE_CONST_LV_FUN_OBJ_STATIC_VAR(mp_lodepng_state_init_obj, 1, mp_lodepng_state_init, lodepng_state_init);
    
/* Reusing lodepng_state_init for lodepng_state_cleanup */

STATIC MP_DEFINE_CONST_LV_FUN_OBJ_STATIC_VAR(mp_lodepng_state_cleanup_obj, 1, mp_lodepng_state_init, lodepng_state_cleanup);
    

/*
 * lodepng extension definition for:
 * void lodepng_state_copy(LodePNGState *dest, const LodePNGState *source)
 */
 
STATIC mp_obj_t mp_lodepng_state_copy(size_t mp_n_args, const mp_obj_t *mp_args, void *lv_func_ptr)
{
    LodePNGState *dest = mp_write_ptr_LodePNGState(mp_args[0]);
    const LodePNGState *source = (const LodePNGState *)mp_write_ptr_LodePNGState(mp_args[1]);
    ((void (*)(LodePNGState *, const LodePNGState *))lv_func_ptr)(dest, source);
    return mp_const_none;
}

 

STATIC MP_DEFINE_CONST_LV_FUN_OBJ_STATIC_VAR(mp_lodepng_state_copy_obj, 2, mp_lodepng_state_copy, lodepng_state_copy);
    

/*
 * lodepng extension definition for:
 * unsigned lodepng_decode(unsigned char **out, unsigned *w, unsigned *h, LodePNGState *state, const unsigned char *in, size_t insize)
 */
 
STATIC mp_obj_t mp_lodepng_decode(size_t mp_n_args, const mp_obj_t *mp_args, void *lv_func_ptr)
{
    unsigned char **out = mp_to_ptr(mp_args[0]);
    unsigned *w = mp_array_to_u32ptr(mp_args[1]);
    unsigned *h = mp_array_to_u32ptr(mp_args[2]);
    LodePNGState *state = mp_write_ptr_LodePNGState(mp_args[3]);
    const unsigned char *in = (const unsigned char *)mp_array_to_u8ptr(mp_args[4]);
    size_t insize = (size_t)mp_obj_get_int(mp_args[5]);
    unsigned _res = ((unsigned (*)(unsigned char **, unsigned *, unsigned *, LodePNGState *, const unsigned char *, size_t))lv_func_ptr)(out, w, h, state, in, insize);
    return mp_obj_new_int_from_uint(_res);
}

 

STATIC MP_DEFINE_CONST_LV_FUN_OBJ_STATIC_VAR(mp_lodepng_decode_obj, 6, mp_lodepng_decode, lodepng_decode);
    

/*
 * lodepng extension definition for:
 * unsigned lodepng_inspect(unsigned *w, unsigned *h, LodePNGState *state, const unsigned char *in, size_t insize)
 */
 
STATIC mp_obj_t mp_lodepng_inspect(size_t mp_n_args, const mp_obj_t *mp_args, void *lv_func_ptr)
{
    unsigned *w = mp_array_to_u32ptr(mp_args[0]);
    unsigned *h = mp_array_to_u32ptr(mp_args[1]);
    LodePNGState *state = mp_write_ptr_LodePNGState(mp_args[2]);
    const unsigned char *in = (const unsigned char *)mp_array_to_u8ptr(mp_args[3]);
    size_t insize = (size_t)mp_obj_get_int(mp_args[4]);
    unsigned _res = ((unsigned (*)(unsigned *, unsigned *, LodePNGState *, const unsigned char *, size_t))lv_func_ptr)(w, h, state, in, insize);
    return mp_obj_new_int_from_uint(_res);
}

 

STATIC MP_DEFINE_CONST_LV_FUN_OBJ_STATIC_VAR(mp_lodepng_inspect_obj, 5, mp_lodepng_inspect, lodepng_inspect);
    

/*
 * lodepng extension definition for:
 * unsigned lodepng_inspect_chunk(LodePNGState *state, size_t pos, const unsigned char *in, size_t insize)
 */
 
STATIC mp_obj_t mp_lodepng_inspect_chunk(size_t mp_n_args, const mp_obj_t *mp_args, void *lv_func_ptr)
{
    LodePNGState *state = mp_write_ptr_LodePNGState(mp_args[0]);
    size_t pos = (size_t)mp_obj_get_int(mp_args[1]);
    const unsigned char *in = (const unsigned char *)mp_array_to_u8ptr(mp_args[2]);
    size_t insize = (size_t)mp_obj_get_int(mp_args[3]);
    unsigned _res = ((unsigned (*)(LodePNGState *, size_t, const unsigned char *, size_t))lv_func_ptr)(state, pos, in, insize);
    return mp_obj_new_int_from_uint(_res);
}

 

STATIC MP_DEFINE_CONST_LV_FUN_OBJ_STATIC_VAR(mp_lodepng_inspect_chunk_obj, 4, mp_lodepng_inspect_chunk, lodepng_inspect_chunk);
    

/*
 * lodepng extension definition for:
 * unsigned lodepng_chunk_length(const unsigned char *chunk)
 */
 
STATIC mp_obj_t mp_lodepng_chunk_length(size_t mp_n_args, const mp_obj_t *mp_args, void *lv_func_ptr)
{
    const unsigned char *chunk = (const unsigned char *)mp_array_to_u8ptr(mp_args[0]);
    unsigned _res = ((unsigned (*)(const unsigned char *))lv_func_ptr)(chunk);
    return mp_obj_new_int_from_uint(_res);
}

 

STATIC MP_DEFINE_CONST_LV_FUN_OBJ_STATIC_VAR(mp_lodepng_chunk_length_obj, 1, mp_lodepng_chunk_length, lodepng_chunk_length);
    

/*
 * lodepng extension definition for:
 * void lodepng_chunk_type(char type[5], const unsigned char *chunk)
 */
 
STATIC mp_obj_t mp_lodepng_chunk_type(size_t mp_n_args, const mp_obj_t *mp_args, void *lv_func_ptr)
{
    char *type = mp_arr_to_char___5__(mp_args[0]);
    const unsigned char *chunk = (const unsigned char *)mp_array_to_u8ptr(mp_args[1]);
    ((void (*)(char [5], const unsigned char *))lv_func_ptr)(type, chunk);
    return mp_const_none;
}

 

STATIC MP_DEFINE_CONST_LV_FUN_OBJ_STATIC_VAR(mp_lodepng_chunk_type_obj, 2, mp_lodepng_chunk_type, lodepng_chunk_type);
    

/*
 * lodepng extension definition for:
 * unsigned char lodepng_chunk_type_equals(const unsigned char *chunk, const char *type)
 */
 
STATIC mp_obj_t mp_lodepng_chunk_type_equals(size_t mp_n_args, const mp_obj_t *mp_args, void *lv_func_ptr)
{
    const unsigned char *chunk = (const unsigned char *)mp_array_to_u8ptr(mp_args[0]);
    const char *type = (const char *)(char*)convert_from_str(mp_args[1]);
    unsigned char _res = ((unsigned char (*)(const unsigned char *, const char *))lv_func_ptr)(chunk, type);
    return mp_obj_new_int_from_uint(_res);
}

 

STATIC MP_DEFINE_CONST_LV_FUN_OBJ_STATIC_VAR(mp_lodepng_chunk_type_equals_obj, 2, mp_lodepng_chunk_type_equals, lodepng_chunk_type_equals);
    

/*
 * lodepng extension definition for:
 * unsigned char lodepng_chunk_ancillary(const unsigned char *chunk)
 */
 
STATIC mp_obj_t mp_lodepng_chunk_ancillary(size_t mp_n_args, const mp_obj_t *mp_args, void *lv_func_ptr)
{
    const unsigned char *chunk = (const unsigned char *)mp_array_to_u8ptr(mp_args[0]);
    unsigned char _res = ((unsigned char (*)(const unsigned char *))lv_func_ptr)(chunk);
    return mp_obj_new_int_from_uint(_res);
}

 

STATIC MP_DEFINE_CONST_LV_FUN_OBJ_STATIC_VAR(mp_lodepng_chunk_ancillary_obj, 1, mp_lodepng_chunk_ancillary, lodepng_chunk_ancillary);
    
/* Reusing lodepng_chunk_ancillary for lodepng_chunk_private */

STATIC MP_DEFINE_CONST_LV_FUN_OBJ_STATIC_VAR(mp_lodepng_chunk_private_obj, 1, mp_lodepng_chunk_ancillary, lodepng_chunk_private);
    
/* Reusing lodepng_chunk_ancillary for lodepng_chunk_safetocopy */

STATIC MP_DEFINE_CONST_LV_FUN_OBJ_STATIC_VAR(mp_lodepng_chunk_safetocopy_obj, 1, mp_lodepng_chunk_ancillary, lodepng_chunk_safetocopy);
    

/*
 * lodepng extension definition for:
 * unsigned char *lodepng_chunk_data(unsigned char *chunk)
 */
 
STATIC mp_obj_t mp_lodepng_chunk_data(size_t mp_n_args, const mp_obj_t *mp_args, void *lv_func_ptr)
{
    unsigned char *chunk = mp_array_to_u8ptr(mp_args[0]);
    unsigned char * _res = ((unsigned char *(*)(unsigned char *))lv_func_ptr)(chunk);
    return mp_array_from_u8ptr((void*)_res);
}

 

STATIC MP_DEFINE_CONST_LV_FUN_OBJ_STATIC_VAR(mp_lodepng_chunk_data_obj, 1, mp_lodepng_chunk_data, lodepng_chunk_data);
    

/*
 * lodepng extension definition for:
 * const unsigned char *lodepng_chunk_data_const(const unsigned char *chunk)
 */
 
STATIC mp_obj_t mp_lodepng_chunk_data_const(size_t mp_n_args, const mp_obj_t *mp_args, void *lv_func_ptr)
{
    const unsigned char *chunk = (const unsigned char *)mp_array_to_u8ptr(mp_args[0]);
    const unsigned char * _res = ((const unsigned char *(*)(const unsigned char *))lv_func_ptr)(chunk);
    return mp_array_from_u8ptr((void*)_res);
}

 

STATIC MP_DEFINE_CONST_LV_FUN_OBJ_STATIC_VAR(mp_lodepng_chunk_data_const_obj, 1, mp_lodepng_chunk_data_const, lodepng_chunk_data_const);
    
/* Reusing lodepng_chunk_length for lodepng_chunk_check_crc */

STATIC MP_DEFINE_CONST_LV_FUN_OBJ_STATIC_VAR(mp_lodepng_chunk_check_crc_obj, 1, mp_lodepng_chunk_length, lodepng_chunk_check_crc);
    

/*
 * lodepng extension definition for:
 * void lodepng_chunk_generate_crc(unsigned char *chunk)
 */
 
STATIC mp_obj_t mp_lodepng_chunk_generate_crc(size_t mp_n_args, const mp_obj_t *mp_args, void *lv_func_ptr)
{
    unsigned char *chunk = mp_array_to_u8ptr(mp_args[0]);
    ((void (*)(unsigned char *))lv_func_ptr)(chunk);
    return mp_const_none;
}

 

STATIC MP_DEFINE_CONST_LV_FUN_OBJ_STATIC_VAR(mp_lodepng_chunk_generate_crc_obj, 1, mp_lodepng_chunk_generate_crc, lodepng_chunk_generate_crc);
    

/*
 * lodepng extension definition for:
 * unsigned char *lodepng_chunk_next(unsigned char *chunk, unsigned char *end)
 */
 
STATIC mp_obj_t mp_lodepng_chunk_next(size_t mp_n_args, const mp_obj_t *mp_args, void *lv_func_ptr)
{
    unsigned char *chunk = mp_array_to_u8ptr(mp_args[0]);
    unsigned char *end = mp_array_to_u8ptr(mp_args[1]);
    unsigned char * _res = ((unsigned char *(*)(unsigned char *, unsigned char *))lv_func_ptr)(chunk, end);
    return mp_array_from_u8ptr((void*)_res);
}

 

STATIC MP_DEFINE_CONST_LV_FUN_OBJ_STATIC_VAR(mp_lodepng_chunk_next_obj, 2, mp_lodepng_chunk_next, lodepng_chunk_next);
    

/*
 * lodepng extension definition for:
 * const unsigned char *lodepng_chunk_next_const(const unsigned char *chunk, const unsigned char *end)
 */
 
STATIC mp_obj_t mp_lodepng_chunk_next_const(size_t mp_n_args, const mp_obj_t *mp_args, void *lv_func_ptr)
{
    const unsigned char *chunk = (const unsigned char *)mp_array_to_u8ptr(mp_args[0]);
    const unsigned char *end = (const unsigned char *)mp_array_to_u8ptr(mp_args[1]);
    const unsigned char * _res = ((const unsigned char *(*)(const unsigned char *, const unsigned char *))lv_func_ptr)(chunk, end);
    return mp_array_from_u8ptr((void*)_res);
}

 

STATIC MP_DEFINE_CONST_LV_FUN_OBJ_STATIC_VAR(mp_lodepng_chunk_next_const_obj, 2, mp_lodepng_chunk_next_const, lodepng_chunk_next_const);
    

/*
 * lodepng extension definition for:
 * unsigned char *lodepng_chunk_find(unsigned char *chunk, unsigned char *end, const char type[5])
 */
 
STATIC mp_obj_t mp_lodepng_chunk_find(size_t mp_n_args, const mp_obj_t *mp_args, void *lv_func_ptr)
{
    unsigned char *chunk = mp_array_to_u8ptr(mp_args[0]);
    unsigned char *end = mp_array_to_u8ptr(mp_args[1]);
    const char *type = (const char *)mp_arr_to_char___5__(mp_args[2]);
    unsigned char * _res = ((unsigned char *(*)(unsigned char *, unsigned char *, const char [5]))lv_func_ptr)(chunk, end, type);
    return mp_array_from_u8ptr((void*)_res);
}

 

STATIC MP_DEFINE_CONST_LV_FUN_OBJ_STATIC_VAR(mp_lodepng_chunk_find_obj, 3, mp_lodepng_chunk_find, lodepng_chunk_find);
    

/*
 * lodepng extension definition for:
 * const unsigned char *lodepng_chunk_find_const(const unsigned char *chunk, const unsigned char *end, const char type[5])
 */
 
STATIC mp_obj_t mp_lodepng_chunk_find_const(size_t mp_n_args, const mp_obj_t *mp_args, void *lv_func_ptr)
{
    const unsigned char *chunk = (const unsigned char *)mp_array_to_u8ptr(mp_args[0]);
    const unsigned char *end = (const unsigned char *)mp_array_to_u8ptr(mp_args[1]);
    const char *type = (const char *)mp_arr_to_char___5__(mp_args[2]);
    const unsigned char * _res = ((const unsigned char *(*)(const unsigned char *, const unsigned char *, const char [5]))lv_func_ptr)(chunk, end, type);
    return mp_array_from_u8ptr((void*)_res);
}

 

STATIC MP_DEFINE_CONST_LV_FUN_OBJ_STATIC_VAR(mp_lodepng_chunk_find_const_obj, 3, mp_lodepng_chunk_find_const, lodepng_chunk_find_const);
    

/*
 * lodepng extension definition for:
 * unsigned lodepng_chunk_append(unsigned char **out, size_t *outsize, const unsigned char *chunk)
 */
 
STATIC mp_obj_t mp_lodepng_chunk_append(size_t mp_n_args, const mp_obj_t *mp_args, void *lv_func_ptr)
{
    unsigned char **out = mp_to_ptr(mp_args[0]);
    size_t *outsize = mp_array_to_u32ptr(mp_args[1]);
    const unsigned char *chunk = (const unsigned char *)mp_array_to_u8ptr(mp_args[2]);
    unsigned _res = ((unsigned (*)(unsigned char **, size_t *, const unsigned char *))lv_func_ptr)(out, outsize, chunk);
    return mp_obj_new_int_from_uint(_res);
}

 

STATIC MP_DEFINE_CONST_LV_FUN_OBJ_STATIC_VAR(mp_lodepng_chunk_append_obj, 3, mp_lodepng_chunk_append, lodepng_chunk_append);
    

/*
 * lodepng extension definition for:
 * unsigned lodepng_chunk_create(unsigned char **out, size_t *outsize, unsigned length, const char *type, const unsigned char *data)
 */
 
STATIC mp_obj_t mp_lodepng_chunk_create(size_t mp_n_args, const mp_obj_t *mp_args, void *lv_func_ptr)
{
    unsigned char **out = mp_to_ptr(mp_args[0]);
    size_t *outsize = mp_array_to_u32ptr(mp_args[1]);
    unsigned length = (unsigned)mp_obj_get_int(mp_args[2]);
    const char *type = (const char *)(char*)convert_from_str(mp_args[3]);
    const unsigned char *data = (const unsigned char *)mp_array_to_u8ptr(mp_args[4]);
    unsigned _res = ((unsigned (*)(unsigned char **, size_t *, unsigned, const char *, const unsigned char *))lv_func_ptr)(out, outsize, length, type, data);
    return mp_obj_new_int_from_uint(_res);
}

 

STATIC MP_DEFINE_CONST_LV_FUN_OBJ_STATIC_VAR(mp_lodepng_chunk_create_obj, 5, mp_lodepng_chunk_create, lodepng_chunk_create);
    

/*
 * lodepng extension definition for:
 * unsigned lodepng_crc32(const unsigned char *buf, size_t len)
 */
 
STATIC mp_obj_t mp_lodepng_crc32(size_t mp_n_args, const mp_obj_t *mp_args, void *lv_func_ptr)
{
    const unsigned char *buf = (const unsigned char *)mp_array_to_u8ptr(mp_args[0]);
    size_t len = (size_t)mp_obj_get_int(mp_args[1]);
    unsigned _res = ((unsigned (*)(const unsigned char *, size_t))lv_func_ptr)(buf, len);
    return mp_obj_new_int_from_uint(_res);
}

 

STATIC MP_DEFINE_CONST_LV_FUN_OBJ_STATIC_VAR(mp_lodepng_crc32_obj, 2, mp_lodepng_crc32, lodepng_crc32);
    
/* Reusing funcptr_custom_zlib for lodepng_inflate */

STATIC MP_DEFINE_CONST_LV_FUN_OBJ_STATIC_VAR(mp_lodepng_inflate_obj, 5, mp_funcptr_custom_zlib, lodepng_inflate);
    
/* Reusing funcptr_custom_zlib for lodepng_zlib_decompress */

STATIC MP_DEFINE_CONST_LV_FUN_OBJ_STATIC_VAR(mp_lodepng_zlib_decompress_obj, 5, mp_funcptr_custom_zlib, lodepng_zlib_decompress);
    

/*
 * lodepng extension definition for:
 * unsigned lodepng_load_file(unsigned char **out, size_t *outsize, const char *filename)
 */
 
STATIC mp_obj_t mp_lodepng_load_file(size_t mp_n_args, const mp_obj_t *mp_args, void *lv_func_ptr)
{
    unsigned char **out = mp_to_ptr(mp_args[0]);
    size_t *outsize = mp_array_to_u32ptr(mp_args[1]);
    const char *filename = (const char *)(char*)convert_from_str(mp_args[2]);
    unsigned _res = ((unsigned (*)(unsigned char **, size_t *, const char *))lv_func_ptr)(out, outsize, filename);
    return mp_obj_new_int_from_uint(_res);
}

 

STATIC MP_DEFINE_CONST_LV_FUN_OBJ_STATIC_VAR(mp_lodepng_load_file_obj, 3, mp_lodepng_load_file, lodepng_load_file);
    

/*
 * lodepng extension definition for:
 * unsigned lodepng_save_file(const unsigned char *buffer, size_t buffersize, const char *filename)
 */
 
STATIC mp_obj_t mp_lodepng_save_file(size_t mp_n_args, const mp_obj_t *mp_args, void *lv_func_ptr)
{
    const unsigned char *buffer = (const unsigned char *)mp_array_to_u8ptr(mp_args[0]);
    size_t buffersize = (size_t)mp_obj_get_int(mp_args[1]);
    const char *filename = (const char *)(char*)convert_from_str(mp_args[2]);
    unsigned _res = ((unsigned (*)(const unsigned char *, size_t, const char *))lv_func_ptr)(buffer, buffersize, filename);
    return mp_obj_new_int_from_uint(_res);
}

 

STATIC MP_DEFINE_CONST_LV_FUN_OBJ_STATIC_VAR(mp_lodepng_save_file_obj, 3, mp_lodepng_save_file, lodepng_save_file);
    

/*
 * Function NOT generated:
 * Callback: user_data NOT FOUND! unsigned custom_zlib(unsigned char **, size_t *, const unsigned char *, size_t, const LodePNGDecompressSettings *)
 * unsigned custom_zlib(unsigned char **, size_t *, const unsigned char *, size_t, const LodePNGDecompressSettings *)
 */
    

/*
 * Function NOT generated:
 * Callback: user_data NOT FOUND! unsigned custom_inflate(unsigned char **, size_t *, const unsigned char *, size_t, const LodePNGDecompressSettings *)
 * unsigned custom_inflate(unsigned char **, size_t *, const unsigned char *, size_t, const LodePNGDecompressSettings *)
 */
    


/*
 * lodepng module definitions
 */

STATIC const mp_rom_map_elem_t lodepng_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_lodepng) },
    
    { MP_ROM_QSTR(MP_QSTR_decode_memory), MP_ROM_PTR(&mp_lodepng_decode_memory_obj) },
    { MP_ROM_QSTR(MP_QSTR_decode32), MP_ROM_PTR(&mp_lodepng_decode32_obj) },
    { MP_ROM_QSTR(MP_QSTR_decode24), MP_ROM_PTR(&mp_lodepng_decode24_obj) },
    { MP_ROM_QSTR(MP_QSTR_decode_file), MP_ROM_PTR(&mp_lodepng_decode_file_obj) },
    { MP_ROM_QSTR(MP_QSTR_decode32_file), MP_ROM_PTR(&mp_lodepng_decode32_file_obj) },
    { MP_ROM_QSTR(MP_QSTR_decode24_file), MP_ROM_PTR(&mp_lodepng_decode24_file_obj) },
    { MP_ROM_QSTR(MP_QSTR_error_text), MP_ROM_PTR(&mp_lodepng_error_text_obj) },
    { MP_ROM_QSTR(MP_QSTR_decompress_settings_init), MP_ROM_PTR(&mp_lodepng_decompress_settings_init_obj) },
    { MP_ROM_QSTR(MP_QSTR_color_mode_init), MP_ROM_PTR(&mp_lodepng_color_mode_init_obj) },
    { MP_ROM_QSTR(MP_QSTR_color_mode_cleanup), MP_ROM_PTR(&mp_lodepng_color_mode_cleanup_obj) },
    { MP_ROM_QSTR(MP_QSTR_color_mode_copy), MP_ROM_PTR(&mp_lodepng_color_mode_copy_obj) },
    { MP_ROM_QSTR(MP_QSTR_color_mode_make), MP_ROM_PTR(&mp_lodepng_color_mode_make_obj) },
    { MP_ROM_QSTR(MP_QSTR_palette_clear), MP_ROM_PTR(&mp_lodepng_palette_clear_obj) },
    { MP_ROM_QSTR(MP_QSTR_palette_add), MP_ROM_PTR(&mp_lodepng_palette_add_obj) },
    { MP_ROM_QSTR(MP_QSTR_get_bpp), MP_ROM_PTR(&mp_lodepng_get_bpp_obj) },
    { MP_ROM_QSTR(MP_QSTR_get_channels), MP_ROM_PTR(&mp_lodepng_get_channels_obj) },
    { MP_ROM_QSTR(MP_QSTR_is_greyscale_type), MP_ROM_PTR(&mp_lodepng_is_greyscale_type_obj) },
    { MP_ROM_QSTR(MP_QSTR_is_alpha_type), MP_ROM_PTR(&mp_lodepng_is_alpha_type_obj) },
    { MP_ROM_QSTR(MP_QSTR_is_palette_type), MP_ROM_PTR(&mp_lodepng_is_palette_type_obj) },
    { MP_ROM_QSTR(MP_QSTR_has_palette_alpha), MP_ROM_PTR(&mp_lodepng_has_palette_alpha_obj) },
    { MP_ROM_QSTR(MP_QSTR_can_have_alpha), MP_ROM_PTR(&mp_lodepng_can_have_alpha_obj) },
    { MP_ROM_QSTR(MP_QSTR_get_raw_size), MP_ROM_PTR(&mp_lodepng_get_raw_size_obj) },
    { MP_ROM_QSTR(MP_QSTR_info_init), MP_ROM_PTR(&mp_lodepng_info_init_obj) },
    { MP_ROM_QSTR(MP_QSTR_info_cleanup), MP_ROM_PTR(&mp_lodepng_info_cleanup_obj) },
    { MP_ROM_QSTR(MP_QSTR_info_copy), MP_ROM_PTR(&mp_lodepng_info_copy_obj) },
    { MP_ROM_QSTR(MP_QSTR_add_text), MP_ROM_PTR(&mp_lodepng_add_text_obj) },
    { MP_ROM_QSTR(MP_QSTR_clear_text), MP_ROM_PTR(&mp_lodepng_clear_text_obj) },
    { MP_ROM_QSTR(MP_QSTR_add_itext), MP_ROM_PTR(&mp_lodepng_add_itext_obj) },
    { MP_ROM_QSTR(MP_QSTR_clear_itext), MP_ROM_PTR(&mp_lodepng_clear_itext_obj) },
    { MP_ROM_QSTR(MP_QSTR_set_icc), MP_ROM_PTR(&mp_lodepng_set_icc_obj) },
    { MP_ROM_QSTR(MP_QSTR_clear_icc), MP_ROM_PTR(&mp_lodepng_clear_icc_obj) },
    { MP_ROM_QSTR(MP_QSTR_convert), MP_ROM_PTR(&mp_lodepng_convert_obj) },
    { MP_ROM_QSTR(MP_QSTR_decoder_settings_init), MP_ROM_PTR(&mp_lodepng_decoder_settings_init_obj) },
    { MP_ROM_QSTR(MP_QSTR_state_init), MP_ROM_PTR(&mp_lodepng_state_init_obj) },
    { MP_ROM_QSTR(MP_QSTR_state_cleanup), MP_ROM_PTR(&mp_lodepng_state_cleanup_obj) },
    { MP_ROM_QSTR(MP_QSTR_state_copy), MP_ROM_PTR(&mp_lodepng_state_copy_obj) },
    { MP_ROM_QSTR(MP_QSTR_decode), MP_ROM_PTR(&mp_lodepng_decode_obj) },
    { MP_ROM_QSTR(MP_QSTR_inspect), MP_ROM_PTR(&mp_lodepng_inspect_obj) },
    { MP_ROM_QSTR(MP_QSTR_inspect_chunk), MP_ROM_PTR(&mp_lodepng_inspect_chunk_obj) },
    { MP_ROM_QSTR(MP_QSTR_chunk_length), MP_ROM_PTR(&mp_lodepng_chunk_length_obj) },
    { MP_ROM_QSTR(MP_QSTR_chunk_type), MP_ROM_PTR(&mp_lodepng_chunk_type_obj) },
    { MP_ROM_QSTR(MP_QSTR_chunk_type_equals), MP_ROM_PTR(&mp_lodepng_chunk_type_equals_obj) },
    { MP_ROM_QSTR(MP_QSTR_chunk_ancillary), MP_ROM_PTR(&mp_lodepng_chunk_ancillary_obj) },
    { MP_ROM_QSTR(MP_QSTR_chunk_private), MP_ROM_PTR(&mp_lodepng_chunk_private_obj) },
    { MP_ROM_QSTR(MP_QSTR_chunk_safetocopy), MP_ROM_PTR(&mp_lodepng_chunk_safetocopy_obj) },
    { MP_ROM_QSTR(MP_QSTR_chunk_data), MP_ROM_PTR(&mp_lodepng_chunk_data_obj) },
    { MP_ROM_QSTR(MP_QSTR_chunk_data_const), MP_ROM_PTR(&mp_lodepng_chunk_data_const_obj) },
    { MP_ROM_QSTR(MP_QSTR_chunk_check_crc), MP_ROM_PTR(&mp_lodepng_chunk_check_crc_obj) },
    { MP_ROM_QSTR(MP_QSTR_chunk_generate_crc), MP_ROM_PTR(&mp_lodepng_chunk_generate_crc_obj) },
    { MP_ROM_QSTR(MP_QSTR_chunk_next), MP_ROM_PTR(&mp_lodepng_chunk_next_obj) },
    { MP_ROM_QSTR(MP_QSTR_chunk_next_const), MP_ROM_PTR(&mp_lodepng_chunk_next_const_obj) },
    { MP_ROM_QSTR(MP_QSTR_chunk_find), MP_ROM_PTR(&mp_lodepng_chunk_find_obj) },
    { MP_ROM_QSTR(MP_QSTR_chunk_find_const), MP_ROM_PTR(&mp_lodepng_chunk_find_const_obj) },
    { MP_ROM_QSTR(MP_QSTR_chunk_append), MP_ROM_PTR(&mp_lodepng_chunk_append_obj) },
    { MP_ROM_QSTR(MP_QSTR_chunk_create), MP_ROM_PTR(&mp_lodepng_chunk_create_obj) },
    { MP_ROM_QSTR(MP_QSTR_crc32), MP_ROM_PTR(&mp_lodepng_crc32_obj) },
    { MP_ROM_QSTR(MP_QSTR_inflate), MP_ROM_PTR(&mp_lodepng_inflate_obj) },
    { MP_ROM_QSTR(MP_QSTR_zlib_decompress), MP_ROM_PTR(&mp_lodepng_zlib_decompress_obj) },
    { MP_ROM_QSTR(MP_QSTR_load_file), MP_ROM_PTR(&mp_lodepng_load_file_obj) },
    { MP_ROM_QSTR(MP_QSTR_save_file), MP_ROM_PTR(&mp_lodepng_save_file_obj) },
    
    { MP_ROM_QSTR(MP_QSTR_LCT), MP_ROM_PTR(&mp_lv_LCT_type.mp_obj_type) },
    
    { MP_ROM_QSTR(MP_QSTR_C_Pointer), MP_ROM_PTR(&mp_C_Pointer_type) },
    { MP_ROM_QSTR(MP_QSTR_LodePNGDecompressSettings), MP_ROM_PTR(&mp_LodePNGDecompressSettings_type) },
    { MP_ROM_QSTR(MP_QSTR_LodePNGColorMode), MP_ROM_PTR(&mp_LodePNGColorMode_type) },
    { MP_ROM_QSTR(MP_QSTR_LodePNGInfo), MP_ROM_PTR(&mp_LodePNGInfo_type) },
    { MP_ROM_QSTR(MP_QSTR_LodePNGTime), MP_ROM_PTR(&mp_LodePNGTime_type) },
    { MP_ROM_QSTR(MP_QSTR_LodePNGDecoderSettings), MP_ROM_PTR(&mp_LodePNGDecoderSettings_type) },
    { MP_ROM_QSTR(MP_QSTR_LodePNGState), MP_ROM_PTR(&mp_LodePNGState_type) },
    
    
    { MP_ROM_QSTR(MP_QSTR_default_decompress_settings), MP_ROM_PTR(&mp_lodepng_default_decompress_settings) },
    
    
#ifdef LV_OBJ_T
    { MP_ROM_QSTR(MP_QSTR_LvReferenceError), MP_ROM_PTR(&mp_type_LvReferenceError) },
#endif // LV_OBJ_T
};


STATIC MP_DEFINE_CONST_DICT (
    mp_module_lodepng_globals,
    lodepng_globals_table
);

const mp_obj_module_t mp_module_lodepng = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t*)&mp_module_lodepng_globals
};

MP_REGISTER_MODULE(MP_QSTR_lodepng, mp_module_lodepng, MICROPY_PY_LVGL);

#endif // MICROPY_PY_LVGL
