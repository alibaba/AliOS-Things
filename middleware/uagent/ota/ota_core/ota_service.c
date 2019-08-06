/*
 *Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <string.h>
#include <cJSON.h>
#include "ota_log.h"
#include "ota_crypto.h"
#include "ota_import.h"
#include "ota_plat_ctrl.h"
#include "ota_hal_common.h"
#include "aos/kernel.h"
#include "aos/hal/wdg.h"
#include "uagent/uagent.h"

#if defined (OTA_CONFIG_UAGENT) && defined (OTA_CONFIG_SECURE_DL_MODE)
#error("not support ota via uagent under security mode temporary, disable 'OTA via uAgent' or discard this warning by remove this statement")
#endif
#ifdef AOS_COMP_PWRMGMT
#include "aos/pwrmgmt.h"
#endif

#ifdef OTA_CONFIG_UAGENT
static int ota_inform(void);
#endif /* OTA_CONFIG_UAGENT */

#if defined OTA_CONFIG_SECURE_DL_MODE
static wdg_dev_t ota_wdg = {0};
static aos_timer_t  ota_mon_tmr = {0};
static void ota_monitor_task(void *arg1, void* arg2)
{
    hal_wdg_reload(&ota_wdg);
}
#endif

static ota_boot_param_t *ota_param = NULL;
static ota_service_t    *ota_ctx   = NULL;

ota_service_t * ota_get_service_manager(void) {
    return ota_ctx;
}

/**
 * ota_parse_host_url  OTA parse host url
 *
 * @param[in] char *url           pasre url to host name & uri.
 *
 * @return host_name              host name from download url.
 * @return host_uri               host uri from download url.
 */
void ota_parse_host_url(char *url, char **host_name, char **host_uri)
{
    char *pa;
    char *pb;
    if ((url == NULL) || (strlen(url) == 0)) {
        OTA_LOG_E("url parms error!");
        return;
    }
    if (!(*url)) {
        return;
    }
    pa = url;
    if (!strncmp(pa, "https://", 8)) {
        pa = url + 8;
    }
    if (!strncmp(pa, "http://", 7)) {
        pa = url + 7;
    }
    *host_name = url;
    pb   = strchr(pa, '/');
    if (pb) {
        *pb = 0;
        pb += 1;
        if (*pb) {
            *host_uri                   = pb;
            *((*host_uri) + strlen(pb)) = 0;
        }
    } else {
        (*host_name)[strlen(pa)] = 0;
    }
}

/**
 * ota_service_start  OTA service start: download and upgrade
 *
 * @param[in] void
 *
 * @return void
 */
int ota_service_start(ota_service_t *ctx)
{
    int ret = 0;
#ifdef AOS_COMP_PWRMGMT
    aos_pwrmgmt_lowpower_suspend(PWRMGMT_OTA);
#endif
    ota_ctx = ctx;
#if defined OTA_CONFIG_SECURE_DL_MODE
    ota_wdg.config.timeout = 180000;
    ota_boot_param_t temp  = {0};
    ota_boot_param_t *param = &temp;
    hal_wdg_init(&ota_wdg);
    aos_timer_new(&ota_mon_tmr, ota_monitor_task, NULL, 3000, 1);
    if (ota_is_download_mode() == 0) {
        return OTA_INIT_FAIL;
    }
    ota_read_parameter(param);
#else
    ota_boot_param_t * param = ota_param;
#endif

    if (param == NULL) {
        return OTA_INIT_FAIL;
    }
    /* OTA download init */
    ret = ota_download_init();
    if (ret < 0) {
        goto EXIT;
    }
    /* init ota partition */
    ret = ota_int(param);
    if (ret < 0) {
        ret = OTA_INIT_FAIL;
        goto EXIT;
    }
    /* download start */
    ret = ota_download_start(param->url);
    if (ret < 0) {
        ota_param->upg_status = OTA_BREAKPOINT;
        goto EXIT;
    }
    /* verify RSA signature */
#if defined OTA_CONFIG_RSA
    ret = ota_verify_download_rsa_sign((unsigned char*)param->sign, (const char*)param->hash, param->hash_type);
    if (ret < 0) {
        ret = OTA_VERIFY_RSA_FAIL;
        goto EXIT;
    }
#endif
    /* verify image */
    if ((param->upg_flag != OTA_UPGRADE_DIFF) && (param->upg_flag != OTA_UPGRADE_CUST)) {
        ret = ota_check_image(param->len);
        if (ret < 0) {
            ret = OTA_VERIFY_IMAGE_FAIL;
            goto EXIT;
        }
    }
EXIT:
    OTA_LOG_E("upgrade complete ret:%d.\n", ret);
    if (ret < 0) {
        param->upg_status = ret;
#if !defined OTA_CONFIG_SECURE_DL_MODE
        if (ctx != NULL) {
#ifdef OTA_CONFIG_UAGENT
            if (OTA_PROCESS_UAGENT_OTA == ctx->ota_process) {
                ota_update_process("general reason", -1);
            } else
#endif /* OTA_CONFIG_UAGENT */
            {
                ota_transport_status(ctx->pk, ctx->dn, ret);
            }
        }
#endif /* !defined OTA_CONFIG_SECURE_DL_MODE */
        ret = ota_clear(param);
    } else {
        param->upg_status = OTA_FINISH;
    }
#if defined OTA_CONFIG_SECURE_DL_MODE
    hal_wdg_finalize(&ota_wdg);
#endif
#ifdef AOS_COMP_PWRMGMT
    aos_pwrmgmt_lowpower_resume(PWRMGMT_OTA);
#endif
    if ((ctx != NULL) && (ctx->on_boot != NULL)) {
        ctx->on_boot(param);
    } else {
        ota_set_boot(param);
    }
    return ret;
}

