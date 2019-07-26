#ifndef __CONFIG_NEW_H__
#define __CONFIG_NEW_H__

#include "iot_export_linkkit.h"
#include "cJSON.h"

#ifndef SDK_TEAM_TEST
    #define TEST_REPLACE_DEVCERT(pk,ps,dn,ds)
#else
    #include "sdk-testsuites_internal.h"
#endif

#define EXAMPLE_TRACE(fmt, ...)  \
    do { \
        HAL_Printf("\033[;31m%s|%03d :: ", __func__, __LINE__); \
        HAL_Printf(fmt, ##__VA_ARGS__); \
        HAL_Printf("%s\033[0m", "\r\n"); \
    } while(0)

#define PROPERTY_IDENTIFIER "LightSwitch"
#define EVENT_IDENTIFIER "Error"
#define EVENT_OUTPUT_IDENTIFIER "Error.ErrorCode"
#define SERVIER_IDENTIFIER "Custom"
#define SERVICE_INPUT_IDENTIFIER "Custom.input"
#define SERVICE_OUTPUT_IDENTIFIER "Custom.ouput"

#define SINGLE_CONFIG_CONNECT_TIMEOUT (10000)

typedef struct {
    int master_devid;
    int cloud_connected;
    int master_initialized;
    int flag;
} user_example_ctx_t;

extern user_example_ctx_t *user_example_get_ctx(void);

extern int user_connected_event_handler(void);

extern int user_disconnected_event_handler(void);

extern int user_down_raw_data_arrived_event_handler(const int devid, const unsigned char *payload,
        const int payload_len);

extern int user_service_request_event_handler(const int devid, const char *serviceid, const int serviceid_len,
        const char *request, const int request_len,
        char **response, int *response_len);

extern int user_property_set_event_handler(const int devid, const char *request, const int request_len);

extern int user_property_get_event_handler(const int devid, const char *request, const int request_len, char **response,
        int *response_len);

extern int user_report_reply_event_handler(const int devid, const int msgid, const int code, const char *reply,
        const int reply_len);

extern int user_ntp_response_event_handler(const char *utc);

extern int user_initialized(const int devid);

extern char TSL_STRING[];
extern char *TEST_PRODUCT_KEY;
extern char *TEST_PRODUCT_SECRET;
extern char *TEST_DEVICE_NAME;
extern char *TEST_DEVICE_SECRET;

void wait_for_connected();
void wait_seconds(int sec);
void *test_malloc(size_t size);
void test_free(void *ptr);

#endif // __CONFIG_H__
