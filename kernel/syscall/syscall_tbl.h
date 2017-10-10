/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

/* --------------------Rhino-------------------- */

#define SYS_REBOOT 0
SYSCALL(SYS_REBOOT, aos_reboot)

#define SYS_GET_HZ 1
SYSCALL(SYS_GET_HZ, aos_get_hz)

#define SYS_VERSION_GET 2
SYSCALL(SYS_VERSION_GET, aos_version_get)

#define SYS_TASK_NEW 3
SYSCALL(SYS_TASK_NEW, aos_task_new)

#define SYS_TASK_NEW_EXT 4
SYSCALL(SYS_TASK_NEW_EXT, aos_task_new_ext)

#define SYS_TASK_EXIT 5
SYSCALL(SYS_TASK_EXIT, aos_task_exit)

#define SYS_TASK_NAME 6
SYSCALL(SYS_TASK_NAME, aos_task_name)

#define SYS_TASK_KEY_CREATE 7
SYSCALL(SYS_TASK_KEY_CREATE, aos_task_key_create)

#define SYS_TASK_KEY_DELETE 8
SYSCALL(SYS_TASK_KEY_DELETE, aos_task_key_delete)

#define SYS_TASK_SETSPECIFIC 9
SYSCALL(SYS_TASK_SETSPECIFIC, aos_task_setspecific)

#define SYS_TASK_GETSPECIFIC 10
SYSCALL(SYS_TASK_GETSPECIFIC, aos_task_getspecific)

#define SYS_MUTEX_NEW 11
SYSCALL(SYS_MUTEX_NEW, aos_mutex_new)

#define SYS_MUTEX_FREE 12
SYSCALL(SYS_MUTEX_FREE, aos_mutex_free)

#define SYS_MUTEX_LOCK 13
SYSCALL(SYS_MUTEX_LOCK, aos_mutex_lock)

#define SYS_MUTEX_UNLOCK 14
SYSCALL(SYS_MUTEX_UNLOCK, aos_mutex_unlock)

#define SYS_SEM_NEW 15
SYSCALL(SYS_SEM_NEW, aos_sem_new)

#define SYS_SEM_FREE 16
SYSCALL(SYS_SEM_FREE, aos_sem_free)

#define SYS_SEM_WAIT 17
SYSCALL(SYS_SEM_WAIT, aos_sem_wait)

#define SYS_SEM_SIGNAL 18
SYSCALL(SYS_SEM_SIGNAL, aos_sem_signal)

#define SYS_QUEUE_NEW 19
SYSCALL(SYS_QUEUE_NEW, aos_queue_new)

#define SYS_QUEUE_FREE 20
SYSCALL(SYS_QUEUE_FREE, aos_queue_free)

#define SYS_QUEUE_SEND 21
SYSCALL(SYS_QUEUE_SEND, aos_queue_send)

#define SYS_QUEUE_RECV 22
SYSCALL(SYS_QUEUE_RECV, aos_queue_recv)

#define SYS_TIMER_NEW 23
SYSCALL(SYS_TIMER_NEW, aos_timer_new)

#define SYS_TIMER_FREE 24
SYSCALL(SYS_TIMER_FREE, aos_timer_free)

#define SYS_TIMER_START 25
SYSCALL(SYS_TIMER_START, aos_timer_start)

#define SYS_TIMER_STOP 26
SYSCALL(SYS_TIMER_STOP, aos_timer_stop)

#define SYS_TIMER_CHANGE 27
SYSCALL(SYS_TIMER_CHANGE, aos_timer_change)

#if (RHINO_CONFIG_WORKQUEUE > 0)
#define SYS_WORKQUEUE_CREATE 28
SYSCALL(SYS_WORKQUEUE_CREATE, aos_workqueue_create)

#define SYS_WORKQUEUE_DEL 29
SYSCALL(SYS_WORKQUEUE_DEL, aos_workqueue_del)

#define SYS_WORK_INIT 30
SYSCALL(SYS_WORK_INIT, aos_work_init)

#define SYS_WORK_DESTROY 31
SYSCALL(SYS_WORK_DESTROY, aos_work_destroy)

#define SYS_WORK_RUN 32
SYSCALL(SYS_WORK_RUN, aos_work_run)

#define SYS_WORK_SCHED 33
SYSCALL(SYS_WORK_SCHED, aos_work_sched)

#define SYS_WORK_CANCEL 34
SYSCALL(SYS_WORK_CANCEL, aos_work_cancel)
#endif

