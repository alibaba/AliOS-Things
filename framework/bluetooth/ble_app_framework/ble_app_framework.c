#include <aos/aos.h>
#include <ble_app_framework_def.h>
#include <ble_app_framework.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/uuid.h>
#include <bluetooth/hci.h>
#include <bluetooth/gatt.h>

#ifndef __packed
#define __packed __attribute__((__packed__))
#endif

struct attr_info {
    /* hold the user-handle related to attr */
    uint16_t handle;
    /* user_data length info related to bt_gatt_attr.user_data*/
    uint16_t user_data_len; /* used by value attr */
};

typedef struct peri_cb_s {
    ble_peripheral_conn_cb_t connected;
    ble_peripheral_disconn_cb_t disconnected;
} peri_cb_t;

struct adv_data_s {
    struct adv_data *ad;
    size_t ad_siz;
    struct adv_data *sd;
    size_t sd_siz;
};

#define BLE_PERI_HDL_MAX 1 /* support only 1 peripheral */
#define DEV_NAME_MAX_LEN 32
#define MANUFACTURE_NAME_MAX_LEN 32
typedef struct ble_peri_s {
    /* gatt db from user app */
    uint8_t *db;
    /* gatt db length */
    int db_len;
    /* attr array built from user db */
    struct bt_gatt_attr *attr;
    /* total number of attrs */
    int attr_num;
    /* hold the attr info, 1 entry for each attr */
    struct attr_info *itbl;
    /**
     * value attributes added by ble_attr_add(),
     * 1 entry for each characteristic value attr.
     */
    ble_gatt_attr_t *vattr;
    /* service structure built from attr */
#ifdef CONFIG_BLE_50
    struct bt_gatt_service svc;
#endif
    char dev_name[DEV_NAME_MAX_LEN];
    char manu_name[MANUFACTURE_NAME_MAX_LEN];
    peri_cb_t cb;
    struct adv_data_s advdata;
} ble_peri_t;

static uint32_t g_ble_hdl = 0;
static ble_peri_t g_peri[BLE_PERI_HDL_MAX];

#define MOD "ble_framework"

int hci_driver_init();

peripheral_hdl_t
ble_peripheral_init
(
    peripheral_init_t *p,
    ble_peripheral_conn_cb_t c,
    ble_peripheral_disconn_cb_t disc,
    const uint8_t *gatt_db,
    int db_len
)
{
    int err;
    peripheral_hdl_t hdl;

    if (!p || !gatt_db) {
        LOGE(MOD, "%s: invalid arguments", __func__);
        return -1;
    }

    if (g_ble_hdl >= BLE_PERI_HDL_MAX) {
        LOGW(MOD, "No more peripheral is allowed.");
        return -1;
    }

    LOG("Initializing BLE Peripheral");

    hci_driver_init();
    err = bt_enable(NULL);
    if (err) {
        LOGE(MOD, "BLE Peripheral initialization failed (err %d)", err);
        return -1;
    }

    hdl = g_ble_hdl++;
    g_peri[hdl].db = (uint8_t *)gatt_db;
    g_peri[hdl].db_len = db_len;
    strncpy(g_peri[hdl].dev_name, p->dev_name, sizeof(g_peri[hdl].dev_name) - 1);
    g_peri[hdl].cb.connected = c;
    g_peri[hdl].cb.disconnected = disc;

    LOG("BLE Peripheral initialization completed");

    return hdl;
}

static void free_val_attrs_list(peripheral_hdl_t hdl)
{

}

static void free_handle_to_attr_tbl(peripheral_hdl_t hdl)
{

}

void ble_peripheral_deinit
(
    peripheral_hdl_t hdl
)
{
    int i;

    /* Clear peripheral device structure here <TODO> */
    if (g_peri[hdl].attr)
        aos_free(g_peri[hdl].attr);

    for (i = 0; i < g_peri[hdl].attr_num; i++) {
        if (g_peri[hdl].attr[i].user_data)
            aos_free(g_peri[hdl].attr[i].user_data);
    }

    free_val_attrs_list(hdl);
    free_handle_to_attr_tbl(hdl);

#ifdef CONFIG_BLE_50
    bt_gatt_service_unregister(&g_peri[hdl].svc);
#endif

    return;
}

//#pragma pack(1)
struct db_genreral_hdr {
    uint8_t handle[2];
    uint8_t perm[1];
    uint8_t len[1];
    uint8_t type[2];
} __packed;

struct db_desc_hdr {
    uint8_t handle[2];
    uint8_t perm[1];
    uint8_t len[1];
} __packed;
//#pragma pack()

#define G_HEADER_SIZE sizeof(struct db_genreral_hdr)
#define C_HEADER_SIZE sizeof(struct db_desc_hdr)
#define TYPE_CMP(typer, typee) (((typer)[0] == ((typee) & 0xff)) && \
                                ((typer)[1] == (((typee) >> 8) & 0xff)))

