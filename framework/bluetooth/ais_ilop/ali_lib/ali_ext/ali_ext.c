/*
 * Copyright (c) 2015 - 2017, Nordic Semiconductor ASA
 * 
 * All rights reserved.
 * 
 * Use in source and binary forms, redistribution in binary form only, with
 * or without modification, are permitted provided that the following conditions
 * are met:
 * 
 * 1. Redistributions in binary form, except as embedded into a Nordic
 *    Semiconductor ASA integrated circuit in a product or a software update for
 *    such product, must reproduce the above copyright notice, this list of
 *    conditions and the following disclaimer in the documentation and/or other
 *    materials provided with the distribution.
 * 
 * 2. Neither the name of Nordic Semiconductor ASA nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 * 
 * 3. This software, with or without modification, must only be used with a Nordic
 *    Semiconductor ASA integrated circuit.
 * 
 * 4. Any software provided in binary form under this license must not be reverse
 *    engineered, decompiled, modified and/or disassembled.
 * 
 * THIS SOFTWARE IS PROVIDED BY NORDIC SEMICONDUCTOR ASA "AS IS" AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, NONINFRINGEMENT, AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL NORDIC SEMICONDUCTOR ASA OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR
 * TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#include "ali_ext.h"
#include "ali_auth.h"
#include "ali_error_src.h"
#include <stdio.h>                  /* Standard input/output definitions */
#include <string.h>                 /* String function definitions */
#include "ali_core.h"
#include "sha256.h"
#include <aos/aos.h>

#ifdef NRF51
#define NRF5_IC_STRING          "nRF51822"
#else
#ifdef NRF52
#define NRF5_IC_STRING          "nRF52832"
#else
#define NRF5_IC_STRING          "ESP32"
#endif
#endif
#define RANDOM_LEN              16

#define NRF_LOG_MODULE_NAME "ALI"

static uint8_t const m_v2sig_p1[8]  = "clientId";       /**< V2 network signature fixed part 1. */
static uint8_t const m_v2sig_p2[10] = "deviceName";     /**< V2 network signature fixed part 2. */
static uint8_t const m_v2sig_p3[12] = "deviceSecret";   /**< V2 network signature fixed part 3. */
static uint8_t const m_v2sig_p4[10] = "productKey";     /**< V2 network signature fixed part 4. */
static uint8_t const m_v2sig_p5[6] = "random";          /**< V2 network signature fixed part 5. */

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
 * @retval    NRF_SUCCESS             If TLV handling was successful.
 * @retval    NRF_ERROR_NOT_SUPPORTED If TLV type was not supported.
 * @retval    NRF_ERROR_DATA_SIZE     If invalid data size have been provided.
 * @retval    NRF_ERROR_NO_MEM        If Tx data buffer was inadequate for placing the response.
 */
typedef ret_code_t (*ali_ext_tlv_handler_t)(ali_ext_t * p_ext, uint8_t * p_buff, uint8_t * p_blen, const uint8_t * p_data, uint8_t dlen);

/**@brief Structure for TLV type handler. */
typedef struct
{
    uint8_t                     tlv_type;               /**< TLV type. */
    ali_ext_tlv_handler_t       handler;                /**< Pointer to handler function. */
} ali_ext_tlv_type_handler_t;


static ret_code_t ali_ext_01_rsp_data(ali_ext_t * p_ext, uint8_t * p_buff, uint8_t * p_blen, const uint8_t * p_data, uint8_t dlen);
static ret_code_t ali_ext_02_rsp_data(ali_ext_t * p_ext, uint8_t * p_buff, uint8_t * p_blen, const uint8_t * p_data, uint8_t dlen);
static ret_code_t ali_ext_03_rsp_data(ali_ext_t * p_ext, uint8_t * p_buff, uint8_t * p_blen, const uint8_t * p_data, uint8_t dlen);
static ret_code_t ali_ext_04_rsp_data(ali_ext_t * p_ext, uint8_t * p_buff, uint8_t * p_blen, const uint8_t * p_data, uint8_t dlen);
static ret_code_t ali_ext_05_rsp_data(ali_ext_t * p_ext, uint8_t * p_buff, uint8_t * p_blen, const uint8_t * p_data, uint8_t dlen);

/**@brief Convert a hex digit to ASCII character. */
static uint8_t hex2ascii(uint8_t digit)
{
    uint8_t val;

    if (digit <= 9)
    {
        val = digit - 0x0 + '0';
    }
    else
    {
        val = digit - 0xA + 'A';
    }

    return val;
}

static void hex2string(uint8_t *hex, uint32_t len, uint8_t *str)
{
    uint32_t i;

    for ( i = 0; i < len; i++ )
    {
        str[i*2] = hex2ascii(hex[i] >> 4 & 0x0f);
        str[i*2 + 1] = hex2ascii(hex[i] & 0x0f);
    }
}

