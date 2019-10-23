/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#include "k_config.h"
#include "task_group.h"

typedef struct {
    klist_t node;
    int     fd;
} socketfd_t;

extern int socketfd_close(int fd);

static int socketfd_close_all(int pid)
{
    task_group_t *group;
    klist_t      *head;
    klist_t      *iter;
    socketfd_t  *fd_obj;

    group = task_group_get_by_pid(pid);
    if (NULL == group) {
        return -1;
    }

    krhino_mutex_lock(&group->socket_mutex, RHINO_WAIT_FOREVER);

    head = &group->socket_fd_list;
    if (!is_klist_empty(head)) {
        iter = head->next;
        while (iter != head) {
            fd_obj = krhino_list_entry(iter, socketfd_t, node);
            iter = iter->next;
            klist_rm(&fd_obj->node);
            socket_close(fd_obj->fd);
            free(fd_obj);
        }
    }

    krhino_mutex_unlock(&group->socket_mutex);

    return 0;
}

int socketfd_deref(int pid, int fd)
{
    task_group_t *group;
    klist_t      *head;
    klist_t      *iter;
    socketfd_t  *fd_obj;

    group = task_group_get_by_pid(pid);
    if (NULL == group) {
        return -1;
    }

    krhino_mutex_lock(&group->socket_mutex, RHINO_WAIT_FOREVER);

    head = &group->socket_fd_list;
    if (is_klist_empty(head)) {
        krhino_mutex_unlock(&group->socket_mutex);
        return -2;
    }

    iter = head->next;
    while (iter != head) {
        fd_obj = krhino_list_entry(iter, socketfd_t, node);
        iter = iter->next;
        if (fd_obj->fd == fd) {
            klist_rm(&fd_obj->node);
            free(fd_obj);
            break;
        }
    }

    krhino_mutex_unlock(&group->socket_mutex);

    return 0;
}

int socketfd_ref(int pid, int fd)
{
    task_group_t *group;
    klist_t      *head;
    socketfd_t  *fd_obj;

    int ret = 0;

    group = task_group_get_by_pid(pid);
    if (NULL == group) {
        return -1;
    }

    krhino_mutex_lock(&group->socket_mutex, RHINO_WAIT_FOREVER);

    head = &group->socket_fd_list;
    fd_obj = (socketfd_t*)malloc(sizeof(socketfd_t));
    if (NULL != fd_obj) {
        fd_obj->fd = fd;
        klist_init(&fd_obj->node);
        klist_insert(head, &fd_obj->node);
    } else {
        /* no mem */
        ret = -2;
    }

    krhino_mutex_unlock(&group->socket_mutex);

    return ret;
}

int socketfd_init(task_group_t *group)
{
    klist_init(&group->socket_fd_list);
    if (RHINO_SUCCESS == krhino_mutex_create(&group->socket_mutex, "socketfd_mutex")) {
        return 0;
    }

    return -1;
}

void socketfd_exit(task_group_t *group)
{
    socketfd_close(group->pid);
    krhino_mutex_del(&group->socket_mutex);
}

