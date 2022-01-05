// Copyright 2017-2019 Espressif Systems (Shanghai) PTE LTD
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

#include <stdint.h>

#include "esp_err.h"

#include "btc_ble_mesh_prov.h"
#include "esp_ble_mesh_defs.h"

int32_t esp_ble_mesh_get_model_publish_period(esp_ble_mesh_model_t *model)
{
    if (model == NULL) {
        return 0;
    }
    return btc_ble_mesh_model_pub_period_get(model);
}

uint16_t esp_ble_mesh_get_primary_element_address(void)
{
    return btc_ble_mesh_get_primary_addr();
}

uint16_t *esp_ble_mesh_is_model_subscribed_to_group(esp_ble_mesh_model_t *model,
                                                    uint16_t group_addr)
{
    if (model == NULL) {
        return NULL;
    }
    return btc_ble_mesh_model_find_group(model, group_addr);
}

esp_ble_mesh_elem_t *esp_ble_mesh_find_element(uint16_t element_addr)
{
    if (!ESP_BLE_MESH_ADDR_IS_UNICAST(element_addr)) {
        return NULL;
    }
    return btc_ble_mesh_elem_find(element_addr);
}

uint8_t esp_ble_mesh_get_element_count(void)
{
    return btc_ble_mesh_elem_count();
}

esp_ble_mesh_model_t *esp_ble_mesh_find_vendor_model(const esp_ble_mesh_elem_t *element,
                                                     uint16_t company_id, uint16_t model_id)
{
    if (element == NULL) {
        return NULL;
    }
    return btc_ble_mesh_model_find_vnd(element, company_id, model_id);
}

esp_ble_mesh_model_t *esp_ble_mesh_find_sig_model(const esp_ble_mesh_elem_t *element,
                                                  uint16_t model_id)
{
    if (element == NULL) {
        return NULL;
    }
    return btc_ble_mesh_model_find(element, model_id);
}

const esp_ble_mesh_comp_t *esp_ble_mesh_get_composition_data(void)
{
    return btc_ble_mesh_comp_get();
}

esp_err_t esp_ble_mesh_model_subscribe_group_addr(uint16_t element_addr, uint16_t company_id,
                                                  uint16_t model_id, uint16_t group_addr)
{
    btc_ble_mesh_prov_args_t arg = {0};
    btc_msg_t msg = {0};

    if (!ESP_BLE_MESH_ADDR_IS_UNICAST(element_addr) ||
        !ESP_BLE_MESH_ADDR_IS_GROUP(group_addr)) {
        return ESP_ERR_INVALID_ARG;
    }

    ESP_BLE_HOST_STATUS_CHECK(ESP_BLE_HOST_STATUS_ENABLED);

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_PROV;
    msg.act = BTC_BLE_MESH_ACT_MODEL_SUBSCRIBE_GROUP_ADDR;

    arg.model_sub_group_addr.element_addr = element_addr;
    arg.model_sub_group_addr.company_id = company_id;
    arg.model_sub_group_addr.model_id = model_id;
    arg.model_sub_group_addr.group_addr = group_addr;

    return (btc_transfer_context(&msg, &arg, sizeof(btc_ble_mesh_prov_args_t), NULL)
            == BT_STATUS_SUCCESS ? ESP_OK : ESP_FAIL);
}

esp_err_t esp_ble_mesh_model_unsubscribe_group_addr(uint16_t element_addr, uint16_t company_id,
                                                    uint16_t model_id, uint16_t group_addr)
{
    btc_ble_mesh_prov_args_t arg = {0};
    btc_msg_t msg = {0};

    if (!ESP_BLE_MESH_ADDR_IS_UNICAST(element_addr) ||
        !ESP_BLE_MESH_ADDR_IS_GROUP(group_addr)) {
        return ESP_ERR_INVALID_ARG;
    }

    ESP_BLE_HOST_STATUS_CHECK(ESP_BLE_HOST_STATUS_ENABLED);

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_PROV;
    msg.act = BTC_BLE_MESH_ACT_MODEL_UNSUBSCRIBE_GROUP_ADDR;

    arg.model_unsub_group_addr.element_addr = element_addr;
    arg.model_unsub_group_addr.company_id = company_id;
    arg.model_unsub_group_addr.model_id = model_id;
    arg.model_unsub_group_addr.group_addr = group_addr;

    return (btc_transfer_context(&msg, &arg, sizeof(btc_ble_mesh_prov_args_t), NULL)
            == BT_STATUS_SUCCESS ? ESP_OK : ESP_FAIL);
}