static const ali_ext_tlv_type_handler_t m_tlv_type_handler_table[] =    /**< TLV type handler table. */
{
    {0x01, ali_ext_01_rsp_data},
    {0x02, ali_ext_02_rsp_data},
    {0x03, ali_ext_03_rsp_data},
    {0x04, ali_ext_04_rsp_data},
    {0x05, ali_ext_05_rsp_data}
};

static const char m_tlv_01_rsp_suffix[] = ":" NRF5_IC_STRING ":" "SDK" ALI_SDK_VERSION;   /**< TLV type 01 response suffix. */


/**@brief Notify error to higher layer. */
static void notify_error (ali_ext_t * p_ext, uint32_t src, uint32_t err_code)
{
    ali_ext_event_t evt;

    /* send event to higher layer. */
    evt.type                = ALI_EXT_EVT_ERROR;
    evt.data.error.source   = src;
    evt.data.error.err_code = err_code;
    p_ext->event_handler(p_ext->p_evt_context, &evt);
}


/**@brief Function for setting TLV type 0x01 response data. */
static ret_code_t ali_ext_01_rsp_data(ali_ext_t * p_ext, uint8_t * p_buff, uint8_t * p_blen, const uint8_t * p_data, uint8_t dlen)
{
    ret_code_t err_code;
    uint8_t len;

    if (dlen > 0)
    {
        err_code = NRF_ERROR_DATA_SIZE;
    }
    else if ((len = p_ext->tlv_01_rsp_len) <= *p_blen)
    {
        memcpy(p_buff, p_ext->tlv_01_rsp, len);
        *p_blen = len;

        err_code = NRF_SUCCESS;
    }
    else
    {
        /* No buffer space! */
        err_code = NRF_ERROR_NO_MEM;
    }

    return err_code;
}


/**@brief Function for setting TLV type 0x02 response data. */
static ret_code_t ali_ext_02_rsp_data(ali_ext_t * p_ext, uint8_t * p_buff, uint8_t * p_blen, const uint8_t * p_data, uint8_t dlen)
{
    ret_code_t err_code;
    uint8_t len;

    if (dlen > 0)
    {
        err_code = NRF_ERROR_DATA_SIZE;
    }
    else if (p_ext->product_key_len == 0)
    {
        err_code = NRF_ERROR_NOT_SUPPORTED;
    }
    else if ((len = p_ext->product_key_len) <= *p_blen)
    {
        memcpy(p_buff, p_ext->p_product_key, len);
        *p_blen = len;

        err_code = NRF_SUCCESS;
    }
    else
    {
        /* No buffer space! */
        err_code = NRF_ERROR_NO_MEM;
    }

    return err_code;
}


/**@brief Function for setting TLV type 0x03 response data. */
static ret_code_t ali_ext_03_rsp_data(ali_ext_t * p_ext, uint8_t * p_buff, uint8_t * p_blen, const uint8_t * p_data, uint8_t dlen)
{
    ret_code_t err_code;
    uint8_t len;

    if (dlen > 0)
    {
        err_code = NRF_ERROR_DATA_SIZE;
    }
    else if (p_ext->device_name_len == 0)
    {
        err_code = NRF_ERROR_NOT_SUPPORTED;
    }
    else if ((len = p_ext->device_name_len) <= *p_blen)
    {
        memcpy(p_buff, p_ext->p_device_name, len);
        *p_blen = len;

        err_code = NRF_SUCCESS;
    }
    else
    {
        /* No buffer space! */
        err_code = NRF_ERROR_NO_MEM;
    }

    return err_code;
}


/**@brief Function for setting TLV type 0x04 response data. */
static ret_code_t ali_ext_04_rsp_data(ali_ext_t * p_ext, uint8_t * p_buff, uint8_t * p_blen, const uint8_t * p_data, uint8_t dlen)
{
    ret_code_t err_code;

    if (dlen > 0)
    {
        err_code = NRF_ERROR_DATA_SIZE;
    }
    else if (p_ext->secret_len == 0)
    {
        err_code = NRF_ERROR_NOT_SUPPORTED;
    }
    else if (RANDOM_LEN <= *p_blen)
    {
        uint8_t bytes_available = 0;
        uint8_t i;
        uint32_t seed = aos_now_ms();
        uint8_t byte[4 + 1];
        uint32_t result = result;
        uint16_t bytes_copy;

        while (bytes_available < RANDOM_LEN) {
            seed += result;
            seed = seed % 9999;
            snprintf((char *)byte, sizeof(byte), "%04d", seed);
            bytes_copy = ALI_AUTH_PRS_LEN - bytes_available;
            bytes_copy = (bytes_copy > 4)? 4: bytes_copy;
            memcpy(p_ext->p_random + bytes_available, byte, bytes_copy);
            bytes_available += bytes_copy;
        }

        //memcpy(p_ext->p_random, random, RANDOM_LEN);
        p_ext->random_len = RANDOM_LEN;

        for (i = 0; i < RANDOM_LEN; i++)
            p_buff[i] = p_ext->p_random[i]^p_ext->p_secret[i];

        *p_blen = RANDOM_LEN;
        err_code = NRF_SUCCESS;
    }
    else
    {
        /* No buffer space! */
        err_code = NRF_ERROR_NO_MEM;
    }

    return err_code;
}

