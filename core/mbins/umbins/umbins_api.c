/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <umbins_api.h>
#include <k_api.h>
#include "aos/kernel.h"
#include "aos/hal/uart.h"

#include <stdarg.h>
#include <stdio.h>

#ifdef CONFIG_AOS_MESH
#include <umesh.h>
#endif

#ifdef MBEDTLS_IN_KERNEL
#include <ali_crypto.h>
#endif

#define SYSCALL(nr, func)

#include <kmbins_tbl.h>

#ifdef WITH_LWIP
#include <network/network.h>
#endif

#ifdef AOS_BT
#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>
#include <bluetooth/conn.h>
#endif


/* --------------------function-------------------- */

int __wrap_vprintf(char *format, va_list param)
{
    return aos_vprintf(format, param);
}

int __wrap_fflush(FILE *stream)
{
    return aos_fflush(stream);
}
/* --------------------function-------------------- */

/* --------------------k_task-------------------- */
ktask_t *krhino_cur_task_get(void)
{
    return SYS_CALL0(SYS_KRHINO_CUR_TASK_GET, ktask_t *);
}

kstat_t krhino_task_info_set(ktask_t *task, size_t idx, void *info)
{
    return SYS_CALL3(SYS_KRHINO_TASK_INFO_SET, kstat_t, ktask_t *, task,
                    size_t, idx, void *, info);
}

kstat_t krhino_task_info_get(ktask_t *task, size_t idx, void **info)
{
    return SYS_CALL3(SYS_KRHINO_TASK_INFO_GET, kstat_t, ktask_t *, task,
                    size_t, idx, void **, info);
}

void aos_msleep(int ms)
{
    SYS_CALL1(SYS_KRHINO_TASK_SLEEP, void, int, ms);
}

kstat_t krhino_task_dyn_create(ktask_t **task, const name_t *name, void *arg,
                               uint8_t pri, tick_t ticks, size_t stack,
                               task_entry_t entry, uint8_t autorun)
{
    return SYS_CALL8(SYS_KRHINO_TASK_DYN_CREATE, kstat_t, ktask_t **, task, const name_t *, name,
                    void *, arg, uint8_t, pri, tick_t, ticks, size_t, stack,
                        task_entry_t, entry, uint8_t, autorun);
}

kstat_t krhino_task_dyn_del(ktask_t *task)
{
    return SYS_CALL1(SYS_KRHINO_TASK_DYN_DEL, kstat_t, ktask_t *, task);
}

/* --------------------k_timer-------------------- */

sys_time_t krhino_sys_time_get(void)
{
    return SYS_CALL0(SYS_KRHINO_SYS_TIME_GET, sys_time_t);
}

tick_t krhino_sys_tick_get(void)
{
    return SYS_CALL0(SYS_KRHINO_SYS_TICK_GET, tick_t);
}

tick_t krhino_ms_to_ticks(sys_time_t ms)
{
    return SYS_CALL1(SYS_KRHINO_MS_TO_TICKS, tick_t, sys_time_t, ms);
}

sys_time_t krhino_ticks_to_ms(tick_t ticks)
{
    return SYS_CALL1(SYS_KRHINO_TICKS_TO_MS, sys_time_t, tick_t, ticks);
}

kstat_t krhino_timer_dyn_create(ktimer_t **timer, const name_t *name,
                                timer_cb_t cb,
                                tick_t first, tick_t round, void *arg, uint8_t auto_run)
{
    return SYS_CALL7(SYS_KRHINO_TIMER_DYN_CREATE, kstat_t, ktimer_t **, timer, const name_t *, name,
                        timer_cb_t, cb, tick_t, first, tick_t, round, void *, arg,
                            uint8_t, auto_run);
}

kstat_t krhino_timer_dyn_del(ktimer_t *timer)
{
    return SYS_CALL1(SYS_KRHINO_TIMER_DYN_DEL, kstat_t, ktimer_t *, timer);
}

kstat_t krhino_timer_start(ktimer_t *timer)
{
    return SYS_CALL1(SYS_KRHINO_TIMER_START, kstat_t, ktimer_t *, timer);
}

kstat_t krhino_timer_stop(ktimer_t *timer)
{
    return SYS_CALL1(SYS_KRHINO_TIMER_STOP, kstat_t, ktimer_t *, timer);
}

kstat_t krhino_timer_change(ktimer_t *timer, tick_t first, tick_t round)
{
    return SYS_CALL3(SYS_KRHINO_TIMER_CHANGE, kstat_t, ktimer_t *, timer, tick_t, first, tick_t, round);
}