#if (RHINO_CONFIG_MM_DEBUG > 0u && RHINO_CONFIG_GCC_RETADDR > 0u)

#define SYS_MALLOC 35
SYSCALL(SYS_MALLOC, sys_aos_malloc)

#define SYS_REALLOC 36
SYSCALL(SYS_REALLOC, sys_aos_realloc)

#define SYS_ZALLOC 37
SYSCALL(SYS_ZALLOC, sys_aos_zalloc)

#else

#define SYS_MALLOC 35
SYSCALL(SYS_MALLOC, aos_malloc)

#define SYS_REALLOC 36
SYSCALL(SYS_REALLOC, aos_realloc)

#define SYS_ZALLOC 37
SYSCALL(SYS_ZALLOC, aos_zalloc)

#endif

#define SYS_ALLOC_TRACE 38
SYSCALL(SYS_ALLOC_TRACE, aos_alloc_trace)

#define SYS_FREE 39
SYSCALL(SYS_FREE, aos_free)

#define SYS_NOW 40
SYSCALL(SYS_NOW, aos_now)

#define SYS_NOW_MS 41
SYSCALL(SYS_NOW_MS, aos_now_ms)

#define SYS_MSLEEP 42
SYSCALL(SYS_MSLEEP, aos_msleep)

/* keep 43~50 for Rhino */

/* --------------------Framework-------------------- */

#define SYS_REGISTER_EVENT_FILTER 51
SYSCALL(SYS_REGISTER_EVENT_FILTER, aos_register_event_filter)

#define SYS_UNREGISTER_EVENT_FILTER 52
SYSCALL(SYS_UNREGISTER_EVENT_FILTER, aos_unregister_event_filter)

#define SYS_POST_EVENT 53
SYSCALL(SYS_POST_EVENT, aos_post_event)

#define SYS_POLL_READ_FD 54
SYSCALL(SYS_POLL_READ_FD, aos_poll_read_fd)

#define SYS_CANCEL_POLL_READ_FD 55
SYSCALL(SYS_CANCEL_POLL_READ_FD, aos_cancel_poll_read_fd)

#define SYS_POST_DELAYED_ACTION 56
SYSCALL(SYS_POST_DELAYED_ACTION, aos_post_delayed_action)

#define SYS_CANCEL_DELAYED_ACTION 57
SYSCALL(SYS_CANCEL_DELAYED_ACTION, aos_cancel_delayed_action)

#define SYS_SCHEDULE_CALL 58
SYSCALL(SYS_SCHEDULE_CALL, aos_schedule_call)

#define SYS_LOOP_INIT 59
SYSCALL(SYS_LOOP_INIT, aos_loop_init)

#define SYS_CURRENT_LOOP 60
SYSCALL(SYS_CURRENT_LOOP, aos_current_loop)

#define SYS_LOOP_RUN 61
SYSCALL(SYS_LOOP_RUN, aos_loop_run)

#define SYS_LOOP_EXIT 62
SYSCALL(SYS_LOOP_EXIT, aos_loop_exit)

#define SYS_LOOP_DESTROY 63
SYSCALL(SYS_LOOP_DESTROY, aos_loop_destroy)

#define SYS_LOOP_SCHEDULE_CALL 64
SYSCALL(SYS_LOOP_SCHEDULE_CALL, aos_loop_schedule_call)

#define SYS_LOOP_SCHEDULE_WORK 65
SYSCALL(SYS_LOOP_SCHEDULE_WORK, aos_loop_schedule_work)

#define SYS_CANCEL_WORK 66
SYSCALL(SYS_CANCEL_WORK, aos_cancel_work)

#define SYS_KV_SET 67
SYSCALL(SYS_KV_SET, aos_kv_set)

#define SYS_KV_GET 68
SYSCALL(SYS_KV_GET, aos_kv_get)

#define SYS_KV_DEL 69
SYSCALL(SYS_KV_DEL, aos_kv_del)

#define SYS_OPEN 70
SYSCALL(SYS_OPEN, aos_open)

#define SYS_CLOSE 71
SYSCALL(SYS_CLOSE, aos_close)

#define SYS_READ 72
SYSCALL(SYS_READ, aos_read)

#define SYS_WRITE 73
SYSCALL(SYS_WRITE, aos_write)

#define SYS_IOCTL 74
SYSCALL(SYS_IOCTL, aos_ioctl)

#define SYS_POLL 75
SYSCALL(SYS_POLL, aos_poll)

#define SYS_FCNTL 76
SYSCALL(SYS_FCNTL, aos_fcntl)

#define SYS_LSEEK 77
SYSCALL(SYS_LSEEK, aos_lseek)