/**@brief Initialize V2 network signature (see specification v1.0.5, ch. 5.6.1). */
static void v2_network_signature_calculate (ali_ext_t * p_ext, uint8_t * p_buff)
{
    uint8_t str_id[8], n;
    uint8_t random_str[32];
    uint32_t model_id;
    SHA256_CTX context;

    sha256_init(&context);
    
    sha256_update(&context, m_v2sig_p1, sizeof(m_v2sig_p1));        /* "clientId" */

    /* Convert 32-bit Model ID to ASCII string. */
    model_id = p_ext->model_id;
    for (n = 0; n < 8; n++)
    {
        str_id[n] = hex2ascii((model_id >> 28));
        model_id <<= 4;
    }
    sha256_update(&context, str_id, sizeof(str_id));                /* model_id from from init parameters. */

    sha256_update(&context, m_v2sig_p2, sizeof(m_v2sig_p2));        /* "deviceName" */
    sha256_update(&context, p_ext->p_device_name, p_ext->device_name_len);      /* Device key from init parameters. */

    sha256_update(&context, m_v2sig_p3, sizeof(m_v2sig_p3));        /* "deviceSecret" */
    sha256_update(&context, p_ext->p_secret, p_ext->secret_len);    /* Secret from init parameters. */

    sha256_update(&context, m_v2sig_p4, sizeof(m_v2sig_p4));                /* "productKey" */
    sha256_update(&context, p_ext->p_product_key, p_ext->product_key_len);  /* Product key from init parameters. */

    sha256_update(&context, m_v2sig_p5, sizeof(m_v2sig_p5));        /* "random" */
    hex2string(p_ext->p_random, p_ext->random_len, random_str);
    sha256_update(&context, random_str, sizeof(random_str));        /* random from init parameters. */
    sha256_final(&context, p_buff);
}

/**@brief Function for setting TLV type 0x05 response data. */
static ret_code_t ali_ext_05_rsp_data(ali_ext_t * p_ext, uint8_t * p_buff, uint8_t * p_blen, const uint8_t * p_data, uint8_t dlen)
{
    ret_code_t err_code;

    if (dlen > 0)
    {
        err_code = NRF_ERROR_DATA_SIZE;
    }
    else if (p_ext->random_len == RANDOM_LEN)
    {
        v2_network_signature_calculate(p_ext, p_buff);
        *p_blen = 32;

        //avod random have not been updated, before calculating signature
        p_ext->random_len = 0;

        err_code = NRF_SUCCESS;
    }
    else
    {
        /* No buffer space! */
        err_code = NRF_ERROR_NO_MEM;
    }

    return err_code;
}


ret_code_t ali_ext_init(ali_ext_t * p_ext, ali_ext_init_t const * p_init)
{
    /* length of TLV type 01 response suffix, excluding the terminating '\0' */
    uint8_t suffix_len = sizeof(m_tlv_01_rsp_suffix) - 1;

    /* check parameters */
    VERIFY_PARAM_NOT_NULL(p_ext);
    VERIFY_PARAM_NOT_NULL(p_init);
    VERIFY_PARAM_NOT_NULL(p_init->event_handler);
    VERIFY_PARAM_NOT_NULL(p_init->tx_func);
    VERIFY_PARAM_NOT_NULL(p_init->p_fw_version);
    if (p_init->fw_version_len == 0 || p_init->fw_version_len > ALI_EXT_MAX_TLV_01_RSP_LEN - suffix_len)
    {
        return NRF_ERROR_INVALID_LENGTH;
    }
    if (p_init->product_key_len > 0)
    {
        VERIFY_PARAM_NOT_NULL(p_init->p_product_key);
    }
    if (p_init->device_name_len > 0)
    {
        VERIFY_PARAM_NOT_NULL(p_init->p_device_name);
    }
    if (p_init->v2_signature_len > 0)
    {
        VERIFY_PARAM_NOT_NULL(p_init->p_v2_signature);
    }

    /* Initialize context */
    memset(p_ext, 0, sizeof(ali_ext_t));
    p_ext->event_handler     = p_init->event_handler;
    p_ext->p_evt_context     = p_init->p_evt_context;
    p_ext->tx_func           = p_init->tx_func;
    p_ext->p_tx_func_context = p_init->p_tx_func_context;
    p_ext->is_authenticated = false;
    p_ext->tlv_01_rsp_len = p_init->fw_version_len + suffix_len;
    memcpy(p_ext->tlv_01_rsp,                          p_init->p_fw_version, p_init->fw_version_len);
    memcpy(p_ext->tlv_01_rsp + p_init->fw_version_len, m_tlv_01_rsp_suffix,  suffix_len);
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

    return NRF_SUCCESS;
}