/**
 * ota_parse_dl_url  OTA parse download url from transport message
 *
 * @param[in] char *json          transport message from Cloud.
 *
 * @return OTA_TRANSPORT_FAIL     transport errno.
 */
void ota_parse_dl_url(const char *json)
{
    int ret            = OTA_PAR_SUCCESS;
    cJSON *root        = NULL;
    if(ota_param == NULL) {
        ret = OTA_INIT_FAIL;
        goto EXIT;
    }
    memset(ota_param->url, 0, sizeof(ota_param->url));
    memset(ota_param->hash, 0, sizeof(ota_param->hash));
    memset(ota_param->sign, 0, sizeof(ota_param->sign));
    memset(ota_param->ver, 0, sizeof(ota_param->ver));
    root = cJSON_Parse(json);
    if (NULL == root) {
        ret = OTA_TRANSPORT_PAR_FAIL;
        goto EXIT;
    } else {
        /* recover the process type */
        cJSON *cmd = cJSON_GetObjectItem(root, "cmd");

        if (NULL == cmd) {
            ota_ctx->ota_process = OTA_PROCESS_NORMAL;
        }

        cJSON *message = cJSON_GetObjectItem(root, "message");
        if (NULL == message) {
            ret = OTA_MESSAGE_PAR_FAIL;
            goto EXIT;
        }
        if ((strncmp(message->valuestring, "success", strlen("success")))) {
            ret = OTA_SUCCESS_PAR_FAIL;
            goto EXIT;
        }
        cJSON *json_obj = cJSON_GetObjectItem(root, "data");
        if (NULL == json_obj) {
            ret = OTA_DATA_PAR_FAIL;
            goto EXIT;
        }
        cJSON *url = cJSON_GetObjectItem(json_obj, "url");
        if (NULL == url) {
            ret = OTA_URL_PAR_FAIL;
            goto EXIT;
        }
        cJSON *version = cJSON_GetObjectItem(json_obj, "version");
        if (NULL == version) {
            ret = OTA_VER_PAR_FAIL;
            goto EXIT;
        } else {
            if (OTA_PROCESS_UAGENT_OTA == ota_ctx->ota_process) {
                /* suppose version parser fail */
                ret = OTA_TRANSPORT_VER_FAIL;
                if (NULL != version && cJSON_IsArray(version) && cJSON_IsArray(version)) {
                    cJSON *p = version->child;

                    while (NULL != p && cJSON_IsObject(p)) {
                        cJSON *type = cJSON_GetObjectItem(p, "type");
                        cJSON *version = cJSON_GetObjectItem(p, "version");
                        if (NULL != type && cJSON_IsNumber(type) && NULL != version && cJSON_IsString(version)) {
                            OTA_LOG_I("version type %d version %s", type->valueint, version->valuestring);
                            if (OTA_UPGRADE_SOC == type->valueint) {
                                int is_ota = strncmp(version->valuestring, ota_version_get(ota_ctx->dev_type, ota_ctx->dn), strlen(version->valuestring));
                                if (is_ota > 0) {
                                    ret = 0;
                                }
                            }
                        } else {
                            OTA_LOG_W("version missing param, try next item");
                        }
                        p = p->next;
                    }
                }

                if (OTA_TRANSPORT_VER_FAIL == ret) { /* still keep fail */
                    goto EXIT;
                }
            } else {
                int is_ota = strncmp(version->valuestring, ota_version_get(ota_ctx->dev_type, ota_ctx->dn), strlen(version->valuestring));
                if (is_ota <= 0) {
                    ret = OTA_TRANSPORT_VER_FAIL;
                    goto EXIT;
                }
            }
        }

        strncpy(ota_param->url, url->valuestring, OTA_URL_LEN - 1);
        cJSON *signMethod = cJSON_GetObjectItem(json_obj, "signMethod");
        if (signMethod != NULL) {
            memset(ota_param->hash, 0x00, OTA_HASH_LEN);
            ret = ota_to_capital(signMethod->valuestring, strlen(signMethod->valuestring));
            if (ret != 0) {
                ret = OTA_VER_PAR_FAIL;
                goto EXIT;
            }
            if (0 == strncmp(signMethod->valuestring, "MD5", strlen("MD5"))) {
                cJSON *md5 = cJSON_GetObjectItem(json_obj, "sign");
                if (NULL == md5) {
                    ret = OTA_MD5_PAR_FAIL;
                    goto EXIT;
                }
                ota_param->hash_type = OTA_MD5;
                strncpy(ota_param->hash, md5->valuestring, strlen(md5->valuestring) + 1);
                ota_param->hash[strlen(md5->valuestring)] = '\0';
                ret = ota_to_capital(ota_param->hash, strlen(ota_param->hash));
                if (ret != 0) {
                    ret = OTA_VER_PAR_FAIL;
                    goto EXIT;
                }
            } else if (0 == strncmp(signMethod->valuestring, "SHA256", strlen("SHA256"))) {
                cJSON *sha256 = cJSON_GetObjectItem(json_obj, "sign");
                if (NULL == sha256) {
                    ret = OTA_SHA256_PAR_FAIL;
                    goto EXIT;
                }
                ota_param->hash_type = OTA_SHA256;
                strncpy(ota_param->hash, sha256->valuestring, strlen(sha256->valuestring) + 1);
                ota_param->hash[strlen(sha256->valuestring)] = '\0';
                ret = ota_to_capital(ota_param->hash, strlen(ota_param->hash));
                if (ret != 0) {
                    ret = OTA_VER_PAR_FAIL;
                    goto EXIT;
                }
            } else {
                ret = OTA_HASH_PAR_FAIL;
                goto EXIT;
            }
        } else { /* old protocol*/
            memset(ota_param->hash, 0x00, OTA_HASH_LEN);
            cJSON *md5 = cJSON_GetObjectItem(json_obj, "md5");
            if (NULL == md5) {
                ret = OTA_MD5_PAR_FAIL;
                goto EXIT;
            }
            ota_param->hash_type = OTA_MD5;
            strncpy(ota_param->hash, md5->valuestring, strlen(md5->valuestring) + 1);
            ota_param->hash[strlen(md5->valuestring)] = '\0';
            ret = ota_to_capital(ota_param->hash, strlen(ota_param->hash));
            if (ret != 0) {
                ret = OTA_VER_PAR_FAIL;
                goto EXIT;
            }
        }
        cJSON *size = cJSON_GetObjectItem(json_obj, "size");
        if (NULL == size) {
            ret = OTA_SIZE_PAR_FAIL;
            goto EXIT;
        }
        ota_param->len = size->valueint;
        cJSON *digestSign = cJSON_GetObjectItem(json_obj, "digestsign");
        if (digestSign != NULL) {
            unsigned int sign_len = OTA_SIGN_LEN;
            memset(ota_param->sign, 0x00, OTA_SIGN_LEN);
            if (ota_base64_decode((const unsigned char *)digestSign->valuestring, strlen(digestSign->valuestring), (unsigned char*)ota_param->sign, &sign_len) != 0) {
                ret = OTA_SIGN_PAR_FAIL;
                goto EXIT;
            }
        }
    }

EXIT:
    if (root != NULL) {
        cJSON_Delete(root);
        root = NULL;
    }
    OTA_LOG_E("Parse Json ret:%d url:%s\n", ret, ota_param->url);
    if (ret == OTA_TRANSPORT_VER_FAIL) {
        OTA_LOG_E("ota version is too old, discard it.");
#ifdef OTA_CONFIG_UAGENT
        if (OTA_PROCESS_UAGENT_OTA == ota_ctx->ota_process) {
            ota_update_process("OTA transport verion is too old", -1);
        } else
#endif /* OTA_CONFIG_UAGENT */
        {
            ota_transport_status(ota_ctx->pk, ota_ctx->dn, ret);
        }
    } else if (ret < 0) {
        ret = OTA_TRANSPORT_PAR_FAIL;
#ifdef OTA_CONFIG_UAGENT
        if (OTA_PROCESS_UAGENT_OTA == ota_ctx->ota_process) {
            ota_update_process("ota upgrade parse failed", -1);
        } else
#endif /* OTA_CONFIG_UAGENT */
        {
            ota_transport_status(ota_ctx->pk, ota_ctx->dn, ret);
        }
    } else {
#if defined CSP_LINUXHOST
        void *thread = NULL;
        ret = ota_thread_create(&thread, (void *)ota_service_start, (void *)ota_ctx, NULL, 1024 * 6);
        if (ret < 0) {
            OTA_LOG_E("ota thread create err;%d ", ret);
        }
#elif defined OTA_CONFIG_SECURE_DL_MODE
        OTA_LOG_E("OTA secure mode.\n");
        ota_param->upg_status = OTA_TRANSPORT;
        ret = ota_update_parameter(ota_param);
        if (ret != 0) {
            ota_transport_status(ota_ctx->pk, ota_ctx->dn, OTA_TRANSPORT_PAR_FAIL);
        }
        ota_transport_status(ota_ctx->pk, ota_ctx->dn, 1);
        ota_reboot();
#else
        {
            OTA_LOG_E("OTA thread start.\n");
            void *thread = NULL;
            ret = ota_thread_create(&thread, (void *)ota_service_start, (void *)ota_ctx, NULL, 1024 * 4);
            if (ret < 0) {
                OTA_LOG_E("ota thread create err;%d ", ret);
            }
        }
#endif
    }
}

