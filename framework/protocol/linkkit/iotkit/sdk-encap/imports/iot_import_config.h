#ifndef __IOT_IMPORT_CONFIG_H__
#define __IOT_IMPORT_CONFIG_H__

#ifndef CONFIG_HTTP_AUTH_TIMEOUT
    #define CONFIG_HTTP_AUTH_TIMEOUT    (5 * 1000)
#endif

#ifndef CONFIG_MID_HTTP_TIMEOUT
    #define CONFIG_MID_HTTP_TIMEOUT     (5 * 1000)
#endif

#ifndef CONFIG_GUIDER_AUTH_TIMEOUT
    #define CONFIG_GUIDER_AUTH_TIMEOUT  (10 * 1000)
#endif

#ifndef CONFIG_COAP_AUTH_TIMEOUT
    #define CONFIG_COAP_AUTH_TIMEOUT    (10 * 1000)
#endif

/* CM */

#define CM_MQTT_MSGLEN_SEND             (5 * 1024)
#define CM_MQTT_MSGLEN_8266             (1024)
#define CM_MQTT_MSGLEN_3060             (10 * 1024)
#define CM_MQTT_MSGLEN_GW               (50 * 1024)

/* CM_MQTT_SEND_LEN
* CM MQTT Send buffer's size.
* The buffer is used to send MQTT data.
*/
#ifndef CM_MQTT_SEND_LEN
#define CM_MQTT_SEND_LEN                  CM_MQTT_MSGLEN_8266
#endif /* CM_MQTT_SEND_LEN */

/* CM_MQTT_RECV_LEN
* CM MQTT Received  buffer's size.
* The buffer is used to receive MQTT data.
* The buffer size is decided on the TSL's size.
*/
#ifndef CM_MQTT_RECV_LEN
#define CM_MQTT_RECV_LEN                  CM_MQTT_MSGLEN_8266
#endif /* CM_MQTT_RECV_LEN */



#endif  /* __IOT_IMPORT_CONFIG_H__ */
