/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <http.h>
#include "ota_log.h"
#include "ota_crypto.h"
#include "ota_import.h"
#include "ota_plat_ctrl.h"
#include "ota_hal_common.h"
#include <network/network.h>
#include "ota/ota_service.h"

static int ota_upgrading = 0;
static unsigned char dl_buf[OTA_DOWNLOAD_BLOCK_SIZE] = {0};
#if !defined BOARD_ESP8266 && defined OTA_CONFIG_SECURE_DL_MODE
static const char *ca_cert = \
{   \
    "-----BEGIN CERTIFICATE-----\r\n"
    "MIIDdTCCAl2gAwIBAgILBAAAAAABFUtaw5QwDQYJKoZIhvcNAQEFBQAwVzELMAkG\r\n" \
    "A1UEBhMCQkUxGTAXBgNVBAoTEEdsb2JhbFNpZ24gbnYtc2ExEDAOBgNVBAsTB1Jv\r\n" \
    "b3QgQ0ExGzAZBgNVBAMTEkdsb2JhbFNpZ24gUm9vdCBDQTAeFw05ODA5MDExMjAw\r\n" \
    "MDBaFw0yODAxMjgxMjAwMDBaMFcxCzAJBgNVBAYTAkJFMRkwFwYDVQQKExBHbG9i\r\n" \
    "YWxTaWduIG52LXNhMRAwDgYDVQQLEwdSb290IENBMRswGQYDVQQDExJHbG9iYWxT\r\n" \
    "aWduIFJvb3QgQ0EwggEiMA0GCSqGSIb3DQEBAQUAA4IBDwAwggEKAoIBAQDaDuaZ\r\n" \
    "jc6j40+Kfvvxi4Mla+pIH/EqsLmVEQS98GPR4mdmzxzdzxtIK+6NiY6arymAZavp\r\n" \
    "xy0Sy6scTHAHoT0KMM0VjU/43dSMUBUc71DuxC73/OlS8pF94G3VNTCOXkNz8kHp\r\n" \
    "1Wrjsok6Vjk4bwY8iGlbKk3Fp1S4bInMm/k8yuX9ifUSPJJ4ltbcdG6TRGHRjcdG\r\n" \
    "snUOhugZitVtbNV4FpWi6cgKOOvyJBNPc1STE4U6G7weNLWLBYy5d4ux2x8gkasJ\r\n" \
    "U26Qzns3dLlwR5EiUWMWea6xrkEmCMgZK9FGqkjWZCrXgzT/LCrBbBlDSgeF59N8\r\n" \
    "9iFo7+ryUp9/k5DPAgMBAAGjQjBAMA4GA1UdDwEB/wQEAwIBBjAPBgNVHRMBAf8E\r\n" \
    "BTADAQH/MB0GA1UdDgQWBBRge2YaRQ2XyolQL30EzTSo//z9SzANBgkqhkiG9w0B\r\n" \
    "AQUFAAOCAQEA1nPnfE920I2/7LqivjTFKDK1fPxsnCwrvQmeU79rXqoRSLblCKOz\r\n" \
    "yj1hTdNGCbM+w6DjY1Ub8rrvrTnhQ7k4o+YviiY776BQVvnGCv04zcQLcFGUl5gE\r\n" \
    "38NflNUVyRRBnMRddWQVDf9VMOyGj/8N7yy5Y0b2qvzfvGn9LhJIZJrglfCm7ymP\r\n" \
    "AbEVtQwdpf5pLGkkeB6zpxxxYu7KyJesF12KwvhHhm4qxFYxldBniYUr+WymXUad\r\n" \
    "DKqC5JlR3XC321Y9YeRq4VzW9v493kHMB65jUr9TU/Qr6cf9tveCX4XSQRjbgbME\r\n" \
    "HMUfpIBvFSDJ3gyICh3WZlXi/EjJKSZp4A==\r\n" \
    "-----END CERTIFICATE-----"
};
#endif

/**
 * ota_download_start  OTA download start
 *
 * @param[in] char *url  download url
 *
 * @return OTA_SUCCESS             OTA success.
 * @return OTA_DOWNLOAD_INIT_FAIL  OTA download init failed.
 * @return OTA_DOWNLOAD_CON_FAIL   OTA download connect failed.
 * @return OTA_DOWNLOAD_REQ_FAIL   OTA download request failed.
 * @return OTA_DOWNLOAD_RECV_FAIL  OTA download receive failed.
 */
