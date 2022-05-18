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

#include <host/ble_gap.h>
#include <console/console.h>
#include <host/util/util.h>
#include <host/ble_l2cap.h>
#include "tx_stress.h"

/* Main test task priority. Set a high value so that the task does not
 * interfere with event handling */
#define TX_STRESS_MAIN_TASK_PRIO 0xf0
#define BASE_UUID_LEN 13

/* Contexts for stress tests. */
static struct com_stress_test_ctx tx_stress_ctxD = {
    .conn_handle = 0xffff,
    .cur_test_id = 0,
};

static struct com_stress_test_ctx *tx_stress_ctx;

/* Define stack, object and semaphore for test main task. */
#define TX_STRESS_MAIN_TASK_STACK_SIZE (500)
static struct os_task tx_stress_main_task;
static os_stack_t tx_stress_main_task_stack[TX_STRESS_MAIN_TASK_STACK_SIZE];
static struct os_sem tx_stress_main_sem;
/* Test use case and address of test advertiser. */
static int tx_stress_use_case;
static int completed_tests = 0;

static void
tx_stress_on_test_finish(int test_num)
{
    console_printf("\033[0;32m\nStress test %d completed\033[0m\n", test_num);
    ++completed_tests;
    tx_stress_ctx->completed[test_num] = true;
    os_sem_release(&tx_stress_main_sem);
}

static void
tx_stress_simple_scan(ble_gap_event_fn *cb, uint16_t duration)
{
    uint8_t own_addr_type;
    struct ble_gap_ext_disc_params params = {0};
    int rc;

    /* Figure out address to use while scanning. */
    rc = ble_hs_id_infer_auto(0, &own_addr_type);
    if (rc != 0) {
        console_printf("\033[0;31mError determining own address type; "
                       "rc=%d\033[0m\n", rc);
        assert(0);
    }

    params.itvl = BLE_GAP_SCAN_FAST_INTERVAL_MAX;
    params.passive = 1;
    params.window = BLE_GAP_SCAN_FAST_WINDOW;

    rc = ble_gap_ext_disc(own_addr_type, duration, 0, 1, 0, 0, &params, NULL,
                          cb, NULL);

    if (rc != 0) {
        console_printf("\033[0;31mError initiating GAP discovery procedure"
                       "; rc=%d\033[0m\n", rc);
    }
}

static int
tx_stress_simple_connect(ble_gap_event_fn *cb, int test_num)
{
    uint8_t own_addr_type;
    int rc;

    /* Set so any PHY mask allowed. */
    ble_gap_set_prefered_default_le_phy(TX_PHY_MASK, RX_PHY_MASK);

    /* Figure out address to use while connecting. */
    rc = ble_hs_id_infer_auto(0, &own_addr_type);
    if (rc != 0) {
        MODLOG_DFLT(INFO, "\033[0;31mError determining own address type; "
                          "rc=%d\033[0m\n", rc);
        return rc;
    }

    MODLOG_DFLT(INFO, "Connection attempt: %d\n",
                ++tx_stress_ctx->con_stat[test_num].attempts_num);

    rc = ble_gap_ext_connect(own_addr_type, &tx_stress_ctx->dev_addr,
                             10000,
                             BLE_GAP_LE_PHY_1M_MASK | BLE_GAP_LE_PHY_2M_MASK,
                             NULL, NULL, NULL, cb, NULL);

    if (rc != 0) {
        MODLOG_DFLT(INFO, "\033[0;31mError during connection; rc=%d\033[0m\n",
                    rc);
    }

    return rc;
}

static int
tx_stress_find_test(struct ble_gap_ext_disc_desc *ext_disc)
{
    struct ble_hs_adv_fields fields;
    int data_len;
    int rc;

    /* Parser refuses greater length data than 31. But known UUID128 will be
     * in first 31 bytes of adv data first packet. */
    if (ext_disc->length_data > 31) {
        data_len = 31;
    } else {
        data_len = ext_disc->length_data;
    }

    /* Parse part of adv data. */
    ble_hs_adv_parse_fields(&fields, ext_disc->data, data_len);
    print_adv_fields(&fields);

    /* UUID128 service data of stress test advert includes only UUID128. */
    if (fields.svc_data_uuid128_len != 16) {
        return -1;
    }

    /* Check if service data include known UUID128. */
    rc = memcmp(fields.svc_data_uuid128, (uint8_t[]) {0xC0, 0xDE}, 2);
    if (rc) {
        return -1;
    }

    rc = memcmp(fields.svc_data_uuid128 + 3, MYNEWT_VAL(BLE_STRESS_UUID_BASE),
                BASE_UUID_LEN);

    if (rc != 0) {
        return -1;
    }

    /* This UUID 128 byte indicates the stress test ID to be executed. */
    return fields.svc_data_uuid128[2];
}

static int
tx_stress_switcher_gap_event(struct ble_gap_event *event, void *arg)
{
    int rc;

    switch (event->type) {
    case BLE_GAP_EVENT_CONNECT:
        /* A new connection was established or a connection attempt failed. */
        if (event->connect.status == 0) {
            MODLOG_DFLT(INFO, "Success to connect to device\n");
            return 0;
        } else if (event->connect.status == BLE_HS_ETIMEOUT_HCI) {
            MODLOG_DFLT(INFO, "Connection timeout\n");
        } else {
            MODLOG_DFLT(INFO, "Error: connection attempt failed; status=%d\n",
                        event->connect.status);
        }
        /* Connect to rx device just to give it a signal to switch test. */
        tx_stress_simple_connect(tx_stress_switcher_gap_event,
                                 tx_stress_ctx->cur_test_id);
        return 0;

    case BLE_GAP_EVENT_DISCONNECT:
        os_sem_release(&tx_stress_main_sem);
        return 0;

    case BLE_GAP_EVENT_EXT_DISC:
        /* Check if caught advert contains known UUID128. The UUID128
         * contains the ID of test use case to be executed. */
        rc = tx_stress_find_test(&event->ext_disc);
        if (rc == 0) {
            tx_stress_ctx->dev_addr = event->ext_disc.addr;
            /* Stop scanning. */
            ble_gap_disc_cancel();
            /* Add token to semaphore. Main task will start the test. */
            os_sem_release(&tx_stress_main_sem);
        }
        return 0;

    case BLE_GAP_EVENT_DISC_COMPLETE:
        MODLOG_DFLT(INFO, "Discovery complete; reason=%d\n",
                    event->disc_complete.reason);
        return 0;

    default:
        MODLOG_DFLT(INFO, "Other event occurs=%d\n", event->type);
        return 0;
    }
}

static void
tx_stress_switch_test()
{
    tx_stress_simple_scan(tx_stress_switcher_gap_event, 0);
    os_sem_pend(&tx_stress_main_sem, OS_TIMEOUT_NEVER);

    tx_stress_simple_connect(tx_stress_switcher_gap_event, 0);
}

static int
tx_stress_1_gap_event(struct ble_gap_event *event, void *arg)
{
    switch (event->type) {
    case BLE_GAP_EVENT_CONNECT:
        /* A new connection was established or a connection attempt failed. */
        MODLOG_DFLT(INFO, "Connection %s; status=%d ",
                    event->connect.status == 0 ? "established" : "failed",
                    event->connect.status);

        if (event->connect.status == 0) {
            /* Connection successfully established. In this use case
             * it is error of 'Connect cancel'. Stress test failed. */
            MODLOG_DFLT(INFO, "Success to connect to device\n");
            ++tx_stress_ctx->con_stat[1].num;

            ble_gap_terminate(event->connect.conn_handle, BLE_ERR_NO_PAIRING);
        }
        return 0;

    case BLE_GAP_EVENT_DISCONNECT:
        MODLOG_DFLT(INFO, "Disconnect; reason=%d ", event->disconnect.reason);
        return 0;

    default:
        MODLOG_DFLT(INFO, "Other event occurs=%d\n", event->type);
        return 0;
    }
}

