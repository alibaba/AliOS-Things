/*
 * Copyright (C) 2018-2020 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <string.h>
#include <genie_provision.h>
#include "genie_service.h"

#ifdef GENIE_ULTRA_PROV
#include <errno.h>
#include <tinycrypt/sha256.h>
#include <tinycrypt/constants.h>

#include <net/buf.h>
#include "crypto.h"
#include "adv.h"

#define BUF_TIMEOUT K_MSEC(400)
#define BT_MESH_ADV(buf) (*(struct bt_mesh_adv **)net_buf_user_data(buf))

static uint8_t p_prov_data_key[32];
static uint8_t p_ultra_prov_devkey[32];
static uint8_t p_confirm[16];
#endif

#define BT_DBG_ENABLED IS_ENABLED(CONFIG_BT_MESH_DEBUG_PROV)
#include "common/log.h"

static struct k_timer pbadv_timeout_timer;
static struct k_timer prov_timeout_timer;

static genie_provision_state_e provision_state = GENIE_PROVISION_UNPROV;
static uint8_t genie_uuid[GENIE_PROVISON_UUID_LEN];

static void genie_provision_set_silent_flag(void)
{
    genie_uuid[13] |= UNPROV_ADV_FEATURE1_SILENT_ADV;
}

void genie_provision_clear_silent_flag(void)
{
    genie_uuid[13] &= UNPROV_ADV_FEATURE1_SILENT_UNMASK;
}

int genie_provision_set_state(genie_provision_state_e state)
{
    provision_state = state;

    return 0;
}

genie_provision_state_e genie_provision_get_state(void)
{
    return provision_state;
}

uint8_t *genie_provision_get_uuid(void)
{
    int i = 0;
    genie_triple_t *p_genie_triple = NULL;

    p_genie_triple = genie_triple_get();

    // all fields in uuid should be in little-endian
    // CID: Taobao
    genie_uuid[0] = CONFIG_MESH_VENDOR_COMPANY_ID & 0xFF;
    genie_uuid[1] = (CONFIG_MESH_VENDOR_COMPANY_ID >> 8) & 0xFF;

    // PID
    // Bit0~Bit3: 0001 (broadcast version)
    // Bit4：1 (one secret pre device)
    // Bit5: 1 (OTA support)
    // Bit6~Bit7: 01 (00:4.0 01:4.2 10:5.0 11:>5.0)
    genie_uuid[2] = 0x71;

    // Product ID
    for (i = 0; i < 4; i++)
    {
        genie_uuid[3 + i] = (p_genie_triple->pid >> (i << 3)) & 0xFF;
    }

    // mac addr (device name)
    for (i = 0; i < GENIE_TRIPLE_MAC_SIZE; i++)
    {
        genie_uuid[7 + i] = p_genie_triple->mac[GENIE_TRIPLE_MAC_SIZE - 1 - i];
    }

    genie_uuid[13] = UNPROV_ADV_FEATURE1_UUID_VERSION;

#if defined(BOARD_TG7100B) 
    genie_uuid[14] = UNPROV_ADV_FEATURE2_GENIE_MESH_STACK_V1;
#elif defined(BOARD_PHY6220_EVB) || defined(BOARD_TG7120B_EVB)
    genie_uuid[14] = UNPROV_ADV_FEATURE2_GENIE_MESH_STACK_V2;
#elif defined(BOARD_TG7121B_EVB)
    genie_uuid[14] = UNPROV_ADV_FEATURE2_GENIE_MESH_STACK_V3;
#endif

#ifdef GENIE_ULTRA_PROV
    genie_uuid[14] |= UNPROV_ADV_FEATURE2_ULTRA_PROV_FLAG;
    genie_uuid[14] |= UNPROV_ADV_FEATURE2_AUTH_FLAG;
#else
#ifndef GENIE_OLD_AUTH
    genie_uuid[14] |= UNPROV_ADV_FEATURE2_AUTH_FLAG;
#endif
#endif

    BT_INFO("uuid: %s", bt_hex(genie_uuid, GENIE_PROVISON_UUID_LEN));

    return genie_uuid;
}

int genie_provision_get_saved_data(genie_provision_t *p_genie_provision)
{
    genie_storage_status_e ret = GENIE_STORAGE_SUCCESS;

    if (p_genie_provision == NULL)
    {
        return -1;
    }

    memset(p_genie_provision, 0, sizeof(genie_provision_t));

    ret = genie_storage_read_addr(&p_genie_provision->addr);
    if (ret != GENIE_STORAGE_SUCCESS)
    {
        return ret;
    }

    ret = genie_storage_read_seq(&p_genie_provision->seq);
    if (ret != GENIE_STORAGE_SUCCESS)
    {
        return ret;
    }

    ret = genie_storage_read_devkey(p_genie_provision->devkey);
    if (ret != GENIE_STORAGE_SUCCESS)
    {
        return ret;
    }

    ret = genie_storage_read_netkey(&p_genie_provision->netkey);
    if (ret != GENIE_STORAGE_SUCCESS)
    {
        return ret;
    }

    ret = genie_storage_read_appkey(&p_genie_provision->appkey);
    if (ret != GENIE_STORAGE_SUCCESS)
    {
        return ret;
    }

    return 0;
}

#ifdef GENIE_ULTRA_PROV
void ultra_prov_get_auth(const uint8_t random_hex[16], const uint8_t key[16], uint8_t cfm[16])
{
    bt_mesh_prov_conf(key, random_hex, genie_crypto_get_auth(random_hex), cfm);

    BT_DBG("cfm: %s", bt_hex(cfm, 16));
}

static int _ultra_prov_send(uint8_t type, uint8_t *p_msg, uint8_t len)
{
    struct net_buf *buf;

    buf = bt_mesh_adv_create(0, 0, BUF_TIMEOUT);
    if (!buf)
    {
        BT_ERR("Out of provisioning buffers");
        return -ENOBUFS;
    }

    BT_MESH_ADV(buf)->tiny_adv = 1;

    net_buf_add_be16(buf, CONFIG_MESH_VENDOR_COMPANY_ID);
    //VID
    net_buf_add_u8(buf, 0x0d);
    net_buf_add_u8(buf, type);
    net_buf_add_mem(buf, p_msg, len);

    BT_DBG("send %s", bt_hex(buf->data, buf->len));

    bt_mesh_adv_send(buf, NULL, NULL);
    net_buf_unref(buf);

    return 0;
}

static void genie_provision_ultra_prov_recv_random(uint8_t *buf)
{
    uint8_t tmp[48];
    uint8_t random_a[17];
    uint8_t random_b[17];
    uint8_t cfm_key[32];

    int ret;
    struct tc_sha256_state_struct sha256_ctx;
    genie_triple_t *p_genie_triple = genie_triple_get();

    if (buf[0] != p_genie_triple->mac[4] || buf[1] != p_genie_triple->mac[5])
    {
        return;
    }
    buf += 2;

    genie_event(GENIE_EVT_SDK_MESH_PROV_START, NULL);

    memset(p_prov_data_key, 0, sizeof(p_prov_data_key));
    memset(p_ultra_prov_devkey, 0, sizeof(p_ultra_prov_devkey));
    memset(p_confirm, 0, sizeof(p_confirm));

    hextostring(buf, (char *)random_a, 8);
    random_a[16] = 0;
    hextostring(buf + 8, (char *)random_b, 8);
    random_b[16] = 0;

    sprintf((char *)tmp, "%s%sConfirmationKey", random_a, random_b);

    BT_DBG("string %s\n", tmp);

    /* calculate the sha256 of random and
         * fetch the top 16 bytes as confirmationKey
         */
    ret = tc_sha256_init(&sha256_ctx);
    if (ret != TC_CRYPTO_SUCCESS)
    {
        BT_ERR("sha256 init fail\n");
    }

    ret = tc_sha256_update(&sha256_ctx, tmp, 47);
    if (ret != TC_CRYPTO_SUCCESS)
    {
        BT_ERR("sha256 udpate fail\n");
    }

    ret = tc_sha256_final(cfm_key, &sha256_ctx);
    if (ret != TC_CRYPTO_SUCCESS)
    {
        BT_ERR("sha256 final fail\n");
    }
    else
    {
        BT_DBG("cfmKey: %s", bt_hex(cfm_key, 16));
    }

    //calc cloud cfm
    memcpy(tmp, buf + 8, 8);
    memcpy(tmp + 8, buf, 8);
    ultra_prov_get_auth(tmp, cfm_key, p_confirm);

    hextostring(p_confirm, (char *)tmp, 16);
    sprintf((char *)tmp + 32, "SessionKey");
    BT_DBG("tmp %s\n", tmp);
    ret = tc_sha256_init(&sha256_ctx);
    if (ret != TC_CRYPTO_SUCCESS)
    {
        BT_ERR("sha256 init fail\n");
    }

    ret = tc_sha256_update(&sha256_ctx, tmp, 42);
    if (ret != TC_CRYPTO_SUCCESS)
    {
        BT_ERR("sha256 udpate fail\n");
    }

    ret = tc_sha256_final(p_prov_data_key, &sha256_ctx);
    if (ret != TC_CRYPTO_SUCCESS)
    {
        BT_ERR("sha256 final fail\n");
    }
    else
    {
        BT_DBG("provKkey: %s", bt_hex(p_prov_data_key, 22));
    }

    //calc dev key
    sprintf((char *)tmp + 32, "DeviceKey");
    BT_DBG("tmp %s\n", tmp);
    ret = tc_sha256_init(&sha256_ctx);
    if (ret != TC_CRYPTO_SUCCESS)
    {
        BT_ERR("sha256 init fail\n");
    }

    ret = tc_sha256_update(&sha256_ctx, tmp, 41);
    if (ret != TC_CRYPTO_SUCCESS)
    {
        BT_ERR("sha256 udpate fail\n");
    }

    ret = tc_sha256_final(p_ultra_prov_devkey, &sha256_ctx);
    if (ret != TC_CRYPTO_SUCCESS)
    {
        BT_ERR("sha256 final fail\n");
    }
    else
    {
        BT_INFO("dk %s", bt_hex(p_ultra_prov_devkey, 16));
    }

    memcpy(tmp, buf, 16);
    ultra_prov_get_auth(tmp, cfm_key, p_confirm);

    tmp[0] = p_genie_triple->mac[4];
    tmp[1] = p_genie_triple->mac[5];
    memcpy(tmp + 2, p_confirm, 16);

    _ultra_prov_send(GENIE_PROV_STATUS_SEND_CONFIRM_DATA, tmp, 18);
}