void ali_ext_reset(ali_ext_t * p_ext)
{
    /* check parameters */
    VERIFY_PARAM_NOT_NULL_VOID(p_ext);

    /* Reset state machine. */
    p_ext->is_authenticated = false;
}


void ali_ext_on_command(ali_ext_t * p_ext, uint8_t cmd, uint8_t * p_data, uint16_t length)
{
    /* check parameters */
    VERIFY_PARAM_NOT_NULL_VOID(p_ext);
    VERIFY_PARAM_NOT_NULL_VOID(p_data);
    if (length == 0 || cmd != ALI_CMD_EXT_DOWN)
    {
        return;
    }

    uint8_t * p_tx_buff = p_ext->tx_buff;
    uint8_t tx_buff_avail = sizeof(p_ext->tx_buff);
    uint8_t tx_buff_size;
    uint8_t tlv_mask, tlv_masked = 0;
    uint8_t tlv_type, tlv_len;

    uint32_t err_code = NRF_SUCCESS;

    /* check that authentication has been passed. */
    if (!p_ext->is_authenticated)
    {
        err_code = NRF_ERROR_INVALID_STATE;
    }

    /* process TLV one at a time (see specification v1.0.5, ch. 5.6) */
    while (length > 0 && err_code == NRF_SUCCESS)
    {
        if (length >= 2)
        {
            /* get TLV type. */
            tlv_type = *p_data++;
            /* get TLV length. */
            tlv_len = *p_data++;

            length -= 2;
        }
        else
        {
            err_code = NRF_ERROR_INVALID_LENGTH;
            break;
        }

        /* each TLV type should not get repeated. */
        tlv_mask = (1 << tlv_type);
        if ((tlv_mask & tlv_masked) != 0)
        {
            err_code = NRF_ERROR_INVALID_DATA;
            break;
        }
        tlv_masked |= tlv_mask;

        /* check that TLV length does not exceed input data boundary. */
        if (tlv_len > length)
        {
            err_code = NRF_ERROR_DATA_SIZE;
            break;
        }

        if (tx_buff_avail < 2)
        {
            err_code = NRF_ERROR_NO_MEM;
            break;
        }

        /* find the TLV type handler in table. */
        uint32_t n, n_max = sizeof(m_tlv_type_handler_table) / sizeof(ali_ext_tlv_type_handler_t);
        for (n = 0; n < n_max; n++)
        {
            if (m_tlv_type_handler_table[n].tlv_type == tlv_type)
            {
                /* call the handler to process the TLV type. */
                tx_buff_size = tx_buff_avail - 2;
                err_code = m_tlv_type_handler_table[n].handler(p_ext, p_tx_buff + 2, &tx_buff_size, p_data, tlv_len);

                break;
            }
        }
        if (n >= n_max)
        {
            /* TLV type is not found in table. */
            err_code = NRF_ERROR_INVALID_DATA;
        }

        if (err_code == NRF_SUCCESS)
        {
            *(p_tx_buff + 0) = tlv_type;
            *(p_tx_buff + 1) = tx_buff_size;

            p_tx_buff += (2 + tx_buff_size);
            tx_buff_avail -= (2 + tx_buff_size);

            p_data += tlv_len;
            length -= tlv_len;
        }
    }

    if (err_code == NRF_SUCCESS)
    {
        err_code = p_ext->tx_func(p_ext->p_tx_func_context, ALI_CMD_EXT_UP, 
                                  p_ext->tx_buff, sizeof(p_ext->tx_buff) - tx_buff_avail);
    }
    else
    {
        err_code = p_ext->tx_func(p_ext->p_tx_func_context, ALI_CMD_ERROR, NULL, 0);
    }

    if (err_code != NRF_SUCCESS)
    {
        notify_error(p_ext, ALI_ERROR_SRC_EXT_SEND_RSP, err_code);
    }
}


void ali_ext_on_auth(ali_ext_t * p_ext, bool is_authenticated)
{
    /* check parameters */
    VERIFY_PARAM_NOT_NULL_VOID(p_ext);

    /* set authentication flag. */
    p_ext->is_authenticated = is_authenticated;
}

