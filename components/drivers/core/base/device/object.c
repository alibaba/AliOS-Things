/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */


#include "device/object.h"
#include "device/aos_device.h"

#include "aos/kernel.h"
#include "k_api.h"

#define OBJ_ASSERT(test)
#define AOS_USING_DEVICE

#ifdef AOS_USING_HOOK
static void (*aos_object_attach_hook)(struct aos_object *object);
static void (*aos_object_detach_hook)(struct aos_object *object);
#endif

/*
 * define object_info for the number of aos_object_container items.
 */
enum aos_object_info_type
{
    AOS_Object_Info_Thread = 0,                         /**< The object is a thread. */
#ifdef AOS_USING_SEMAPHORE
    AOS_Object_Info_Semaphore,                          /**< The object is a semaphore. */
#endif
#ifdef AOS_USING_MUTEX
    AOS_Object_Info_Mutex,                              /**< The object is a mutex. */
#endif
#ifdef AOS_USING_EVENT
    AOS_Object_Info_Event,                              /**< The object is a event. */
#endif
#ifdef AOS_USING_MAILBOX
    AOS_Object_Info_MailBox,                            /**< The object is a mail box. */
#endif
#ifdef AOS_USING_MESSAGEQUEUE
    AOS_Object_Info_MessageQueue,                       /**< The object is a message queue. */
#endif
#ifdef AOS_USING_MEMHEAP
    AOS_Object_Info_MemHeap,                            /**< The object is a memory heap */
#endif
#ifdef AOS_USING_MEMPOOL
    AOS_Object_Info_MemPool,                            /**< The object is a memory pool. */
#endif
#ifdef AOS_USING_DEVICE
    AOS_Object_Info_Device,                             /**< The object is a device */
#endif
    AOS_Object_Info_Timer,                              /**< The object is a timer. */
#ifdef AOS_USING_MODULE
    AOS_Object_Info_Module,                             /**< The object is a module. */
#endif
    AOS_Object_Info_Unknown,                            /**< The object is unknown. */
};

#define _OBJ_CONTAINER_LIST_INIT(c)     \
    {&(aos_object_container[c].object_list), &(aos_object_container[c].object_list)}
static struct aos_object_information aos_object_container[AOS_Object_Info_Unknown] =
{
    /* initialize object container - thread */
    {AOS_Object_Class_Thread, _OBJ_CONTAINER_LIST_INIT(AOS_Object_Info_Thread), sizeof(aos_task_t)},
#ifdef AOS_USING_SEMAPHORE
    /* initialize object container - semaphore */
    {AOS_Object_Class_Semaphore, _OBJ_CONTAINER_LIST_INIT(AOS_Object_Info_Semaphore), sizeof(aos_semt_t)},
#endif
#ifdef AOS_USING_MUTEX
    /* initialize object container - mutex */
    {AOS_Object_Class_Mutex, _OBJ_CONTAINER_LIST_INIT(AOS_Object_Info_Mutex), sizeof(aos_mutex_t)},
#endif
#ifdef AOS_USING_EVENT
    /* initialize object container - event */
    {AOS_Object_Class_Event, _OBJ_CONTAINER_LIST_INIT(AOS_Object_Info_Event), sizeof(aos_event_t)},
#endif
#ifdef AOS_USING_MAILBOX
    /* initialize object container - mailbox */
    {AOS_Object_Class_MailBox, _OBJ_CONTAINER_LIST_INIT(AOS_Object_Info_MailBox), sizeof(aos_queue_t)},
#endif
#ifdef AOS_USING_MESSAGEQUEUE
    /* initialize object container - message queue */
    {AOS_Object_Class_MessageQueue, _OBJ_CONTAINER_LIST_INIT(AOS_Object_Info_MessageQueue), sizeof(struct aos_messagequeue)},
#endif
#ifdef AOS_USING_MEMHEAP
    /* initialize object container - memory heap */
    {AOS_Object_Class_MemHeap, _OBJ_CONTAINER_LIST_INIT(AOS_Object_Info_MemHeap), sizeof(struct memheap)},
#endif
#ifdef AOS_USING_MEMPOOL
    /* initialize object container - memory pool */
    {AOS_Object_Class_MemPool, _OBJ_CONTAINER_LIST_INIT(AOS_Object_Info_MemPool), sizeof(struct aos_mempool)},
#endif
#ifdef AOS_USING_DEVICE
    /* initialize object container - device */
    {AOS_Object_Class_Device, _OBJ_CONTAINER_LIST_INIT(AOS_Object_Info_Device), sizeof(struct aos_device)},
#endif
    /* initialize object container - timer */
    {AOS_Object_Class_Timer, _OBJ_CONTAINER_LIST_INIT(AOS_Object_Info_Timer), sizeof(aos_timer_t)},
#ifdef AOS_USING_MODULE
    /* initialize object container - module */
    {AOS_Object_Class_Module, _OBJ_CONTAINER_LIST_INIT(AOS_Object_Info_Module), sizeof(struct aos_dlmodule)},
#endif
};

