/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef SERVICE_H
#define SERVICE_H

#include "aos/aos.h"
#define NAME_LENGTH 12

enum SERVICE_CODE {
    SERVICE_BUFFER_INSUFFICENT = -3,
    SERVICE_CODE_UNKNOWN       = -2,
    SERVICE_RESULT_ERR         = -1,
    SERVICE_RESULT_OK          =  0,

    SERVICE_CODE_BEGIN         = 0x0100,

    SERVICE_EVENT,
    SERVICE_DATA,

    SERVICE_CONN_INIT,
    SERVICE_CONN_READY,
    SERVICE_CONN_CLOSE,

    SERVICE_ATTACH,
    SERVICE_DETACH,

    /* service state */
    SERVICE_STATE_INIT,
    SERVICE_STATE_PREPARE,
    SERVICE_STATE_READY,
    SERVICE_STATE_BUSY,
    SERVICE_STATE_STOP,
    SERVICE_CODE_END           = 0x01FF,
};

typedef int (*service_cb)(int type, void *data, int dlen, void *result,
                          int *rlen);

typedef struct service {
    dlist_t list_head;
    char name[NAME_LENGTH];
    int state;
    int (*_init)(void);
    int (*prepare)(void);
    int (*start)(void);
    int (*stop)(void);
    int (*put)(void *, int);
    int (*put_async)(void *, int, void *( *)(void *), void *);
    int (*get)(void *, int, void *, int);
    int (*add_listener)(service_cb);
    int (*del_listener)(service_cb);
} service_t;

typedef struct service_listener {
    dlist_t list_head;
    char name[NAME_LENGTH];
    service_cb listen;
} service_listener_t;

/*
#define SERVICE_DELCARE(svc) \
        typedef struct svc##_service { \
            dlist_t list_head; \
            char name[NAME_LENGTH]; \
            int state; \
            int (*init)(void); \
            int (*start)(void); \
            int (*stop)(void); \
            int (*put)(void*, int); \
            int (*put_async)(void*, int, void *(*)(void *), void *); \
            int (*get)(void*, int); \
            int (*addlistener)(service_cb*); \
        } svc##_service_t; \
        extern svc##_service_t svc

#define SERVICE_DEFINE(svc) \
        svc##_service_t svc
*/
#define SERVICE_DELCARE(svc) \
        extern service_t svc; \
        extern int _##svc##_init();

#define SERVICE_DEFINE(svc) \
        service_t svc; \
        int _##svc##_init() { \
            sprintf(svc.name, #svc); \
            svc.state = SERVICE_STATE_STOP; \
            svc.prepare = &svc##_prepare; \
            svc.start = &svc##_start; \
            svc.stop = &svc##_stop; \
            svc.put = &svc##_put; \
            svc.put_async = &svc##_put_async; \
            svc.get = &svc##_get; \
            svc.add_listener = &svc##_add_listener; \
            svc.del_listener = &svc##_del_listener; \
            dlist_init(&g_##svc##_listener_list); \
            LOGI("SVG",#svc " initial"); \
            return SERVICE_RESULT_OK; \
        }

#endif
