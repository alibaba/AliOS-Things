/**
 * \file            lwesp.c
 * \brief           LwESP core file
 */

/*
 * Copyright (c) 2022 Tilen MAJERLE
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without restriction,
 * including without limitation the rights to use, copy, modify, merge,
 * publish, distribute, sublicense, and/or sell copies of the Software,
 * and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE
 * AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 *
 * This file is part of LwESP - Lightweight ESP-AT parser library.
 *
 * Author:          Tilen MAJERLE <tilen@majerle.eu>
 * Version:         v1.1.2-dev
 */
#include "lwesp/lwesp_private.h"
#include "lwesp/lwesp_mem.h"
#include "lwesp/lwesp_timeout.h"
#include "lwesp/lwesp_threads.h"
#include "system/lwesp_ll.h"

/* Check for configuration */
#if LWESP_CFG_OS != 1
#error LWESP_CFG_OS must be set to 1 in current revision!
#endif /* LWESP_CFG_OS != 1 */

#if LWESP_CFG_CONN_MANUAL_TCP_RECEIVE
//#error LWESP_CFG_CONN_MANUAL_TCP_RECEIVE must be set to 0 in current revision!
#endif /* LWESP_CFG_CONN_MANUAL_TCP_RECEIVE */

static lwesp_evt_func_t   def_evt_link;

/* Global ESP structure */
lwesp_t esp;

/**
 * \brief           Default callback function for events
 * \param[in]       evt: Pointer to callback data structure
 * \return          Member of \ref lwespr_t enumeration
 */
static lwespr_t
prv_def_callback(lwesp_evt_t* evt) {
    LWESP_UNUSED(evt);
    return lwespOK;
}

#if LWESP_CFG_KEEP_ALIVE

/**
 * \brief           Keep-alive timeout callback function
 * \param[in]       arg: Custom user argument
 */
static void
prv_keep_alive_timeout_fn(void* arg) {
    /* Dispatch keep-alive events */
    lwespi_send_cb(LWESP_EVT_KEEP_ALIVE);

    /* Start new timeout */
    lwesp_timeout_add(LWESP_CFG_KEEP_ALIVE_TIMEOUT, prv_keep_alive_timeout_fn, arg);
}

#endif /* LWESP_CFG_KEEP_ALIVE */

/**
 * \brief           Init and prepare ESP stack for device operation
 * \note            Function must be called from operating system thread context.
 *                  It creates necessary threads and waits them to start, thus running operating system is important.
 *                  - When \ref LWESP_CFG_RESET_ON_INIT is enabled, reset sequence will be sent to device
 *                      otherwise manual call to \ref lwesp_reset is required to setup device
 *                  - When \ref LWESP_CFG_RESTORE_ON_INIT is enabled, restore sequence will be sent to device.
 *
 * \param[in]       evt_func: Global event callback function for all major events
 * \param[in]       blocking: Status whether command should be blocking or not.
 *                      Used when \ref LWESP_CFG_RESET_ON_INIT or \ref LWESP_CFG_RESTORE_ON_INIT are enabled.
 * \return          \ref lwespOK on success, member of \ref lwespr_t enumeration otherwise
 */
