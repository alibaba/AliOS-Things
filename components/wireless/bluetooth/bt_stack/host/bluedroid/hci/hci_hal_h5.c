/******************************************************************************
 *
 *  Copyright (C) 2014 Google, Inc.
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at:
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 ******************************************************************************/

#define LOG_TAG "bt_hci_h5"

#include <assert.h>
#include <errno.h>
#include <string.h>

#include "hci/hci_hal.h"
#include "osi/osi.h"
#include "osi/thread.h"
#include "hci/vendor.h"

#include "hci/hci_layer.h"
#include "hci/hci_h5_int.h"

#if BLUETOOTH_H5
#define HCI_HAL_SERIAL_BUFFER_SIZE 1026
#define HCI_BLE_EVENT 0x3e

// Our interface and modules we import
static const hci_hal_t h5_interface;
static const hci_h5_t *h5_int_interface;
static const hci_hal_callbacks_t *h5_hal_callbacks;
typedef struct BtTaskEvt_t {
    uint32_t sig;
    void  *par;
    void *cb;
    void *arg;
}BtTaskEvt_t;

static void h5_data_ready_cb(BT_HDR *packet);

static  hci_h5_hal_callbacks_t h5_int_hal_callbacks = {
    .h5_data_ready_cb = h5_data_ready_cb,
};

static bool h5_hal_open(const hci_hal_callbacks_t *upper_callbacks, void *task_thread)
{
    HCI_TRACE_DEBUG("%s", __func__);

    assert(upper_callbacks != NULL);
  
    h5_hal_callbacks = upper_callbacks;
    h5_int_interface = hci_get_h5_int_interface();
    h5_int_interface->h5_int_init(&h5_int_hal_callbacks);
    // TODO(zachoverflow): close if already open / or don't reopen (maybe at the hci layer level)

    return true;
}

static void h5_hal_close()
{
    HCI_TRACE_DEBUG("%s", __func__);

    h5_int_interface->h5_int_cleanup();
}

static void h5_data_ready_cb(BT_HDR *packet)
{
    h5_hal_callbacks->packet_ready(packet);
}

static uint16_t h5_transmit_data(serial_data_type_t type, uint8_t *data, uint16_t length)
{
    assert(data != NULL);
    assert(length > 0);

    uint16_t transmitted_length = 0;
    uint16_t opcode;
    uint8_t  *data_temp = data;

    if (type < DATA_TYPE_COMMAND || type > DATA_TYPE_SCO) {
        HCI_TRACE_ERROR("%s invalid data type: %d", __func__, type);
        return 0;
    }

    switch (type) {
        case DATA_TYPE_COMMAND:
            STREAM_TO_UINT16(opcode, data_temp);

            if (opcode == HCI_VSC_H5_INIT) {
                transmitted_length = length;
                h5_int_interface->h5_send_sync_cmd(opcode, NULL, length);
                break;
            }

            transmitted_length = h5_int_interface->h5_send_cmd(type, data, length);
            break;

        case DATA_TYPE_ACL:
            transmitted_length = h5_int_interface->h5_send_acl_data(type, data, length);
            break;

        case DATA_TYPE_SCO:
            transmitted_length = h5_int_interface->h5_send_sco_data(type, data, length);
            break;

        default:
            break;
    }

    return transmitted_length;
}

static const hci_hal_t h5_interface = {
    .open          = h5_hal_open,
    .close         = h5_hal_close,
    .transmit_data = h5_transmit_data,
};

const hci_hal_t *hci_hal_h5_get_interface()
{
    return &h5_interface;
}

const hci_hal_t *hci_hal_h5_get_test_interface(vendor_t *vendor_interface)
{
    return &h5_interface;
}
#endif
