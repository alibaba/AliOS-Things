#include <string.h>
#include "ota_breeze.h"
#include "ota_breeze_os.h"
#include "ota_breeze_plat.h"
#include "ota_breeze_transport.h"

static unsigned char ota_breeze_new_fw = 0;
#define EXTRACT_U16(d) (*((unsigned char *)(d)) | (*((unsigned char *)(d) + 1) << 8))
#define EXTRACT_U32(d)                                              \
    (*((unsigned char *)(d)) | (*((unsigned char *)(d) + 1) << 8) | \
    (*((unsigned char *)(d) + 2) << 16) | (*((unsigned char *)(d) + 3) << 24))

#define ENCODE_U16(d, val)                             \
{                                                      \
    *((unsigned char *)(d))     = (val)&0xFF;          \
    *((unsigned char *)(d) + 1) = ((val) >> 8) & 0xFF; \
}

#define ENCODE_U32(d, val)                              \
{                                                       \
    *((unsigned char *)(d))     = (val)&0xFF;           \
    *((unsigned char *)(d) + 1) = ((val) >> 8) & 0xFF;  \
    *((unsigned char *)(d) + 2) = ((val) >> 16) & 0xFF; \
    *((unsigned char *)(d) + 3) = ((val) >> 24) & 0xFF; \
}

bool ota_breeze_check_if_bins_supported()
{
#ifdef AOS_BINS
    bool is_support_mbins = true;
#else
    bool is_support_mbins = false;
#endif
    return is_support_mbins;
}

void ota_breeze_send_error()
{
    unsigned int err_code = 0;

    err_code = breeze_post_ext(OTA_BREEZE_CMD_ERROR, NULL, 0);
    if (err_code != OTA_BREEZE_SUCCESS) {
        OTA_BREEZE_LOG_E("send err failed\r\n");
    }
}

unsigned int ota_breeze_send_fw_version_rsp(unsigned char ota_cmd, unsigned char *buffer, unsigned int length)
{
    if((buffer == NULL) || (length == 0)) {
        return OTA_BREEZE_ERROR_INVALID_PARAM;
    }
    return breeze_post_ext(OTA_BREEZE_CMD_FW_VERSION_RSP, buffer, length);
}

static unsigned int ota_breeze_check_upgrade_fw_version(ota_breeze_version_t *version, unsigned char *p_data, unsigned char length)
{
    unsigned ret = 0;
    int version_result = 0;
    unsigned char  l_data_new[OTA_BREEZE_FW_VER_LEN + 1]; /* +1 for trailing zero */
    unsigned char  l_len;

    if((version == NULL) || (p_data == NULL) || (length == 0)) {
        ret = OTA_BREEZE_ERROR_INVALID_PARAM;
        goto VER_ERRO;
    }

    l_len = length - sizeof(unsigned int) - sizeof(unsigned short) - 1;
    if(l_len > OTA_BREEZE_FW_VER_LEN) {
        OTA_BREEZE_LOG_E("ver length too long");
        ret = OTA_BREEZE_ERROR_DATA_SIZE;
        goto VER_ERRO;
    }
    memcpy(l_data_new, p_data, l_len);
    l_data_new[l_len] = 0;
    version_result = strncmp(version->fw_ver, l_data_new, l_len);
    if (version_result == 0) {
        ret = OTA_BREEZE_ERROR_INVALID_VERSION;
        OTA_BREEZE_LOG_E("ver same err!");
    }
VER_ERRO:
    return ret;
}

unsigned int ota_breeze_align_to_page(unsigned int val, unsigned int page_size)
{
    return ((val + page_size - 1) & ~(page_size - 1));
}

/*@brief Send firmware upgrade response. */
unsigned int ota_breeze_send_fw_upgrade_rsp(unsigned char allow_upgrade)
{
    unsigned char ack = 0;
    ack = (allow_upgrade) ? 1 : 0;
    return breeze_post_ext(OTA_BREEZE_CMD_FW_UPGRADE_RSP, &ack, 1);
}

