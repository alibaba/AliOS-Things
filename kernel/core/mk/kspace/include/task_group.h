/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#ifndef TASK_GROUP_H
#define TASK_GROUP_H

#include "k_api.h"

#if (AOS_KERNEL_OBJECT > 0)
    #include "object_api.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

/* user resource ID */
#define USER_RES_REQUEST_BUF_QUEUE  "/USER_RES_REQUEST_"
#define USER_RES_REPLY_BUF_QUEUE    "/USER_RES_REPLY_"
#define USER_CLI_BUF_QUEUE          "/USER_CLI_"
#define USER_CB_CALL_BUF_QUEUE      "/USER_CB_CALL_"
#define USER_CB_RET_BUF_QUEUE       "/USER_CB_RET_"

#define USER_NAME_MAX       63
#define RES_QUEUE_MSG_NUM   20

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

#define TG_NAME_MAX_LEN (16)

typedef struct task_group {

#if (AOS_KERNEL_OBJECT > 0)
    struct obj_core        obj_core;
    master_handle_table_t  mstr_handle_tbl;
    kmutex_t               obj_mgt_lock;
#endif

    klist_t  node;
    uint32_t pid;
    name_t   tg_name[TG_NAME_MAX_LEN];
    uint8_t  state;
    int32_t  task_cnt;

    /* user task return entry */
    void *ret_entry;

    /* resource list object */
    kobj_list_t kobj_list;

    /* resource buf queue, used to malloc and free user space resource */
    kmutex_t      res_mutex;

    /* list fs fd on this list, close the fd duruing
     * process exitting if they are not closed by user
     */
    klist_t  fs_fd_list;
    kmutex_t fs_mutex;

    /* list socket fd on this list, close the fd duruing
     * process exitting if they are not closed by user
     */
    klist_t  socket_fd_list;
    kmutex_t socket_mutex;

    /* command line queue, to support user space
     * command line
     */
    klist_t       cli_cmd_list;
    kmutex_t      cli_mutex;
    /* inited in kernel space, using in kernel space only */
    void          *cli_q;
    int32_t       cli_cmd_cnt;
    /* inited in kernel space, using in kernel space only */
    void          *cb_call_buf_queue;
    /* inited in kernel space, using in kernel space only */
    void          *cb_ret_buf_queue;
    /* user private data */
    void *priv;
#if (RHINO_CONFIG_USIGNAL > 0)
    /* The user space process signal control block. */
    sigpcb_t       signal;
#endif
} task_group_t;

klist_t *task_group_get_list_head(void);

task_group_t *task_group_get_by_pid(uint32_t pid);

int task_group_add_res_obj(task_group_t *group, unsigned int type, void *res);

int task_group_remove_res_obj(task_group_t *group, unsigned int type, void *res);

int task_group_init(task_group_t *group, const char *name, int pid);

void task_group_proc_free(task_group_t *group);

#ifdef __cplusplus
}
#endif

#endif /* TASK_GROUP_H */

