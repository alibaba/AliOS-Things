#include "ota_breeze_transport.h"
#include "ota_breeze.h"
#include "ota_log.h"
#include "ota_breeze_plat.h"

static unsigned char ota_breeze_new_fw = 0;
#define EXTRACT_U16(d) (*((unsigned char *)(d)) | (*((unsigned char *)(d) + 1) << 8))
#define EXTRACT_U32(d)                                     \
    (*((unsigned char *)(d)) | (*((unsigned char *)(d) + 1) << 8) | \
    (*((unsigned char *)(d) + 2) << 16) | (*((unsigned char *)(d) + 3) << 24))

#define ENCODE_U16(d, val)                           \
{                                                \
    *((unsigned char *)(d))     = (val)&0xFF;          \
    *((unsigned char *)(d) + 1) = ((val) >> 8) & 0xFF; \
}

#define ENCODE_U32(d, val)                            \
{                                                 \
    *((unsigned char *)(d))     = (val)&0xFF;           \
    *((unsigned char *)(d) + 1) = ((val) >> 8) & 0xFF;  \
    *((unsigned char *)(d) + 2) = ((val) >> 16) & 0xFF; \
    *((unsigned char *)(d) + 3) = ((val) >> 24) & 0xFF; \
}

static ota_breeze_bin_info_t ota_breeze_bin_info[] = {
    { OTA_BREEZE_BIN_TYPE_APP,    OTA_BREEZE_BIN_TYPE_MAGIC_APP},
    { OTA_BREEZE_BIN_TYPE_KERNEL, OTA_BREEZE_BIN_TYPE_MAGIC_KERNEL},
    { OTA_BREEZE_BIN_TYPE_SINGLE, OTA_BREEZE_BIN_TYPE_MAGIC_SINGLE},
};

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

    err_code = breeze_post_ext( OTA_BREEZE_CMD_ERROR, NULL, 0);
    if (err_code != OTA_BREEZE_SUCCESS) {
        printf("send error failed\r\n");
    }
}

unsigned int ota_breeze_send_fw_version_rsp(unsigned char ota_cmd, unsigned char *buffer, unsigned int length)
{
    if((buffer == NULL) || (length == 0)) {
        return OTA_BREEZE_ERROR_INVALID_PARAM;
    }
    return breeze_post_ext(OTA_BREEZE_CMD_FW_VERSION_RSP, buffer, length);
}

int ota_breeze_split_sw_ver(char *data, unsigned int *v0, unsigned int *v1, unsigned int *v2)
{
    int i = sscanf(data, "%d.%d.%d", (int *)v0, (int *)v1, (int *)v2);
    return ((i == 3) ? 0 : -1);
}