#ifdef AOS_USING_HOOK
static void (*aos_object_attach_hook)(struct aos_object *object);
static void (*aos_object_detach_hook)(struct aos_object *object);
void (*aos_object_trytake_hook)(struct aos_object *object);
void (*aos_object_take_hook)(struct aos_object *object);
void (*aos_object_put_hook)(struct aos_object *object);

/**
 * @addtogroup Hook
 */

/**@{*/

/**
 * This function will set a hook function, which will be invoked when object
 * attaches to kernel object system.
 *
 * @param hook the hook function
 */
void aos_object_attach_sethook(void (*hook)(struct aos_object *object))
{
    aos_object_attach_hook = hook;
}

/**
 * This function will set a hook function, which will be invoked when object
 * detaches from kernel object system.
 *
 * @param hook the hook function
 */
void aos_object_detach_sethook(void (*hook)(struct aos_object *object))
{
    aos_object_detach_hook = hook;
}

/**
 * This function will set a hook function, which will be invoked when object
 * is taken from kernel object system.
 *
 * The object is taken means:
 * semaphore - semaphore is taken by thread
 * mutex - mutex is taken by thread
 * event - event is received by thread
 * mailbox - mail is received by thread
 * message queue - message is received by thread
 *
 * @param hook the hook function
 */
void aos_object_trytake_sethook(void (*hook)(struct aos_object *object))
{
    aos_object_trytake_hook = hook;
}

/**
 * This function will set a hook function, which will be invoked when object
 * have been taken from kernel object system.
 *
 * The object have been taken means:
 * semaphore - semaphore have been taken by thread
 * mutex - mutex have been taken by thread
 * event - event have been received by thread
 * mailbox - mail have been received by thread
 * message queue - message have been received by thread
 * timer - timer is started
 *
 * @param hook the hook function
 */
void aos_object_take_sethook(void (*hook)(struct aos_object *object))
{
    aos_object_take_hook = hook;
}

/**
 * This function will set a hook function, which will be invoked when object
 * is put to kernel object system.
 *
 * @param hook the hook function
 */
void aos_object_put_sethook(void (*hook)(struct aos_object *object))
{
    aos_object_put_hook = hook;
}

/**@}*/
#endif

/**
 * @ingroup SystemInit
 *
 * This function will initialize system object management.
 *
 * @deprecated since 0.3.0, this function does not need to be invoked
 * in the system initialization.
 */
void aos_system_object_init(void)
{
}

/**
 * @addtogroup KernelObject
 */

/**@{*/

/**
 * This function will return the specified type of object information.
 *
 * @param type the type of object
 * @return the object type information or NULL
 */
struct aos_object_information *
aos_object_get_information(enum aos_object_class_type type)
{
    int index;

    for (index = 0; index < AOS_Object_Info_Unknown; index ++)
        if (aos_object_container[index].type == type) return &aos_object_container[index];

    return NULL;
}


/**
 * This function will initialize an object and add it to object system
 * management.
 *
 * @param object the specified object to be initialized.
 * @param type the object type.
 * @param name the object name. In system, the object's name must be unique.
 */
