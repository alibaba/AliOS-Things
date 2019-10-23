/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include "k_config.h"
#include "task_group.h"

typedef struct {
    klist_t node;
    int     fd;
    int     ref_cnt;
} fsfd_t;

extern int fsfd_close(int fd);

static int fsfd_close_all(int pid)
{
    task_group_t *group;
    klist_t      *head;
    klist_t      *iter;
    fsfd_t       *fd_obj;

    group = task_group_get_by_pid(pid);
    if (NULL == group) {
        return -1;
    }

    krhino_mutex_lock(&group->fs_mutex, RHINO_WAIT_FOREVER);

    head = &group->fs_fd_list;
    if (!is_klist_empty(head)) {
        iter = head->next;
        while (iter != head) {
            fd_obj = krhino_list_entry(iter, fsfd_t, node);
            iter = iter->next;
            klist_rm(&fd_obj->node);
            fsfd_close(fd_obj->fd);
            krhino_mm_free(fd_obj);
        }
    }

    krhino_mutex_unlock(&group->fs_mutex);

    return 0;
}

/* called in fs close */
int fsfd_deref(int pid, int fd)
{
    task_group_t *group;
    klist_t      *head;
    klist_t      *iter;
    fsfd_t      *fd_obj;

    group = task_group_get_by_pid(pid);
    if (NULL == group) {
        return -1;
    }

    head = &group->fs_fd_list;
    if (is_klist_empty(head)) {
        return -2;
    }

    krhino_mutex_lock(&group->fs_mutex, RHINO_WAIT_FOREVER);

    iter = head->next;
    while (iter != head) {
        fd_obj = krhino_list_entry(iter, fsfd_t, node);
        iter = iter->next;
        if (fd_obj->fd == fd) {
            if (--fd_obj->ref_cnt == 0) {
                klist_rm(&fd_obj->node);
                krhino_mm_free(fd_obj);
                break;
            }
        }
    }

    krhino_mutex_unlock(&group->fs_mutex);

    return 0;
}

/* called in fs open */
int fsfd_ref(int pid, int fd)
{
    task_group_t *group;
    klist_t      *head;
    klist_t      *iter;
    fsfd_t      *fd_obj;

    int found;
    int ret = 0;

    group = task_group_get_by_pid(pid);
    if (NULL == group) {
        return -1;
    }

    krhino_mutex_lock(&group->fs_mutex, RHINO_WAIT_FOREVER);

    found = 0;
    head  = &group->fs_fd_list;
    iter  = head->next;
    while (iter != head) {
        fd_obj = krhino_list_entry(iter, fsfd_t, node);
        iter = iter->next;
        if (fd_obj->fd == fd) {
            fd_obj->ref_cnt++;
            found = 1;
            break;
        }
    }

    if (found == 0) {
        fd_obj = (fsfd_t*)krhino_mm_alloc(sizeof(fsfd_t));
        if (NULL != fd_obj) {
            fd_obj->fd      = fd;
            fd_obj->ref_cnt = 1;

            klist_init(&fd_obj->node);
            klist_insert(head, &fd_obj->node);
        } else {
            /* no mem */
            ret = -3;
        }
    }

    krhino_mutex_unlock(&group->fs_mutex);

    return ret;
}

int fsfd_init(task_group_t *group)
{
    klist_init(&group->fs_fd_list);
    krhino_mutex_create(&group->fs_mutex, "fs_fd_mutex");

    return 0;
}

void fsfd_exit(task_group_t *group)
{
    fsfd_close_all(group->pid);
    krhino_mutex_del(&group->fs_mutex);
}

