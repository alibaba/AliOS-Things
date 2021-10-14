/*
 * Copyright (C) 2018-2020 Alibaba Group Holding Limited
 */

#include <aos/kernel.h>

#define BT_DBG_ENABLED IS_ENABLED(CONFIG_DEBUG_GENIE_TRANSPORT)
#include "common/log.h"
#include "mesh.h"
#include "mesh/access.h"
#include "net.h"
#include "access.h"

#ifdef CONFIG_SCAN_DURATION_AFTER_GENIE_MODEL_SEND
#include "adv.h"
#endif

#include "genie_service.h"

static uint16_t last_src_addr = BT_MESH_ADDR_TMALL_GENIE;
static genie_transport_tid_queue_t tid_queue[RECV_MSG_TID_QUEUE_SIZE];
#ifdef CONFIG_SCAN_DURATION_AFTER_GENIE_MODEL_SEND
static struct k_timer scan_off_timer;
#endif

/**
 * send_list was used to save the unconfirmed vendor messages
 * retransmit_timer was used to control when the vendor messages saved in send_list will be resent
 * */
static sys_dlist_t send_list;
static struct k_timer retransmit_timer;
static aos_mutex_t transport_mutex;

/** @def genie_transport_msg_node_generate
 *
 *  @brief duplicate genie_transport_model_param_t and save to genie_transport_node_t
 *
 *  @param pointer to the vendor model message to be duplicated
 *
 *  @return pointer to genie_transport_node_t for success, NULL for failure
 */
static genie_transport_node_t *genie_transport_msg_node_generate(genie_transport_model_param_t *p_model_msg)
{
    genie_transport_node_t *p_node = NULL;

    if (p_model_msg->retry > VENDOR_MODEL_MSG_MAX_RETRY_TIMES)
    {
        p_model_msg->retry = VENDOR_MODEL_MSG_MAX_RETRY_TIMES;
    }

    p_node = hal_malloc(sizeof(genie_transport_node_t) + p_model_msg->len);
    if (!p_node)
    {
        BT_ERR("malloc for genie_transport_node_t failed");
        return NULL;
    }

    memcpy(&p_node->msg, p_model_msg, sizeof(genie_transport_model_param_t));
    BT_DBG("p_node->msg:%p, data:%p, %p", &p_node->msg, &p_node->msg.data, &p_node->msg.data + 1);
    p_node->msg.data = (uint8_t *)(&p_node->msg.data + 1);
    memcpy(p_node->msg.data, p_model_msg->data, p_model_msg->len);
    BT_DBG("p_model_msg->data:%p, %s", p_model_msg->data, bt_hex(p_model_msg->data, p_model_msg->len));
    BT_DBG("p_node->msg.data:%p, %s", p_node->msg.data, bt_hex(p_node->msg.data, p_node->msg.len));
    p_node->timeout = k_uptime_get() + p_model_msg->retry_period;

    p_node->left_retry = p_model_msg->retry;

    return p_node;
}

static genie_transport_model_param_t *genie_transport_find_by_tid(uint8_t tid)
{
    sys_dnode_t *p_node = NULL;
    genie_transport_model_param_t *p_msg = NULL;
    genie_transport_node_t *p_msg_node = NULL;

    SYS_DLIST_FOR_EACH_NODE(&send_list, p_node)
    {
        p_msg_node = CONTAINER_OF(p_node, genie_transport_node_t, node);
        if (p_msg_node == NULL)
        {
            continue;
        }

        p_msg = &p_msg_node->msg;

        if (p_msg->tid == tid)
        {
            return p_msg;
        }
    }

    return NULL;
}

#ifdef CONFIG_SCAN_DURATION_AFTER_GENIE_MODEL_SEND
static void scan_off_timer_cb(void *p_timer, void *args)
{
    bt_mesh_scan_disable();
}

static void send_start_cb(u16_t duration, int err, void *cb_data)
{
    bt_mesh_scan_disable();
}

static void send_end_cb(int err, void *cb_data)
{
    bt_mesh_scan_enable();
    k_timer_start(&scan_off_timer, CONFIG_SCAN_DURATION_AFTER_GENIE_MODEL_SEND);
}
#endif

/** @def genie_transport_model_send
 *
 *  @brief send the vendor model message
 *
 *  @param pointer to the message to be sent
 *
 *  @return 0 for success; negative for failure
 */
