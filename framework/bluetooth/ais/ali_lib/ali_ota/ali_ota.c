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
#include "ali_ota.h"
#include "ali_error_src.h"
#include <stdio.h>                  /* Standard input/output definitions */
#include <string.h>                 /* String function definitions */
#include <stdbool.h>
//#include "nordic_common.h"
//#include "sdk_macros.h"
//#include "nrf_soc.h"
//#include "crc16.h"
//#include "crc32.h"
//#include "nrf_dfu_types.h"
//#include "nrf_dfu_flash.h"
//#include "nrf_dfu_settings.h"
#include <hal/ais_ota.h>
#include <crc.h>
#include <aos/aos.h>

#define EXTRACT_U16(d)  (*((uint8_t *)(d)) | (*((uint8_t *)(d) + 1) << 8))
#define EXTRACT_U32(d)  (*((uint8_t *)(d)) | (*((uint8_t *)(d) + 1) << 8) | (*((uint8_t *)(d) + 2) << 16) | (*((uint8_t *)(d) + 3) << 24))

#define ENCODE_U16(d, val)                       \
{                                                \
    *((uint8_t *)(d)) = (val) & 0xFF;            \
    *((uint8_t *)(d) + 1) = ((val) >> 8) & 0xFF; \
}

#define ENCODE_U32(d, val)                        \
{                                                 \
    *((uint8_t *)(d)) = (val) & 0xFF;             \
    *((uint8_t *)(d) + 1) = ((val) >> 8) & 0xFF;  \
    *((uint8_t *)(d) + 2) = ((val) >> 16) & 0xFF; \
    *((uint8_t *)(d) + 3) = ((val) >> 24) & 0xFF; \
}


static void * m_ptr;    /**< Pointer to ali_ota, used only in bootloader_settings_event_handler() and flash_event_handler(). */


/**@brief Try to split software version string into 3 parts:
 *        v0.v2.v2.
 */
static uint32_t split_sw_ver (char * data, uint32_t * v0, uint32_t * v1, uint32_t * v2)
{
    int i = sscanf(data, "%d.%d.%d", (int *)v0, (int *)v1, (int *)v2);
    return ((i == 3)? NRF_SUCCESS: NRF_ERROR_INVALID_DATA);
}


/**@brief Try parsing firmware version string. */
static uint32_t parse_firmware_version (uint8_t * data, uint8_t len)
{
    uint32_t err_code;
    uint32_t v[3];
    uint8_t l_data[ALI_OTA_MAX_FW_VERSION_LEN + 1];   // +1 for trailing zero

    // Copy to stack variable as trailing zero is required.
    memcpy(l_data, data, len);
    l_data[len] = 0;

    err_code = split_sw_ver((char *)l_data, v, v+1, v+2);
    VERIFY_SUCCESS(err_code);

    // Try to reconstruct the version string.
    memset(l_data, 0, sizeof(l_data));
    sprintf((char *)l_data, "%d.%d.%d", (int)v[0], (int)v[1], (int)v[2]);

    if (memcmp(l_data, data, len) == 0)
    {
        return NRF_SUCCESS;
    }
    else
    {
        return NRF_ERROR_INVALID_DATA;
    }
}


/**@brief Align address to ceil(page_size), where page_size is 2 to the integer power. */
static uint32_t align_to_page(uint32_t val, uint32_t page_size)
{
    return ((val + page_size - 1 ) &~ (page_size - 1));
}


/**@brief Notify error to higher layer. */
static void notify_error (ali_ota_t * p_ota, uint32_t src, uint32_t err_code)
{
    ali_ota_event_t evt;

    /* send event to higher layer. */
    evt.type                = ALI_OTA_EVT_ERROR;
    evt.data.error.source   = src;
    evt.data.error.err_code = err_code;
    p_ota->event_handler(p_ota->p_evt_context, &evt);
}