static void
tx_stress_1_test()
{
    int rc;
    uint8_t own_addr_type;
    ble_addr_t rnd_rx_addr;
    int delay_time;

    rc = ble_gap_disc_active();
    assert(rc == 0);

    /* Figure out address to use while advertising. */
    rc = ble_hs_id_infer_auto(0, &own_addr_type);
    if (rc != 0) {
        MODLOG_DFLT(INFO, "\033[0;31mError determining own address type; "
                          "rc=%d\033[0m\n", rc);
        os_sem_release(&tx_stress_main_sem);
        return;
    }

    while (tx_stress_ctx->con_stat[1].attempts_num <
           MYNEWT_VAL(BLE_STRESS_REPEAT)) {
        /* Rand ble address to connect*/
        rc = ble_hs_id_gen_rnd(1, &rnd_rx_addr);
        assert (rc == 0);

        MODLOG_DFLT(INFO, "Connection attempt; num=%d\n",
                    ++tx_stress_ctx->con_stat[1].attempts_num);

        rc = ble_gap_connect(own_addr_type, &rnd_rx_addr, 10000, NULL,
                             tx_stress_1_gap_event, NULL);

        if (rc != 0) {
            MODLOG_DFLT(INFO, "\033[0;31mConnection error; rc=%d\033[0m\n",
                        rc);
            os_sem_release(&tx_stress_main_sem);
            return;
        }

        MODLOG_DFLT(INFO, "Connect cancel\n");
        ble_gap_conn_cancel();
        console_printf("\033[0;32m>\033[0m");
    }

    console_printf(
        "\033[0;32m\nFirst part of test completed\nStart second part: "
        "Connect->random delay->cancel\n\033[0m");

    while (tx_stress_ctx->con_stat[1].attempts_num <
           2 * MYNEWT_VAL(BLE_STRESS_REPEAT)) {
        /* Rand ble address to connect*/
        rc = ble_hs_id_gen_rnd(1, &rnd_rx_addr);
        assert (rc == 0);

        MODLOG_DFLT(INFO, "Connection attempt; num=%d\n",
                    ++tx_stress_ctx->con_stat[1].attempts_num);

        delay_time = rand() % 1000;

        MODLOG_DFLT(INFO, "Time to delay=%d\n", delay_time);

        rc = ble_gap_connect(own_addr_type, &rnd_rx_addr, 10000, NULL,
                             tx_stress_1_gap_event, NULL);

        if (rc != 0) {
            MODLOG_DFLT(INFO, "\033[0;31mConnection error; rc=%d\033[0m\n",
                        rc);
            os_sem_release(&tx_stress_main_sem);
            return;
        }

        os_time_delay(os_time_ms_to_ticks32(delay_time));

        MODLOG_DFLT(INFO, "Connect cancel\n");
        ble_gap_conn_cancel();
        console_printf("\033[0;32m>\033[0m");
    }

    tx_stress_on_test_finish(1);
}

static int
tx_stress_2_gap_event(struct ble_gap_event *event, void *arg)
{
    struct ble_gap_conn_desc desc;
    int rc;

    switch (event->type) {
    case BLE_GAP_EVENT_CONNECT:
        /* A new connection was established or a connection attempt failed. */
        if (event->connect.status == 0) {
            MODLOG_DFLT(INFO, "Success to connect to device\n");

            ++tx_stress_ctx->con_stat[2].num;
            rc = ble_gap_conn_find(event->connect.conn_handle, &desc);
            assert(rc == 0);

            tx_stress_ctx->conn_handle = desc.conn_handle;

            ble_gap_terminate(event->connect.conn_handle,
                              BLE_ERR_REM_USER_CONN_TERM);
        } else {
            MODLOG_DFLT(INFO, "\033[0;31mError: connection attempt failed; "
                              "status=%d\033[0m\n", event->connect.status);
            os_sem_release(&tx_stress_main_sem);
        }
        return 0;

    case BLE_GAP_EVENT_DISCONNECT:
        MODLOG_DFLT(INFO, "Disconnect; reason=%d \n",
                    event->disconnect.reason);
        console_printf("\033[0;32m>\033[0m");

        if (tx_stress_ctx->con_stat[2].num >= MYNEWT_VAL(BLE_STRESS_REPEAT)) {
            tx_stress_on_test_finish(2);
            return 0;
        }
        tx_stress_simple_connect(tx_stress_2_gap_event,
                                 tx_stress_ctx->cur_test_id);
        return 0;

    default:
        MODLOG_DFLT(INFO, "Other event occurs=%d\n", event->type);
        return 0;
    }
}

static int
tx_stress_3_gap_event(struct ble_gap_event *event, void *arg)
{
    int rc;

    switch (event->type) {
    case BLE_GAP_EVENT_CONNECT:
        /* A new connection was established or a connection attempt failed. */
        if (event->connect.status == 0) {
            ++tx_stress_ctx->con_stat[3].num;
            MODLOG_DFLT(INFO, "Success to connect to device\n");

            rc = ble_gap_terminate(event->connect.conn_handle,
                                   BLE_ERR_REM_USER_CONN_TERM);

            MODLOG_DFLT(INFO, "rc=%d\n", rc);
        } else {
            MODLOG_DFLT(INFO, "\033[0;31mError: connection attempt failed; "
                              "status=%d\033[0m\n", event->connect.status);
            os_sem_release(&tx_stress_main_sem);
        }
        return 0;

    case BLE_GAP_EVENT_DISCONNECT:
        MODLOG_DFLT(INFO, "Disconnect; reason=%d \n",
                    event->disconnect.reason);
        console_printf("\033[0;32m>\033[0m");

        if (tx_stress_ctx->con_stat[3].num >= MYNEWT_VAL(BLE_STRESS_REPEAT)) {
            tx_stress_on_test_finish(3);
            return 0;
        }
        tx_stress_simple_connect(tx_stress_3_gap_event,
                                 tx_stress_ctx->cur_test_id);
        return 0;

    default:
        MODLOG_DFLT(INFO, "Other event occurs=%d\n", event->type);
        return 0;
    }
}

static int
tx_stress_4_con_update(void)
{
    int rc;

    /* With every next update at least one param must change. Otherwise no
     * event occurs and test will not be continued */
    struct ble_gap_upd_params params = {
        .itvl_min = BLE_GAP_INITIAL_CONN_ITVL_MIN,
        .itvl_max = BLE_GAP_INITIAL_CONN_ITVL_MAX,
        .latency = BLE_GAP_INITIAL_CONN_LATENCY,
        /* So let's change e.g. timeout value. Put ...% 2 ? 1 : 2 to make sure
         * that value won't grow significantly and will be different with every
         * iteration. */
        .supervision_timeout = BLE_GAP_INITIAL_SUPERVISION_TIMEOUT +
                               (tx_stress_ctx->con_stat[4].prms_upd_num % 2 ?
                                1 : 2),
        .min_ce_len = BLE_GAP_INITIAL_CONN_MIN_CE_LEN,
        .max_ce_len = BLE_GAP_INITIAL_CONN_MAX_CE_LEN,
    };

    rc = ble_gap_update_params(tx_stress_ctx->conn_handle, &params);

    if (rc == BLE_HS_ENOTCONN) {
        MODLOG_DFLT(INFO, "Device disconnected. Connection update failed\n");
        assert(0);
    }

    if (rc != 0) {
        MODLOG_DFLT(ERROR, "\033[0;31mError during connection update; "
                           "rc=%d\033[0m\n", rc);
        assert(0);
    }

    return rc;
}

