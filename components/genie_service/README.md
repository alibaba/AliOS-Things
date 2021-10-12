## 1.概述
`genie_service`是对天猫精灵定义的Vendor Model相关协议的实现，同时包含SIG Model中Generic On Off等Model的解析和数据应答

## 2.组件安装
```bash
yoc init
yoc install genie_service
```

## 3. 配置
### 主要宏定义说明
用户可配置的宏定义的配置文件：components/genie_service/package.yaml

| 宏定义名称 | 功能说明 | 备注 |
| --- | --- | --- |
| CONFIG_BT_MESH_PB_ADV | 定义支持基于PB-ADV的广播承载配网 | 所有genie mesh设备都需要支持PB-ADV |
| CONFIG_GENIE_MESH_ENABLE | 定义genie mesh SDK使能 | 默认使能 |
| CONFIG_GENIE_MESH_PORT | 定义在genie mesh SDK中使用genie event | 默认使能 |
| CONFIG_AOS_CLI | 定义支持串口命令 | 默认支持 |
| CONFIG_BT_MESH_PROV_TIMEOUT | 定义关闭bt_mesh中CONFIG_BT_MESH_PROV_TIMEOUT | provision timeout由genie_provision中的定时器接管 |
| CONFIG_BT_MESH_MODEL_GROUP_COUNT | 定义支持的组播地址个数 | 默认为8 |
| GENIE_DEFAULT_DURATION | 定义ADV发包时长（ms） | 默认为205ms |

## 4.接口
### 4.1 初始化
函数名称：int genie_service_init(genie_service_ctx_t* p_ctx)
参数说明：
结构体genie_service_ctx_t的成员参数如下：

| 参数名称 | 参数类型 | 功能说明 | 备注 |
| --- | --- | --- | --- |
| p_mesh_elem | structbt_mesh_elem | Element的地址 |  |
| mesh_elem_counts | uint8_t | Element的数目 |  |
| event_cb | user_event_cb | SDK上传给应用的事件 | 用户事件回调，系统初始化和复位通知及接收Mesh数据通知，另外还有定时事件和用户自定义串口透传数据通知 |
| prov_timeout | uint32_t | 默认10分钟，超时之后进入静默广播状态 | 静默广播可以进行设备发现，但是不能直接配网 |
| lpm_conf | genie_lpm_conf_t | 低功耗相关参数配置 | 低功耗相关，由宏定义CONFIG_PM_SLEEP开关该功能 |



结构体genie_lpm_conf_t的成员参数如下：

| 参数名称 | 参数类型 | 功能说明 | 备注 |
| --- | --- | --- | --- |
| lpm_wakeup_io | uint8_t | 是否支持GPIO唤醒，0：不支持，>0：支持 |  |
| lpm_wakeup_io_config | genie_lpm_wakeup_io_t | 配置多个唤醒GPIO | 多键开关可以使用 |
| is_auto_enable | uint8_t | 上电之后是否自动进入低功耗状态 | 大于0自动进入低功耗 |
| delay_sleep_time | uint32_t | 上电多久之后进入低功耗状态，默认是10秒 |  |
| sleep_ms | uint16_t | 低功耗睡眠时长 | GLP模式的时候1100ms |
| wakeup_ms | uint16_t | 低功耗唤醒时长 | GLP模式的时候60ms |
| genie_lpm_cb | genie_lpm_cb_t | 睡眠和唤醒状态通知用户的回调函数 |  |

### 4.2 发送Mesh数据
SDK为Genie Vendor Model的数据发送提供了两个API接口，一个需要element参数，另外一个不需要element参数，会直接使用primary element，两个函数的参数介绍如下：
函数名称：int genie_transport_send_model(genie_transport_model_param_t* p_vendor_msg)
参数说明：
结构体genie_transport_model_param_t的成员参数如下：

