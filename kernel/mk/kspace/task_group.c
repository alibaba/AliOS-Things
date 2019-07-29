/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#include <stdio.h>

#include "k_api.h"
#include "res.h"
#include "fs_fd.h"
#include "socket_fd.h"
#include "callback.h"

#include "cli/cli_api.h"

extern int linkkit_close(int pid);

static klist_t task_group_list_head = {
    &task_group_list_head,
    &task_group_list_head
};

static void task_group_list_add(task_group_t *group)
{
    CPSR_ALLOC();

    RHINO_CRITICAL_ENTER();
    klist_add(&task_group_list_head, &group->node);
    RHINO_CRITICAL_EXIT();
}

static void task_group_list_rm(task_group_t *group)
{
    CPSR_ALLOC();

    RHINO_CRITICAL_ENTER();
    klist_rm(&group->node);
    RHINO_CRITICAL_EXIT();
}

klist_t* task_group_get_list_head(void)
{
    return &task_group_list_head;
}

task_group_t* task_group_get_by_pid(uint32_t pid)
{
    klist_t *head;
    klist_t *list_iter;

    task_group_t *group_iter;
    task_group_t *group;

    CPSR_ALLOC();

    group = NULL;
    head  = task_group_get_list_head();

    RHINO_CRITICAL_ENTER();

    list_iter  = head->next;
    for (; list_iter != head; list_iter = list_iter->next) {
        group_iter = group_info_entry(list_iter, task_group_t, node);
        if (group_iter && (group_iter->pid == pid)) {
            group = group_iter;
            break;
        }
    }

    RHINO_CRITICAL_EXIT();

    return group;
}

int task_group_add_res_obj(task_group_t *group, unsigned int type, void *res)
{
    CPSR_ALLOC();

    int ret = 0;

    RHINO_CRITICAL_ENTER();

    switch(type) {
        case RES_OBJ_TASK:
            klist_init(&((ktask_t*)res)->task_user);
            klist_insert(&group->kobj_list.task_head, &((ktask_t*)res)->task_user);
            group->task_cnt++;
            break;
        case RES_OBJ_MUTEX:
            klist_insert(&group->kobj_list.mutex_head, &((kmutex_t*)res)->blk_obj.obj_list);
            break;
#if (RHINO_CONFIG_SEM > 0)
        case RES_OBJ_SEM:
            klist_insert(&group->kobj_list.sem_head, &((ksem_t*)res)->blk_obj.obj_list);
            break;
#endif
#if (RHINO_CONFIG_QUEUE > 0)
        case RES_OBJ_QUEUE:
            klist_insert(&group->kobj_list.queue_head, &((kqueue_t*)res)->blk_obj.obj_list);
            break;
#endif
#if (RHINO_CONFIG_BUF_QUEUE > 0)
        case RES_OBJ_BUF_QUEUE:
            klist_insert(&group->kobj_list.buf_queue_head, &((kbuf_queue_t*)res)->blk_obj.obj_list);
            break;
#endif
#if (RHINO_CONFIG_EVENT_FLAG > 0)
        case RES_OBJ_EVENT:
            klist_insert(&group->kobj_list.event_head, &((kevent_t*)res)->blk_obj.obj_list);
            break;
#endif
        default:
            ret = -1;
            break;
    }

    RHINO_CRITICAL_EXIT();

    return ret;
}

int task_group_remove_res_obj(task_group_t *group, unsigned int type, void *res)
{
    CPSR_ALLOC();

    int ret = 0;

    RHINO_CRITICAL_ENTER();

    switch(type) {
        case RES_OBJ_TASK:
            klist_rm(&((ktask_t*)res)->task_user);
            group->task_cnt--;
            break;
        case RES_OBJ_MUTEX:
            klist_rm(&((kmutex_t*)res)->blk_obj.obj_list);
            break;
#if (RHINO_CONFIG_SEM > 0)
        case RES_OBJ_SEM:
            klist_rm(&((ksem_t*)res)->blk_obj.obj_list);
            break;
#endif
#if (RHINO_CONFIG_QUEUE > 0)
        case RES_OBJ_QUEUE:
            klist_rm(&((kqueue_t*)res)->blk_obj.obj_list);
            break;
#endif
#if (RHINO_CONFIG_BUF_QUEUE > 0)
        case RES_OBJ_BUF_QUEUE:
            klist_rm(&((kbuf_queue_t*)res)->blk_obj.obj_list);
            break;
#endif
#if (RHINO_CONFIG_EVENT_FLAG > 0)
        case RES_OBJ_EVENT:
            klist_rm(&((kevent_t*)res)->blk_obj.obj_list);
            break;
#endif
        default:
            ret = -1;
            break;
    }

    RHINO_CRITICAL_EXIT();

    return ret;
}

