//
// Created by ethan on 2019/7/15.
//
#include <api/mesh.h>
#include <mesh/access.h>
#include "vendor_model.h"
#include "vendor_model_srv.h"
#include "misc/dlist.h"

#define BT_DBG_ENABLED IS_ENABLED(CONFIG_BT_MESH_DEBUG_VENDOR_MODEL)
#include "common/log.h"

static void _vendor_model_retry_timer_cb(void *p_timer, void *args);

/**
 * g_vnd_msg_list was used to save the unconfirmed vendor messages
 * g_vnd_msg_timer was used to control when the vendor messages saved in g_vnd_msg_list will be resent
 * */
sys_dlist_t g_vnd_msg_list;
struct k_timer g_vnd_msg_timer;

/**
 * vendor model publish context, alloc maximum message buffer size
 * Ethan: do we need to shrink the message size here?
 * */
struct bt_mesh_model_pub g_vendor_model_alibaba_pub = {
    .msg = NET_BUF_SIMPLE(3 + 377 + 4), // allocate maximum payload size
};

/** @def _vendor_model_msg_gen_tid
 *
 *  @brief generate tid used in vendor model message
 *
 *  @param NULL
 *
 *  @return tid with range of [0x80, 0xff]
 */
u8_t _vendor_model_msg_gen_tid(void) {
    static u8_t tid = 0x80;

    return (tid++ | 0x80);
}


/** @def vendor_model_init
 *
 *  @brief vendor model server related global parameter initialization
 *
 *  @param NULL
 *
 *  @return always return true
 */
u16_t vendor_model_init(void) {
    static bool init_flag = false;

    if (!init_flag) {
        BT_INFO("init g_vnd_msg_timer");
        k_timer_init(&g_vnd_msg_timer, _vendor_model_retry_timer_cb, &g_vnd_msg_list);
        sys_dlist_init(&g_vnd_msg_list);
    }
    init_flag = true;

    return true;
}

/** @def _vendor_model_msg_node_free
 *
 *  @brief free the vendor model message node struct's memory
 *
 *  @param pointer to the vendor model message node to be freed
 *
 *  @return 0 for success; negative for failure
 */
static s16_t _vendor_model_msg_node_free(vnd_model_msg_n *p_node) {
    k_free(p_node);
    return 0;
}

/** @def _vendor_model_msg_node_generate
 *
 *  @brief duplicate vnd_model_msg and save to vnd_model_msg_n
 *
 *  @param pointer to the vendor model message to be duplicated
 *
 *  @return pointer to vnd_model_msg_n for success, NULL for failure
 */
static vnd_model_msg_n* _vendor_model_msg_node_generate(vnd_model_msg *p_model_msg) {
    vnd_model_msg_n *p_node = NULL;

    if (!p_model_msg->retry)
        p_model_msg->retry = VENDOR_MODEL_MSG_DFT_RETRY_TIMES;
    else if (p_model_msg->retry > VENDOR_MODEL_MSG_MAX_RETRY_TIMES)
        p_model_msg->retry = VENDOR_MODEL_MSG_MAX_RETRY_TIMES;

    p_node = k_malloc(sizeof(vnd_model_msg_n) + p_model_msg->len);
    if (!p_node) {
        BT_ERR("malloc for vnd_model_msg_n failed");
        return NULL;
    }

    memcpy(&p_node->msg, p_model_msg, sizeof(vnd_model_msg));
    BT_DBG("p_node->msg:%p, data:%p, %p", &p_node->msg, &p_node->msg.data, &p_node->msg.data + 1)
    p_node->msg.data = (&p_node->msg.data + 1);
    memcpy(p_node->msg.data, p_model_msg->data, p_model_msg->len);
    BT_DBG("p_model_msg->data:%p, %s", p_model_msg->data, bt_hex(p_model_msg->data, p_model_msg->len));
    BT_DBG("p_node->msg.data:%p, %s", p_node->msg.data, bt_hex(p_node->msg.data, p_node->msg.len));
    p_node->timeout = k_now_ms() + VENDOR_MODEL_MSG_RETRY_PERIOD;

    p_node->left_retry =  p_model_msg->retry;

    return p_node;
}

