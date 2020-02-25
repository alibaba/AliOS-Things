/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <string.h>

#include "ulog/ulog.h"
#include "atcmd_util.h"

#define TAG "linkkit_at"

#define DEMO_PRODUCT_KEY      "a1FxISeKbq9"
#define DEMO_PRODUCT_SECRET   "ThNbP5iNUQ1lQe2Q"
#define DEMO_DEVICE_NAME      "alen-activate-test"
#define DEMO_DEVICE_SECRET    "jcumDL5AJRgU7zRNcCcnHRiQmtii0vDn"

/*
 * User-defined function to send property to Cloud via AT module
 * e.g. AT+IDMPP=0,"{\"LightSwitch\":1}"
 */
int user_send_property(const char *payload, int payload_len)
{
    int len = 0;
    char cmd_str[100] = { 0 };
    char *ptr = NULL;
    int pkid = -1;
    int devid = 0; /* master device default id */

    /*
     *  Construct AT command
     *  e.g. AT+IDMPP=0,
     */
    len = snprintf(cmd_str, sizeof(cmd_str),
                   "%s=%d,", "AT+IDMPP", 0);

    ptr = cmd_str + len;

    /*
     *  Add quote in payload string
     *  e.g. {"LightSwitch":1} ==> "{\"LightSwitch\":1}"
     */
    len += atcmd_add_slash_quote(ptr, sizeof(cmd_str) - len, payload, payload_len);

    /*
     *  Send out cmd and wait for return value
     *  e.g. packet ID for property post message
     */
    pkid = atcmd_send_wait_ret_val(cmd_str, len, true, WAIT_PACKET_ID,
                                   SEND_WAIT_REPLY_TIMEOUT_MS);

    LOGI(TAG, "Sent Property Message ID: %d", pkid);

    return pkid;
}

/*
 * User-defined function to answer service set message from Cloud
 * e.g. AT+IDMSSR=0,"ReceiveEvents","115686478","{}"
 */
int user_answer_service(int devid, char *serviceid, int serviceid_len,
                       char *payload, int payload_len, void *p_service_ctx)
{
    char cmd_str[100] = { 0 };
    int len = 0;
    char *ptr = NULL;
    int pkid = -1;

    char *msg_id = (char *)p_service_ctx;

    if (!msg_id) {
        LOGE(TAG, "Invalid msg id context!");
        return -1;
    }

    /*
     *  Construct AT command
     *  e.g. AT+IDMPP=0,
     */
    len = snprintf(cmd_str, sizeof(cmd_str),
                   "%s=%d,\"%s\",\"%s\",",
                   "AT+IDMSSR",
                   devid, serviceid, msg_id);

    ptr = cmd_str + len;

    /*
     *  Add quote in payload string
     *  e.g. {"LightSwitch":1} ==> "{\"LightSwitch\":1}"
     */
    len += atcmd_add_slash_quote(ptr, sizeof(cmd_str) - len, payload, payload_len);

    /*
     *  Send out cmd without waiting
     */
    atcmd_send_no_reply(cmd_str, len, true);

    return 0;
}

/*
 * User-defined function to process message from Cloud.
 * e.g service set message
 */
void user_process_cloud_msg(void)
{
    int res = 0;
    char *answer_payload = "{\"Error\": 0}";
    req_msg_t *req_msg = NULL;
    int len;

    /* check cloud message box */
    if(atcmd_tryfetch_msg(&req_msg) != 0) {
        return;
    }

    /* process msg according to msg type */
    switch(req_msg->type) {
        case SERV_SET_MSG:
            /* handle service set message here */

            /* reply service set message */
            res =  user_answer_service(req_msg->dev_id, req_msg->serv_id, strlen(req_msg->serv_id),
                                       answer_payload, strlen(answer_payload), (void *)req_msg->msg_id);
            LOGI(TAG, "Answer Server ret: %d msg: %s", res, req_msg->payload);
            break;

        /* add more message process here */
        /* case PROP_SET_MSG: */
        /* case OTA_INFO_MSG: */

        default:
            LOGI(TAG, "Unknown msg type");
            break;
    }

    aos_free(req_msg->payload);
    aos_free(req_msg);
}

/*
 *  Example entry
 */
int application_start(int argc, char *argv[])
{
    char buffer[20] = { 0 };
    int  count = 0;
    int  len;
    int  ret;

    /* set log level */
    aos_set_log_level(AOS_LL_DEBUG);

    /* init AT device */
    at_device_init();

    /* reset connection */
    at_device_reset();

    /* frequent conn/discon is not allowed */
    aos_msleep(1000);

    /* Config AT device */
    at_device_config(DEMO_PRODUCT_KEY,
                     DEMO_DEVICE_NAME,
                     DEMO_DEVICE_SECRET,
                     DEMO_PRODUCT_SECRET);

    /*
    * Connect to Cloud.
    * Input parameter: WiFi AP ssid, password
    * If ssid/password is NULL, use AWSS to connect WiFi 
    */
    do {
        ret = at_device_connect(NULL, NULL);
        if (ret >= 0) {
            break;
        }
        LOGI(TAG, "AT device connect Cloud failed! retry...\n");
        aos_msleep(5000);
    } while (1);

    /* Main loop */
    while (1) {
        count++;

        if (count % 5 == 0) {
            memset(buffer, sizeof(buffer), 0);

            /* Change "buffer" to your property name */
            len = snprintf(buffer, sizeof(buffer), "{\"Counter\": %d}", count);

            /* Send property to Cloud */
            user_send_property(buffer, len);
        }

        /* Process message from Cloud */
        user_process_cloud_msg();

        aos_msleep(1000);
    }

    return 0;
}
