#include <k_api.h>
#include <u_proc_msg.h>

#define MAX_MSG_SIZE 0xffffffffu

size_t krhino_msg_get(uint32_t key, uint32_t flg, size_t size)
{
    klist_t      *tmp;
    klist_t      *head;
    kbuf_queue_t *queue;
    kstat_t       ret;

    CPSR_ALLOC();

    head = &g_kobj_list.buf_queue_head;

    RHINO_CRITICAL_ENTER();
    for (tmp = head->next; tmp != head; tmp = tmp->next) {
        queue = krhino_list_entry(tmp, kbuf_queue_t, buf_queue_item);
        if (queue->key == key) {
            return (size_t)queue;
        }
    }
    RHINO_CRITICAL_EXIT();

    ret = krhino_buf_queue_dyn_create(&queue, "buf_queue", size, MAX_MSG_SIZE);

    if (ret != RHINO_SUCCESS) {
        return 0;
    }

    queue->key = key;

    return (size_t)queue;        
}

kstat_t krhino_msg_snd(size_t msq_id, void *msg, size_t msg_sz)
{
    kstat_t ret;

    ret = krhino_buf_queue_send((kbuf_queue_t *)msq_id, msg, msg_sz);
    return ret;
}

kstat_t krhino_msg_recv(size_t msq_id, tick_t ticks, void *msg, size_t *msg_sz)
{
    kstat_t ret;

    ret = krhino_buf_queue_recv((kbuf_queue_t *)msq_id, ticks, msg, msg_sz);
    return ret;
}