void aos_object_init(struct aos_object         *object,
                    enum aos_object_class_type type,
                    const char               *name)
{
    struct dlist_s *node = NULL;
    struct aos_object_information *information;
#ifdef AOS_USING_MODULE
    struct aos_dlmodule *module = dlmodule_self();
#endif

    CPSR_ALLOC();

    /* get object information */
    information = aos_object_get_information(type);
    OBJ_ASSERT(information != NULL);

    /* check object type to avoid re-initialization */

    RHINO_CRITICAL_ENTER();

    /* try to find object */
    for (node  = information->object_list.next;
            node != &(information->object_list);
            node  = node->next)
    {
        struct aos_object *obj;

        obj = dlist_entry(node, struct aos_object, list);
        if (obj) /* skip warning when disable debug */
        {
            OBJ_ASSERT(obj != object);
        }
    }
    /* leave critical */
    RHINO_CRITICAL_EXIT();

    /* initialize object's parameters */
    /* set object type to static */
    object->type = type | AOS_Object_Class_Static;
    /* copy name */
    strncpy(object->name, name, AOS_OBJ_NAME_MAX);

#ifdef AOS_USING_HOOK
    AOS_OBJECT_HOOK_CALL(aos_object_attach_hook, (object));
#endif

    /* lock interrupt */
    RHINO_CRITICAL_ENTER();

#ifdef AOS_USING_MODULE
    if (module)
    {
        dlist_add_tail(&(module->object_list), &(object->list));
        object->module_id = (void *)module;
    }
    else
#endif
    {
        /* insert object into information object list */
        dlist_add_tail( &(object->list),&(information->object_list));
    }

    /* unlock interrupt */
    RHINO_CRITICAL_EXIT();
}

/**
 * This function will detach a static object from object system,
 * and the memory of static object is not freed.
 *
 * @param object the specified object to be detached.
 */
void aos_object_detach(aos_object_t object)
{
    /* object check */
    OBJ_ASSERT(object != NULL);

#ifdef AOS_USING_HOOK
    AOS_OBJECT_HOOK_CALL(aos_object_detach_hook, (object));
#endif

    CPSR_ALLOC();

    /* reset object type */
    object->type = 0;

    /* enter critical */
    RHINO_CRITICAL_ENTER();

    /* remove from old list */
    dlist_del(&(object->list));

    RHINO_CRITICAL_EXIT();
}

/**
 * This function will judge the object is system object or not.
 * Normally, the system object is a static object and the type
 * of object set to RT_Object_Class_Static.
 *
 * @param object the specified object to be judged.
 *
 * @return RT_TRUE if a system object, RT_FALSE for others.
 */
bool aos_object_is_systemobject(aos_object_t object)
{
    /* object check */
    OBJ_ASSERT(object != NULL);

    if (object->type & AOS_Object_Class_Static)
        return true;

    return false;
}

/**
 * This function will return the type of object without
 * RT_Object_Class_Static flag.
 *
 * @param object the specified object to be get type.
 *
 * @return the type of object.
 */
uint8_t aos_object_get_type(aos_object_t object)
{
    /* object check */
    OBJ_ASSERT(object != NULL);

    return object->type & ~AOS_Object_Class_Static;
}

/**
 * This function will find specified name object from object
 * container.
 *
 * @param name the specified name of object.
 * @param type the type of object
 *
 * @return the found object or RT_NULL if there is no this object
 * in object container.
 *
 * @note this function shall not be invoked in interrupt status.
 */
aos_object_t aos_object_find(const char *name, uint8_t type)
{
    aos_object_t object = NULL;
    struct dlist_s *node = NULL;
    struct aos_object_information *information = NULL;

    CPSR_ALLOC();

    /* parameter check */
    if ((name == NULL) || (type > AOS_Object_Class_Unknown))
        return NULL;

    /* enter critical */
    RHINO_CRITICAL_ENTER();

    /* try to find object */
    if (information == NULL)
    {
        information = aos_object_get_information((enum aos_object_class_type)type);
        OBJ_ASSERT(information != NULL);
    }
    for (node  = information->object_list.next;
            node != &(information->object_list);
            node  = node->next)
    {
        object = dlist_entry(node, struct aos_object, list);
        if (strncmp(object->name, name, AOS_OBJ_NAME_MAX) == 0)
        {
            /* leave critical */
            RHINO_CRITICAL_EXIT();

            return object;
        }
    }

    /* leave critical */
    RHINO_CRITICAL_EXIT();

    return NULL;
}

/**@}*/
