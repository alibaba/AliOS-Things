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

#include "hci/hci_layer.h"

typedef struct thread_t thread_t;
typedef struct vendor_t vendor_t;

typedef struct low_power_manager_t {
  // Initialize the low power manager, and use |post_thread| to synchronize actions.
  void (*init)(thread_t *post_thread);

  // Clean up the low power manager and release resources.
  void (*cleanup)(void);

  // Performs |command| synchronized on the thread that was provided
  // at initialization time.
  void (*post_command)(low_power_command_t command);

  // Assert wake (for transmission). Must be called on the thread provided
  // at initialization time.
  void (*wake_assert)(void);

  // Tell the low power manager that you're done transmitting data. Must be
  // called on the thread provided at initialization time.
  void (*transmit_done)(void);
} low_power_manager_t;

const low_power_manager_t *low_power_manager_get_interface();
const low_power_manager_t *low_power_manager_get_test_interface(const vendor_t *vendor_interface);
