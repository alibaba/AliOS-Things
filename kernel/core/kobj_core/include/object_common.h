/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#ifndef K_OBJ_COMMON_H
#define K_OBJ_COMMON_H
#include <stdint.h>
#include <k_api.h>
#include <errno.h>


#ifdef __cplusplus
extern "C" {
#endif

#define OBJ_CONFIG_QUICK_MODE   1
/* max object name length */
#define OBJ_NAME_MAX             1024

#define OBJ_NAME_DYNAMIC         0x01  /* object name dynamically allocated */
#define OBJ_DEALLOC_MEM          0x02  /* free object memory on destroy */
#define OBJ_INSTRUMENTED         0x04  /* instrumented object */
#define OBJ_DELETE_ON_LAST_CLOSE 0x08  /* object will be deleted on the last close even if it is not previously unlinked */
#define OBJ_ORPHANED             0x10  /* unable to reclaim object */
#define OBJ_NO_RECLAIM           0x20  /* automatic reclamation disabled */
#define OBJ_PUBLIC               0x40  /* object is public */
#define OBJ_NAMED                0x80  /* object is named */
#define OBJ_DELETED              0x100 /* object is deleted */
#define OBJ_STACKTRACE           0x200 /* systemview stacktrace object events */

#define OBJ_HANDLE_INDEX_BITS     16  /* bit count for handle index */
#define OBJ_HANDLE_GEN_BITS       15  /* bit count for generation nunmber */
#define OBJ_HANDLE_ENTRY_BITS     8   /* portion of index used for */
#define OBJ_HANDLE_INDEX_CNT       (1 << OBJ_HANDLE_INDEX_BITS)    /* 00010000 */
#define OBJ_HANDLE_TBL_SIZE        (1 << OBJ_HANDLE_ENTRY_BITS)        /* 256 */
#define OBJ_MASTER_TBL_SIZE        (OBJ_HANDLE_INDEX_CNT / OBJ_HANDLE_TBL_SIZE)


/* Error Status codes */
/* status codes */
#define K_OBJ_ERRNO_BASE                           0x20000
#define K_OBJ_ERRNO_OBJ_ID_ERROR                   (K_OBJ_ERRNO_BASE | 1)
#define K_OBJ_ERRNO_OBJ_UNAVAILABLE                (K_OBJ_ERRNO_BASE | 2)
#define K_OBJ_ERRNO_OBJ_DELETED                    (K_OBJ_ERRNO_BASE | 3)
#define K_OBJ_ERRNO_OBJ_TIMEOUT                    (K_OBJ_ERRNO_BASE | 4)
#define K_OBJ_ERRNO_OBJ_NO_METHOD                  (K_OBJ_ERRNO_BASE | 5)
#define K_OBJ_ERRNO_OBJ_NO_ACCESS_RIGHTS           (K_OBJ_ERRNO_BASE | 6)
#define K_OBJ_ERRNO_OBJ_NAME_TRUNCATED             (K_OBJ_ERRNO_BASE | 7)
#define K_OBJ_ERRNO_OBJ_NAME_CLASH                 (K_OBJ_ERRNO_BASE | 8)
#define K_OBJ_ERRNO_OBJ_NOT_NAMED                  (K_OBJ_ERRNO_BASE | 9)
#define K_OBJ_ERRNO_OBJ_ILLEGAL_CLASS_TYPE         (K_OBJ_ERRNO_BASE | 10)
#define K_OBJ_ERRNO_OBJ_OPERATION_UNSUPPORTED      (K_OBJ_ERRNO_BASE | 11)
#define K_OBJ_ERRNO_OBJ_INVALID_OWNER              (K_OBJ_ERRNO_BASE | 12)
#define K_OBJ_ERRNO_OBJ_RENAME_NOT_ALLOWED         (K_OBJ_ERRNO_BASE | 13)
#define K_OBJ_ERRNO_OBJ_DESTROY_ERROR              (K_OBJ_ERRNO_BASE | 14)
#define K_OBJ_ERRNO_OBJ_HANDLE_TBL_FULL            (K_OBJ_ERRNO_BASE | 15)
#define K_OBJ_ERRNO_OBJ_NOT_FOUND                  (K_OBJ_ERRNO_BASE | 16)
#define K_OBJ_ERRNO_OBJ_INVALID_ARGUMENT           (K_OBJ_ERRNO_BASE | 17)
#define K_OBJ_ERRNO_OBJ_LIB_NOT_INITIALIZED        (K_OBJ_ERRNO_BASE | 18)
#define K_OBJ_ERRNO_ACCESS_DENIED                  (K_OBJ_ERRNO_BASE | 19)
#define K_OBJ_ERRNO_NO_PROC                        (K_OBJ_ERRNO_BASE | 20)

/* Object creation mode options */
#define OM_CREATE               0x10000000
#define OM_EXCL                 0x20000000
#define OM_DELETE_ON_LAST_CLOSE 0x40000000

#define INT_RESTRICT()      (OK)

/* todo: lock CPU preemption for calling task */
#define TASK_LOCK()
#define TASK_UNLOCK()

/* todo: prevent task deletion */
#define TASK_SAFE()
#define  TASK_UNSAFE()

#define OBJ_SET_DEALLOC_MEM(obj_id)                                           \
    (                                                                         \
        ((obj_core_t *)(obj_id))->attributes |= OBJ_DEALLOC_MEM        \
    )

#define OBJ_RECLAIM_DISABLE(obj_id)                                           \
    (                                                                         \
        ((obj_core_t *)(obj_id))->attributes |= OBJ_NO_RECLAIM         \
    )

#define OBJ_IS_RECLAIM_DISABLE(obj_id)                                        \
    (                                                                         \
        (((obj_core_t *)(obj_id))->attributes & OBJ_NO_RECLAIM) != 0   \
    )

#define OBJ_INSTRUMENTATION_ENABLE(obj_id)                                    \
    (                                                                         \
        ((obj_core_t *)(obj_id))->attributes |= OBJ_INSTRUMENTED       \
    )

#define OBJ_INSTRUMENTATION_DISABLE(obj_id)                        \
    (                                                              \
        ((obj_core_t *)(obj_id))->attributes =              \
        (uint16_t)((((obj_core_t *)(obj_id))->attributes) & \
        (~OBJ_INSTRUMENTED))                                       \
    )


#define OBJ_STACKTRACE_ENABLE(obj_id)                                    \
    (                                                                    \
        ((obj_core_t *)(obj_id))->attributes |= OBJ_STACKTRACE    \
    )

#define OBJ_STACKTRACE_DISABLE(obj_id)                                \
    (                                                                 \
        ((obj_core_t *)(obj_id))->attributes =                 \
        (uint16_t)((((obj_core_t *)(obj_id))->attributes) &    \
        (~_OBJ_STACKTRACE))                                           \
    )

#define OBJ_IS_OWNER(obj_id)                                  \
    (                                                         \
        &((obj_core_t *)(obj_id))->owner_list != NULL         \
    )

#define OBJ_IS_REFERENCED(obj_id)                                   \
    (                                                               \
        &((obj_core_t *)(obj_id))->obj_handle_list != NULL          \
    )

#define OBJ_IS_PUBLIC(obj_id)                                              \
    (                                                                      \
        (((obj_core_t *)(obj_id))->attributes & OBJ_PUBLIC) != 0    \
    )


#define OBJ_SET_PUBLIC(obj_id)                                          \
    (                                                                   \
        ((obj_core_t *)(obj_id))->attributes |= OBJ_PUBLIC       \
    )

#define OBJ_IS_NAMED(obj_id)                                                \
    (                                                                       \
        (((obj_core_t *)(obj_id))->attributes & OBJ_NAMED) != 0      \
    )

#define OBJ_SET_NAMED(obj_id)                                             \
    (                                                                     \
        ((obj_core_t *)(obj_id))->attributes |= OBJ_NAMED          \
    )

#define OBJ_IS_DELETE_ON_LAST_CLOSE(obj_id)                                \
    (                                                                      \
        (((obj_core_t *)(obj_id))->attributes &                     \
        OBJ_DELETE_ON_LAST_CLOSE) != 0                                     \
    )

#define OBJ_SET_DELETE_ON_LAST_CLOSE(obj_id)                               \
    (                                                                      \
        ((obj_core_t *)(obj_id))->attributes |=                     \
        OBJ_DELETE_ON_LAST_CLOSE                                           \
    )

#define OBJ_IS_DELETED(obj_id)                                             \
    (                                                                      \
        (((obj_core_t *)(obj_id))->attributes & OBJ_DELETED) != 0   \
    )


#define OBJ_SET_DELETED(obj_id)                                           \
    do  {                                                                 \
        ((obj_core_t *)(obj_id))->attributes |= OBJ_DELETED;       \
        OBJ_ACCESS_SET_DELETED((obj_core_t *)(obj_id));                   \
    } while ((0))

#ifndef ERROR
#define ERROR                                       -1
#endif

#ifndef OK
#define OK                                          0
#endif

typedef void *(*routine_func_t)(void *);

//#define  obj_errno  errno

extern int obj_errno;
extern int obj_errno_get(void);

struct obj_class;
struct handle;
struct task_group;

typedef int                   obj_handle_t;
typedef  struct task_group    task_group_t;
extern struct obj_class        *proc_class_id;

#ifdef __cplusplus
}

#endif

#endif /* K_OBJ_COMMON_H */