static int
tx_stress_4_gap_event(struct ble_gap_event *event, void *arg)
{
    int rc;

    switch (event->type) {
    case BLE_GAP_EVENT_CONNECT:
        ++tx_stress_ctx->con_stat[4].attempts_num;
        /* A new connection was established or a connection attempt failed. */
        if (event->connect.status == 0) {
            MODLOG_DFLT(INFO, "Success to connect to device\n");

            ++tx_stress_ctx->con_stat[4].num;
            tx_stress_ctx->conn_handle = event->connect.conn_handle;

            tx_stress_4_con_update();
        } else {
            MODLOG_DFLT(INFO, "\033[0;31mError: connection attempt failed; "
                              "status=%d\033[0m\n", event->connect.status);
            os_sem_release(&tx_stress_main_sem);
        }
        return 0;

    case BLE_GAP_EVENT_DISCONNECT:
        MODLOG_DFLT(INFO, "Disconnect; reason=%d \n",
                    event->disconnect.reason);
        tx_stress_on_test_finish(4);
        return 0;

    case BLE_GAP_EVENT_CONN_UPDATE:
        if (event->conn_update.status != 0) {
            MODLOG_DFLT(INFO, "Connection update failed\n");
        } else {
            MODLOG_DFLT(INFO, "Connection updated; num=%d\n",
                        ++tx_stress_ctx->con_stat[4].prms_upd_num);
            console_printf("\033[0;32m>\033[0m");
        }

        if (tx_stress_ctx->con_stat[4].prms_upd_num >=
            MYNEWT_VAL(BLE_STRESS_REPEAT)) {
            ble_gap_terminate(tx_stress_ctx->conn_handle,
                              BLE_ERR_REM_USER_CONN_TERM);
        } else {
            /* Update connection. */
            rc = tx_stress_4_con_update();

            if (rc != 0) {
                MODLOG_DFLT(INFO, "\033[0;31mError: update fail; "
                                  "rc=%d\033[0m\n", rc);
                os_sem_release(&tx_stress_main_sem);
            }
        }
        return 0;

    case BLE_GAP_EVENT_CONN_UPDATE_REQ:
        MODLOG_DFLT(INFO, "Connection update request\n");
        return 0;

    default:
        MODLOG_DFLT(INFO, "Other event occurs=%d\n", event->type);
        return 0;
    }
}

static int
tx_stress_5_gap_event(struct ble_gap_event *event, void *arg)
{
    switch (event->type) {
    case BLE_GAP_EVENT_CONNECT:
        /* A new connection was established or a connection attempt failed. */
        if (event->connect.status == 0) {
            MODLOG_DFLT(INFO, "Success to connect to device\n");

            ++tx_stress_ctx->con_stat[5].num;
            tx_stress_ctx->conn_handle = event->connect.conn_handle;
        } else {
            console_printf("\033[0;31mError: Update fail; "
                           "status=%d\033[0m\n", event->connect.status);
            os_sem_release(&tx_stress_main_sem);
        }
        return 0;

    case BLE_GAP_EVENT_DISCONNECT:
        MODLOG_DFLT(INFO, "Disconnect; reason=%d \n",
                    event->disconnect.reason);
        tx_stress_on_test_finish(5);
        return 0;

    case BLE_GAP_EVENT_CONN_UPDATE:
        if (event->conn_update.status != 0) {
            MODLOG_DFLT(INFO, "Connection update failed\n");
        } else {
            MODLOG_DFLT(INFO, "Connection updated; num=%d\n",
                        ++tx_stress_ctx->con_stat[5].prms_upd_num);
            console_printf("\033[0;32m>\033[0m");
        }

        if (tx_stress_ctx->con_stat[5].prms_upd_num >=
            MYNEWT_VAL(BLE_STRESS_REPEAT)) {
            ble_gap_terminate(tx_stress_ctx->conn_handle,
                              BLE_ERR_REM_USER_CONN_TERM);
        }
        return 0;

    case BLE_GAP_EVENT_CONN_UPDATE_REQ:
        MODLOG_DFLT(INFO, "Connection update request\n");
        return 0;

    default:
        MODLOG_DFLT(INFO, "Other event occurs=%d\n", event->type);
        return 0;
    }
}

static int
tx_stress_6_gap_event(struct ble_gap_event *event, void *arg)
{
    static int start_id = 0;
    int use_case = 0;
    int adv_pattern_len;
    const uint8_t *adv_pattern;

    switch (event->type) {
    case BLE_GAP_EVENT_EXT_DISC:

        /* Instance 0 reserved for SWITCH advert. */
        if (event->ext_disc.sid == 0) {
            return 0;
        }

        /* Check if advertiser is known rx device. */
        if (memcmp(tx_stress_ctx->dev_addr.val,
                   event->ext_disc.addr.val, 6) != 0) {
            return 0;
        }

        /* Return -1 if not first package of advert. */
        use_case = tx_stress_find_test(&event->ext_disc);

        if (use_case > 0) {
            /* If first package of advert */
            ++tx_stress_ctx->s6_rcv_adv_first;
            start_id = 0;
            adv_pattern = &event->ext_disc.data[29];
            adv_pattern_len = event->ext_disc.length_data - 29;
        } else {
            if (start_id == 0) {
                return 0;
            }
            /* If not first package of advert */
            adv_pattern = event->ext_disc.data;
            adv_pattern_len = event->ext_disc.length_data;
        }

        /* Check data pattern */
        if (memcmp(adv_pattern, test_6_pattern + start_id,
                   adv_pattern_len) != 0) {
            /* Pattern does not match. May lost some data or package.
             * Reset data pattern index. */
            start_id = 0;
            return 0;
        }

        /* At the next adv data package, start comparing from this index.*/
        start_id += adv_pattern_len;

        /* Check if last packet of advert. */
        if (event->ext_disc.data_status ==
            BLE_GAP_EXT_ADV_DATA_STATUS_COMPLETE) {
            /* Got all packets of advert. */
            ++tx_stress_ctx->s6_rcv_adv_suc;
            MODLOG_DFLT(INFO, "Got all packets of advert. num=%d\n",
                        tx_stress_ctx->s6_rcv_adv_suc);
            console_printf("\033[0;32m>\033[0m");
            start_id = 0;

            if (tx_stress_ctx->s6_rcv_adv_suc >=
                MYNEWT_VAL(BLE_STRESS_REPEAT)) {
                /* Stop scanning. */
                ble_gap_disc_cancel();
                tx_stress_on_test_finish(6);
            }
        }
        return 0;

    case BLE_GAP_EVENT_DISC_COMPLETE:
        MODLOG_DFLT(INFO, "Discovery complete; reason=%d\n",
                    event->disc_complete.reason);
        return 0;

    default:
        MODLOG_DFLT(INFO, "Other event occurs=%d\n", event->type);
        return 0;
    }
}