#ifdef OTA_CONFIG_UAGENT
static int on_ota_handler(void *p, const unsigned short len, void *str)
{
    int rc = -1;

    if (NULL != str && 0 != len) {
        cJSON *root = NULL;

        OTA_LOG_I("Handle ota payload %s", str);
        root = cJSON_Parse(str);
        if (NULL != root) {
            cJSON *cmd = cJSON_GetObjectItem(root, "cmd");

            if (NULL != cmd) {
                if (cJSON_IsNumber(cmd)) {
                    if (OTA_REQ_VERSION == cmd->valueint) {
                        rc = 0;
                        if (0 != ota_inform()) {
                            OTA_LOG_E("ota version report via uagent fail");
                        }
                    } else if (0 != (OTA_UPGRADE_SOC&cmd->valueint)) {
                        rc = 0;
                        ota_ctx->ota_process = OTA_PROCESS_UAGENT_OTA;
                        ota_parse_dl_url(str);
                    } else {
                        OTA_LOG_W("unsupport command %d", cmd->valueint);
                    }
                }
            }
            cJSON_Delete(root);
        }
        if (0 != rc) {
            ota_update_process("upgrade command not support", -1);
        }
    }
    return rc;
}

static int ota_inform(void)
{
    char info_msg[OTA_MSG_LEN] = { 0 };

    const int len = ota_snprintf(info_msg, sizeof(info_msg), "{\"version\":[ {\"type\":%d, \"version\":\"%s\"}]}",
        OTA_UPGRADE_SOC, ota_version_get(ota_ctx->dev_type, ota_ctx->dn));
    OTA_LOG_I("version report %s", ota_version_get(ota_ctx->dev_type, ota_ctx->dn));
    return uagent_send(UAGENT_MOD_OTA, VERSION_INFORM, len < OTA_MSG_LEN ? len : OTA_MSG_LEN,
        info_msg, send_policy_object);
}