/**@brief Notify new firmware event to higher layer. */
static void notify_new_fw (ali_ota_t * p_ota)
{
    ali_ota_event_t evt;

    /* send event to higher layer. */
    evt.type = ALI_OTA_EVT_NEW_FW;
    p_ota->event_handler(p_ota->p_evt_context, &evt);
}


/**@brief Check if the received firmware version is newer than the current one. */
static uint32_t check_upgrade_fw_version (ali_ota_t * p_ota, uint8_t * p_data, uint8_t length)
{
    uint32_t err_code;
    uint32_t v_old[3], v_new[3];
    uint8_t l_data_old[ALI_OTA_MAX_FW_VERSION_LEN + 1];   // +1 for trailing zero
    uint8_t l_data_new[ALI_OTA_MAX_FW_VERSION_LEN + 1];   // +1 for trailing zero
    uint8_t l_len;

    // Copy to stack variable as trailing zero is required.
    memcpy(l_data_old, p_ota->fw_ver, p_ota->fw_ver_len);
    l_data_old[p_ota->fw_ver_len] = 0;

    l_len = length - sizeof(uint32_t) - sizeof(uint16_t) - 1;
    memcpy(l_data_new, p_data, l_len);
    l_data_new[l_len] = 0;

    // Split SW version into 3 parts.
    err_code = split_sw_ver ((char *)l_data_old, v_old, v_old+1, v_old+2);
    VERIFY_SUCCESS(err_code);

    err_code = split_sw_ver ((char *)l_data_new, v_new, v_new+1, v_new+2);
    VERIFY_SUCCESS(err_code);
    
    // Try to reconstruct the version string.
    memset(l_data_new, 0, sizeof(l_data_new));
    sprintf((char *)l_data_new, "%d.%d.%d", (int)v_new[0], (int)v_new[1], (int)v_new[2]);
    if (memcmp(l_data_new, p_data, l_len) != 0)
    {
        return NRF_ERROR_INVALID_DATA;
    }

    printf("Old fw ver: %d.%d.%d, new ver: %d.%d.%d\r\n",
           v_old[0], v_old[1], v_old[2], v_new[0], v_new[1], v_new[2]);

    // Check digits in software version
    if (v_new[0] > v_old[0])        // x
    {
        return NRF_SUCCESS;
    }
    else if (v_new[0] < v_old[0])
    {
        return NRF_ERROR_FORBIDDEN;
    }

    if (v_new[1] > v_old[1])        // y
    {
        return NRF_SUCCESS;
    }
    else if (v_new[1] < v_old[1])
    {
        return NRF_ERROR_FORBIDDEN;
    }

    if (v_new[2] <= v_old[2])        // z
    {
        return NRF_ERROR_FORBIDDEN;
    }

    return NRF_SUCCESS;
}


/**@brief Send firmware version response. */
static uint32_t send_fw_version_rsp (ali_ota_t * p_ota)
{
    uint32_t err_code;

    err_code = p_ota->tx_func(p_ota->p_tx_func_context, ALI_CMD_FW_VERSION_RSP,
                              p_ota->fw_ver, p_ota->fw_ver_len);
    if (err_code != NRF_SUCCESS)
    {
        notify_error(p_ota, ALI_ERROR_SRC_OTA_SEND_VER_RSP, err_code);
    }
    return err_code;
}


/**@brief Send firmware upgrade response. */
static uint32_t send_fw_upgrade_rsp (ali_ota_t * p_ota, bool allow_upgrade)
{
    uint32_t err_code;

    p_ota->tx_buff[0] = (allow_upgrade)? 1: 0;
    err_code = p_ota->tx_func(p_ota->p_tx_func_context, ALI_CMD_FW_UPGRADE_RSP,
                              p_ota->tx_buff, 1);
    if (err_code != NRF_SUCCESS)
    {
        notify_error(p_ota, ALI_ERROR_SRC_OTA_SEND_UPGRADE_RSP, err_code);
    }
    return err_code;
}