static int16_t genie_transport_model_send(genie_transport_model_param_t *p_model_msg)
{
    int16_t err = -1;
    struct bt_mesh_model_pub *p_pub = &genie_model_pub;
    struct net_buf_simple *p_msg = p_pub->msg;
    struct bt_mesh_model *p_model = bt_mesh_model_find_vnd(p_model_msg->p_elem, CONFIG_MESH_VENDOR_COMPANY_ID, CONFIG_MESH_VENDOR_MODEL_SRV);
    struct bt_mesh_msg_ctx ctx;

    if (!p_model)
    {
        BT_ERR("cannot find vendor model server %p\n", p_model_msg->p_elem);
        return err;
    }

    BT_DBG("p_model:%p, cid:0x%x, id:0x%x, retry:%d", p_model, p_model->vnd.company, p_model->vnd.id, p_model_msg->retry);

    BT_DBG("tid:%02x opcode:%02x", p_model_msg->tid, p_model_msg->opid);
    BT_DBG("p_model_msg->data:%p, %d, %s", p_model_msg, p_model_msg->len, bt_hex(p_model_msg->data, p_model_msg->len));

    //prepare buffer
    bt_mesh_model_msg_init(p_msg, BT_MESH_MODEL_OP_3(p_model_msg->opid, CONFIG_MESH_VENDOR_COMPANY_ID));
    net_buf_simple_add_u8(p_msg, p_model_msg->tid);
    if (p_model_msg->len)
    {
        net_buf_simple_add_mem(p_msg, p_model_msg->data, p_model_msg->len);
    }

    ctx.app_idx = bt_mesh_model_get_appkey_id(p_model_msg->p_elem, p_model);
    ctx.net_idx = bt_mesh_model_get_netkey_id(p_model_msg->p_elem);
    ctx.addr = p_model_msg->dst_addr;
    ctx.send_ttl = GENIE_TRANSPORT_DEFAULT_TTL;
    ctx.send_rel = 0;

    if (ctx.addr == BT_MESH_ADDR_UNASSIGNED)
    {
        ctx.addr = BT_MESH_ADDR_TMALL_GENIE;
    }

#ifdef CONFIG_SCAN_DURATION_AFTER_GENIE_MODEL_SEND
    static struct bt_mesh_send_cb model_send_cb = {
        .start = send_start_cb,
        .end = send_end_cb,
    };

    err = bt_mesh_model_send(p_model, &ctx, p_msg, &model_send_cb, NULL);
#else
    err = bt_mesh_model_send(p_model, &ctx, p_msg, NULL, NULL);
#endif

    if (err)
    {
        BT_ERR("genie transport send fail:%d\n", err);
    }
    else
    {
        BT_DBG("genie transport send success");
    }

    return err;
}

/** @def genie_transport_append_mesg
 *
 *  @brief duplicate genie_transport_model_param_t and append it to vendor model message list to be monitored
 *
 *  @param pointer to the vendor model message to be duplicated
 *
 *  @return 0 for success; negative for failure
 */
static int16_t genie_transport_append_mesg(genie_transport_model_param_t *p_model_msg)
{
    genie_transport_node_t *p_msg_node = NULL;

    BT_DBG("append msg:%p, opid:%x, retry:%d, head:%p, node:%p", p_model_msg, p_model_msg->opid, p_model_msg->retry, &send_list, &p_msg_node->node);
    if (sys_dlist_node_number(&send_list) >= CONFIG_VENDOR_SEND_MSG_MAX)
    {
        GENIE_LOG_WARN("send list full");
        return -1;
    }

    aos_mutex_lock(&transport_mutex, AOS_WAIT_FOREVER);
    p_msg_node = genie_transport_msg_node_generate(p_model_msg);
    if (!p_msg_node)
    {
        return -1;
    }

    sys_dlist_append(&send_list, &p_msg_node->node);
    aos_mutex_unlock(&transport_mutex);

    //Check retry timer, if timer is not started yet, start it
    if (!k_timer_is_started(&retransmit_timer))
    {
        k_timer_start(&retransmit_timer, p_model_msg->retry_period);
    }

    return 0;
}

