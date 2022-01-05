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

#ifndef _UPNP_H
#define _UPNP_H

#ifdef __cplusplus
extern "C" {
#endif

#include "esp_http_server.h"
#include "upnp_service.h"

typedef struct upnp_* upnp_handle_t;

/**
 * UPnP Device Description – general info
 * Official: http://www.upnp.org/specs/arch/UPnP-arch-DeviceArchitecture-v1.1.pdf
 * Ref: https://embeddedinn.wordpress.com/tutorials/upnp-device-architecture/
 */
typedef struct {
    const char              *friendly_name;     /*!< Short user-friendly title */
    const char              *serial;            /*!< Device manufacturer's serial number */
    const char              *uuid;              /*!< Device UUID */
    const char              *root_path;         /*!< The custom XML root path */
    const char              *device_type;       /*!< Will replace for deviceType in UPnP schema urn:schemas-upnp-org:device:deviceType:version */
    const char              *version;           /*!< Will replace for version in UPnP schema urn:schemas-upnp-org:device:deviceType:version */
    const char              *upc;               /*!< Universal Product Code */
    const char              *manufacturer;      /*!< Manufacturer name */
    const char              *manufacturer_url;  /*!< URL to manufacturer site */
    const char              *model_description; /*!< long user-friendly title */
    const char              *model_name;        /*!< model name */
    const char              *model_number;      /*!< model number */
    const char              *model_url;         /*!< URL to model site */
    const upnp_file_info_t  logo;               /*!< Logo for the device*/
    httpd_handle_t          httpd;              /*!< UPnP needs a HTTP server to work */
    int                     port;               /*!< Server port (for advertise)*/
    void                    *user_ctx;          /*!< User context, it be will passed to attribute/notify and action callback */
    bool                    device_list;        /*<! Support Device List */
} upnp_config_t;


/**
 * @brief      Intialize UPnP object
 *
 * @param[in]  config  The configuration
 *
 * @return     UPnP handle
 */
upnp_handle_t upnp_init(const upnp_config_t *config);

/**
 * @brief      Send the notification to the client
 *
 * @param[in]  upnp           The upnp handle
 * @param[in]  service_name   The service name
 *
 * @return
 *     - ESP_OK
 *     - ESP_xx if any errors
 */
esp_err_t upnp_send_notify(upnp_handle_t upnp, const char *service_name);

/**
 * @brief      Send the AVTransport notification to the client
 *
 * @param[in]  upnp           The upnp handle
 * @param[in]  action_name    The action name
 *
 * @return
 *     - ESP_OK
 *     - ESP_xx if any errors
 */
esp_err_t upnp_send_avt_notify(upnp_handle_t upnp, const char *action_name);

/**
 * @brief      Send custom notification to the client
 *
 * @param[in]  upnp           The upnp handle
 * @param[in]  service_name   The service name
 * @param[in]  event_xml      custom event xml (ref: EventLastChange.xml)
 * @param[in]  xml_length     The xml length
 *
 * @return
 *     -    ESP_OK
 *     -    ESP_xx if any errors
 */
esp_err_t upnp_send_custom_notify(upnp_handle_t upnp, const char *service_name, const char *event_xml, int32_t xml_length);

/**
 * @brief      Clean up and destroy the UPnP handle
 *
 * @param[in]  upnp  The UPnP handle
 *
 * @return
 *     - ESP_OK
 *     - ESP_xx if any errors
 */
esp_err_t upnp_destroy(upnp_handle_t upnp);

/**
 * @brief      Serve http static files (using upnp_file_info_t format)
 *
 * @param      req   The request
 *
 * @return
 *     - ESP_OK
 *     - ESP_xx if any errors
 */
esp_err_t upnp_serve_static_files(httpd_req_t *req);

/**
 * @brief      Register actions/notifies for the service
 *
 * @param[in]  upnp             The UPnP handle
 * @param[in]  service_item     The service item
 * @param[in]  service_actions  The service actions
 * @param[in]  service_notify   The service notify
 *
 * @return
 *     - ESP_OK
 *     - ESP_xx if any errors
 */
esp_err_t upnp_register_service(upnp_handle_t               upnp,
                                const upnp_service_item_t   *service_item,
                                const service_action_t      service_actions[],
                                const service_notify_t      service_notify[]);
#ifdef __cplusplus
}
#endif

#endif
