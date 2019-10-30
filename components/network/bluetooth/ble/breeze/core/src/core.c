/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#include <string.h>

#include "core.h"
#include "transport.h"
#include "auth.h"
#include "extcmd.h"
#include "common.h"
#include "ble_service.h"
#include "utils.h"
#include "breeze_hal_ble.h"
#include "breeze_hal_sec.h"

core_t g_core;
extern auth_t g_auth;

#ifdef CONFIG_AIS_SECURE_ADV
#define AIS_SEQ_KV_KEY      "ais_adv_seq"
#define AIS_SEQ_UPDATE_FREQ (1 * 60 * 60) /* in second uint */
static uint32_t g_seq = 0;
static os_timer_t g_secadv_timer;
#endif

static ali_init_t const *g_ali_init;

void event_notify(uint8_t event_type, uint8_t *data, uint16_t length)
{
    ali_event_t event;

    event.type = event_type;
    event.rx_data.p_data = data;
    event.rx_data.length = length;
    g_core.event_handler(&event);
}

static void create_bz_adv_data(uint32_t model_id, uint8_t *mac_bin)
{
    uint16_t i;
    uint8_t  fmsk = 0;
    char* ver_str;
    char* p;
    uint8_t ver_int;

    SET_U16_LE(g_core.adv_data, ALI_COMPANY_ID);
    i = sizeof(uint16_t);
    /*extract Breeze version from BZ_VERSION*/
    char t_ver_info[20] = { 0 };
    strncpy(t_ver_info, BZ_VERSION, sizeof(t_ver_info) - 1);
    p = strtok(t_ver_info, ".");
    while(p = strtok(NULL, ".")){
        ver_str = p;
    }
    g_core.adv_data[i++] = atoi(ver_str);
    fmsk = BZ_BLUETOOTH_VER << FMSK_BLUETOOTH_VER_Pos;
#if BZ_ENABLE_AUTH
    fmsk |= 1 << FMSK_SECURITY_Pos;
#endif
#if BZ_ENABLE_OTA
    fmsk |= 1 << FMSK_OTA_Pos;
#endif

    if(g_auth.dyn_update_device_secret == true || g_auth.device_secret_len == DEVICE_SECRET_LEN){
        BREEZE_LOG_INFO("Breeze auth type: security per device.\n");
        fmsk |= 1 << FMSK_SECRET_TYPE_Pos;
    } else{
        BREEZE_LOG_INFO("Breeze auth type: security per product.\n");
        fmsk &= ~(1 << FMSK_SECRET_TYPE_Pos);
    }

#ifdef CONFIG_AIS_SECURE_ADV
    fmsk |= 1 << FMSK_SIGNED_ADV_Pos;
#endif
    g_core.adv_data[i++] = fmsk;
    SET_U32_LE(g_core.adv_data + i, model_id);
    i += sizeof(uint32_t);

    memcpy(&g_core.adv_data[i], mac_bin, 6);
    i += 6;
    g_core.adv_data_len = i;
}

static uint32_t tx_func_indicate(uint8_t cmd, uint8_t *p_data, uint16_t length)
{
    return transport_tx(TX_INDICATION, cmd, p_data, length);
}

static void ais_init_done(uint8_t res)
{
    int err = AIS_ERR_SUCCESS;
    uint8_t  mac_be[BLE_MAC_LEN];
    uint32_t size;
    ais_adv_init_t adv_data = {
        .flag = AIS_AD_GENERAL | AIS_AD_NO_BREDR,
        .name = { .ntype = AIS_ADV_NAME_FULL, .name = "AZ" },
    };

    if (res == 0) {
        ble_get_mac(mac_be);
        transport_init(g_ali_init);
        extcmd_init(g_ali_init, tx_func_indicate);
        create_bz_adv_data(g_ali_init->model_id, mac_be);
        adv_data.vdata.len = sizeof(adv_data.vdata.data);
        if (get_bz_adv_data(adv_data.vdata.data, &(adv_data.vdata.len))) {
            BREEZE_LOG_ERR("%s %d fail.\r\n", __func__, __LINE__);
            err = AIS_ERR_INVALID_ADV_DATA;
            goto exit;
        }

        /* append user adv data if any. */
        if (g_ali_init->user_adv_len > 0) {
            size = sizeof(adv_data.vdata.data) - adv_data.vdata.len;
            if (size < g_ali_init->user_adv_len) {
                BREEZE_LOG_ERR("Warning: no space for user adv data (expected %u but"
                               " only %u left). No user adv data set!!!\r\n",
                               g_ali_init->user_adv_len, size);
            } else {
                memcpy(adv_data.vdata.data + adv_data.vdata.len,
                       g_ali_init->user_adv_data, g_ali_init->user_adv_len);
                adv_data.vdata.len += g_ali_init->user_adv_len;
            }
        }
        ble_advertising_start(&adv_data);
    }

exit:
    return;
}