lwespr_t
lwesp_init(lwesp_evt_fn evt_func, const uint32_t blocking) {
    lwespr_t res = lwespOK;
    static int software_inited = 0;
    if (software_inited == 0) {
        memset(&esp, 0x00, sizeof(esp));            /* Reset structure to all zeros */
        esp.status.f.initialized = 0;               /* Clear possible init flag */
        def_evt_link.fn = evt_func != NULL ? evt_func : prv_def_callback;
        esp.evt_func = &def_evt_link;               /* Set callback function */
        esp.evt_server = NULL;                      /* Set default server callback function */
        
        esp.m.device = LWESP_DEVICE_ESP8266; 
        if (!lwesp_sys_init()) {                    /* Init low-level system */
            goto cleanup;
        }

        if (!lwesp_sys_sem_create(&esp.sem_sync, 1)) {  /* Create sync semaphore between threads */
            LWESP_DEBUGF(LWESP_CFG_DBG_INIT | LWESP_DBG_LVL_SEVERE | LWESP_DBG_TYPE_TRACE,
                         "[LWESP CORE] Cannot create sync semaphore!\r\n");
            goto cleanup;
        }

        /* Create message queues */
        if (!lwesp_sys_mbox_create(&esp.mbox_producer, LWESP_CFG_THREAD_PRODUCER_MBOX_SIZE)) {  /* Producer */
            LWESP_DEBUGF(LWESP_CFG_DBG_INIT | LWESP_DBG_LVL_SEVERE | LWESP_DBG_TYPE_TRACE,
                         "[LWESP CORE] Cannot create producer mbox queue!\r\n");
            goto cleanup;
        }
        if (!lwesp_sys_mbox_create(&esp.mbox_process, LWESP_CFG_THREAD_PROCESS_MBOX_SIZE)) {/* Process */
            LWESP_DEBUGF(LWESP_CFG_DBG_INIT | LWESP_DBG_LVL_SEVERE | LWESP_DBG_TYPE_TRACE,
                         "[LWESP CORE] Cannot create process mbox queue!\r\n");
            goto cleanup;
        }

        /*
         * Create threads
         *
         * Each thread receives handle of semaphore that must be released inside the thread.
         * This is to make sure threads start immediately after they are created
         */
        lwesp_sys_sem_wait(&esp.sem_sync, 0);       /* Lock semaphore */
        if (!lwesp_sys_thread_create(&esp.thread_produce, "lwesp_produce", lwesp_thread_produce, &esp.sem_sync, LWESP_SYS_THREAD_SS, LWESP_SYS_THREAD_PRIO)) {
            LWESP_DEBUGF(LWESP_CFG_DBG_INIT | LWESP_DBG_LVL_SEVERE | LWESP_DBG_TYPE_TRACE,
                         "[LWESP CORE] Cannot create producing thread!\r\n");
            lwesp_sys_sem_release(&esp.sem_sync);   /* Release semaphore and return */
            goto cleanup;
        }
        lwesp_sys_sem_wait(&esp.sem_sync, 0);       /* Wait semaphore, should be unlocked in process thread */
        if (!lwesp_sys_thread_create(&esp.thread_process, "lwesp_process", lwesp_thread_process, &esp.sem_sync, LWESP_SYS_THREAD_PROCESS_SS, LWESP_SYS_THREAD_PRIO)) {
            LWESP_DEBUGF(LWESP_CFG_DBG_INIT | LWESP_DBG_LVL_SEVERE | LWESP_DBG_TYPE_TRACE,
                         "[LWESP CORE] Cannot create processing thread!\r\n");
            lwesp_sys_thread_terminate(&esp.thread_produce);/* Delete produce thread */
            lwesp_sys_sem_release(&esp.sem_sync);   /* Release semaphore and return */
            goto cleanup;
        }
        lwesp_sys_sem_wait(&esp.sem_sync, 0);       /* Wait semaphore, should be unlocked in produce thread */
        lwesp_sys_sem_release(&esp.sem_sync);       /* Release semaphore manually */
        LWESP_DEBUGF(LWESP_CFG_DBG_THREAD | LWESP_DBG_TYPE_TRACE,
                     "[lwesp_init] %d \r\n", __LINE__);


        lwesp_core_lock();
        esp.ll.uart.baudrate = LWESP_CFG_AT_PORT_BAUDRATE;  /* Set default baudrate value */
        lwesp_ll_init(&esp.ll);                     /* Init low-level communication */
        LWESP_DEBUGF(LWESP_CFG_DBG_THREAD | LWESP_DBG_TYPE_TRACE,
                     "[lwesp_init] %d \r\n", __LINE__);



#if !LWESP_CFG_INPUT_USE_PROCESS
        lwesp_buff_init(&esp.buff, LWESP_CFG_RCV_BUFF_SIZE);/* Init buffer for input data */
#endif /* !LWESP_CFG_INPUT_USE_PROCESS */

        esp.status.f.initialized = 1;               /* We are initialized now */
        esp.status.f.dev_present = 1;               /* We assume device is present at this point */

        lwespi_send_cb(LWESP_EVT_INIT_FINISH);      /* Call user callback function */
        /*
         * Call reset command and call default
         * AT commands to prepare basic setup for device
         */
        lwespi_conn_init();                         /* Init connection module */

#if LWESP_CFG_KEEP_ALIVE
        /* Register keep-alive events */
        lwesp_timeout_add(LWESP_CFG_KEEP_ALIVE_TIMEOUT, prv_keep_alive_timeout_fn, NULL);
#endif /* LWESP_CFG_KEEP_ALIVE */
         LWESP_DEBUGF(LWESP_CFG_DBG_THREAD | LWESP_DBG_TYPE_TRACE,
                     "[lwesp_init] %d \r\n", __LINE__);
        software_inited = 1;
    }

    if (esp.status.f.dev_present) {             /* In case device exists */
        lwesp_core_unlock();
        res = lwesp_at_test(NULL, NULL, blocking);  /* test device if connected*/
        lwesp_core_lock();
        if (res != lwespOK) {
            goto out;
        }
    }
#if LWESP_CFG_RESTORE_ON_INIT
    if (esp.status.f.dev_present) {             /* In case device exists */
        lwesp_core_unlock();
        res = lwesp_restore(NULL, NULL, blocking);  /* Restore device */
        lwesp_core_lock();
        if (res != lwespOK) {
            goto out;
        }
    }
#endif /* LWESP_CFG_RESTORE_ON_INIT */
    LWESP_DEBUGF(LWESP_CFG_DBG_THREAD | LWESP_DBG_TYPE_TRACE,
                 "[lwesp_init] %d \r\n", __LINE__);


#if LWESP_CFG_RESET_ON_INIT
    if (esp.status.f.dev_present) {
        lwesp_core_unlock();
        res = lwesp_reset_with_delay(LWESP_CFG_RESET_DELAY_DEFAULT, NULL, NULL, blocking);  /* Send reset sequence with delay */
        lwesp_core_lock();
    }
#endif /* LWESP_CFG_RESET_ON_INIT */
    LWESP_DEBUGF(LWESP_CFG_DBG_THREAD | LWESP_DBG_TYPE_TRACE,
                 "[lwesp_init] %d \r\n", __LINE__);


    LWESP_UNUSED(blocking);                     /* Prevent compiler warnings */
out:
    lwesp_core_unlock();
    return res;
cleanup:
    if (lwesp_sys_mbox_isvalid(&esp.mbox_producer)) {
        lwesp_sys_mbox_delete(&esp.mbox_producer);
        lwesp_sys_mbox_invalid(&esp.mbox_producer);
    }
    if (lwesp_sys_mbox_isvalid(&esp.mbox_process)) {
        lwesp_sys_mbox_delete(&esp.mbox_process);
        lwesp_sys_mbox_invalid(&esp.mbox_process);
    }
    if (lwesp_sys_sem_isvalid(&esp.sem_sync)) {
        lwesp_sys_sem_delete(&esp.sem_sync);
        lwesp_sys_sem_invalid(&esp.sem_sync);
    }
    return lwespERRMEM;
}