/* Place below items in data area */
static struct bt_uuid *primary_svc_uuid = BT_UUID_GATT_PRIMARY;
static struct bt_uuid *secondary_svc_uuid = BT_UUID_GATT_PRIMARY;
static struct bt_uuid *include_svc_uuid = BT_UUID_GATT_SECONDARY;
static struct bt_uuid *chrc_uuid = BT_UUID_GATT_CHRC;
static struct bt_uuid *ccc_uuid = BT_UUID_GATT_CCC;
static struct bt_uuid *cep_uuid = BT_UUID_GATT_CEP;
static struct bt_uuid *cud_uuid = BT_UUID_GATT_CUD;
static struct bt_uuid *cpf_uuid = BT_UUID_GATT_CPF;

static void ccc_cfg_changed(const struct bt_gatt_attr *attr,
                                 u16_t value)
{
    LOGI(MOD, "Cfg changed, attr.handle is %d, "
         "value is %d", attr->handle, value);

    if(value == BT_GATT_CCC_NOTIFY)
        LOGI(MOD, "Cfg changed, value is BT_GATT_CCC_NOTIFY");
    if(value == BT_GATT_CCC_INDICATE)
        LOGI(MOD, "Cfg changed, value is BT_GATT_CCC_INDICATE");
}

static int make_attr_and_svc(peripheral_hdl_t hdl)
{
    uint8_t *db = g_peri[hdl].db, *p;
#ifdef CONFIG_BLE_50
    uint8_t attr_ignore_flag = 0;;
#endif
    int db_len = g_peri[hdl].db_len, len = 0, attr_cnt = 0, attr_idx = 0;
    struct db_genreral_hdr *iter = (struct db_genreral_hdr *)db;
    struct db_desc_hdr *c;
    struct bt_gatt_attr *a, *a2;
    uint16_t handle;

    if (hdl >= BLE_PERI_HDL_MAX) {
        LOGE(MOD, "hdl number is not within valid range.");
        return -1;
    }

    if (!db || !db_len) {
        LOGE(MOD, "Invalid GATT database.");
        return -1;
    }

    /* get attr count */
    while ((uint32_t)iter < (uint32_t)(db + db_len)) {
        if (TYPE_CMP(iter->type, GATT_UUID_PRI_SERVICE) || /* primary service */
            TYPE_CMP(iter->type, GATT_UUID_SEC_SERVICE)) { /* secondary service */
            LOGD(MOD, "A primary or secondary service item found.");
#ifdef CONFIG_BLE_50
            p = (uint8_t *)iter + G_HEADER_SIZE;
            /* Skip GAP and GATT services */
            if (((iter->len[0 ] == 4) && (p[0] == 0x00) && (p[1] == 0x18)) ||
                ((iter->len[0] == 4) && (p[0] == 0x01) && (p[1] == 0x18))) {
                LOGD(MOD, "GAP/GATT service, ignore it.");
                attr_ignore_flag = 1; /* ignore following char and desc */
            } else {
                attr_ignore_flag = 0;
#endif
                attr_cnt++;
#ifdef CONFIG_BLE_50
            }
#endif
            p = (uint8_t *)iter;
            p += G_HEADER_SIZE + iter->len[0] - 2;
            iter = (struct db_genreral_hdr *)p;
        } else if (TYPE_CMP(iter->type, GATT_UUID_INCLUDE_SERVICE)) { /* included service */
            LOGW(MOD, "Included service is not supported yet!!");
            return -1;
        } else if (TYPE_CMP(iter->type, GATT_UUID_CHAR_DECLARE)) { /* characteristic declare */
            LOGD(MOD, "A characteristic item found.");

#ifdef CONFIG_BLE_50
            if (!attr_ignore_flag) {
#endif
                attr_cnt += 2; /* 1 for c, one for c-v */
#ifdef CONFIG_BLE_50
            } else {
                LOGD(MOD, "GAP/GATT characteristic, ignore it.");
            }
#endif

            p = (uint8_t *)iter;
            p += G_HEADER_SIZE + iter->len[0] - 2; /* eat until uuid */
            c = (struct db_desc_hdr *)p;
            p += C_HEADER_SIZE + c->len[0];
            /* writable charateristic? */
            if (c->perm[0] & (uint8_t)LEGATTDB_PERM_WRITABLE)
                p++;

            c = (struct db_desc_hdr *)p;

            /* Check descriptor, [0 - n] elments may exist. */
            while ((c->len[0] == (uint8_t)LEGATTDB_UUID16_SIZE) ||
                   (c->len[0] == (uint8_t)LEGATTDB_UUID128_SIZE)) {
                LOGD(MOD, "A characteristic descriptor item found.");
                /* Update index. */
                attr_cnt++;
                /* Update the position pointer. */
                p += C_HEADER_SIZE + c->len[0];
                if (c->perm[0] & (uint8_t)LEGATTDB_PERM_WRITABLE)
                    p++;
                c = (struct db_desc_hdr *)p;
            }

            iter = (struct db_genreral_hdr *)p;
        } else {
            LOGE(MOD, "Invalid attribute type in gatt database "
                 "(type: 0x%04x)!!!", iter->type);
            return -1;
        }
    }

    LOGI(MOD, "%d attributes detected.", attr_cnt);

    /* Allocate memory for attribute array*/
    g_peri[hdl].attr_num = attr_cnt;
    g_peri[hdl].attr = (struct bt_gatt_attr *)aos_malloc(
        sizeof(struct bt_gatt_attr) * attr_cnt);
    g_peri[hdl].itbl = (struct attr_info *)aos_malloc(
        sizeof(struct attr_info) * attr_cnt);
    if (g_peri[hdl].attr == NULL || g_peri[hdl].itbl == NULL) {
        LOGE(MOD, "Failed to allocate memory for peripheral (%d)", hdl);
        return -1;
    }
    memset(g_peri[hdl].attr, 0, sizeof(struct bt_gatt_attr) * attr_cnt);
    memset(g_peri[hdl].itbl, 0, sizeof(struct attr_info) * attr_cnt);

    /* Make attr array */
    len = 0;
    iter = (struct db_genreral_hdr *)db;
    attr_idx = 0;
    while ((uint32_t)iter < (uint32_t)(db + db_len)) {
        if (TYPE_CMP(iter->type, GATT_UUID_PRI_SERVICE) ||
            TYPE_CMP(iter->type, GATT_UUID_SEC_SERVICE)) { /* service */
#ifdef CONFIG_BLE_50
            p = (uint8_t *)iter + G_HEADER_SIZE;
            /* Skip GAP and GATT services */
            if (((iter->len[0] == 4) && (p[0] == 0x00) && (p[1] == 0x18)) ||
                ((iter->len[0] == 4) && (p[0] == 0x01) && (p[1] == 0x18))) {
                LOGD(MOD, "GAP/GATT service, ignore it.");
                attr_ignore_flag = 1; /* ignore following char and desc */
            } else {
                attr_ignore_flag = 0;
            }

            if (!attr_ignore_flag) {
#endif

            a = &(g_peri[hdl].attr)[attr_idx];

            if (TYPE_CMP(iter->type, GATT_UUID_PRI_SERVICE)) {
                LOGD(MOD, "Adding a primary service attribute.");
                a->uuid = primary_svc_uuid;
            } else {
                LOGD(MOD, "Adding a secondary service attribute.");
                a->uuid = secondary_svc_uuid;
            }

            /* Fill the attribute array */
            a->perm = BT_GATT_PERM_READ;
            a->read = bt_gatt_attr_read_service;

            /* service handle into attr info table */
            handle = ((uint16_t)iter->handle[0] | ((uint16_t)iter->handle[1] << 8));
            g_peri[hdl].itbl[attr_idx].handle = handle;
            a->handle = handle;

            LOGD(MOD, "New handle (0x%04x, idx: %d) added in attr info table.",
                 handle, attr_idx);

            /* determine the user data */
            if (iter->len[0] == 4) { /* uuid16 */
                struct bt_uuid_16 *tmp;
                tmp = (struct bt_uuid_16 *)aos_malloc(sizeof(struct bt_uuid_16));
                memset(tmp, 0, sizeof(struct bt_uuid_16));
                tmp->uuid.type = BT_UUID_TYPE_16;
                p = (uint8_t *)iter;
                p += G_HEADER_SIZE;
                tmp->val = (uint16_t)(*p & 0xff) + (((uint16_t)(*(p+1)) << 8) & 0xff00);
                a->user_data = tmp;
            } else if (iter->len[0] == 18) { /* uui128 */
                struct bt_uuid_128 *tmp;
                tmp = (struct bt_uuid_128 *)aos_malloc(sizeof(struct bt_uuid_128));
                memset(tmp, 0, sizeof(struct bt_uuid_128));
                tmp->uuid.type = BT_UUID_TYPE_128;
                p = (uint8_t *)iter;
                p += G_HEADER_SIZE;
                memcpy(tmp->val, p, 16);
                a->user_data = tmp;
            } else {
                LOGE(MOD, "Not supported service uuid for peripheral "
                     "(%d, attr_idx: %d).", hdl, attr_idx);
                return -1;
            }

            attr_idx++;

#ifdef CONFIG_BLE_50
            } /* if (!ignore) */
#endif

            /* update pointer*/
            p = (uint8_t *)iter;
            p += G_HEADER_SIZE + iter->len[0] - 2;
            iter = (struct db_genreral_hdr *)p;
        } else if (TYPE_CMP(iter->type, GATT_UUID_INCLUDE_SERVICE)) {
            LOGW(MOD, "Included service is not supported yet!!");
            return -1;
        } else if (TYPE_CMP(iter->type, GATT_UUID_CHAR_DECLARE)) {
#ifdef CONFIG_BLE_50
            if (!attr_ignore_flag) {
#endif
            LOGD(MOD, "Adding a characteristic and a value attribute.");

            /* Fill the charateristic attribute */
            a = &(g_peri[hdl].attr)[attr_idx];
            a->uuid = chrc_uuid;
            a->perm = BT_GATT_PERM_READ;
            a->read = bt_gatt_attr_read_chrc;

            /* characteristic handle into attr info table */
            handle = ((uint16_t)iter->handle[0] | ((uint16_t)iter->handle[1] << 8));
            g_peri[hdl].itbl[attr_idx].handle = handle;
            a->handle = handle;

            LOGD(MOD, "New handle (0x%04x, idx: %d) added in attr info table.",
                 handle, attr_idx);

            /* Fill the charateristic value attribute */
            a2 = &(g_peri[hdl].attr)[attr_idx+1];
            a2->read = NULL;
            a2->write = NULL;
            a2->user_data = NULL;
            p = (uint8_t *)iter;
            p += G_HEADER_SIZE + iter->len[0] -2;
            c = (struct db_desc_hdr *)p;
            //LOGD(MOD, "c->perm[0]: %d", c->perm[0]);
            uint8_t perm = c->perm[0] & LEGATTDB_PERM_MASK;
            if ((perm == LEGATTDB_PERM_NONE) || ((perm & \
                (LEGATTDB_PERM_READABLE|LEGATTDB_PERM_WRITABLE)) == 0)) {
                a2->perm = BT_GATT_PERM_NONE;
            } else {
                if (perm & LEGATTDB_PERM_READABLE)
                    a2->perm |= BT_GATT_PERM_READ;
                if (perm & LEGATTDB_PERM_WRITABLE)
                    a2->perm |= BT_GATT_PERM_WRITE;
            }
            //LOGD(MOD, "Perimission for this value attribute: %d", a2->perm);

            /* characteristic value handle into attr info table */
            handle = ((uint16_t)c->handle[0] | ((uint16_t)c->handle[1] << 8));
            g_peri[hdl].itbl[attr_idx+1].handle = handle;
            a2->handle = handle;

            LOGD(MOD, "New handle (0x%04x, idx: %d) added in attr info table.",
                 handle, attr_idx+1);

            /* user data */
            if (iter->len[0] == 7) { /* uui16 */
                struct bt_gatt_chrc *tmp = (struct bt_gatt_chrc *)aos_malloc(
                    sizeof(struct bt_gatt_chrc) + sizeof(struct bt_uuid_16));
                struct bt_uuid_16 *tmp_uuid = (struct bt_uuid_16 *)( \
                    (uint8_t *)tmp + sizeof(struct bt_gatt_chrc));
                if (!tmp) {
                    LOGE(MOD, "Failed to allocate memory %f %d",
                         __FILE__, __LINE__);
                    return -1;
                }
                memset(tmp, 0, sizeof(struct bt_gatt_chrc) + sizeof(struct bt_uuid_16));
                tmp_uuid->uuid.type = BT_UUID_TYPE_16;
                p = (uint8_t *)iter;
                p += G_HEADER_SIZE;
                tmp->properties = *p;
                p += iter->len[0] - 2 - 2;
                tmp_uuid->val = ((uint16_t)(*p) & 0xff) + ((((uint16_t)(*(p+1))) << 8) & 0xff00);
                tmp->uuid = (struct bt_uuid *)tmp_uuid;
                a->user_data = tmp;
                a2->uuid = (struct bt_uuid *)tmp_uuid;
            } else if (iter->len[0] == 21) { /* uuid128 */
                struct bt_gatt_chrc *tmp = (struct bt_gatt_chrc *)aos_malloc(
                    sizeof(struct bt_gatt_chrc) + sizeof(struct bt_uuid_128));
                struct bt_uuid_128 *tmp_uuid = (struct bt_uuid_128 *)( \
                    (uint8_t *)tmp + sizeof(struct bt_gatt_chrc));
                if (!tmp) {
                    LOGE(MOD, "Failed to allocate memory %f %d",
                         __FILE__, __LINE__);
                    return -1;
                }
                memset(tmp, 0, sizeof(struct bt_gatt_chrc) + sizeof(struct bt_uuid_128));
                tmp_uuid->uuid.type = BT_UUID_TYPE_128;
                p = (uint8_t *)iter;
                p += G_HEADER_SIZE;
                tmp->properties = *p;
                p += iter->len[0] - 2 - 16;
                memcpy(tmp_uuid->val, p, 16);
                tmp->uuid = (struct bt_uuid *)tmp_uuid;
                a->user_data = tmp;
                a2->uuid = (struct bt_uuid *)tmp_uuid;
            } else {
                LOGE(MOD, "Not supported charateristic uuid for peripheral "
                     "(%d, attr_idx: %d).", hdl, attr_idx);
                return -1;
            }

            attr_idx += 2;

#if 0
            /* Deal with CCC if any */
            p = (uint8_t *)iter + G_HEADER_SIZE; /* Properties offset */
            if ((*p & LEGATTDB_CHAR_PROP_NOTIFY) || \
                (*p & LEGATTDB_CHAR_PROP_INDICATE)) {
                LOGD(MOD, "Adding a CCC attribute.");

                struct _bt_gatt_ccc *c = (struct _bt_gatt_ccc *)aos_malloc(\
                                         sizeof(struct _bt_gatt_ccc) + \
                                         sizeof(struct bt_gatt_ccc_cfg) * \
                                         BT_GATT_CCC_MAX);
                struct bt_gatt_ccc_cfg *c_cfg;
                if (!c) {
                    LOGE(MOD, "%s %d malloc failed", __FILE__, __LINE__);
                    return -1;
                }

                memset(c, 0, sizeof(struct _bt_gatt_ccc) + \
                       sizeof(struct bt_gatt_ccc_cfg) * BT_GATT_CCC_MAX);
                c_cfg = (struct bt_gatt_ccc_cfg *)((uint8_t *)c +
                        sizeof(struct _bt_gatt_ccc));
                c->cfg = c_cfg;
                c->cfg_len = BT_GATT_CCC_MAX;
                c->cfg_changed = ccc_cfg_changed;

                a = &(g_peri[hdl].attr)[attr_idx];
                a->uuid = ccc_uuid;
                a->perm = BT_GATT_PERM_READ | BT_GATT_PERM_WRITE;
                a->read = bt_gatt_attr_read_ccc;
                a->write = bt_gatt_attr_write_ccc;
                a->user_data = c;

                g_peri[hdl].itbl[attr_idx].handle = -1; /* Special handle for CCC */

                LOGD(MOD, "CCC handle (0x%04x, idx: %d) added in attr info table.",
                     g_peri[hdl].itbl[attr_idx].handle, attr_idx);

                attr_idx++;
            }
#endif
#ifdef CONFIG_BLE_50
            } /* if (!attr_ignore_flag) */
#endif

            /* update pointer */
            p = (uint8_t *)iter;
            c = (struct db_desc_hdr *)(p + G_HEADER_SIZE + iter->len[0] - 2);
            p += G_HEADER_SIZE + iter->len[0] - 2 + C_HEADER_SIZE + c->len[0];
            if (c->perm[0] & (uint8_t)LEGATTDB_PERM_WRITABLE)
                p++;

            c = (struct db_desc_hdr *)p;

            /* Deal with descriptors. */
            uint16_t uuid16;
            uint8_t *u;
            while ((c->len[0] == (uint8_t)LEGATTDB_UUID16_SIZE) ||
                   (c->len[0] == (uint8_t)LEGATTDB_UUID128_SIZE)) {
                if (c->len[0] == (uint8_t)LEGATTDB_UUID128_SIZE) {
                    LOGW(MOD, "UUID128 type descriptor is not supported yet.");
                    /* <TODO> */
                    (g_peri[hdl].attr_num)--; /* Not to add to attr list */
                } else {
                    u = (uint8_t *)c + C_HEADER_SIZE;
                    if (c->perm[0] & (uint8_t)LEGATTDB_PERM_WRITABLE)
                        u++;
                    uuid16 = ((uint16_t)(*u)) | ((uint16_t)(*(u+1)) << 8);
                    switch (uuid16) {
                        case BT_UUID_GATT_CCC_VAL:
                            {
                            struct _bt_gatt_ccc *ccc;
                            struct bt_gatt_ccc_cfg *c_cfg;

                            LOGD(MOD, "Adding a CCC attribute.");

                            /* User data */
                            ccc = (struct _bt_gatt_ccc *)aos_malloc(\
                                sizeof(struct _bt_gatt_ccc) + \
                                sizeof(struct bt_gatt_ccc_cfg) * \
                                BT_GATT_CCC_MAX);
                            if (!ccc) {
                                LOGE(MOD, "%s %d malloc failed", __FILE__, __LINE__);
                                return -1;
                            }

                            memset(ccc, 0, sizeof(struct _bt_gatt_ccc) + \
                                sizeof(struct bt_gatt_ccc_cfg) * BT_GATT_CCC_MAX);
                            c_cfg = (struct bt_gatt_ccc_cfg *)((uint8_t *)ccc +
                                sizeof(struct _bt_gatt_ccc));
                            ccc->cfg = c_cfg;
                            ccc->cfg_len = BT_GATT_CCC_MAX;
                            ccc->cfg_changed = ccc_cfg_changed;

                            a = &(g_peri[hdl].attr)[attr_idx];
                            a->uuid = ccc_uuid;
                            a->perm = BT_GATT_PERM_READ | BT_GATT_PERM_WRITE;
                            a->read = bt_gatt_attr_read_ccc;
                            a->write = bt_gatt_attr_write_ccc;
                            a->user_data = ccc;

                            /* Attr handle */
                            handle = ((uint16_t)c->handle[0] | ((uint16_t)c->handle[1] << 8));
                            a->handle = handle;
                            g_peri[hdl].itbl[attr_idx].handle = handle;

                            LOGD(MOD, "CCC handle (0x%04x, idx: %d) added in attr info table.",
                                 handle, attr_idx);

                            attr_idx++;
                            }

                            break;
                        case BT_UUID_GATT_CEP_VAL:
                            LOGW(MOD, "CEP type descriptor is not supported yet!!!");
                            /* <TODO> */
                            (g_peri[hdl].attr_num)--; /* Not to add to attr list */
                            break;
                        case BT_UUID_GATT_CUD_VAL:
                            LOGW(MOD, "CUD type descriptor is not supported yet!!!");
                            /* <TODO> */
                            (g_peri[hdl].attr_num)--; /* Not to add to attr list */
                            break;
                        case BT_UUID_GATT_CPF_VAL:
                            LOGW(MOD, "CPF type descriptor is not supported yet!!!");
                            /* <TODO> */
                            (g_peri[hdl].attr_num)--; /* Not to add to attr list */
                            break;
                        default:
                            LOGW(MOD, "This descriptor (uuid: 0x%04x) is not supported yet!!!", uuid16);
                            /* <TODO> */
                            (g_peri[hdl].attr_num)--; /* Not to add to attr list */
                            break;
                    }
                }

                /* Update the position pointer. */
                p += C_HEADER_SIZE + c->len[0];
                if (c->perm[0] & (uint8_t)LEGATTDB_PERM_WRITABLE)
                    p++;
                c = (struct db_desc_hdr *)p;
            }

            /* update the iteration pointer */
            iter = (struct db_genreral_hdr *)p;
        } else {
            LOGE(MOD, "Invalid attribute type in gatt database!!!");
            return -1;
        }
    }

    if (attr_idx != g_peri[hdl].attr_num) {
        LOGE(MOD, "Error accured (attr_idx (%d) not match attr_num (%d))",
             attr_idx, g_peri[hdl].attr_num);
        return -1;
    }

#ifdef CONFIG_BLE_50
    g_peri[hdl].svc.attrs = g_peri[hdl].attr;
    g_peri[hdl].svc.attr_count = g_peri[hdl].attr_num;
#endif

    return 0;
}

