/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <string.h>
#include <breeze_hal_os.h>
#include "ali_ext.h"
#include "ali_auth.h"
#include "ali_common.h"
#include "ali_core.h"
#include "sha256.h"
#include "breeze_export.h"
#include "chip_code.h"

#ifdef BREEZE51
#define BREEZE5_IC_STRING "nRF51822"
#else
#ifdef BREEZE52
#define BREEZE5_IC_STRING "nRF52832"
#else
#define BREEZE5_IC_STRING "ESP32"
#endif
#endif
#define RANDOM_LEN 16
#define SHA256_DATA_LEN 32

#define BREEZE_LOG_MODULE_NAME "ALI"

static uint8_t const m_v2sig_p1[8] =
  "clientId"; /**< V2 network signature fixed part 1. */
static uint8_t const m_v2sig_p2[10] =
  "deviceName"; /**< V2 network signature fixed part 2. */
static uint8_t const m_v2sig_p3[12] =
  "deviceSecret"; /**< V2 network signature fixed part 3. */
static uint8_t const m_v2sig_p4[10] =
  "productKey"; /**< V2 network signature fixed part 4. */
static uint8_t const m_v2sig_p5[6] =
  "random"; /**< V2 network signature fixed part 5. */

static char m_tlv_01_rsp_suffix[ALI_EXT_MAX_TLV_01_RSP_LEN] = {
    0
}; /*< TLV type 01 response suffix. */
const static char m_sdk_version[] = ":" ALI_SDK_VERSION;

ali_ext_event_t ext_evt;

/**
 * @brief TLV type handler function.
 *
 * @param[in]     p_ext         Extend module structure.
 * @param[in]     p_buff        Pointer to Tx data buffer.
 * @param[in,out] p_blen        Pointer to Tx data buffer size.
 *                              On return, this contains the used buffer size.
 * @param[in]     p_data        Pointer to Rx TLV data.
 * @param[in]     dlen          Length of Rx TLV data.
 *
 * @retval    BREEZE_SUCCESS             If TLV handling was successful.
 * @retval    BREEZE_ERROR_NOT_SUPPORTED If TLV type was not supported.
 * @retval    BREEZE_ERROR_DATA_SIZE     If invalid data size have been provided.
 * @retval    BREEZE_ERROR_NO_MEM        If Tx data buffer was inadequate for
 * placing the response.
 */
typedef ret_code_t (*ali_ext_tlv_handler_t)(ali_ext_t *p_ext, uint8_t *p_buff,
                                            uint8_t       *p_blen,
                                            const uint8_t *p_data,
                                            uint8_t        dlen);

/**@brief Structure for TLV type handler. */
typedef struct
{
    uint8_t               tlv_type; /**< TLV type. */
    ali_ext_tlv_handler_t handler;  /**< Pointer to handler function. */
} ali_ext_tlv_type_handler_t;


static ret_code_t ali_ext_01_rsp_data(ali_ext_t *p_ext, uint8_t *p_buff,
                                      uint8_t *p_blen, const uint8_t *p_data,
                                      uint8_t dlen);
static ret_code_t ali_ext_02_rsp_data(ali_ext_t *p_ext, uint8_t *p_buff,
                                      uint8_t *p_blen, const uint8_t *p_data,
                                      uint8_t dlen);
static ret_code_t ali_ext_03_rsp_data(ali_ext_t *p_ext, uint8_t *p_buff,
                                      uint8_t *p_blen, const uint8_t *p_data,
                                      uint8_t dlen);
static ret_code_t ali_ext_04_rsp_data(ali_ext_t *p_ext, uint8_t *p_buff,
                                      uint8_t *p_blen, const uint8_t *p_data,
                                      uint8_t dlen);
static ret_code_t ali_ext_05_rsp_data(ali_ext_t *p_ext, uint8_t *p_buff,
                                      uint8_t *p_blen, const uint8_t *p_data,
                                      uint8_t dlen);
static ret_code_t ali_ext_06_rsp_data(ali_ext_t *p_ext, uint8_t *p_buff,
                                      uint8_t *p_blen, const uint8_t *p_data,
                                      uint8_t dlen);
#ifdef CONFIG_AIS_SECURE_ADV
static ret_code_t ali_ext_07_rsp_data(ali_ext_t *p_ext, uint8_t *p_buff,
                                      uint8_t *p_blen, const uint8_t *p_data,
                                      uint8_t dlen);
#endif

