/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <breeze_hal_ble.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>
#include <bluetooth/conn.h>
#include <bluetooth/uuid.h>
#include <bluetooth/gatt.h>
#include <aos/aos.h>
#include <aos/list.h>
#include <dis.h>

struct bt_conn *g_conn       = NULL;
ais_bt_init_t * bt_init_info = NULL;

#define BT_UUID_AIS_SERVICE BT_UUID_DECLARE_16(BLE_UUID_AIS_SERVICE)
#define BT_UUID_AIS_RC BT_UUID_DECLARE_16(BLE_UUID_AIS_RC)
#define BT_UUID_AIS_WC BT_UUID_DECLARE_16(BLE_UUID_AIS_WC)
#define BT_UUID_AIS_IC BT_UUID_DECLARE_16(BLE_UUID_AIS_IC)
#define BT_UUID_AIS_WWNRC BT_UUID_DECLARE_16(BLE_UUID_AIS_WWNRC)
#define BT_UUID_AIS_NC BT_UUID_DECLARE_16(BLE_UUID_AIS_NC)

static struct bt_gatt_ccc_cfg ais_ic_ccc_cfg[BT_GATT_CCC_MAX] = {};
static struct bt_gatt_ccc_cfg ais_nc_ccc_cfg[BT_GATT_CCC_MAX] = {};

void ble_disconnect(uint8_t reason)
{
    uint8_t zreason = 0;

    if (!g_conn) {
        return;
    }

    switch (reason) {
        case AIS_BT_REASON_REMOTE_USER_TERM_CONN:
            zreason = BT_HCI_ERR_REMOTE_USER_TERM_CONN;
            break;
        default:
            zreason = BT_HCI_ERR_UNSPECIFIED;
            break;
    }

    bt_conn_disconnect(g_conn, zreason);
}

static void connected(struct bt_conn *conn, uint8_t err)
{
    if (err) {
        printf("Connection failed (err %u)\n", err);
    } else {
        printf("Connected\n");
        g_conn = conn;
        if (bt_init_info && (bt_init_info->on_connected)) {
            bt_init_info->on_connected();
        }
    }
}

static void disconnected(struct bt_conn *conn, u8_t reason)
{
    printf("Disconnected (reason %u)\n", reason);
    g_conn = NULL;
    if (bt_init_info && (bt_init_info->on_disconnected)) {
        bt_init_info->on_disconnected();
    }
}

static void ais_nc_ccc_cfg_changed(const struct bt_gatt_attr *attr,
                                   uint16_t                   value)
{
    ais_ccc_value_t val;

    switch (value) {
        case BT_GATT_CCC_NOTIFY:
            printf("CCC cfg changed to NOTIFY (%d).\r\n", value);
            val = AIS_CCC_VALUE_NOTIFY;
            break;
        default:
            printf("%s CCC cfg changed to %d.\r\n", __func__, value);
            val = AIS_CCC_VALUE_NONE;
            break;
    }

    if (bt_init_info && bt_init_info->nc.on_ccc_change) {
        bt_init_info->nc.on_ccc_change(val);
    }
}

static void ais_ic_ccc_cfg_changed(const struct bt_gatt_attr *attr,
                                   uint16_t                   value)
{
    ais_ccc_value_t val;

    switch (value) {
        case BT_GATT_CCC_INDICATE:
            printf("CCC cfg changed to INDICATE (%d).\r\n", value);
            val = AIS_CCC_VALUE_INDICATE;
            break;
        default:
            printf("%s CCC cfg changed to %d.\r\n", __func__, value);
            val = AIS_CCC_VALUE_NONE;
            break;
    }

    if (bt_init_info && bt_init_info->ic.on_ccc_change) {
        bt_init_info->ic.on_ccc_change(val);
    }
}

static struct bt_conn_cb conn_callbacks = {
    .connected    = connected,
    .disconnected = disconnected,
};