E_MESH_ERROR_TYPE genie_transport_check_tid(u16_t src_addr, uint8_t tid, uint8_t elem_id)
{
    static uint8_t cur_index = 0;
    uint8_t i = cur_index;
    uint8_t ri = 0;
    uint32_t cur_time = k_uptime_get();
    uint32_t end_time = 0;

    if (src_addr >= TMALL_GENIE_UADDR_START && src_addr <= TMALL_GENIE_UADDR_END)
    {
        src_addr = TMALL_GENIE_UADDR_START;
    }

    while (i < cur_index + RECV_MSG_TID_QUEUE_SIZE)
    {
        ri = i % RECV_MSG_TID_QUEUE_SIZE;
        if ((tid_queue[ri].tid == tid) && (tid_queue[ri].addr == src_addr) && (tid_queue[ri].elemid == elem_id))
        {
            end_time = tid_queue[ri].time + GENIE_TRANSPORT_DEDUPLICATE_DURATION;
            if (cur_time < end_time)
            {
                break;
            }
        }
        i++;
    }
    if (i < cur_index + RECV_MSG_TID_QUEUE_SIZE)
    {
        return MESH_TID_REPEAT;
    }
    else
    {
        tid_queue[cur_index].tid = tid;
        tid_queue[cur_index].elemid = elem_id;
        tid_queue[cur_index].addr = src_addr;
        tid_queue[cur_index].time = cur_time;
        cur_index++;
        cur_index %= RECV_MSG_TID_QUEUE_SIZE;

        return MESH_SUCCESS;
    }
}

void genie_transport_src_addr_set(uint16_t src_addr)
{
    last_src_addr = src_addr;
}

uint16_t genie_transport_src_addr_get()
{
    return last_src_addr;
}

/** @def transport_msg_node_free
 *
 *  @brief free the vendor model message node struct's memory
 *
 *  @param pointer to the vendor model message node to be freed
 *
 *  @return 0 for success; negative for failure
 */
static int16_t transport_msg_node_free(genie_transport_node_t *p_node)
{
    hal_free(p_node);

    return 0;
}

static int transport_remove_by_tid(uint8_t tid)
{
    sys_dnode_t *p_node = NULL;
    sys_dnode_t *p_node_safe = NULL;

    if (sys_dlist_is_empty(&send_list))
    {
        return -1;
    }

    aos_mutex_lock(&transport_mutex, AOS_WAIT_FOREVER);
    SYS_DLIST_FOR_EACH_NODE_SAFE(&send_list, p_node, p_node_safe)
    {
        genie_transport_model_param_t *p_msg = NULL;
        genie_transport_node_t *p_msg_node = NULL;

        p_msg_node = CONTAINER_OF(p_node, genie_transport_node_t, node);
        p_msg = &p_msg_node->msg;

        BT_DBG("msg %02x", p_msg->tid);
        if (p_msg->tid == tid)
        {
            sys_dlist_remove(p_node);
            transport_msg_node_free((genie_transport_node_t *)p_node);
            return 0;
        }
    }
    aos_mutex_unlock(&transport_mutex);

    return -1;
}

static uint16_t transport_get_send_timeout(uint16_t payload_len)
{
    return GENIE_TRANSPORT_EACH_PDU_TIMEOUT * genie_transport_get_seg_count(payload_len);
}

