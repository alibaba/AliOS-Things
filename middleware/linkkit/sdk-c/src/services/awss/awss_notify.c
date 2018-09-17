/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */



#include <stdlib.h>
#include <stdbool.h>
#include "os.h"
#include "enrollee.h"
#include "utils.h"
#include "zconfig_utils.h"
#include "passwd.h"
#include "platform.h"
#include "awss_notify.h"
#include "json_parser.h"
#include "awss_cmp.h"
#include "awss_wifimgr.h"
#include "awss_main.h"
#include "awss_timer.h"
#include "awss.h"

#if defined(__cplusplus) /* If this is a C++ compiler, use C linkage */
extern "C"
{
#endif

#define AWSS_CHECK_RESP_TIME (300)
#define AWSS_NOTIFY_PORT (5683)
#define AWSS_NOTIFY_HOST "255.255.255.255"
#define AWSS_DEV_NOTIFY_FMT \
    "{\"id\":\"%u\",\"version\":\"1.0\",\"method\":\"%s\",\"params\":{%s}}"

    struct notify_map_t
    {
        unsigned char notify_type;
        char *        notify_method;
        char *        notify_topic;
        void *        cb;
    };

    static unsigned char  g_notify_id;
    static unsigned short g_notify_msg_id;
    static char           awss_notify_resp[AWSS_NOTIFY_MAX] = { 0 };

    static void *get_devinfo_timer      = NULL;
    static void *connectap_notify_timer = NULL;
    static void *devinfo_notify_timer   = NULL;
    static void *suc_notify_timer       = NULL;

    extern char awss_report_token_suc;
    extern char awss_report_token_cnt;

    static inline int awss_connectap_notify_resp(void *context, int result,
                                                 void *userdata, void *remote,
                                                 void *message);
    static inline int awss_devinfo_notify_resp(void *context, int result,
                                               void *userdata, void *remote,
                                               void *message);
    static inline int awss_suc_notify_resp(void *context, int result,
                                           void *userdata, void *remote,
                                           void *message);
    static int        awss_notify_response(int type, int result, void *message);
    static int        awss_process_get_devinfo();
    int               awss_connectap_notify();
    int               awss_devinfo_notify();
    int               awss_suc_notify();

    static bool       awss_notify_running = false;

    static const struct notify_map_t notify_map[] = {
        { AWSS_NOTIFY_DEV_TOKEN, METHOD_DEV_INFO_NOTIFY, TOPIC_NOTIFY,
          awss_connectap_notify_resp },
        { AWSS_NOTIFY_DEV_RAND, METHOD_AWSS_DEV_INFO_NOTIFY, TOPIC_AWSS_NOTIFY,
          awss_devinfo_notify_resp },
        { AWSS_NOTIFY_SUC, METHOD_AWSS_CONNECTAP_NOTIFY,
          TOPIC_AWSS_CONNECTAP_NOTIFY, awss_suc_notify_resp }
    };

    /*
     * {
     *  "id": "123",
     *  "code": 200,
     *  "data": {}
     * }
     */
    static inline int awss_connectap_notify_resp(void *context, int result,
                                                 void *userdata, void *remote,
                                                 void *message)
    {
        int res = awss_notify_response(AWSS_NOTIFY_DEV_TOKEN, result, message);
        if (res == 1) {
            awss_update_token();
#ifdef DEV_BIND_TEST
            os_reboot();
#endif
        }
        return res;
    }

    static inline int awss_devinfo_notify_resp(void *context, int result,
                                               void *userdata, void *remote,
                                               void *message)
    {
        return awss_notify_response(AWSS_NOTIFY_DEV_RAND, result, message);
    }

    static inline int awss_suc_notify_resp(void *context, int result,
                                           void *userdata, void *remote,
                                           void *message)
    {
        return awss_notify_response(AWSS_NOTIFY_SUC, result, message);
    }

    static int awss_notify_response(int type, int result, void *message)
    {
        awss_debug("%s, type:%d,result:%u\r\n", __func__, type, result);

        if (message == NULL)
            return -1;

        if (result != 0)
            return 0;

        if (awss_cmp_get_coap_code(message) >= 0x60)
            return 0;

        {
            int   val = 0;
            int   len = 0, mlen = 0;
            char *payload = NULL, *elem = NULL;

            if ((payload = awss_cmp_get_coap_payload(message, &len)) == NULL ||
                len > 0x40 || len == 0)
                return 0;

            awss_debug("payload:%s\r\n", payload);

            elem = json_get_value_by_name(payload, len, AWSS_JSON_ID, &mlen, 0);
            if (elem == NULL)
                return 0;

            val = atoi(elem);
            if (val != 123 && val > g_notify_id)
                return 0;

            elem =
              json_get_value_by_name(payload, len, AWSS_JSON_CODE, &mlen, 0);
            if (elem == NULL)
                return 0;

            val = atoi(elem);
            if (val != 200)
                return 0;
        }

        unsigned char i = 0;
        for (i = 0; i < sizeof(notify_map) / sizeof(notify_map[0]); i++) {
            if (notify_map[i].notify_type != type) {
                continue;
            }
            awss_notify_resp[type] = 1;
            break;
        }

        return awss_notify_resp[type];
    }

    int awss_notify_dev_info(int type, int count)
    {
        char *buf      = NULL;
        char                *dev_info = NULL;
        int                 i;
        platform_netaddr_t  notify_sa;

        do {
            void *cb     = NULL;
            char *method = NULL, *topic = NULL;
            for (i = 0; i < sizeof(notify_map) / sizeof(notify_map[0]); i++) {
                if (notify_map[i].notify_type != type) {
                    continue;
                }

                method = notify_map[i].notify_method;
                topic  = notify_map[i].notify_topic;
                cb     = notify_map[i].cb;
                break;
            }
            if (method == NULL || topic == NULL) {
                break;
            }

            buf      = os_zalloc(DEV_INFO_LEN_MAX);
            dev_info = os_zalloc(DEV_INFO_LEN_MAX);
            if (buf == NULL || dev_info == NULL) {
                break;
            }

            memset(&notify_sa, 0, sizeof(notify_sa));

            memcpy(notify_sa.host, AWSS_NOTIFY_HOST, strlen(AWSS_NOTIFY_HOST));
            notify_sa.port = AWSS_NOTIFY_PORT;

            awss_build_dev_info(type, dev_info, DEV_INFO_LEN_MAX);

            snprintf(buf, DEV_INFO_LEN_MAX - 1, AWSS_DEV_NOTIFY_FMT,
                     ++g_notify_id, method, dev_info);

            awss_debug("topic:%s, %s\n", topic, buf);
            for (i = 0; i < count; i++) {
                awss_cmp_coap_send(buf, strlen(buf), &notify_sa, topic, cb,
                                   &g_notify_msg_id);
                if (count > 1) {
                    os_msleep(200 + 100 * i);
                }
                if (awss_notify_resp[type]) {
                    break;
                }
            }
        } while (0);

        if (buf)
            os_free(buf);
        if (dev_info)
            os_free(dev_info);

        return awss_notify_resp[type];
    }

#define AWSS_NOTIFY_CNT_MAX (30)

    static char connectap_cnt = 0;
    int         awss_connectap_notify_stop()
    {
        if (connectap_notify_timer == NULL)
            return 0;
        awss_stop_timer(connectap_notify_timer);
        connectap_notify_timer = NULL;
        connectap_cnt          = AWSS_NOTIFY_CNT_MAX;
        return 0;
    }

    static void *coap_session_ctx = NULL;

    static int awss_process_get_devinfo()
    {
        if (awss_report_token_suc == 0) {
            HAL_Timer_Start(get_devinfo_timer, AWSS_CHECK_RESP_TIME);
            return 0;
        }

        if (coap_session_ctx == NULL) {
            return -1;
        }

        char *                     buf      = NULL;
        char *                     dev_info = NULL;
        int                        len = 0, id_len = 0;
        char *                     msg = NULL, *id = NULL;
        char                       req_msg_id[MSG_REQ_ID_LEN];
        struct coap_session_ctx_t *ctx =
          (struct coap_session_ctx_t *)coap_session_ctx;

        buf = os_zalloc(DEV_INFO_LEN_MAX);
        if (buf == NULL) {
            goto GET_DEV_INFO_ERR;
        }
        dev_info = os_zalloc(DEV_INFO_LEN_MAX);
        if (dev_info == NULL) {
            goto GET_DEV_INFO_ERR;
        }
        msg = awss_cmp_get_coap_payload(ctx->request, &len);
        if (msg == NULL) {
            goto GET_DEV_INFO_ERR;
        }
        id = json_get_value_by_name(msg, len, "id", &id_len, 0);
        memset(req_msg_id, 0, sizeof(req_msg_id));
        memcpy(req_msg_id, id, id_len);

        awss_build_dev_info(AWSS_NOTIFY_DEV_TOKEN, buf, DEV_INFO_LEN_MAX);
        snprintf(dev_info, DEV_INFO_LEN_MAX - 1, "{%s}", buf);
        memset(buf, 0x00, DEV_INFO_LEN_MAX);
        snprintf(buf, DEV_INFO_LEN_MAX - 1, AWSS_ACK_FMT, req_msg_id, 200,
                 dev_info);
        os_free(dev_info);

        awss_debug("sending message to app: %s", buf);
        char topic[TOPIC_LEN_MAX] = { 0 };
        if (ctx->is_mcast) {
            awss_build_topic((const char *)TOPIC_GETDEVICEINFO_MCAST, topic,
                             TOPIC_LEN_MAX);
        } else {
            awss_build_topic((const char *)TOPIC_GETDEVICEINFO_UCAST, topic,
                             TOPIC_LEN_MAX);
        }
        if (0 != awss_cmp_coap_send_resp(buf, strlen(buf), ctx->remote, topic,
                                         ctx->request)) {
            awss_debug("sending failed.");
        }
        os_free(buf);
        awss_release_coap_ctx(coap_session_ctx);
        coap_session_ctx = NULL;
        awss_stop_timer(get_devinfo_timer);
        get_devinfo_timer = NULL;
        awss_update_token();
        return 0;

    GET_DEV_INFO_ERR:
        awss_release_coap_ctx(coap_session_ctx);
        coap_session_ctx = NULL;
        awss_stop_timer(get_devinfo_timer);
        get_devinfo_timer = NULL;
        if (buf) {
            os_free(buf);
        }
        if (dev_info) {
            os_free(dev_info);
        }
        return -1;
    }

    bool get_awss_notify_running_flag(void)
    {
        return awss_notify_running;
    }

    static int online_get_device_info(void *ctx, void *resource, void *remote,
                                      void *request, char is_mcast)
    {
        int timeout = 0;
        /*
         * if cloud is not ready, don't response token
         */
        if (awss_report_token_cnt == 0) {
            return -1;
        }
        /*
         * if the last one is not finished, drop current request
         */
        if (coap_session_ctx != NULL) {
            return -1;
        }
        /*
         * copy coap session context
         */
        coap_session_ctx = awss_cpy_coap_ctx(request, remote, is_mcast);
        if (coap_session_ctx == NULL) {
            return -1;
        }

        timeout = awss_token_timeout();
        if (timeout) {
            produce_random(aes_random, sizeof(aes_random));
            awss_report_token();
        }

        if (get_devinfo_timer == NULL)
            get_devinfo_timer = HAL_Timer_Create(
              "get_devinfo", (void (*)(void *))awss_process_get_devinfo, NULL);
        HAL_Timer_Stop(get_devinfo_timer);
        HAL_Timer_Start(get_devinfo_timer, timeout ? AWSS_CHECK_RESP_TIME : 1);

        return 0;
    }

    int online_mcast_get_device_info(void *ctx, void *resource, void *remote,
                                     void *request)
    {
        return online_get_device_info(ctx, resource, remote, request, 1);
    }

    int online_ucast_get_device_info(void *ctx, void *resource, void *remote,
                                     void *request)
    {
        return online_get_device_info(ctx, resource, remote, request, 0);
    }

    int awss_connectap_notify()
    {
        static int connectap_interval = 0;

        /*
         * wait for token is sent to cloud and rx reply from cloud
         */
        if (awss_report_token_suc == 0) {
            if (connectap_notify_timer == NULL)
                connectap_notify_timer = HAL_Timer_Create(
                  "connectap", (void (*)(void *))awss_connectap_notify, NULL);
            HAL_Timer_Stop(connectap_notify_timer);
            HAL_Timer_Start(connectap_notify_timer, AWSS_CHECK_RESP_TIME);
            return 0;
        }

        if (connectap_cnt == 0)
            awss_event_post(AWSS_BIND_NOTIFY);

        do {
            if (awss_notify_resp[AWSS_NOTIFY_DEV_TOKEN] != 0) {
                break;
            }

            unsigned char i = 0;
            for (i = 0; i < RANDOM_MAX_LEN; i++)
                if (aes_random[i] != 0x00) {
                    break;
                }

            if (i >= RANDOM_MAX_LEN) {
                produce_random(aes_random, sizeof(aes_random));
            }

            awss_notify_dev_info(AWSS_NOTIFY_DEV_TOKEN, 1);
#ifdef DEV_BIND_TEST
            if (connectap_cnt > 3)
                os_reboot();
#endif

            connectap_interval += 100;
            awss_notify_running = true;
            if (connectap_cnt++ < AWSS_NOTIFY_CNT_MAX &&
                awss_notify_resp[AWSS_NOTIFY_DEV_TOKEN] == 0) {
                if (connectap_notify_timer == NULL)
                    connectap_notify_timer = HAL_Timer_Create(
                      "connectap", (void (*)(void *))awss_connectap_notify,
                      NULL);
                HAL_Timer_Stop(connectap_notify_timer);
                HAL_Timer_Start(connectap_notify_timer, connectap_interval);
                return 0;
            }
        } while (0);

        awss_notify_resp[AWSS_NOTIFY_DEV_TOKEN] = 0;
        connectap_interval                      = 0;
        connectap_cnt                           = 0;
        awss_notify_running                     = false;
        if (connectap_notify_timer) {
            awss_stop_timer(connectap_notify_timer);
            connectap_notify_timer = NULL;
        }
        return 1;
    }

    int awss_devinfo_notify_stop()
    {
        if (devinfo_notify_timer == NULL)
            return 0;
        awss_stop_timer(devinfo_notify_timer);
        devinfo_notify_timer = NULL;
        return 0;
    }

    int awss_suc_notify_stop()
    {
        if (suc_notify_timer == NULL)
            return 0;
        awss_stop_timer(suc_notify_timer);
        suc_notify_timer = NULL;
        return 0;
    }

    int awss_suc_notify()
    {
        static int  suc_interval = 0;
        static char suc_cnt      = 0;

        awss_debug("resp:%d\r\n", awss_notify_resp[AWSS_NOTIFY_SUC]);

        if (suc_cnt == 0)
            awss_event_post(AWSS_SUC_NOTIFY);

        do {
            if (awss_notify_resp[AWSS_NOTIFY_SUC] != 0) {
                break;
            }

            awss_notify_dev_info(AWSS_NOTIFY_SUC, 1);

            suc_interval += 100;
            if (suc_cnt++ < AWSS_NOTIFY_CNT_MAX &&
                awss_notify_resp[AWSS_NOTIFY_SUC] == 0) {
                if (suc_notify_timer == NULL)
                    suc_notify_timer = HAL_Timer_Create(
                      "awss_suc", (void (*)(void *))awss_suc_notify, NULL);
                HAL_Timer_Stop(suc_notify_timer);
                HAL_Timer_Start(suc_notify_timer, suc_interval);
                return 0;
            }
        } while (0);

        awss_notify_resp[AWSS_NOTIFY_SUC] = 0;
        suc_interval                      = 0;
        suc_cnt                           = 0;
        if (suc_notify_timer) {
            awss_stop_timer(suc_notify_timer);
            suc_notify_timer = NULL;
        }
        return 1;
    }

    int awss_devinfo_notify()
    {
        static int  devinfo_interval = 0;
        static char devinfo_cnt      = 0;

        do {
            if (awss_notify_resp[AWSS_NOTIFY_DEV_RAND] != 0) {
                break;
            }

            awss_notify_dev_info(AWSS_NOTIFY_DEV_RAND, 1);

            devinfo_interval += 100;
            if (devinfo_cnt++ < AWSS_NOTIFY_CNT_MAX &&
                awss_notify_resp[AWSS_NOTIFY_DEV_RAND] == 0) {
                if (devinfo_notify_timer == NULL)
                    devinfo_notify_timer = HAL_Timer_Create(
                      "devinfo", (void (*)(void *))awss_devinfo_notify, NULL);
                HAL_Timer_Stop(devinfo_notify_timer);
                HAL_Timer_Start(devinfo_notify_timer, devinfo_interval);
                return 0;
            }
        } while (0);

        awss_notify_resp[AWSS_NOTIFY_DEV_RAND] = 0;
        devinfo_interval                       = 0;
        devinfo_cnt                            = 0;
        if (devinfo_notify_timer) {
            awss_stop_timer(devinfo_notify_timer);
            devinfo_notify_timer = NULL;
        }
        return 1;
    }

#if defined(__cplusplus) /* If this is a C++ compiler, use C linkage */
}
#endif