static ssize_t read_ais_rc(struct bt_conn *           conn,
                           const struct bt_gatt_attr *attr, void *buf,
                           uint16_t len, uint16_t offset)
{
    ssize_t ret = 0;

    printf("%s length to read: %d, offset: %d\r\n", __func__, len, offset);
    if (bt_init_info && bt_init_info->rc.on_read) {
        ret = bt_init_info->rc.on_read(buf, len);
    }

    return ret;
}

static ssize_t read_ais_wc(struct bt_conn *           conn,
                           const struct bt_gatt_attr *attr, void *buf,
                           uint16_t len, uint16_t offset)
{
    ssize_t ret = 0;

    printf("%s length to read: %d, offset: %d\r\n", __func__, len, offset);
    if (bt_init_info && bt_init_info->wc.on_read) {
        ret = bt_init_info->wc.on_read(buf, len);
    }

    return ret;
}

static ssize_t write_ais_wc(struct bt_conn *           conn,
                            const struct bt_gatt_attr *attr, const void *buf,
                            uint16_t len, uint16_t offset, uint8_t flags)
{
    ssize_t ret = 0;

    if (bt_init_info && bt_init_info->wc.on_write) {
        ret = bt_init_info->wc.on_write(buf, len);
    }

    return ret;
}

static ssize_t read_ais_ic(struct bt_conn *           conn,
                           const struct bt_gatt_attr *attr, void *buf,
                           uint16_t len, uint16_t offset)
{
    ssize_t ret = 0;

    printf("%s length to read: %d, offset: %d\r\n", __func__, len, offset);
    if (bt_init_info && bt_init_info->ic.on_read) {
        ret = bt_init_info->ic.on_read(buf, len);
    }

    return ret;
}

static ssize_t read_ais_wwnrc(struct bt_conn *           conn,
                              const struct bt_gatt_attr *attr, void *buf,
                              uint16_t len, uint16_t offset)
{
    ssize_t ret = 0;

    printf("%s length to read: %d, offset: %d\r\n", __func__, len, offset);
    if (bt_init_info && bt_init_info->wwnrc.on_read) {
        ret = bt_init_info->wwnrc.on_read(buf, len);
    }

    return ret;
}

static ssize_t write_ais_wwnrc(struct bt_conn *           conn,
                               const struct bt_gatt_attr *attr, const void *buf,
                               uint16_t len, uint16_t offset, uint8_t flags)
{
    ssize_t ret = 0;

    if (bt_init_info && bt_init_info->wwnrc.on_write) {
        ret = bt_init_info->wwnrc.on_write(buf, len);
    }

    return ret;
}

static ssize_t read_ais_nc(struct bt_conn *           conn,
                           const struct bt_gatt_attr *attr, void *buf,
                           uint16_t len, uint16_t offset)
{
    ssize_t ret = 0;

    printf("%s length to read: %d, offset: %d\r\n", __func__, len, offset);
    if (bt_init_info && bt_init_info->nc.on_read) {
        ret = bt_init_info->nc.on_read(buf, len);
    }

    return ret;
}

static struct bt_gatt_attr *  ais_attrs = NULL;
static struct bt_gatt_service ais_svc;

struct bt_uuid *bt_prisvc_uuid = BT_UUID_GATT_PRIMARY;
struct bt_uuid *bt_chrc_uuid   = BT_UUID_GATT_CHRC;
struct bt_uuid *bt_ccc_uuid    = BT_UUID_GATT_CCC;

static int setup_ais_service_attr(struct bt_gatt_attr *attr,
                                  struct bt_uuid *     uuid)
{
    attr->uuid      = bt_prisvc_uuid;
    attr->perm      = BT_GATT_PERM_READ;
    attr->read      = bt_gatt_attr_read_service;
    attr->user_data = (void *)uuid;
    return 0;
}

