/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include "aos/kernel.h"
#include "aos/list.h"
#include "utask.h"
#include "utimer.h"
#include "uassert.h"


#define err_proc(err) \
    do { \
        printf("err %d at file %s, line %d", err, __FILE__, __LINE__); \
        while (1); \
    } while (0)

static dlist_t       u_timer_head;
static ktask_t      *u_timer_task;
static aos_hdl_t     u_timer_queue;

static void timer_list_pri_insert(dlist_t *head, utimer_t *timer)
{
    dlist_t    *q;
    dlist_t    *start;
    dlist_t    *end;
    utimer_t   *timer_iter_temp;

    start = head;
    end   = head;

    /* sort by 'match' time */
    for (q = start->next; q != end; q = q->next) {
        timer_iter_temp = krhino_list_entry(q, utimer_t, timer_list);
        if (timer_iter_temp->match > timer->match) {
            break;
        }
    }

    dlist_add_tail(&timer->timer_list, q);
}

static void timer_list_rm(utimer_t *timer)
{
    dlist_t *head;

    head = timer->to_head;
    if (head != NULL) {
        dlist_del(&timer->timer_list);
        timer->to_head = NULL;
    }
}

static int timer_create(utimer_t *timer, const name_t *name, timer_cb_t cb, uint64_t first,
                        uint64_t round, void *arg, uint8_t auto_run, uint8_t mm_alloc_flag)
{
    int err = RHINO_SUCCESS;

    NULL_PARA_CHK(timer);
    NULL_PARA_CHK(name);
    NULL_PARA_CHK(cb);
    if (first == 0u) {
        return RHINO_INV_PARAM;
    }

    if (first > MAX_TIMER_TICKS) {
        return RHINO_INV_PARAM;
    }

    if (round > MAX_TIMER_TICKS) {
        return RHINO_INV_PARAM;
    }

    timer->name          = name;
    timer->cb            = cb;
    timer->init_count    = first;
    timer->round_ms   = round;
    timer->match         = 0u;
    timer->timer_state   = TIMER_DEACTIVE;
    timer->to_head       = NULL;
    timer->mm_alloc_flag = mm_alloc_flag;
    timer->timer_cb_arg  = arg;

    dlist_init(&timer->timer_list);

    timer->obj_type = RHINO_TIMER_OBJ_TYPE;

    if (auto_run > 0u) {
        err = utimer_start(timer);
    }

    TRACE_TIMER_CREATE(krhino_cur_task_get(), timer);

    return err;
}

int utimer_create(utimer_t *timer, const name_t *name, timer_cb_t cb,
                  uint64_t first, uint64_t round, void *arg, uint8_t auto_run)
{
    return timer_create(timer, name, cb, first, round, arg, auto_run, K_OBJ_STATIC_ALLOC);
}

int utimer_del(utimer_t *timer)
{
    u_timer_queue_cb cb;

    NULL_PARA_CHK(timer);

    cb.timer  = timer;
    cb.cb_num = TIMER_CMD_DEL;

    return aos_queue_send(&u_timer_queue, &cb, sizeof(u_timer_queue_cb));
}

#if (RHINO_CONFIG_KOBJ_DYN_ALLOC > 0)
int utimer_dyn_create(utimer_t **timer, const name_t *name, timer_cb_t cb,
                      uint64_t first, uint64_t round, void *arg, uint8_t auto_run)
{
    int   ret;
    utimer_t *timer_obj;

    NULL_PARA_CHK(timer);
    timer_obj = malloc(sizeof(utimer_t));
    if (timer_obj == NULL) {
        return RHINO_NO_MEM;
    }

    ret = timer_create(timer_obj, name, cb, first, round, arg, auto_run, K_OBJ_DYN_ALLOC);
    if (ret != RHINO_SUCCESS) {
        free(timer_obj);
        return ret;
    }

    *timer = timer_obj;

    return ret;
}