unsigned int ota_breeze_send_fwup_success()
{
    unsigned int err_code;
    unsigned char fwup_success = 0x01;
    return breeze_post_ext(OTA_BREEZE_CMD_FW_UPDATE_PROCESS, &fwup_success, 1);
}

/*@brief Send number of bytes of firmware received. */
unsigned int ota_breeze_send_bytes_received()
{
    unsigned int err_code;
    unsigned char tx_buff[16];
    _ota_ble_global_dat_t* p_ota = ota_breeze_get_global_data_center();
    if(p_ota == NULL) {
        return OTA_BREEZE_ERROR_INVALID_PARAM;
    }
    memset(tx_buff, 0x00, sizeof(tx_buff));
    ENCODE_U16(tx_buff, p_ota->frames_recvd);
    ENCODE_U32(tx_buff + sizeof(unsigned short), p_ota->bytes_recvd);

    err_code = breeze_post_ext(OTA_BREEZE_CMD_FW_BYTES_RECEIVED, tx_buff, sizeof(unsigned short) + sizeof(unsigned int));
    if (err_code != OTA_BREEZE_SUCCESS) {
        OTA_BREEZE_LOG_E("send rec err\r\n");
    }
    return err_code;
}

/*@brief Send the result of CRC check. */
unsigned int ota_breeze_send_crc_result(unsigned char crc_ok)
{
    unsigned char ack = 0;
    ack = (crc_ok) ? 1 : 0;
    return  breeze_post_ext(OTA_BREEZE_CMD_FW_CHECK_RESULT, &ack, 1);
}

ota_breeze_bin_type_t ota_breeze_get_image_type(unsigned int image_magic)
{
    ota_breeze_bin_type_t image_type = OTA_BIN_TYPE_INVALID;
    switch(image_magic) {
        case OTA_BIN_TYPE_MAGIC_APP:
            image_type = OTA_BIN_TYPE_APP;
            break;
        case OTA_BIN_TYPE_MAGIC_KERNEL:
            image_type = OTA_BIN_TYPE_MAGIC_KERNEL;
            break;
        case OTA_BIN_TYPE_MAGIC_SINGLE:
            image_type = OTA_BIN_TYPE_SINGLE;
            break;
        case OTA_BIN_TYPE_MAGIC_APP_KERNEL:
            image_type = OTA_BIN_TYPE_SINGLE;
            break;
        default:
            break;
    }
    return image_type;
}

unsigned int ota_breeze_on_fw_upgrade_req(unsigned char *buffer, unsigned int length)
{
    unsigned int err_code = 0;
    unsigned char l_len = 0;
    unsigned char resume = false;
    int ret = 0;
    _ota_ble_global_dat_t* p_ota = NULL;
    if((buffer == NULL) || (length <  sizeof(unsigned int) + sizeof(unsigned short))) {
        return OTA_BREEZE_ERROR_INVALID_PARAM;
    }
    p_ota = ota_breeze_get_global_data_center();
    if(p_ota == NULL) {
        return OTA_BREEZE_ERROR_INVALID_PARAM;
    }
    resume = ota_breeze_check_if_resume(buffer, length);
    ret = ota_breeze_check_upgrade_fw_version(&p_ota->verison, buffer, length);
    if (ret == 0 || resume) {
        ota_breeze_update_fw_version(buffer, length);
        l_len      = length - sizeof(unsigned int) - sizeof(unsigned short);
        p_ota->rx_fw_size = EXTRACT_U32(buffer + l_len);
        p_ota->frames_recvd = 0;
        p_ota->crc = EXTRACT_U16(buffer + l_len + sizeof(unsigned int));
        if(p_ota->rx_fw_size > sizeof(ota_breeze_image_t)) {
            p_ota->valid_fw_size = p_ota->rx_fw_size - sizeof(ota_breeze_image_t);
            ret = ota_breeze_breakpoint_process(p_ota->valid_fw_size, &p_ota->valid_bytes_recvd, resume);
            if(ret == 0) {
                if(p_ota->valid_bytes_recvd > 0) {
                    p_ota->bytes_recvd = p_ota->valid_bytes_recvd + sizeof(ota_breeze_image_t);
                }
            }
            else {
                err_code = OTA_BREEZE_ERROR_GET_BREAKPOINT_FAIL;
            }
        }
        else {
            err_code = OTA_BREEZE_ERROR_DATA_SIZE;
        }
    }
    return err_code;
}

