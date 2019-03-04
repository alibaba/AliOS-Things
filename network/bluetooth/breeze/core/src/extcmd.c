/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <string.h>
#include "extcmd.h"
#include "auth.h"
#include "common.h"
#include "core.h"
#include "utils.h"
#include "breeze_export.h"
#include "chip_code.h"
#include "bzopt.h"

#include <breeze_hal_os.h>
#include <breeze_hal_sec.h>

#define RANDOM_LEN 16
#define SHA256_DATA_LEN 32

#define SSID_READY 0x01
#define PASSWORD_READY 0x02
#define BSSID_READY 0x04
#define ALL_READY (SSID_READY | PASSWORD_READY | BSSID_READY)

#define UTF8_MAX_SSID     32
#define UTF8_MAX_PASSWORD 64

enum {
    BLE_AWSS_CTYPE_SSID     = 0x01,
    BLE_AWSS_CTYPE_PASSWORD = 0x02,
    BLE_AWSS_CTYPE_BSSID    = 0x03,
};

breeze_apinfo_t comboinfo;
#define CLIENTID_STR "clientId"
#define DEVICE_NAME_STR "deviceName"
#define DEVICE_SECRET_STR "deviceSecret"
#define PRODUCT_SECRET_STR "productSecret"
#define PRODUCT_KEY_STR "productKey"
extern uint8_t product_secret[PRODUCT_SECRET_LEN];

const static char m_sdk_version[] = ":" BZ_VERSION;

typedef ret_code_t (*ext_tlv_handler_t)(uint8_t *p_buff, uint8_t *p_blen,
                                        const uint8_t *p_data, uint8_t dlen);

extcmd_t g_extcmd;

typedef struct {
    uint8_t tlv_type;
    ext_tlv_handler_t handler;
} ext_tlv_type_handler_t;

static ret_code_t ext_cmd01_rsp(uint8_t *p_buff, uint8_t *p_blen, const uint8_t *p_data, uint8_t dlen);
static ret_code_t ext_cmd02_rsp(uint8_t *p_buff, uint8_t *p_blen, const uint8_t *p_data, uint8_t dlen);
static ret_code_t ext_cmd03_rsp(uint8_t *p_buff, uint8_t *p_blen, const uint8_t *p_data, uint8_t dlen);
static ret_code_t ext_cmd04_rsp(uint8_t *p_buff, uint8_t *p_blen, const uint8_t *p_data, uint8_t dlen);
static ret_code_t ext_cmd05_rsp(uint8_t *p_buff, uint8_t *p_blen, const uint8_t *p_data, uint8_t dlen);
static ret_code_t ext_cmd06_rsp(uint8_t *p_buff, uint8_t *p_blen, const uint8_t *p_data, uint8_t dlen);
#ifdef CONFIG_AIS_SECURE_ADV
static ret_code_t ext_cmd07_rsp(uint8_t *p_buff, uint8_t *p_blen, const uint8_t *p_data, uint8_t dlen);
#endif

static const ext_tlv_type_handler_t
  m_tlv_type_handler_table[] = /**< TLV type handler table. */
  { { 0x01, ext_cmd01_rsp}, { 0x02, ext_cmd02_rsp},
    { 0x03, ext_cmd03_rsp}, { 0x04, ext_cmd04_rsp},
    { 0x05, ext_cmd05_rsp},
#if BZ_ENABLE_COMBO_NET
    { 0x06, ext_cmd06_rsp},
#endif
#ifdef CONFIG_AIS_SECURE_ADV
    { 0x07, ext_cmd07_rsp}
#endif
  };

static ret_code_t ext_cmd01_rsp(uint8_t *p_buff, uint8_t *p_blen, const uint8_t *p_data, uint8_t dlen)
{
    ret_code_t err_code = BZ_ENOMEM;
    uint8_t len;

    if (dlen > 0) {
        err_code = BZ_EDATASIZE;
    } else if ((len = g_extcmd.tlv_01_rsp_len) <= *p_blen) {
        memcpy(p_buff, g_extcmd.tlv_01_rsp, len);
        *p_blen = len;
        err_code = BZ_SUCCESS;
    }

    return err_code;
}

static ret_code_t ext_cmd02_rsp(uint8_t *p_buff, uint8_t *p_blen, const uint8_t *p_data, uint8_t dlen)
{
    ret_code_t err_code = BZ_ENOMEM;
    uint8_t len;

    if (dlen > 0) {
        err_code = BZ_EDATASIZE;
    } else if (g_extcmd.product_key_len == 0) {
        err_code = BZ_ENOTSUPPORTED;
    } else if ((len = g_extcmd.product_key_len) <= *p_blen) {
        memcpy(p_buff, g_extcmd.p_product_key, len);
        *p_blen = len;

        err_code = BZ_SUCCESS;
    }

    return err_code;
}