/**@brief Convert a hex digit to ASCII character. */
static uint8_t hex2ascii(uint8_t digit)
{
    uint8_t val;

    if (digit <= 9) {
        val = digit - 0x0 + '0';
    } else {
        val = digit - 0xA + 'A';
    }

    return val;
}

static void hex2string(uint8_t *hex, uint32_t len, uint8_t *str)
{
    uint32_t i;

    for (i = 0; i < len; i++) {
        str[i * 2]     = hex2ascii(hex[i] >> 4 & 0x0f);
        str[i * 2 + 1] = hex2ascii(hex[i] & 0x0f);
    }
}

static const ali_ext_tlv_type_handler_t
  m_tlv_type_handler_table[] = /**< TLV type handler table. */
  { { 0x01, ali_ext_01_rsp_data }, { 0x02, ali_ext_02_rsp_data },
    { 0x03, ali_ext_03_rsp_data }, { 0x04, ali_ext_04_rsp_data },
    { 0x05, ali_ext_05_rsp_data }, { 0x06, ali_ext_06_rsp_data },
#ifdef CONFIG_AIS_SECURE_ADV
    { 0x07, ali_ext_07_rsp_data }
#endif
  };

/**@brief Notify error to higher layer. */
static void notify_error(ali_ext_t *p_ext, uint32_t src, uint32_t err_code)
{
    /* send event to higher layer. */
    ext_evt.data.error.source   = src;
    ext_evt.data.error.err_code = err_code;
    os_post_event(OS_EV_EXT, OS_EV_CODE_EXT_ERROR, (unsigned long)&ext_evt);
}

/**@brief Function for setting TLV type 0x01 response data. */
static ret_code_t ali_ext_01_rsp_data(ali_ext_t *p_ext, uint8_t *p_buff,
                                      uint8_t *p_blen, const uint8_t *p_data,
                                      uint8_t dlen)
{
    ret_code_t err_code;
    uint8_t    len;

    if (dlen > 0) {
        err_code = BREEZE_ERROR_DATA_SIZE;
    } else if ((len = p_ext->tlv_01_rsp_len) <= *p_blen) {
        memcpy(p_buff, p_ext->tlv_01_rsp, len);
        *p_blen = len;

        err_code = BREEZE_SUCCESS;
    } else {
        /* No buffer space! */
        err_code = BREEZE_ERROR_NO_MEM;
    }

    return err_code;
}


/**@brief Function for setting TLV type 0x02 response data. */
static ret_code_t ali_ext_02_rsp_data(ali_ext_t *p_ext, uint8_t *p_buff,
                                      uint8_t *p_blen, const uint8_t *p_data,
                                      uint8_t dlen)
{
    ret_code_t err_code;
    uint8_t    len;

    if (dlen > 0) {
        err_code = BREEZE_ERROR_DATA_SIZE;
    } else if (p_ext->product_key_len == 0) {
        err_code = BREEZE_ERROR_NOT_SUPPORTED;
    } else if ((len = p_ext->product_key_len) <= *p_blen) {
        memcpy(p_buff, p_ext->p_product_key, len);
        *p_blen = len;

        err_code = BREEZE_SUCCESS;
    } else {
        /* No buffer space! */
        err_code = BREEZE_ERROR_NO_MEM;
    }

    return err_code;
}


/**@brief Function for setting TLV type 0x03 response data. */
static ret_code_t ali_ext_03_rsp_data(ali_ext_t *p_ext, uint8_t *p_buff,
                                      uint8_t *p_blen, const uint8_t *p_data,
                                      uint8_t dlen)
{
    ret_code_t err_code;
    uint8_t    len;

    if (dlen > 0) {
        err_code = BREEZE_ERROR_DATA_SIZE;
    } else if (p_ext->device_name_len == 0) {
        err_code = BREEZE_ERROR_NOT_SUPPORTED;
    } else if ((len = p_ext->device_name_len) <= *p_blen) {
        memcpy(p_buff, p_ext->p_device_name, len);
        *p_blen = len;

        err_code = BREEZE_SUCCESS;
    } else {
        /* No buffer space! */
        err_code = BREEZE_ERROR_NO_MEM;
    }

    return err_code;
}