/**@brief Send number of bytes of firmware received. */
static uint32_t send_bytes_received (ali_ota_t * p_ota)
{
    uint32_t err_code;

    ENCODE_U16(p_ota->tx_buff, p_ota->frames_recvd);
    ENCODE_U32(p_ota->tx_buff + sizeof(uint16_t), p_ota->bytes_recvd);

    err_code = p_ota->tx_func(p_ota->p_tx_func_context, ALI_CMD_FW_BYTES_RECEIVED,
                              p_ota->tx_buff, sizeof(uint16_t) + sizeof(uint32_t));
    if (err_code != NRF_SUCCESS)
    {
        notify_error(p_ota, ALI_ERROR_SRC_OTA_SEND_BYTES_RECVD, err_code);
    }
    return err_code;
}


/**@brief Send the result of CRC check. */
static uint32_t send_crc_result (ali_ota_t * p_ota, bool crc_ok)
{
    uint32_t err_code;

    p_ota->tx_buff[0] = (crc_ok)? 1: 0;
    err_code = p_ota->tx_func(p_ota->p_tx_func_context, ALI_CMD_FW_CHECK_RESULT,
                              p_ota->tx_buff, 1);
    if (err_code != NRF_SUCCESS)
    {
        notify_error(p_ota, ALI_ERROR_SRC_OTA_SEND_CRC_RESULT, err_code);
    }
    return err_code;
}


/**@brief Send "New firmware" (cmd=0x26). */
static uint32_t send_fwup_success (ali_ota_t * p_ota)
{
    uint32_t err_code;

    p_ota->tx_buff[0] = 1;
    err_code = p_ota->tx_func(p_ota->p_tx_func_context, ALI_CMD_FW_UPDATE_PROCESS,
                              p_ota->tx_buff, 1);
    if (err_code != NRF_SUCCESS)
    {
        notify_error(p_ota, ALI_ERROR_SRC_OTA_SEND_FWUP_SUCCESS, err_code);
    }
    return err_code;
}


/**@brief Send ERROR. */
static uint32_t send_error (ali_ota_t * p_ota)
{
    uint32_t err_code;

    err_code = p_ota->tx_func(p_ota->p_tx_func_context, ALI_CMD_ERROR, NULL, 0);
    if (err_code != NRF_SUCCESS)
    {
        notify_error(p_ota, ALI_ERROR_SRC_OTA_SEND_ERROR, err_code);
    }
    return err_code;
}


/**@brief Callback function for bootloader settings. */
static void bootloader_settings_event_handler(ali_ota_flash_evt_t t)
{
    ali_ota_t * p_ota = (ali_ota_t *)m_ptr;

    switch (p_ota->state)
    {
        case ALI_OTA_STATE_UPGRADE_REPORT:
            if (ALI_OTA_FLASH_STORE_OK == t)
            {
                p_ota->state = (p_ota->feature_enable)? ALI_OTA_STATE_IDLE: ALI_OTA_STATE_OFF;
                (void) send_fwup_success(p_ota);
            }
            else
            {
                notify_error(p_ota, ALI_ERROR_SRC_OTA_SETTINGS_WRITE, NRF_ERROR_FLASH_STORE_FAIL);
                return;
            }
            break;

        case ALI_OTA_STATE_WRITE_SETTINGS:
            if (ALI_OTA_FLASH_STORE_OK == t)
            {
                p_ota->state = (p_ota->bytes_recvd >= p_ota->rx_fw_size)? ALI_OTA_STATE_FW_CHECK: ALI_OTA_STATE_RECEIVE;
                (void) send_bytes_received(p_ota);
            }
            else
            {
                notify_error(p_ota, ALI_ERROR_SRC_OTA_FW_RX_WR_SETTINGS_CB, NRF_ERROR_FLASH_STORE_FAIL);
                return;
            }
            break;

        case ALI_OTA_STATE_RESET_PREPARE:
            (void) send_crc_result(p_ota, true);
            break;

        default:
            break;
    }
}