static void genie_provision_ultra_prov_recv_data(uint8_t *buf)
{
    uint8_t i = 0;
    uint16_t net_idx;
    uint8_t flags;
    uint32_t iv_index;
    uint16_t addr;
    genie_triple_t *p_genie_triple = genie_triple_get();

    if (genie_provision_get_state() != GENIE_PROVISION_START)
    {
        BT_WARN("I not in provisioning");
        return;
    }

    while (i < 22)
    {
        buf[i] ^= p_prov_data_key[i];
        i++;
    }
    BT_DBG("%s", bt_hex(buf, 22));

    if (buf[0] != p_genie_triple->mac[4] || buf[1] != p_genie_triple->mac[5])
    {
        return;
    }
    buf += 2;

    flags = buf[0];
    net_idx = 0;
    iv_index = buf[17];
    addr = sys_get_be16(&buf[18]);

    BT_DBG("net_idx %u iv_index 0x%08x, addr 0x%04x",
           net_idx, iv_index, addr);

    genie_event(GENIE_EVT_SDK_MESH_PROV_DATA, &addr);
    BT_INFO("nk %s", bt_hex(buf + 1, 16));
    bt_mesh_provision(buf + 1, net_idx, flags, iv_index, addr, p_ultra_prov_devkey);

    _ultra_prov_send(GENIE_PROV_STATUS_SEND_COMPLETED, p_genie_triple->mac, 6);
}