/** @def _vendor_model_msg_list_append
 *
 *  @brief duplicate vnd_model_msg and append it to vendor model message list to be monitored
 *
 *  @param pointer to the vendor model message to be duplicated
 *
 *  @return 0 for success; negative for failure
 */
s16_t _vendor_model_msg_list_append(vnd_model_msg *p_model_msg) {
    vnd_model_msg_n *p_msg_node = NULL;

    p_msg_node = _vendor_model_msg_node_generate(p_model_msg);

    if (!p_msg_node)
        return -2;

    BT_DBG("append msg:%p, opid:%x, retry:%d, head:%p, node:%p", p_model_msg, p_model_msg->opid, p_model_msg->retry, &g_vnd_msg_list, &p_msg_node->node);

    sys_dlist_append(&g_vnd_msg_list, &p_msg_node->node);

    //Check retry timer, if timer is not started yet, start it
    if (!k_timer_is_started(&g_vnd_msg_timer))
        k_timer_start(&g_vnd_msg_timer, VENDOR_MODEL_MSG_RETRY_PERIOD);

    return 0;
}

/** @def _vendor_model_retry_timer_cb
 *
 *  @brief timeout handler for the g_vnd_msg_timer
 *
 *  @param p_timer - pointer to the timer; args - pointer to g_vnd_msg_list
 *
 *  @return N/A
 */
static void _vendor_model_retry_timer_cb(void *p_timer, void *args) {
    sys_dlist_t *p_head = (sys_dlist_t *)args;
    sys_dnode_t *p_node = NULL;
    u32_t nearest = VENDOR_MODEL_MSG_RETRY_PERIOD;
    vnd_model_msg_n *p_msg_node = NULL;
    vnd_model_msg *p_msg = NULL;

    BT_DBG("g_vnd_msg_timer timeout, p_head:%p", p_head);

    /**
     * 1. go through p_head
     * 2. resend the no responsed messages if timeout happens and refresh timeout value
     * */
    SYS_DLIST_FOR_EACH_NODE(p_head, p_node)
    {
        p_msg_node = CONTAINER_OF(p_node, vnd_model_msg_n, node);
        p_msg = &p_msg_node->msg;
        BT_DBG("msg:%p, opid:%d, left:%d", p_msg, p_msg->opid, p_msg_node->left_retry);

        /* Ethan: TODO - need to consider the wrap round case */
        if (p_msg_node->timeout <= k_now_ms()) {
            BT_DBG("timeout - msg:%p, opid:%x, left:%d", p_msg, p_msg->opid, p_msg_node->left_retry);
            vendor_model_msg_send(p_msg);
            if (--p_msg_node->left_retry <= 0) {
                sys_dlist_remove(p_node);
                _vendor_model_msg_node_free(p_node);
                break;
            }
            p_msg_node->timeout = k_now_ms() + VENDOR_MODEL_MSG_RETRY_PERIOD;
        } else {
            if (nearest > p_msg_node->timeout)
                nearest = p_msg_node->timeout;
         }
    }

    /* start new timer */
    if(!sys_dlist_is_empty(p_head)) {
        k_timer_start(&g_vnd_msg_timer, nearest);
        BT_DBG("restart retry timer, timeout:%d", nearest);
    } else {
        k_timer_stop(&g_vnd_msg_timer);
        BT_DBG("list empty, stop timer");
    }
    return;
}

/** @def _vendor_model_msg_check_tid
 *
 *  @brief check received vendor message's tid
 *
 *  @param pointer to g_vnd_msg_list, tid of the received vendor model message
 *
 *  @return 0 for success; negative for failure
 */
static s16_t _vendor_model_msg_check_tid(sys_dlist_t *p_head, u8_t tid) {
    sys_dnode_t *p_node = NULL;

    if (!p_head)
        return -1;

    if (sys_dlist_is_empty(p_head))
        return 0;

    /**
     * go through message list and dequeue the vendor model's message and free it if received message
     * s tid equals this message's tid
     * */
    SYS_DLIST_FOR_EACH_NODE(p_head, p_node) {
        vnd_model_msg *p_msg = NULL;
        vnd_model_msg_n *p_msg_node = NULL;

        p_msg_node = CONTAINER_OF(p_node, vnd_model_msg_n, node);
        p_msg = &p_msg_node->msg;

        if (p_msg->tid == tid) {
            BT_DBG("dequeue msg:%p, opid:%x, retry:%d", p_msg, p_msg->opid, p_msg->retry);
            sys_dlist_remove(p_node);
            _vendor_model_msg_node_free(p_node);
            break;
        }

    }

    return 0;
}