static void
tx_stress_6_perform(void)
{
    tx_stress_simple_scan(tx_stress_6_gap_event, 0);
    os_sem_pend(&tx_stress_main_sem, OS_TIMEOUT_NEVER);
    tx_stress_switch_test();
}

static int
tx_stress_7_phy_update(void)
{
    int rc;
    uint8_t tx_phys_mask;
    uint8_t rx_phys_mask;

    ble_gap_read_le_phy(tx_stress_ctx->conn_handle, &tx_phys_mask,
                        &rx_phys_mask);


    /* With every next update at least one param must change */
    switch (rx_phys_mask) {
    case BLE_GAP_LE_PHY_1M_MASK:
        rx_phys_mask = BLE_GAP_LE_PHY_2M_MASK;
        break;
    case BLE_GAP_LE_PHY_2M_MASK:
#if MYNEWT_VAL(BLE_LL_CFG_FEAT_LE_CODED_PHY)
        rx_phys_mask = BLE_GAP_LE_PHY_CODED_MASK;
        break;
    case BLE_GAP_LE_PHY_CODED_MASK:
#endif
        rx_phys_mask = BLE_GAP_LE_PHY_1M_MASK;
        break;
    default:
        rx_phys_mask = BLE_GAP_LE_PHY_1M_MASK;
        break;
    }

    switch (tx_phys_mask) {
    case BLE_GAP_LE_PHY_1M_MASK:
        tx_phys_mask = BLE_GAP_LE_PHY_2M_MASK;
        break;
    case BLE_GAP_LE_PHY_2M_MASK:
#if MYNEWT_VAL(BLE_LL_CFG_FEAT_LE_CODED_PHY)
        tx_phys_mask = BLE_GAP_LE_PHY_CODED_MASK;
        break;
    case BLE_GAP_LE_PHY_CODED_MASK:
#endif
        tx_phys_mask = BLE_GAP_LE_PHY_1M_MASK;
        break;
    default:
        tx_phys_mask = BLE_GAP_LE_PHY_1M_MASK;
        break;
    }

    rc = ble_gap_set_prefered_le_phy(tx_stress_ctx->conn_handle,
                                     tx_phys_mask, rx_phys_mask, 0);

    if (rc == BLE_HS_ENOTCONN) {
        MODLOG_DFLT(INFO, "Device disconnected. Connection update failed\n");
        return rc;
    }

    if (rc != 0) {
        MODLOG_DFLT(ERROR, "\033[0;31mError during PHY update; "
                           "rc=%d\033[0m\n", rc);
    }

    return rc;
}

static int
tx_stress_7_gap_event(struct ble_gap_event *event, void *arg)
{
    int rc;

    switch (event->type) {
    case BLE_GAP_EVENT_CONNECT:
        /* A new connection was established or a connection attempt failed. */
        if (event->connect.status == 0) {
            MODLOG_DFLT(INFO, "Success to connect to device\n");

            ++tx_stress_ctx->con_stat[7].num;
            tx_stress_ctx->conn_handle = event->connect.conn_handle;

            tx_stress_7_phy_update();
        } else {
            console_printf("\033[0;31mError: Update fail; "
                           "status=%d\033[0m\n", event->connect.status);
            os_sem_release(&tx_stress_main_sem);
        }
        return 0;

    case BLE_GAP_EVENT_DISCONNECT:
        MODLOG_DFLT(INFO, "Disconnect; reason=%d \n",
                    event->disconnect.reason);
        tx_stress_on_test_finish(7);
        return 0;

    case BLE_GAP_EVENT_CONN_UPDATE:
        MODLOG_DFLT(INFO, "Connection updated\n");
        return 0;

    case BLE_GAP_EVENT_CONN_UPDATE_REQ:
        MODLOG_DFLT(INFO, "Connection update request\n");
        return 0;

    case BLE_GAP_EVENT_PHY_UPDATE_COMPLETE:
        if (event->phy_updated.status != 0) {
            MODLOG_DFLT(INFO, "PHY update failed\n");
        } else {
            MODLOG_DFLT(INFO, "PHY updated; num=%d; rx:%d, tx:%d\n",
                        ++tx_stress_ctx->con_stat[7].phy_upd_num,
                        event->phy_updated.rx_phy, event->phy_updated.tx_phy);
            console_printf("\033[0;32m>\033[0m");
        }

        if (tx_stress_ctx->con_stat[7].phy_upd_num >=
            MYNEWT_VAL(BLE_STRESS_REPEAT)) {
            ble_gap_terminate(tx_stress_ctx->conn_handle,
                              BLE_ERR_REM_USER_CONN_TERM);
        } else {
            /* Update connection. */
            rc = tx_stress_7_phy_update();
            if (rc != 0) {
                console_printf("\033[0;31mError: PHPY update fail; "
                               "rc=%d\033[0m\n", event->phy_updated.status);
                assert(0);
            }
        }
        return 0;

    default:
        MODLOG_DFLT(INFO, "Other event occurs=%d\n", event->type);
        return 0;
    }
}

static int
tx_stress_8_gap_event(struct ble_gap_event *event, void *arg)
{
    switch (event->type) {
    case BLE_GAP_EVENT_CONNECT:
        /* A new connection was established or a connection attempt failed. */
        if (event->connect.status == 0) {
            MODLOG_DFLT(INFO, "Success to connect to device\n");

            ++tx_stress_ctx->con_stat[8].num;
            tx_stress_ctx->conn_handle = event->connect.conn_handle;
        } else {
            MODLOG_DFLT(INFO, "\033[0;31mError: connection attempt failed; "
                              "status=%d\033[0m\n", event->connect.status);
            os_sem_release(&tx_stress_main_sem);
        }
        return 0;

    case BLE_GAP_EVENT_DISCONNECT:
        MODLOG_DFLT(INFO, "Disconnect; reason=%d \n",
                    event->disconnect.reason);
        tx_stress_on_test_finish(8);
        return 0;

    case BLE_GAP_EVENT_CONN_UPDATE:
        if (event->conn_update.status != 0) {
            MODLOG_DFLT(INFO, "Connection update failed\n");
        } else {
            MODLOG_DFLT(INFO, "Connection updated; num=%d\n",
                        ++tx_stress_ctx->con_stat[8].prms_upd_num);
        }
        return 0;

    case BLE_GAP_EVENT_CONN_UPDATE_REQ:
        MODLOG_DFLT(INFO, "Connection update request\n");
        return 0;

    case BLE_GAP_EVENT_PHY_UPDATE_COMPLETE:
        if (event->phy_updated.status != 0) {
            MODLOG_DFLT(INFO, "PHY update failed\n");
        } else {
            MODLOG_DFLT(INFO, "PHY updated; num=%d\n",
                        ++tx_stress_ctx->con_stat[8].phy_upd_num);
            console_printf("\033[0;32m>\033[0m");
        }

        if (tx_stress_ctx->con_stat[8].phy_upd_num >=
            MYNEWT_VAL(BLE_STRESS_REPEAT)) {
            ble_gap_terminate(tx_stress_ctx->conn_handle,
                              BLE_ERR_REM_USER_CONN_TERM);
        }
        return 0;

    default:
        MODLOG_DFLT(INFO, "Other event occurs=%d\n", event->type);
        return 0;
    }
}