/* --------------------k_mutex-------------------- */
kstat_t krhino_mutex_create(kmutex_t *mutex, const name_t *name)
{
    return SYS_CALL2(SYS_KRHINO_MUTEX_CREATE, kstat_t, kmutex_t *, mutex, const name_t *, name);
}

kstat_t krhino_mutex_del(kmutex_t *mutex)
{
    return SYS_CALL1(SYS_KRHINO_MUTEX_DEL, kstat_t, kmutex_t *, mutex);
}

kstat_t krhino_mutex_lock(kmutex_t *mutex, tick_t ticks)
{
    return SYS_CALL2(SYS_KRHINO_MUTEX_LOCK, kstat_t, kmutex_t *, mutex, tick_t, ticks);
}

kstat_t krhino_mutex_unlock(kmutex_t *mutex)
{
    return SYS_CALL1(SYS_KRHINO_MUTEX_UNLOCK, kstat_t, kmutex_t *, mutex);
}

/* --------------------k_sem-------------------- */

kstat_t krhino_sem_create(ksem_t *sem, const name_t *name, sem_count_t count)
{
    return SYS_CALL3(SYS_KRHINO_SEM_CREATE, kstat_t, ksem_t *, sem, const name_t *, name,
                    sem_count_t, count);

}

kstat_t krhino_sem_del(ksem_t *sem)
{
    return SYS_CALL1(SYS_KRHINO_SEM_DEL, kstat_t, ksem_t *, sem);
}

kstat_t krhino_sem_take(ksem_t *sem, tick_t ticks)
{
    return SYS_CALL2(SYS_KRHINO_SEM_TAKE, kstat_t, ksem_t *, sem, tick_t, ticks);
}

kstat_t krhino_sem_give(ksem_t *sem)
{
    return SYS_CALL1(SYS_KRHINO_SEM_GIVE, kstat_t, ksem_t *, sem);
}

/* --------------------k_mm-------------------- */
void *aos_malloc(unsigned int size)
{
    return SYS_CALL1(SYS_KRHINO_MM_ALLOC, void *, unsigned int, size);
}

void aos_free(void *mem)
{
    SYS_CALL1(SYS_KRHINO_MM_FREE, void, void *, mem);
}

void *aos_realloc(void *mem, unsigned int size)
{
    return SYS_CALL2(SYS_KRHINO_MM_REALLOC, void *, void *, mem, unsigned int, size);
}

void aos_alloc_trace(void *addr, size_t allocator)
{
    SYS_CALL2(SYS_AOS_ALLOC_TRACE, void, void *, addr, size_t, allocator);
}

/* ----------------k_buf_queue----------------- */
kstat_t krhino_buf_queue_send(kbuf_queue_t *queue, void *msg, size_t size)
{
    return SYS_CALL3(SYS_KRHINO_BUF_QUEUE_SEND, kstat_t, kbuf_queue_t *, queue, void *, msg,
                    size_t, size);
}

kstat_t krhino_buf_queue_recv(kbuf_queue_t *queue, tick_t ticks, void *msg,
                              size_t *size)
{
    return SYS_CALL4(SYS_KRHINO_BUF_QUEUE_RECV, kstat_t, kbuf_queue_t *, queue, tick_t, ticks,
                    void *, msg, size_t *, size);
}

kstat_t krhino_buf_queue_create(kbuf_queue_t *queue, const name_t *name,
                                void *buf, size_t size, size_t max_msg)
{
    return SYS_CALL5(SYS_KRHINO_BUF_QUEUE_CREATE, kstat_t, kbuf_queue_t *, queue,
                    const name_t *, name, void *, buf, size_t, size, size_t, max_msg);

}

kstat_t krhino_buf_queue_del(kbuf_queue_t *queue)
{
    return SYS_CALL1(SYS_KRHINO_BUF_QUEUE_DEL, kstat_t, kbuf_queue_t *, queue);
}

/* --------------------vfs-------------------- */
off_t aos_lseek(int fd, off_t offset, int whence)
{
    return SYS_CALL3(SYS_AOS_LSEEK, off_t, int, fd, off_t, offset, int, whence);
}

int aos_close(int fd)
{
    return SYS_CALL1(SYS_AOS_CLOSE, int, int, fd);
}

ssize_t aos_read(int fd, void *buf, size_t nbytes)
{
    return SYS_CALL3(SYS_AOS_READ, ssize_t, int, fd, void *, buf, size_t, nbytes);
}

ssize_t aos_write(int fd, const void *buf, size_t nbytes)
{
    return SYS_CALL3(SYS_AOS_WRITE, ssize_t, int, fd, const void *, buf, size_t, nbytes);
}

/* --------------------Framework-------------------- */

/* --------------------OTA-------------------- */
#ifdef AOS_OTA