/**
 * Ethan: need to add lock to prevent from multiple access
 * */

/** @def vendor_model_msg_send
 *
 *  @brief send the vendor model message
 *
 *  @param pointer to the message to be sent
 *
 *  @return 0 for success; negative for failure
 */
s16_t vendor_model_msg_send(vnd_model_msg *p_model_msg)
{
    struct bt_mesh_model_pub *p_pub = &g_vendor_model_alibaba_pub;
    struct net_buf_simple *p_msg = &p_pub->msg;
    s16_t err = -1;
    struct bt_mesh_model *p_model = bt_mesh_model_find_vnd(p_model_msg->p_elem, BT_MESH_MODEL_VND_COMPANY_ID, BT_MESH_MODEL_VND_MODEL_SRV);
    struct bt_mesh_msg_ctx ctx;
    bool resend_flag = false;

    // Ethan: temp solution
    vendor_model_init();

    if (!p_model){
        BT_ERR("cannot find vendor model server\n");
        return err;
    }

    BT_INFO("p_model:%p, cid:0x%x, id:0x%x, retry:%d\n", p_model, p_model->vnd.company, p_model->vnd.id, p_model_msg->retry);
    /**
     * no need to duplicate the following messages
     * 1. retry <= 0 - the message won't want to be resent
     * 2. already duplicated or CONFIME/CONFIME_TG - p_model_msg->tid is in valid range [0x80, 0xff]
     * 3. SET_UNACK/CONFIME/CONFIME_TG/TRANSPARENT_MSG/TRANSPARENT_ACK
     * 4. invalid tid (< 0x80)
     * */
    if ((p_model_msg->retry > 1) &&
        (p_model_msg->tid < 0x80) &&
        (p_model_msg->opid != VENDOR_OP_ATTR_SET_UNACK) &&
        (p_model_msg->opid != VENDOR_OP_ATTR_CONFIME) &&
        (p_model_msg->opid != VENDOR_OP_ATTR_CONFIME_TG) &&
        (p_model_msg->opid != VENDOR_OP_ATTR_TRANS_MSG) &&
        (p_model_msg->opid != VENDOR_OP_ATTR_TRANS_ACK)
        ) {
        resend_flag = true;
    }
    /**
     * Ethan: if tid is invalid [0, 0x80), assign valid tid
     * only when opid is one of  VENDOR_OP_ATTR_CONFIME, VENDOR_OP_ATTR_CONFIME_TG and VENDOR_OP_ATTR_TRANS_ACK, shall we keep tid as it is
     * */
    if (!(p_model_msg->tid) &&
        (p_model_msg->opid != VENDOR_OP_ATTR_CONFIME) &&
        (p_model_msg->opid != VENDOR_OP_ATTR_CONFIME_TG) &&
        (p_model_msg->opid != VENDOR_OP_ATTR_TRANS_MSG) &&
        (p_model_msg->opid != VENDOR_OP_ATTR_TRANS_ACK)) {
        p_model_msg->tid = _vendor_model_msg_gen_tid();
    }

    //prepare buffer
    bt_mesh_model_msg_init(p_msg, BT_MESH_MODEL_OP_3(p_model_msg->opid, CONFIG_MESH_VENDOR_COMPANY_ID));
    net_buf_simple_add_u8(p_msg, p_model_msg->tid);
    if (p_model_msg->len) {
        net_buf_simple_add_mem(p_msg, p_model_msg->data, p_model_msg->len);
    }
    p_model_msg->retry--;

    BT_DBG("p_model_msg->data:%p, %d, %s", p_model_msg, p_model_msg->len, bt_hex(p_model_msg->data, p_model_msg->len));

    if (resend_flag)
        _vendor_model_msg_list_append(p_model_msg);

#if 0
    struct bt_mesh_model_pub *pub = p_model->pub;
    pub->addr = 0xF000;
    err = bt_mesh_model_publish(p_model);
#else
    ctx.app_idx = bt_mesh_model_get_appkey_id();
    ctx.net_idx = bt_mesh_model_get_netkey_id();
    ctx.addr = 0xF000;
    ctx.send_ttl = 3;
    ctx.send_rel = 0;
    err = bt_mesh_model_send(p_model, &ctx, p_msg, NULL, NULL);
#endif

    if (err) {
        BT_ERR("bt_mesh_model_publish err %d\n", err);
    } else
        BT_DBG("Success!!!");

    return err;
}

