/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <syscall_no.h>
#include "rhino_ksyscall.h"
#include "hal_uart_ksyscall.h"
#include "lwip_ksyscall.h"

/**************************************************************
 *   syscall table
 *************************************************************/
void *syscall_tbl[] = {
                       /* ------------------- task ----------------------*/
                       [SYS_KRHINO_CUR_TASK_GET] = sys_krhino_cur_task_get_stub,
                       [SYS_KRHINO_TASK_SLEEP]   = sys_krhino_task_sleep_stub,
                       [SYS_KRHINO_UTASK_CREATE] = sys_krhino_utask_create_stub,
                       [SYS_KRHINO_UTASK_DEL]    = sys_krhino_utask_del_stub,

                       /* ------------------- porcess ----------------------*/
                       [SYS_KRHINO_UPROCESS_CREATE] = sys_krhino_uprocess_create_stub,
                       [SYS_KRHINO_UPROCESS_EXIT]   = sys_krhino_uprocess_exit_stub,
                       [SYS_KRHINO_UPROCESS_RES_GET]   = sys_krhino_uprocess_res_get_stub,

                       /* ------------------- time ----------------------*/
                       [SYS_KRHINO_SYS_TIME_GET] = sys_krhino_sys_time_get_stub,
                       [SYS_KRHINO_SYS_TICK_GET] = sys_krhino_sys_tick_get_stub,
                       [SYS_KRHINO_MS_TO_TICKS]  = sys_krhino_ms_to_ticks_stub,
                       [SYS_KRHINO_TICKS_TO_MS]  = sys_krhino_ticks_to_ms_stub,

                       /* ------------------- mutex ---------------------*/
                       [SYS_KRHINO_MUTEX_CREATE]     = sys_krhino_mutex_create_stub,
                       [SYS_KRHINO_MUTEX_DEL]        = sys_krhino_mutex_del_stub,
                       [SYS_KRHINO_MUTEX_DYN_CREATE] = sys_krhino_mutex_dyn_create_stub,
                       [SYS_KRHINO_MUTEX_DYN_DEL]    = sys_krhino_mutex_dyn_del_stub,
                       [SYS_KRHINO_MUTEX_LOCK]       = sys_krhino_mutex_lock_stub,
                       [SYS_KRHINO_MUTEX_UNLOCK]     = sys_krhino_mutex_unlock_stub,

                       /* ------------------ semphore --------------------*/
                       [SYS_KRHINO_SEM_CREATE]     = sys_krhino_sem_create_stub,
                       [SYS_KRHINO_SEM_DEL]        = sys_krhino_sem_del_stub,
                       [SYS_KRHINO_SEM_DYN_CREATE] = sys_krhino_sem_dyn_create_stub,
                       [SYS_KRHINO_SEM_DYN_DEL]    = sys_krhino_sem_dyn_del_stub,
                       [SYS_KRHINO_SEM_TAKE]       = sys_krhino_sem_take_stub,
                       [SYS_KRHINO_SEM_GIVE]       = sys_krhino_sem_give_stub,

                       /* --------------------- queue -----------------------*/
                       [SYS_KRHINO_QUEUE_CREATE]     = sys_krhino_queue_create_stub,
                       [SYS_KRHINO_QUEUE_DEL]        = sys_krhino_queue_del_stub,
                       [SYS_KRHINO_QUEUE_DYN_CREATE] = sys_krhino_queue_dyn_create_stub,
                       [SYS_KRHINO_QUEUE_DYN_DEL]    = sys_krhino_queue_dyn_del_stub,
                       [SYS_KRHINO_QUEUE_BACK_SEND]  = sys_krhino_queue_back_send_stub,
                       [SYS_KRHINO_QUEUE_ALL_SEND]   = sys_krhino_queue_all_send_stub,
                       [SYS_KRHINO_QUEUE_RECV]       = sys_krhino_queue_recv_stub,
                       [SYS_KRHINO_QUEUE_FLUSH]      = sys_krhino_queue_flush_stub,

                       /* ------------------ buf queue --------------------*/
                       [SYS_KRHINO_BUF_QUEUE_CREATE]         = sys_krhino_buf_queue_create_stub,
                       [SYS_KRHINO_FIX_BUF_QUEUE_CREATE]     = sys_krhino_fix_buf_queue_create_stub,
                       [SYS_KRHINO_BUF_QUEUE_DEL]            = sys_krhino_buf_queue_del_stub,
                       [SYS_KRHINO_BUF_QUEUE_DYN_CREATE]     = sys_krhino_buf_queue_dyn_create_stub,
                       [SYS_KRHINO_FIX_BUF_QUEUE_DYN_CREATE] = sys_krhino_fix_buf_queue_dyn_create_stub,
                       [SYS_KRHINO_BUF_QUEUE_DYN_DEL]        = sys_krhino_buf_queue_dyn_del_stub,
                       [SYS_KRHINO_BUF_QUEUE_SEND]           = sys_krhino_buf_queue_send_stub,
                       [SYS_KRHINO_BUF_QUEUE_RECV]           = sys_krhino_buf_queue_recv_stub,
                       [SYS_KRHINO_BUF_QUEUE_FLUSH]          = sys_krhino_buf_queue_flush_stub,

                       /* ------------------ u_proc_msg -------------------*/
                       [SYS_KRHINO_MSG_GET]  = sys_krhino_msg_get_stub,
                       [SYS_KRHINO_MSG_SND]  = sys_krhino_msg_snd_stub,
                       [SYS_KRHINO_MSG_RECV] = sys_krhino_msg_recv_stub,

                       /* ------------------- hal uart ---------------------*/
                       [SYS_HAL_UART_INIT]     = sys_hal_uart_init_stub,
                       [SYS_HAL_UART_SEND]     = sys_hal_uart_send_stub,
                       [SYS_HAL_UART_RECV]     = sys_hal_uart_recv_stub,
                       [SYS_HAL_UART_RECV_II]  = sys_hal_uart_recv_II_stub,
                       [SYS_HAL_UART_FINALIZE] = sys_hal_uart_finalize_stub,

                       /* ---------------------- lwip ----------------------*/
#if (CONFIG_LWIP_SYSCALL > 0)
                       [SYS_LWIP_ACCEPT]      = sys_lwip_accept_stub,
                       [SYS_LWIP_BIND]        = sys_lwip_bind_stub,
                       [SYS_LWIP_SHUTDOWN]    = sys_lwip_shutdown_stub,
                       [SYS_LWIP_GETPEERNAME] = sys_lwip_getpeername_stub,
                       [SYS_LWIP_GETSOCKNAME] = sys_lwip_getsockname_stub,
                       [SYS_LWIP_GETSOCKOPT]  = sys_lwip_getsockopt_stub,
                       [SYS_LWIP_SETSOCKET]   = sys_lwip_setsockopt_stub,
                       [SYS_LWIP_CLOSE]       = sys_lwip_close_stub,
                       [SYS_LWIP_CONNECT]     = sys_lwip_connect_stub,
                       [SYS_LWIP_LISTEN]      = sys_lwip_listen_stub,
                       [SYS_LWIP_RECV]        = sys_lwip_recv_stub,
                       [SYS_LWIP_READ]        = sys_lwip_read_stub,
                       [SYS_LWIP_RECVFROM]    = sys_lwip_recvfrom_stub,
                       [SYS_LWIP_SEND]        = sys_lwip_send_stub,
                       [SYS_LWIP_SENDMSG]     = sys_lwip_sendmsg_stub,
                       [SYS_LWIP_SENDTO]      = sys_lwip_sendto_stub,
                       [SYS_LWIP_SOCKET]      = sys_lwip_socket,
                       [SYS_LWIP_WRITE]       = sys_lwip_write_stub,
                       [SYS_LWIP_WRITEV]      = sys_lwip_writev_stub,
                       [SYS_LWIP_SELECT]      = sys_lwip_select_stub,
                       [SYS_LWIP_IOCTL]       = sys_lwip_ioctl_stub,
                       [SYS_LWIP_FCNTL]       = sys_lwip_fcntl_stub,
                       [SYS_LWIP_EVENTFD]     = sys_lwip_eventfd_stub,
#if LWIP_PACKET
                       [SYS_LWIP_TRY_WAKEUP]  = sys_lwip_try_wakeup_stub,
#endif
#endif /* CONFIG_LWIP_SYSCALL */

                       /* ---------------- end ------------------*/
                       NULL
                      };

