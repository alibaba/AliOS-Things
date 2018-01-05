/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include "internal/ble_helper_internal.h"

/* Peripheral security settings */
static const mico_bt_smart_security_settings_t security_settings = {
    .timeout_second                 = 15,
    .io_capabilities                = BT_SMART_IO_DISPLAY_ONLY,
    .authentication_requirements    = BT_SMART_AUTH_REQ_BONDING,
    .oob_authentication             = BT_SMART_OOB_AUTH_NONE,
    .max_encryption_key_size        = 16,
    .master_key_distribution        = BT_SMART_DISTRIBUTE_ALL_KEYS,
    .slave_key_distribution         = BT_SMART_DISTRIBUTE_ALL_KEYS,
};

peripheral_socket_t *g_skt_head = NULL;

static uint32_t g_ble_hdl = 0;
peripheral_hdl_t ble_peripheral_init(peripheral_init_t *p,
                                     ble_peripheral_conn_cb_t c,
                                     ble_peripheral_disconn_cb_t disc,
                                     const uint8_t *db, int db_len)
{
    peripheral_socket_t *s = (peripheral_socket_t *)aos_malloc((sizeof(peripheral_socket_t)));

    // mutex? <TODO>
    s->hdl = g_ble_hdl++;
    s->next = g_skt_head;
    g_skt_head = s;

    printf("%d, hdl (%d) inserted\r\n", __LINE__, s->hdl);

    /* Initialize MICO bluetooth Framework */
    mico_bt_init(MICO_BT_HCI_MODE, p->dev_name, p->client_links, p->server_links);

    /* Initialize MICO bluetooth peripheral */
    mico_bt_peripheral_init(&s->skt, &security_settings, c, disc, NULL);

    /* Build BT Stack layer GATT database */
    mico_bt_gatt_db_init(db, db_len);

    return s->hdl;
}

// Mutex? <TODO>
void ble_peripheral_deinit(peripheral_hdl_t hdl)
{
    peripheral_socket_t *p, *q;

    for (p = g_skt_head; p; q = p, p = p->next)
        if (p->hdl == hdl) {
            break;
        }

    if (!p) {
        printf("%s %d no hdl found\r\n", __func__, __LINE__);
        return;
    }

    if (p == g_skt_head) {
        g_skt_head = p->next;
        mico_free(p);
    } else {
        q->next = p->next;
        mico_free(p);
    }

    mico_bt_peripheral_deinit();
    mico_bt_deinit();
}