/** @def _vendor_model_analyze
 *
 *  @brief analyze the received message and notify genie SDK
 *
 *  @param pointer to the received message (vendor model, context and the message buffer) and opid
 *
 *  @return if success return 0; if fails return error no.
 */
static s16_t _vendor_model_analyze(struct bt_mesh_model *p_model,
                           struct bt_mesh_msg_ctx *p_ctx,
                           struct net_buf_simple *p_buf,
                           u8_t opid)
{
    vnd_model_msg msg;
    if (!p_model || !p_buf)
        return MESH_ANALYZE_ARGS_ERROR;

    if(p_buf->len < 3) {
        BT_ERR("invalid buf len(%d)", p_buf->len);
        return MESH_ANALYZE_SIZE_ERROR;
    }

    memset(&msg, 0, sizeof(vnd_model_msg));

    msg.opid = opid;
    msg.tid = net_buf_simple_pull_u8(p_buf);

    if(mesh_check_tid(p_ctx->addr, msg.tid) != MESH_SUCCESS) {
        BT_ERR("MESH_TID_REPEAT src_addr(0x%04x) tid(0x%02x)", p_ctx->addr, msg.tid);
        return MESH_TID_REPEAT;
    }

    msg.len = p_buf->len;
    BT_INFO("opcode:0x%x, tid:%d, len:%d", msg.opid, msg.tid, msg.len);
    if (msg.len) {
        msg.data = (u8_t *)p_buf->data;
        net_buf_simple_pull(p_buf, msg.len);
        BT_DBG("payload:%s", bt_hex(msg.data, msg.len));
    } else
        msg.data = NULL;

    genie_event(GENIE_EVT_SDK_VENDOR_MSG, (void *)&msg);

    return 0;
}

/** @def _vendor_model_get
 *
 *  @brief handle VENDOR_OP_ATTR_GET_STATUS message
 *
 *  @param pointer to the received message (vendor model, context and the message buffer)
 *
 *  @return N/A
 */
static void _vendor_model_get(struct bt_mesh_model *model,
                                  struct bt_mesh_msg_ctx *ctx,
                                  struct net_buf_simple *buf)
{
    return  _vendor_model_analyze(model, ctx, buf, VENDOR_OP_ATTR_GET_STATUS);
}

/** @def _vendor_model_set_ack
 *
 *  @brief handle VENDOR_OP_ATTR_SET_ACK message
 *
 *  @param pointer to the received message (vendor model, context and the message buffer)
 *
 *  @return N/A
 */
static void _vendor_model_set_ack(struct bt_mesh_model *model,
                              struct bt_mesh_msg_ctx *ctx,
                              struct net_buf_simple *buf)
{
    return  _vendor_model_analyze(model, ctx, buf, VENDOR_OP_ATTR_SET_ACK);
}

/** @def _vendor_model_set_unack
 *
 *  @brief handle VENDOR_OP_ATTR_SET_UNACK message
 *
 *  @param pointer to the received message (vendor model, context and the message buffer)
 *
 *  @return N/A
 */
static void _vendor_model_set_unack(struct bt_mesh_model *model,
                              struct bt_mesh_msg_ctx *ctx,
                              struct net_buf_simple *buf)
{
    return  _vendor_model_analyze(model, ctx, buf, VENDOR_OP_ATTR_SET_UNACK);
}

/** @def _vendor_model_confirm
 *
 *  @brief handle VENDOR_OP_ATTR_CONFIME message
 *
 *  @param pointer to the received message (vendor model, context and the message buffer)
 *
 *  @return N/A
 */
static void _vendor_model_confirm(struct bt_mesh_model *model,
                                     struct bt_mesh_msg_ctx *ctx,
                                     struct net_buf_simple *buf)
{
    u8_t tid = 0x0;