#define SYS_SYNC 78
SYSCALL(SYS_SYNC, aos_sync)

#define SYS_STAT 79
SYSCALL(SYS_STAT, aos_stat)

#define SYS_UNLINK 80
SYSCALL(SYS_UNLINK, aos_unlink)

#define SYS_RENAME 81
SYSCALL(SYS_RENAME, aos_rename)

#define SYS_OPENDIR 82
SYSCALL(SYS_OPENDIR, aos_opendir)

#define SYS_CLOSEDIR 83
SYSCALL(SYS_CLOSEDIR, aos_closedir)

#define SYS_READDIR 84
SYSCALL(SYS_READDIR, aos_readdir)

#define SYS_MKDIR 85
SYSCALL(SYS_MKDIR, aos_mkdir)

/* keep 86~90 for Framework */

/* --------------------MBEDTLS-------------------- */

#ifdef MBEDTLS_IN_KERNEL

#define SYS_MBEDTLS_CONNECT 91
SYSCALL(SYS_MBEDTLS_CONNECT, mbedtls_ssl_connect)

#define SYS_MBEDTLS_SEND 92
SYSCALL(SYS_MBEDTLS_SEND, mbedtls_ssl_send)

#define SYS_MBEDTLS_RECV 93
SYSCALL(SYS_MBEDTLS_RECV, mbedtls_ssl_recv)

#define SYS_MBEDTLS_CLOSE 94
SYSCALL(SYS_MBEDTLS_CLOSE, mbedtls_ssl_close)

#define SYS_CRYPTO_AES_INIT 95
SYSCALL(SYS_CRYPTO_AES_INIT, ali_aes_init)

#define SYS_CRYPTO_AES_FINISH 96
SYSCALL(SYS_CRYPTO_AES_FINISH, ali_aes_finish)

#define SYS_CRYPTO_AES_GET_CTX_SIZE 97
SYSCALL(SYS_CRYPTO_AES_GET_CTX_SIZE, ali_aes_get_ctx_size)

#endif /* MBEDTLS_IN_KERNEL */

/* --------------------LWIP-------------------- */

#ifdef WITH_LWIP

#define SYS_LWIP_ACCEPT 101
SYSCALL(SYS_LWIP_ACCEPT, lwip_accept)

#define SYS_LWIP_BIND 102
SYSCALL(SYS_LWIP_BIND, lwip_bind)

#define SYS_LWIP_SHUTDOWN 103
SYSCALL(SYS_LWIP_SHUTDOWN, lwip_shutdown)

#define SYS_LWIP_GETPEERNAME 104
SYSCALL(SYS_LWIP_GETPEERNAME, lwip_getpeername)

#define SYS_LWIP_GETSOCKNAME 105
SYSCALL(SYS_LWIP_GETSOCKNAME, lwip_getsockname)

#define SYS_LWIP_GETSOCKOPT 106
SYSCALL(SYS_LWIP_GETSOCKOPT, lwip_getsockopt)

#define SYS_LWIP_SETSOCKOPT 107
SYSCALL(SYS_LWIP_SETSOCKOPT, lwip_setsockopt)

#define SYS_LWIP_CLOSE 108
SYSCALL(SYS_LWIP_CLOSE, lwip_close)

#define SYS_LWIP_CONNECT 109
SYSCALL(SYS_LWIP_CONNECT, lwip_connect)

#define SYS_LWIP_LISTEN 110
SYSCALL(SYS_LWIP_LISTEN, lwip_listen)

#define SYS_LWIP_RECV 111
SYSCALL(SYS_LWIP_RECV, lwip_recv)

#define SYS_LWIP_READ 112
SYSCALL(SYS_LWIP_READ, lwip_read)

#define SYS_LWIP_RECVFROM 113
SYSCALL(SYS_LWIP_RECVFROM, lwip_recvfrom)

#define SYS_LWIP_SEND 114
SYSCALL(SYS_LWIP_SEND, lwip_send)

#define SYS_LWIP_SENDMSG 115
SYSCALL(SYS_LWIP_SENDMSG, lwip_sendmsg)

#define SYS_LWIP_SENDTO 116
SYSCALL(SYS_LWIP_SENDTO, lwip_sendto)

#define SYS_LWIP_SOCKET 117
SYSCALL(SYS_LWIP_SOCKET, lwip_socket)

#define SYS_LWIP_WRITE 118
SYSCALL(SYS_LWIP_WRITE, lwip_write)