int utimer_dyn_del(utimer_t *timer)
{
    u_timer_queue_cb cb;

    NULL_PARA_CHK(timer);

    cb.timer  = timer;
    cb.cb_num = TIMER_CMD_DYN_DEL;

    return aos_queue_send(&u_timer_queue, &cb, sizeof(u_timer_queue_cb));
}
#endif

int utimer_start(utimer_t *timer)
{
    u_timer_queue_cb cb;

    NULL_PARA_CHK(timer);

    cb.timer  = timer;
    cb.cb_num = TIMER_CMD_START;

    return aos_queue_send(&u_timer_queue, &cb, sizeof(u_timer_queue_cb));

}

int utimer_stop(utimer_t *timer)
{
    u_timer_queue_cb cb;

    NULL_PARA_CHK(timer);

    cb.timer  = timer;
    cb.cb_num = TIMER_CMD_STOP;

    return aos_queue_send(&u_timer_queue, &cb, sizeof(u_timer_queue_cb));
}

int utimer_change(utimer_t *timer, uint64_t first, uint64_t round)
{
    u_timer_queue_cb cb;

    NULL_PARA_CHK(timer);
    if (first == 0u) {
        return RHINO_INV_PARAM;
    }

    if (first > MAX_TIMER_TICKS) {
        return RHINO_INV_PARAM;
    }

    if (round > MAX_TIMER_TICKS) {
        return RHINO_INV_PARAM;
    }

    cb.timer   = timer;
    cb.first   = first;
    cb.u.round = round;
    cb.cb_num  = TIMER_CMD_CHG;

    return aos_queue_send(&u_timer_queue, &cb, sizeof(u_timer_queue_cb));
}

int utimer_arg_change(utimer_t *timer, void *arg)
{
    u_timer_queue_cb cb;

    NULL_PARA_CHK(timer);

    cb.timer  = timer;
    cb.u.arg  = arg;
    cb.cb_num = TIMER_ARG_CHG;

    return aos_queue_send(&u_timer_queue, &cb, sizeof(u_timer_queue_cb));
}

int utimer_arg_change_auto(utimer_t *timer, void *arg)
{
    u_timer_queue_cb cb;

    NULL_PARA_CHK(timer);

    cb.timer  = timer;
    cb.u.arg  = arg;
    cb.cb_num = TIMER_ARG_CHG_AUTO;

    return aos_queue_send(&u_timer_queue, &cb, sizeof(u_timer_queue_cb));
}

static void cmd_proc(u_timer_queue_cb *cb, uint8_t cmd)
{
    utimer_t *timer = cb->timer;

    if (timer->obj_type != RHINO_TIMER_OBJ_TYPE) {
        return;
    }

    switch (cmd) {
        case TIMER_CMD_START:
            if (timer->timer_state == TIMER_ACTIVE) {
                break;
            }

            timer->match   = aos_now_ms() + timer->init_count;
            /* used by timer delete */
            timer->to_head = &u_timer_head;
            timer_list_pri_insert(&u_timer_head, timer);
            timer->timer_state = TIMER_ACTIVE;
            break;
        case TIMER_CMD_STOP:
            if (timer->timer_state == TIMER_DEACTIVE) {
                break;
            }
            timer_list_rm(timer);
            timer->timer_state = TIMER_DEACTIVE;
            break;
        case TIMER_CMD_CHG:
            if (cb->first == 0u) {
                break;
            }

            if (timer->timer_state != TIMER_DEACTIVE) {
                /* should stop timer before change attributes */
                break;
            }

            timer->init_count  = cb->first;
            timer->round_ms = cb->u.round;
            break;
        case TIMER_ARG_CHG:
            if (timer->timer_state != TIMER_DEACTIVE) {
                break;
            }

            timer->timer_cb_arg = cb->u.arg;
            break;
        case TIMER_CMD_DEL:
            if (timer->timer_state != TIMER_DEACTIVE) {
                break;
            }

            if (timer->mm_alloc_flag != K_OBJ_STATIC_ALLOC) {
                break;
            }

            timer->obj_type = RHINO_OBJ_TYPE_NONE;
            TRACE_TIMER_DEL(krhino_cur_task_get(), timer);
            break;
#if (RHINO_CONFIG_KOBJ_DYN_ALLOC > 0)
        case TIMER_CMD_DYN_DEL:
            if (timer->timer_state != TIMER_DEACTIVE) {
                break;
            }

            if (timer->mm_alloc_flag != K_OBJ_DYN_ALLOC) {
                break;
            }

            timer->obj_type = RHINO_OBJ_TYPE_NONE;
            TRACE_TIMER_DEL(krhino_cur_task_get(), timer);
            free(timer);
            break;
#endif
        default:
            err_proc(RHINO_SYS_FATAL_ERR);
            //break;
    }

}

