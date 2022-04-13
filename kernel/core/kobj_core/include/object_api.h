/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#ifndef K_OBJECT_API_H
#define K_OBJECT_API_H

#include "object_common.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef void *(*obj_create_func_t)(const char *name, va_list vaList);
typedef int (*obj_destroy_func_t)(void *obj_id, RHINO_BOOL dealloc);
typedef int (*obj_show_func_t)(void *obj_id, int show_type);
typedef void *(*obj_alloc_func_t)(void *mem_pool, size_t nbytes, size_t alignment);
typedef int (*obj_free_func_t)(void *mem_pool, void *obj);

enum OBJ_CLASS_TYPE {
    INVALID_CLASS_TYPE  = 0,    /* invalid class type class     */
    SEM_CLASS_TYPE,             /* aos semaphore                */
    EVENT_CLASS_TYPE,           /* aos event                    */
    MUTEX_CLASS_TYPE,           /* aos mutex                    */
    QUEUE_CLASS_TYPE,           /* aos message queue            */
    WORKQUEUE_CLASS_TYPE,       /* aos workqueue                */
    WORK_CLASS_TYPE,            /* aos workqueue-work           */
    PROC_CLASS_TYPE,            /* aos taskgroup                */
    TIMER_CLASS_TYPE,           /* aos timer                    */
    TASK_CLASS_TYPE,            /* task                         */
    FD_CLASS_TYPE,              /* file descriptor              */
    VM_CONTEXT_CLASS_TYPE,      /* virtual memory context       */
    MEM_PART_CLASS_TYPE,        /* memory partition             */
    TEST_CLASS_TYPE,            /* only for test                */
    NUM_OBJ_CLASS_TYPE          /* object count                 */
};


typedef struct obj_class {
    unsigned long        magic;
    enum OBJ_CLASS_TYPE  class_type;          /* class type              */
    int                  options;             /* object class options    */
    size_t               obj_size;            /* size of object          */
    obj_create_func_t    create_rtn;          /* object create routine   */
    obj_destroy_func_t   destroy_rtn;         /* object destroy          */
    obj_show_func_t      show_rtn;            /* object show routine     */
    obj_alloc_func_t     obj_mem_alloc_rtn;   /* object memory alloc     */
    obj_free_func_t      obj_mem_free_rtn;    /* object memory free      */
    void                *obj_part_id;         /* object memory pool ID   */
    klist_t              obj_priv_list;       /* list of private objects */
    klist_t              obj_pub_list;        /* list of public objects  */
    kmutex_t            *class_sem;          /* lists' mutual exclusion */
} obj_class_t;                                /* sizeof obj_class_t      */


typedef struct obj_core {
    unsigned long       magic;
    uint16_t            attributes;         /* attribute bit set */
    klist_t             owner_list;         /* list of owned objects        */
    klist_t             owner_node;         /* owner list node              */
    klist_t             class_node;         /* class list node              */
    struct obj_core    *owner_id;           /* pointer to obj owner         */
    struct task_group  *owner_proc_id;      /* pointer to obj process owner */
    char               *name;               /* object name                  */
    struct obj_class   *obj_class;          /* pointer to obj class         */
    klist_t             obj_handle_list;    /* list of handles              */
    uint16_t            ref_cnt;            /* reference counter            */
    uint16_t            access_cnt;         /* access counter               */
} obj_core_t;                               /* sizeof obj_core_t            */

struct handle_table;
typedef struct master_handle_table {
    struct handle_table *handle_table [OBJ_MASTER_TBL_SIZE];
    int               first_free; /* index of most recent free slot */
    int               size;       /* size of all handle tables combined */
    uint32_t          init_index;
    uint32_t          gen_number;
    kmutex_t          hdl_tbl_sem_id;
} master_handle_table_t;

/* for class implementation */
int class_init(obj_class_t *obj_class,
               size_t obj_size,
               obj_create_func_t create_rtn,
               obj_destroy_func_t destroy_rtn,
               obj_alloc_func_t obj_alloc_rtn,
               obj_free_func_t obj_free_rtn,
               void *obj_mem_pool_id,
               enum OBJ_CLASS_TYPE class_type,
               int options);

/* for object implementation */
void *obj_alloc(obj_class_t *class_id);
void *obj_alloc_extra(obj_class_t *class_id, size_t extra_bytes_num, void **extra);
int  obj_free(obj_class_t *class_id, obj_core_t *object_id);
void obj_core_init(obj_core_t *obj_core, obj_class_t *obj_class);
int  obj_core_deinit(obj_core_t *obj_core);
void *obj_open(const char *name, int mode, obj_class_t *class_id, ...);
int  obj_close(void *obj_id, obj_class_t *class_id);
int  obj_unlink(const char *name, obj_class_t *class_id);
char *obj_name_ptr_get(void *obj_id);
int  obj_name_ptr_set(void *obj_id, char *name);
int  obj_name_set(void *obj_id, const char *name);
int  obj_name_get(void *obj_id, char *name_buf, size_t buf_size);
ssize_t obj_name_len_get(void *obj_id);
enum OBJ_CLASS_TYPE obj_class_type_get(void *obj_id);
void *obj_name_to_id(enum OBJ_CLASS_TYPE class_type, const char *name);
void *obj_owner_get(void *obj_id);
int  obj_owner_set(void *obj_id, void *owner_id);
int  obj_verify(void *obj_id, void *class_id);
void obj_core_invalidate(obj_core_t *obj_core);
int  obj_core_release(void *obj_id);
/* for syscall, handle <-> object */
obj_handle_t obj_handle_alloc(void *obj_id, void *proc_id);
void *obj_handle_to_obj_id(obj_handle_t handle, void *proc_id,
                           RHINO_BOOL access);
int  obj_handle_close(obj_handle_t handle, void *proc_id);
int  obj_handle_delete(obj_handle_t handle, int options);
void *get_task_group(void);
/* for initialization */
int  obj_init_all(void);
int  obj_handle_tbl_create(void *proc_id);
void obj_handle_tbl_terminate(void *proc_id);

#ifdef __cplusplus
}
#endif

#endif /* K_OBJECT_API_H */