static void connected(struct bt_conn *conn, uint8_t err)
{
    char addr[BT_ADDR_LE_STR_LEN];
    peripheral_hdl_t hdl = 0;

    bt_addr_le_to_str(bt_conn_get_dst(conn), addr, sizeof(addr));

    if (err) {
        printf("Failed to connect to %s (%u)\n", addr, err);
        return;
    }

    LOGI(MOD, "Connected %s\n", addr);

    if (bt_conn_security(conn, BT_SECURITY_FIPS)) {
        printf("Failed to set security\n");
    }

    if (g_peri[hdl].cb.connected) {
        g_peri[hdl].cb.connected();
    }
}

static void disconnected(struct bt_conn *conn, uint8_t reason)
{
    char addr[BT_ADDR_LE_STR_LEN];
    peripheral_hdl_t hdl = 0;

    bt_addr_le_to_str(bt_conn_get_dst(conn), addr, sizeof(addr));

    LOGI(MOD, "Disconnected from %s (reason %u)\n", addr, reason);

    if (g_peri[hdl].cb.disconnected) {
        g_peri[hdl].cb.disconnected();
    }
}

static void identity_resolved(struct bt_conn *conn, const bt_addr_le_t *rpa,
                              const bt_addr_le_t *identity)
{
    char addr_identity[BT_ADDR_LE_STR_LEN];
    char addr_rpa[BT_ADDR_LE_STR_LEN];

    bt_addr_le_to_str(identity, addr_identity, sizeof(addr_identity));
    bt_addr_le_to_str(rpa, addr_rpa, sizeof(addr_rpa));

    printf("Identity resolved %s -> %s\n", addr_rpa, addr_identity);
}

