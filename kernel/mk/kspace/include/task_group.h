/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef TASK_GROUP_H
#define TASK_GROUP_H

#include "k_api.h"

/* task group object type */
#define RES_OBJ_TASK      0x00UL
#define RES_OBJ_MUTEX     0x01UL
#define RES_OBJ_SEM       0x02UL
#define RES_OBJ_QUEUE     0x03UL
#define RES_OBJ_BUF_QUEUE 0x04UL
#define RES_OBJ_EVENT     0x05UL

/* task group state normal */
#define TGS_NORMAL  0x00UL
/* task group state exitting */
#define TGS_EXITING 0x01UL

#define group_info_entry(node, type, member) \
    ((type *)((uint8_t *)(node) - (size_t)(&((type *)0)->member)))

#define TG_NAME_MAX_LEN   (16)

typedef struct {
    klist_t        node;
    uint32_t       pid;
    name_t         tg_name[TG_NAME_MAX_LEN];
    uint8_t        state;
    int32_t        task_cnt;

    /* resource list object */
    kobj_list_t    kobj_list;

    /* resource buf queue, used to malloc and free user space resource */
    kbuf_queue_t  *res_request_q;
    kbuf_queue_t  *res_reply_q;
    kmutex_t       res_mutex;

    /* list fs fd on this list, close the fd duruing
     * process exitting if they are not closed by user
     */
    klist_t   fs_fd_list;
    kmutex_t  fs_mutex;

    /* list socket fd on this list, close the fd duruing
     * process exitting if they are not closed by user
     */
    klist_t  socket_fd_list;
    kmutex_t socket_mutex;

    /* end linkkit during process exitting if linkkit
     * is not end
     */
    int linkkit_active;

    /* command line queue, to support user space
     * command line
     */
    kbuf_queue_t *cli_q;

    kbuf_queue_t *cb_call_buf_queue;
    kbuf_queue_t *cb_ret_buf_queue;

    /* user private data */
    void *priv;
} task_group_t;

klist_t *task_group_get_list_head(void);

task_group_t *task_group_get_by_pid(uint32_t pid);

int task_group_add_res_obj(task_group_t *group, unsigned int type, void *res);

int task_group_remove_res_obj(task_group_t *group, unsigned int type, void *res);

int task_group_init(task_group_t *group, const char *name, int pid);

void task_group_release(task_group_t *group);

void task_group_destory(task_group_t *group);

#endif /* TASK_GROUP_H */

