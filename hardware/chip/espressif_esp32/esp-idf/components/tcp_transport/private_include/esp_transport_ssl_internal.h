// Copyright 2015-2019 Espressif Systems (Shanghai) PTE LTD
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

#ifndef _ESP_TRANSPORT_INTERNAL_H_
#define _ESP_TRANSPORT_INTERNAL_H_

/**
 * @brief      Sets error to common transport handle
 *
 *             Note: This function copies the supplied error handle object to tcp_transport's internal
 *             error handle object
 *
 * @param[in]  A transport handle
 *
 */
void esp_transport_set_errors(esp_transport_handle_t t, const esp_tls_error_handle_t error_handle);


#endif /* _ESP_TRANSPORT_INTERNAL_H_ */