unsigned int ota_breeze_on_fw_data(unsigned char *buffer, unsigned int length, unsigned char num_frames)
{
    unsigned int           err_code = OTA_BREEZE_SUCCESS;
    unsigned int           i = 0;
    unsigned int           bin_info_len = 0;
    ota_breeze_image_t     bin_info;
    ota_breeze_bin_type_t  bin_type;
    static unsigned short  last_percent = 0;
    unsigned short         percent;
    _ota_ble_global_dat_t* p_ota = NULL;
    if((buffer == NULL) || (length == 0)) {
        err_code = OTA_BREEZE_ERROR_INVALID_PARAM;
        goto OTA_BREEZE_TRANS_ERRO;
    }
    if ((length & 0x03) != 0) {
        ota_breeze_send_error();
        err_code = OTA_BREEZE_ERROR_DATA_SIZE;
        goto OTA_BREEZE_TRANS_ERRO;
    }
    p_ota = ota_breeze_get_global_data_center();
    if(p_ota == NULL) {
        err_code = OTA_BREEZE_ERROR_INVALID_PARAM;
        goto OTA_BREEZE_TRANS_ERRO;
    }
    bin_info_len = sizeof(ota_breeze_image_t);
    if (p_ota->valid_bytes_recvd == OTA_IMAGE_MAGIC_OFFSET) {
        if(length >= bin_info_len) {
            memcpy(&bin_info, buffer, bin_info_len);
            bin_type = ota_breeze_get_image_type(bin_info.image_magic);
            if(bin_type == OTA_BIN_TYPE_INVALID) {
                OTA_BREEZE_LOG_E("magic error\r\n");
                err_code = OTA_BREEZE_ERROR_NOT_SUPPORTED;
                goto OTA_BREEZE_TRANS_ERRO;
            }
            if ((bin_type != OTA_BIN_TYPE_SINGLE) &&
                (ota_breeze_check_if_bins_supported() == false)) {
                    err_code = OTA_BREEZE_ERROR_NOT_SUPPORTED;
                    goto OTA_BREEZE_TRANS_ERRO;
            }
            if(bin_info.image_size != p_ota->valid_fw_size) {
                err_code = OTA_BREEZE_ERROR_INVALID_LENGTH;
                goto OTA_BREEZE_TRANS_ERRO;
            }
            ota_breeze_set_image_info_crc16(buffer, bin_info_len);
            buffer += bin_info_len;
            length -= bin_info_len;
            p_ota->bytes_recvd += bin_info_len;
            ota_breeze_set_bin_type((unsigned char)bin_type);
        }
        else {
            err_code = OTA_BREEZE_ERROR_INVALID_LENGTH;
            goto OTA_BREEZE_TRANS_ERRO;
        }
    }

    if (ota_breeze_write(&p_ota->valid_bytes_recvd, (char *)buffer, length) != 0) {
        err_code = OTA_BREEZE_ERROR_FLASH_STORE_FAIL;
        goto OTA_BREEZE_TRANS_ERRO;
    }
    p_ota->frames_recvd += num_frames;
    p_ota->bytes_recvd += length;
    percent = p_ota->bytes_recvd * 100 / p_ota->rx_fw_size; /* Ensure no overflow */
    if(percent < last_percent) {
        /*breakpoint need to clear last_percent*/
        last_percent = 0;
    }
    if ((percent - last_percent) >= 5) {
        OTA_BREEZE_LOG_I("===>%dB\t%d%% ...\r\n", p_ota->bytes_recvd, percent);
        last_percent = percent;
        if(ota_breeze_save_breakpoint(p_ota->valid_bytes_recvd) !=0 ) {
            err_code = OTA_BREEZE_ERROR_SETTINGS_FAIL;
        }
    }
OTA_BREEZE_TRANS_ERRO:
    return err_code;
}