static int
tx_stress_9_gap_event(struct ble_gap_event *event, void *arg)
{
    ble_addr_t addr;
    int test;

    switch (event->type) {
    case BLE_GAP_EVENT_EXT_DISC:
        /* Looking for next instance of test 9 advert. */
        test = tx_stress_find_test(&event->ext_disc);
        /* To avoid messing by rest of test 9 events in queue, check if handle
         * filled */
        if (test == 9 && tx_stress_ctx->conn_handle == 0xffff) {
            tx_stress_ctx->conn_handle = 0;
            ble_gap_disc_cancel();
            tx_stress_ctx->dev_addr = event->ext_disc.addr;
            tx_stress_simple_connect(tx_stress_9_gap_event,
                                     tx_stress_ctx->cur_test_id);
        }
        return 0;

    case BLE_GAP_EVENT_DISC_COMPLETE:
        if (event->disc_complete.reason == 0 && !tx_stress_ctx->completed[9]) {
            console_printf("\033[0;31mScanning timeout\033[0m");
            tx_stress_ctx->completed[9] = true;
            os_sem_release(&tx_stress_main_sem);
            return 0;
        }
        break;

    case BLE_GAP_EVENT_CONNECT:
        /* A new connection was established or a connection attempt failed. */
        if (event->connect.status == 0) {
            MODLOG_DFLT(INFO, "Success to connect to device\n");
            MODLOG_DFLT(INFO, "Connections num: %d\n",
                        ++tx_stress_ctx->con_stat[9].num);
            console_printf("\033[0;32m>\033[0m");
            /* Remember max number of handled connections */
            if (tx_stress_ctx->con_stat[9].num >
                tx_stress_ctx->con_stat[9].max_num) {
                tx_stress_ctx->con_stat[9].max_num = tx_stress_ctx->con_stat[9].num;
            }
        } else {
            MODLOG_DFLT(INFO, "\033[0;31mError: connection attempt failed; "
                              "status=%d\033[0m\n", event->connect.status);
        }
        break;

    case BLE_GAP_EVENT_DISCONNECT:
        MODLOG_DFLT(INFO, "Disconnect; reason=%d ", event->disconnect.reason);
        console_printf("\033[0;31mX\033[0m");
        MODLOG_DFLT(INFO, "Connections num: %d\n",
                    --tx_stress_ctx->con_stat[9].num);

        if (tx_stress_ctx->con_stat[9].num == 0) {
            os_sem_release(&tx_stress_main_sem);
            return 0;
        }
        break;

    default:
        MODLOG_DFLT(INFO, "Other event occurs=%d\n", event->type);
        return 0;
    }

    if (tx_stress_ctx->completed[9]) {
        return 0;
    }

    /* End use case after specified number of scan times or max config number
     * of connections */
    if (tx_stress_ctx->con_stat[9].attempts_num <
        MYNEWT_VAL(BLE_STRESS_REPEAT) &&
        tx_stress_ctx->con_stat[9].max_num < MYNEWT_VAL(BLE_MAX_CONNECTIONS)) {
        if (ble_gap_disc_active() == 0) {
            /* Generate and set new random address */
            ble_hs_id_gen_rnd(0, &addr);
            ble_hs_id_set_rnd(addr.val);
            tx_stress_ctx->conn_handle = 0xffff;
            /* Scan for next instance of the test. */
            tx_stress_simple_scan(tx_stress_9_gap_event, 2000);
        }
    } else {
        tx_stress_ctx->completed[9] = true;
        os_sem_release(&tx_stress_main_sem);
    }
    return 0;
}

static void
tx_stress_9_perform()
{
    int i, rc;

    /* Scan for next instance of the test. */
    tx_stress_simple_scan(tx_stress_9_gap_event, 2000);

    os_sem_pend(&tx_stress_main_sem, OS_TIMEOUT_NEVER);

    /* On use case finishing terminate all handled connections */
    for (i = 0; i <= MYNEWT_VAL(BLE_MAX_CONNECTIONS); ++i) {
        rc = ble_gap_conn_find(i, NULL);
        if (rc == 0) {
            MODLOG_DFLT(INFO, "Terminating...\n");
            ble_gap_terminate(i, BLE_ERR_REM_USER_CONN_TERM);
        }
    }

    os_sem_pend(&tx_stress_main_sem, OS_TIMEOUT_NEVER);
    tx_stress_on_test_finish(9);
}

static int
tx_stress_10_l2cap_send(struct ble_l2cap_chan *chan, uint8_t *data,
                        int data_len)
{
    struct os_mbuf *data_buf;
    int rc;

    /* Get mbuf for adv data */
    data_buf = os_msys_get_pkthdr(data_len, 0);
    assert(data != NULL);

    /* Fill mbuf with pattern data */
    rc = os_mbuf_append(data_buf, data, data_len);

    if (rc) {
        os_mbuf_free_chain(data_buf);
        assert(0);
    }

    /* Send data with L2CAP */
    rc = ble_l2cap_send(chan, data_buf);

    return rc;
}

void tx_stress_10_timer_ev_cb(struct os_event *ev)
{
    assert(ev != NULL);

    if (tx_stress_ctx->rcv_data_flag) {
        return;
    }

    tx_stress_ctx->timeout_flag = true;
    MODLOG_DFLT(INFO, "L2CAP receiving timeout\n");
    ble_gap_terminate(tx_stress_ctx->conn_handle,
                      BLE_ERR_REM_USER_CONN_TERM);
}

static void
tx_stress_10_l2cap_send_req()
{
    int rc;
    /* Send a request to the RX device */

    MODLOG_DFLT(INFO, "L2CAP sending request\n");
    tx_stress_ctx->timeout_flag = false;
    tx_stress_ctx->rcv_data_flag = false;
    stress_start_timer(7000, tx_stress_10_timer_ev_cb);

    /* Get the sending begin time */
    tx_stress_ctx->begin_us = os_get_uptime_usec();

    /* Send anything just to give a signal to start sending data
     * by RX device */
    rc = tx_stress_10_l2cap_send(tx_stress_ctx->chan, (uint8_t *) "S",
                                 sizeof("S"));
    assert(rc == 0);
}

