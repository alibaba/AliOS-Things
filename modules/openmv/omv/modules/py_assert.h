/*
 * This file is part of the OpenMV project.
 *
 * Copyright (c) 2013-2021 Ibrahim Abdelkader <iabdalkader@openmv.io>
 * Copyright (c) 2013-2021 Kwabena W. Agyeman <kwagyeman@openmv.io>
 *
 * This work is licensed under the MIT license, see the file LICENSE for details.
 *
 * MP assertions.
 */
#ifndef __PY_ASSERT_H__
#define __PY_ASSERT_H__
#define PY_ASSERT_TRUE(cond)                            \
    do {                                                \
        if ((cond) == 0) {                              \
            mp_raise_msg(&mp_type_OSError,              \
                         MP_ERROR_TEXT(                 \
                         "Operation not supported"));   \
        }                                               \
    } while(0)

#define PY_ASSERT_TRUE_MSG(cond, msg)                   \
    do {                                                \
        if ((cond) == 0) {                              \
            mp_raise_msg(&mp_type_OSError,              \
                         MP_ERROR_TEXT(msg));           \
        }                                               \
    } while(0)

#define PY_ASSERT_FALSE_MSG(cond, msg)                  \
    do {                                                \
        if ((cond) == 1) {                              \
            mp_raise_msg(&mp_type_OSError,              \
                         MP_ERROR_TEXT(msg));           \
        }                                               \
    } while(0)

#define PY_ASSERT_TYPE(obj, type)                       \
    do {                                                \
        __typeof__ (obj) _a = (obj);                    \
        __typeof__ (type) _b = (type);                  \
        if (!MP_OBJ_IS_TYPE(_a, _b)) {                  \
            mp_raise_msg_varg(&mp_type_TypeError,       \
                        MP_ERROR_TEXT(                  \
                        "Can't convert %s to %s"),      \
                        mp_obj_get_type_str(_a),        \
                        mp_obj_get_type_str(_b));       \
        }                                               \
    } while(0)
/* IS_TYPE doesn't work for str objs */
#define PY_ASSERT_STR(obj)                              \
    do {                                                \
        __typeof__ (obj) _a = (obj);                    \
        if (!MP_OBJ_IS_STR(_a)) {                       \
            mp_raise_msg_varg(                          \
                        &mp_type_TypeError,             \
                        MP_ERROR_TEXT(                  \
                        "Can't convert %s to %s"),      \
                        mp_obj_get_type_str(_a),        \
                        str_type.name);                 \
        }                                               \
    } while(0)

#endif // __PY_ASSERT_H__