static int setup_ais_char_attr(struct bt_gatt_attr *attr, struct bt_uuid *uuid,
                               uint8_t prop)
{
    struct bt_gatt_chrc *chrc = NULL;

    chrc = (struct bt_gatt_chrc *)aos_malloc(sizeof(struct bt_gatt_chrc));
    if (!chrc) {
        printf("%s malloc failed\r\n", __func__);
        return -1;
    }

    chrc->uuid       = uuid;
    chrc->properties = prop;

    attr->uuid      = bt_chrc_uuid;
    attr->perm      = BT_GATT_PERM_READ;
    attr->read      = bt_gatt_attr_read_chrc;
    attr->user_data = (void *)chrc;

    return 0;
}

static int setup_ais_char_desc_attr(struct bt_gatt_attr *attr,
                                    struct bt_uuid *uuid, uint8_t perm,
                                    void *read, void *write, void *userdata)
{
    attr->uuid      = uuid;
    attr->perm      = perm;
    attr->read      = read;
    attr->write     = write;
    attr->user_data = userdata;
    return 0;
}

static int setup_ais_char_ccc_attr(struct bt_gatt_attr *   attr,
                                   struct bt_gatt_ccc_cfg *cfg, size_t cfg_len,
                                   void *cfg_handler)
{
    struct _bt_gatt_ccc *ccc = NULL;

    ccc = (struct _bt_gatt_ccc *)aos_malloc(sizeof(struct _bt_gatt_ccc));
    if (!ccc) {
        printf("%s malloc failed.\r\n", __func__);
        return -1;
    }

    ccc->cfg         = cfg;
    ccc->cfg_len     = cfg_len;
    ccc->cfg_changed = cfg_handler;

    attr->uuid      = bt_ccc_uuid;
    attr->perm      = BT_GATT_PERM_READ | BT_GATT_PERM_WRITE;
    attr->read      = bt_gatt_attr_read_ccc;
    attr->write     = bt_gatt_attr_write_ccc;
    attr->user_data = (void *)ccc;

    return 0;
}

enum
{
    SVC_ATTR_IDX = 0,
    RC_CHRC_ATTR_IDX,
    RC_DESC_ATTR_IDX,
    WC_CHRC_ATTR_IDX,
    WC_DESC_ATTR_IDX,
    IC_CHRC_ATTR_IDX,
    IC_DESC_ATTR_IDX,
    IC_CCC_ATTR_IDX,
    WWNRC_CHRC_ATTR_IDX,
    WWNRC_DESC_ATTR_IDX,
    NC_CHRC_ATTR_IDX,
    NC_DESC_ATTR_IDX,
    NC_CCC_ATTR_IDX,
    /* Add more entry here if necessary */
    AIS_ATTR_NUM,
};

