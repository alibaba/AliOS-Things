/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */



#include "iotx_dm_internal.h"
#include "iot_export_ota.h"
#include "dm_ota.h"
#include "dm_fota.h"
#include "dm_conn.h"
#include "dm_cm_wrapper.h"
#include "dm_message.h"

#ifdef OTA_ENABLED
static dm_fota_ctx_t g_dm_fota_ctx;

static dm_fota_ctx_t* _dm_fota_get_ctx(void)
{
    return &g_dm_fota_ctx;
}
#endif

int dm_fota_init(void)
{
#ifdef OTA_ENABLED
    dm_fota_ctx_t *ctx = _dm_fota_get_ctx();

    memset(ctx,0,sizeof(dm_fota_ctx_t));
#endif
    return SUCCESS_RETURN;
}

int dm_fota_deinit(void)
{
#ifdef OTA_ENABLED
    dm_fota_ctx_t *ctx = _dm_fota_get_ctx();

     memset(ctx,0,sizeof(dm_fota_ctx_t));
#endif
    return SUCCESS_RETURN;
}

#ifdef OTA_ENABLED
static int _dm_fota_send_new_config_to_user(void *ota_handle)
{
    int res = 0, message_len = 0;
    char *message = NULL;
    char *version = NULL;
    const char *fota_new_config_fmt = "{\"version\":\"%s\"}";

    IOT_OTA_Ioctl(ota_handle, IOT_OTAG_VERSION, (void *)&version, 1);

    if (version == NULL) {
        res = FAIL_RETURN;
        goto ERROR;
    }

    message_len = strlen(fota_new_config_fmt) + strlen(version) + 1;
    
    message = DM_malloc(message_len);
    if (message == NULL) {
        dm_log_err(DM_UTILS_LOG_MEMORY_NOT_ENOUGH);
        res = FAIL_RETURN;
        goto ERROR;
    }
    memset(message,0,message_len);
    HAL_Snprintf(message,message_len,fota_new_config_fmt,version);

    dm_log_info("Send To User: %s",message);

    res = _dm_msg_send_to_user(IOTX_DM_EVENT_FOTA_NEW_FIRMWARE,message);
    if (res != SUCCESS_RETURN) {
        if (message) {DM_free(message);}
        res = FAIL_RETURN;
        goto ERROR;
    }

    res = SUCCESS_RETURN;
ERROR:
    if (version) {free(version);}

    return res;
}
#endif

int dm_fota_perform_sync(_OU_ char *output, _IN_ int output_len)
{
#ifdef OTA_ENABLED
    int res = 0, file_download = 0;
    uint32_t file_size = 0, file_downloaded = 0;
    uint32_t percent_pre = 0, percent_now = 0;
    unsigned long long report_pre = 0, report_now = 0;
    dm_fota_ctx_t *ctx = _dm_fota_get_ctx();
    void *ota_handle = NULL;
    uint32_t ota_type = IOT_OTAT_NONE;

    if (output == NULL || output_len <= 0) {
        dm_log_err(DM_UTILS_LOG_INVALID_PARAMETER);
		return FAIL_RETURN;
    }

    /* Get Ota Handle */
    res = dm_ota_get_ota_handle(&ota_handle);
    if (res != SUCCESS_RETURN) {
        return FAIL_RETURN;
    }

    if (ota_handle == NULL) {return FAIL_RETURN;}
    IOT_OTA_Ioctl(ota_handle,IOT_OTAG_OTA_TYPE,&ota_type,4);

    if (ota_type != IOT_OTAT_FOTA) {return FAIL_RETURN;}

    /* Prepare Write Data To Storage */
    HAL_Firmware_Persistence_Start();
    ctx->is_report_new_config = 0;

    while (1) {
        file_download = IOT_OTA_FetchYield(ota_handle,output,output_len,1);
        if (file_download < 0) {
            dm_log_err(DM_UTILS_LOG_OTA_FETCH_FAILED);
            IOT_OTA_ReportProgress(ota_handle, IOT_OTAP_FETCH_FAILED, NULL);
            HAL_Firmware_Persistence_Stop();
            return FAIL_RETURN;
        }

        /* Write Config File Into Stroage */
        HAL_Firmware_Persistence_Write(output,file_download);

        /* Get OTA information */
        IOT_OTA_Ioctl(ota_handle, IOT_OTAG_FETCHED_SIZE, &file_downloaded, 4);
        IOT_OTA_Ioctl(ota_handle, IOT_OTAG_FILE_SIZE, &file_size, 4);

        /* Calculate Download Percent And Update Report Timestamp*/
        percent_now = (file_downloaded * 100) / file_size;
        report_now = HAL_UptimeMs();

        /* Report Download Process To Cloud */
        if (report_now < report_pre) {report_pre = report_now;}
        if ((((percent_now - percent_pre) > 5) && 
            ((report_now - report_pre) > 50)) || (percent_now >= IOT_OTAP_FETCH_PERCENTAGE_MAX)) {
                IOT_OTA_ReportProgress(ota_handle, percent_now, NULL);
                percent_pre = percent_now;
                report_pre = report_now;
            }

        /* Check If OTA Finished */
        if (IOT_OTA_IsFetchFinish(ota_handle)) {
            uint32_t file_isvalid = 0;
            IOT_OTA_Ioctl(ota_handle, IOT_OTAG_CHECK_FIRMWARE, &file_isvalid, 4);
            if (file_isvalid == 0) {
                HAL_Firmware_Persistence_Stop();
                return FAIL_RETURN;
            }else{
                break;
            }
        }
    }

    HAL_Firmware_Persistence_Stop();
#endif
    return SUCCESS_RETURN;
}

int dm_fota_status_check(void)
{
#ifdef OTA_ENABLED
    int res = 0;
    dm_fota_ctx_t *ctx = _dm_fota_get_ctx();
    void *ota_handle = NULL;

    /* Get Ota Handle */
    res = dm_ota_get_ota_handle(&ota_handle);
    if (res != SUCCESS_RETURN) {
        return FAIL_RETURN;
    }

    if (IOT_OTA_IsFetching(ota_handle)) {
        uint32_t ota_type = IOT_OTAT_NONE;

        IOT_OTA_Ioctl(ota_handle,IOT_OTAG_OTA_TYPE,&ota_type,4);

        if (ota_type == IOT_OTAT_FOTA) {
            /* Send New Config Information To User */
            if (ctx->is_report_new_config == 0) {
                dm_log_debug("Cota Status Check");
                res = _dm_fota_send_new_config_to_user(ota_handle);
                if (res == SUCCESS_RETURN) {ctx->is_report_new_config = 1;}
            }
        }
    }
#endif
    return SUCCESS_RETURN;
}