static void security_changed(struct bt_conn *conn, bt_security_t level)
{
    char addr[BT_ADDR_LE_STR_LEN];

    bt_addr_le_to_str(bt_conn_get_dst(conn), addr, sizeof(addr));

    printf("Security changed: %s level %u\n", addr, level);
}

static struct bt_conn_cb conn_callbacks = {
    .connected = connected,
    .disconnected = disconnected,
    .identity_resolved = identity_resolved,
    .security_changed = security_changed,
};

static void auth_passkey_display(struct bt_conn *conn, unsigned int passkey)
{
    char addr[BT_ADDR_LE_STR_LEN];

    bt_addr_le_to_str(bt_conn_get_dst(conn), addr, sizeof(addr));

    printf("Passkey for %s: %u\n", addr, passkey);
}

static void auth_cancel(struct bt_conn *conn)
{
    char addr[BT_ADDR_LE_STR_LEN];

    bt_addr_le_to_str(bt_conn_get_dst(conn), addr, sizeof(addr));

    printf("Pairing cancelled: %s\n", addr);
}

static struct bt_conn_auth_cb auth_cb_display = {
    .passkey_display = auth_passkey_display,
    .passkey_entry = NULL,
    .cancel = auth_cancel,
};

void ble_adv_start
(
    ble_adv_complete_cb_t adv_handler,
    const char *manufacture,
    peripheral_hdl_t hdl
)
{
    int err;
    static uint8_t first_time = 1;

    if (!first_time) goto adv_operation;

    struct adv_data ad_default[] = {
        ADV_DATA_BYTES(EIRADV_DATA_FLAGS, (AD_FLAG_GENERAL | AD_FLAG_NO_BREDR)),
        ADV_DATA_BYTES(EIRADV_DATA_UUID16_ALL, 0x0a, 0x18),
    };

    struct adv_data sd_default[] = {
        ADV_DATA(EIRADV_DATA_NAME_COMPLETE, g_peri[hdl].dev_name,
                 strlen(g_peri[0].dev_name)),
    };

    (void *)adv_handler;

    if (g_peri[hdl].advdata.ad_siz == 0) {
        g_peri[hdl].advdata.ad_siz = ARRAY_SIZE(ad_default);
        g_peri[hdl].advdata.ad = ad_default;
    }

    if (g_peri[hdl].advdata.sd_siz == 0) {
        g_peri[hdl].advdata.sd_siz = ARRAY_SIZE(sd_default);
        g_peri[hdl].advdata.sd = sd_default;
    }

    strncpy(g_peri[hdl].manu_name, manufacture,
            sizeof(g_peri[hdl].manu_name) - 1);

    if (make_attr_and_svc(hdl) != 0) return;
#ifdef CONFIG_BLE_50
    err = bt_gatt_service_register(&(g_peri[hdl].svc));
    if (err != 0) {
        LOGE(MOD, "Failed to bt_gatt_service_register");
        return;
    }
#else
    bt_gatt_register(g_peri[hdl].attr, g_peri[hdl].attr_num);
#endif

    bt_conn_auth_cb_register(&auth_cb_display);
    bt_conn_cb_register(&conn_callbacks);

    LOGD(MOD, "ad size is %d, sd size is %d",
         g_peri[hdl].advdata.ad_siz, g_peri[hdl].advdata.sd_siz);

adv_operation:
    err = bt_le_adv_start(BT_LE_ADV_CONN,
                          (struct bt_data *)g_peri[hdl].advdata.ad,
                          g_peri[hdl].advdata.ad_siz,
                          (struct bt_data *)g_peri[hdl].advdata.sd,
                          g_peri[hdl].advdata.sd_siz);

    if (err) {
        LOGE(MOD, "Advertising failed to start (err %d).", err);
        return;
    }

    if (first_time) first_time = 0;

    LOG("Advertising successfully started.");
}