static int
tx_stress_10_l2cap_event(struct ble_l2cap_event *event, void *arg)
{
    static int i = 0;
    int64_t us = 0;
    struct ble_l2cap_chan_info chan_info;

    switch (event->type) {
    case BLE_L2CAP_EVENT_COC_CONNECTED:
        /* A new L2CAP connection was established. */
        if (event->connect.status == 0) {
            MODLOG_DFLT(INFO, "Established L2CAP connection\n");
            tx_stress_ctx->chan = event->connect.chan;

            ble_l2cap_get_chan_info(event->connect.chan, &chan_info);

            MODLOG_DFLT(INFO,
                        "LE COC connected, conn: %d, chan: 0x%08lx, scid: 0x%04x, "
                        "dcid: 0x%04x, our_mtu: 0x%04x, peer_mtu: 0x%04x\n",
                        event->connect.conn_handle,
                        (uint32_t) event->connect.chan,
                        chan_info.scid,
                        chan_info.dcid,
                        chan_info.our_l2cap_mtu,
                        chan_info.peer_l2cap_mtu);

            tx_stress_10_l2cap_send_req();
        }
        return 0;

    case BLE_L2CAP_EVENT_COC_DISCONNECTED:
        MODLOG_DFLT(INFO, "Remote device disconnected from L2CAP server\n");
        return 0;

    case BLE_L2CAP_EVENT_COC_ACCEPT:
        stress_l2cap_coc_accept(event->accept.peer_sdu_size,
                                event->accept.chan);
        return 0;

    case BLE_L2CAP_EVENT_COC_DATA_RECEIVED:
        /* Get the time of data receive */
        tx_stress_ctx->end_us = os_get_uptime_usec();

        /* And test after timeout */
        if (tx_stress_ctx->timeout_flag) {
            ble_gap_terminate(tx_stress_ctx->conn_handle,
                              BLE_ERR_REM_USER_CONN_TERM);
            return 0;
        }

        tx_stress_ctx->rcv_data_flag = true;

        stress_l2cap_coc_recv(event->receive.chan, event->receive.sdu_rx);

        /* Time of data sending */
        us = tx_stress_ctx->end_us - tx_stress_ctx->begin_us;
        MODLOG_DFLT(INFO, "Time of receiving L2CAP data: %ld \n",
                    tx_stress_ctx->end_us);

        /* Remember size of entire mbuf chain */
        tx_stress_ctx->rcv_data_bytes = OS_MBUF_PKTLEN(
            event->receive.sdu_rx);
        MODLOG_DFLT(INFO, "Num of received bytes: %lld\n",
                    tx_stress_ctx->rcv_data_bytes);

        /* Calculate the bit rate of this send */
        tx_stress_ctx->s10_bit_rate =
            stress_calc_bit_rate(us, tx_stress_ctx->rcv_data_bytes);
        MODLOG_DFLT(INFO, "Bit rate: %d B/s\n", tx_stress_ctx->s10_bit_rate);

        /* Remember the sum of bytes and the time to calculate the average
         * bit rate. */
        tx_stress_ctx->bytes_sum += tx_stress_ctx->rcv_data_bytes;
        tx_stress_ctx->time_sum += us;

        /* Remember max received MTU */
        if (tx_stress_ctx->s10_max_mtu < tx_stress_ctx->rcv_data_bytes) {
            tx_stress_ctx->s10_max_mtu = tx_stress_ctx->rcv_data_bytes;
        }
        console_printf("\033[0;32m>\033[0m");
        MODLOG_DFLT(INFO, "Loop nr: %d\n", ++i);

        tx_stress_10_l2cap_send_req();
        return 0;

    case BLE_L2CAP_EVENT_COC_TX_UNSTALLED:
        MODLOG_DFLT(INFO, "L2CAP event unstalled\n");
        return 0;

    default:
        MODLOG_DFLT(INFO, "Other L2CAP event occurs; rc=%d\n", event->type);
        return 0;
    }
}

static int
tx_stress_10_gap_event(struct ble_gap_event *event, void *arg)
{
    int rc;
    struct os_mbuf *sdu_rx;

    switch (event->type) {
    case BLE_GAP_EVENT_CONNECT:
        /* A new connection was established or a connection attempt failed. */
        if (event->connect.status == 0) {
            MODLOG_DFLT(INFO, "Success to connect to device; num: %d\n",
                        ++tx_stress_ctx->con_stat[10].num);

            sdu_rx = os_msys_get_pkthdr(STRESS_COC_MTU, 0);
            assert(sdu_rx != NULL);

            tx_stress_ctx->conn_handle = event->connect.conn_handle;
            rc = ble_l2cap_connect(event->connect.conn_handle, 1,
                                   STRESS_COC_MTU, sdu_rx,
                                   tx_stress_10_l2cap_event, NULL);
            assert(rc == 0);
        } else {
            MODLOG_DFLT(INFO, "\033[0;31mError: connection attempt failed; "
                              "status=%d\033[0m\n", event->connect.status);
        }
        return 0;

    case BLE_GAP_EVENT_DISCONNECT:
        tx_stress_ctx->s10_bit_rate = 1000000 * tx_stress_ctx->bytes_sum /
                                      tx_stress_ctx->time_sum;

        MODLOG_DFLT(INFO, "Average bit rate: %d B/s\n",
                    tx_stress_ctx->s10_bit_rate);
        tx_stress_on_test_finish(10);
        return 0;

    default:
        MODLOG_DFLT(INFO, "Other event occurs=%d\n", event->type);
        return 0;
    }
}

static int
tx_stress_11_gap_event(struct ble_gap_event *event, void *arg)
{
    int test;

    switch (event->type) {
    case BLE_GAP_EVENT_EXT_DISC:
        /* Looking for next instance of test 9 advert. */
        test = tx_stress_find_test(&event->ext_disc);

        /* To avoid messing by rest of test 9 events in queue, check if handle
         * filled */
        if (test == 11 && tx_stress_ctx->conn_handle == 0xffff) {
            tx_stress_ctx->conn_handle = 0;
            ble_gap_disc_cancel();
            tx_stress_ctx->dev_addr = event->ext_disc.addr;
            tx_stress_simple_connect(tx_stress_11_gap_event,
                                     tx_stress_ctx->cur_test_id);
        }
        return 0;

    case BLE_GAP_EVENT_CONNECT:
        /* A new connection was established or a connection attempt failed. */
        if (event->connect.status == 0) {
            ++tx_stress_ctx->con_stat[11].num;
            MODLOG_DFLT(INFO, "Success to connect to device\n");
        } else {
            MODLOG_DFLT(INFO, "\033[0;31mError: connection attempt failed; "
                              "status=%d\033[0m\n", event->connect.status);
            break;
        }
        return 0;

    case BLE_GAP_EVENT_DISCONNECT:
        MODLOG_DFLT(INFO, "Disconnect; reason=%d \n",
                    event->disconnect.reason);
        console_printf("\033[0;32m>\033[0m");

        if (tx_stress_ctx->con_stat[11].num >= MYNEWT_VAL(BLE_STRESS_REPEAT)) {
            tx_stress_on_test_finish(11);
            return 0;
        }
        break;

    default:
        MODLOG_DFLT(INFO, "Other event occurs=%d\n", event->type);
        return 0;
    }

    tx_stress_ctx->conn_handle = 0xffff;

    /* Scan for next instance of the test. */
    tx_stress_simple_scan(tx_stress_11_gap_event, 750);

    return 0;
}

static int
tx_stress_12_gap_event(struct ble_gap_event *event, void *arg)
{
    switch (event->type) {
    case BLE_GAP_EVENT_CONNECT:
        /* A new connection was established or a connection attempt failed. */
        if (event->connect.status == 0) {
            ++tx_stress_ctx->con_stat[12].num;
            MODLOG_DFLT(INFO, "Success to connect to device\n");
            tx_stress_ctx->conn_handle = event->connect.conn_handle;
        } else {
            MODLOG_DFLT(INFO, "\033[0;31mError: connection attempt failed; "
                              "status=%d\033[0m\n", event->connect.status);
        }
        return 0;

    case BLE_GAP_EVENT_DISCONNECT:
        MODLOG_DFLT(INFO, "Disconnect; reason=%d \n",
                    event->disconnect.reason);
        /* Finish test after first disconnection */
        tx_stress_on_test_finish(12);
        return 0;

    case BLE_GAP_EVENT_NOTIFY_RX:
        /* Received indication */
        MODLOG_DFLT(INFO, "Notify RX event\n");
        console_printf("\033[0;32m>\033[0m");
        return 0;

    default:
        MODLOG_DFLT(INFO, "Other event occurs=%d\n", event->type);
        return 0;
    }
}

