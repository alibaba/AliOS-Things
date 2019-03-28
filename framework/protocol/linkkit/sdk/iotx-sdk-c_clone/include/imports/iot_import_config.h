/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */



#ifndef __IOT_IMPORT_CONFIG_H__
#define __IOT_IMPORT_CONFIG_H__

#ifndef CONFIG_HTTP_AUTH_TIMEOUT
    #define CONFIG_HTTP_AUTH_TIMEOUT        (5 * 1000)
#endif

#ifndef CONFIG_MID_HTTP_TIMEOUT
    #define CONFIG_MID_HTTP_TIMEOUT         (5 * 1000)
#endif

#ifndef CONFIG_GUIDER_AUTH_TIMEOUT
    #define CONFIG_GUIDER_AUTH_TIMEOUT      (10 * 1000)
#endif

#ifndef CONFIG_COAP_AUTH_TIMEOUT
    #define CONFIG_COAP_AUTH_TIMEOUT        (3 * 1000)
#endif

#ifndef CONFIG_MQTT_TX_MAXLEN
    #define CONFIG_MQTT_TX_MAXLEN           (2048)
#endif

#ifndef CONFIG_MQTT_RX_MAXLEN
    #define CONFIG_MQTT_RX_MAXLEN           (2048)
#endif

#ifndef CONFIG_SDK_THREAD_COST
    #define CONFIG_SDK_THREAD_COST          (0)
#endif

#ifndef CONFIG_MBEDTLS_DEBUG_LEVEL
    #define CONFIG_MBEDTLS_DEBUG_LEVEL      (0)
#endif

#ifndef CONFIG_RUNTIME_LOG_LEVEL
    #define CONFIG_RUNTIME_LOG_LEVEL        (2)
#endif

#ifndef CONFIG_DISPATCH_QUEUE_MAXLEN
    #define CONFIG_DISPATCH_QUEUE_MAXLEN    (50)
#endif

#ifndef CONFIG_DISPATCH_PACKET_MAXCOUNT
    #define CONFIG_DISPATCH_PACKET_MAXCOUNT (0)
#endif

#ifndef CONFIG_MSGCACHE_QUEUE_MAXLEN
    #define CONFIG_MSGCACHE_QUEUE_MAXLEN    (50)
#endif

#endif  /* __IOT_IMPORT_CONFIG_H__ */
