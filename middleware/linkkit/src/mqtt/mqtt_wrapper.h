#include "infra_types.h"
#include "infra_defs.h"
#include "wrappers_defs.h"
#include "wrappers.h"
#include "mqtt_api.h"

/* mqtt protocol wrapper */
void *wrapper_mqtt_init(iotx_mqtt_param_t *mqtt_params);
int wrapper_mqtt_connect(void *client);
int wrapper_mqtt_yield(void *client, int timeout_ms);
int wrapper_mqtt_check_state(void *client);
int wrapper_mqtt_subscribe(void *client,
                           const char *topicFilter,
                           iotx_mqtt_qos_t qos,
                           iotx_mqtt_event_handle_func_fpt topic_handle_func,
                           void *pcontext);
int wrapper_mqtt_subscribe_sync(void *client,
                                const char *topic_filter,
                                iotx_mqtt_qos_t qos,
                                iotx_mqtt_event_handle_func_fpt topic_handle_func,
                                void *pcontext,
                                int timeout_ms);
int wrapper_mqtt_unsubscribe(void *client, const char *topicFilter);
int wrapper_mqtt_publish(void *client, const char *topicName, iotx_mqtt_topic_info_pt topic_msg);
int wrapper_mqtt_release(void **pclient);
int wrapper_mqtt_nwk_event_handler(void *client, iotx_mqtt_nwk_event_t event, iotx_mqtt_nwk_param_t *param);


