/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include "se_driver.h"
#include "irot_hal.h"
#include "chip_log.h"
#include "chip_config.h"

#if (CONFIG_CHIP_TYPE == CHIP_TYPE_SE_MTK_CMD)

#define DES_BLOCK_LENGTH        0x08
#define DES_BLOCK_PADING_MASK   0xF8

static void* session_handle = NULL;

static int crc16_ccit_false(const uint8_t* in_data, uint32_t in_len, uint16_t init_value, uint8_t* pout)
{
    uint16_t poly = init_value;
    uint16_t wcrc = 0xFFFF;
    uint32_t i;
    int j;

    for (i = 0; i < in_len; ++i) {
        wcrc ^= (in_data[i] << 8);
        for (j = 0; j < 8; ++j) {
            if (wcrc & 0x8000) {
                wcrc = (wcrc << 1) ^ poly;
            } else {
                wcrc <<= 1;
            }
        }
    }

    wcrc ^= 0xFFFF;
    pout[0] = (uint8_t)(wcrc >> 8);
    pout[1] = (uint8_t)(wcrc & 0xFF);

    return 0;
}

#if (CONFIG_CHIP_KEY_TYPE == CHIP_KEY_TYPE_3DES || \
     CONFIG_CHIP_KEY_TYPE == CHIP_KEY_TYPE_AES)
static int pkcs5_pading(uint8_t* in_data, uint32_t in_len, uint32_t* out_len, uint8_t block_len)
{
    uint8_t fill_len;
    uint8_t fill_value;

    fill_len = block_len - (in_len % block_len);
    fill_value = fill_len;

    while (fill_len) {
         in_data[in_len++] = fill_value;
         fill_len--;
    }

    *out_len = in_len;

    return 0;
}
#endif

////////////////////////////////////////////////////////////////////////////////

#define SE_ID2_LENGTH                       12
#define CRC_INIT_VALUE                      0x1021
#define CONST_KEY_ID                        0x12

enum
{
    INDEX_CLA                               = 0x00,
    INDEX_INS                               = 0x01,
    INDEX_P1                                = 0x02,
    INDEX_P2                                = 0x03,
    INDEX_LC                                = 0x04,
    INDEX_DATA                              = 0x05,
};

#define CMD_APDU_HEAD_LENGTH                0x05
#define RSP_APDU_SW_LENGTH                  0x02

#define MAX_CMD_APDU_LENGTH                 (CMD_APDU_HEAD_LENGTH + 255 + 1)
#define MAX_RSP_APDU_LENGTH                 (0x100 + RSP_APDU_SW_LENGTH)

#define BLOCK_DATA_LENGTH                   0xF0

static irot_result_t open_session(void** handle)
{
    irot_result_t ret;

    chip_log_debug("=> SE open session\n");

    ret = se_open_session(handle);
    chip_log_debug("<= SE open session, (ret = %d).\n", ret);
    if (ret != IROT_SUCCESS) {
        chip_log_error("SE open session error.\n");
    }

    return ret;
}

static irot_result_t close_session(void* handle)
{
    irot_result_t ret;

    chip_log_debug("=> SE close session.\n");

    ret = se_close_session(handle);
    chip_log_debug("<= SE close session, (ret = %d).\n", ret);
    if (ret != IROT_SUCCESS) {
        chip_log_error("SE close session error.\n");
    }

    return ret;
}