/**@brief Function for setting TLV type 0x04 response data. */
static ret_code_t ali_ext_04_rsp_data(ali_ext_t *p_ext, uint8_t *p_buff,
                                      uint8_t *p_blen, const uint8_t *p_data,
                                      uint8_t dlen)
{
    ret_code_t err_code;

    if (dlen > 0) {
        err_code = BREEZE_ERROR_DATA_SIZE;
    } else if (p_ext->secret_len == 0) {
        err_code = BREEZE_ERROR_NOT_SUPPORTED;
    } else if (RANDOM_LEN <= *p_blen) {
        uint8_t  bytes_available = 0;
        uint8_t  i;
        uint32_t seed = os_now_ms();
        uint8_t  byte[4 + 1];
        uint32_t result;
        uint16_t bytes_copy;

        srand((unsigned int)seed);
        result = rand();

        while (bytes_available < RANDOM_LEN) {
            seed += result;
            seed = seed % 9999;
            snprintf((char *)byte, sizeof(byte), "%04d", seed);
            bytes_copy = ALI_AUTH_PRS_LEN - bytes_available;
            bytes_copy = (bytes_copy > 4) ? 4 : bytes_copy;
            memcpy(p_ext->p_random + bytes_available, byte, bytes_copy);
            bytes_available += bytes_copy;
        }

        // memcpy(p_ext->p_random, random, RANDOM_LEN);
        p_ext->random_len = RANDOM_LEN;


        for (i = 0; i < RANDOM_LEN; i++) {
            p_buff[i] = p_ext->p_random[i];
        }

        *p_blen  = RANDOM_LEN;
        err_code = BREEZE_SUCCESS;
    } else {
        /* No buffer space! */
        err_code = BREEZE_ERROR_NO_MEM;
    }

    return err_code;
}


static uint32_t swp_bytes(uint32_t value)
{
    return (value & 0x000000FFU) << 24 | (value & 0x0000FF00U) << 8 |
           (value & 0x00FF0000U) >> 8 | (value & 0xFF000000U) >> 24;
}

/**@brief Initialize V2 network signature (see specification v1.0.5, ch. 5.6.1).
 */
static void v2_network_signature_calculate(ali_ext_t *p_ext, uint8_t *p_buff)
{
    uint8_t    str_id[8], n;
    uint8_t    random_str[32];
    uint32_t   model_id;
    SHA256_CTX context;
    uint8_t    cli_id[4];

    model_id = swp_bytes(p_ext->model_id);
    memcpy(cli_id, (uint8_t *)&model_id, 4);
    hex2string(cli_id, sizeof(cli_id), str_id);
    sha256_init(&context);

    sha256_update(&context, m_v2sig_p1, sizeof(m_v2sig_p1)); /* "clientId" */
    sha256_update(&context, str_id,
                  sizeof(str_id)); /* model_id from from init parameters. */

    sha256_update(&context, m_v2sig_p2, sizeof(m_v2sig_p2)); /* "deviceName" */
    sha256_update(
      &context, p_ext->p_device_name,
      p_ext->device_name_len); /* Device key from init parameters. */

    sha256_update(&context, m_v2sig_p3,
                  sizeof(m_v2sig_p3)); /* "deviceSecret" */
    sha256_update(&context, p_ext->p_secret,
                  p_ext->secret_len); /* Secret from init parameters. */

    sha256_update(&context, m_v2sig_p4, sizeof(m_v2sig_p4)); /* "productKey" */
    sha256_update(
      &context, p_ext->p_product_key,
      p_ext->product_key_len); /* Product key from init parameters. */
    sha256_final(&context, p_buff);
}

/**@brief Function for setting TLV type 0x05 response data. */
static ret_code_t ali_ext_05_rsp_data(ali_ext_t *p_ext, uint8_t *p_buff,
                                      uint8_t *p_blen, const uint8_t *p_data,
                                      uint8_t dlen)
{
    ret_code_t err_code;
    p_ext->random_len = RANDOM_LEN;

    if (dlen > 0) {
        err_code = BREEZE_ERROR_DATA_SIZE;
    } else if (*p_blen >= SHA256_DATA_LEN) {
        v2_network_signature_calculate(p_ext, p_buff);
        *p_blen = SHA256_DATA_LEN;
        err_code = BREEZE_SUCCESS;
    } else {
        /* No buffer space! */
        err_code = BREEZE_ERROR_NO_MEM;
    }

    return err_code;
}

enum
{
    BLE_AWSS_CTYPE_SSID     = 0x01,
    BLE_AWSS_CTYPE_PASSWORD = 0x02,
    BLE_AWSS_CTYPE_BSSID    = 0x03,
};

static void utf8_to_str(uint8_t *data, uint8_t len, char *result)
{
    /* TODO: simply treat all as ascii for now, rework later */
    memcpy(result, data, len);
}

