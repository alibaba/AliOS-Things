/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include "wsf_config.h"
#include "wsf_log.h"
#include "wsf_msg_queue.h"
#include "wsf.h"
#include "os.h"

extern wsf_list_node_t *wsf_list_pop_front(wsf_list_t *list);

void wsf_msg_queue_init(wsf_request_queue_t **req_queue)
{
    wsf_request_queue_t *pqueue;
    if (!req_queue) {
        return;
    }

    *req_queue = NULL;

    pqueue = (wsf_request_queue_t *)os_malloc(sizeof(wsf_request_queue_t));
    if ( NULL == pqueue ) {
        LOGW(MODULE_NAME, "memory allocate fail.");
        return;
    }

    memset(pqueue, 0, sizeof(wsf_request_queue_t));

    pqueue->mutex = os_mutex_init();
    if ( NULL == pqueue->mutex ) {
        LOGW(MODULE_NAME, "create mutex fail.");
        goto do_error;
    }

    pqueue->psem = os_semaphore_init();
    if ( NULL == pqueue->psem ) {
        LOGW(MODULE_NAME, "create semaphore fail.");
        goto do_error;
    }

    dlist_init(&pqueue->list);
    pqueue->length = 0;

    *req_queue = pqueue;

    return ;

do_error:
    wsf_msg_queue_destroy(pqueue);

    return;
}

void wsf_msg_queue_flush(wsf_request_queue_t *req_queue)
{
    if (!req_queue) {
        return;
    }

    while(req_queue->length) {
        os_mutex_lock(req_queue->mutex);
        dlist_t *list = &req_queue->list;

        wsf_request_node_t *node, *next;
        dlist_t *tmp = NULL;
        dlist_for_each_entry_safe(list, tmp, node, wsf_request_node_t, list_head) {
            if (node) {
                wsf_msg_session_t session = node->session;
                //LOG("--->session: %p, req: %p, res: %p\n",node,session.request,session.response);
                if (session.cb) {
                    session.cb(NULL, session.extra);
                } else {
                    wsf_msg_session_signal(&node->session);
                    continue;
                }
                if (session.psem) {
                    os_semaphore_destroy(session.psem);
                }
                if (session.request) {
                    os_free(session.request);
                }
                if (session.response) {
                    os_free(session.response);
                }
                dlist_del(&node->list_head);
                os_free(node);
                req_queue->length --;
            }
        }
        os_mutex_unlock(req_queue->mutex);
        os_msleep(100);
    }
}

//destroy the queue
void wsf_msg_queue_destroy(wsf_request_queue_t *req_queue)
{
    if (!req_queue) {
        return;
    }

    wsf_msg_queue_flush(req_queue);
    os_mutex_destroy(req_queue->mutex);
    os_semaphore_destroy(req_queue->psem);

    os_free(req_queue);
}

static void wsf_del_first_msg(wsf_request_queue_t *req_queue)
{
    dlist_t *node = NULL;
    wsf_request_node_t *req_node = NULL;

    if (!req_queue) {
        return;
    }

    os_mutex_lock(req_queue->mutex);

    node = req_queue->list.prev;
    if (dlist_empty(&req_queue->list)) {
        os_mutex_unlock(req_queue->mutex);
        return;
    }

    req_node = (wsf_request_node_t *)node;
    dlist_del(node);
    req_queue->length--;

    if (req_node->session.cb) {
        req_node->session.cb(NULL, req_node->session.extra);
    }

    wsf_msg_session_destroy(&req_node->session);
    os_free(req_node);
    os_mutex_unlock(req_queue->mutex);
}

//FIXME: delete the msg.
int wsf_request_queue_push(wsf_request_queue_t *req_queue,
                           wsf_request_node_t *req_node)
{
    if (!req_node || !req_queue) {
        LOGE(MODULE_NAME, "!!!wsf_request_queue_push err\n");
        return -1;
    }

    if (req_queue->length >= wsf_get_config()->max_msg_queue_length) {
        if (req_node->session.cb) {
            LOGI(MODULE_NAME, "wsf msg queue is full , drop the last msg.\n");
            wsf_del_first_msg(req_queue);
        } else {
            return -1;
        }
    }

    os_mutex_lock(req_queue->mutex);
    dlist_add(&req_node->list_head, &req_queue->list);
    req_queue->length++;
    os_mutex_unlock(req_queue->mutex);

    return 0;
}

int wsf_request_queue_pop(wsf_request_queue_t *req_queue,
                          wsf_request_node_t *req_node)
{
    if (dlist_empty(&req_node->list_head)) {
        return 0;
    }

    if (!req_node || !req_queue || !req_queue->length) {
        LOGE(MODULE_NAME, "!!!wsf_request_queue_pop err, length=%d", req_queue->length);
        return -1;
    }

    os_mutex_lock(req_queue->mutex);
    if (!dlist_empty(&req_node->list_head)) {
        dlist_del(&req_node->list_head);
        req_queue->length--;
    }
    os_mutex_unlock(req_queue->mutex);

    return 0;
}

wsf_request_node_t *wsf_request_queue_trigger(wsf_request_queue_t *req_queue,
                                              wsf_msg_t *rsp)
{
    wsf_request_node_t *ret = NULL;
    uint32_t msg_id = 0;

    if (!req_queue || !rsp) {
        LOGE(MODULE_NAME, "!!!wsf_request_queue_trigger error");
        return ret;
    }
    memcpy(&msg_id, &rsp->header.msg_id, sizeof(uint32_t));

    os_mutex_lock(req_queue->mutex);
    dlist_t *list = &req_queue->list;
    wsf_request_node_t *node = NULL;
    dlist_for_each_entry(list, node, wsf_request_node_t, list_head) {
        wsf_msg_session_t *session = &node->session;
        if (session && session->id == msg_id) {
            dlist_del(&node->list_head);
            dlist_init(&node->list_head);
            req_queue->length--;
            session->response = rsp;
            ret = node;
            break;
        }
    }
    os_mutex_unlock(req_queue->mutex);

    return ret;
}
