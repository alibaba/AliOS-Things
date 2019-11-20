/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include "posix/pthread.h"

#if (POSIX_CONFIG_PTHREAD_ENABLE > 0)

int pthread_attr_init(pthread_attr_t *attr)
{
    if (attr == NULL) {
        return -1;
    }

    attr->stacksize                 = DEFAULT_THREAD_STACK_SIZE;
    attr->schedparam.sched_priority = DEFAULT_THREAD_PRIORITY;
    attr->schedparam.slice          = DEFAULT_THREAD_SLICE;
    attr->detachstate               = PTHREAD_CREATE_JOINABLE;
    attr->contentionscope           = PTHREAD_SCOPE_SYSTEM;
    attr->inheritsched              = PTHREAD_EXPLICIT_SCHED;
    attr->guardsize                 = 0;
    attr->stackaddr                 = NULL;
    attr->is_initialized            = 1;

    return 0;
}

int pthread_attr_destroy(pthread_attr_t *attr)
{
    memset(attr, 0, sizeof(pthread_attr_t));

    return 0;
}

int pthread_attr_setdetachstate(pthread_attr_t *attr, int detachstate)
{
    if (attr == NULL) {
        return -1;
    }

    attr->detachstate = (unsigned char)detachstate;

    return 0;
}

int pthread_attr_getdetachstate(const pthread_attr_t *attr, int *detachstate)
{
    if ((attr == NULL)||(detachstate == NULL)) {
        return -1;
    }

    *detachstate = (int)attr->detachstate;

    return 0;
}

int pthread_attr_setschedpolicy(pthread_attr_t *attr, int policy)
{
    if (attr == NULL) {
        return -1;
    }

    attr->schedpolicy = (unsigned char)policy;

    return 0;
}

int pthread_attr_getschedpolicy(pthread_attr_t const *attr, int *policy)
{
    if ((attr == NULL)||(policy == NULL)) {
        return -1;
    }

    *policy = (int)attr->schedpolicy;

    return 0;
}

int pthread_attr_setschedparam(pthread_attr_t *attr, const struct sched_param *param)
{
    if ((attr == NULL)||(param == NULL)) {
        return -1;
    }

    attr->schedparam.sched_priority = (int)param->sched_priority;
    attr->schedparam.slice = (size_t)param->slice;

    return 0;
}

int pthread_attr_getschedparam(const pthread_attr_t *attr, struct sched_param *param)

{
    if ((attr == NULL)||(param == NULL)) {
        return -1;
    }

    param->sched_priority = attr->schedparam.sched_priority;
    param->slice = attr->schedparam.slice;

    return 0;
}

int pthread_attr_setstacksize(pthread_attr_t *attr, size_t stacksize)
{
    if (attr == NULL) {
        return -1;
    }

    attr->stacksize = stacksize;

    return 0;
}

int pthread_attr_getstacksize(const pthread_attr_t *attr, size_t *stacksize)
{
    if ((attr == NULL)||(stacksize == NULL)) {
        return -1;
    }

    *stacksize = attr->stacksize;

    return 0;
}

int pthread_attr_setstackaddr(pthread_attr_t *attr, void *stackaddr)
{
    if ((attr == NULL)||(stackaddr == NULL)) {
        return -1;
    }

    attr->stackaddr = stackaddr;

    return 0;
}

int pthread_attr_getstackaddr(const pthread_attr_t *attr, void **stackaddr)
{
    if ((attr == NULL)||(stackaddr == NULL)) {
        return -1;
    }

    *stackaddr = attr->stackaddr;

    return 0;
}

int pthread_attr_setstack(pthread_attr_t *attr, void *stackaddr, size_t stacksize)
{
    if ((attr == NULL)||(stackaddr == NULL)) {
        return -1;
    }

    attr->stackaddr = stackaddr;
    attr->stacksize = stacksize;

    return 0;
}

int pthread_attr_getstack(const pthread_attr_t *attr, void **stackaddr, size_t *stacksize)
{
    if ((attr == NULL)||(stackaddr == NULL)) {
        return -1;
    }

    *stackaddr = attr->stackaddr;
    *stacksize = attr->stacksize;

    return 0;
}

int pthread_attr_getinheritsched(const pthread_attr_t *restrict attr, int *restrict inheritsched)
{
    if ((attr == NULL)||(inheritsched == NULL)) {
        return -1;
    }

    *inheritsched = (int)attr->inheritsched;

    return 0;
}

int pthread_attr_setinheritsched(pthread_attr_t *attr, int inheritsched)
{
    if ((attr == NULL)||(inheritsched == NULL)) {
        return -1;
    }

    attr->inheritsched = (unsigned char)inheritsched;

    return 0;
}

int pthread_attr_setguardsize(pthread_attr_t *attr, size_t guardsize)
{
    /* guardsize stack protection is not supported by kernel */
    return -1;
}

int pthread_attr_getguardsize(const pthread_attr_t *attr, size_t *guardsize)
{
    /* guardsize stack protection is not supported by kernel */
    return -1;
}

int pthread_attr_getscope(const pthread_attr_t *restrict attr, int *restrict contentionscope)
{
    if ((attr == NULL)||(contentionscope == NULL)) {
        return -1;
    }

    *contentionscope = (int)attr->contentionscope;

    return 0;
}

int pthread_attr_setscope(pthread_attr_t *attr, int contentionscope)
{
    if (attr == NULL) {
        return -1;
    }

    /* since there is no implementation of process, the scope must
       be PTHREAD_SCOPE_SYSTEM */
    if (contentionscope == PTHREAD_SCOPE_SYSTEM) {
        attr->contentionscope = (unsigned char)PTHREAD_SCOPE_SYSTEM;
    } else {
        return -1;
    }

    return 0;
}

#endif