| 参数名称 | 参数类型 | 功能说明 | 备注 |
| --- | --- | --- | --- |
| p_elem | struct bt_mesh_elem* | 指定发送数据的Element，主要是获取源地址 |  |
| p_model | struct bt_mesh_model* | 指定发送数据的model，主要是获取app key |  |
| result_cb | transport_result_cb | 发送结果回调给应用 | 发送超时或者发送成功（D3和CF消息不会执行回调） |
| retry | uint8_t | 没有收到应答执行重发的次数，默认重发两次 | 如果是GLP设备默认重发五次 |
| opid | uint8_t | opcode id，例如：0xD4 |  |
| tid | uint8_t | 传输ID，通常传入值0，为0的时候SDK会自动生成TID | Mesh设备发送数据的TID从0x80到0xBF |
| dst_addr | uint16_t | 数据目的地址 |  |
| retry_period | uint16_t | 重传的时间间隔 |  |
| len | uint16_t | 发送数据的长度 |  |
| data | uint8_t* | 发送数据的起始地址 |  |



函数名称：int genie_transport_send_payload(genie_transport_payload_param_t* payload_param)
结构体genie_transport_payload_param_t的成员参数如下：

| 参数名称 | 参数类型 | 功能说明 | 备注 |
| --- | --- | --- | --- |
| opid | uint8_t | opcode id，例如：0xD4 |  |
| tid | uint8_t | 传输ID，通常传入值0，为0的时候SDK会自动生成TID | Mesh设备发送数据的TID从0x80到0xBF |
| retry_cnt | uint8_t | 没有收到应答执行重发的次数，默认重发两次 | 如果是GLP设备默认重发五次 |
| dst_addr | uint16_t | 数据目的地址 |  |
| p_payload | uint8_t* | 发送数据的起始地址 |  |
| payload_len | uint16_t | 发送数据的长度 |  |
| result_cb | transport_result_cb | 发送结果回调给应用 | 发送超时或者发送成功（D3和CF消息不会执行回调） |



函数名称：int genie_transport_send_payload(genie_transport_payload_param_t* payload_param)
结构体genie_transport_payload_param_t的成员参数如下：

| 参数名称 | 参数类型 | 功能说明 | 备注 |
| --- | --- | --- | --- |
| opid | uint8_t | opcode id，例如：0xD4 |  |
| tid | uint8_t | 传输ID，通常传入值0，为0的时候SDK会自动生成TID | Mesh设备发送数据的TID从0x80到0xBF |
| retry_cnt | uint8_t | 没有收到应答执行重发的次数，默认重发两次 | 如果是GLP设备默认重发五次 |
| dst_addr | uint16_t | 数据目的地址 |  |
| p_payload | uint8_t* | 发送数据的起始地址 |  |
| payload_len | uint16_t | 发送数据的长度 |  |
| result_cb | transport_result_cb | 发送结果回调给应用 | 发送超时或者发送成功（D3和CF消息不会执行回调） |

## 5.示例
### 5.1 发送数据

- genie_transport_send_model

参考代码如下，可以调用bt_mesh_elem_find_by_id函数获取对应的element指针：
```c
...

genie_transport_model_param_t transport_model_param = {0};

memset(&transport_model_param, 0, sizeof(genie_transport_model_param_t));
transport_model_param.opid = VENDOR_OP_ATTR_INDICATE;
transport_model_param.data = payload;
transport_model_param.len = MIN(SIG_MODEL_INDICATE_PAYLOAD_MAX_LEN, payload_len);
transport_model_param.p_elem = bt_mesh_elem_find_by_id(p_elem->element_id);
transport_model_param.retry_period = GENIE_TRANSPORT_EACH_PDU_TIMEOUT * genie_transport_get_seg_count(transport_model_param.len);
transport_model_param.retry = GENIE_TRANSPORT_DEFAULT_RETRY_COUNT;

genie_transport_send_model(&transport_model_param);

...
```


- genie_transport_send_payload

参考代码如下，
```c
...

genie_transport_payload_param_t transport_payload_param;

memset(&transport_payload_param, 0, sizeof(genie_transport_payload_param_t));
transport_payload_param.opid = VENDOR_OP_ATTR_INDICATE;
transport_payload_param.p_payload = payload;
transport_payload_param.payload_len = sizeof(payload);
transport_payload_param.retry_cnt = GENIE_TRANSPORT_DEFAULT_RETRY_COUNT;
transport_payload_param.result_cb = report_poweron_callback;

genie_transport_send_payload(&transport_payload_param);

...
```


### 5.2 接收数据
应用程序接收Mesh数据是通过初始化的时候注册的用户回调函数接收的，如下两个Event分别接收SIG Model的数据和Genie的Vendor Model的数据：