/**@brief Callback function for flash write/erase operations. */
static void flash_event_handler(ali_ota_flash_evt_t t)
{
    uint32_t err_code;
    ali_ota_t * p_ota = (ali_ota_t *)m_ptr;

    switch (p_ota->state)
    {
        case ALI_OTA_STATE_IDLE:
            if (ALI_OTA_FLASH_ERASE_OK == t)
            {
                err_code = send_fw_upgrade_rsp(p_ota, true);
                if (err_code == NRF_SUCCESS)
                {
                    p_ota->state = ALI_OTA_STATE_RECEIVE;
                }
            }
            else
            {
                notify_error(p_ota, ALI_ERROR_SRC_OTA_FLASH_ERASE, NRF_ERROR_FLASH_ERASE_FAIL);
                return;
            }
            break;

        case ALI_OTA_STATE_WRITE:
            if (ALI_OTA_FLASH_STORE_OK == t)
            {
                bool update_bldr_settings = false;

                if (p_ota->bytes_recvd == p_ota->rx_fw_size)
                {
                    ais_ota_set_setting_fw_offset(p_ota->bytes_recvd);
                    update_bldr_settings = true;
                }
                else if (p_ota->bytes_recvd >= ais_ota_get_setting_fw_offset() + ais_ota_get_page_size())
                {
                    ais_ota_set_setting_fw_offset(ais_ota_get_setting_fw_offset() + ais_ota_get_page_size());
                    update_bldr_settings = true;
                }

                if (update_bldr_settings)
                {
                    err_code = ais_ota_settings_write(bootloader_settings_event_handler);
                    if (err_code != ALI_OTA_SETTINGS_CODE_SUCCESS)
                    {
                        notify_error(p_ota, ALI_ERROR_SRC_OTA_FW_RX_WRITE_SETTINGS, NRF_ERROR_SETTINGS_FAIL);
                        return;
                    }
                    p_ota->state = ALI_OTA_STATE_WRITE_SETTINGS;
                }
                else
                {
                    p_ota->state = ALI_OTA_STATE_RECEIVE;
                    (void) send_bytes_received(p_ota);
                }
            }
            else
            {
                notify_error(p_ota, ALI_ERROR_SRC_OTA_FLASH_WRITE, NRF_ERROR_FLASH_STORE_FAIL);
                return;
            }
            break;

        default:
            break;
    }
}

/**@brief Function for handling command @ref ALI_CMD_FW_UPGRADE_REQ in state
 *        @ref ALI_OTA_STATE_IDLE.
 */
static void on_fw_upgrade_req (ali_ota_t * p_ota, uint8_t * p_data, uint16_t length)
{
    uint32_t num_pages, rx_fw_size;
    uint32_t err_code;
    uint8_t  l_len;
    bool     send_nack = false;
    bool     resume = false;

    /* Check parameters. */
    VERIFY_PARAM_NOT_NULL_VOID(p_data);
    if (length == 0)
    {
        return;
    }

    /* Check if resume from previous process. */
    resume = ais_ota_check_if_resume(p_data, length);

    /* Check if the new firmware version is accepted. */
    err_code = check_upgrade_fw_version(p_ota, p_data, length);
    if (err_code == NRF_SUCCESS || resume)
    {
        ais_ota_update_fw_version(p_data, length);

        l_len      = length - sizeof(uint32_t) - sizeof(uint16_t);
        rx_fw_size = EXTRACT_U32(p_data + l_len);

        if (rx_fw_size != 0)
        {
            p_ota->rx_fw_size   = rx_fw_size;
            p_ota->frames_recvd = 0;
            p_ota->crc          = EXTRACT_U16(p_data + l_len + sizeof(uint32_t));
            if (resume)
            {
                p_ota->bytes_recvd = (p_ota->rx_fw_size == ais_ota_get_setting_fw_offset())?
                                      ais_ota_get_setting_fw_offset():
                                      ais_ota_get_setting_fw_offset() & ~(ais_ota_get_page_size() - 1);
            }
            else
            {
                p_ota->bytes_recvd = 0;
            }

            /* Check if anything to erase. */
            num_pages = align_to_page(p_ota->rx_fw_size - p_ota->bytes_recvd, ais_ota_get_page_size()) / ais_ota_get_page_size();
            if (num_pages == 0)
            {
                /* Nothing to erase. */
                err_code = send_fw_upgrade_rsp(p_ota, true);
                if (err_code == NRF_SUCCESS)
                {
                    p_ota->state = ALI_OTA_STATE_RECEIVE;
                }
            }
            else
            {
                /* Erase flash. */
                err_code = ais_ota_flash_erase((uint32_t const *)(p_ota->bank_1_addr + p_ota->bytes_recvd),
                                               num_pages, flash_event_handler);
                if (err_code != ALI_OTA_FLASH_CODE_SUCCESS)
                {
                    notify_error(p_ota, ALI_ERROR_SRC_OTA_FWUP_REQ, NRF_ERROR_FLASH_ERASE_FAIL);
                    return;
                }
            }
        }
        else
        {
            send_nack = true;
        }
    }
    else
    {
        send_nack = true;
    }

    if (send_nack)
    {
        (void) send_fw_upgrade_rsp(p_ota, false);
    }
}