void ble_adv_stop()
{
    bt_le_adv_stop();
}

static uint16_t find_val_len_by_attr
(
    const struct bt_gatt_attr *attr,
    peripheral_hdl_t hdl
)
{
    uint16_t attr_idx;

    for (attr_idx = 0; attr_idx < g_peri[hdl].attr_num; attr_idx++) {
        if (attr == &(g_peri[hdl].attr[attr_idx])) break;
    }

    if (attr_idx >= g_peri[hdl].attr_num) {
        LOGE(MOD, "No corresponding attribute entry found.");
        return -1;
    }

    return g_peri[hdl].itbl[attr_idx].user_data_len;
}

static ssize_t vattr_read
(
    struct bt_conn *conn,
    const struct bt_gatt_attr *attr,
    void *buf,
    uint16_t len,
    uint16_t offset
)
{
    void *value = attr->user_data;
    uint16_t val_len = 0;
    peripheral_hdl_t hdl = 0; /* <TODO> */

    if (attr->perm & BT_GATT_PERM_READ == 0) {
        LOGE(MOD, "READ is not supported for this attribute.");
        return 0;
    }

    if (attr->user_data == NULL) {
        LOGI(MOD, "No data to read for this attribute");
        return 0;
    }

    if ((val_len = find_val_len_by_attr(attr, hdl)) == -1)
        return 0;
    else
        LOGD(MOD, "Data length to read is %d", val_len);

    return bt_gatt_attr_read(conn, attr, buf, len, offset, value, val_len);
}