- 接收SIG Model的数据
```c
case GENIE_EVT_SIG_MODEL_MSG:    {
    sig_model_msg *p_msg = (sig_model_msg *)p_arg;

    if (p_msg)
    {
        GENIE_LOG_INFO("SIG mesg ElemID(%d)", p_msg->element_id);
    }
}
break;
```


- 接收Vendor Model的数据
```c
case GENIE_EVT_VENDOR_MODEL_MSG:
{
    genie_transport_model_param_t *p_msg = (genie_transport_model_param_t *)p_arg;

    if (p_msg && p_msg->p_model && p_msg->p_model->user_data)
    {
        sig_model_element_state_t *p_elem_state = (sig_model_element_state_t *)p_msg->p_model->user_data;
        GENIE_LOG_INFO("ElemID(%d) TID(%02X)", p_elem_state->element_id, p_msg->tid);
        (void)p_elem_state;
    }
}
break;
```


### 5.3 透传收发数据示例
#### 5.3.1 数据上行（模组发送收到的MCU的数据）
```c

int genie_sal_ble_send_msg(uint8_t element_id, uint8_t *p_data, uint8_t len)
{
    int ret = -1;
    uint8_t i = 0;
    uint8_t seg_count = 0;
    struct bt_mesh_model *p_model = NULL;
    struct net_buf_simple *msg = NULL;
    struct bt_mesh_msg_ctx ctx;
    struct bt_mesh_elem *p_element = NULL;
    genie_transport_model_param_t request_msg;

    if (len > GENIE_HAL_BLE_SEND_MAX_DATA_LEN)
    {
        return -1;
    }

    p_element = bt_mesh_elem_find_by_id(element_id);
    if (p_element == NULL)
    {
        printf("not find element by id:%d\r\n", element_id);
        return -1;
    }

    if ((len >= 4) && (p_data[0] == VENDOR_OP_ATTR_INDICATE || p_data[0] == VENDOR_OP_ATTR_INDICATE_TG))
    {
        seg_count = genie_transport_get_seg_count(len - 4 + 4);
        memset(&request_msg, 0, sizeof(genie_transport_model_param_t));
        request_msg.opid = p_data[0];
        request_msg.tid = p_data[3];
        request_msg.data = (u8_t *)(p_data + 4);
        request_msg.len = len - 4;
        request_msg.p_elem = p_element;
        request_msg.retry_period = GENIE_TRANSPORT_EACH_PDU_TIMEOUT * seg_count;
        request_msg.retry = VENDOR_MODEL_MSG_MAX_RETRY_TIMES;

        ret = genie_transport_send_model(&request_msg);
        if (ret != 0)
        {
            printf("vendor model msg send fail\n");
            return -1;
        }
        return 0;
    }
    else
    {
        ctx.app_idx = 0;
        ctx.net_idx = 0;
        ctx.addr = genie_transport_src_addr_get();//发送的目的地址是上一个接收数据的发送者
        ctx.send_ttl = GENIE_TRANSPORT_DEFAULT_TTL;
        ctx.send_rel = 0;

        msg = NET_BUF_SIMPLE(GENIE_HAL_BLE_SEND_MAX_DATA_LEN + 4);
        if (msg == NULL)
        {
            printf("no mem\n");
            return -1;
        }

        net_buf_simple_init(msg, 0);
        while (i < len)
        {
            net_buf_simple_add_u8(msg, p_data[i]);
            i++;
        }

        if ((p_data[0] & 0xC0) == 0xC0)
        {
            p_model = p_element->vnd_models;
        }
        else
        {
            p_model = p_element->models;
        }

        if (p_model == NULL)
        {
            printf("no model\n");
            return -1;
        }

        if (bt_mesh_model_send(p_model, &ctx, msg, NULL, NULL))
        {
            printf("mesh send fail\n");
            return -1;
        }

        return 0;
    }
}
```