int genie_provision_ultra_prov_handle(uint8_t frame_type, void *frame_buf)
{
    uint16_t company = 0;
    uint8_t fixed_byte = 0;
    uint8_t prov_cmd = 0;
    struct net_buf_simple *buf = NULL;

    buf = (struct net_buf_simple *)frame_buf;
    if (frame_type != GENIE_PROV_ADV_TYPE || buf == NULL)
    {
        return -1;
    }

    company = net_buf_simple_pull_be16(buf);
    if (company == CONFIG_MESH_VENDOR_COMPANY_ID)
    {
        fixed_byte = net_buf_simple_pull_u8(buf);
        if (fixed_byte == GENIE_PROV_FIXED_BYTE)
        {
            prov_cmd = net_buf_simple_pull_u8(buf);
            switch (prov_cmd)
            {
            case GENIE_PROV_STATUS_RECV_RANDOM:
                genie_provision_ultra_prov_recv_random(buf->data);
                break;
            case GENIE_PROV_STATUS_RECV_PROV_DATA:
                genie_provision_ultra_prov_recv_data(buf->data);
                break;
            default:
                break;
            }
        }
    }

    return 0;
}
#endif

static void _genie_pbadv_timer_cb(void *p_timer, void *args)
{
    genie_event(GENIE_EVT_SDK_MESH_PBADV_TIMEOUT, NULL);
}

void genie_provision_pbadv_timer_start(uint32_t prov_timeout)
{
    static uint8_t inited = 0;

    if (!inited)
    {
        k_timer_init(&pbadv_timeout_timer, _genie_pbadv_timer_cb, NULL);
        inited = 1;
    }

    k_timer_start(&pbadv_timeout_timer, prov_timeout);
}

void genie_provision_pbadv_timer_stop(void)
{
    k_timer_stop(&pbadv_timeout_timer);
}

void genie_provision_start_slient_pbadv(void)
{
    genie_provision_set_silent_flag();
    //bt_mesh_prov_disable(BT_MESH_PROV_GATT | BT_MESH_PROV_ADV);
    extern void genie_set_silent_unprov_beacon_interval(bool is_silent);
    genie_set_silent_unprov_beacon_interval(true);
    bt_mesh_prov_enable(BT_MESH_PROV_ADV);
}

static void _genie_prov_timer_cb(void *p_timer, void *args)
{
    genie_event(GENIE_EVT_SDK_MESH_PROV_TIMEOUT, NULL);
}

void genie_provision_prov_timer_start(void)
{
    static uint8_t inited = 0;

    if (!inited)
    {
        k_timer_init(&prov_timeout_timer, _genie_prov_timer_cb, NULL);
        inited = 1;
    }
    k_timer_start(&prov_timeout_timer, MESH_PROVISIONING_TIMEOUT);
}

void genie_provision_prov_timer_stop(void)
{
    k_timer_stop(&prov_timeout_timer);
}
