/*
 * Copyright (C) 2018-2020 Alibaba Group Holding Limited
 */

#include <port/mesh_hal_ble.h>

//#include "multi_adv.h"

#define BT_DBG_ENABLED IS_ENABLED(CONFIG_BT_MESH_DEBUG_OTA)
#include "common/log.h"
#include "bt_mesh_custom_log.h"

#include "genie_service.h"
#include "ali_dfu_port.h"

#define DEBUG_AIS_DETAIL_DATA 1

static genie_ais_ctx_t genie_ais_ctx;
extern genie_ota_ctx_t genie_ota_ctx;

static struct bt_gatt_attr _ais_srv_attrs[];

static int _ais_decrypt(uint8_t *payload, uint8_t len)
{
    uint8_t dec[16];

    if (genie_crypto_decrypt(payload, dec) != 0)
    {
        printf("ais decrypt:%s fail\r\n", bt_hex(payload, len));
        return -1;
    }

    memcpy(payload, dec, 16);

    return 0;
}

static void _ais_indicate_rsp(struct bt_conn *conn, const struct bt_gatt_attr *attr, u8_t err, void *p_params)
{
    struct bt_gatt_indicate_params *p_indicate = (struct bt_gatt_indicate_params *)p_params;

    printf("ais rsp err(%d) p_ind(%p)\r\n", err, p_indicate);
    if (p_indicate)
    {
        aos_free(p_indicate);
        p_indicate = NULL;
    }
}

static void _ais_server_indicate(uint8_t msg_id, uint8_t cmd, uint8_t *p_msg, uint16_t len)
{
    int ret = 0;
    ais_pdu_t msg;
    struct bt_gatt_indicate_params *p_indicate = NULL;

    p_indicate = aos_malloc(sizeof(struct bt_gatt_indicate_params));

    if (!p_indicate)
    {
        BT_ERR("no mem");
        return;
    }

    memset(&msg, 0, sizeof(msg));
    if (genie_ais_ctx.state == AIS_STATE_IDLE)
    {
        msg.header.enc = 1;
    }
    else
    {
        msg.header.enc = 0;
    }
    msg.header.msg_id = msg_id;
    msg.header.cmd = cmd;
    msg.header.payload_len = len;
    if (p_msg)
    {
        memcpy(msg.payload, p_msg, len);
    }

    BT_DBG("len %d: %s", len + 4, bt_hex(&msg, len + 4));

    //indicate._req
    p_indicate->attr = &_ais_srv_attrs[6];
    p_indicate->func = _ais_indicate_rsp;
    p_indicate->data = &msg;
    p_indicate->len = len + 4;

    if (genie_ais_ctx.p_conn)
    {
#ifdef DEBUG_AIS_DETAIL_DATA
        printf("ais ind msgid:%d cmd:%02x p_ind:%p len:%d data:%s\r\n", msg_id, cmd, p_indicate, len, bt_hex(&msg, len + 4));
#else
        printf("ais ind msgid:%d cmd:%02x p_ind:%p len:%d\r\n", msg_id, cmd, p_indicate, len);
#endif

        ret = bt_gatt_indicate(genie_ais_ctx.p_conn, p_indicate);
        if (ret < 0)
        {
            printf("ais bt_gatt_indicate err:%d\r\n", ret);
            aos_free(p_indicate);
        }
    }
    else
    {
        printf("ais not connect\r\n");
        aos_free(p_indicate);
    }
}

void genie_ais_notify(uint8_t msg_id, uint8_t cmd, uint8_t *p_msg, uint16_t len)
{
    ais_pdu_t msg;

    memset(&msg, 0, sizeof(msg));
    if (genie_ais_ctx.state >= AIS_STATE_IDLE && genie_ais_ctx.state <= AIS_STATE_REBOOT)
    {
        msg.header.enc = 1;
    }
    else
    {
        msg.header.enc = 0;
    }

    msg.header.cmd = cmd;
    msg.header.msg_id = msg_id;
    msg.header.payload_len = len;
    if (p_msg)
    {
        memcpy(msg.payload, p_msg, len);
    }

    BT_DBG("len %d: %s", len + 4, bt_hex(&msg, len + 4));

    if (genie_ais_ctx.p_conn)
    {
#ifdef DEBUG_AIS_DETAIL_DATA
        printf("ais notify msgid:%d cmd:%02x len:%d data:%s\r\n", msg_id, cmd, len, bt_hex(&msg, len + 4));
#else
        printf("ais notify msgid:%d cmd:%02x len:%d\r\n", msg_id, cmd, len);
#endif

        bt_gatt_notify(genie_ais_ctx.p_conn, &_ais_srv_attrs[11], &msg, len + 4);
    }
}