/**@brief Function for handling command @ref ALI_CMD_FW_DATA in state
 *        @ref ALI_OTA_STATE_RECEIVE.
 */
static void on_fw_data (ali_ota_t * p_ota, uint8_t * p_data, uint16_t length, uint8_t num_frames)
{
    uint32_t err_code;
    static uint16_t last_percent = 0;
    uint16_t percent;

    VERIFY_PARAM_NOT_NULL_VOID(p_data);
    if (length == 0)
    {
        return;
    }
    else if ((length & 0x3) != 0)
    {
        (void) send_error(p_ota);
        return;
    }

    /* Write Flash. */
    err_code = ais_ota_flash_store((uint32_t const *)(p_ota->bank_1_addr + p_ota->bytes_recvd),
                                   (uint32_t const *)p_data, length >> 2, flash_event_handler);
    if (err_code != ALI_OTA_FLASH_CODE_SUCCESS)
    {
        notify_error(p_ota, ALI_ERROR_SRC_OTA_FW_DATA, NRF_ERROR_FLASH_STORE_FAIL);
        return;
    }

    p_ota->state = ALI_OTA_STATE_WRITE;
    p_ota->bytes_recvd  += length;
    p_ota->frames_recvd += num_frames;

    /* Display progress, 5% as step */
    percent = p_ota->bytes_recvd * 100 / p_ota->rx_fw_size; /* Ensure no overflow */
    if ((percent - last_percent) >= 2) {
        printf("===>%dB\t%d%% ...\r\n", p_ota->bytes_recvd, percent);
        last_percent = percent;
    }
}

static uint16_t crc16_compute(uint8_t const *add, uint32_t size, void *p)
{
    return utils_crc16((uint8_t *)add, size);
}

static uint32_t crc32_compute(uint8_t const *add, uint32_t size, void *p)
{
    return utils_crc32((uint8_t *)add, size);
}

/**@brief Function for handling command @ref ALI_CMD_FW_XFER_FINISH in state
 *        @ref ALI_OTA_STATE_FW_CHECK.
 */
