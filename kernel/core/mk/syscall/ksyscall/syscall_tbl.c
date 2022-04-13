/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#include "k_config.h"
#include "syscall_no.h"
#include "rhino_ksyscall.h"
#include "aos_ksyscall.h"
#include "debug_ksyscall.h"
#include "aos_obj_ksyscall.h"
#include "iomap_ksyscall.h"
#include "hal_uart_ksyscall.h"
#include "hal_flash_ksyscall.h"
#include "hal_wdg_ksyscall.h"
#include "network_ksyscall.h"
#include "signal_ksyscall.h"

#if (AOS_COMP_KV > 0)
    #include "kv_ksyscall.h"
#endif
#if (AOS_COMP_VFS > 0)
    #include "vfs_ksyscall.h"
#endif
#if (AOS_COMP_CLI > 0)
    #include "cli_ksyscall.h"
#endif

#if (AOS_COMP_IRQ > 0)
    #include "irq_ksyscall.h"
#endif

#if (AOS_COMP_EPOLL > 0)
    #include "epoll_ksyscall.h"
#endif

#if (AOS_COMP_SELECT > 0)
    #include "select_ksyscall.h"
#endif

#if (AOS_COMP_NETMGR > 0)
    #include "netmgr_ksyscall.h"
#endif

/**************************************************************
 *   syscall table
 *************************************************************/
#if defined(__CC_ARM) || defined (__GNUC__)
    #if (RHINO_CONFIG_DYN_SYSCALL_TBL > 0)
        const void *syscall_tbl[] =
    #else
        const void *const syscall_tbl[] =
    #endif
#elif defined (__ICCARM__)
    #if (RHINO_CONFIG_DYN_SYSCALL_TBL > 0)
        __root void *syscall_tbl[] =
    #else
        const __root void *const syscall_tbl[] =
    #endif