#### 5.3.2 数据下行（模组收到的数据转发给MCU)
```c
int genie_down_msg(genie_down_mesg_type msg_type, uint32_t opcode, void *p_msg)
{
    uint8_t *p_data = NULL;
    uint16_t data_len = 0;
    genie_service_ctx_t *p_context = NULL;

#ifdef CONIFG_GENIE_MESH_USER_CMD
    uint8_t element_id = 0;
    genie_down_msg_t down_msg;
#endif

    p_context = genie_service_get_context();

    if (p_msg == NULL || !p_context || !p_context->event_cb)
    {
        GENIE_LOG_ERR("param err");
        return -1;
    }

    if (GENIE_DOWN_MESG_VENDOR_TYPE == msg_type)
    {
        genie_transport_model_param_t *p_vnd_mesg = (genie_transport_model_param_t *)p_msg;
        data_len = 4 + p_vnd_mesg->len;
        p_data = (uint8_t *)aos_malloc(data_len);
        if (p_data == NULL)
        {
            return -1;
        }

        p_data[0] = p_vnd_mesg->opid;
        p_data[1] = opcode;
        p_data[2] = opcode >> 8;
        p_data[3] = p_vnd_mesg->tid;
        if (p_vnd_mesg->len > 0)
        {
            memcpy(&p_data[4], p_vnd_mesg->data, p_vnd_mesg->len);
        }

#ifdef CONIFG_GENIE_MESH_USER_CMD
        sig_model_element_state_t *p_elem_state = (sig_model_element_state_t *)p_vnd_mesg->p_model->user_data;
        if (p_elem_state)
        {
            element_id = p_elem_state->element_id;
        }
#endif
    }
    else
    {
        sig_model_msg *p_net_buf = (sig_model_msg *)p_msg;

        p_context->event_cb(GENIE_EVT_SIG_MODEL_MSG, (void *)p_msg);

        if (opcode < 0x7F) //one byte opcode
        {
            data_len = 1 + p_net_buf->len;
            p_data = (uint8_t *)aos_malloc(data_len);
            if (p_data == NULL)
            {
                return -1;
            }
            p_data[0] = opcode & 0xFF;
            memcpy(&p_data[1], p_net_buf->data, p_net_buf->len);
        }
        else
        {
            data_len = 2 + p_net_buf->len;
            p_data = (uint8_t *)aos_malloc(data_len);
            if (p_data == NULL)
            {
                return -1;
            }
            p_data[0] = (opcode >> 8) & 0xFF;
            p_data[1] = opcode & 0xFF;
            memcpy(&p_data[2], p_net_buf->data, p_net_buf->len);
        }
#ifdef CONIFG_GENIE_MESH_USER_CMD
        element_id = p_net_buf->element_id;
#endif
    }

#ifdef CONFIG_GENIE_MESH_AT_CMD
    genie_at_cmd_send_data_to_mcu(p_data, data_len);
#endif

#ifdef CONIFG_GENIE_MESH_BINARY_CMD
    genie_bin_cmds_send_data_to_mcu(p_data, data_len);
#endif

#ifdef CONIFG_GENIE_MESH_USER_CMD
    down_msg.len = data_len;
    down_msg.data = p_data;
    down_msg.element_id = element_id;

    p_context->event_cb(GENIE_EVT_DOWN_MSG, (void *)&down_msg);
#endif

    aos_free(p_data);

    return 0;
}
```


