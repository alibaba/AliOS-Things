/*
 *Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */
#include <string.h>
#include "ota_service.h"
#include "ota_transport.h"
#include "ota_hal_os.h"
#include "ota_hal_plat.h"
#include "ota_manifest.h"
#include "ota_version.h"
#include "ota_util.h"
#include "ota_log.h"


static ota_service_manager ctx = { 0 };
static ota_write_cb_t      ota_write_callback;
static ota_read_cb_t       ota_read_callback;
static ota_finish_cb_t     ota_finish_callbak;
extern hal_ota_module_t    ota_hal_module;

static void ota_set_callbacks(ota_write_cb_t write_cb, ota_read_cb_t read_cb,
                              ota_finish_cb_t finish_cb)
{
    ota_write_callback = write_cb;
    ota_read_callback  = read_cb;
    ota_finish_callbak = finish_cb;
}

static int ota_hal_write_cb(int32_t writed_size, uint8_t *buf, int32_t buf_len,
                            int type)
{
    return hal_ota_write(hal_ota_get_default_module(), NULL, buf, buf_len);
}

static int ota_hal_read_cb(volatile uint32_t *off_set, uint8_t *buf,
                           int32_t buf_len, int type)
{
    return hal_ota_read(hal_ota_get_default_module(), off_set, buf, buf_len);
}

static int ota_hal_finish_cb(OTA_ENUM_RESULT_TYPE finished_result,
                             void                *updated_type)
{
    ota_finish_param_t finsh_para;
    finsh_para.update_type   = *(OTA_ENUM_UPDATE_TYPE *)updated_type;
    finsh_para.result_type   = finished_result;
    finsh_para.firmware_type = ota_get_firmware_type();
    finsh_para.splict_size   = ota_get_splict_size();
    finsh_para.diff_version  = ota_get_diff_version();
    OTA_LOG_D("ota_hal_finish_cb:%d %d %d\n", finsh_para.update_type,
              finsh_para.result_type, finsh_para.firmware_type);
    return hal_ota_set_boot(hal_ota_get_default_module(), (void *)&finsh_para);
}

/*
const char *ota_info = "{\"md5\":\"6B21342306D0F619AF97006B7025D18A\","
        "\"resourceUrl\":\"http:\/\/otalink.alicdn.com\/ALINKTEST_LIVING_LIGHT_ALINK_TEST\/v2.0.0.1\/uthash-master.zip\","
        "\"size\": \"265694 \",\"uuid\":
\"5B7CFD5C6B1D6A231F5FB6B7DB2B71FD\",\"version\": \"v2.0.0.1\",\"zip\": \"0\"}";
*/
// const char *ota_info = "{\"uuid\": \"5B7CFD5C6B1D6A231F5FB6B7DB2B71FD\"}";

static void update_action(void *buf)
{
    if (!buf) {
        OTA_LOG_E("do update buf is null");
        return;
    }

    ota_response_params response_parmas;
    ota_request_params  request_parmas;
    memset((void *)&response_parmas, 0, sizeof(response_parmas));
    memset((void *)&request_parmas, 0, sizeof(request_parmas));
#ifdef OTA_MULTI_BINS
    request_parmas.primary_version   = ota_get_os_version();
    request_parmas.secondary_version = ota_get_app_version();
#else
    request_parmas.primary_version   = ota_get_app_version();
    request_parmas.secondary_version = "\0";
#endif
    OTA_LOG_D("update action pk:%s dn:%s ver:%s", ctx.pk, ctx.dn,
              ota_get_app_version());
    ota_transport *trans =
      (ota_transport *)ota_get_transport(ctx.trans_protcol);
    request_parmas.device_uuid = trans->get_uuid();
    ota_set_callbacks(ota_hal_write_cb, ota_hal_read_cb, ota_hal_finish_cb);
    if (0 == trans->parse_response((char *)buf, strlen((char *)buf),
                                   &response_parmas)) {
        ota_do_update_packet(&response_parmas, &request_parmas,
                             ota_write_callback, ota_read_callback,
                             ota_finish_callbak);
    }
}

static void do_update(int len, const char *buf)
{
    ota_set_resp_msg(buf);
    update_action((void *)ota_get_resp_msg());
}

static void cancel_update(int len, const char *buf)
{
    OTA_LOG_D("begin cancel update %s\n", (char *)buf);
    if (!buf) {
        OTA_LOG_E("cancel update buf is null");
        return;
    }

    ota_transport *trans =
      (ota_transport *)ota_get_transport(ctx.trans_protcol);
    ota_response_params response_parmas;
    trans->parse_cancel_response(buf, strlen((char *)buf), &response_parmas);
    ota_cancel_update_packet(&response_parmas);
}

static void ota_regist_upgrade(void)
{
    ota_transport *trans =
      (ota_transport *)ota_get_transport(ctx.trans_protcol);
    trans->subscribe_upgrade(&do_update);
    ota_post_version_msg();
    trans->cancel_upgrade(&cancel_update);
}



int ota_service_init(ota_service_manager *context)
{
#ifdef OTA_HAL_MODULE
    hal_ota_register_module(&ota_hal_module);
#endif
    memset((char *)&ctx, 0, sizeof(ota_service_manager));
    if (!context) {
        OTA_LOG_I("OTA ctx is NULL, to use def.");
        ctx.trans_protcol = OTA_PROTCOL_MQTT;
        ctx.dl_protcol    = OTA_PROTCOL_HTTPS;
        ota_HAL_GetProductKey(ctx.pk);
        ota_HAL_GetDeviceName(ctx.dn);
        ota_HAL_GetDeviceSecret(ctx.ds);
        ota_HAL_GetProductSecret(ctx.ps);
    } else {
        memcpy((char *)&ctx, context, sizeof(ota_service_manager));
    }
    if (ctx.inited) {
        OTA_LOG_I("OTA is inited.");
        return -1;
    }
    ota_status_init();
    ctx.inited = 1;
    ota_transport *trans =
      (ota_transport *)ota_get_transport(ctx.trans_protcol);
    trans->init();
    ota_regist_upgrade();
    return 0;
}

int ota_service_deinit(void)
{
    ota_transport *trans =
      (ota_transport *)ota_get_transport(ctx.trans_protcol);
    trans->deinit();
    if (ctx.dl_url) {
        ota_free(ctx.dl_url);
        ctx.dl_url = NULL;
        OTA_LOG_I("free url");
    }
    if (ctx.h_coap) {
        ota_free(ctx.h_coap);
        ctx.h_coap = NULL;
        OTA_LOG_I("free h_coap");
    }
    ota_status_deinit();
    return 0;
}

const void *ota_get_transport(int protcol)
{
    if (OTA_PROTCOL_COAP == protcol) {
        return ota_get_transport_coap();
    } else {
        return ota_get_transport_mqtt();
    }
}

const void *get_ota_service_manager(void)
{
    return &ctx;
}