int ais_ota_bt_storage_init(void)
{
    return SYS_CALL0(SYS_AIS_OTA_BT_STORAGE_INIT, int);
}

int ais_ota_get_local_addr(bt_addr_le_t *addr)
{
    return SYS_CALL1(SYS_AIS_OTA_GET_LOCAL_ADDR, int, bt_addr_le_t *, addr);
}
#endif

/* --------------------ALINK-------------------- */
#ifdef AOS_BREEZE

int alink_start(struct device_config *dev_conf)
{
    SYS_CALL1(SYS_ALINK_START, int, struct device_config *, dev_conf);
}

int alink_end(void)
{
    SYS_CALL0(SYS_ALINK_END, int);
}

void alink_post(uint8_t *buffer, uint32_t length)
{
    SYS_CALL2(SYS_ALINK_POST, void, uint8_t *, buffer, uint32_t, length);
}

void alink_post_fast(uint8_t *buffer, uint32_t length)
{
    SYS_CALL2(SYS_ALINK_POST_FAST, void, uint8_t *, buffer, uint32_t, length);

}
#endif

/* --------------------BLE-------------------- */
#ifdef AOS_BT

void hrs_init(u8_t blsc)
{
    SYS_CALL1(SYS_HRS_INIT, void, u8_t, blsc);
}

void hrs_notify(void)
{
    SYS_CALL0(SYS_HRS_NOTIFY, void);
}

void bas_init(void)
{
    SYS_CALL0(SYS_BAS_INIT, void);
}

void bas_notify(void)
{
    SYS_CALL0(SYS_BAS_NOTIFY, void);
}

void dis_init(const char *model, const char *manuf)
{
    SYS_CALL2(SYS_DIS_INIT, void, const char *, model, const char *, manuf);
}

int bt_le_adv_start(const struct bt_le_adv_param *param,
		    const struct bt_data *ad, size_t ad_len,
		    const struct bt_data *sd, size_t sd_len)
{
    return SYS_CALL5(SYS_BT_LE_ADV_START, int, const struct bt_le_adv_param *, param,
        const struct bt_data *, ad, size_t, ad_len,
        const struct bt_data *, sd, size_t, sd_len);
}

const bt_addr_le_t *bt_conn_get_dst(const struct bt_conn *conn)
{
    return SYS_CALL1(SYS_BT_CONN_GET_DST, const bt_addr_le_t *, const struct bt_conn *, conn);
}

struct bt_conn *bt_conn_ref(struct bt_conn *conn)
{
    return SYS_CALL1(SYS_BT_CONN_REF, struct bt_conn *, struct bt_conn *, conn);
}

int bt_conn_security(struct bt_conn *conn, bt_security_t sec)
{
    return SYS_CALL2(SYS_BT_CONN_SECURITY, int, struct bt_conn *, conn, bt_security_t, sec);
}

int hci_driver_init(void)
{
    return SYS_CALL0(SYS_HCI_DRIVER_INIT, int);
}

int bt_enable(bt_ready_cb_t cb)
{
    return SYS_CALL1(SYS_BT_ENABLE, int, bt_ready_cb_t, cb);
}

int bt_conn_auth_cb_register(const struct bt_conn_auth_cb *cb)
{
    return SYS_CALL1(SYS_BT_CONN_AUTH_CB_REGISTER, int, const struct bt_conn_auth_cb *, cb);
}

void bt_conn_cb_register(struct bt_conn_cb *cb)
{
    SYS_CALL1(SYS_BT_CONN_CB_REGISTER, void, struct bt_conn_cb *, cb);
}

void bt_conn_unref(struct bt_conn *conn)
{
    SYS_CALL1(SYS_BT_CONN_UNREF, void, struct bt_conn *, conn);
}

#endif


/* -----------------end BLE-------------------- */


/* -----------------OTHERS--------------------- */

int get_errno(void)
{
    return SYS_CALL0(SYS_GET_ERRNO, int);
}

void set_errno(int err)
{
    return SYS_CALL1(SYS_SET_ERRNO, void, int, err);
}

int32_t hal_uart_send(uart_dev_t *uart, const void *data, uint32_t size, uint32_t timeout)
{
    return SYS_CALL4(SYS_HAL_UART_SEND, int32_t, uart_dev_t *, uart, const void *, data,
        uint32_t, size, uint32_t, timeout);
}

int aos_vprintf(char *format, va_list param)
{
    return SYS_CALL2(SYS_VPRINTF, int, char *, format, va_list, param);
}

int aos_fflush(FILE *stream)
{
    return SYS_CALL1(SYS_FFLUSH, int, FILE *, stream);
}

/* -----------------end OTHERS-------------------- */