static void handle_ais_disconnect(void)
{
    if (genie_ais_ctx.p_conn)
    {
        BT_DBG("ais handle disconn:%p\r\n", genie_ais_ctx.p_conn);
        bt_conn_disconnect(genie_ais_ctx.p_conn, BT_HCI_ERR_REMOTE_USER_TERM_CONN);
        genie_ais_ctx.p_conn = NULL;
    }
}

void genie_ais_disconnect(uint8_t reason)
{
    BT_DBG("ais disconn 0x%x, state %d\r\n", reason, genie_ais_ctx.state);
    if (genie_ais_ctx.state != AIS_STATE_REBOOT)
    {
        k_timer_stop(&genie_ais_ctx.state_update_timer);
        handle_ais_disconnect();

        genie_ais_ctx.state = AIS_STATE_DISCON;
        /* Flash is dirty, need erase */
        if (genie_ota_ctx.flash_clean == 1 &&
            genie_ota_ctx.ota_ready == 0)
        {
            erase_dfu_flash();
            genie_ota_ctx.flash_clean = 0;
        }
        /* restart adv */
        genie_event(GENIE_EVT_SDK_AIS_DISCON, NULL);
    }
    else
    {
        if (genie_ota_ctx.ota_flag != OTA_FLAG_SILENT || genie_sal_ota_is_allow_reboot())
        {
            //clear image change
            BT_WARN("OTA Reboot!");
            genie_storage_delete_reliable(GFI_OTA_IMAGE_ID);
            dfu_reboot();
        }
    }
}

static void genie_ais_state_update(void)
{
    if (genie_ais_ctx.state != AIS_STATE_OTA)
    {
        BT_DBG("ais update state:%d", genie_ais_ctx.state);
    }

    if (genie_ais_ctx.p_conn == NULL)
    {
        BT_WARN("proxy is disconnected");
        return;
    }

    switch (genie_ais_ctx.state)
    {
    case AIS_STATE_DISCON:
        k_timer_stop(&genie_ais_ctx.state_update_timer);
        handle_ais_disconnect();
        break;
    case AIS_STATE_CONNECT:
    case AIS_STATE_IDLE:
        //no disconnect after one minute
        //k_timer_start(&genie_ais_ctx.state_update_timer, AIS_OTA_DISCONN_TIMEOUT);
        break;
    case AIS_STATE_AUTH:
        k_timer_start(&genie_ais_ctx.state_update_timer, AIS_OTA_AUTH_TIMEOUT);
        break;
    case AIS_STATE_OTA:
        k_timer_start(&genie_ais_ctx.state_update_timer, AIS_OTA_REPORT_TIMEOUT);
        break;
    case AIS_STATE_REBOOT:
        k_timer_start(&genie_ais_ctx.state_update_timer, AIS_OTA_REBOOT_TIMEOUT);
        k_timer_start(&genie_ais_ctx.disconnect_timer, AIS_DISCONNECT_TIMEOUT);
        break;
    default:
        break;
    }
}

static void state_update_timer_cb(void *p_timer, void *args)
{
    BT_DBG(" %d", genie_ais_ctx.state);
    switch (genie_ais_ctx.state)
    {
    case AIS_STATE_CONNECT:
    case AIS_STATE_IDLE:
    {
        genie_ais_ctx.state = AIS_STATE_DISCON;
    }
    break;
    case AIS_STATE_AUTH:
    {
        genie_ais_reset();
        genie_ais_ctx.state = AIS_STATE_CONNECT;
    }
    break;
    case AIS_STATE_OTA:
    {
        if (genie_ota_ctx.err_count++ >= OTA_RECV_MAX_ERR_COUNT)
        {
            BT_ERR("OTA failed");
            genie_ais_ctx.state = AIS_STATE_IDLE;
            genie_ota_ctx.rx_len = 0;
            _ais_server_indicate(0, AIS_RESP_ERR, NULL, 0);
        }
        else
        {
            genie_ota_status_report();
        }
    }
    break;
    case AIS_STATE_REBOOT:
    {
        printf("ais ota update done,do reboot ...\r\n");
        dfu_reboot();
    }
    break;
    default:
        break;
    }
    genie_ais_state_update();
}

