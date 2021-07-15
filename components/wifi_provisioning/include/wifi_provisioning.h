/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#ifndef  _WIFI_PROVISIONING_H_
#define  _WIFI_PROVISIONING_H_
#include <stdint.h>
#include <aos/list.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    char       ssid[33];
    char       password[65];
    uint8_t    bssid[6];
    uint8_t    channel;
    uint8_t    auth_mode;
} wifi_prov_result_t;

typedef enum {
    WIFI_PROV_EVENT_TIMEOUT,
    WIFI_RPOV_EVENT_GOT_RESULT,
} wifi_prov_event_t;

typedef void (*wifi_prov_cb)(uint32_t method_id, wifi_prov_event_t event, wifi_prov_result_t *result);

typedef struct {
    slist_t  next;
    char    *name;
    uint32_t method_id;
    int  (*start)(wifi_prov_cb cb);
    void (*stop)(void);
} wifi_prov_t;


/** register the provisoning method
  * @param prov prov method
  * @return method_id if success, -1 if failed
 */
int wifi_prov_method_register(wifi_prov_t *prov);


/** start provisioning
  * @param method_ids prov method_ids: you can use | (or), for start mutil prov method at same time
  * @param cb         callback for notify result: it will stop prov automatically after on call.
  * @param timeout_s  timeout：if we reach timeout, it will stop prov automatically.
  * @return method_ids for the ids that start failed or can not found, 0 for all methods start success.
 */
int wifi_prov_start(uint32_t method_ids, wifi_prov_cb cb, uint32_t timeout_s);

/** stop provisioning
 */
void wifi_prov_stop(void);

/** get prov method id
  * @param method_name the name for the prov method
  * @return method_id for the name or 0 if not found.
 */
uint32_t wifi_prov_get_method_id(char *method_name);

#ifdef __cplusplus
}
#endif

#endif