static unsigned int ota_breeze_check_upgrade_fw_version(ota_breeze_version_t *version, unsigned char *p_data, unsigned char length)
{
#ifndef AOS_BINS // <TODO> rework later to support BINS, the version foramt may
                 // need change
    unsigned int v_old[3], v_new[3];
    unsigned char  l_data_old[OTA_BREEZE_FW_VER_LEN + 1]; // +1 for trailing zero
    unsigned char  l_data_new[OTA_BREEZE_FW_VER_LEN + 1]; // +1 for trailing zero
    unsigned char  l_len;

    if((version == NULL) || (p_data == NULL) || (length == 0)) {
        return OTA_BREEZE_ERROR_INVALID_PARAM;
    }
    // Copy to stack variable as trailing zero is required.
    memcpy(l_data_old, version->fw_ver, version->fw_ver_len);
    l_data_old[version->fw_ver_len] = 0;

    l_len = length - sizeof(unsigned int) - sizeof(unsigned short) - 1;
    memcpy(l_data_new, p_data, l_len);
    l_data_new[l_len] = 0;

    // Split SW version into 3 parts.
    if(ota_breeze_split_sw_ver((char *)l_data_old, v_old, v_old + 1, v_old + 2) < 0) {
        return OTA_BREEZE_ERROR_INVALID_DATA;
    }

    if(ota_breeze_split_sw_ver((char *)l_data_new, v_new, v_new + 1, v_new + 2) < 0) {
        return OTA_BREEZE_ERROR_INVALID_DATA;
    }

    // Try to reconstruct the version string.
    memset(l_data_new, 0, sizeof(l_data_new));
    sprintf((char *)l_data_new, "%d.%d.%d", (int)v_new[0], (int)v_new[1],
            (int)v_new[2]);
    if (memcmp(l_data_new, p_data, l_len) != 0) {
        return OTA_BREEZE_ERROR_FORBIDDEN;
    }

    // Check digits in software version
    if (v_new[0] > v_old[0]) { // x
        return 0;
    } else if (v_new[0] < v_old[0]) {
        return OTA_BREEZE_ERROR_FORBIDDEN;
    }

    if (v_new[1] > v_old[1]) { // y
        return 0;
    } else if (v_new[1] < v_old[1]) {
        return OTA_BREEZE_ERROR_FORBIDDEN;
    }

    if (v_new[2] <= v_old[2]) { // z
        return OTA_BREEZE_ERROR_FORBIDDEN;
    }
#endif
    return OTA_BREEZE_SUCCESS;
}

unsigned int ota_breeze_align_to_page(unsigned int val, unsigned int page_size)
{
    return ((val + page_size - 1) & ~(page_size - 1));
}

/**@brief Send firmware upgrade response. */
unsigned int ota_breeze_send_fw_upgrade_rsp(unsigned char allow_upgrade)
{
    unsigned int err_code;
    unsigned char tx_buf[2] = {0, 0};
    tx_buf[0] = (allow_upgrade) ? 1 : 0;
    err_code = breeze_post_ext(OTA_BREEZE_CMD_FW_UPGRADE_RSP, tx_buf, 1);
    return err_code;
}

void ota_breeze_send_fwup_success()
{
    unsigned int err_code;
    unsigned char fwup_success = 0x01;
    err_code = breeze_post_ext(OTA_BREEZE_CMD_FW_UPDATE_PROCESS, &fwup_success, 1);
    if (err_code != OTA_BREEZE_SUCCESS) {
        printf("report fwup failed\r\n");
    }
}

/**@brief Send number of bytes of firmware received. */
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
        printf("send recvd err\r\n");
    }
    return err_code;
}

/**@brief Send the result of CRC check. */
unsigned int ota_breeze_send_crc_result(unsigned char crc_ok)
{
    unsigned int err_code;
    unsigned char tx_buff[2] = {0x00, 0x00};
    tx_buff[0] = (crc_ok) ? 1 : 0;
    err_code = breeze_post_ext(OTA_BREEZE_CMD_FW_CHECK_RESULT, tx_buff, 1);
    if (err_code != OTA_BREEZE_SUCCESS) {
        printf("send crc err\r\n");
    }
    return err_code;
}

