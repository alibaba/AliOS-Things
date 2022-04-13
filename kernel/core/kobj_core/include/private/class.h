/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#ifndef K_CLASS_H
#define K_CLASS_H

#include <stdint.h>
#include <k_api.h>
#include "private/obj.h"
#include "object_api.h"

#ifdef __cplusplus
extern "C" {
#endif

#define OFFSET(structure, member)   /* byte offset of member in structure*/\
    ((size_t) &(((structure *) 0) -> member))

/* class options */
#define CLASS_ALLOW_DUPLICATE_NAMES     0x01
#define CLASS_RENAME_NOT_ALLOWED        0x02
#define CLASS_INSTRUMENTED              0x04
#define CLASS_STACKTRACE                0x08
#define CLASS_ADD_TO_CLASS_LIST         0x10

/* Locking of class list */
#define CLASS_LIST_LOCK(lock)                   \
    krhino_mutex_lock(lock, RHINO_WAIT_FOREVER)

#define CLASS_LIST_UNLOCK(lock)                 \
    krhino_mutex_unlock (lock)

/* Initialize list maintaining tasks when class list configuration disabled */
#define TASK_CLASS_LIST_INIT(class_id, lock)                \
    klist_init (&(class_id)->obj_priv_list);                \
    krhino_mutex_create(lock, "obj_class_lock")

/* Add/remove objects to class list */
#define CLASS_LIST_ADD(list, node)  klist_add (list, node)
#define CLASS_LIST_REMOVE(node)     klist_rm (node)


#define CLASS_NODE_TO_OBJ_ID(pNode)                    \
    (obj_core_t *) ((long)(pNode) - OFFSET (obj_core_t, class_node))

#define CLASS_LIST_INFO(list, max, element)             \
    {                                                   \
        klist_t * pNode;                                \
        pNode = &list;                                  \
        while ((pNode != NULL) && (--max >= 0))         \
        {                                               \
            *(element++) = CLASS_NODE_TO_OBJ_ID (pNode); \
            pNode = pNode->next;                        \
        }                                               \
    }

typedef void (*class_list_init_func_t)(obj_class_t *obj_class,
                                       enum OBJ_CLASS_TYPE obj_class_type);
typedef void (*class_list_add_func_t)(obj_core_t *obj_core,
                                      obj_class_t *class_id);
typedef void (*class_list_remove_func_t)(obj_core_t *obj_core,
        obj_class_t *class_id);
typedef  int (*class_list_tbl_init_func_t)(void);
typedef  int (*class_list_lock_func_t)(obj_class_t *class_id);
typedef  int (*class_list_unlock_func_t)(obj_class_t *class_id);

/* variable declarations */
extern obj_class_t *class_id_table [];   /* array of class IDs */
extern class_list_lock_func_t     _func_class_list_lock;
extern class_list_unlock_func_t   _func_class_list_unlock;
extern class_list_init_func_t     _func_class_list_init;
extern class_list_add_func_t      _func_class_list_add;
extern class_list_remove_func_t   _func_class_list_remove;

/* function declarations */
int class_lib_init(void);
int class_list_lib_init(void);
obj_class_t *obj_class_id_get(enum OBJ_CLASS_TYPE class_type);

#ifdef __cplusplus
}
#endif

#endif /* K_CLASS_H */

