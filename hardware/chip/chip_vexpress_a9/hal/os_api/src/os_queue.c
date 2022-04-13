#include <stdlib.h>
#include "aos_common.h"
#include "aos_queue.h"
#include "typedef.h"
#include "k_api.h"

os_queue_handle_t _os_queue_create(u32_t queue_len,u32_t item_size)
{
    int ret = 0;
    os_queue__t *queue = NULL;
    char *buf = NULL;
    buf = malloc(queue_len*item_size);
    NULL_PARA_CHECK(buf,NULL);
    queue = malloc(sizeof(os_queue__t));
    if(queue == NULL) {
        free(buf);
        buf = NULL;
    }
    NULL_PARA_CHECK(queue,NULL);
    ret = krhino_fix_buf_queue_create(queue, "default_queue", buf, item_size, queue_len);
    if(0 == ret)
    {
        queue_debug_info("os queue create sucess,handle:%x,len:%u,size:%u\r\n",queue,queue_len,item_size);
        return queue;
    }
    else
    {
        if (buf)
            free(buf);
        if (queue)
            free(queue);
        queue_debug_error("os queue create fail,reason:%d\r\n",ret);
        return NULL;
    }

}

int _os_queue_send(os_queue_handle_t queue_handle,void *send_buf,os_tick_type_t wait_time)
{
    NULL_PARA_CHECK(queue_handle,os_false);
    int ret = 0;
    int count = 0;
    unsigned int item_size = ((kbuf_queue_t*)queue_handle)->max_msg_size;
    ret = krhino_buf_queue_send(queue_handle,send_buf,item_size);

    while(RHINO_BUF_QUEUE_FULL == ret)//if queue full wait
    {
        if(0 == wait_time)  break;
        _os_delay_ms(20);
        count+=20;
        ret = krhino_buf_queue_send((kbuf_queue_t*)queue_handle,send_buf,item_size);
        if(0 == ret || count>=wait_time)
        {
            break;
        }
        if(count == 5000)
        {
            queue_debug_info("queue send fail after 5 seconds because of queue full,block time:%u\r\n",wait_time);
        }
    }
    if(0 != ret)
    {
        queue_debug_error("os queue send fail,reason:%d\r\n",ret);
        return os_false;
    }
    else
    {
        queue_debug_info("os queue send sucess,handle:%x\r\n",queue_handle);
        return os_true;
    }

}
int _os_queue_recive(os_queue_handle_t queue_handle,void *recv_buf,os_tick_type_t wait_time)
{
    NULL_PARA_CHECK(queue_handle,os_false);
    NULL_PARA_CHECK(recv_buf,os_false);

    int ret = 0;
    int recv_size=0;
    if(OS_WAIT_FOREVER == wait_time)
    {
        ret = krhino_buf_queue_recv(queue_handle,RHINO_WAIT_FOREVER,recv_buf,&recv_size);
    }
    else
    {
        ret = krhino_buf_queue_recv(queue_handle,wait_time,recv_buf,&recv_size);
    }
    if(0 == ret)
    {
        queue_debug_info("os queue recive sucess,handle:%x,size:%d\r\n",queue_handle,recv_size);
        return os_true;
    }
    else
    {
        queue_debug_error("os queue recive fail,reason:%d,block time:%x\r\n",ret,wait_time);
        return os_false;
    }
}
int _os_queue_delete(os_queue_handle_t queue_handle)
{
    NULL_PARA_CHECK(queue_handle,os_false);
    int ret = 0;
    ret = krhino_buf_queue_del(queue_handle);
    if(0!=ret)
    {
        queue_debug_error("os queue delete fail,reason:%d\r\n",ret);
        return os_false;
    }
    queue_debug_info("os queue delete sucess,handle:%x\r\n",queue_handle);
    if(NULL != queue_handle)
        _os_free(queue_handle);
    return os_true;
}

int _os_queue_used_nums(os_queue_handle_t queue_handle)
{
    NULL_PARA_CHECK(queue_handle,os_false);
    int ret =0;
    kbuf_queue_info_t queue_info;
    ret = krhino_buf_queue_info_get(queue_handle,&queue_info);
    if(0!=ret)
    {
        queue_debug_error("queue query error,handle:%x reason:%d\r\n",queue_handle,ret);
        return os_false;
    }
    queue_debug_info("queue query sucess,handle:%x used items:%d\r\n",queue_handle,queue_info.cur_num);
    return queue_info.cur_num;
}
int _os_queue_free_nums(os_queue_handle_t queue_handle)
{
    NULL_PARA_CHECK(queue_handle,os_false);
    int available_num = 0;
    int ret = 0;

    int num_in_queue = _os_queue_used_nums(queue_handle);
    //available_num = ((os_queue__t*)queue_handle)->max_msg_size - num_in_queue;
    available_num = (((os_queue__t*)queue_handle)->ringbuf).freesize/(((os_queue__t*)queue_handle)->ringbuf).blk_size;
    if(num_in_queue>=0)
    {
        queue_debug_info("queue query sucess,handle:%x available items:%d\r\n",queue_handle,available_num);
    }
    else
    {
        queue_debug_error("queue query error,handle:%x reason:%d\r\n",queue_handle,ret);
        return os_false;
    }
    return available_num;
}