static void utf8_to_ssid(uint8_t *data, uint8_t len, char *ssid)
{
    utf8_to_str(data, len, ssid);
}

static void utf8_to_pw(uint8_t *data, uint8_t len, char *pw)
{
    utf8_to_str(data, len, pw);
}

/**@brief Notify received data to higher layer. */
static void notify_apinfo(ali_ext_t *p_ext, breeze_apinfo_t *ap)
{
    /* send event to higher layer. */
    ext_evt.data.rx_data.p_data = ap;
    ext_evt.data.rx_data.length = sizeof(breeze_apinfo_t);
    os_post_event(OS_EV_EXT, OS_EV_CODE_EXT_APIINFO, (unsigned long)&ext_evt);
}

#define SSID_READY     0x01
#define PASSWORD_READY 0x02
#define BSSID_READY    0x04
#define ALL_READY      (SSID_READY | PASSWORD_READY | BSSID_READY)

#define UTF8_MAX_SSID     32
#define UTF8_MAX_PASSWORD 64

breeze_apinfo_t comboinfo;
/**@brief Function for setting TLV type 0x06 response data. */
static ret_code_t ali_ext_06_rsp_data(ali_ext_t *p_ext, uint8_t *p_buff,
                                      uint8_t *p_blen, const uint8_t *p_data,
                                      uint8_t dlen)
{
    ret_code_t     err_code   = BREEZE_SUCCESS;
    uint8_t        idx        = 0, tlvtype, tlvlen;
    static uint8_t ready_flag = 0;
    /* tlv format, rsp[0]: CType, rsp[1]: CLen, rsp[2]: error code*/
    uint8_t rsp[] = { 0x01, 0x01, 0x01 };

    if (dlen < 2) {
        err_code = BREEZE_ERROR_DATA_SIZE;
        goto end;
    }

    if (*p_blen < sizeof(rsp)) {
        err_code = BREEZE_ERROR_NO_MEM;
        goto end;
    }

    while (idx < dlen) {
        tlvtype = p_data[idx++];
        tlvlen  = p_data[idx++];

        switch (tlvtype) {
            case BLE_AWSS_CTYPE_SSID: /* utf8 */
                if (tlvlen < 1 || tlvlen > UTF8_MAX_SSID) {
                    err_code = BREEZE_ERROR_INVALID_TLVDATA;
                    goto end;
                }
                utf8_to_pw(&p_data[idx], tlvlen, comboinfo.ssid);
                ready_flag |= SSID_READY;
                break;
            case BLE_AWSS_CTYPE_PASSWORD: /* utf8 */
                if (tlvlen > UTF8_MAX_PASSWORD) {
                    err_code = BREEZE_ERROR_INVALID_TLVDATA;
                    goto end;
                }
                utf8_to_pw(&p_data[idx], tlvlen, comboinfo.pw);
                ready_flag |= PASSWORD_READY;
                break;
            case BLE_AWSS_CTYPE_BSSID: /* 6-byte hex */
                if (tlvlen != 6) {
                    err_code = BREEZE_ERROR_INVALID_TLVDATA;
                    goto end;
                }
                memcpy(comboinfo.bssid, &p_data[idx], tlvlen);
                ready_flag |= BSSID_READY;
                break;
            default:
                err_code = BREEZE_ERROR_INVALID_TLVDATA;
                goto end;
                break;
        }

        idx += tlvlen;
    }

end:
    if (err_code != BREEZE_SUCCESS) {
        rsp[2] = 2; /* set failure code */
    }

    /* rsp */
    memcpy(p_buff, rsp, sizeof(rsp));
    *p_blen = sizeof(rsp);

    if (ready_flag & ALL_READY) {
        notify_apinfo(p_ext, &comboinfo);
    }

    return err_code;
}

#ifdef CONFIG_AIS_SECURE_ADV
static ret_code_t ali_ext_07_rsp_data(ali_ext_t *p_ext, uint8_t *p_buff,
                                      uint8_t *p_blen, const uint8_t *p_data,
                                      uint8_t dlen)
{
    ret_code_t err_code = BREEZE_SUCCESS;
    /* tlv format, rsp[0]: CType, rsp[1]: CLen, rsp[2]: error code*/
    uint8_t  ret_code = 1, i = 0;
    uint32_t seq = 0;

    if (dlen != sizeof(seq)) {
        printf("Error: invalid sequence data size\r\n");
        err_code = BREEZE_ERROR_DATA_SIZE;
        goto end;
    }

    if (*p_blen < 1) {
        printf("Error: invalid sequence rsp buffer size\r\n");
        err_code = BREEZE_ERROR_NO_MEM;
        goto end;
    }

    printf("The sequence bytes from cloud: %02x %02x %02x %02x\r\n", p_data[0],
           p_data[1], p_data[2], p_data[3]);

    while (i < sizeof(seq)) {
        seq |= p_data[i] << (i << 3);
        i++;
    }

    printf("The sequence hex to be saved is %08x\r\n", seq);
    set_adv_sequence(seq);

end:
    if (err_code != BREEZE_SUCCESS) {
        ret_code = 0; /* set failure code */
    }

    /* rsp */
    p_buff[0] = ret_code;
    *p_blen   = 1;

    return err_code;
}
#endif