static ret_code_t ext_cmd03_rsp(uint8_t *p_buff, uint8_t *p_blen, const uint8_t *p_data, uint8_t dlen)
{
    ret_code_t err_code = BZ_ENOMEM;
    uint8_t len;

    if (dlen > 0) {
        err_code = BZ_EDATASIZE;
    } else if (g_extcmd.device_name_len == 0) {
        err_code = BZ_ENOTSUPPORTED;
    } else if ((len = g_extcmd.device_name_len) <= *p_blen) {
        memcpy(p_buff, g_extcmd.p_device_name, len);
        *p_blen = len;

        err_code = BZ_SUCCESS;
    }

    return err_code;
}

static ret_code_t ext_cmd04_rsp(uint8_t *p_buff, uint8_t *p_blen, const uint8_t *p_data, uint8_t dlen)
{
    ret_code_t err_code = BZ_ENOMEM;

    if (dlen > 0) {
        err_code = BZ_EDATASIZE;
    } else if (g_extcmd.secret_len == 0) {
        err_code = BZ_ENOTSUPPORTED;
    } else if (RANDOM_LEN <= *p_blen) {
        get_random(p_buff, RANDOM_LEN);
        *p_blen = RANDOM_LEN;
        err_code = BZ_SUCCESS;
    }
    return err_code;
}

static void network_signature_calculate(uint8_t *p_buff)
{
    uint8_t str_id[8], n;
    uint8_t random_str[32];
    SHA256_CTX context;
    uint8_t cli_id[4];

    SET_U32_BE(cli_id, g_extcmd.model_id);
    hex2string(cli_id, sizeof(cli_id), str_id);
    sec_sha256_init(&context);

    sec_sha256_update(&context, CLIENTID_STR, strlen(CLIENTID_STR));
    sec_sha256_update(&context, str_id, sizeof(str_id));

    sec_sha256_update(&context, DEVICE_NAME_STR, strlen(DEVICE_NAME_STR)); /* "deviceName" */
    sec_sha256_update(&context, g_extcmd.p_device_name, g_extcmd.device_name_len);

#ifndef CONFIG_MODEL_SECURITY
    sec_sha256_update(&context, DEVICE_SECRET_STR, strlen(DEVICE_SECRET_STR)); /* "deviceSecret" */
    sec_sha256_update(&context, g_extcmd.p_secret, g_extcmd.secret_len);
#else
    sec_sha256_update(&context, PRODUCT_SECRET_STR, strlen(PRODUCT_SECRET_STR)); /* "productSecret" */
    sec_sha256_update(&context, g_extcmd.p_secret, g_extcmd.secret_len);
#endif

    sec_sha256_update(&context, PRODUCT_KEY_STR, strlen( PRODUCT_KEY_STR)); /* "productKey" */
    sec_sha256_update(&context, g_extcmd.p_product_key, g_extcmd.product_key_len);

    sec_sha256_final(&context, p_buff);
}

static ret_code_t ext_cmd05_rsp(uint8_t *p_buff, uint8_t *p_blen, const uint8_t *p_data, uint8_t dlen)
{
    ret_code_t err_code = BZ_ENOMEM;

    if (dlen > 0) {
        err_code = BZ_EDATASIZE;
    } else if (*p_blen >= SHA256_DATA_LEN) {
        network_signature_calculate(p_buff);
        *p_blen = SHA256_DATA_LEN;
        err_code = BZ_SUCCESS;
    }

    return err_code;
}