#endif
{
    /* obj common */
    [SYS_AOS_HANDLE_DELETE]                  = (void *)sys_aos_handle_delete_stub,
    [SYS_AOS_HANDLE_CLOSE]                   = (void *)sys_aos_handle_close_stub,
    [SYS_AOS_HANDLE_VALID]                   = (void *)sys_aos_handle_valid_stub,
    /* aos sem */
    [SYS_AOS_SEM_NEW]                        = (void *)sys_aos_sem_new_stub,
    [SYS_AOS_SEM_OPEN]                       = (void *)sys_aos_sem_open_stub,
    [SYS_AOS_SEM_POST]                       = (void *)sys_aos_sem_signal_stub,
    [SYS_AOS_SEM_POST_ALL]                   = (void *)sys_aos_sem_signal_all_stub,
    [SYS_AOS_SEM_WAIT]                       = (void *)sys_aos_sem_wait_stub,
    [SYS_AOS_SEM_GETVALUE]                   = (void *)sys_aos_sem_getvalue_stub,
    [SYS_AOS_SEM_UNLINK]                     = (void *)sys_aos_sem_unlink_stub,
    /* aos mutex */
    [SYS_AOS_MUTEX_NEW]                      = (void *)sys_aos_mutex_new_stub,
    [SYS_AOS_MUTEX_LOCK]                     = (void *)sys_aos_mutex_lock_stub,
    [SYS_AOS_MUTEX_UNLOCK]                   = (void *)sys_aos_mutex_unlock_stub,
    /* aos event */
    [SYS_AOS_EVENT_NEW]                      = (void *)sys_aos_event_new_stub,
    [SYS_AOS_EVENT_OPEN]                     = (void *)sys_aos_event_open_stub,
    [SYS_AOS_EVENT_GET]                      = (void *)sys_aos_event_get_stub,
    [SYS_AOS_EVENT_SET]                      = (void *)sys_aos_event_set_stub,

    /* aos queue */
    [SYS_AOS_QUEUE_OPEN]                     = (void *)sys_aos_queue_open_stub,
    [SYS_AOS_QUEUE_NEW]                      = (void *)sys_aos_queue_new_stub,
    [SYS_AOS_QUEUE_SEND]                     = (void *)sys_aos_queue_send_stub,
    [SYS_AOS_QUEUE_RECV]                     = (void *)sys_aos_queue_recv_stub,
    [SYS_AOS_QUEUE_GET_COUNT]                = (void *)sys_aos_queue_get_count_stub,
#if 0
    /* aos timer */
    [SYS_AOS_TIMER_NEW_EXT]                  = (void *)sys_aos_timer_new_ext_stub,
    [SYS_AOS_TIMER_START]                    = (void *)sys_aos_timer_start_stub,
    [SYS_AOS_TIMER_STOP]                     = (void *)sys_aos_timer_stop_stub,
    [SYS_AOS_TIMER_CHANGE]                   = (void *)sys_aos_timer_change_stub,
    [SYS_AOS_TIMER_CHANGE_ONCE]              = (void *)sys_aos_timer_change_once_stub,
#endif
    /* futex */
    [SYS_AOS_FUTEX]                          = (void *)sys_aos_futex_stub,
    /* ------------------ aos info -------------------*/
    [SYS_AOS_VERSION_STR_GET]                = (void *)sys_aos_version_str_get_stub,
    [SYS_AOS_GET_CALENDAR]                   = (void *)sys_aos_get_calendar_time_stub,
    [SYS_AOS_SET_CALENDAR]                   = (void *)sys_aos_set_calendar_time_stub,
    [SYS_AOS_GET_UTC_OFFSET]                 = (void *)sys_aos_get_utc_offset_stub,
    [SYS_AOS_SET_UTC_OFFSET]                 = (void *)sys_aos_set_utc_offset_stub,
    [SYS_AOS_REBOOT]                         = (void *)sys_aos_reboot_stub,
    [SYS_AOS_NOW_MS]                         = (void *)sys_aos_now_ms_stub,
    [SYS_AOS_GET_HZ]                         = (void *)sys_aos_get_hz_stub,
    [SYS_AOS_IPC_COPY]                       = (void *)sys_aos_ipc_copy_stub,
    [SYS_AOS_PRINT]                          = (void *)sys_aos_print_stub,
#if (AOS_COMP_IRQ > 0)
    /* ----------------  irq  ------------------*/
    [SYS_AOS_IRQ_REG]                           = (void *)sys_register_irq_global_stub,
    [SYS_AOS_IRQ_UNREG]                         = (void *)sys_unregister_irq_global_stub,
    [SYS_AOS_IRQ_IPI_SEND]                      = (void *)sys_ipi_send_stub,
    [SYS_AOS_IRQ_ENABLE]                        = (void *)sys_irq_enable_stub,
    [SYS_AOS_IRQ_DISABLE]                       = (void *)sys_irq_disable_stub,
    [SYS_AOS_IRQ_IS_ACTIVE]                     = (void *)sys_irq_is_active_stub,
    [SYS_AOS_IRQ_SET_PRIORITY]                  = (void *)sys_irq_set_priority_stub,
    [SYS_AOS_IRQ_SET_PENDING]                   = (void *)sys_irq_set_pending_stub,
    [SYS_AOS_IRQ_CLEAR_PENDING]                 = (void *)sys_irq_clear_pending_stub,
    [SYS_AOS_IRQ_ALL_UNREG]                     = (void *)sys_unregister_allirq_global_stub,
    [SYS_AOS_IRQ_GET_ACTIVE]                    = (void *)sys_irq_get_active_irq_stub,
    [SYS_AOS_IRQ_END_INTERRUPT]                 = (void *)sys_irq_end_interrupt_stub,
    [SYS_AOS_IRQ_SET_TYPE]                      = (void *)sys_irq_set_type_stub,
    [SYS_AOS_IRQ_SET_NMI]                       = (void *)sys_irq_set_nmi_stub,
    [SYS_AOS_IRQ_SET_AFFINITY]                  = (void *)sys_irq_set_affinity_stub,
    [SYS_AOS_INT_LOCK]                          = (void *)sys_aos_int_lock_stub,
    [SYS_AOS_INT_UNLOCK]                        = (void *)sys_aos_int_unlock_stub,
#endif
    /* ----------------  iomap  ------------------*/
    [SYS_AOS_IOMAP_MAP]                           = (void *)sys_aos_io_mmap_stub,
    [SYS_AOS_IOMAP_UNMAP]                         = (void *)sys_aos_io_unmmap_stub,
    [SYS_AOS_IOMAP_VA2PA]                         = (void *)sys_aos_io_mva2pa_stub,
    [SYS_AOS_IOMAP_PA2VA]                         = (void *)sys_aos_io_mpa2va_stub,
    [SYS_AOS_MEMMAP_MAP]                          = (void *)sys_aos_mem_mmap_stub,
    /* ----------------  sys mem get  ------------------*/
    [SYS_AOS_MEM_INFO_GET]                        = (void *)sys_aos_get_sys_mem_info_stub,

    /* ------------------- task ----------------------*/
    [SYS_KRHINO_CUR_TASK_GET]        = (void *)sys_krhino_cur_task_get_stub,
    [SYS_KRHINO_TASK_SLEEP]          = (void *)sys_krhino_task_sleep_stub,
    [SYS_KRHINO_TASK_SUSPEND]        = (void *)sys_krhino_task_suspend_stub,
    [SYS_KRHINO_TASK_RESUME]         = (void *)sys_krhino_task_resume_stub,
    [SYS_KRHINO_TASK_WAIT_ABORT]     = (void *)sys_krhino_task_wait_abort_stub,
    [SYS_KRHINO_TASK_YIELD]          = (void *)sys_krhino_task_yield_stub,
    [SYS_KRHINO_TASK_PRI_CHANGE]     = (void *)sys_krhino_task_pri_change_stub,
    [SYS_KRHINO_TASK_INFO_SET]       = (void *)sys_krhino_task_info_set_stub,
    [SYS_KRHINO_TASK_INFO_GET]       = (void *)sys_krhino_task_info_get_stub,
    [SYS_KRHINO_TASK_NAME_SET]       = (void *)sys_krhino_task_name_set_stub,
    [SYS_KRHINO_TASK_NAME_GET]       = (void *)sys_krhino_task_name_get_stub,
    [SYS_KRHINO_TASK_TIME_SLICE_GET] = (void *)sys_krhino_task_time_slice_get_stub,
    [SYS_KRHINO_TASK_TIME_SLICE_SET] = (void *)sys_krhino_task_time_slice_set_stub,
    [SYS_KRHINO_UTASK_CREATE]        = (void *)sys_krhino_utask_create_stub,
    [SYS_KRHINO_UTASK_DEL]           = (void *)sys_krhino_utask_del_stub,
    [SYS_KRHINO_UTASK_COUPLE_DATA]   = (void *)sys_krhino_utask_couple_data_stub,
    [SYS_KRHINO_TASK_PTCB_GET]       = (void *)sys_krhino_task_ptcb_get_stub,
    [SYS_KRHINO_TASK_PTCB_SET]       = (void *)sys_krhino_task_ptcb_set_stub,
    [SYS_KRHINO_TASK_PRI_GET]        = (void *)sys_krhino_task_pri_get_stub,
    [SYS_KRHINO_TASK_ID_GET]         = (void *)sys_krhino_task_id_get_stub,

    /* ------------------- sched ----------------------*/
    [SYS_KRHINO_SCHED_POLICY_GET] = (void *)sys_krhino_sched_policy_get_stub,
    [SYS_KRHINO_SCHED_POLICY_SET] = (void *)sys_krhino_sched_policy_set_stub,
    [SYS_KRHINO_SCHED_GET_PRIORITY_MAX] = (void *)sys_krhino_sched_get_priority_max_stub,
    [SYS_KRHINO_SCHED_GET_PRIORITY_MIN] = (void *)sys_krhino_sched_get_priority_min_stub,

    /* ------------------- porcess ----------------------*/
    [SYS_KRHINO_UPROCESS_CREATE]  = (void *)sys_krhino_uprocess_create_stub,
    [SYS_KRHINO_UPROCESS_EXIT]    = (void *)sys_krhino_uprocess_exit_stub,
    [SYS_KRHINO_UPROCESS_LOAD]    = (void *)sys_krhino_process_load_stub,
    [SYS_KRHINO_UPROCESS_RAMLOAD] = (void *)sys_krhino_process_ramload_stub,
    [SYS_KRHINO_UPROCESS_UNLOAD]  = (void *)sys_krhino_process_unload_stub,
    [SYS_KRHINO_UPROCESS_ID_GET]  = (void *)sys_krhino_uprocess_id_get_stub,

    /* ------------------ u_proc_msg -------------------*/
#if (RHINO_CONFIG_IPC > 0)
    [SYS_KRHINO_MSG_GET]        = (void *)sys_krhino_msg_get_stub,
    [SYS_KRHINO_MSG_SND]        = (void *)sys_krhino_msg_snd_stub,
    [SYS_KRHINO_MSG_RECV]       = (void *)sys_krhino_msg_recv_stub,
    [SYS_KRHINO_MSG_DEL]        = (void *)sys_krhino_msg_del_stub,
    [SYS_KRHINO_IPC_STREAM]     = (void *)sys_krhino_ipc_create_stream_stub,
    [SYS_KRHINO_IPC_FIFO]       = (void *)sys_krhino_ipc_create_fifo_stub,
    [SYS_KRHINO_IPC_MSG]        = (void *)sys_krhino_ipc_create_msg_stub,
    [SYS_KRHINO_IPC_SEND]       = (void *)sys_krhino_ipc_send_stub,
    [SYS_KRHINO_IPC_SENDTO]     = (void *)sys_krhino_ipc_sendto_stub,
    [SYS_KRHINO_IPC_SEND_OWNER] = (void *)sys_krhino_ipc_send_owner_stub,
    [SYS_KRHINO_IPC_RECV]       = (void *)sys_krhino_ipc_recv_stub,
    [SYS_KRHINO_IPC_CYCLE]      = (void *)sys_krhino_ipc_cycle_stub,
    [SYS_KRHINO_IPC_DEL]        = (void *)sys_krhino_ipc_del_stub,
    [SYS_KRHINO_IPC_SETOPT]     = (void *)sys_krhino_ipc_setopt_stub,
    [SYS_KRHINO_IPC_GETOPT]     = (void *)sys_krhino_ipc_getopt_stub,
    [SYS_KRHINO_IPC_PERF]       = (void *)sys_krhino_ipc_perf_stub,
#endif

#if (RHINO_CONFIG_USIGNAL > 0)
    /* ------------------ signal -------------------*/
    [SYS_SIGNAL_KILL]      = (void *)sys_aos_kill_stub,
    [SYS_SIGNAL_TKILL]     = (void *)sys_aos_tkill_stub,
    [SYS_SIGNAL_PENDING]   = (void *)sys_aos_sigpending_stub,
    [SYS_SIGNAL_PROCMASK]  = (void *)sys_aos_sigprocmask_stub,
    [SYS_SIGNAL_ACTION]    = (void *)sys_aos_sigaction_stub,
#endif

    /* ----------------  vfs  ------------------*/
#if (AOS_COMP_VFS > 0)
    [SYS_VFS_OPEN]      = (void *)sys_vfs_open_stub,
    [SYS_VFS_CLOSE]     = (void *)sys_vfs_close_stub,
    [SYS_VFS_READ]      = (void *)sys_vfs_read_stub,
    [SYS_VFS_WRITE]     = (void *)sys_vfs_write_stub,
    [SYS_VFS_IOCTL]     = (void *)sys_vfs_ioctl_stub,
    [SYS_VFS_LSEEK]     = (void *)sys_vfs_lseek_stub,
    [SYS_VFS_FSYNC]     = (void *)sys_vfs_fsync_stub,
    [SYS_VFS_SYNC]      = (void *)sys_vfs_sync_stub,
    [SYS_VFS_STAT]      = (void *)sys_vfs_stat_stub,
    [SYS_VFS_FSTAT]     = (void *)sys_vfs_fstat_stub,
    [SYS_VFS_MMAP]      = (void *)sys_vfs_mmap_stub,
    [SYS_VFS_MUNMAP]    = (void *)sys_vfs_munmap_stub,
    [SYS_VFS_LINK]      = (void *)sys_vfs_link_stub,
    [SYS_VFS_UNLINK]    = (void *)sys_vfs_unlink_stub,
    [SYS_VFS_REMOVE]    = (void *)sys_vfs_remove_stub,
    [SYS_VFS_RENAME]    = (void *)sys_vfs_rename_stub,
    [SYS_VFS_OPENDIR]   = (void *)sys_vfs_opendir_stub,
    [SYS_VFS_DIRFD]     = (void *)sys_vfs_dirfd_stub,
    [SYS_VFS_CLOSEDIR]  = (void *)sys_vfs_closedir_stub,
    [SYS_VFS_READDIR]   = (void *)sys_vfs_readdir_stub,
    [SYS_VFS_MKDIR]     = (void *)sys_vfs_mkdir_stub,
    [SYS_VFS_RMDIR]     = (void *)sys_vfs_rmdir_stub,
    [SYS_VFS_REWINDDIR] = (void *)sys_vfs_rewinddir_stub,
    [SYS_VFS_TELLDIR]   = (void *)sys_vfs_telldir_stub,
    [SYS_VFS_SEEKDIR]   = (void *)sys_vfs_seekdir_stub,
    [SYS_VFS_STATFS]    = (void *)sys_vfs_statfs_stub,
    [SYS_VFS_ACCESS]    = (void *)sys_vfs_access_stub,
    [SYS_VFS_CHDIR]     = (void *)sys_vfs_chdir_stub,
    [SYS_VFS_GETCWD]    = (void *)sys_vfs_getcwd_stub,
    [SYS_VFS_PATHCONF]  = (void *)sys_vfs_pathconf_stub,
    [SYS_VFS_FPATHCONF] = (void *)sys_vfs_fpathconf_stub,
    [SYS_VFS_UTIME]     = (void *)sys_vfs_utime_stub,
    [SYS_VFS_FCNTL]     = (void *)sys_vfs_fcntl_stub,
#if (AOS_COMP_EPOLL > 0)
    [SYS_EPOLL_CREATE1] = (void *)sys_epoll_create1_stub,
    [SYS_EPOLL_CTL]     = (void *)sys_epoll_ctl_stub,
    [SYS_EPOLL_WAIT]    = (void *)sys_epoll_wait_stub,
#endif
#if (AOS_COMP_SELECT > 0)
    [SYS_AOS_POLL]      = (void *)sys_aos_poll_stub,
    [SYS_AOS_SELECT]    = (void *)sys_aos_select_stub,
#endif
#endif

    /* ---------------- network --------------------*/
    [SYS_NETWORK_ACCEPT]      = (void *)sys_network_accept_stub,
    [SYS_NETWORK_BIND]        = (void *)sys_network_bind_stub,
    [SYS_NETWORK_SHUTDOWN]    = (void *)sys_network_shutdown_stub,
    [SYS_NETWORK_GETPEERNAME] = (void *)sys_network_getpeername_stub,
    [SYS_NETWORK_GETSOCKNAME] = (void *)sys_network_getsockname_stub,
    [SYS_NETWORK_GETSOCKOPT]  = (void *)sys_network_getsockopt_stub,
    [SYS_NETWORK_SETSOCKET]   = (void *)sys_network_setsockopt_stub,
    [SYS_NETWORK_CLOSE]       = (void *)sys_network_close_stub,
    [SYS_NETWORK_CONNECT]     = (void *)sys_network_connect_stub,
    [SYS_NETWORK_LISTEN]      = (void *)sys_network_listen_stub,
    [SYS_NETWORK_RECV]        = (void *)sys_network_recv_stub,
    [SYS_NETWORK_READ]        = (void *)sys_network_read_stub,
    [SYS_NETWORK_RECVFROM]    = (void *)sys_network_recvfrom_stub,
    [SYS_NETWORK_SEND]        = (void *)sys_network_send_stub,
    [SYS_NETWORK_SENDMSG]     = (void *)sys_network_sendmsg_stub,
    [SYS_NETWORK_SENDTO]      = (void *)sys_network_sendto_stub,
    [SYS_NETWORK_SOCKET]      = (void *)sys_network_socket,
    [SYS_NETWORK_WRITE]       = (void *)sys_network_write_stub,
    [SYS_NETWORK_WRITEV]      = (void *)sys_network_writev_stub,
    [SYS_NETWORK_SELECT]      = (void *)sys_network_select_stub,
    [SYS_NETWORK_IOCTL]       = (void *)sys_network_ioctl_stub,
    [SYS_NETWORK_FCNTL]       = (void *)sys_network_fcntl_stub,
    [SYS_NETWORK_EVENTFD]     = (void *)sys_network_eventfd_stub,
    [SYS_NETWORK_TRY_WAKEUP]  = (void *)sys_network_try_wakeup_stub,
    [SYS_NETWORK_GETHOSTBYNAME]      = (void *)sys_network_gethostbyname_stub,
    [SYS_NETWORK_GETHOSTBYNAME_R]    = (void *)sys_network_gethostbyname_r_stub,
    [SYS_NETWORK_FREEADDRINFO]       = (void *)sys_network_freeaddrinfo_stub,
    [SYS_NETWORK_GETADDRINOF]        = (void *)sys_network_getaddrinfo_stub,
    /* ---------------- netmgr --------------------*/
#if (AOS_COMP_NETMGR > 0)
    [SYS_NETMGR_SERVICE_INIT]                  = (void *)sys_netmgr_service_init_stub,
    [SYS_NETMGR_SERVICE_DEINIT]                = (void *)sys_netmgr_service_deinit_stub,
    [SYS_NETMGR_GET_DEV]                       = (void *)sys_netmgr_get_dev_stub,
    [SYS_NETMGR_SET_IFCONFIG]                  = (void *)sys_netmgr_set_ifconfig_stub,
    [SYS_NETMGR_GET_IFCONFIG]                  = (void *)sys_netmgr_get_ifconfig_stub,
    [SYS_NETMGR_SET_AUTO_RECONNECT]            = (void *)sys_netmgr_set_auto_reconnect_stub,
    [SYS_NETMGR_GET_CONFIG]                    = (void *)sys_netmgr_get_config_stub,
    [SYS_NETMGR_DEL_CONFIG]                    = (void *)sys_netmgr_del_config_stub,
    [SYS_NETMGR_GET_STATE]                     = (void *)sys_netmgr_get_state_stub,
    [SYS_NETMGR_CONNECT]                       = (void *)sys_netmgr_connect_stub,
    [SYS_NETMGR_DISCONNECT]                    = (void *)sys_netmgr_disconnect_stub,
    [SYS_NETMGR_SAVE_CONFIG]                   = (void *)sys_netmgr_save_config_stub,
    [SYS_NETMGR_SET_CONNECT_PARAMS]            = (void *)sys_netmgr_set_connect_params_stub,
    [SYS_NETMGR_SET_MSG_CB]                    = (void *)sys_netmgr_set_msg_cb_stub,
    [SYS_NETMGR_DEL_MSG_CB]                    = (void *)sys_netmgr_del_msg_cb_stub,
#if AOS_NETWORK_WITH_WIFI
    [SYS_NETMGR_WIFI_SCAN_RESULT]              = (void *)sys_netmgr_wifi_scan_result_stub,
    [SYS_NETMGR_WIFI_START_MGNT_MONITOR]       = (void *)sys_netmgr_wifi_start_mgnt_monitor_stub,
    [SYS_NETMGR_WIFI_STOP_MGNT_MONITOR]        = (void *)sys_netmgr_wifi_stop_mgnt_monitor_stub,
    [SYS_NETMGR_WIFI_REGISTER_MGNT_MONITOR_CB] = (void *)sys_netmgr_wifi_register_mgnt_monitor_cb_stub,
    [SYS_NETMGR_WIFI_SEND_80211_RAW_FRAME]     = (void *)sys_netmgr_wifi_send_80211_raw_frame_stub,
#endif
#endif /*RHINO_CONFIG_NETMGR_SYSCALL*/

    /* ----------------   cli  ------------------*/
#if (AOS_COMP_CLI > 0)
    [SYS_CLI_REGISTER_CMD]     = (void *)sys_cli_register_command_stub,
    [SYS_CLI_UNREGISTER_CMD]   = (void *)sys_cli_unregister_command_stub,
    [SYS_CLI_REGISTER_CMDS]    = (void *)sys_cli_register_commands_stub,
    [SYS_CLI_UNREGISTER_CMDS]  = (void *)sys_cli_unregister_commands_stub,
    [SYS_CLI_TASK_SET_CONSOLE] = (void *)sys_cli_task_set_console_stub,
#endif /* AOS_COMP_CLI */

    /* ----------------  kv  ------------------*/
#if (AOS_COMP_KV > 0)
    [SYS_KV_SET] = (void *)sys_kv_set_stub,
    [SYS_KV_GET] = (void *)sys_kv_get_stub,
    [SYS_KV_DEL] = (void *)sys_kv_del_stub,
#endif /* AOS_COMP_KV */
    /* ----------------  debug  ------------------*/
    [SYS_BACKTRACE_NOW]                  = (void *)sys_backtrace_now_stub,
    [SYS_DEBUG_FATAL_ERROR]              = (void *)sys_debug_fatal_error_stub,
    [SYS_DEBUG_CPU_STOP]                 = (void *)sys_debug_cpu_stop,
    [SYS_DEBUG_MM_SET_HEAD]              = (void *)sys_debug_mm_set_head,

    /* ------------------- hal uart ---------------------*/
    [SYS_HAL_UART_INIT]     = (void *)sys_hal_uart_init_stub,
    [SYS_HAL_UART_SEND]     = (void *)sys_hal_uart_send_stub,
    [SYS_HAL_UART_RECV]     = (void *)sys_hal_uart_recv_stub,
    [SYS_HAL_UART_RECV_II]  = (void *)sys_hal_uart_recv_II_stub,
    [SYS_HAL_UART_FINALIZE] = (void *)sys_hal_uart_finalize_stub,

    [SYS_HAL_FLASH_INFO_GET]      = (void *)sys_hal_flash_info_get_stub,
    [SYS_HAL_FLASH_ERASE]         = (void *)sys_hal_flash_erase_stub,
    [SYS_HAL_FLASH_WRITE]         = (void *)sys_hal_flash_write_stub,
    [SYS_HAL_FLASH_READ]          = (void *)sys_hal_flash_read_stub,
    [SYS_HAL_FLASH_ENABLE_SECURE] = (void *)sys_hal_flash_enable_secure_stub,
    [SYS_HAL_FLASH_DIS_SECURE]    = (void *)sys_hal_flash_dis_secure_stub,

    [SYS_HAL_WDG_INIT]            = (void *)sys_hal_wdg_init_stub,
    [SYS_HAL_WDG_RELOAD]          = (void *)sys_hal_wdg_reload_stub,
    [SYS_HAL_WDG_FINALIZE]        = (void *)sys_hal_wdg_finalize_stub,

    /* ---------------- end ------------------*/
};