static uint32_t ais_init(ali_init_t const *p_init)
{
    ret_code_t ret = BZ_SUCCESS;
    ble_ais_init_t init_ais;

    memset(&init_ais, 0, sizeof(ble_ais_init_t));
    init_ais.mtu = p_init->max_mtu;
    init_ais.init_done = ais_init_done;
    g_ali_init = p_init;
#if BZ_ENABLE_AUTH
    /*this is workaroud for asyn mechanism, auth init should be done first, since data
     * structures :product key, secret, device name parameters are passed by local variable, p_init,
     * if we do in asyn callback, ais_init_done, stack of g_ali_init will be corrupted.
     * */
    ret = auth_init(g_ali_init, tx_func_indicate);
    if(ret != BZ_SUCCESS){
        BREEZE_LOG_ERR("AUTH init failed(%d) %s\n", __func__, ret);
        return ret;
    }
#endif

    return ble_ais_init(&init_ais);
}

#ifdef CONFIG_AIS_SECURE_ADV
static void update_seq(void *arg1, void *arg2)
{
    os_kv_set(AIS_SEQ_KV_KEY, &g_seq, sizeof(g_seq), 1);
    os_timer_start(&g_secadv_timer);
}

static void init_seq_number(uint32_t *seq)
{
    int len = sizeof(uint32_t);

    if (!seq)
        return;

    if (os_kv_get(AIS_SEQ_KV_KEY, seq, &len) != 0) {
        *seq = 0;
        len  = sizeof(uint32_t);
        os_kv_set(AIS_SEQ_KV_KEY, seq, len, 1);
    }

    os_timer_new(&g_secadv_timer, update_seq, NULL, AIS_SEQ_UPDATE_FREQ);
    os_timer_start(&g_secadv_timer);
}
#endif

ret_code_t core_init(ali_init_t const *p_init)
{
    memset(&g_core, 0, sizeof(core_t));
    g_core.event_handler = p_init->event_handler;

#ifdef CONFIG_AIS_SECURE_ADV
    init_seq_number(&g_seq);
#endif

    ais_init(p_init);
    return BZ_SUCCESS;
}


void core_reset(void)
{
#if BZ_ENABLE_AUTH
    auth_reset();
#endif
    transport_reset();
}

void core_handle_err(uint8_t src, uint8_t code)
{
    uint8_t err;

    BREEZE_LOG_ERR("err at %04x, code %04x\r\n", src, code);
    switch (src & BZ_ERR_MASK) {
        case BZ_TRANS_ERR:
            if (code != BZ_EINTERNAL) {
                if (src == ALI_ERROR_SRC_TRANSPORT_FW_DATA_DISC) {
                    event_notify(BZ_EVENT_ERR_DISCONT, NULL, 0);
                }
                err = transport_tx(TX_NOTIFICATION, BZ_CMD_ERR, NULL, 0);
                if (err != BZ_SUCCESS) {
                    BREEZE_LOG_ERR("err at %04x, code %04x\r\n", ALI_ERROR_SRC_TRANSPORT_SEND, code);
                }
            }
            break;
        case BZ_AUTH_ERR:
#if BZ_ENABLE_AUTH
            auth_reset();
#endif
            if (code == BZ_ETIMEOUT) {
                ble_disconnect(AIS_BT_REASON_REMOTE_USER_TERM_CONN);
            }
            break;
        case BZ_EXTCMD_ERR:
            break;
        default:
            BREEZE_LOG_ERR("unknow bz err\r\n");
            break;
    }
}

ret_code_t get_bz_adv_data(uint8_t *p_data, uint16_t *length)
{
#ifdef CONFIG_AIS_SECURE_ADV
    if (*length < (g_core.adv_data_len + 4 + 4)) {
#else
    if (*length < g_core.adv_data_len) {
#endif
        return BZ_ENOMEM;
    }

#ifdef CONFIG_AIS_SECURE_ADV
    uint8_t  sign[4];
    uint32_t seq;

    seq = (++g_seq);
#if BZ_ENABLE_AUTH
    auth_calc_adv_sign(seq, sign);
#endif
    memcpy(p_data, g_core.adv_data, g_core.adv_data_len);
    memcpy(p_data + g_core.adv_data_len, sign, 4);
    memcpy(p_data + g_core.adv_data_len + 4, &seq, 4);
    *length = g_core.adv_data_len + 4 + 4;
#else
    memcpy(p_data, g_core.adv_data, g_core.adv_data_len);
    *length = g_core.adv_data_len;
#endif

    return BZ_SUCCESS;
}

#ifdef CONFIG_AIS_SECURE_ADV
void set_adv_sequence(uint32_t seq)
{
    g_seq = seq;
    os_kv_set(AIS_SEQ_KV_KEY, &g_seq, sizeof(g_seq), 1);
}
#endif