static ssize_t vattr_write
(
    struct bt_conn *conn,
    const struct bt_gatt_attr *attr,
    const void *buf,
    uint16_t len,
    uint16_t offset,
    uint8_t flags
)
{
    uint8_t *value = attr->user_data;
    uint16_t val_len = 0;
    peripheral_hdl_t hdl = 0; /* <TODO> */

    if (attr->perm & BT_GATT_PERM_WRITE == 0) {
        LOGE(MOD, "WRITE is not supported for this attribute.");
        return 0;
    }

    if (attr->user_data == NULL) {
        LOGI(MOD, "Not writable for this attribute");
        return 0;
    }

    if ((val_len = find_val_len_by_attr(attr, hdl)) == -1)
        return 0;
    else
        LOGD(MOD, "Max writable data length is %d", val_len);

    if (offset + len > val_len) {
        return BT_GATT_ERR(BT_ATT_ERR_INVALID_OFFSET);
    }

    memcpy(value + offset, buf, len);

    return len;
}

ble_gatt_attr_t *
ble_attr_add
(
    uint16_t handle, /* Note: this handle is not peripheral_hdl_t */
    uint16_t val_len,
    const uint8_t *val
)
{
    peripheral_hdl_t hdl;
    ble_gatt_attr_t *vattr, *p;

    vattr = (ble_gatt_attr_t *)aos_malloc(sizeof(ble_gatt_attr_t));
    if (!vattr) {
        LOGE(MOD, "Failed to alloca memory %s %d", __FILE__, __LINE__);
        return NULL;
    }
    memset(vattr, 0, sizeof(ble_gatt_attr_t));

    vattr->handle = handle;
    vattr->this_node.next = NULL;
    vattr->this_node.prev = NULL;
    vattr->value_buffer_length = val_len;
    /* Do not care other field at this moment. May add later. */

    /**
     * Assume the case that only 1 peripheral supported,
     * need fix if multiple peripherals. <TODO>
     */
    hdl = g_ble_hdl - 1;

    /* vattr node into the end of vattr list */
    p = g_peri[hdl].vattr;
    if (g_peri[hdl].vattr == NULL) {
        g_peri[hdl].vattr = vattr;
    } else {
        /* quickly goes to end */
        dlist_node_t *n = &p->this_node;
        while (n->next != NULL) n = n->next;
        p = CONTAINER_OF(n, ble_gatt_attr_t, this_node);
        vattr->this_node.prev = &p->this_node;
        p->this_node.next = &vattr->this_node;
    }

    /* Find the associated attr in DB */
    int i;
    struct attr_info *t;
    LOGD(MOD, "Attr handle to find: 0x%04x, in %s", handle, __func__);
    for (i = 0; i < g_peri[hdl].attr_num; i++) {
        t = &g_peri[hdl].itbl[i];
        LOGD(MOD, "Checking t->handle: 0x%04x", t->handle);
        if (t->handle == handle) break;
    }

    if (i >= g_peri[hdl].attr_num) {
        LOGE(MOD, "No corresponding handle (0x%04x) found in DB.", handle);
        return NULL;
    }

    LOGD(MOD, "Attr found on idx: %d", i);

    /* associated attr found, fill the user_data field. */
    struct bt_gatt_attr *a = &(g_peri[hdl].attr[i]);
    a->user_data = (void *)val;
    g_peri[hdl].itbl[i].user_data_len = val_len;
    if (a->read != NULL) {
        LOGW(MOD, "There is already a read func for this attr. Do you really "
             "want to add one more? Something may be wrong!");
    } else {
        a->read = vattr_read;
    }
    if (a->write != NULL) {
        LOGW(MOD, "There is already a write func for this attr. Do you really "
             "want to add one more? Something may be wrong!");
    } else {
        a->write = vattr_write;
    }

    return vattr;
}

