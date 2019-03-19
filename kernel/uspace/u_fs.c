/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <u_task.h>
#include <k_list.h>

typedef struct {
    klist_t node;
    int     fd;
    int     ref_cnt;
} fs_fd_t;

#define close_your_fs_fd(fd)

int fs_fd_close(int pid)
{
    task_group_t *group;
    klist_t *head;
    klist_t *iter;
    fs_fd_t *fd_obj;

    group = get_task_group_by_pid(pid);
    if (NULL == group) {
        printf("%s: Invalid pid %d\r\n", __func__, pid);
        return -1;
    }

    krhino_sem_take(&group->fs_sem, RHINO_WAIT_FOREVER);

    head = &group->fs_fd_list;
    if (!is_klist_empty(head)) {
        iter = head->next;
        while (iter != head) {
            fd_obj = krhino_list_entry(iter, fs_fd_t, node);
            iter = iter->next;
            klist_rm(&fd_obj->node);
            close_your_fs_fd(fd_obj->fd);
            free(fd_obj);
        }
    }

    krhino_sem_give(&group->fs_sem);

    return 0;
}

// called in fs close
int fs_fd_dec_ref(int pid, int fd)
{
    task_group_t *group;
    klist_t *head;
    klist_t *iter;
    fs_fd_t *fd_obj;

    group = get_task_group_by_pid(pid);
    if (NULL == group) {
        printf("%s: Invalid pid %d\r\n", __func__, pid);
        return -1;
    }

    head = &group->fs_fd_list;
    if (is_klist_empty(head)) {
        printf("%s: Not found fd %d, pid %d\r\n", __func__, fd, pid);
        return -2;
    }

    krhino_sem_take(&group->fs_sem, RHINO_WAIT_FOREVER);

    iter = head->next;
    while (iter != head) {
        fd_obj = krhino_list_entry(iter, fs_fd_t, node);
        iter = iter->next;
        if (fd_obj->fd == fd) {
            if (--fd_obj->ref_cnt == 0) {
                klist_rm(&fd_obj->node);
                free(fd_obj);
                break;
            }
        }
    }

    krhino_sem_give(&group->fs_sem);

    return 0;
}


// called in fs open
int fs_fd_inc_ref(int pid, int fd)
{
    task_group_t *group;
    klist_t *head;
    klist_t *iter;
    fs_fd_t *fd_obj;
    int      found;
    int      ret = 0;

    group = get_task_group_by_pid(pid);
    if (NULL == group) {
        printf("%s: Invalid pid %d\r\n", __func__, pid);
        return -1;
    }

    head = &group->fs_fd_list;
    if (is_klist_empty(head)) {
        printf("%s: Not found fd %d, pid %d\r\n", __func__, fd, pid);
        return -2;
    }

    krhino_sem_take(&group->fs_sem, RHINO_WAIT_FOREVER);

    found = 0;
    iter = head->next;
    while (iter != head) {
        fd_obj = krhino_list_entry(iter, fs_fd_t, node);
        iter = iter->next;
        if (fd_obj->fd == fd) {
            fd_obj->ref_cnt++;
            found = 1;
            break;
        }
    }

    if (!found) {
        fd_obj = (fs_fd_t*)malloc(sizeof(fs_fd_t));
        if (NULL != fd_obj) {
            fd_obj->fd = fd;
            fd_obj->ref_cnt = 1;
            klist_init(&fd_obj->node);
            klist_insert(head, &fd_obj->node);
        } else {
            /* no mem */
            ret = -3;
        }
    }

    krhino_sem_give(&group->fs_sem);

    return ret;
}