static int
tx_stress_13_gap_event(struct ble_gap_event *event, void *arg)
{
    switch (event->type) {
    case BLE_GAP_EVENT_CONNECT:
        /* A new connection was established or a connection attempt failed. */
        if (event->connect.status == 0) {
            ++tx_stress_ctx->con_stat[13].num;
            MODLOG_DFLT(INFO, "Success to connect to device\n");
            tx_stress_ctx->conn_handle = event->connect.conn_handle;
        } else {
            MODLOG_DFLT(INFO, "\033[0;31mError: connection attempt failed; "
                              "status=%d\033[0m\n", event->connect.status);
            assert(0);
        }
        return 0;

    case BLE_GAP_EVENT_DISCONNECT:
        MODLOG_DFLT(INFO, "Disconnect; reason=%d \n",
                    event->disconnect.reason);
        /* Finish test after disconnection */
        tx_stress_on_test_finish(13);
        return 0;

    case BLE_GAP_EVENT_NOTIFY_RX:
        MODLOG_DFLT(INFO, "Notify RX event\n");
        console_printf("\033[0;32m>\033[0m");
        os_mbuf_free_chain(event->notify_rx.om);
        ++tx_stress_ctx->rcv_num;
        return 0;

    default:
        MODLOG_DFLT(INFO, "Other event occurs=%d\n", event->type);
        return 0;
    }
}

static int
tx_stress_14_subs_cb(uint16_t conn_handle, const struct ble_gatt_error *error,
                     struct ble_gatt_attr *attr, void *arg)
{
    struct os_mbuf *om;
    bool *sub;
    int rc;

    assert(error->status == 0);

    /* If the first subscription after finding cccd */
    if(arg == NULL) {
        return 0;
    }

    sub = (bool*)arg;

    /* Enable notifications */
    if(*sub == 0) {
        *sub = true;
        om = ble_hs_mbuf_from_flat(
            (uint8_t[]) {0x01, 0x00}, 2);

        tx_stress_ctx->begin_us = tx_stress_ctx->end_us;

        rc = ble_gattc_write(tx_stress_ctx->conn_handle,
                             tx_stress_ctx->dsc_handle, om,
                             tx_stress_14_subs_cb, arg);
        assert(rc == 0);
    }

    return 0;
}

static void
tx_stress_14_disc_cccd_fn(struct stress_gatt_search_ctx *search_ctx)
{
    int rc;
    struct os_mbuf *om;
    MODLOG_DFLT(INFO, "CCCD found\n");

    /* Enable notifications */
    om = ble_hs_mbuf_from_flat((uint8_t[]) {0x01, 0x00}, 2);
    tx_stress_ctx->begin_us = os_get_uptime_usec();
    tx_stress_ctx->dsc_handle = search_ctx->dsc_handle;

    rc = ble_gattc_write(tx_stress_ctx->conn_handle,
                         tx_stress_ctx->dsc_handle, om,
                         tx_stress_14_subs_cb, NULL);
    assert(rc == 0);
}

static int
tx_stress_14_gap_event(struct ble_gap_event *event, void *arg)
{
    int64_t us = 0;
    struct os_mbuf *om;
    int rc;
    static bool subscribed = true;

    switch (event->type) {
    case BLE_GAP_EVENT_CONNECT:
        /* A new connection was established or a connection attempt failed. */
        if (event->connect.status == 0) {
            ++tx_stress_ctx->con_stat[14].num;
            MODLOG_DFLT(INFO, "Success to connect to device\n");
            tx_stress_ctx->conn_handle = event->connect.conn_handle;

            /* Find CCCD handle (with default UUID16 = 0x2902) */
            stress_find_dsc_handle(event->connect.conn_handle,
                                   BLE_UUID16_DECLARE(STRESS_GATT_UUID),
                                   BLE_UUID16_DECLARE(STRESS_GATT_NOTIFY_UUID),
                                   BLE_UUID16_DECLARE(0x2902),
                                   &tx_stress_14_disc_cccd_fn);
        } else {
            MODLOG_DFLT(INFO, "\033[0;31mError: connection attempt failed; "
                              "status=%d\033[0m\n", event->connect.status);
            assert(0);
        }
        return 0;

    case BLE_GAP_EVENT_DISCONNECT:
        MODLOG_DFLT(INFO, "Disconnect; reason=%d \n",
                    event->disconnect.reason);
        /* Calc average notifying time */
        if (tx_stress_ctx->rcv_num > 0) {
            tx_stress_ctx->s14_notif_time = tx_stress_ctx->time_sum /
                                            tx_stress_ctx->rcv_num;
        }
        MODLOG_DFLT(INFO, "Average notification time: %d\n",
                    tx_stress_ctx->s14_notif_time);
        /* Finish test after first disconnection */
        tx_stress_on_test_finish(14);
        return 0;

    case BLE_GAP_EVENT_NOTIFY_RX:
        tx_stress_ctx->end_us = os_get_uptime_usec();
        MODLOG_DFLT(INFO, "Notify RX event\n");

        /* Time of data sending */
        us = tx_stress_ctx->end_us - tx_stress_ctx->begin_us;
        MODLOG_DFLT(INFO, "Notification time: %lld\n us", us);

        tx_stress_ctx->time_sum += us;
        console_printf("\033[0;32m>\033[0m");

        /* Perform use case specified number of times */
        if (++tx_stress_ctx->rcv_num >= MYNEWT_VAL(BLE_STRESS_REPEAT)) {
            rc = ble_gap_terminate(event->notify_rx.conn_handle,
                                   BLE_ERR_REM_USER_CONN_TERM);
            MODLOG_DFLT(INFO, "rc=%d\n");
            assert(rc == 0);
            return 0;
        }

        /* Disable notifications */
        subscribed = false;
        om = ble_hs_mbuf_from_flat(
            (uint8_t[]) {0x00, 0x00}, 2);

        rc = ble_gattc_write(tx_stress_ctx->conn_handle,
                             tx_stress_ctx->dsc_handle, om,
                             tx_stress_14_subs_cb, &subscribed);
        assert(rc == 0);
        return 0;

    default:
        MODLOG_DFLT(INFO, "Other event occurs=%d\n", event->type);
        return 0;
    }
}

static int
tx_stress_15_write_cb(uint16_t conn_handle, const struct ble_gatt_error *error,
                     struct ble_gatt_attr *attr, void *arg)
{
    /* Disconnect */
    ble_gap_terminate(conn_handle, BLE_ERR_REM_USER_CONN_TERM);
    console_printf("\033[0;32m>\033[0m");
    return 0;
}

static void
tx_stress_15_disc_chr_fn(struct stress_gatt_search_ctx *search_ctx)
{
    int rc;
    struct os_mbuf *om;

    /* Send some data */
    MODLOG_DFLT(INFO, "Write to chr\n");
    om = ble_hs_mbuf_from_flat(test_6_pattern, 20);

    rc = ble_gattc_write(tx_stress_ctx->conn_handle,
                         search_ctx->chr_start_handle, om,
                         tx_stress_15_write_cb, NULL);
    assert(rc == 0);
}