/**
 * \brief           Execute reset and send default commands
 * \param[in]       evt_fn: Callback function called when command has finished. Set to `NULL` when not used
 * \param[in]       evt_arg: Custom argument for event callback function
 * \param[in]       blocking: Status whether command should be blocking or not
 * \return          \ref lwespOK on success, member of \ref lwespr_t enumeration otherwise
 */
lwespr_t
lwesp_reset(const lwesp_api_cmd_evt_fn evt_fn, void* const evt_arg, const uint32_t blocking) {
    return lwesp_reset_with_delay(0, evt_fn, evt_arg, blocking);
}

/**
 * \brief           Execute reset and send default commands with delay before first command
 * \param[in]       delay: Number of milliseconds to wait before initiating first command to device
 * \param[in]       evt_fn: Callback function called when command has finished. Set to `NULL` when not used
 * \param[in]       evt_arg: Custom argument for event callback function
 * \param[in]       blocking: Status whether command should be blocking or not
 * \return          \ref lwespOK on success, member of \ref lwespr_t enumeration otherwise
 */
lwespr_t
lwesp_reset_with_delay(uint32_t delay,
                       const lwesp_api_cmd_evt_fn evt_fn, void* const evt_arg, const uint32_t blocking) {
    LWESP_MSG_VAR_DEFINE(msg);

    LWESP_MSG_VAR_ALLOC(msg, blocking);
    LWESP_MSG_VAR_SET_EVT(msg, evt_fn, evt_arg);
    LWESP_MSG_VAR_REF(msg).cmd_def = LWESP_CMD_RESET;
    LWESP_MSG_VAR_REF(msg).msg.reset.delay = delay;

    return lwespi_send_msg_to_producer_mbox(&LWESP_MSG_VAR_REF(msg), lwespi_initiate_cmd, 5000);
}