#define SYS_LWIP_WRITEV 119
SYSCALL(SYS_LWIP_WRITEV, lwip_writev)

#define SYS_LWIP_SELECT 120
SYSCALL(SYS_LWIP_SELECT, lwip_select)

#define SYS_LWIP_IOCTL 121
SYSCALL(SYS_LWIP_IOCTL, lwip_ioctl)

#define SYS_LWIP_FCNTL 122
SYSCALL(SYS_LWIP_FCNTL, lwip_fcntl)

#define SYS_LWIP_EVENTFD 123
SYSCALL(SYS_LWIP_EVENTFD, lwip_eventfd)

#define SYS_LWIP_HTONL 124
SYSCALL(SYS_LWIP_HTONL, lwip_htonl)

#define SYS_LWIP_HTONS 125
SYSCALL(SYS_LWIP_HTONS, lwip_htons)

#define SYS_LWIP_GETHOSTBYNAME 126
SYSCALL(SYS_LWIP_GETHOSTBYNAME, lwip_gethostbyname)

#define SYS_LWIP_IP4ADDR_ATON 127
SYSCALL(SYS_LWIP_IP4ADDR_ATON, ip4addr_aton)

#define SYS_LWIP_IP4ADDR_NTOA 128
SYSCALL(SYS_LWIP_IP4ADDR_NTOA, ip4addr_ntoa)

#define SYS_LWIP_IPADDR_ADDR 129
SYSCALL(SYS_LWIP_IPADDR_ADDR, ipaddr_addr)

#define SYS_LWIP_FREEADDRINFO 130
SYSCALL(SYS_LWIP_FREEADDRINFO, lwip_freeaddrinfo)

#define SYS_LWIP_GETADDRINFO 131
SYSCALL(SYS_LWIP_GETADDRINFO, lwip_getaddrinfo)

/* keep 132~135 for LWIP */

#endif /* WITH_LWIP */

/* --------------------WIFI-------------------- */

#define SYS_WIFI_GET_DEFAULT_MODULE 136
SYSCALL(SYS_WIFI_GET_DEFAULT_MODULE, hal_wifi_get_default_module)

#define SYS_WIFI_REGISTER_MODULE 137
SYSCALL(SYS_WIFI_REGISTER_MODULE, hal_wifi_register_module)

#define SYS_WIFI_INIT 138
SYSCALL(SYS_WIFI_INIT, hal_wifi_init)

#define SYS_WIFI_GET_MAC_ADDR 139
SYSCALL(SYS_WIFI_GET_MAC_ADDR, hal_wifi_get_mac_addr)

#define SYS_WIFI_START 140
SYSCALL(SYS_WIFI_START, hal_wifi_start)

#define SYS_WIFI_START_ADV 141
SYSCALL(SYS_WIFI_START_ADV, hal_wifi_start_adv)

#define SYS_WIFI_GET_IP_STAT 142
SYSCALL(SYS_WIFI_GET_IP_STAT, hal_wifi_get_ip_stat)

#define SYS_WIFI_GET_LINK_STAT 143
SYSCALL(SYS_WIFI_GET_LINK_STAT, hal_wifi_get_link_stat)

#define SYS_WIFI_START_SCAN 144
SYSCALL(SYS_WIFI_START_SCAN, hal_wifi_start_scan)

#define SYS_WIFI_START_SCAN_ADV 145
SYSCALL(SYS_WIFI_START_SCAN_ADV, hal_wifi_start_scan_adv)

#define SYS_WIFI_POWER_OFF 146
SYSCALL(SYS_WIFI_POWER_OFF, hal_wifi_power_off)

#define SYS_WIFI_POWER_ON 147
SYSCALL(SYS_WIFI_POWER_ON, hal_wifi_power_on)

#define SYS_WIFI_SUSPEND 148
SYSCALL(SYS_WIFI_SUSPEND, hal_wifi_suspend)

#define SYS_WIFI_SUSPEND_STATION 149
SYSCALL(SYS_WIFI_SUSPEND_STATION, hal_wifi_suspend_station)

#define SYS_WIFI_SUSPEND_SOFT_AP 150
SYSCALL(SYS_WIFI_SUSPEND_SOFT_AP, hal_wifi_suspend_soft_ap)

#define SYS_WIFI_SET_CHANNEL 151
SYSCALL(SYS_WIFI_SET_CHANNEL, hal_wifi_set_channel)

#define SYS_WIFI_START_WIFI_MONITOR 152
SYSCALL(SYS_WIFI_START_WIFI_MONITOR, hal_wifi_start_wifi_monitor)