#define RING_BUF_LEN sizeof(size_t)

kstat_t ringbuf_peek(k_ringbuf_t *p_ringbuf, void *pdata, size_t *plen)
{
    size_t   split_len           = 0;
    uint8_t *data                = pdata;
    size_t   len                 = 0;
    uint8_t  c_len[RING_BUF_LEN] = {0};
    size_t   len_bytes           = 0;

    if (p_ringbuf->type == RINGBUF_TYPE_FIX) {
        if (p_ringbuf->head == p_ringbuf->end) {
            p_ringbuf->head = p_ringbuf->buf;
        }

        memcpy(pdata, p_ringbuf->head, p_ringbuf->blk_size);
        //p_ringbuf->head += p_ringbuf->blk_size;
        //p_ringbuf->freesize += p_ringbuf->blk_size;

        if (plen != NULL) {
            *plen = p_ringbuf->blk_size;
        }

        return RHINO_SUCCESS;
    } else {

        return -1;

    }
}


kstat_t krhino_buf_queue_peek(kbuf_queue_t *queue, tick_t ticks, void *msg, size_t *size)
{
    CPSR_ALLOC();

    kstat_t ret;
    uint8_t cur_cpu_num;

    NULL_PARA_CHK(queue);
    NULL_PARA_CHK(msg);
    NULL_PARA_CHK(size);

    RHINO_CRITICAL_ENTER();

    cur_cpu_num = cpu_cur_get();
    TASK_CANCEL_CHK(queue);

    if ((g_intrpt_nested_level[cur_cpu_num] > 0u) && (ticks != RHINO_NO_WAIT)) {
        RHINO_CRITICAL_EXIT();
        return RHINO_NOT_CALLED_BY_INTRPT;
    }

    if (queue->blk_obj.obj_type != RHINO_BUF_QUEUE_OBJ_TYPE) {
        RHINO_CRITICAL_EXIT();
        return RHINO_KOBJ_TYPE_ERR;
    }

    if (queue->cur_num > 0u) {
        ringbuf_peek(&(queue->ringbuf), msg, size);
        //queue->cur_num --;
        RHINO_CRITICAL_EXIT();
        return RHINO_SUCCESS;
    }

    if (ticks == RHINO_NO_WAIT) {
        *size = 0u;
        RHINO_CRITICAL_EXIT();
        return RHINO_NO_PEND_WAIT;
    }

    if (g_sched_lock[cur_cpu_num] > 0u) {
        *size = 0u;
        RHINO_CRITICAL_EXIT();
        return RHINO_SCHED_DISABLE;
    }

    g_active_task[cur_cpu_num]->msg = msg;
    pend_to_blk_obj((blk_obj_t *)queue, g_active_task[cur_cpu_num], ticks);

    TRACE_BUF_QUEUE_GET_BLK(g_active_task[cur_cpu_num], queue, ticks);

    RHINO_CRITICAL_EXIT_SCHED();

    RHINO_CPU_INTRPT_DISABLE();

    cur_cpu_num = cpu_cur_get();

    ret = pend_state_end_proc(g_active_task[cur_cpu_num], &queue->blk_obj);

    switch (ret) {
        case RHINO_SUCCESS:
            *size = g_active_task[cur_cpu_num]->bq_msg_size;
            break;

        default:
            *size = 0u;
            break;
    }

    RHINO_CPU_INTRPT_ENABLE();

    return ret;
}

int _os_queue_peek(os_queue_handle_t queue_handle,void *recv_buf,os_tick_type_t wait_time)
{
    NULL_PARA_CHECK(queue_handle,os_false);
    NULL_PARA_CHECK(recv_buf,os_false);

    int ret = 0;
    int recv_size=0;
    if(OS_WAIT_FOREVER == wait_time)
    {
        ret = krhino_buf_queue_peek(queue_handle,RHINO_WAIT_FOREVER,recv_buf,&recv_size);
    }
    else
    {
        ret = krhino_buf_queue_peek(queue_handle,wait_time,recv_buf,&recv_size);
    }
    if(0 == ret)
    {
        queue_debug_info("os queue recive sucess,handle:%x,size:%d\r\n",queue_handle,recv_size);
        return os_true;
    }
    else
    {
        queue_debug_error("os queue recive fail,reason:%d,block time:%x\r\n",ret,wait_time);
        return os_false;
    }
}