static irot_result_t apdu_transmit_wrap(void* handle, uint8_t* cmd_buf, uint32_t cmd_len,
                                        uint8_t* rsp_buf, uint32_t* rsp_len, uint32_t max_expect_len)
{
    irot_result_t ret;
    uint32_t counter = 0;
    uint32_t buf_len = 0;

    /* backup the buffer length */
    buf_len = *rsp_len;

    do {
        /* ensure exit the loop */
        counter++;
        if (counter >= 3) {
            ret = IROT_ERROR_GENERIC;
            break;
        }

        chip_log_debug("================================================================================\n");
        chip_log_hex_dump("Command APDU:", cmd_buf, cmd_len);

        /* reset the buffer length */
        *rsp_len = buf_len;

        /* call driver send the APDU command */
        chip_log_debug("=> SE transmit.\n");
        ret = se_transmit(handle, cmd_buf, cmd_len, rsp_buf, rsp_len);
        chip_log_debug("<= SE transmit, (ret = %d, rsp_len = %d).\n", ret, *rsp_len);
        if (ret != IROT_SUCCESS) {
            chip_log_debug("SE transmit error.\n");
            break;
        }

        chip_log_hex_dump("Response APDU:", rsp_buf, *rsp_len);
        chip_log_debug("================================================================================\n");

        /* length error */
        if ((*rsp_len < RSP_APDU_SW_LENGTH) || (*rsp_len > MAX_RSP_APDU_LENGTH))
        {
            chip_log_error("bad response apdu length, %d.\n", *rsp_len);
            ret = IROT_ERROR_GENERIC;
            break;
        }

        /* CASE: return 0x9F */
        if ((*rsp_len == 0x02) && (rsp_buf[*rsp_len - 2] == 0x9F)) {
            chip_log_debug("response apdu with 0x9F.\n");
            cmd_buf[INDEX_CLA] = 0x00;
            cmd_buf[INDEX_INS] = 0xB2;
            cmd_buf[INDEX_P1] = 0xFF;
            cmd_buf[INDEX_P2] = 0xFF;
            cmd_buf[INDEX_LC] = rsp_buf[*rsp_len - 1];
            cmd_len = CMD_APDU_HEAD_LENGTH;
            continue;
        } else if ((rsp_buf[*rsp_len - 2] == 0x90) && (rsp_buf[*rsp_len - 1] == 0x00)) {
            *rsp_len -= 0x02;
            break;
        } else {
            chip_log_error("response apdu data error.\n");
            ret = IROT_ERROR_GENERIC;
            break;
        }
    } while (1);

    return ret;
}

#if defined(CONFIG_CHIP_SEND_SELECT_COMMAND)
static irot_result_t select_application(void* handle, uint8_t* cmd_buf, uint8_t* rsp_buf, uint32_t* rsp_len)
{
    irot_result_t ret;

    /* select command */
    cmd_buf[INDEX_CLA] = 0x00;
    cmd_buf[INDEX_INS] = 0xA4;
    cmd_buf[INDEX_P1] = 0x00;
    cmd_buf[INDEX_P2] = 0x0C;
    cmd_buf[INDEX_LC] = 0x02;
    cmd_buf[INDEX_DATA] = 0x1D;
    cmd_buf[INDEX_DATA + 1] = 0x02;
    ret = apdu_transmit_wrap(handle, cmd_buf, CMD_APDU_HEAD_LENGTH + 0x02, rsp_buf, rsp_len, 0x00);

    return ret;
}
#endif

irot_result_t irot_hal_init()
{
    irot_result_t ret;

    ret = open_session(&session_handle);
    if (ret != IROT_SUCCESS) {
        goto _out;
    }

_out:
    return ret;
}

irot_result_t irot_hal_cleanup()
{
    return close_session(session_handle);
}