static void on_xfer_finished (ali_ota_t * p_ota, uint8_t * p_data, uint16_t length)
{
    uint32_t err_code;
    uint16_t crc;

    VERIFY_PARAM_NOT_NULL_VOID(p_data);
    if (length == 0)
    {
        return;
    }

    /* Check CRC here. */
    crc = crc16_compute((uint8_t const *)p_ota->bank_1_addr, p_ota->rx_fw_size, NULL);
    if (crc == p_ota->crc)
    {
        int32_t img_crc;

        img_crc = crc32_compute((uint8_t const *)p_ota->bank_1_addr, p_ota->rx_fw_size, NULL);
        ais_ota_update_setting_after_xfer_finished(p_ota->rx_fw_size, img_crc);

        err_code = ais_ota_settings_write(bootloader_settings_event_handler);
        if (err_code != ALI_OTA_SETTINGS_CODE_SUCCESS)
        {
            notify_error(p_ota, ALI_ERROR_SRC_OTA_XFER_FINISHED, NRF_ERROR_SETTINGS_FAIL);
            return;
        }

        p_ota->state = ALI_OTA_STATE_RESET_PREPARE;
    }
    else
    {
        err_code = send_crc_result(p_ota, false);
        if (err_code == NRF_SUCCESS)
        {
            p_ota->state = ALI_OTA_STATE_IDLE;
        }
    }
}


ret_code_t ali_ota_init(ali_ota_t * p_ota, ali_ota_init_t const * p_init)
{
    uint32_t  err_code;

    /* check parameters */
    VERIFY_PARAM_NOT_NULL(p_ota);
    VERIFY_PARAM_NOT_NULL(p_init);
    VERIFY_PARAM_NOT_NULL(p_init->event_handler);
    VERIFY_PARAM_NOT_NULL(p_init->tx_func);
    VERIFY_PARAM_NOT_NULL(p_init->p_fw_version);
    if (p_init->fw_version_len == 0 || p_init->fw_version_len > ALI_OTA_MAX_FW_VERSION_LEN)
    {
        return NRF_ERROR_INVALID_LENGTH;
    }

    /* Check version string. */
    err_code = parse_firmware_version (p_init->p_fw_version, p_init->fw_version_len);
    VERIFY_SUCCESS(err_code);

    /* Initialize DFU modules. */
    ais_ota_flash_init(true);

    ais_ota_settings_init();

    /* Initialize context */
    memset(p_ota, 0, sizeof(ali_ota_t));
    p_ota->feature_enable    = p_init->feature_enable;
    p_ota->state             = ALI_OTA_STATE_OFF;
    p_ota->event_handler     = p_init->event_handler;
    p_ota->p_evt_context     = p_init->p_evt_context;
    p_ota->tx_func           = p_init->tx_func;
    p_ota->p_tx_func_context = p_init->p_tx_func_context;
    p_ota->fw_ver_len        = p_init->fw_version_len;

    // derive border between application and swap
    p_ota->bank_1_addr = ais_ota_get_dst_addr();

    memcpy(p_ota->fw_ver, p_init->p_fw_version, p_init->fw_version_len);
    m_ptr = p_ota;

    return NRF_SUCCESS;
}


void ali_ota_reset(ali_ota_t * p_ota)
{
    /* check parameters */
    VERIFY_PARAM_NOT_NULL_VOID(p_ota);

    /* Reset state machine. */
    p_ota->state        = ALI_OTA_STATE_OFF;
    p_ota->rx_fw_size   = 0;
    p_ota->bytes_recvd  = 0;
    p_ota->frames_recvd = 0;
    p_ota->crc          = 0;
}