#if BZ_ENABLE_COMBO_NET
static ret_code_t ext_cmd06_rsp(uint8_t *p_buff, uint8_t *p_blen, const uint8_t *p_data, uint8_t dlen)
{

    ret_code_t err_code = BZ_SUCCESS;
    uint8_t idx = 0, tlvtype, tlvlen;
    static uint8_t ready_flag = 0;
    uint8_t rsp[] = { 0x01, 0x01, 0x01 };

    if (dlen < 2) {
        err_code = BZ_EDATASIZE;
        goto end;
    }

    if (*p_blen < sizeof(rsp)) {
        err_code = BZ_ENOMEM;
        goto end;
    }

    while (idx < dlen) {
        tlvtype = p_data[idx++];
        tlvlen  = p_data[idx++];

        switch (tlvtype) {
            case BLE_AWSS_CTYPE_SSID: /* utf8 */
                if (tlvlen < 1 || tlvlen > UTF8_MAX_SSID) {
                    err_code = BZ_EINVALIDTLV;
                    goto end;
                }
                utf8_to_pw(&p_data[idx], tlvlen, comboinfo.ssid);
                ready_flag |= SSID_READY;
                break;
            case BLE_AWSS_CTYPE_PASSWORD: /* utf8 */
                if (tlvlen > UTF8_MAX_PASSWORD) {
                    err_code = BZ_EINVALIDTLV;
                    goto end;
                }
                utf8_to_pw(&p_data[idx], tlvlen, comboinfo.pw);
                ready_flag |= PASSWORD_READY;
                break;
            case BLE_AWSS_CTYPE_BSSID: /* 6-byte hex */
                if (tlvlen != 6) {
                    err_code = BZ_EINVALIDTLV;
                    goto end;
                }
                memcpy(comboinfo.bssid, &p_data[idx], tlvlen);
                ready_flag |= BSSID_READY;
                break;
            default:
                err_code = BZ_EINVALIDTLV;
                goto end;
                break;
        }

        idx += tlvlen;
    }

end:
    if (err_code != BZ_SUCCESS) {
        rsp[2] = 2; /* set failure code */
    }

    /* rsp */
    memcpy(p_buff, rsp, sizeof(rsp));
    *p_blen = sizeof(rsp);

    if (ready_flag & ALL_READY) {
        event_notify(BZ_EVENT_APINFO, &comboinfo, sizeof(comboinfo));
    }

    return err_code;
}
#endif

#ifdef CONFIG_AIS_SECURE_ADV
static ret_code_t ext_cmd07_rsp(uint8_t *p_buff, uint8_t *p_blen, const uint8_t *p_data, uint8_t dlen)
{
    ret_code_t err_code = BZ_SUCCESS;
    uint8_t ret_code = 1, i = 0;
    uint32_t seq = 0;

    if (dlen != sizeof(seq)) {
        BREEZE_LOG_ERR("Error: invalid sequence data size\r\n");
        err_code = BZ_EDATASIZE;
        goto end;
    }

    if (*p_blen < 1) {
        BREEZE_LOG_ERR("Error: invalid sequence rsp buffer size\r\n");
        err_code = BZ_ENOMEM;
        goto end;
    }

    BREEZE_LOG_INFO("The sequence bytes from cloud: %02x %02x %02x %02x\r\n", p_data[0],
           p_data[1], p_data[2], p_data[3]);

    while (i < sizeof(seq)) {
        seq |= p_data[i] << (i << 3);
        i++;
    }

    BREEZE_LOG_INFO("The sequence hex to be saved is %08x\r\n", seq);
    set_adv_sequence(seq);

end:
    if (err_code != BZ_SUCCESS) {
        ret_code = 0; /* set failure code */
    }

    /* rsp */
    p_buff[0] = ret_code;
    *p_blen   = 1;

    return err_code;
}
#endif

static void get_os_info(void)
{
    uint8_t chip_code[4] = { 0 };
    uint8_t chip_id_str[8] = { 0 };
    const char *aostype = "AOS";
    uint8_t suffix_len = 0;
    char t_os_info[20] = { 0 };

#ifdef BUILD_AOS
    strcpy(t_os_info, aos_version_get());
    char *m_os_type = strtok(t_os_info, "-");
    if (strcmp(aostype, m_os_type) == 0) {
        m_os_type = strtok(NULL, "-");
        m_os_type = strtok(NULL, "-");
        strcat(m_os_type, ":");
        BREEZE_LOG_INFO("AOS version %s(%d)\n", m_os_type, strlen(m_os_type));

        suffix_len = strlen(m_os_type);
        memcpy(g_extcmd.tlv_01_rsp, m_os_type, suffix_len);
        chip_code_st *p_chip_code_obj = get_chip_code(MCU_FAMILY);
        if (p_chip_code_obj != NULL) {
            chip_code[0] = (uint8_t)(p_chip_code_obj->vendor >> 8);
            chip_code[1] = (uint8_t)p_chip_code_obj->vendor;
            chip_code[2] = (uint8_t)(p_chip_code_obj->id >> 8);
            chip_code[3] = (uint8_t)p_chip_code_obj->id;
        }

        hex2string(chip_code, sizeof(chip_code), chip_id_str);
        memcpy(g_extcmd.tlv_01_rsp + suffix_len, chip_id_str, sizeof(chip_id_str));
        suffix_len += sizeof(chip_id_str);
        memcpy(g_extcmd.tlv_01_rsp + suffix_len, m_sdk_version, sizeof(m_sdk_version) - 1);
        suffix_len += sizeof(m_sdk_version) - 1;
        g_extcmd.tlv_01_rsp[suffix_len] = '\0';
        strcat(g_extcmd.tlv_01_rsp, ":1");
        suffix_len = strlen(g_extcmd.tlv_01_rsp);
    }
#else
    memcpy(g_extcmd.tlv_01_rsp, "NON-AOS", strlen("NON-AOS"));
    g_extcmd.tlv_01_rsp[suffix_len] = '\0';
    suffix_len = strlen("NON-AOS");
#endif
    g_extcmd.tlv_01_rsp_len = suffix_len;
}