/**
 * \brief           Execute restore command and set module to default values
 * \param[in]       evt_fn: Callback function called when command has finished. Set to `NULL` when not used
 * \param[in]       evt_arg: Custom argument for event callback function
 * \param[in]       blocking: Status whether command should be blocking or not
 * \return          \ref lwespOK on success, member of \ref lwespr_t enumeration otherwise
 */
lwespr_t
lwesp_restore(const lwesp_api_cmd_evt_fn evt_fn, void* const evt_arg, const uint32_t blocking) {
    LWESP_MSG_VAR_DEFINE(msg);

    LWESP_MSG_VAR_ALLOC(msg, blocking);
    LWESP_MSG_VAR_SET_EVT(msg, evt_fn, evt_arg);
    LWESP_MSG_VAR_REF(msg).cmd_def = LWESP_CMD_RESTORE;
    LWESP_MSG_VAR_REF(msg).cmd = LWESP_CMD_RESET;

    return lwespi_send_msg_to_producer_mbox(&LWESP_MSG_VAR_REF(msg), lwespi_initiate_cmd, 5000);
}

lwespr_t
lwesp_at_test(const lwesp_api_cmd_evt_fn evt_fn, void* const evt_arg, const uint32_t blocking) {
    LWESP_MSG_VAR_DEFINE(msg);

    LWESP_MSG_VAR_ALLOC(msg, blocking);
    LWESP_MSG_VAR_SET_EVT(msg, evt_fn, evt_arg);
    LWESP_MSG_VAR_REF(msg).cmd_def = LWESP_CMD_AT_TEST;
    LWESP_MSG_VAR_REF(msg).cmd = LWESP_CMD_AT_TEST;

    return lwespi_send_msg_to_producer_mbox(&LWESP_MSG_VAR_REF(msg), lwespi_initiate_cmd, 500);
}

lwespr_t
lwesp_sysmsg_def(const lwesp_api_cmd_evt_fn evt_fn, void* const evt_arg, const uint32_t blocking) {
    LWESP_MSG_VAR_DEFINE(msg);

    LWESP_MSG_VAR_ALLOC(msg, blocking);
    LWESP_MSG_VAR_SET_EVT(msg, evt_fn, evt_arg);
    LWESP_MSG_VAR_REF(msg).cmd_def = LWESP_CMD_SYSMSG;
    LWESP_MSG_VAR_REF(msg).cmd = LWESP_CMD_SYSMSG;

    return lwespi_send_msg_to_producer_mbox(&LWESP_MSG_VAR_REF(msg), lwespi_initiate_cmd, 5000);
}
/**
 * \brief           Sets WiFi mode to either station only, access point only or both
 *
 * Configuration changes will be saved in the NVS area of ESP device.
 *
 * \param[in]       mode: Mode of operation. This parameter can be a value of \ref lwesp_mode_t enumeration
 * \param[in]       evt_fn: Callback function called when command has finished. Set to `NULL` when not used
 * \param[in]       evt_arg: Custom argument for event callback function
 * \param[in]       blocking: Status whether command should be blocking or not
 * \return          \ref lwespOK on success, member of \ref lwespr_t enumeration otherwise
 */
lwespr_t
lwesp_set_wifi_mode(lwesp_mode_t mode,
                    const lwesp_api_cmd_evt_fn evt_fn, void* const evt_arg, const uint32_t blocking) {
    LWESP_MSG_VAR_DEFINE(msg);

    LWESP_MSG_VAR_ALLOC(msg, blocking);
    LWESP_MSG_VAR_SET_EVT(msg, evt_fn, evt_arg);
    LWESP_MSG_VAR_REF(msg).cmd_def = LWESP_CMD_WIFI_CWMODE;
    LWESP_MSG_VAR_REF(msg).msg.wifi_mode.mode = mode;

    return lwespi_send_msg_to_producer_mbox(&LWESP_MSG_VAR_REF(msg), lwespi_initiate_cmd, 1000);
}