unsigned int ota_breeze_on_fw_upgrade_req(unsigned char *buffer, unsigned int length)
{
    unsigned int err_code = 0;
    unsigned char l_len = 0;
    unsigned char resume = false;
    _ota_ble_global_dat_t* p_ota = NULL;
    if((buffer == NULL) || (length <  sizeof(unsigned int) + sizeof(unsigned short))) {
        return OTA_BREEZE_ERROR_INVALID_PARAM;
    }
    p_ota = ota_breeze_get_global_data_center();
    if(p_ota == NULL) {
        return OTA_BREEZE_ERROR_INVALID_PARAM;
    }
    resume = ota_breeze_check_if_resume(buffer, length);
    err_code = ota_breeze_check_upgrade_fw_version(&p_ota->verison, buffer, length);
    if (err_code == OTA_BREEZE_SUCCESS || resume) {
        ota_breeze_update_fw_version(buffer, length);
        l_len      = length - sizeof(unsigned int) - sizeof(unsigned short);
        p_ota->rx_fw_size = EXTRACT_U32(buffer + l_len);
        p_ota->frames_recvd = 0;
        p_ota->crc = EXTRACT_U16(buffer + l_len + sizeof(unsigned int));
        if(p_ota->rx_fw_size > 0) {
            err_code = ota_breeze_breakpoint_process(p_ota->rx_fw_size, &p_ota->bytes_recvd, resume);
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
    unsigned int           bin_magic = 0;
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
    if (p_ota->bytes_recvd <= OTA_BREEZE_BIN_TYPE_INFO_OFFSET &&
        p_ota->bytes_recvd + length > OTA_BREEZE_BIN_TYPE_INFO_OFFSET) {
        bin_magic = EXTRACT_U32(buffer + OTA_BREEZE_BIN_TYPE_INFO_OFFSET - p_ota->bytes_recvd);
        for (i = 0; i < sizeof(ota_breeze_bin_info) / sizeof(ota_breeze_bin_info[0]); i++) {
            if (ota_breeze_bin_info[i].magic == bin_magic) {
                break;
            }
        }
        if (i >= sizeof(ota_breeze_bin_info) / sizeof(ota_breeze_bin_info[0])) {
            printf("magic error\r\n");
            err_code = OTA_BREEZE_ERROR_NOT_SUPPORTED;
            goto OTA_BREEZE_TRANS_ERRO;
        }
        bin_type = ota_breeze_bin_info[i].type;
#ifdef AOS_BINS
        if (bin_type == OTA_BREEZE_BIN_TYPE_SINGLE ||
            ota_breeze_check_if_bins_supported() == false) {
            err_code = OTA_BREEZE_ERROR_NOT_SUPPORTED;
            goto OTA_BREEZE_TRANS_ERRO;
        }
        //printf("bin type 0x%02x ...", bin_type);
        ais_ota_set_upgrade_bin_type_info(bin_type);
#else
        if (bin_type != OTA_BREEZE_BIN_TYPE_SINGLE ||
            ota_breeze_check_if_bins_supported() == true) {
            err_code = OTA_BREEZE_ERROR_NOT_SUPPORTED;
            goto OTA_BREEZE_TRANS_ERRO;
        }
#endif
    }
    if (ota_breeze_write(&p_ota->bytes_recvd, (char *)buffer, length) != 0) {
        err_code = OTA_BREEZE_ERROR_FLASH_STORE_FAIL;
        goto OTA_BREEZE_TRANS_ERRO;
    }
    p_ota->frames_recvd += num_frames;
    if(ota_breeze_set_breakpoint(p_ota->bytes_recvd) !=0 ) {
        err_code = OTA_BREEZE_ERROR_SETTINGS_FAIL;
        goto OTA_BREEZE_TRANS_ERRO;
    }
    percent = p_ota->bytes_recvd * 100 / p_ota->rx_fw_size; /* Ensure no overflow */
    if(percent < last_percent) {
        last_percent = 0;//breakpoint need to clear last_percent
    }
    if ((percent - last_percent) >= 2) {
        printf("===>%dB\t%d%% ...\r\n", p_ota->bytes_recvd, percent);
        last_percent = percent;
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
    printf("disconnect\r\n");
    /* Reset state machine. */
    p_ota->ota_breeze_status = OTA_BREEZE_STATE_OFF;
    p_ota->rx_fw_size   = 0;
    p_ota->bytes_recvd  = 0;
    p_ota->frames_recvd = 0;
    p_ota->crc          = 0;

    if (ota_breeze_new_fw) {
        printf("ota completed reboot!");
        ota_reboot();
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
        printf("ota success\r\n");
        ota_breeze_send_fwup_success();
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
        printf("error status\r\n");
    }
OTA_BREEZE_AUTH_OVER:
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
    ota_reboot();
}