ais_err_t ble_stack_init(ais_bt_init_t *info)
{
    int              err;
    uint32_t         attr_cnt = AIS_ATTR_NUM, size;
    ais_char_init_t *c;

    bt_init_info = info;

    hci_driver_init();
    ais_ota_bt_storage_init();

    err = bt_enable(NULL);
    if (err) {
        printf("Bluetooth init failed (err %d)\n", err);
        return AIS_ERR_STACK_FAIL;
    }
    printf("Bluetooth init succeed.\n");

    size      = attr_cnt * sizeof(struct bt_gatt_attr);
    ais_attrs = (struct bt_gatt_attr *)aos_malloc(size);
    if (!ais_attrs) {
        printf("%s %d memory allocate failed.\r\n", __func__, __LINE__);
        return AIS_ERR_MEM_FAIL;
    }

    memset(ais_attrs, 0, size);

    /* AIS primary service */
    setup_ais_service_attr(&ais_attrs[SVC_ATTR_IDX], info->uuid_svc);

    /* rc */
    c = &(info->rc);
    setup_ais_char_attr(&ais_attrs[RC_CHRC_ATTR_IDX], c->uuid, c->prop);
    setup_ais_char_desc_attr(&ais_attrs[RC_DESC_ATTR_IDX], c->uuid, c->perm,
                             read_ais_rc, NULL, NULL);

    /* wc */
    c = &(info->wc);
    setup_ais_char_attr(&ais_attrs[WC_CHRC_ATTR_IDX], c->uuid, c->prop);
    setup_ais_char_desc_attr(&ais_attrs[WC_DESC_ATTR_IDX], c->uuid, c->perm,
                             read_ais_wc, write_ais_wc, NULL);

    /* ic */
    c = &(info->ic);
    setup_ais_char_attr(&ais_attrs[IC_CHRC_ATTR_IDX], c->uuid, c->prop);
    setup_ais_char_desc_attr(&ais_attrs[IC_DESC_ATTR_IDX], c->uuid, c->perm,
                             read_ais_ic, NULL, NULL);
    setup_ais_char_ccc_attr(&ais_attrs[IC_CCC_ATTR_IDX], ais_ic_ccc_cfg,
                            sizeof(ais_ic_ccc_cfg) / sizeof(ais_ic_ccc_cfg[0]),
                            ais_ic_ccc_cfg_changed);

    /* wwnrc */
    c = &(info->wwnrc);
    setup_ais_char_attr(&ais_attrs[WWNRC_CHRC_ATTR_IDX], c->uuid, c->prop);
    setup_ais_char_desc_attr(&ais_attrs[WWNRC_DESC_ATTR_IDX], c->uuid, c->perm,
                             read_ais_wwnrc, write_ais_wwnrc, NULL);

    /* nc */
    c = &(info->nc);
    setup_ais_char_attr(&ais_attrs[NC_CHRC_ATTR_IDX], c->uuid, c->prop);
    setup_ais_char_desc_attr(&ais_attrs[NC_DESC_ATTR_IDX], c->uuid, c->perm,
                             read_ais_nc, NULL, NULL);
    setup_ais_char_ccc_attr(&ais_attrs[NC_CCC_ATTR_IDX], ais_nc_ccc_cfg,
                            sizeof(ais_nc_ccc_cfg) / sizeof(ais_nc_ccc_cfg[0]),
                            ais_nc_ccc_cfg_changed);

    memset(&ais_svc, 0, sizeof(ais_svc));
    ais_svc.attrs      = ais_attrs;
    ais_svc.attr_count = attr_cnt;

    bt_conn_cb_register(&conn_callbacks);
    bt_gatt_service_register(&ais_svc);
    dis_init("AIS", "AliOSThings");

    return AIS_ERR_SUCCESS;
}

ais_err_t ble_stack_deinit()
{
    if (ais_attrs) {
        aos_free(ais_attrs);
    }

    /* Free other memory here when necessary. */

    return AIS_ERR_SUCCESS;
}

ais_err_t ble_send_notification(uint8_t *p_data, uint16_t length)
{
    return bt_gatt_notify(NULL, &ais_attrs[NC_DESC_ATTR_IDX],
                          (const void *)p_data, length);
}

slist_t params_list;

typedef struct bt_gatt_indicate_param_s
{
    slist_t                         next;
    struct bt_gatt_indicate_params *ind_params;
} bt_gatt_indicate_param_t;

static void indicate_cb(struct bt_conn *conn, const struct bt_gatt_attr *attr,
                        uint8_t err)
{
    bt_gatt_indicate_param_t *param;

    param = slist_first_entry(&params_list, bt_gatt_indicate_param_t, next);
    if (param) {
        aos_free(param->ind_params);
        slist_del(&param->next, &params_list);
        aos_free(param);
    }
}

ais_err_t ble_send_indication(uint8_t *p_data, uint16_t length)
{
    int                             err;
    struct bt_gatt_indicate_params *ind_params;
    bt_gatt_indicate_param_t *      param;

    param = aos_malloc(sizeof(bt_gatt_indicate_param_t));
    if (param == NULL) {
        return AIS_ERR_MEM_FAIL;
    }

    ind_params = aos_malloc(sizeof(struct bt_gatt_indicate_params));
    if (ind_params == NULL) {
        aos_free(param);
        return AIS_ERR_MEM_FAIL;
    }

    param->ind_params = ind_params;
    slist_add_tail(&param->next, &params_list);

    ind_params->attr = &ais_attrs[IC_DESC_ATTR_IDX];
    ind_params->func = indicate_cb;
    ind_params->data = p_data;
    ind_params->len  = length;

    err = bt_gatt_indicate(NULL, ind_params);

    if (err) {
        aos_free(param->ind_params);
        slist_del(&param->next, &params_list);
        aos_free(param);
        return AIS_ERR_GATT_INDICATE_FAIL;
    } else {
        return AIS_ERR_SUCCESS;
    }
}