static int
tx_stress_15_gap_event(struct ble_gap_event *event, void *arg)
{
    switch (event->type) {
    case BLE_GAP_EVENT_CONNECT:
        /* A new connection was established or a connection attempt failed. */
        if (event->connect.status == 0) {
            MODLOG_DFLT(INFO, "Success to connect to device; num: %d\n",
                        ++tx_stress_ctx->con_stat[15].num);
            tx_stress_ctx->conn_handle = event->connect.conn_handle;

            /* Find characteristic handle */
            stress_find_chr_handle(event->connect.conn_handle,
                                   BLE_UUID16_DECLARE(STRESS_GATT_UUID),
                                   BLE_UUID16_DECLARE(STRESS_GATT_WRITE_UUID),
                                   &tx_stress_15_disc_chr_fn);
        } else {
            MODLOG_DFLT(INFO, "\033[0;31mError: connection attempt failed; "
                              "status=%d\033[0m\n", event->connect.status);
            assert(0);
        }
        return 0;

    case BLE_GAP_EVENT_DISCONNECT:
        /* Perform use case specified number of times */
        if(tx_stress_ctx->con_stat[15].num >= MYNEWT_VAL(BLE_STRESS_REPEAT)) {
            tx_stress_on_test_finish(15);
            return 0;
        }
        /* Reconnect */
        tx_stress_simple_connect(tx_stress_15_gap_event, 15);
        return 0;

    default:
    MODLOG_DFLT(INFO, "Other event occurs=%d\n", event->type);
        return 0;
    }
}

static int
scan_for_test_gap_event(struct ble_gap_event *event, void *arg)
{
    int use_case;
    int rc;

    switch (event->type) {
    case BLE_GAP_EVENT_EXT_DISC:
        /* Check if caught advert contains known UUID128. The UUID128
         * contains the ID of test use case to be executed. */
        use_case = tx_stress_find_test(&event->ext_disc);
        if (use_case > 0) {
            rc = ble_gap_disc_cancel();
            tx_stress_ctx->dev_addr = event->ext_disc.addr;

            /* After discovery cancel there are still some events in queue. */
            if (rc == 0) {
                tx_stress_use_case = use_case;
                /* Add token to semaphore. Main task will start the test. */
                os_sem_release(&tx_stress_main_sem);
            }
        }
        return 0;

    case BLE_GAP_EVENT_DISC_COMPLETE:
        /* On timeout */
        tx_stress_ctx->scan_timeout = true;
        console_printf("\033[1;36mDiscover complete\033[0m\n");
        os_sem_release(&tx_stress_main_sem);
        return 0;

    default:
        MODLOG_DFLT(INFO, "Other event occurs=%d\n", event->type);
        return 0;
    }
}

static void
tx_stress_test_perform(int test_num)
{
    /* Perform every test only once */
//    if (test_num <= 0 || tx_stress_ctx->completed[test_num] == true) {
//        return;
//    }

    tx_stress_ctx->cur_test_id = test_num;
    tx_stress_ctx->completed[test_num] = false;
    tx_stress_ctx->conn_handle = 0xffff;

    console_printf("\033[1;36mStart test num %d - ", test_num);

    /* Start test */
    switch (test_num) {
    case 0:
        return;
    case 1:
        console_printf("Stress Connect -> Connect Cancel\033[0m\n");
        tx_stress_1_test();
        break;
    case 2:
        console_printf("Stress Connect/Disconnect legacy\033[0m\n");
        tx_stress_simple_connect(&tx_stress_2_gap_event, 2);
        break;
    case 3:
        console_printf("Stress Connect/Disconnect ext adv\033[0m\n");
        tx_stress_simple_connect(&tx_stress_3_gap_event, 3);
        break;
    case 4:
        console_printf("Stress connection params update (TX)\033[0m\n");
        tx_stress_simple_connect(&tx_stress_4_gap_event, 4);
        break;
    case 5:
        console_printf("Stress connection params update (RX)\033[0m\n");
        tx_stress_simple_connect(&tx_stress_5_gap_event, 5);
        break;
    case 6:
        console_printf("Stress Scan\033[0m\n");
        tx_stress_6_perform();
        break;
    case 7:
        console_printf("Stress PHY Update (TX)\033[0m\n");
        tx_stress_simple_connect(&tx_stress_7_gap_event, 7);
        break;
    case 8:
        console_printf("Stress PHY Update (RX)\033[0m\n");
        tx_stress_simple_connect(&tx_stress_8_gap_event, 8);
        break;
    case 9:
        console_printf("Stress multi connection\033[0m\n");
        tx_stress_9_perform();
        break;
    case 10:
        console_printf("Stress L2CAP send\033[0m\n");
        tx_stress_simple_connect(&tx_stress_10_gap_event, 10);
        break;
    case 11:
        console_printf("Stress Advertise/Connect/Disconnect\033[0m\n");
        tx_stress_simple_connect(&tx_stress_11_gap_event, 11);
        break;
    case 12:
        console_printf("Stress GATT indication\033[0m\n");
        tx_stress_simple_connect(&tx_stress_12_gap_event, 12);
        break;
    case 13:
        console_printf("Stress GATT notification\033[0m\n");
        tx_stress_simple_connect(&tx_stress_13_gap_event, 13);
        break;
    case 14:
        console_printf("Stress GATT Subscribe/Notify/Unsubscribe\033[0m\n");
        tx_stress_simple_connect(&tx_stress_14_gap_event, 14);
        break;
    case 15:
        console_printf("Stress Connect/Send/Disconnect\033[0m\n");
        tx_stress_simple_connect(&tx_stress_15_gap_event, 15);
        break;
    default:
        console_printf("\033[0;31mFound test, but do not know how to perform."
                       "\033[0m\n");
        assert(0);
    }

    /* Wait for the test to finish. Then 1 token will be released
    * allowing to pass through semaphore. */
    os_sem_pend(&tx_stress_main_sem, OS_TIMEOUT_NEVER);

    stress_clear_ctx_reusable_var(tx_stress_ctx);
}

static void
tx_stress_read_command_cb(void) {
    console_printf("Start testing\n");
    os_sem_release(&tx_stress_main_sem);
}

static void
tx_stress_main_task_fn(void *arg)
{
    int rc;

    tx_stress_ctx = &tx_stress_ctxD;

    console_printf("\033[1;36mTX device\033[0m\n");
    console_printf("Press ENTER to start: \n");
    console_init(&tx_stress_read_command_cb);

    /* Waite for pressing ENTER in console */
    os_sem_pend(&tx_stress_main_sem, OS_TIMEOUT_NEVER);

    /* Init semaphore with 0 tokens. */
    rc = os_sem_init(&tx_stress_main_sem, 0);
    assert(rc == 0);

    /* Start test 1 - Connect/Connect cancel */
    //tx_stress_test_perform(1);

    while (1) {
        console_printf("\033[0;36mStart scan for test\033[0m\n");

        /* Scan for known UUID128 of one of the stress tests. */
        tx_stress_simple_scan(scan_for_test_gap_event, 2000);

        /* Wait for the scan to find the test. Then 1 token will be
         * released allowing to pass through semaphore. */
        os_sem_pend(&tx_stress_main_sem, OS_TIMEOUT_NEVER);
        if(tx_stress_ctx->scan_timeout) {
            break;
        }

        /* Start test. */
        tx_stress_test_perform(tx_stress_use_case);
        tx_stress_use_case = -1;
    }

    /* Print tests results */
    com_stress_print_report(tx_stress_ctx);

    /* Task should never return */
    while (1) {
        /* Delay used only to prevent watchdog to reset the device. */
        os_time_delay(os_time_ms_to_ticks32(2000));
    }
}

void tx_stress_start_auto()
{
    /* Start task that will run all stress tests one by one. */
    os_task_init(&tx_stress_main_task, "tx_stress_main_task",
                 tx_stress_main_task_fn, NULL, TX_STRESS_MAIN_TASK_PRIO,
                 OS_WAIT_FOREVER, tx_stress_main_task_stack,
                 TX_STRESS_MAIN_TASK_STACK_SIZE);
}
