/*
 * Copyright (C) 2015-2021 Alibaba Group Holding Limited
 */

#include <errno.h>
#include <string.h>
#include <sched.h>

#include "internal/pthread.h"

int pthread_attr_init(pthread_attr_t *attr)
{
    if (attr == NULL) {
        return EINVAL;
    }

    memset(attr, 0, sizeof(pthread_attr_t));
    attr->stacksize                 = PTHREAD_DEFAULT_STACK_SIZE;
    attr->sched_priority            = PTHREAD_DEFAULT_PRIORITY;
    attr->sched_slice               = PTHREAD_DEFAULT_SLICE;
    attr->detachstate               = PTHREAD_CREATE_JOINABLE;
    attr->contentionscope           = PTHREAD_SCOPE_SYSTEM;
    attr->inheritsched              = PTHREAD_EXPLICIT_SCHED;
    attr->guardsize                 = PTHREAD_DEFAULT_GUARD_SIZE;
    attr->stackaddr                 = NULL;
    attr->flag                      = PTHREAD_DYN_INIT;

    return 0;
}

int pthread_attr_destroy(pthread_attr_t *attr)
{
    if (attr == NULL) {
        return EINVAL;
    }

    memset(attr, 0, sizeof(pthread_attr_t));

    return 0;
}

int pthread_attr_setdetachstate(pthread_attr_t *attr, int detachstate)
{
    if ((attr == NULL) || ((detachstate != PTHREAD_CREATE_DETACHED) &&
                            (detachstate != PTHREAD_CREATE_JOINABLE))) {
        return EINVAL;
    }

    attr->detachstate = detachstate;

    return 0;
}

int pthread_attr_getdetachstate(const pthread_attr_t *attr, int *detachstate)
{
    if ((attr == NULL) || (detachstate == NULL)) {
        return EINVAL;
    }

    *detachstate = attr->detachstate;

    return 0;
}

int pthread_attr_setschedpolicy(pthread_attr_t *attr, int policy)
{
    if ((attr == NULL) || ((policy < SCHED_OTHER) || (policy > SCHED_RR))) {
        return EINVAL;
    }

    attr->schedpolicy = policy;

    return 0;
}

int pthread_attr_getschedpolicy(const pthread_attr_t *attr, int *policy)
{
    if ((attr == NULL) || (policy == NULL)) {
        return EINVAL;
    }

    *policy = attr->schedpolicy;

    return 0;
}

int pthread_attr_setschedparam(pthread_attr_t *attr, const struct sched_param *param)
{
    if ((attr == NULL) || (param == NULL)) {
        return EINVAL;
    }

    attr->sched_priority = param->sched_priority;
    attr->sched_slice = param->slice;
    return 0;
}

int pthread_attr_getschedparam(const pthread_attr_t *attr, struct sched_param *param)
{
    if ((attr == NULL) || (param == NULL)) {
        return EINVAL;
    }

    param->sched_priority = attr->sched_priority;
    param->slice = attr->sched_slice;

    return 0;
}

int pthread_attr_setstacksize(pthread_attr_t *attr, size_t stacksize)
{
    if ((attr == NULL) || (stacksize <= 0)) {
        return EINVAL;
    }

    attr->stacksize = stacksize;

    return 0;
}

int pthread_attr_getstacksize(const pthread_attr_t *attr, size_t *stacksize)
{
    if ((attr == NULL) || (stacksize == NULL)) {
        return EINVAL;
    }

    *stacksize = attr->stacksize;

    return 0;
}

int pthread_attr_setstackaddr(pthread_attr_t *attr, void *stackaddr)
{
    if ((attr == NULL) || (stackaddr == NULL)) {
        return EINVAL;
    }

    attr->stackaddr = stackaddr;

    return 0;
}

int pthread_attr_getstackaddr(const pthread_attr_t *attr, void **stackaddr)
{
    if ((attr == NULL) || (stackaddr == NULL)) {
        return EINVAL;
    }

    *stackaddr = attr->stackaddr;

    return 0;
}

int pthread_attr_setstack(pthread_attr_t *attr, void *stackaddr, size_t stacksize)
{
    if ((attr == NULL) || (stackaddr == NULL) || (stacksize <= 0)) {
        return EINVAL;
    }

    attr->stackaddr = stackaddr;
    attr->stacksize = stacksize;

    return 0;
}

int pthread_attr_getstack(const pthread_attr_t *attr, void **stackaddr, size_t *stacksize)
{
    if ((attr == NULL) || (stackaddr == NULL) || (stacksize == NULL)) {
        return EINVAL;
    }

    *stackaddr = attr->stackaddr;
    *stacksize = attr->stacksize;

    return 0;
}

int pthread_attr_getinheritsched(const pthread_attr_t *restrict attr, int *restrict inheritsched)
{
    if ((attr == NULL) || (inheritsched == NULL)) {
        return EINVAL;
    }

    *inheritsched = attr->inheritsched;

    return 0;
}

int pthread_attr_setinheritsched(pthread_attr_t *attr, int inheritsched)
{
    if ((attr == NULL) || (inheritsched < PTHREAD_INHERIT_SCHED) ||
                          (inheritsched > PTHREAD_EXPLICIT_SCHED)) {
        return EINVAL;
    }

    attr->inheritsched = inheritsched;

    return 0;
}

int pthread_attr_setguardsize(pthread_attr_t *attr, size_t guardsize)
{
    /* guardsize stack protection is not supported by kernel */
    return ENOSYS;
}

int pthread_attr_getguardsize(const pthread_attr_t *attr, size_t *guardsize)
{
    /* guardsize stack protection is not supported by kernel */
    return ENOSYS;
}

int pthread_attr_getscope(const pthread_attr_t *restrict attr, int *restrict contentionscope)
{
    if ((attr == NULL) || (contentionscope == NULL)) {
        return EINVAL;
    }

    *contentionscope = attr->contentionscope;

    return 0;
}

int pthread_attr_setscope(pthread_attr_t *attr, int contentionscope)
{
    if ((attr == NULL) || (contentionscope != PTHREAD_SCOPE_PROCESS) &&
                          (contentionscope != PTHREAD_SCOPE_SYSTEM)) {
        return EINVAL;
    }

    attr->contentionscope = contentionscope;

    return 0;
}