/**
 * \brief           Gets WiFi mode of either station only, access point only or both
 *
 * \param[in]       mode: point to space of Mode to get. This parameter can be a pointer of \ref lwesp_mode_t enumeration
 * \param[in]       evt_fn: Callback function called when command has finished. Set to `NULL` when not used
 * \param[in]       evt_arg: Custom argument for event callback function
 * \param[in]       blocking: Status whether command should be blocking or not
 * \return          \ref lwespOK on success, member of \ref lwespr_t enumeration otherwise
 */
lwespr_t
lwesp_get_wifi_mode(lwesp_mode_t* mode,
                    const lwesp_api_cmd_evt_fn evt_fn, void* const evt_arg, const uint32_t blocking) {
    LWESP_MSG_VAR_DEFINE(msg);

    LWESP_MSG_VAR_ALLOC(msg, blocking);
    LWESP_MSG_VAR_SET_EVT(msg, evt_fn, evt_arg);
    LWESP_MSG_VAR_REF(msg).cmd_def = LWESP_CMD_WIFI_CWMODE_GET;
    LWESP_MSG_VAR_REF(msg).msg.wifi_mode.mode_get = mode;

    return lwespi_send_msg_to_producer_mbox(&LWESP_MSG_VAR_REF(msg), lwespi_initiate_cmd, 1000);
}

/**
 * \brief           Sets baudrate of AT port (usually UART)
 * \param[in]       baud: Baudrate in units of bits per second
 * \param[in]       evt_fn: Callback function called when command has finished. Set to `NULL` when not used
 * \param[in]       evt_arg: Custom argument for event callback function
 * \param[in]       blocking: Status whether command should be blocking or not
 * \return          \ref lwespOK on success, member of \ref lwespr_t enumeration otherwise
 */
lwespr_t
lwesp_set_at_baudrate(uint32_t baud,
                      const lwesp_api_cmd_evt_fn evt_fn, void* const evt_arg, const uint32_t blocking) {
    LWESP_MSG_VAR_DEFINE(msg);

    LWESP_MSG_VAR_ALLOC(msg, blocking);
    LWESP_MSG_VAR_SET_EVT(msg, evt_fn, evt_arg);
    LWESP_MSG_VAR_REF(msg).cmd_def = LWESP_CMD_UART;
    LWESP_MSG_VAR_REF(msg).msg.uart.baudrate = baud;

    return lwespi_send_msg_to_producer_mbox(&LWESP_MSG_VAR_REF(msg), lwespi_initiate_cmd, 2000);
}

#if LWESP_CFG_MODE_STATION || __DOXYGEN__

/**
 * \brief           Update ESP software remotely
 * \note            ESP must be connected to access point to use this feature
 * \param[in]       evt_fn: Callback function called when command has finished. Set to `NULL` when not used
 * \param[in]       evt_arg: Custom argument for event callback function
 * \param[in]       blocking: Status whether command should be blocking or not
 * \return          \ref lwespOK on success, member of \ref lwespr_t enumeration otherwise
 */
lwespr_t
lwesp_update_sw(const lwesp_api_cmd_evt_fn evt_fn, void* const evt_arg, const uint32_t blocking) {
    LWESP_MSG_VAR_DEFINE(msg);

    LWESP_MSG_VAR_ALLOC(msg, blocking);
    LWESP_MSG_VAR_SET_EVT(msg, evt_fn, evt_arg);
    LWESP_MSG_VAR_REF(msg).cmd_def = LWESP_CMD_TCPIP_CIUPDATE;

    return lwespi_send_msg_to_producer_mbox(&LWESP_MSG_VAR_REF(msg), lwespi_initiate_cmd, 180000);
}

#endif /* LWESP_CFG_MODE_STATION || __DOXYGEN__ */

/**
 * \brief           Lock stack from multi-thread access, enable atomic access to core
 *
 * If lock was `0` prior function call, lock is enabled and increased
 *
 * \note            Function may be called multiple times to increase locks.
 *                  Application must take care to call \ref lwesp_core_unlock
 *                  the same amount of time to make sure lock gets back to `0`
 * \return          \ref lwespOK on success, member of \ref lwespr_t enumeration otherwise
 */