irot_result_t irot_hal_get_id2(uint8_t* id, uint32_t* len)
{
    irot_result_t ret;
    uint8_t cmd_buf[MAX_CMD_APDU_LENGTH];
    uint8_t rsp_buf[MAX_RSP_APDU_LENGTH];
    uint32_t rsp_len = sizeof(rsp_buf);
    uint8_t crc_buf[0x02];

    /* select application */
#if defined(CONFIG_CHIP_SEND_SELECT_COMMAND)
    ret = select_application(session_handle, cmd_buf, rsp_buf, &rsp_len);
    if (ret != IROT_SUCCESS)
    {
        goto _out;
    }
#endif

    /* get ID */
    memset(cmd_buf, 0x00, CMD_APDU_HEAD_LENGTH);
    cmd_buf[INDEX_CLA] = 0x00;
    cmd_buf[INDEX_INS] = 0xDC;
    cmd_buf[INDEX_P1] = 0xFF;
    cmd_buf[INDEX_P2] = 0xFF;
    cmd_buf[INDEX_LC] = 0x03;
    cmd_buf[INDEX_DATA + 0] = 0x52;
    cmd_buf[INDEX_DATA + 1] = 0xE6;
    cmd_buf[INDEX_DATA + 2] = 0x02;
    rsp_len = sizeof(rsp_buf);
    ret = apdu_transmit_wrap(session_handle,
               cmd_buf, CMD_APDU_HEAD_LENGTH + 0x03, rsp_buf, &rsp_len, 0x11);
    if (ret != IROT_SUCCESS) {
        goto _out;
    }

    /* 3 bytes head */
    if (rsp_len != (0x03 + SE_ID2_LENGTH + 0x02)) {
        ret = IROT_ERROR_GENERIC;
        goto _out;
    }

    /* ID2 data */
    if (rsp_buf[2] != SE_ID2_LENGTH) {
        ret = IROT_ERROR_GENERIC;
        goto _out;
    }

    /* check crc */
    crc16_ccit_false(rsp_buf, rsp_len - 2, CRC_INIT_VALUE, crc_buf);
    if ((rsp_buf[rsp_len - 2] != crc_buf[0]) || (rsp_buf[rsp_len - 1] != crc_buf[1])) {
        ret = IROT_ERROR_GENERIC;
        goto _out;
    }

    rsp_len -= (0x03 + 0x02);
    if (rsp_len > *len) {
        ret = IROT_ERROR_GENERIC;
        goto _out;
    } else {
        /* | 2 TAG | 1 len | */
        memcpy(id, rsp_buf + 3, rsp_len);
        *len = rsp_len;
    }

_out:
    return ret;
}

#if (CONFIG_CHIP_KEY_TYPE == CHIP_KEY_TYPE_3DES || \
     CONFIG_CHIP_KEY_TYPE == CHIP_KEY_TYPE_AES)
