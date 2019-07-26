#ifndef __TEST_MAQTT_H__
#define __TEST_MAQTT_H__

#if defined(MQTT_COMM_ENABLED)

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "iot_import.h"
#include "iot_export.h"

#ifndef SDK_TEAM_TEST
    #define TEST_REPLACE_DEVCERT(pk,ps,dn,ds)
#else
    #include "sdk-testsuites_internal.h"
#endif

extern char *UTEST_MQTT_PRODUCT_KEY;
extern char *UTEST_MQTT_PRODUCT_SECRET;
extern char *UTEST_MQTT_DEVICE_NAME;
extern char *UTEST_MQTT_DEVICE_SECRET;

#define TOPIC_MAXLEN 256
extern char UTEST_TOPIC_UPDATE[TOPIC_MAXLEN];
extern char UTEST_TOPIC_ERROR[TOPIC_MAXLEN];
extern char UTEST_TOPIC_GET[TOPIC_MAXLEN];
extern char UTEST_TOPIC_DATA[TOPIC_MAXLEN];
extern char UTEST_TOPIC_DATA0[TOPIC_MAXLEN];
extern char UTEST_TOPIC_DATA1[TOPIC_MAXLEN];
extern char UTEST_TOPIC_DATA2[TOPIC_MAXLEN];
extern char UTEST_TOPIC_DATA3[TOPIC_MAXLEN];
extern char UTEST_TOPIC_NOTAUTHED[TOPIC_MAXLEN];

#define MSG_LEN_MAX         (1024)

#define EXAMPLE_TRACE(fmt, args...)  do { \
        printf("%s|%03d :: ", __func__, __LINE__); \
        printf(fmt, ##args); \
        printf("%s", "\r\n"); \
    } while(0)

void  utest_mqtt_event_handle(void *pcontext, void *pclient, iotx_mqtt_event_msg_pt msg);
void  utest_mqtt_recvmsg_callback(void *pcontext, void *pclient, iotx_mqtt_event_msg_pt msg);
void *utest_mqtt_init(void);
void utest_mqtt_update_topic(void);

extern long recvmsg_cnt;
extern char msg_buf[MSG_LEN_MAX];
extern char msg_readbuf[MSG_LEN_MAX];
extern char msg_pub[MSG_LEN_MAX];
extern void *mqtt_init_by_dev(int clean_session);

// extern void utest_mqtt_recvmsg_callback(void *pcontext, void *pclient, iotx_mqtt_event_msg_pt msg);
// extern void mqtt_recvmsg_callback_2(void *pcontext, void *pclient, iotx_mqtt_event_msg_pt msg);
// extern void *mqtt_init_by_dev(int clean_session);
// #define PRODUCT_KEY             "a1Zd7n5yTt8"
// #define DEVICE_NAME             "t1116_v2"
// #define DEVICE_SECRET           "PPcCin95MEz1RR8AlXmT3xcRjoVvTECO"
// #define PRODUCT_SECRET           "1P0lUzaTRJEYwTQc"
// #define UTEST_TOPIC_DATA_KJ       "/"PRODUCT_KEY"/"DEVICE_NAME"/data"
// #define UTEST_TOPIC_DATA0          UTEST_TOPIC_DATA_KJ
// #define UTEST_TOPIC_DATA1          "/"PRODUCT_KEY"/"DEVICE_NAME"/data1"
// #define UTEST_TOPIC_DATA2          "/"PRODUCT_KEY"/"DEVICE_NAME"/data2"
// #define UTEST_TOPIC_DATA3          "/"PRODUCT_KEY"/"DEVICE_NAME"/data3"



#endif // __TEST_MAQTT_H__
#endif