int ota_download_start(char *url)
{
    char hdr[512] = {0};
    int ret = OTA_DOWNLOAD_INIT_FAIL;
    unsigned int offset = 0;
    http_rsp_info_t rsp_info = {0};
    char *content = NULL;
    int fd = 0;
    int i = 0, j = 0;
    int ota_rx_size = 0;
    int ota_file_size = 0;
    unsigned char ota_header_found = false;
    httpc_connection_t settings = {0};
    httpc_handle_t httpc_handle = 0;
    int percent                 = 0;
    int divisor                 = 10;
#if defined OTA_CONFIG_ITLS
    char pkps[128] = {0};
#endif
    ota_service_t* ctx = ota_get_service_manager();
    if(ota_upgrading == 1) {
        ret = OTA_DOWNLOAD_INIT_FAIL;
        return ret;
    }

    for(j = OTA_DOWNLOAD_RETRY_CNT; (j > 0)&&(ret < 0); j--) {
        char *host_name = NULL;
        char *host_uri = NULL;
        ota_parse_host_url((char*)url, &host_name, &host_uri);
        if (host_name == NULL || host_uri == NULL) {
            ret = OTA_DOWNLOAD_INIT_FAIL;
            return ret;
        }
        fd = socket(AF_INET, SOCK_STREAM, 0);
        if (fd < 0) {
            ret = OTA_DOWNLOAD_INIT_FAIL;
            goto EXIT;
        }
        memset(&settings, 0, sizeof(settings));
        settings.socket = fd;
        settings.server_name = host_name;
#if defined BOARD_ESP8266
        settings.flags = 0x80;
#elif defined OTA_CONFIG_ITLS
        if(ctx != NULL) {
            strncpy(pkps, ctx->pk, strlen(ctx->pk));
            strncpy(pkps + strlen(ctx->pk) + 1, ctx->ps, strlen(ctx->ps));
        }
        settings.ca_cert = pkps;
#elif defined OTA_CONFIG_SECURE_DL_MODE
       settings.ca_cert = ca_cert;
#else
       settings.flags = 0x80;
#endif
       settings.req_buf = dl_buf;
       settings.req_buf_size = OTA_DOWNLOAD_BLOCK_SIZE;
       httpc_handle = httpc_init(&settings);
       if (httpc_handle == 0) {
           ret = OTA_DOWNLOAD_INIT_FAIL;
           goto EXIT;
       }

       if (j >= OTA_DOWNLOAD_RETRY_CNT) {
           ret = httpc_construct_header(hdr, 512, "Accept", "*/*");
           if (ret < 0) {
                ret = OTA_DOWNLOAD_CON_FAIL;
                goto EXIT;
           }
       } else {
           ota_msleep(6000);
           OTA_LOG_E("reconnect retry:%d ret:%d \n", j, ret);
           char resume[64] = {0};
           ota_snprintf(resume, 64, "bytes=%d-", offset);
           ret = httpc_construct_header(hdr, 512, "Range", resume);
           if (ret < 0) {
                ret = OTA_DOWNLOAD_CON_FAIL;
                goto EXIT;
           }
       }
       memset(dl_buf, 0, OTA_DOWNLOAD_BLOCK_SIZE);
       for(i = 0; i < OTA_DOWNLOAD_RETRY_CNT; i++){
           ret = httpc_send_request(httpc_handle, HTTP_GET, host_uri, hdr, NULL, NULL, 0);
           if(ret >= 0){
                break;
           }
           else {
                ota_msleep(1500);
                OTA_LOG_E("send cnt;%d ret:%d \n",i ,ret);
           }
       }
       if (ret < 0) {
           ret = OTA_DOWNLOAD_REQ_FAIL;
           goto EXIT;
       }
       ota_upgrading = 1;
       memset(dl_buf, 0, OTA_DOWNLOAD_BLOCK_SIZE);
       while (ota_file_size == 0 || ota_rx_size < ota_file_size) {
           for(i = 0; i < OTA_DOWNLOAD_RETRY_CNT; i++){
                ret = httpc_recv_response(httpc_handle, dl_buf, OTA_DOWNLOAD_BLOCK_SIZE, &rsp_info, OTA_DOWNLOAD_TIMEOUT);
                if ((ret == HTTP_ERECV) || (ret == HTTP_ETIMEOUT)) {
                    ota_msleep(1500);
                    OTA_LOG_E("recv retry:%d ret:%d \n", i, ret);
                    continue;
                } else {
                    break;
                }
           }
           if (ota_upgrading == 0) {
                OTA_LOG_E("download stop.\n");
                ret = OTA_DOWNLOAD_RECV_FAIL;
                break;
           } else if (ret < 0) {
                ret = OTA_DOWNLOAD_RECV_FAIL;
                break;
           } else {
                if (rsp_info.body_present || rsp_info.message_complete) {
                    if (ota_header_found == false) {
                        if (ota_file_size == 0) {
                             content = strstr((const char *)dl_buf, "Content-Length");
                             if (content) {
                                 ret = sscanf(content, "%*[^ ]%d", &ota_file_size);
                                 if (ret < 0) {
                                     OTA_LOG_I("header fail\n");
                                     break;
                                 }
                                 ota_header_found = true;
                                 OTA_LOG_I("header file size %d\r\n", ota_file_size);
                             } else {
                                 continue;
                             }
                         }
                         content = strstr((const char *)dl_buf, "\r\n\r\n");
                         if (content) {
                             content += 4;
                             ota_rx_size = rsp_info.rsp_len - ((unsigned char *)content - dl_buf);
                             if((ctx != NULL)&&(ctx->on_data != NULL)) {
                                 ret = ctx->on_data(content, ota_rx_size);
                             } else {
                                 ret = ota_write(&offset, content, ota_rx_size);
                             }
                             if (ret < 0) {
                                 ret = OTA_UPGRADE_WRITE_FAIL;
                                 goto EXIT;
                             }
                         }
                    } else {
                         if((ctx != NULL)&&(ctx->on_data != NULL)) {
                             ret = ctx->on_data((char *)dl_buf, rsp_info.rsp_len);
                         } else {
                             ret = ota_write(&offset, (char *)dl_buf, rsp_info.rsp_len);
                         }
                         if (ret < 0) {
                             ret = OTA_UPGRADE_WRITE_FAIL;
                             goto EXIT;
                         }
                         ota_rx_size += rsp_info.rsp_len;
                    }
                    //OTA_LOG_E("ota (%d/%d) recv len:%d \r\n", ota_rx_size, ota_file_size, rsp_info.rsp_len);
                    if(ota_file_size) {
                         percent = (ota_rx_size * 100) /ota_file_size;
                         if(percent / divisor) {
                             divisor += 10;
                             if((ctx != NULL)&&(ctx->on_percent != NULL)) {
                                 ctx->on_percent(percent);
                             } else {
#if !defined BOARD_ESP8266 && !defined OTA_CONFIG_SECURE_DL_MODE
                                 if (ctx != NULL) {
#ifdef OTA_CONFIG_UAGENT
                                     OTA_LOG_I("download process %d", ctx->ota_process);
                                     if (OTA_PROCESS_UAGENT_OTA == ctx->ota_process) {
                                         ota_update_process(NULL, percent);
                                     } else
#endif /* OTA_CONFIG_UAGENT */
                                     {
                                         ota_transport_status(ctx->pk, ctx->dn, percent);
                                     }
                                }else{
                                    OTA_LOG_W("download ctx NULL");
                                }
#endif /* !defined BOARD_ESP8266 && !defined OTA_CONFIG_SECURE_DL_MODE */
                             }
                             OTA_LOG_I("ota recv data(%d/%d)\r\n", ota_rx_size, ota_file_size);
                         }
                     }
                 }
             }
         }

EXIT:
         ota_upgrading = 0;
         if(settings.socket > 0) {
             close(settings.socket);
             settings.socket = 0;
         }
         if(httpc_handle > 0) {
             httpc_deinit(httpc_handle);
             httpc_handle = 0;
         }
         httpc_handle = 0;
         ota_header_found = false;
         ota_file_size = 0;
         ota_rx_size = 0;
    }
    OTA_LOG_E("download complete:%d \n", ret);
    return ret;
}

