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

#pragma once

#include <stdbool.h>
#include <stdint.h>

#include "stack/bt_types.h"
#include "common/bt_vendor_lib.h"
#include "hci/hci_internals.h"
#include "hci/hci_layer.h"

typedef enum {
  VENDOR_CHIP_POWER_CONTROL   = BT_VND_OP_POWER_CTRL,
  VENDOR_OPEN_USERIAL         = BT_VND_OP_USERIAL_OPEN,
  VENDOR_CLOSE_USERIAL        = BT_VND_OP_USERIAL_CLOSE,
  VENDOR_GET_LPM_IDLE_TIMEOUT = BT_VND_OP_GET_LPM_IDLE_TIMEOUT,
  VENDOR_SET_LPM_WAKE_STATE   = BT_VND_OP_LPM_WAKE_SET_STATE,
  VENDOR_SET_AUDIO_STATE      = BT_VND_OP_SET_AUDIO_STATE
} vendor_opcode_t;

typedef enum {
  VENDOR_CONFIGURE_FIRMWARE   = BT_VND_OP_FW_CFG,
  VENDOR_CONFIGURE_SCO        = BT_VND_OP_SCO_CFG,
  VENDOR_SET_LPM_MODE         = BT_VND_OP_LPM_SET_MODE,
  VENDOR_DO_EPILOG            = BT_VND_OP_EPILOG,
  //VENDOR_A2DP_OFFLOAD_START   = BT_VND_OP_A2DP_OFFLOAD_START,
  //VENDOR_A2DP_OFFLOAD_STOP    = BT_VND_OP_A2DP_OFFLOAD_STOP,
  VENDOR_LAST_OP
} vendor_async_opcode_t;

typedef void (*vendor_cb)(bool success);

typedef struct vendor_t{
  // Opens the vendor-specific library and sets the Bluetooth
  // address of the adapter to |local_bdaddr|. |hci_interface| is
  // used to send commands on behalf of the vendor library.
  bool (*open)(
    const uint8_t *local_bdaddr,
    const hci_t *hci_interface
  );

  // Closes the vendor-specific library and frees all associated resources.
  // Only |vendor_open| may be called after |vendor_close|.
  void (*close)(void);

  // Sends a vendor-specific command to the library.
  int (*send_command)(vendor_opcode_t opcode, void *param);

  // Sends an asynchronous vendor-specific command to the library.
  int (*send_async_command)(vendor_async_opcode_t opcode, void *param);

  // Registers a callback for an asynchronous vendor-specific command.
  void (*set_callback)(vendor_async_opcode_t opcode, vendor_cb callback);
} vendor_t;

const vendor_t *vendor_get_interface();