int ota_update_process(const char *error_description, const int step)
{
    int rc = -1;
    char msg[OTA_MSG_LEN] = { 0 };

    if (NULL != error_description) {
        rc = ota_snprintf(msg, sizeof(msg), "{\"step\": %d,\"desc\":\"%s\"}", step, error_description);
    } else {
        rc = ota_snprintf(msg, sizeof(msg), "{\"step\": %d,\"desc\":\"%s\"}", step, "");
    }
    if (rc > 0) {
        rc = uagent_send(UAGENT_MOD_OTA, OTA_INFO, rc < OTA_MSG_LEN ? rc : OTA_MSG_LEN, msg, send_policy_object);
    }
    return rc;
}

static uagent_func_node_t ota_uagent_funclist[] =
{
    {OTA_INFO,        "otainfo", NULL,           NULL, &ota_uagent_funclist[1]},
    {VERSION_INFORM,  "inform",  NULL,           NULL, &ota_uagent_funclist[2]},
    {OTA_UPGRADE_CMD, "otacmd",  on_ota_handler, NULL, NULL}
};
static mod_func_t ulog_uagent_func =
{
    { UAGENT_MOD_OTA, 3, "OTA", OTA_VERSION },
    ota_uagent_funclist,
};

#endif /* OTA_CONFIG_UAGENT */