void genie_ais_connect(struct bt_conn *p_conn)
{
    BT_DBG("ais connect state:%d conn:%p", genie_ais_ctx.state, p_conn);
    if (genie_ais_ctx.state != AIS_STATE_REBOOT)
    {
        BT_DBG("status %d", genie_ais_ctx.state);
        genie_ais_ctx.p_conn = p_conn;
        genie_ais_ctx.state = AIS_STATE_CONNECT;
        genie_ais_state_update();
        genie_event(GENIE_EVT_SDK_AIS_CONNECT, NULL);
    }
}

static void _ais_dis_timer_cb(void *p_timer, void *args)
{
    printf("ais timeout disconnect\r\n");
    handle_ais_disconnect();
}

static bool _ais_scrt_random(uint8_t msg_id, ais_scrt_random_t *p_scrt_random)
{
    uint8_t cipher[16];

    genie_ais_ctx.state = AIS_STATE_AUTH;
    genie_ais_get_cipher(p_scrt_random->random, cipher);
    _ais_server_indicate(msg_id, AIS_SCRT_CIPHER, cipher, 16);

    return true;
}

static bool _ais_scrt_result(uint8_t msg_id, ais_scrt_result_t *p_scrt_result)
{
    uint8_t ack = 0;

    if (p_scrt_result->result == 1)
    {
        genie_ais_reset();
        genie_ais_ctx.state = AIS_STATE_CONNECT;
    }
    else
    {
        genie_ais_ctx.state = AIS_STATE_IDLE;
    }

    _ais_server_indicate(msg_id, AIS_SCRT_ACK, &ack, 1);

    return true;
}

static bool _ais_link_ack(uint8_t msg_id, ais_scrt_result_t *p_scrt_result)
{
    uint8_t plaine_data[GENIE_CRYPTO_UNIT_SIZE];
    uint8_t encrypt_data[GENIE_CRYPTO_UNIT_SIZE];

    if (p_scrt_result->result == 0)
    {
        genie_ais_reset();
        genie_ais_ctx.state = AIS_STATE_CONNECT;
    }
    memset(plaine_data, 0x0F, sizeof(plaine_data));
    plaine_data[0] = 1;

    genie_crypto_encrypt(plaine_data, encrypt_data);
    _ais_server_indicate(msg_id, AIS_LINK_ACK, encrypt_data, GENIE_CRYPTO_UNIT_SIZE);

    return true;
}

static bool _ais_msg_check_header(ais_header_t *p_msg_header)
{
    //check seq & total, in ota case, the seq & total must be 0
    if (p_msg_header->total_frame != 0 || p_msg_header->seq != 0 || p_msg_header->ver != 0 || p_msg_header->seq > p_msg_header->total_frame)
    {
        BT_ERR("fail %s", bt_hex(p_msg_header, sizeof(ais_header_t)));
        return false;
    }
    return true;
}