int task_group_init(task_group_t *group, const char *name, int pid)
{
    int ret;
    int len;

    memset(group, 0, sizeof(task_group_t));

    group->pid = pid;

    len = strlen((const char*)name);
    if (len >= TG_NAME_MAX_LEN) {
        len = TG_NAME_MAX_LEN - 1;
    }

    memcpy(group->tg_name, (const char*)name, len);

    group->state = TGS_NORMAL;

    /* process kobj init */
    klist_init(&(group->kobj_list.task_head));
    klist_init(&(group->kobj_list.mutex_head));
#if (RHINO_CONFIG_SEM > 0)
    klist_init(&(group->kobj_list.sem_head));
#endif
#if (RHINO_CONFIG_QUEUE > 0)
    klist_init(&(group->kobj_list.queue_head));
#endif
#if (RHINO_CONFIG_BUF_QUEUE > 0)
    klist_init(&(group->kobj_list.buf_queue_head));
#endif
#if (RHINO_CONFIG_EVENT_FLAG > 0)
    klist_init(&(group->kobj_list.event_head));
#endif

    task_group_list_add(group);

#if (RHINO_CONFIG_UFS > 0)
    fs_init(group);
#endif

#if (RHINO_CONFIG_UTCPIP > 0)
    socket_init(group);
#endif

#if (RHINO_CONFIG_ULINKKIT > 0)
    group->linkkit_active = 0;
#endif

#if (RHINO_CONFIG_URES > 0)
    ret = res_init(group);
    if (ret != 0) {
        goto delete_group_info;
    }
#endif

#if (RHINO_CONFIG_UCALLBACK > 0)
    ret = cb_init(group);
    if (ret != 0) {
        goto delete_res;
    }
#endif

#if (RHINO_CONFIG_UCLI > 0)
    ret = cli_process_init(group->pid);
    if (ret != 0) {
        goto delete_cb;
    }
#endif

    return 0;

delete_cb:
#if (RHINO_CONFIG_UCALLBACK > 0)
    cb_destory(group);
#endif
delete_res:
#if (RHINO_CONFIG_URES > 0)
    res_destory(group);
#endif
delete_group_info:
#if (RHINO_CONFIG_UTCPIP > 0)
    socket_destory(group);
#endif
#if (RHINO_CONFIG_UFS > 0)
    fs_destory(group);
#endif
    task_group_list_rm(group);

    return ret;
}

void task_group_release(task_group_t *group)
{
#if (RHINO_CONFIG_UCLI > 0)
    cli_process_exit(group->pid);
    cli_process_destory(group->pid);
#endif

#if (RHINO_CONFIG_ULINKKIT > 0)
    linkkit_close(group->pid);
#endif

#if (RHINO_CONFIG_UFS > 0)
    fs_fd_close(group->pid);
    fs_destory(group);
#endif

#if (RHINO_CONFIG_UTCPIP > 0)
    socket_fd_close(group->pid);
    socket_destory(group);
#endif

#if (RHINO_CONFIG_URES > 0)
    res_destory(group);
#endif

#if (RHINO_CONFIG_UCALLBACK > 0)
    cb_destory(group);
#endif
}

void task_group_destory(task_group_t *group)
{
    task_group_list_rm(group);

    printf("process %s  is unloaded, pid %d\r\n", group->tg_name, group->pid);

    krhino_mm_free(group);
}