/**
 * ota_service_init  OTA service init
 *
 * @param[in] ota_service_t *ctx OTA service context
 *
 * @return OTA_SUCCESS         OTA service init success.
 * @return OTA_INIT_FAIL       OTA service init fail.
 */
int ota_service_init(ota_service_t *ctx)
{
    int ret = 0;
    char *version = NULL;
    if (NULL == ctx) {
        ret = OTA_INIT_FAIL;
        return ret;
    }
    ota_ctx = ctx;
    if (ota_ctx->inited > 0) {
        OTA_LOG_E("Reconnected, Redo OTA.");
        ret = ota_transport_upgrade(ota_ctx->pk, ota_ctx->dn);
        if (ret < 0) {
            OTA_LOG_E("Failed to do OTA.");
            ret = OTA_INIT_FAIL;
            return ret;
        }
    }
    ota_ctx->inited = 1;
    ota_param = ota_malloc(sizeof(ota_boot_param_t));
    if (ota_param == NULL) {
        ret = OTA_INIT_FAIL;
        return ret;
    }
    memset(ota_param, 0, sizeof(ota_boot_param_t));
    /* transport init */
    ret = ota_transport_init();
    if (ret < 0) {
        goto EXIT;
    }
    /* inform version to cloud */
    version = (char *)ota_version_get(ota_ctx->dev_type, ota_ctx->dn);
    if (version == NULL) {
        ret = OTA_INIT_FAIL;
        goto EXIT;
    }

    ota_ctx->ota_process = OTA_PROCESS_NORMAL;

#ifdef OTA_CONFIG_UAGENT
    OTA_LOG_I("register ota cb into uagent");
    uagent_func_node_t *p = ulog_uagent_func.header;
    while (NULL != p) {
        if (0 != uagent_register(ulog_uagent_func.mod_info.mod, ulog_uagent_func.mod_info.name,
            ulog_uagent_func.mod_info.version, p->func, p->func_name,
            p->service, p->argu)) {
            OTA_LOG_E("register into uagent fail");
        }
        p = p->next;
    }

    if (0 != ota_inform()) {
        OTA_LOG_E("uagent inform fail");
    }
#endif /* OTA_CONFIG_UAGENT */
    ret = ota_transport_inform(ota_ctx->pk, ota_ctx->dn, version);
    if (ret < 0) {
        goto EXIT;
    }
    /* subcribe uprade */
    ret = ota_transport_upgrade(ota_ctx->pk, ota_ctx->dn);
    if (ret < 0) {
        goto EXIT;
    }
    /* rollback set */
    ret = ota_rollback();
    if (ret < 0) {
        ret = OTA_INIT_FAIL;
        goto EXIT;
    }
    OTA_LOG_E("ota init success, ret:%d", ret);
    return ret;
EXIT:
    if (ota_param != NULL) {
        ota_free(ota_param);
        ota_param = NULL;
    }
    OTA_LOG_E("ota init fail, ret:%d", ret);
    ota_ctx->inited = 0;
    return ret;
}

/**
 * ota_service_deinit  OTA service deinit
 *
 * @param[in] ota_service_t *ctx OTA service context
 *
 * @return OTA_SUCCESS         OTA service deinit success.
 * @return OTA_INIT_FAIL       OTA service deinit fail.
 */
int ota_service_deinit(ota_service_t *ctx)
{
    int ret = 0;
    if (ota_param != NULL) {
        ota_free(ota_param);
        ota_param = NULL;
    }
    if (NULL == ctx) {
        ret = OTA_INIT_FAIL;
        return ret;
    }
    ctx->inited = 0;
    return ret;
}
