// Copyright 2015-2018 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef _UPNP_SERVICE_H
#define _UPNP_SERVICE_H

#ifdef __cplusplus
extern "C" {
#endif

#include "upnp_utils.h"
#include "esp_http_server.h"

typedef struct upnp_service_* upnp_service_handle_t;

typedef enum {
    ATTR_POINTER =  0x10000000, /*!< The execution type is POINTER, the value can be resolved by reference */
    ATTR_CONST =    0x20000000, /*!< The execution type is CONSTANT, the value can be resolved directly */
    ATTR_RESPONSE = 0x40000000, /*!< The execution type is RESPONSE, it can produce the response tag */
    ATTR_CALLBACK = 0x80000000, /*!< The execution type is upnp_attr_cb, the value can be resolved by calling this callback */
    ATTR_TYPE_INT = 0x01000000, /*!< The execution type is integer, the value can be resolved as integer */
    ATTR_TYPE_STR = 0x02000000, /*!< The execution type is string, the value can be resolved as string */
    /* User attribute space (lower 2bytes) */
    /* ... */
} upnp_attr_type_t;

/**
 * UPnP attribute
 */
typedef struct {
    const char       *name;         /*!< The XML tag attribute (for notificaiton) or tag name (for SOAP action) */
    void             *val;          /*!< Attribute execution type, depends on the type, it can be a callback, a pointer or a constant */
    upnp_attr_type_t type;          /*!< Attribute type */
    const char       *service_name; /*!< UPnP service name */
    const char       *action_name;  /*!< UPnP SOAP action name */
} upnp_attr_t;

typedef int (*upnp_attr_cb)(void *user_ctx, const upnp_attr_t *attr, int attr_num, char *buffer, int max_buffer_len);

/**
 * UPnP Service notify definition, currently only LastChange events are supported
 */
typedef struct {
    const char        *name;        /*!< Notify Tag name */
    const int         num_attrs;    /*!< Notify attributes number */
    const upnp_attr_t *attrs;       /*!< Notifies pointer */
} service_notify_t;

/**
 * UPnP Service action definition
 */
typedef struct service_action {
    const char          *name;            /*!< The action name */
    const int           num_attrs;        /*!< Action attributes number */
    const upnp_attr_t   *attrs;           /*!< Action attributes pointer */
    upnp_attr_cb        callback;         /*!< The callback to execute when this action name meets the request */
} service_action_t;

/**
 * UPnP Service definition
 */
typedef struct {
    const char               *name;             /*!< Service name */
    const char               *version;          /*!< Service version */
    const upnp_file_info_t   service_desc;      /*!< Service XML description */
    const upnp_file_info_t   notify_template;   /*!< Service XML notification template */
} upnp_service_item_t;

/**
 * UPnP Service configurations
 */
typedef struct {
    void            *user_ctx;  /*!< User context, it will be passed to the action & attribute callback */
    httpd_handle_t  httpd;      /*!< HTTPD handle need to be register the service */
} upnp_service_config_t;

/**
 * @brief      Initialize UPnP service
 *
 * @param      config  The configuration
 *
 * @return     The UPnP service handle
 */
upnp_service_handle_t upnp_service_init(upnp_service_config_t *config);

/**
 * @brief      Clean up and destroy the UPnP service, including unregistering of all services
 *
 * @param[in]  services  The services
 *
 * @return
 *     -    ESP_OK
 *     -    ESP_xx if any errors
 */
esp_err_t upnp_service_destroy(upnp_service_handle_t services);

/**
 * @brief      Register SOAP action for UPnP service
 *
 * @param[in]  services  The UPnP service handle
 * @param[in]  httpd     The httpd handle
 * @param[in]  service   The service item definition
 * @param[in]  actions   The actions
 * @param[in]  notifies  The notifies
 *
 * @return
 *     -    ESP_OK
 *     -    ESP_xx if any errors
 */
esp_err_t upnp_service_register_actions(upnp_service_handle_t     services,
                                        httpd_handle_t            httpd,
                                        const upnp_service_item_t *service,
                                        const service_action_t    actions[],
                                        const service_notify_t    notifies[]);

/**
 * @brief      Get XML descriptions for all registered services
 *             Notes: The returned string must be freed after use
 *
 * @param[in]  services  The UPnP service handle
 *
 * @return     The xml string
 */
char *upnp_service_get_xml_description(upnp_service_handle_t services);

/**
 * @brief      Send all service actions notification to the client
 *
 * @param[in]  services       The services
 * @param[in]  service_name   The service name
 *
 * @return
 *     -    ESP_OK
 *     -    ESP_xx if any errors
 */
esp_err_t upnp_service_send_notify(upnp_service_handle_t services,
                                   const char            *service_name);

/**
 * @brief      Send single AVTransport actions notification to the client
 *
 * @param[in]  services       The services
 * @param[in]  service_name   The service name
 *
 * @return
 *     -    ESP_OK
 *     -    ESP_xx if any errors
 */
esp_err_t upnp_service_send_avt_notify(upnp_service_handle_t services,
                                       const char            *action_name);

/**
 * @brief      Send custom notification to the client
 *
 * @param[in]  services       The services
 * @param[in]  service_name   The service name
 * @param[in]  event_xml      custom event xml (ref: EventLastChange.xml)
 * @param[in]  xml_length     The xml length
 *
 * @return
 *     -    ESP_OK
 *     -    ESP_xx if any errors
 */
esp_err_t upnp_service_send_custom_notify(upnp_service_handle_t services, const char *service_name, const char *event_xml, int32_t xml_length);

#ifdef __cplusplus
}
#endif

#endif
