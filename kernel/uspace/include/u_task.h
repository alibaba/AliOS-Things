/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef U_TASK_H
#define U_TASK_H

#include <k_api.h>

/* process object type */
#define RES_OBJ_TASK        0x00UL
#define RES_OBJ_MUTEX       0x01UL
#define RES_OBJ_SEM         0x02UL
#define RES_OBJ_QUEUE       0x03UL
#define RES_OBJ_BUF_QUEUE   0x04UL
#define RES_OBJ_EVENT       0x05UL

/* user resource ID */
#define USER_RES_REQUEST_BUF_QUEUE          0x00UL
#define USER_RES_REPLY_BUF_QUEUE            0x01UL
#define USER_CLI_BUF_QUEUE                  0x02UL
#define USER_CB_CALL_BUF_QUEUE              0x03UL
#define USER_CB_RET_BUF_QUEUE               0x04UL

/* task group state normal */
#define TGS_NORMAL  0x00UL
/* task group state exitting */
#define TGS_EXITING 0x01UL

#define group_info_entry(node, type, member) \
    ((type *)((uint8_t *)(node) - (size_t)(&((type *)0)->member)))

#define PNAME_MAX_LEN   (16)

typedef struct {
    klist_t       node;
    uint32_t      pid;
    char          pname[PNAME_MAX_LEN];

    unsigned int state;

    int task_cnt;

    /* resource list object */
    kobj_list_t   kobj_list;

    ksem_t exit_sem;

    /* list fs fd on this list, close the fd duruing
     * process exitting if they are not closed by user
     */
    klist_t fs_fd_list;
    ksem_t fs_sem;

    /* list socket fd on this list, close the fd duruing
     * process exitting if they are not closed by user
     */
    klist_t socket_fd_list;
    ksem_t socket_sem;

    /* end linkkit during process exitting if linkkit
     * is not end
     */
    int linkkit_running;

    /* resource buf queue, used to malloc and free user space resource */
    kbuf_queue_t     *res_request_q;
    kbuf_queue_t     *res_reply_q;
    ksem_t            res_sem;

    /* command line queue, to support user space
     * command line
     */
    kbuf_queue_t     *cli_q;

    kbuf_queue_t *cb_call_buf_queue;
    kbuf_queue_t *cb_ret_buf_queue;

    /* user private data */
    unsigned long priv;
} task_group_t;

kstat_t krhino_utask_create(ktask_t **task, const name_t *name, void *arg,
                            uint8_t pri, tick_t ticks, cpu_stack_t *ustack_buf,
                            size_t ustack, size_t kstack, task_entry_t entry,
                            uint8_t autorun);

kstat_t krhino_utask_del(ktask_t *task);

kstat_t krhino_uprocess_create(const name_t *process_name, ktask_t **task,
                               const name_t *name, void *arg,
                               uint8_t pri, tick_t ticks, cpu_stack_t *ustack_buf,
                               size_t ustack, size_t kstack, task_entry_t entry,
                               uint32_t pid, uint8_t autorun);

/* the function only should be called in user space */
kstat_t krhino_utask_dyn_create(ktask_t **task, const name_t *name, void *arg,
                                uint8_t pri, tick_t ticks ,size_t ustack,
                                size_t kstack, task_entry_t entry, uint8_t autorun);

int krhino_uprocess_kill(int pid);

int krhino_uprocess_kill_force(int pid);

kstat_t krhino_uprocess_exit(void);

kstat_t krhino_uprocess_join(void);

int krhino_uprocess_is_try_exit(void);

void krhino_uprocess_try_exit(void);

void krhino_uprocess_res_get(int32_t id, void **res);

int task_group_add_res_obj(task_group_t *group, unsigned int type, void *res);

int task_group_remove_res_obj(task_group_t *group, unsigned int type, void *res);

task_group_t* get_task_group_by_pid(uint32_t pid);

void task_group_release(task_group_t *group);

#endif /* U_TASK_H */