    if(buf->len != 1) {
        BT_ERR("invalid buf len(%d)", buf->len);
        return MESH_ANALYZE_SIZE_ERROR;
    }

    tid = net_buf_simple_pull_u8(buf);
    _vendor_model_msg_check_tid(&g_vnd_msg_list, tid);
}

/** @def _vendor_model_confirm_tg
 *
 *  @brief handle VENDOR_OP_ATTR_CONFIME_TG message
 *
 *  @param pointer to the received message (vendor model, context and the message buffer)
 *
 *  @return N/A
 */
static void _vendor_model_confirm_tg(struct bt_mesh_model *model,
                                  struct bt_mesh_msg_ctx *ctx,
                                  struct net_buf_simple *buf)
{
    u8_t tid = 0x0;

    if(buf->len != 1) {
        BT_ERR("invalid buf len(%d)", buf->len);
        return MESH_ANALYZE_SIZE_ERROR;
    }

    tid = net_buf_simple_pull_u8(buf);
    _vendor_model_msg_check_tid(&g_vnd_msg_list, tid);
}

/** @def _vendor_model_transparent
 *
 *  @brief handle VENDOR_OP_ATTR_TRANS_MSG message
 *
 *  @param pointer to the received message (vendor model, context and the message buffer)
 *
 *  @return N/A
 */
static void _vendor_model_transparent(struct bt_mesh_model *model,
                                     struct bt_mesh_msg_ctx *ctx,
                                     struct net_buf_simple *buf)
{
    return  _vendor_model_analyze(model, ctx, buf, VENDOR_OP_ATTR_TRANS_MSG);
}

/** @def _vendor_model_transparent_ack
 *
 *  @brief handle VENDOR_OP_ATTR_TRANS_ACK message
 *
 *  @param pointer to the received message (vendor model, context and the message buffer)
 *
 *  @return N/A
 */
static void _vendor_model_transparent_ack(struct bt_mesh_model *model,
                                          struct bt_mesh_msg_ctx *ctx,
                                          struct net_buf_simple *buf)
{
    u8_t tid = 0x0;

    if(buf->len != 1) {
        BT_ERR("invalid buf len(%d)", buf->len);
        return MESH_ANALYZE_SIZE_ERROR;
    }

    tid = net_buf_simple_pull_u8(buf);
    _vendor_model_msg_check_tid(&g_vnd_msg_list, tid);
}

/** @def g_vendor_model_alibaba_op
 *
 *  @brief vendor model operations struct
 *
 */
const struct bt_mesh_model_op g_vendor_model_alibaba_op[VENDOR_MODEL_OPC_NUM] = {
    { BT_MESH_MODEL_OP_3(VENDOR_OP_ATTR_GET_STATUS, CONFIG_MESH_VENDOR_COMPANY_ID), 2, _vendor_model_get },
    { BT_MESH_MODEL_OP_3(VENDOR_OP_ATTR_SET_ACK, CONFIG_MESH_VENDOR_COMPANY_ID), 2, _vendor_model_set_ack },
    { BT_MESH_MODEL_OP_3(VENDOR_OP_ATTR_SET_UNACK, CONFIG_MESH_VENDOR_COMPANY_ID), 2, _vendor_model_set_unack },
    { BT_MESH_MODEL_OP_3(VENDOR_OP_ATTR_CONFIME, CONFIG_MESH_VENDOR_COMPANY_ID), 1, _vendor_model_confirm },
    { BT_MESH_MODEL_OP_3(VENDOR_OP_ATTR_CONFIME_TG, CONFIG_MESH_VENDOR_COMPANY_ID), 1, _vendor_model_confirm_tg },
    { BT_MESH_MODEL_OP_3(VENDOR_OP_ATTR_TRANS_MSG, CONFIG_MESH_VENDOR_COMPANY_ID), 1, _vendor_model_transparent },
    { BT_MESH_MODEL_OP_3(VENDOR_OP_ATTR_TRANS_ACK, CONFIG_MESH_VENDOR_COMPANY_ID), 1, _vendor_model_transparent_ack },
    BT_MESH_MODEL_OP_END,
};
