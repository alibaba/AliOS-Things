/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */
#ifndef K_OBJ_BASE_H
#define K_OBJ_BASE_H

#include <stdint.h>
#include <stdlib.h>
#include <k_api.h>
#include "object_api.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* convert a specific object type's struct ptr to an obj_core_t ptr */
#define OBJ_RESOLVE(obj)                ((obj_core_t *)(obj))

/* convert an obj_core_t ptr to a specific object type's struct ptr */
#define OBJ_UNRESOLVE(obj)              ((void *) (obj))

/* convert from owner_node to object ID */

#define OBJ_OWNER_NODE_TO_ID(pNode) ((obj_core_t *)((long)(pNode) - \
                                     OFFSET (obj_core_t, owner_node)))

#define MAX_ROUTINE_NAME_LEN         64  /* maximum length for routine symbol */
#define OBJ_ACCESS_DELETED           0x8000
#define OBJ_ACCESS_FREE_ON_RELEASE   (OBJ_ACCESS_DELETED + 1)


#define OBJ_ACCESS_INC(obj)                \
    do                                     \
    {                                      \
        CPSR_ALLOC();                      \
        RHINO_CRITICAL_ENTER();            \
        (obj)->access_cnt++;               \
        RHINO_CRITICAL_EXIT();             \
    } while (RHINO_FALSE )

#define OBJ_ACCESS_DEC(obj)                \
    do                                     \
    {                                      \
        CPSR_ALLOC();                      \
        RHINO_CRITICAL_ENTER();            \
        (obj)->access_cnt--;               \
        RHINO_CRITICAL_EXIT();             \
    } while (RHINO_FALSE )

#define OBJ_ACCESS_SET_DELETED(obj)                \
    do                                             \
    {                                              \
        CPSR_ALLOC();                              \
        RHINO_CRITICAL_ENTER();                    \
        (obj)->access_cnt |= OBJ_ACCESS_DELETED;   \
        RHINO_CRITICAL_EXIT();                     \
    } while (RHINO_FALSE )

#define OBJ_REFCNT_INC(obj)                        \
    do                                             \
    {                                              \
        CPSR_ALLOC();                              \
        RHINO_CRITICAL_ENTER();                    \
        (obj)->ref_cnt++;                          \
        RHINO_CRITICAL_EXIT();                     \
    } while (RHINO_FALSE )

#if 0
#define OBJ_REFCNT_DEC(obj)              \
    do{                                  \
        CPSR_ALLOC();                    \
        RHINO_CRITICAL_ENTER();          \
        (obj)->ref_cnt--;                \
        RHINO_CRITICAL_EXIT();           \
    } while (RHINO_FALSE )
#endif

#define OBJ_REFCNT_DEC_UNSAFE(obj)       \
    (obj)->ref_cnt--


#define OBJ_EACH(plist, node, routine, arg, obj_id)                       \
    {                                                                     \
        node = plist->next;                                               \
        while (node != plist) {                                           \
            obj_id = krhino_list_entry(node, obj_core_t, class_node);     \
            if ((* routine) (obj_id, arg) == RHINO_FALSE ) {              \
                break;                                                    \
            }                                                             \
            node = node->next;                                            \
            obj_id = NULL;                                                \
        }                                                                 \
    }

#define OBJ_VERIFY(obj, data)                                             \
    (                                                                     \
        (                                                                 \
            (((obj_core_t *) (obj))->magic == (unsigned long)(data))      \
        ) ? OK : ERROR                                                    \
    )

typedef RHINO_BOOL(*obj_each_func_t)(void *obj_id, void *pArg);

typedef int (*obj_owner_lock_func_t)(void *proc_id);
typedef int (*obj_owner_unlock_func_t)(void *proc_id);
typedef void (*obj_owner_add_func_t)(struct obj_core *obj_core);
typedef void (*obj_owner_remove_func_t)(struct obj_core *obj_core);
typedef int (*obj_owner_reclaim_func_t)(struct obj_core *obj_core);
typedef int (*obj_owner_setbase_func_t)(struct obj_core *obj, struct obj_core *pOwner);
typedef int (*obj_owner_set_func_t)(void *obj_id, void *owner_id);

typedef void (*obj_hfree_all_func_t)(struct obj_core *obj_core);
typedef obj_handle_t (*obj_halloc_func_t)(void *obj_id, void *proc_id);
typedef int (*obj_core_rts_func_t)(void *obj_id);
typedef void *(*obj_handle_to_id_func_t)(obj_handle_t handle, void *proc_id,
        RHINO_BOOL access);
typedef int (*obj_name_set_func_t)(struct obj_core *obj, obj_class_t *class_id,
                                   char *name);

/* variable declarations */
extern RHINO_BOOL obj_owner_lib_installed;
extern obj_owner_lock_func_t      _func_owner_list_lock;
extern obj_owner_unlock_func_t    _func_owner_list_unlock;
extern obj_owner_add_func_t       _func_obj_owner_list_add;
extern obj_owner_remove_func_t    _func_obj_owner_list_remove;
extern obj_owner_reclaim_func_t   _func_obj_owner_reclaim;
extern obj_owner_setbase_func_t   _func_obj_owner_set_base_internal;
extern obj_owner_set_func_t       _func_obj_owner_set_internal;
extern obj_hfree_all_func_t    _func_handle_free_all;
extern obj_halloc_func_t       _func_handle_alloc;
extern obj_core_rts_func_t     _func_obj_core_release;
extern obj_handle_to_id_func_t _func_obj_handle_to_obj_id;
extern obj_name_set_func_t     _func_obj_name_verified_set;

/* function declarations */
void *obj_alloc_func(void *mem_pool, size_t nbytes, size_t alignment);
int obj_free_func(void *mem_pool, void *obj);
obj_core_t *obj_each(enum OBJ_CLASS_TYPE class_type,
                     obj_each_func_t routine, void *pArg,
                     RHINO_BOOL objPublic);
int  obj_generic_verify(void *obj_id);
int  obj_owner_set_base(obj_core_t *obj, obj_core_t *owner_obj);
int  obj_lib_init(obj_alloc_func_t obj_mem_alloc_rtn,
                  obj_free_func_t obj_mem_free_rtn,
                  void *obj_mem_pool_id, int options);
void obj_ownership_init(void);
RHINO_BOOL obj_owner_list_installed(void);
int  obj_core_reclaim(obj_core_t *obj_core);
void    *obj_mem_alloc(size_t nBytes);
int  obj_mem_free(void *pBuffer);
char *obj_routine_name_get(routine_func_t entry, char *string, size_t strLen);
int  obj_verified_safe(void *obj_id, obj_class_t *class_id);
void obj_core_unlink(obj_core_t *obj_core, obj_class_t *class_id);
int _obj_close(obj_core_t *obj_core, int level, RHINO_BOOL dealloc);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* K_OBJ_BASE_H */