static void _ais_server_msg_handle(struct bt_conn *p_conn, ais_pdu_t *p_msg, uint16_t len)
{
    bool timer_refresh = false;

    if (p_msg->header.cmd != AIS_OTA_DATA && !_ais_msg_check_header((ais_header_t *)p_msg))
    {
        printf("ais invalid msg, ignore");
    }

    if (p_msg->header.cmd != AIS_OTA_DATA)
    {
#ifdef DEBUG_AIS_DETAIL_DATA
        printf("ais cmd:0x%02x state:%d len:%d data:%s\r\n", p_msg->header.cmd, genie_ais_ctx.state, len, bt_hex(p_msg, len));
#else
        printf("ais cmd:0x%02x state:%d len:%d\r\n", p_msg->header.cmd, genie_ais_ctx.state, len);
#endif
    }

    switch (p_msg->header.cmd)
    {
    case AIS_SCRT_RANDOM:
    {
        //len = 4+16
        if (len == 20) //At any state we can negotiation the cipher again
        {
            timer_refresh = _ais_scrt_random(p_msg->header.msg_id,
                                             (ais_scrt_random_t *)p_msg->payload);
        }
    }
    break;

    case AIS_SCRT_RESULT:
    {
        //len = 4+1
        if (len == 5) //Allow duplicating of this message
        {
            k_timer_stop(&genie_ais_ctx.state_update_timer);
            timer_refresh = _ais_scrt_result(p_msg->header.msg_id, (ais_scrt_result_t *)p_msg->payload);
        }
    }
    break;

    case AIS_LINK_STATUS:
    {
        //len = 4+16
        if (len == 20) //Allow duplicating of this message
        {
            if (0 == _ais_decrypt(p_msg->payload, 16))
            {
                timer_refresh = _ais_link_ack(p_msg->header.msg_id, (ais_scrt_result_t *)p_msg->payload);
            }
        }
    }
    break;

    case AIS_OTA_VER_REQ:
    {
        if (len == 20) //This is encrypted data
        {
            if (0 == _ais_decrypt(p_msg->payload, 16))
            {
                timer_refresh = genie_ota_handle_version_request(p_msg->header.msg_id, (ais_ota_ver_req_t *)p_msg->payload, 1);
            }
        }
        else if (len == 5)
        {
            timer_refresh = genie_ota_handle_version_request(p_msg->header.msg_id, (ais_ota_ver_req_t *)p_msg->payload, 0);
        }
    }
    break;

    case AIS_OTA_FIRMWARE_REQ:
    {
        //len = 4+16
        if (len == 20) //Encrypted data
        {
            if (0 == _ais_decrypt(p_msg->payload, 16))
            {
                timer_refresh = genie_ota_handle_update_request(p_msg->header.msg_id, (ais_ota_upd_req_t *)p_msg->payload, 1);
            }
        }
        else if (len == 16) //Plain data
        {
            timer_refresh = genie_ota_handle_update_request(p_msg->header.msg_id, (ais_ota_upd_req_t *)p_msg->payload, 0);
        }
    }
    break;

    case AIS_OTA_DATA:
    {
        if ((len == sizeof(ais_header_t) + p_msg->header.payload_len) && p_msg->header.ver == 0)
        {
            timer_refresh = genie_ota_parse_pdu(p_msg);
        }
    }
    break;

    case AIS_OTA_CHECK_REQ:
    {
        if (len == 20 && genie_ais_ctx.state == AIS_STATE_OTA)
        {
            if (0 == _ais_decrypt(p_msg->payload, 16))
            {
                timer_refresh = genie_ota_check_firmware(p_msg->header.msg_id, (ais_ota_check_req_t *)p_msg->payload);
            }
        }
    }
    break;

    default:
    {
        /* recv some unsupport cmd, just return */
        BT_WARN("ais unsupport cmd %x\r\n", p_msg->header.cmd);
    }
        return;
    }

    if (timer_refresh)
    {
        genie_ais_state_update();
    }
    else
    {
        if (genie_ais_ctx.state != AIS_STATE_OTA)
        {
            printf("ais err state:%d cmd:0x%02x len:%d\r\n", genie_ais_ctx.state, p_msg->header.cmd, len);
            _ais_server_indicate(p_msg->header.msg_id, AIS_RESP_ERR, NULL, 0);
        }
    }
}

static ssize_t _ais_server_read(struct bt_conn *p_conn, const struct bt_gatt_attr *p_attr,
                                void *buf, u16_t len, u16_t offset)
{
    u16_t *value = p_attr->user_data;

    BT_DBG("len %d: %s", len, bt_hex(buf, len));

    return bt_gatt_attr_read(p_conn, p_attr, buf, len, offset, value, sizeof(*value));
}

static ssize_t _ais_service_write(struct bt_conn *p_conn, const struct bt_gatt_attr *p_attr,
                                  const void *p_buf, u16_t len, u16_t offset, u8_t flags)
{
    //BT_DBG("len %d: %s", len, bt_hex(p_buf, len));

    if (len != 0)
    {
        _ais_server_msg_handle(p_conn, (ais_pdu_t *)p_buf, len);
    }

    return len;
}

static ssize_t _ais_service_write_nr(struct bt_conn *p_conn, const struct bt_gatt_attr *p_attr,
                                     const void *p_buf, u16_t len, u16_t offset, u8_t flags)
{
    //BT_DBG("len %d: %s", len, bt_hex(p_buf, len));
    return _ais_service_write(p_conn, p_attr, p_buf, len, offset, flags);
}

static void _ais_service_ccc_cfg_changed(const struct bt_gatt_attr *p_attr, uint16_t value)
{
}

/* AIS OTA Service Declaration */
static struct bt_gatt_attr _ais_srv_attrs[] = {
    BT_GATT_PRIMARY_SERVICE(AIS_SERVICE_UUID),

    BT_GATT_CHARACTERISTIC(AIS_READ_UUID, BT_GATT_CHRC_READ, BT_GATT_PERM_READ, _ais_server_read, NULL, NULL),