static struct bt_data ad[3] = {
    BT_DATA_BYTES(BT_DATA_FLAGS, (BT_LE_AD_GENERAL | BT_LE_AD_NO_BREDR)),
    BT_DATA_BYTES(BT_DATA_UUID16_ALL, 0x0a, 0x18),
};

static const struct bt_data sd[] = {
    BT_DATA(BT_DATA_NAME_COMPLETE, CONFIG_BT_DEVICE_NAME,
            sizeof(CONFIG_BT_DEVICE_NAME) - 1),
};

ais_err_t ble_advertising_start(ais_adv_init_t *adv)
{
    int            err;
    uint8_t        flag = 0, dis[] = { 0x0a, 0x18 }, ad_len = 3, sd_len = 1;
    struct bt_data ad[ad_len];
    struct bt_data sd[sd_len];

    if (adv->flag & AIS_AD_GENERAL) {
        flag |= BT_LE_AD_GENERAL;
    }
    if (adv->flag & AIS_AD_NO_BREDR) {
        flag |= BT_LE_AD_NO_BREDR;
    }
    if (!flag) {
        printf("Invalid adv flag.\r\n");
        return AIS_ERR_INVALID_ADV_DATA;
    }

    ad[0].type     = BT_DATA_FLAGS;
    ad[0].data     = &flag;
    ad[0].data_len = 1;

    ad[1].type     = BT_DATA_UUID16_ALL;
    ad[1].data     = dis;
    ad[1].data_len = sizeof(dis);

    if (adv->vdata.len != 0) {
        ad[2].type     = 0xFF;
        ad[2].data_len = adv->vdata.len;
        ad[2].data     = adv->vdata.data;
    } else {
        ad_len--;
    }

    switch (adv->name.ntype) {
        case AIS_ADV_NAME_SHORT:
            sd[0].type = BT_DATA_NAME_SHORTENED;
            break;
        case AIS_ADV_NAME_FULL:
            sd[0].type = BT_DATA_NAME_COMPLETE;
            break;
        default:
            printf("Invalid adv name type.\r\n");
            return AIS_ERR_INVALID_ADV_DATA;
    }

    if (adv->name.name == NULL) {
        printf("Invalid adv device name.\r\n");
        return AIS_ERR_INVALID_ADV_DATA;
    }

    sd[0].data     = adv->name.name;
    sd[0].data_len = strlen(adv->name.name);

    err = bt_le_adv_start(BT_LE_ADV_CONN, ad, ad_len, sd, sd_len);
    if (err) {
        printf("Advertising failed to start (err %d)\n", err);
        return AIS_ERR_ADV_FAIL;
    }

    return 0;
}

ais_err_t ble_advertising_stop()
{
    int ret;
    ret = bt_le_adv_stop();
    return ret ? AIS_ERR_STOP_ADV_FAIL : 0;
}

ais_err_t ble_get_mac(uint8_t *mac)
{
    ais_err_t    err;
    bt_addr_le_t laddr;

    err = ais_ota_get_local_addr(&laddr);
    if (err != AIS_ERR_SUCCESS) {
        printf("Failed to get local addr.\r\n");
    } else {
        memcpy(mac, laddr.a.val, 6);
        printf("Local addr got (%02x:%02x:%02x:%02x:%02x:%02x).\n", mac[0],
               mac[1], mac[2], mac[3], mac[4], mac[5]);
    }

    return err;
}