irot_result_t irot_hal_sym_crypto(key_object* key_obj, uint8_t key_id,
                                  const uint8_t* iv, uint32_t iv_len,
                                  const uint8_t* in, uint32_t in_len,
                                  uint8_t* out, uint32_t* out_len,
                                  sym_crypto_param_t* crypto_param)
{
    irot_result_t ret;
    uint8_t cmd_buf[MAX_CMD_APDU_LENGTH];
    uint8_t rsp_buf[MAX_RSP_APDU_LENGTH];
    uint32_t rsp_len = sizeof(rsp_buf);
    uint32_t copy_len = 0;
    uint32_t offset;
    uint32_t max_expect_len;
    int result;
    uint8_t crc_buf[0x02];
    uint32_t input_data_len = in_len;
    uint8_t mode = crypto_param->mode;
    uint32_t out_buf_len = *out_len;
    uint32_t out_offset = 0;

    if ((in == NULL) || (in_len == 0x00) ||
        ((in_len % DES_BLOCK_LENGTH) != 0) || (in_len > BLOCK_DATA_LENGTH)) {
        ret = IROT_ERROR_BAD_PARAMETERS;
        goto _out;
    }

    /* select application */
#if defined(CONFIG_CHIP_SEND_SELECT_COMMAND)
    ret = select_application(session_handle, cmd_buf, rsp_buf, &rsp_len);
    if (ret != IROT_SUCCESS) {
        goto _out;
    }
#endif
    memset(cmd_buf, 0x00, CMD_APDU_HEAD_LENGTH);

    /* send data in loop, may be more than 1 block */
    while (in_len > 0) {
        cmd_buf[INDEX_CLA] = 0x00;
        cmd_buf[INDEX_INS] = 0xDC;

        /* fill P1,P2 */
        offset = INDEX_P1;
        cmd_buf[offset++] = 0xFF;
        cmd_buf[offset++] = 0xFF;

        /* skip LC */
        offset++;

        cmd_buf[offset++] = 0x51;
        cmd_buf[offset++] = 0x00;
        cmd_buf[offset++] = 0x01;
        cmd_buf[offset++] = (mode == MODE_ENCRYPT) ? 0x51 : 0x52;
        cmd_buf[offset++] = 0x02;
        cmd_buf[offset++] = CONST_KEY_ID;
        copy_len = (in_len > BLOCK_DATA_LENGTH) ? BLOCK_DATA_LENGTH : in_len;
        cmd_buf[offset++] = (uint8_t)((copy_len >> 8) & 0xFF);
        cmd_buf[offset++] = (uint8_t)(copy_len & 0xFF);

        /* data */
        memcpy(&cmd_buf[offset], in, copy_len);
        in += copy_len;
        in_len -= copy_len;
        offset += copy_len;

        /* the length of LC */
        cmd_buf[INDEX_LC] = (offset - CMD_APDU_HEAD_LENGTH) + 0x02;
        crc16_ccit_false(cmd_buf, offset, CRC_INIT_VALUE, cmd_buf + offset);
        offset += 0x02;

        rsp_len = sizeof(rsp_buf);
        if (mode == MODE_ENCRYPT) {
            max_expect_len = (copy_len + DES_BLOCK_LENGTH) & DES_BLOCK_PADING_MASK;
            max_expect_len += 0x02;
        } else {
            max_expect_len = (copy_len - 1);
            max_expect_len += 0x02;
        }

        ret = apdu_transmit_wrap(session_handle, cmd_buf, offset, rsp_buf, &rsp_len, max_expect_len);
        if (ret != IROT_SUCCESS) {
            goto _out;
        }

        /* check crc */
        crc16_ccit_false(rsp_buf, rsp_len - 2, CRC_INIT_VALUE, crc_buf);
        if ((rsp_buf[rsp_len - 2] != crc_buf[0]) || (rsp_buf[rsp_len - 1] != crc_buf[1])) {
            ret = IROT_ERROR_GENERIC;
            goto _out;
        }
        rsp_len -= 0x02;

        /* output data */
        if (rsp_len > out_buf_len) {
            ret = IROT_ERROR_GENERIC;
            goto _out;
        } else {
            memcpy(out + out_offset, rsp_buf, rsp_len);
            out_offset += rsp_len;
            out_buf_len -= rsp_len;
        }
    }

    *out_len = out_offset;
    if (mode == MODE_ENCRYPT) {
        if ((input_data_len + DES_BLOCK_LENGTH) != *out_len) {
            chip_log_error("sym crypto encrypt data length error\n");
            ret = IROT_ERROR_GENERIC;
            goto _out;
        }
        *out_len -= DES_BLOCK_LENGTH;
    } else {
        result = pkcs5_pading(out, *out_len, out_len, DES_BLOCK_LENGTH);
        if (result != 0) {
            ret = IROT_ERROR_BAD_PARAMETERS;
            goto _out;
        }
        if (input_data_len != *out_len) {
            chip_log_error("sym crypto decrypt pkcs5 pading data length error\n");
            ret = IROT_ERROR_GENERIC;
        }
    }

_out:
    return ret;
}

#endif  /* CONFIG_CHIP_KEY_TYPE */

irot_result_t irot_hal_hash_sum(const uint8_t* in, uint32_t in_len,
                                uint8_t* out, uint32_t* out_len, hash_t type)
{
    return IROT_ERROR_NOT_SUPPORTED;
}

#endif  /* CONFIG_CHIP_TYPE */