ret_code_t ali_ext_init(ali_ext_t *p_ext, ali_ext_init_t const *p_init)
{
    int         ret;
    uint8_t     chip_code[4]   = { 0 };
    uint8_t     chip_id_str[8] = { 0 };
    const char *aostype        = "AOS";
    uint8_t     suffix_len     = 0;
    char        t_os_info[20]  = { 0 };

#ifdef BUILD_AOS
    strcpy(t_os_info, aos_version_get());
    char *m_os_type = strtok(t_os_info, "-");
    if (strcmp(aostype, m_os_type) == 0) {
        m_os_type = strtok(NULL, "-");
        m_os_type = strtok(NULL, "-");
        strcat(m_os_type, ":");
        printf("AOS version %s(%d)\n", m_os_type, strlen(m_os_type));

        suffix_len = strlen(m_os_type);
        memcpy(m_tlv_01_rsp_suffix, m_os_type, suffix_len);
        chip_code_st *p_chip_code_obj = get_chip_code(MCU_FAMILY);
        if (p_chip_code_obj != NULL) {
            chip_code[0] = (uint8_t)(p_chip_code_obj->vendor >> 8);
            chip_code[1] = (uint8_t)p_chip_code_obj->vendor;
            chip_code[2] = (uint8_t)(p_chip_code_obj->id >> 8);
            chip_code[3] = (uint8_t)p_chip_code_obj->id;
        }

        hex2string(chip_code, sizeof(chip_code), chip_id_str);
        memcpy(m_tlv_01_rsp_suffix + suffix_len, chip_id_str,
               sizeof(chip_id_str));
        suffix_len += sizeof(chip_id_str);
        memcpy(m_tlv_01_rsp_suffix + suffix_len, m_sdk_version,
               sizeof(m_sdk_version) - 1);
        suffix_len += sizeof(m_sdk_version) - 1;
        m_tlv_01_rsp_suffix[suffix_len] = '\0';
        strcat(m_tlv_01_rsp_suffix, ":1");
        suffix_len = strlen(m_tlv_01_rsp_suffix);
    }
#else
    memcpy(m_tlv_01_rsp_suffix, "NON-AOS", strlen("NON-AOS"));
    m_tlv_01_rsp_suffix[suffix_len] = '\0';
    suffix_len = strlen("NON-AOS");
#endif

    /* check parameters */
    VERIFY_PARAM_NOT_NULL(p_ext);
    VERIFY_PARAM_NOT_NULL(p_init);
    VERIFY_PARAM_NOT_NULL(p_init->tx_func);
    VERIFY_PARAM_NOT_NULL(p_init->p_fw_version);
    if (p_init->product_key_len > 0) {
        VERIFY_PARAM_NOT_NULL(p_init->p_product_key);
    }
    if (p_init->device_name_len > 0) {
        VERIFY_PARAM_NOT_NULL(p_init->p_device_name);
    }
    if (p_init->v2_signature_len > 0) {
        VERIFY_PARAM_NOT_NULL(p_init->p_v2_signature);
    }

    /* Initialize context */
    memset(p_ext, 0, sizeof(ali_ext_t));
    p_ext->p_evt_context     = p_init->p_evt_context;
    p_ext->tx_func           = p_init->tx_func;
    p_ext->p_tx_func_context = p_init->p_tx_func_context;
    p_ext->is_authenticated  = false;
    p_ext->tlv_01_rsp_len    = suffix_len;
    memcpy(p_ext->tlv_01_rsp, m_tlv_01_rsp_suffix, suffix_len);
    p_ext->model_id         = p_init->model_id;
    p_ext->p_device_name    = p_init->p_device_name;
    p_ext->device_name_len  = p_init->device_name_len;
    p_ext->p_product_key    = p_init->p_product_key;
    p_ext->product_key_len  = p_init->product_key_len;
    p_ext->p_secret         = p_init->p_secret;
    p_ext->secret_len       = p_init->secret_len;
    p_ext->random_len       = 0;
    p_ext->p_v2_signature   = p_init->p_v2_signature;
    p_ext->v2_signature_len = p_init->v2_signature_len;

    return BREEZE_SUCCESS;
}