static struct bt_gatt_attr* find_attr_entry
(
    ble_gatt_attr_t *attr,
    peripheral_hdl_t hdl
)
{
    ble_gatt_attr_t *v = g_peri[hdl].vattr;
    dlist_node_t *n;
    uint16_t handle;
    int idx;

    if (!v) return NULL;

    n = &v->this_node;

    /* Find the vattr entry */
    while (n) {
        v = CONTAINER_OF(n, ble_gatt_attr_t, this_node);
        if (v == attr) break;
        else {n = n->next; continue;}
    }

    if (!n) {
        LOGE(MOD, "No vattr entry found for peripheral (%d)", hdl);
        return NULL;
    }

    /* vattr found, use the handle to find attr entry*/
    handle = v->handle;
    LOGD(MOD, "Attr handle to find: 0x%04x, in %s", handle, __func__);
    for (idx = 0; idx < g_peri[hdl].attr_num; idx++) {
        LOGD(MOD, "Checking handle: 0x%04x", g_peri[hdl].itbl[idx].handle);
        if (g_peri[hdl].itbl[idx].handle == handle) break;
    }

    if (idx >= g_peri[hdl].attr_num) {
        LOGE(MOD, "No attr entry found for peripheral (%d)", hdl);
        return NULL;
    }

    /* attr found */
    LOGD(MOD, "Attr found on idx: %d", idx);
    return &g_peri[hdl].attr[idx];
}