lwespr_t
lwesp_core_lock(void) {
    lwesp_sys_protect();
    ++esp.locked_cnt;
    return lwespOK;
}

/**
 * \brief           Unlock stack for multi-thread access
 *
 * Used in conjunction with \ref lwesp_core_lock function
 *
 * If lock was non-zero before function call, lock is decreased.
 * When `lock == 0`, protection is disabled and other threads may access to core
 *
 * \return          \ref lwespOK on success, member of \ref lwespr_t enumeration otherwise
 */
lwespr_t
lwesp_core_unlock(void) {
    --esp.locked_cnt;
    if (esp.locked_cnt < 0) {
        esp.locked_cnt = 0;
    }
    lwesp_sys_unprotect();
    return lwespOK;
}

/**
 * \brief           Notify stack if device is present or not
 *
 * Use this function to notify stack that device is not physically connected
 * and not ready to communicate with host device
 *
 * \param[in]       present: Flag indicating device is present
 * \param[in]       evt_fn: Callback function called when command has finished. Set to `NULL` when not used
 * \param[in]       evt_arg: Custom argument for event callback function
 * \param[in]       blocking: Status whether command should be blocking or not
 * \return          \ref lwespOK on success, member of \ref lwespr_t enumeration otherwise
 */
lwespr_t
lwesp_device_set_present(uint8_t present,
                         const lwesp_api_cmd_evt_fn evt_fn, void* const evt_arg, const uint32_t blocking) {
    lwespr_t res = lwespOK;
    lwesp_core_lock();
    present = present ? 1 : 0;
    if (present != esp.status.f.dev_present) {
        esp.status.f.dev_present = present;

        if (!esp.status.f.dev_present) {
            /* Manually reset stack to default device state */
            lwespi_reset_everything(1);
        } else {
#if LWESP_CFG_RESET_ON_DEVICE_PRESENT
            lwesp_core_unlock();
            res = lwesp_reset_with_delay(LWESP_CFG_RESET_DELAY_DEFAULT, evt_fn, evt_arg, blocking);
            lwesp_core_lock();
#endif /* LWESP_CFG_RESET_ON_DEVICE_PRESENT */
        }
        lwespi_send_cb(LWESP_EVT_DEVICE_PRESENT);   /* Send present event */
    }
    lwesp_core_unlock();

    LWESP_UNUSED(evt_fn);
    LWESP_UNUSED(evt_arg);
    LWESP_UNUSED(blocking);

    return res;
}

/**
 * \brief           Check if device is present
 * \return          `1` on success, `0` otherwise
 */
uint8_t
lwesp_device_is_present(void) {
    uint8_t res;
    lwesp_core_lock();
    res = esp.status.f.dev_present;
    lwesp_core_unlock();
    return res;
}

#if LWESP_CFG_ESP8266 || __DOXYGEN__

/**
 * \brief           Check if modem device is ESP8266
 * \note            Function is only available if \ref LWESP_CFG_ESP8266 is enabled, otherwise it is defined as macro and evaluated to `0`
 * \return          `1` on success, `0` otherwise
 */
uint8_t
lwesp_device_is_esp8266(void) {
    uint8_t res;
    lwesp_core_lock();
    res = esp.status.f.dev_present && esp.m.device == LWESP_DEVICE_ESP8266;
    lwesp_core_unlock();
    return res;
}

#endif /* LWESP_CFG_ESP8266 || __DOXYGEN__ */

#if LWESP_CFG_ESP32 || __DOXYGEN__

/**
 * \brief           Check if modem device is ESP32
 * \note            Function is only available if \ref LWESP_CFG_ESP32 is enabled, otherwise it is defined as macro and evaluated to `0`
 * \return          `1` on success, `0` otherwise
 */
uint8_t
lwesp_device_is_esp32(void) {
    uint8_t res;
    lwesp_core_lock();
    res = esp.status.f.dev_present && esp.m.device == LWESP_DEVICE_ESP32;
    lwesp_core_unlock();
    return res;
}

#endif /* LWESP_CFG_ESP32 || __DOXYGEN__ */

#if LWESP_CFG_ESP32_C3 || __DOXYGEN__