void ali_ext_reset(ali_ext_t *p_ext)
{
    /* check parameters */
    VERIFY_PARAM_NOT_NULL_VOID(p_ext);

    /* Reset state machine. */
    p_ext->is_authenticated = false;
}


void ali_ext_on_command(ali_ext_t *p_ext, uint8_t cmd, uint8_t *p_data,
                        uint16_t length)
{
    /* check parameters */
    VERIFY_PARAM_NOT_NULL_VOID(p_ext);
    VERIFY_PARAM_NOT_NULL_VOID(p_data);
    if (length == 0 || cmd != ALI_CMD_EXT_DOWN) {
        return;
    }


    uint8_t *p_tx_buff     = p_ext->tx_buff;
    uint8_t  tx_buff_avail = sizeof(p_ext->tx_buff);
    uint8_t  tx_buff_size;
    uint8_t  tlv_mask, tlv_masked = 0;
    uint8_t  tlv_type, tlv_len;

    uint32_t err_code = BREEZE_SUCCESS;

    /* check that authentication has been passed. */
    if (!p_ext->is_authenticated) {
        err_code = BREEZE_ERROR_INVALID_STATE;
    }

    /* process TLV one at a time (see specification v1.0.5, ch. 5.6) */
    while (length > 0 && err_code == BREEZE_SUCCESS) {
        if (length >= 2) {
            /* get TLV type. */
            tlv_type = *p_data++;
            /* get TLV length. */
            tlv_len = *p_data++;

            length -= 2;
        } else {
            err_code = BREEZE_ERROR_INVALID_LENGTH;
            break;
        }

        /* each TLV type should not get repeated. */
        tlv_mask = (1 << tlv_type);
        if ((tlv_mask & tlv_masked) != 0) {
            err_code = BREEZE_ERROR_INVALID_DATA;
            break;
        }
        tlv_masked |= tlv_mask;

        /* check that TLV length does not exceed input data boundary. */
        if (tlv_len > length) {
            err_code = BREEZE_ERROR_DATA_SIZE;
            break;
        }

        if (tx_buff_avail < 2) {
            err_code = BREEZE_ERROR_NO_MEM;
            break;
        }

        /* find the TLV type handler in table. */
        uint32_t n, n_max = sizeof(m_tlv_type_handler_table) /
                            sizeof(ali_ext_tlv_type_handler_t);
        for (n = 0; n < n_max; n++) {
            if (m_tlv_type_handler_table[n].tlv_type == tlv_type) {
                /* call the handler to process the TLV type. */
                tx_buff_size = tx_buff_avail - 2;
                err_code     = m_tlv_type_handler_table[n].handler(
                  p_ext, p_tx_buff + 2, &tx_buff_size, p_data, tlv_len);

                break;
            }
        }
        if (n >= n_max) {
            /* TLV type is not found in table. */
            err_code = BREEZE_ERROR_INVALID_DATA;
        }

        if (err_code == BREEZE_SUCCESS) {
            *(p_tx_buff + 0) = tlv_type;
            *(p_tx_buff + 1) = tx_buff_size;

            p_tx_buff += (2 + tx_buff_size);
            tx_buff_avail -= (2 + tx_buff_size);

            p_data += tlv_len;
            length -= tlv_len;
        }
    }

    if (err_code == BREEZE_SUCCESS) {
        err_code = p_ext->tx_func(p_ext->p_tx_func_context, ALI_CMD_EXT_UP,
                                  p_ext->tx_buff,
                                  sizeof(p_ext->tx_buff) - tx_buff_avail);

    } else {
        err_code =
          p_ext->tx_func(p_ext->p_tx_func_context, ALI_CMD_ERROR, NULL, 0);
    }

    if (err_code != BREEZE_SUCCESS) {
        notify_error(p_ext, ALI_ERROR_SRC_EXT_SEND_RSP, err_code);
    }
}


void ali_ext_on_auth(ali_ext_t *p_ext, bool is_authenticated)
{
    /* check parameters */
    VERIFY_PARAM_NOT_NULL_VOID(p_ext);

    /* set authentication flag. */
    p_ext->is_authenticated = is_authenticated;
}