### 5.4 GenieEvent说明
#### 5.4.1 GenieEvent
定义在在genie_event.h文件中
```c
typedef enum
{
    GENIE_EVT_NONE = 0,
    GENIE_EVT_SW_RESET,       /* triggered from cloud */
    GENIE_EVT_HW_RESET_START, /* triggered from user */
    GENIE_EVT_BT_READY,
    GENIE_EVT_MESH_READY, // Used to sync device's state with cloud

    GENIE_EVT_SDK_AIS_DISCON = 10,
    GENIE_EVT_SDK_AIS_CONNECT,
    GENIE_EVT_SDK_MESH_PBADV_START,
    GENIE_EVT_SDK_MESH_PBADV_TIMEOUT,
    GENIE_EVT_SDK_MESH_SILENT_START,

    GENIE_EVT_SDK_MESH_PROV_START = 20,
    GENIE_EVT_SDK_MESH_PROV_DATA,
    GENIE_EVT_SDK_MESH_PROV_TIMEOUT,
    GENIE_EVT_SDK_MESH_PROV_SUCCESS,
    GENIE_EVT_SDK_MESH_PROV_FAIL,

    GENIE_EVT_SDK_APPKEY_ADD = 30,
    GENIE_EVT_SDK_APPKEY_DEL,
    GENIE_EVT_SDK_APPKEY_UPDATE,
    GENIE_EVT_SDK_NETKEY_ADD,
    GENIE_EVT_SDK_NETKEY_DEL,
    GENIE_EVT_SDK_NETKEY_UPDATE,

    GENIE_EVT_SDK_HB_SET = 40,
#ifdef CONFIG_BT_MESH_CTRL_RELAY
    GENIE_EVT_SDK_CTRL_RELAY_SET,
#endif
    GENIE_EVT_SDK_SEQ_UPDATE,

    GENIE_EVT_TIMEOUT = 50,
    GENIE_EVT_DOWN_MSG,
    GENIE_EVT_VENDOR_MODEL_MSG,
    GENIE_EVT_SIG_MODEL_MSG,
    GENIE_EVT_USER_TRANS_CYCLE,
    GENIE_EVT_USER_ACTION_DONE
} genie_event_e;
```


#### 5.4.2 SigEvent
定义在sig_model_event.h文件中
```c
typedef enum
{
    SIG_MODEL_EVT_NONE = 0,
    SIG_MODEL_EVT_ANALYZE_MSG,
    SIG_MODEL_EVT_TIME_OUT,
    SIG_MODEL_EVT_DOWN_MSG,
    SIG_MODEL_EVT_ACTION_DONE,
    SIG_MODEL_EVT_INDICATE,

    SIG_MODEL_EVT_DELAY_START = 10,
    SIG_MODEL_EVT_DELAY_END,

#ifdef CONFIG_MESH_MODEL_TRANS
    SIG_MODEL_EVT_TRANS_START,
    SIG_MODEL_EVT_TRANS_CYCLE,
    SIG_MODEL_EVT_TRANS_END,
#endif

    SIG_MODEL_EVT_GENERIC_MESG = 20,
} sig_model_event_e;
```


#### 5.4.3 两个event的处理函数说明
下面两个函数都是递归调用函数，所以如果扩展event需要注意递归深度太深可能导致线程栈溢出
```c
void genie_event(genie_event_e event, void *p_arg)
void sig_model_event(sig_model_event_e event, void *p_arg)
```


#### 5.4.4 日志查看Event值
```

[   0.354]<I>AOSBT sig_model_event:SigE:10
[   0.439]<I>AOSBT genie_event:GenieE:3
[   0.859]<I>AOSBT sig_model_event:SigE:11
[   0.864]<I>AOSBT sig_model_event:SigE:12
[   1.819]<I>AOSBT sig_model_event:SigE:14
[   1.824]<I>AOSBT sig_model_event:SigE:4
[   1.828]<I>AOSBT sig_model_event:SigE:5
```


例如
[   0.354]<I>AOSBT sig_model_event:SigE:10  SigE：10 就是  SIG_MODEL_EVT_DELAY_START = 10,
[   0.439]<I>AOSBT genie_event:GenieE:3 GenieE：3 就是GENIE_EVT_BT_READY,


### 5.5 genie_transport说明
#### 5.5.1 TID（transaction identity）生成算法
TID范围在128-191之间，上电第一个tid采用随机数
```c
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
```