static void timer_cmd_proc(u_timer_queue_cb *cb)
{
    if (cb->cb_num == TIMER_ARG_CHG_AUTO) {
        cmd_proc(cb, TIMER_CMD_STOP);
        cmd_proc(cb, TIMER_ARG_CHG);
        cmd_proc(cb, TIMER_CMD_START);
    } else {
        cmd_proc(cb, cb->cb_num);
    }
}

static void timer_task(void *pa)
{
    utimer_t         *first_timer;
    u_timer_queue_cb  cb_msg;
    int           ret;
    size_t            msg_size;
    uint64_t          ms_now;

    (void)pa;

    while (RHINO_TRUE) {
        /* no first_timer is pending, waiting for first_timer operation */
        ret =  aos_queue_recv(&u_timer_queue, RHINO_WAIT_FOREVER, &cb_msg, &msg_size);
        if (ret != RHINO_SUCCESS) {
            err_proc(RHINO_SYS_FATAL_ERR);
        }

        /* handle timer operations */
        timer_cmd_proc(&cb_msg);

        /* check if there is timer pending */
        while (!dlist_empty(&u_timer_head)) {

            /* get this first coming timer */
            first_timer = krhino_list_entry(u_timer_head.next, utimer_t, timer_list);
            ms_now = aos_now_ms();

            /* check if first_timer run out */
            if (first_timer->match > ms_now) {

                /* first_timer not run out, waiting for timer operation */
                ret =  aos_queue_recv(&u_timer_queue, first_timer->match - ms_now, &cb_msg, &msg_size);
                if (ret == RHINO_SUCCESS) {
                    /* handle timer operations */
                    timer_cmd_proc(&cb_msg);
                    continue;
                }

                if (ret != -ETIMEDOUT) {
                    err_proc(RHINO_SYS_FATAL_ERR);
                }

                /* TIMEOUT, first_timer run out */
            }

            /* handle the first run out timer */
            first_timer->cb(first_timer, first_timer->timer_cb_arg);
            timer_list_rm(first_timer);

            if (first_timer->round_ms > 0u) {
                first_timer->match   = first_timer->match + first_timer->round_ms;
                first_timer->to_head = &u_timer_head;
                timer_list_pri_insert(&u_timer_head, first_timer);
            } else {
                first_timer->timer_state = TIMER_DEACTIVE;
            }
        }
    }
}

int timer_task_start(size_t kstack_size, size_t ustack_size, uint8_t prio, size_t msg_num)
{
    int          ret = 0;

    dlist_init(&u_timer_head);

    ret = aos_queue_new(&u_timer_queue, sizeof(u_timer_queue_cb) * msg_num, sizeof(u_timer_queue_cb));
    uassert(RHINO_SUCCESS == ret);

    ret = krhino_utask_dyn_create(&u_timer_task, "timer_task", NULL,
                                  prio, (uint64_t)0u, ustack_size, kstack_size,
                                  timer_task, 1u);
    if (RHINO_SUCCESS != ret) {
        aos_queue_close(&u_timer_queue);
        uassert(0);
    }

    ret = krhino_sched_policy_set(u_timer_task, KSCHED_RR, prio);
    if (RHINO_SUCCESS != ret) {
        aos_queue_close(&u_timer_queue);
        uassert(0);
    }

    return 0;
}