/**
 * ota_download_stop  OTA download stop
 *
 * @param[in] void
 *
 * @return OTA_SUCCESS             OTA success.
 * @return OTA_DOWNLOAD_INIT_FAIL  OTA download init failed.
 * @return OTA_DOWNLOAD_CON_FAIL   OTA download connect failed.
 * @return OTA_DOWNLOAD_REQ_FAIL   OTA download request failed.
 * @return OTA_DOWNLOAD_RECV_FAIL  OTA download receive failed.
 */
int ota_download_stop(void)
{
    int ret = 0;
    ota_upgrading = 0;
    return ret;
}

/**
 * ota_download_init  OTA download init
 *
 * @param[in] void
 *
 * @return OTA_SUCCESS             OTA success.
 * @return OTA_DOWNLOAD_INIT_FAIL  OTA download init failed.
 * @return OTA_DOWNLOAD_CON_FAIL   OTA download connect failed.
 * @return OTA_DOWNLOAD_REQ_FAIL   OTA download request failed.
 * @return OTA_DOWNLOAD_RECV_FAIL  OTA download receive failed.
 */
int ota_download_init(void)
{
    int ret = 0;
    ret = http_client_initialize();
    if(ret < 0) {
        ret = OTA_DOWNLOAD_INIT_FAIL;
    }
    return ret;
}

/**
 * ota_download_deinit  OTA download deinit
 *
 * @param[in] void
 *
 * @return OTA_SUCCESS             OTA success.
 * @return OTA_DOWNLOAD_INIT_FAIL  OTA download init failed.
 * @return OTA_DOWNLOAD_CON_FAIL   OTA download connect failed.
 * @return OTA_DOWNLOAD_REQ_FAIL   OTA download request failed.
 * @return OTA_DOWNLOAD_RECV_FAIL  OTA download receive failed.
 */
int ota_download_deinit(void)
{
    int ret = 0;
    return ret;
}
