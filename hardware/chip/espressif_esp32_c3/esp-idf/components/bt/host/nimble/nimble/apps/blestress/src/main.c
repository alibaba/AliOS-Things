/*
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 */

#include <assert.h>
#include <stdio.h>
#include "os/mynewt.h"
#include "config/config.h"
#include "bsp.h"
#include "hal/hal_gpio.h"

/* BLE */
#include "nimble/ble.h"
#include "host/ble_hs.h"
#include "host/util/util.h"
#include "services/gap/ble_svc_gap.h"

/* Application-specified header. */
#include "rx_stress.h"
#include "tx_stress.h"
#include "stress_gatt.h"

static void
stress_test_on_reset(int reason)
{
    MODLOG_DFLT(ERROR, "Resetting state; reason=%d\n", reason);
}

static void
stress_test_on_sync(void)
{
    int rc;

    /* Make sure we have proper identity address set (public preferred) */
    rc = ble_hs_util_ensure_addr(1);
    assert(rc == 0);

#if MYNEWT_VAL(BLE_STRESS_TEST_ROLE)
    rx_stress_start_auto();
#else
    tx_stress_start_auto();
#endif
}

/**
 * main
 *
 * The main task for the project. This function initializes the packages,
 * then starts serving events from default event queue.
 *
 * @return int NOTE: this function should never return!
 */
int
main(void)
{
    int rc;

    sysinit();

    ble_hs_cfg.reset_cb = stress_test_on_reset;
    ble_hs_cfg.sync_cb = stress_test_on_sync;
    ble_hs_cfg.store_status_cb = ble_store_util_status_rr;

    /* Please do not change name. Otherwise some tests could fail. */
    rc = ble_svc_gap_device_name_set("STRESS");
    assert(rc == 0);

    conf_load();

    rc = gatt_svr_init();
    assert(rc == 0);

#if MYNEWT_VAL(BLE_STRESS_TEST_ROLE)
    /* RX device */
    ble_hs_cfg.gatts_register_cb = gatt_svr_register_cb;
    hal_gpio_init_out(LED_2, 1);
    hal_gpio_toggle(LED_2);
#endif

    while (1) {
        os_eventq_run(os_eventq_dflt_get());
    }
    return 0;
}