#### 5.5.2 消息重发实现
数据结构和重发函数如下：
```c
typedef struct _genie_transport_model_param_s
{
    struct bt_mesh_elem *p_elem;
    transport_result_cb result_cb;
    uint8_t element_id;
    uint8_t retry;
    uint8_t opid;
    uint8_t tid;
    uint16_t retry_period;
    uint16_t len;
    uint8_t *data;
} genie_transport_model_param_t;

typedef struct _genie_transport_node_s
{
    sys_dnode_t node;
    uint8_t left_retry;
    long long timeout;
    genie_transport_model_param_t msg;
} genie_transport_node_t;


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
        BT_DBG("msg:%p, opid:%d, left:%d", p_msg, p_msg->opid, p_msg_node->left_retry);

        if (p_msg_node->timeout <= k_uptime_get())
        {
            BT_DBG("timeout - msg:%p, opid:%x, left:%d", p_msg, p_msg->opid, p_msg_node->left_retry);
            if (p_msg_node->left_retry-- == 0)
            {
                GENIE_LOG_INFO("TID(%02X) timeout", p_msg_node->msg.tid);
                if (p_msg_node->msg.result_cb)
                {
                    p_msg_node->msg.result_cb(SEND_RESULT_TIMEOUT);
                }
                #ifdef CONFIG_BT_MESH_CTRL_RELAY
                ctrl_relay_open_send();
                #endif
                sys_dlist_remove(p_node);
                transport_msg_node_free((genie_transport_node_t *)p_node);
                break;
            }
            GENIE_LOG_INFO("ReTID(%02X), LR(%d)", p_msg->tid, p_msg_node->left_retry);
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
        BT_DBG("restart retry timer, timeout:%d", nearest);
    }
    else
    {
        k_timer_stop(&retransmit_timer);
        BT_DBG("list empty, stop timer");
    }

    return;
}
```


设计思路：在一个最大为八个节点的双链表管理应用的发送数据，在每次添加节点的时候计算出最快的timeout时间，然后启动timer执行，在每次timeout的时候计算出下一次最快timeout的时间再次启动timer执行。


#### 5.5.3 接收的消息去重
基本思路：缓存最近接收的五个消息的TID，如果在六秒内收到相同TID的消息那么认为是重复的消息，忽略重复的消息。
```c

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
```


### 5.6 主要串口命令说明
| 命令名称 | 命令说明 | 使用参考（示例） |
| --- | --- | --- |
| set_tt | 设备蓝牙Mesh设备三元组 | set_tt 5297793 0c51b11c6ec78b52b803b3bbaae64fba 486e704a5bf6 |
| get_tt | 查看蓝牙Mesh设备三元组 | 无参数 |
| get_info | 查看版本和MAC等信息 | 无参数 |
| reboot | 系统重启 | 无参数 |
| reset | 设备复位 | 无参数 |
| mesg | 通过MESH发送数据 | mesg d4 1 f000 010203 |



### 5.7 mesg命令参数说明：

-  第一个参数d4就是indication发送，其他有D3、CE及CF等；
-  第二个参数是发送模式和重发次数参数
   - 0表示不重发
   - 1-252表示重发次数
   - 253表示使用payload的第一个字节作为时间间隔参数，单位是100ms，例如：mesg d4 253 f000 030201  表示300毫秒发一次0201，mesg d4 253 f000 1e0201是3秒一次0201
   - 254表示收到回复或者发送超时就再次发送
   - 255表示每秒自动发送一次

 

-  第三个参数是接收者地址，必须是四个字符如果设置为0000会使用音箱默认组播地址F000；
-  第四个参数是发送的内容，例如010203就是发送0x01，0x02，0x03因此必须是偶数个0-f之间的字符；



## 6.运行资源
不同方案下资源消耗参考

| Solution | Code size/Bytes | Ro data/Bytes | RW Data/Bytes | BSS/Bytes |
| --- | --- | --- | --- | --- |
| genie_mesh_light_ctl | 19810 | 4975 | 768 | 4130 |
| genie_mesh_node_ctl | 22236 | 6117 | 753 | 4448 |
| genie_mesh_switches | 16336 | 4062 | 600 | 4078 |



## 7.依赖资源
- ble_host
- ble_mesh

## 8.参考文档

- [蓝牙mesh智能家居产品规范](https://help.aliyun.com/document_detail/173319.html?spm=a2c4g.11186623.6.694.77dc41f0KNwhT0)
- [蓝牙设备属性表](https://help.aliyun.com/document_detail/173320.html?spm=a2c4g.11186623.6.694.51385ed8bpGvnn)
- [蓝牙Mesh模组软件规范](https://help.aliyun.com/document_detail/173310.html?spm=a2c4g.11174283.6.689.2d197132x9Gqoi)
- [蓝牙Mesh设备扩展协议](https://help.aliyun.com/document_detail/173311.html?spm=a2c4g.11186623.6.685.6e5c59eev3fdLS)
- [蓝牙mesh设备开发FAQ](https://help.aliyun.com/document_detail/173313.html?spm=a2c4g.11186623.6.686.6fdb14262iF9B3)