int genie_transport_send_model(genie_transport_model_param_t *p_model_msg)
{
    int r = -1;

    if (!p_model_msg || !p_model_msg->data || p_model_msg->len == 0 || !bt_mesh_is_provisioned())
    {
        BT_ERR("send param err");
        return r;
    }

    BT_DBG("opcode:0x%x, tid:%d, len:%d", p_model_msg->opid, p_model_msg->tid, p_model_msg->len);
    BT_DBG("payload:%s", p_model_msg->len ? bt_hex(p_model_msg->data, p_model_msg->len) : "empty");

    switch (p_model_msg->opid)
    {
    case VENDOR_OP_ATTR_STATUS:
    case VENDOR_OP_ATTR_INDICATE:
    case VENDOR_OP_ATTR_INDICATE_TG:
    case VENDOR_OP_ATTR_TRANS_MSG:
    case VENDOR_OP_ATTR_TRANS_INDICATE:
    {
        if (p_model_msg->tid == 0)
        {
            p_model_msg->tid = genie_transport_gen_tid();
        }

        /**
     * no need to duplicate the following messages
     * 1. retry <= 0 - the message won't want to be resent
     * 2. tid device tid is [0x80, 0xBF]
     * 3. opcode is not VENDOR_OP_ATTR_INDICATE/VENDOR_OP_ATTR_INDICATE_TG/VENDOR_OP_ATTR_TRANS_INDICATE
     * 4. already duplicated or CONFIME/CONFIME_TG
     * */
        if ((p_model_msg->tid >= GENIE_TRANSPORT_TID_MIN &&
             p_model_msg->tid <= GENIE_TRANSPORT_TID_MAX))
        {
            if (genie_transport_find_by_tid(p_model_msg->tid) != NULL)
            {
                transport_remove_by_tid(p_model_msg->tid); //remove old
            }

            if ((p_model_msg->opid != VENDOR_OP_ATTR_STATUS) && (p_model_msg->opid != VENDOR_OP_ATTR_TRANS_MSG))
            {
                //append for retransmit
                if (p_model_msg->retry_period > 0)
                {
                    r = genie_transport_append_mesg(p_model_msg); //append new
                }
            }

            GENIE_LOG_INFO("SendTID(%02X)\n", p_model_msg->tid);
            r = genie_transport_model_send(p_model_msg); //Send at first time
        }
        else
        {
            BT_WARN("no send");
        }
    }
    break;
    default:
    {
        BT_WARN("unknown opid:0x%x", p_model_msg->opid);
    }
    break;
    }

    return r;
}

int genie_transport_send_payload(genie_transport_payload_param_t *payload_param)
{
    genie_transport_model_param_t transport_model_param = {0};

    if (payload_param == NULL || payload_param->p_payload == NULL)
    {
        GENIE_LOG_ERR("invalid param");
        return -1;
    }

    memset(&transport_model_param, 0, sizeof(genie_transport_model_param_t));
    transport_model_param.opid = payload_param->opid;
    transport_model_param.tid = payload_param->tid;
    transport_model_param.data = payload_param->p_payload;
    transport_model_param.len = payload_param->payload_len;
    transport_model_param.p_elem = genie_mesh_get_primary_element();
    transport_model_param.retry_period = transport_get_send_timeout(payload_param->payload_len);
    transport_model_param.retry = payload_param->retry_cnt;
    transport_model_param.result_cb = payload_param->result_cb;
    transport_model_param.dst_addr = payload_param->dst_addr;

    return genie_transport_send_model(&transport_model_param);
}

/** @def genie_transport_gen_tid
 *
 *  @brief generate tid used in vendor model message
 *
 *  @param NULL
 *
 *  @return tid with range of [0x80, 0xBF]
 */
uint8_t genie_transport_gen_tid(void)
{
    static uint8_t tid = GENIE_TRANSPORT_TID_MAX;

    if (tid == GENIE_TRANSPORT_TID_MAX) //When bootup use rand tid
    {
        bt_rand(&tid, 1);
        tid &= 0x3F;
    }
    else
    {
        tid = (tid + 1) & 0x3F;
    }

    return (tid | 0x80);
}

uint8_t genie_transport_get_seg_count(uint16_t msg_len)
{
    if (msg_len <= 11) //One byte is Lower PDU header
    {
        return 1;
    }

    msg_len -= 8; //SZMIC

    return (msg_len % GENIE_TRANSPORT_EACH_PDU_SIZE) ? (msg_len / GENIE_TRANSPORT_EACH_PDU_SIZE + 2) : (msg_len / GENIE_TRANSPORT_EACH_PDU_SIZE + 1);
}

bool genie_transport_tx_in_progress(void)
{
    if (!sys_dlist_is_empty(&send_list))
    {
        return true;
    }

    return false;
}

/** @def retransmit_timer_cb
 *
 *  @brief timeout handler for the retransmit_timer
 *
 *  @param p_timer - pointer to the timer; args - pointer to send_list
 *
 *  @return N/A
 */