unsigned int ota_breeze_is_in_check_status()
{
    unsigned int status = OTA_BREEZE_ERROR_INVALID_STATE;
    _ota_ble_global_dat_t* p_ota = ota_breeze_get_global_data_center();
    if(p_ota == NULL) {
        return OTA_BREEZE_ERROR_INVALID_PARAM;
    }
    if (p_ota->bytes_recvd >= p_ota->rx_fw_size) {
        status = OTA_BREEZE_SUCCESS;
    }
    return status;
}

void ota_breeze_reset()
{
    _ota_ble_global_dat_t* p_ota = ota_breeze_get_global_data_center();
    if(p_ota == NULL) {
        return;
    }
    OTA_BREEZE_LOG_I("discnt\r\n");
    /* Reset state machine. */
    p_ota->ota_breeze_status = OTA_BREEZE_STATE_OFF;
    p_ota->rx_fw_size   = 0;
    p_ota->bytes_recvd  = 0;
    p_ota->frames_recvd = 0;
    p_ota->crc          = 0;
    p_ota->valid_bytes_recvd = 0;
    if (ota_breeze_new_fw) {
        ota_breeze_reboot();
    }
}

void ota_breeze_on_tx_done(unsigned char cmd)
{
    _ota_ble_global_dat_t* p_ota = ota_breeze_get_global_data_center();
    if(p_ota == NULL) {
        return;
    }
    switch (p_ota->ota_breeze_status) {
        case OTA_BREEZE_STATE_RESET_PREPARE:
            if (cmd == OTA_BREEZE_CMD_FW_CHECK_RESULT) {
                ota_breeze_new_fw = true;
                ota_breeze_disconnect();
            }
            break;
        case OTA_BREEZE_STATE_RECEIVE_ERR:
            if (cmd == OTA_BREEZE_CMD_ERROR) {
                ota_breeze_send_bytes_received();
            }
            else if (cmd == OTA_BREEZE_CMD_FW_BYTES_RECEIVED) {
                p_ota->ota_breeze_status = OTA_BREEZE_STATE_RECEIVE;
            }
            break;
        default:
            break;
    }
}

void ota_breeze_on_auth(unsigned char is_authenticated)
{
    unsigned int err_code;
    _ota_ble_global_dat_t* p_ota = ota_breeze_get_global_data_center();
    if(p_ota == NULL) {
        goto OTA_BREEZE_AUTH_OVER;
    }
    if (ota_breeze_rollback() == 0) {
        if(ota_breeze_send_fwup_success() != OTA_BREEZE_SUCCESS) {
            goto OTA_BREEZE_AUTH_OVER;
        }
        OTA_BREEZE_LOG_I("OTA OK!\r\n");
    }
    if (!is_authenticated) {
        goto OTA_BREEZE_AUTH_OVER;
    }
    if (p_ota->ota_breeze_status == OTA_BREEZE_STATE_OFF) {
        if (p_ota->feature_enable) {
            p_ota->ota_breeze_status = OTA_BREEZE_STATE_IDLE;
        }
    }
    else {
        OTA_BREEZE_LOG_E("err stus\r\n");
    }
OTA_BREEZE_AUTH_OVER:
    OTA_BREEZE_LOG_I("auth over");
    return;
}

void ota_breeze_on_discontinuous_frame()
{
    _ota_ble_global_dat_t* p_ota = ota_breeze_get_global_data_center();
    if(p_ota == NULL) {
        return;
    }
    if (p_ota->ota_breeze_status == OTA_BREEZE_STATE_RECEIVE) {
        p_ota->ota_breeze_status = OTA_BREEZE_STATE_RECEIVE_ERR;
    }
    ota_breeze_reboot();
}