static void indicate_cb
(
    struct bt_conn *conn,
    const struct bt_gatt_attr *attr,
    uint8_t err
)
{
    if (err != 0) {
        LOGE(MOD, "Indication fail (err: %d, %p).", err, attr);
    } else {
        LOGI(MOD, "Indication success (%p).", attr);
    }
}

void ble_attr_indicate
(
    ble_gatt_attr_t *attr,
    peripheral_hdl_t hdl,
    uint16_t len,
    const uint8_t *data
)
{
    struct bt_gatt_attr *a;
    struct bt_gatt_indicate_params params;

    a = find_attr_entry(attr, hdl);
    if (!a) return;

    params.attr = a;
    params.func = indicate_cb;
    params.data = (const void *)data;
    params.len = len;

    bt_gatt_indicate(NULL, &params);
}

void ble_attr_notify
(
    ble_gatt_attr_t *attr,
    peripheral_hdl_t hdl,
    uint16_t len,
    const uint8_t *data
)
{
    struct bt_gatt_attr *a;

    a = find_attr_entry(attr, hdl);
    if (!a) return;

    /* attr found, notify it */
    bt_gatt_notify(NULL, a, (const void *)data, len);
}

void ble_set_ad_data
(
    peripheral_hdl_t hdl,
    const struct adv_data *ad,
    size_t ad_siz
)
{
    if (!ad || (ad_siz < 1)) {
        LOGE(MOD, "%s invalid argument. Adv data not set!", __func__);
        return;
    }

    g_peri[hdl].advdata.ad = (struct adv_data *)ad;
    g_peri[hdl].advdata.ad_siz = ad_siz;
}

void ble_set_sd_data
(
    peripheral_hdl_t hdl,
    const struct adv_data *sd,
    size_t sd_siz
)
{
    if (!sd || (sd_siz < 1)) {
        LOGE(MOD, "%s invalid argument. Adv data not set!", __func__);
        return;
    }

    g_peri[hdl].advdata.sd = (struct adv_data *)sd;
    g_peri[hdl].advdata.sd_siz = sd_siz;
}
