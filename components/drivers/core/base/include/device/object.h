/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#ifndef _AOS_OBJECT_H
#define _AOS_OBJECT_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#include "aos/list.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef AOS_OBJ_NAME_MAX
#define AOS_OBJ_NAME_MAX 10
#endif

#ifndef AOS_ALIGN
#define AOS_ALIGN(size, align)           (((size) + (align) - 1) & ~((align) - 1))
#endif

#ifndef AOS_ALIGN_SIZE
#define AOS_ALIGN_SIZE  4
#endif

enum aos_object_class_type
{
    AOS_Object_Class_Null   = 0,                         /**< The object is not used. */
    AOS_Object_Class_Thread,                             /**< The object is a thread. */
    AOS_Object_Class_Semaphore,                          /**< The object is a semaphore. */
    AOS_Object_Class_Mutex,                              /**< The object is a mutex. */
    AOS_Object_Class_Event,                              /**< The object is a event. */
    AOS_Object_Class_MailBox,                            /**< The object is a mail box. */
    AOS_Object_Class_MessageQueue,                       /**< The object is a message queue. */
    AOS_Object_Class_MemHeap,                            /**< The object is a memory heap */
    AOS_Object_Class_MemPool,                            /**< The object is a memory pool. */
    AOS_Object_Class_Device,                             /**< The object is a device */
    AOS_Object_Class_Timer,                              /**< The object is a timer. */
    AOS_Object_Class_Module,                             /**< The object is a module. */
    AOS_Object_Class_Unknown,                            /**< The object is unknown. */
    AOS_Object_Class_Static = 0x80                       /**< The object is a static object. */
};

struct aos_object
{
    char    name[AOS_OBJ_NAME_MAX];                  /**< name of kernel object */
    uint8_t type;                                    /**< type of kernel object */
    uint8_t flag;                                    /**< flag of kernel object */

#ifdef AOS_USING_MODULE
    void      *module_id;                               /**< id of application module */
#endif
    dlist_t  list;                                    /**< list node of kernel object */
};

typedef struct aos_object *aos_object_t;

/**
 * The information of the kernel object
 */
struct aos_object_information
{
    enum aos_object_class_type  type;                     /**< object class type */
    dlist_t                     object_list;              /**< object list */
    size_t                      object_size;              /**< object size */
};


void aos_object_init(struct aos_object      *object,
                     enum aos_object_class_type type,
                     const char               *name);

struct aos_object_information *
aos_object_get_information(enum aos_object_class_type type);

#ifdef AOS_USING_MODULE
struct aos_dlmodule {
    /* modeule definition */
};
#endif

#ifdef __cplusplus
}
#endif


#endif