ret_code_t extcmd_init(ali_init_t const *p_init, tx_func_t tx_func)
{
    memset(&g_extcmd, 0, sizeof(extcmd_t));
    get_os_info();
#if BZ_ENABLE_AUTH
    auth_get_device_name(&g_extcmd.p_device_name, &g_extcmd.device_name_len);
    auth_get_product_key(&g_extcmd.p_product_key, &g_extcmd.product_key_len);
    auth_get_secret(&g_extcmd.p_secret, &g_extcmd.secret_len);
#endif

    g_extcmd.tx_func = tx_func;
    g_extcmd.model_id = p_init->model_id;
    return BZ_SUCCESS;
}

void extcmd_rx_command(uint8_t cmd, uint8_t *p_data, uint16_t length)
{
    if (length == 0 || cmd != BZ_CMD_EXT_DOWN) {
        return;
    }

    uint8_t *p_tx_buff = g_extcmd.tx_buff;
    uint8_t tx_buff_avail = sizeof(g_extcmd.tx_buff);
    uint8_t tx_buff_size;
    uint8_t tlv_mask, tlv_masked = 0;
    uint8_t tlv_type, tlv_len;

    uint32_t err_code = BZ_SUCCESS;

#if BZ_ENABLE_AUTH
    if (!auth_is_authdone()) {
        err_code = BZ_EINVALIDSTATE;
    }
#endif

    while (length > 0 && err_code == BZ_SUCCESS) {
        if (length >= 2) {
            /* get TLV type. */
            tlv_type = *p_data++;
            /* get TLV length. */
            tlv_len = *p_data++;

            length -= 2;
        } else {
            err_code = BZ_EINVALIDLEN;
            break;
        }

        /* each TLV type should not get repeated. */
        tlv_mask = (1 << tlv_type);
        if ((tlv_mask & tlv_masked) != 0) {
            err_code = BZ_EINVALIDDATA;
            break;
        }
        tlv_masked |= tlv_mask;

        /* check that TLV length does not exceed input data boundary. */
        if (tlv_len > length) {
            err_code = BZ_EDATASIZE;
            break;
        }

        if (tx_buff_avail < 2) {
            err_code = BZ_ENOMEM;
            break;
        }

        uint32_t n, n_max = sizeof(m_tlv_type_handler_table) / sizeof(ext_tlv_type_handler_t);
        for (n = 0; n < n_max; n++) {
            if (m_tlv_type_handler_table[n].tlv_type == tlv_type) {
                tx_buff_size = tx_buff_avail - 2;
                err_code = m_tlv_type_handler_table[n].handler(p_tx_buff + 2, &tx_buff_size, p_data, tlv_len);
                break;
            }
        }
        if (n >= n_max) {
            err_code = BZ_EINVALIDDATA;
        }

        if (err_code == BZ_SUCCESS) {
            *(p_tx_buff + 0) = tlv_type;
            *(p_tx_buff + 1) = tx_buff_size;

            p_tx_buff += (2 + tx_buff_size);
            tx_buff_avail -= (2 + tx_buff_size);

            p_data += tlv_len;
            length -= tlv_len;
        }
    }

    if (err_code == BZ_SUCCESS) {
        err_code = g_extcmd.tx_func(BZ_CMD_EXT_UP, g_extcmd.tx_buff, sizeof(g_extcmd.tx_buff) - tx_buff_avail);
    } else {
        err_code = g_extcmd.tx_func(BZ_CMD_ERR, NULL, 0);
    }

    if (err_code != BZ_SUCCESS) {
        core_handle_err(ALI_ERROR_SRC_EXT_SEND_RSP, err_code);
    }
}