#define SYS_WIFI_STOP_WIFI_MONITOR 153
SYSCALL(SYS_WIFI_STOP_WIFI_MONITOR, hal_wifi_stop_wifi_monitor)

#define SYS_WIFI_REGISTER_MONITOR_CB 154
SYSCALL(SYS_WIFI_REGISTER_MONITOR_CB, hal_wifi_register_monitor_cb)

#define SYS_WIFI_INSTALL_EVENT 155
SYSCALL(SYS_WIFI_INSTALL_EVENT, hal_wifi_install_event)

#define SYS_HAL_WLAN_REG_MGNT_MONITOR_CB 156
SYSCALL(SYS_HAL_WLAN_REG_MGNT_MONITOR_CB, hal_wlan_register_mgnt_monitor_cb)

#define SYS_HAL_WLAN_SEND_80211_RAW_FRAME 157
SYSCALL(SYS_HAL_WLAN_SEND_80211_RAW_FRAME, hal_wlan_send_80211_raw_frame)

/* --------------------MESH-------------------- */

#ifdef CONFIG_AOS_MESH
#define SYS_UR_MESH_INIT 158
SYSCALL(SYS_UR_MESH_INIT, umesh_init)

#define SYS_UR_MESH_START 159
SYSCALL(SYS_UR_MESH_START, umesh_start)

#define SYS_UR_MESH_STOP 160
SYSCALL(SYS_UR_MESH_STOP, umesh_stop)

#define SYS_UR_MESH_SET_MODE 161
SYSCALL(SYS_UR_MESH_SET_MODE, umesh_set_mode)

#define SYS_UR_MESH_GET_MODE 162
SYSCALL(SYS_UR_MESH_GET_MODE, umesh_get_mode)

#define SYS_UR_MESH_GET_MCAST_ADDR 163
SYSCALL(SYS_UR_MESH_GET_MCAST_ADDR, umesh_get_mcast_addr)

#define SYS_UR_MESH_GET_UCAST_ADDR 164
SYSCALL(SYS_UR_MESH_GET_UCAST_ADDR, umesh_get_ucast_addr)

#define SYS_UR_MESH_NET_GET_MAC_ADDRESS 165
SYSCALL(SYS_UR_MESH_NET_GET_MAC_ADDRESS, umesh_get_mac_address)

#define SYS_UR_MESH_GET_DEVICE_STATE 166
SYSCALL(SYS_UR_MESH_GET_DEVICE_STATE, umesh_get_device_state)
#endif


/* --------------------OTA-------------------- */

#define SYS_OTA_REGISTER_MODULE 171
SYSCALL(SYS_OTA_REGISTER_MODULE, hal_ota_register_module)

#define SYS_OTA_INIT 172
SYSCALL(SYS_OTA_INIT, hal_ota_init)

#define SYS_OTA_WRITE 173
SYSCALL(SYS_OTA_WRITE, hal_ota_write)

#define SYS_OTA_READ 174
SYSCALL(SYS_OTA_READ, hal_ota_read)

#define SYS_OTA_SET_BOOT 175
SYSCALL(SYS_OTA_SET_BOOT, hal_ota_set_boot)

#define SYS_OTA_GET_DEFAULT_MODULE 176
SYSCALL(SYS_OTA_GET_DEFAULT_MODULE, hal_ota_get_default_module)

/* keep 177~180 for OTA */

/* --------------------CLI-------------------- */
#define SYS_CLI_REG_CMD 181
SYSCALL(SYS_CLI_REG_CMD, aos_cli_register_command)

#define SYS_CLI_UNREG_CMD 182
SYSCALL(SYS_CLI_UNREG_CMD, aos_cli_unregister_command)

#define SYS_CLI_REG_CMDS 183
SYSCALL(SYS_CLI_REG_CMDS, aos_cli_register_commands)

#define SYS_CLI_UNREG_CMDS 184
SYSCALL(SYS_CLI_UNREG_CMDS, aos_cli_unregister_commands)

#define SYS_CLI_INIT 185
SYSCALL(SYS_CLI_INIT, aos_cli_init)

#define SYS_CLI_STOP 186
SYSCALL(SYS_CLI_STOP, aos_cli_stop)

/* --------------------OTHERS-------------------- */
#define SYS_UART_SEND 187
SYSCALL(SYS_UART_SEND, aos_uart_send)

#define SYS_GET_ERRNO 188
SYSCALL(SYS_GET_ERRNO, get_errno)

#define SYS_SET_ERRNO 189
SYSCALL(SYS_SET_ERRNO, set_errno)