/**
 * \brief           Check if modem device is ESP32-C3
 * \note            Function is only available if \ref LWESP_CFG_ESP32_C3 is enabled, otherwise it is defined as macro and evaluated to `0`
 * \return          `1` on success, `0` otherwise
 */
uint8_t
lwesp_device_is_esp32_c3(void) {
    uint8_t res;
    lwesp_core_lock();
    res = esp.status.f.dev_present && esp.m.device == LWESP_DEVICE_ESP32_C3;
    lwesp_core_unlock();
    return res;
}

#endif /* LWESP_CFG_ESP32_C3 || __DOXYGEN__ */

/**
 * \brief           Get currently connected Espressif device to AT port
 * \return          Member of \ref lwesp_device_t enumeration
 */
lwesp_device_t
lwesp_device_get_device(void) {
    lwesp_device_t dev;
    lwesp_core_lock();
    dev = esp.m.device;
    lwesp_core_unlock();
    return dev;
}

/**
 * \brief           Get current AT firmware version of connected Espressif device.
 *                  It copies version from internal buffer to user variable,
 *                  and is valid only if reset/restore operation is successful.
 * \param[out]      version: Output version variable
 * \return          `1` on success, `0` otherwise
 */
lwespr_t
lwesp_get_current_at_fw_version(lwesp_sw_version_t* const version) {
    LWESP_ASSERT("version != NULL", version != NULL);
    lwesp_core_lock();
    LWESP_MEMCPY(version, &esp.m.version_at, sizeof(*version));
    lwesp_core_unlock();
    return lwespOK;
}

/**
 * \brief           Get minimal AT version required to run on Espressif device, to be well supported by LwESP library
 *                  and to ensure proper compatibility and correct operation
 * \param[out]      version: Version output, pointer to \ref lwesp_sw_version_t structure
 * \return          \ref lwespOK on success, member of \ref lwespr_t otherwise
 */
lwespr_t
lwesp_get_min_at_fw_version(lwesp_sw_version_t* const version) {
    uint8_t res = lwespOK;
    LWESP_ASSERT("version != NULL", version != NULL);

    lwesp_core_lock();
    if (0) {
#if LWESP_CFG_ESP8266
    } else if (esp.m.device == LWESP_DEVICE_ESP8266) {
        lwesp_set_fw_version(version, LWESP_MIN_AT_VERSION_MAJOR_ESP8266, LWESP_MIN_AT_VERSION_MINOR_ESP8266, LWESP_MIN_AT_VERSION_PATCH_ESP8266);
#endif /* LWESP_CFG_ESP8266 */
#if LWESP_CFG_ESP32
    } else if (esp.m.device == LWESP_DEVICE_ESP32) {
        lwesp_set_fw_version(version, LWESP_MIN_AT_VERSION_MAJOR_ESP32, LWESP_MIN_AT_VERSION_MINOR_ESP32, LWESP_MIN_AT_VERSION_PATCH_ESP32);
#endif /* LWESP_CFG_ESP32 */
#if LWESP_CFG_ESP32_C3
    } else if (esp.m.device == LWESP_DEVICE_ESP32_C3) {
        lwesp_set_fw_version(version, LWESP_MIN_AT_VERSION_MAJOR_ESP32_C3, LWESP_MIN_AT_VERSION_MINOR_ESP32_C3, LWESP_MIN_AT_VERSION_PATCH_ESP32_C3);
#endif /* LWESP_CFG_ESP32_C3 */
    } else {
        res = lwespERR;
    }
    lwesp_core_unlock();
    return res;
}

/**
 * \brief           Delay for amount of milliseconds
 *
 * Delay is based on operating system semaphores.
 * It locks semaphore and waits for timeout in `ms` time.
 * Based on operating system, thread may be put to \e blocked list during delay and may improve execution speed
 *
 * \param[in]       ms: Milliseconds to delay
 * \return          `1` on success, `0` otherwise
 */
uint8_t
lwesp_delay(const uint32_t ms) {
    lwesp_sys_sem_t sem;
    if (ms == 0) {
        return 1;
    }
    if (lwesp_sys_sem_create(&sem, 0)) {
        lwesp_sys_sem_wait(&sem, ms);
        lwesp_sys_sem_release(&sem);
        lwesp_sys_sem_delete(&sem);
        return 1;
    }
    return 0;
}