    BT_GATT_CHARACTERISTIC(AIS_WRITE_UUID, BT_GATT_CHRC_READ | BT_GATT_CHRC_WRITE, BT_GATT_PERM_WRITE,
                           NULL, _ais_service_write, NULL),

    BT_GATT_CHARACTERISTIC(AIS_INDICATE_UUID, BT_GATT_CHRC_READ | BT_GATT_CHRC_INDICATE,
                           BT_GATT_PERM_READ | BT_GATT_PERM_WRITE, _ais_server_read, NULL, NULL),

    BT_GATT_CCC(_ais_service_ccc_cfg_changed, BT_GATT_PERM_READ | BT_GATT_PERM_WRITE),

    BT_GATT_CHARACTERISTIC(AIS_WRITE_WO_RESP_UUID, BT_GATT_CHRC_READ | BT_GATT_CHRC_WRITE_WITHOUT_RESP,
                           BT_GATT_PERM_WRITE, NULL, _ais_service_write_nr, NULL),

    BT_GATT_CHARACTERISTIC(AIS_NOTIFY_UUID, BT_GATT_CHRC_READ | BT_GATT_CHRC_NOTIFY,
                           BT_GATT_PERM_READ | BT_GATT_PERM_WRITE, _ais_server_read, NULL, NULL),

    BT_GATT_CCC(_ais_service_ccc_cfg_changed, BT_GATT_PERM_READ | BT_GATT_PERM_WRITE),
};

static struct bt_gatt_service _ais_srv = BT_GATT_SERVICE(_ais_srv_attrs);

#ifdef CONFIG_BT_MESH_MULTIADV
static u8_t g_ais_adv_data[14] = {
    0xa8, 0x01,                        //taobao
    0x85,                              //vid & sub
    0x15,                              //FMSK
    0x15, 0x11, 0x22, 0x33,            //PID
    0xAA, 0xBB, 0xCC, 0x11, 0x22, 0x33 //MAC
};

struct bt_data g_ais_adv[] = {
    BT_DATA_BYTES(BT_DATA_FLAGS, (BT_LE_AD_GENERAL | BT_LE_AD_NO_BREDR)),
    BT_DATA_BYTES(BT_DATA_UUID16_SOME, 0xB3, 0xFE),
    BT_DATA(BT_DATA_MANUFACTURER_DATA, g_ais_adv_data, 14),
};

static const struct bt_data g_ais_sd[] = {
    BT_DATA_BYTES(BT_DATA_FLAGS, (BT_LE_AD_GENERAL | BT_LE_AD_NO_BREDR)),
    BT_DATA(BT_DATA_NAME_COMPLETE, CONFIG_BT_DEVICE_NAME, (sizeof(CONFIG_BT_DEVICE_NAME) - 1)),
};

struct bt_le_adv_param fast_adv_param = {
    .options = (BT_LE_ADV_OPT_CONNECTABLE | BT_LE_ADV_OPT_ONE_TIME),
    .interval_min = BT_GAP_ADV_FAST_INT_MIN_2,
    .interval_max = BT_GAP_ADV_FAST_INT_MAX_2,
    .own_addr = NULL,
};
#endif

int genie_ais_state_set(uint8_t state)
{
    genie_ais_ctx.state = state;

    return 0;
}

uint8_t genie_ais_state_get(void)
{
    return genie_ais_ctx.state;
}

#ifdef CONFIG_BT_MESH_MULTIADV
int g_multiadv_instant_id;
static int multi_adv_init(void)
{
    int err = -1;

    genie_crypto_adv_create(g_ais_adv_data, 0);

    err = bt_le_multi_adv_start(&fast_adv_param, g_ais_adv, ARRAY_SIZE(g_ais_adv), g_ais_sd, ARRAY_SIZE(g_ais_sd), &g_multiadv_instant_id);
    if (err)
    {
        BT_ERR("Multi Advertising failed to start (err %d)\n", err);
    }

    return err;
}
#endif

int genie_ais_init(void)
{
#ifdef CONFIG_BT_MESH_MULTIADV
    multi_adv_init();
#endif

    bt_gatt_service_register(&_ais_srv);

    k_timer_init(&genie_ais_ctx.state_update_timer, state_update_timer_cb, NULL);
    k_timer_init(&genie_ais_ctx.disconnect_timer, _ais_dis_timer_cb, NULL);

    printf("ais init\r\n");

    return 0;
}

int genie_ais_pre_init(void)
{
    printf("ais pre init\r\n");

    memset(&genie_ais_ctx, 0, sizeof(genie_ais_ctx));

    return 0;
}