static void retransmit_timer_cb(void *p_timer, void *args)
{
    sys_dlist_t *p_head = (sys_dlist_t *)args;
    sys_dnode_t *p_node = NULL;
    sys_dnode_t *p_node_safe = NULL;
    uint32_t nearest = 0;
    genie_transport_node_t *p_msg_node = NULL;
    genie_transport_model_param_t *p_msg = NULL;

    BT_DBG("retransmit_timer timeout, p_head:%p", p_head);

    /**
     * 1. go through p_head
     * 2. resend the no responsed messages if timeout happens and refresh timeout value
     * */
    aos_mutex_lock(&transport_mutex, AOS_WAIT_FOREVER);
    SYS_DLIST_FOR_EACH_NODE_SAFE(p_head, p_node, p_node_safe)
    {
        p_msg_node = CONTAINER_OF(p_node, genie_transport_node_t, node);
        nearest = p_msg_node->msg.retry_period;
        p_msg = &p_msg_node->msg;
        BT_DBG("msg:%p, opid:%d, left:%d\n", p_msg, p_msg->opid, p_msg_node->left_retry);

        if (p_msg_node->timeout <= k_uptime_get())
        {
            BT_DBG("timeout - msg:%p, opid:%x, left:%d\n", p_msg, p_msg->opid, p_msg_node->left_retry);
            if (p_msg_node->left_retry-- == 0)
            {
                GENIE_LOG_INFO("TID(%02X) timeout\n", p_msg_node->msg.tid);
                if (p_msg_node->msg.result_cb)
                {
                    p_msg_node->msg.result_cb(SEND_RESULT_TIMEOUT);
                }
                sys_dlist_remove(p_node);
                transport_msg_node_free((genie_transport_node_t *)p_node);
                break;
            }
            GENIE_LOG_INFO("ReTID(%02X), LR(%d)\n", p_msg->tid, p_msg_node->left_retry);
            genie_transport_model_send(p_msg);
            p_msg_node->timeout = k_uptime_get() + p_msg_node->msg.retry_period;
        }
        else
        {
            if (nearest > p_msg_node->timeout)
            {
                nearest = p_msg_node->timeout;
            }
        }
    }
    aos_mutex_unlock(&transport_mutex);

    /* start new timer */
    if (!sys_dlist_is_empty(p_head))
    {
        k_timer_start(&retransmit_timer, nearest);
        BT_DBG("restart retry timer, timeout:%d\n", nearest);
    }
    else
    {
        k_timer_stop(&retransmit_timer);
        BT_DBG("list empty, stop timer\n");
    }

    return;
}

/** @def genie_transport_ack
 *
 *  @brief check received vendor message's tid
 *
 *  @param pointer to send_list, tid of the received vendor model message
 *
 *  @return 0 for success; negative for failure
 */
int genie_transport_ack(uint8_t tid)
{
    sys_dnode_t *p_node = NULL;
    sys_dnode_t *p_node_safe = NULL;

    if (sys_dlist_is_empty(&send_list))
    {
        return 0;
    }

    /**
     * go through message list and dequeue the vendor model's message and free it if received message
     * s tid equals this message's tid
     * */
    BT_DBG("recv %02x\n", tid);
    aos_mutex_lock(&transport_mutex, AOS_WAIT_FOREVER);
    SYS_DLIST_FOR_EACH_NODE_SAFE(&send_list, p_node, p_node_safe)
    {
        genie_transport_model_param_t *p_msg = NULL;
        genie_transport_node_t *p_msg_node = NULL;

        p_msg_node = CONTAINER_OF(p_node, genie_transport_node_t, node);
        p_msg = &p_msg_node->msg;

        BT_DBG("msg %02x\n", p_msg->tid);
        if (p_msg->tid == tid)
        {
            BT_DBG("dequeue msg:%p, opid:%x, retry:%d\n", p_msg, p_msg->opid, p_msg->retry);
            if (p_msg->result_cb)
            {
                p_msg->result_cb(SEND_RESULT_SUCCESS);
            }

            sys_dlist_remove(p_node);
            transport_msg_node_free((genie_transport_node_t *)p_node);
            break;
        }
    }
    aos_mutex_unlock(&transport_mutex);

    return 0;
}

void genie_transport_init(void)
{
    sys_dlist_init(&send_list);
    k_timer_init(&retransmit_timer, retransmit_timer_cb, &send_list);
    aos_mutex_new(&transport_mutex);

#ifdef CONFIG_SCAN_DURATION_AFTER_GENIE_MODEL_SEND
    k_timer_init(&scan_off_timer, scan_off_timer_cb, NULL);
#endif
}