void ali_ota_on_command(ali_ota_t * p_ota, uint8_t cmd, uint8_t * p_data, uint16_t length, uint8_t num_frames)
{
    bool send_err = false;

    /* check parameters */
    VERIFY_PARAM_NOT_NULL_VOID(p_ota);
    if ((cmd & ALI_CMD_TYPE_MASK) != ALI_CMD_TYPE_FW_UPGRADE)
    {
        return;
    }

    /* State machine. */
    switch (p_ota->state)
    {
        case ALI_OTA_STATE_IDLE:
            if (cmd == ALI_CMD_FW_VERSION_REQ)              // cmd=0x20
            {
                (void) send_fw_version_rsp(p_ota);
            }
            else if (cmd == ALI_CMD_FW_UPGRADE_REQ)         // cmd=0x22
            {
                on_fw_upgrade_req(p_ota, p_data, length);
            }
            else
            {
                send_err = true;
            }
            break;

        case ALI_OTA_STATE_RECEIVE:
            if (cmd == ALI_CMD_FW_DATA)                     // cmd=0x2F
            {
                on_fw_data(p_ota, p_data, length, num_frames);
            }
            else if (cmd == ALI_CMD_FW_GET_INIT_FW_SIZE)    // cmd=0x27
            {
                (void) send_bytes_received(p_ota);
                if (p_ota->bytes_recvd >= p_ota->rx_fw_size)
                {
                    p_ota->state = ALI_OTA_STATE_FW_CHECK;
                }
            }
            else
            {
                send_err = true;
            }
            break;

        case ALI_OTA_STATE_FW_CHECK:
            if (cmd == ALI_CMD_FW_XFER_FINISH)              // cmd=0x28
            {
                char flag[5] = "Yes";
                int len = sizeof(flag);
                on_xfer_finished(p_ota, p_data, length);
                printf("Firmware download completed, let's set the flag.\r\n");
                aos_kv_set("fwup_ongoing", flag, len, 1);
            }
            else
            {
                send_err = true;
            }
            break;

        case ALI_OTA_STATE_UPGRADE_REPORT:
        case ALI_OTA_STATE_RESET_PREPARE:
        case ALI_OTA_STATE_OFF:
        case ALI_OTA_STATE_WRITE:
            send_err = true;
            break;
        default:
            break;
    }

    if (send_err)
    {
        (void) send_error(p_ota);
    }
}


void ali_ota_on_tx_done(ali_ota_t * p_ota, uint8_t cmd)
{
    /* check parameters */
    VERIFY_PARAM_NOT_NULL_VOID(p_ota);

    /* Check if it is the correct state. */
    switch (p_ota->state)
    {
        case ALI_OTA_STATE_RESET_PREPARE:
            if (cmd == ALI_CMD_FW_CHECK_RESULT)
            {
                notify_new_fw(p_ota);  // Let application reset.
            }
            break;

        case ALI_OTA_STATE_RECEIVE_ERR:
            if (cmd == ALI_CMD_ERROR)
            {
                (void) send_bytes_received(p_ota);
            }
            else if (cmd == ALI_CMD_FW_BYTES_RECEIVED)
            {
                p_ota->state = ALI_OTA_STATE_RECEIVE;
            }
            break;

        default:
            break;
    }
}


void ali_ota_on_auth(ali_ota_t * p_ota, bool is_authenticated)
{
    uint32_t err_code;

    /* check parameters */
    VERIFY_PARAM_NOT_NULL_VOID(p_ota);

    if (!is_authenticated)
    {
        return;
    }

    if (p_ota->state == ALI_OTA_STATE_OFF)
    {
        /* Check if image swapping has happened before boot. */
        if (ais_ota_check_if_update_finished())
        {
            ais_ota_update_settings_after_update_finished();
            err_code = ais_ota_settings_write(bootloader_settings_event_handler);
            if (err_code != ALI_OTA_SETTINGS_CODE_SUCCESS)
            {
                notify_error(p_ota, ALI_ERROR_SRC_OTA_NOTIFY_NEW_FW, NRF_ERROR_SETTINGS_FAIL);
                return;
            }

            p_ota->state = ALI_OTA_STATE_UPGRADE_REPORT;
        }
        else if (p_ota->feature_enable)
        {
            p_ota->state = ALI_OTA_STATE_IDLE;
        }
    }
    else
    {
        notify_error(p_ota, ALI_ERROR_SRC_OTA_AUTH_TIMING, NRF_ERROR_INVALID_STATE);
    }
}


void ali_ota_on_discontinuous_frame(ali_ota_t * p_ota)
{
    /* check parameters */
    VERIFY_PARAM_NOT_NULL_VOID(p_ota);

    /* Reactive only when receiving FW data. */
    if (p_ota->state == ALI_OTA_STATE_RECEIVE)
    {
        p_ota->state = ALI_OTA_STATE_RECEIVE_ERR;
    }
}


