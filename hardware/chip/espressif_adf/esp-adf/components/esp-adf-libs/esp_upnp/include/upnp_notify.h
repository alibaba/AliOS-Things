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

#ifndef _UPNP_NOTIF_H_
#define _UPNP_NOTIF_H_

#ifdef __cplusplus
extern "C" {
#endif

typedef struct upnp_notify_* upnp_notify_handle_t;

/**
 * @brief      Initialize UPnP notify, the value returned by this function can be used to
 *             subscribe/unsubscribe and to send an event to a client
 *
 * @return     The upnp notify handle
 */
upnp_notify_handle_t upnp_notify_init();

/**
 * @brief      Subscribe/Re-new the subscriptions for UPnP events
 *
 * @param[in]  notify        The notify handle
 * @param[in]  service_name  The service name
 * @param[in]  uri_or_sid    The uri or sid
 * @param[in]  timeout_sec   The timeout in seconds (If after this time passes without any renew
 *                           this subscription will be unsubscribed automatically )
 *
 * @return     The sid
 */
char *upnp_notify_subscribe(upnp_notify_handle_t notify,
                            const char           *service_name,
                            const char           *uri_or_sid,
                            int                  timeout_sec);

/**
 * @brief      Unsubscribe the notify by sid
 *
 * @param[in]  notify  The notify handle
 * @param[in]  sid     The sid
 *
 * @return
 *     -    ESP_OK
 *     -    ESP_xx if any errors
 */
esp_err_t upnp_notify_unsubscribe(upnp_notify_handle_t notify, char *sid);

/**
 * @brief      Unsubscribe the notify by service name
 *
 * @param[in]  notify           The notify handle
 * @param[in]  service_name     The service name
 *
 * @return
 *     -    ESP_OK
 *     -    ESP_xx if any errors
 */
esp_err_t upnp_notify_unsubscribe_by_name(upnp_notify_handle_t notify, const char *service_name);

/**
 * @brief      Send the notification to the client
 *
 * @param[in]  notify         The notify handle
 * @param      data           The data
 * @param[in]  data_len       The data length
 * @param[in]  service_name   The service name
 * @param[in]  delay_send_ms  The delay timeout, after a period of milliseconds the message will be sent
 *
 * @return
 *     -    ESP_OK
 *     -    ESP_xx if any errors
 */
esp_err_t upnp_notify_send(upnp_notify_handle_t notify,
                           char                 *data,
                           int                  data_len,
                           const char           *service_name);

/**
 * @brief      Clean up & destroy the notify
 *
 * @param[in]  notify  The notify
 *
 * @return
 *     -    ESP_OK
 *     -    ESP_xx if any errors
 */
esp_err_t upnp_notify_destroy(upnp_notify_handle_t notify);

#ifdef __cplusplus
}
#endif

#endif
