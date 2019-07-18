/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#include "k_config.h"
#include "syscall_no.h"
#include "rhino_ksyscall.h"
#if (RHINO_CONFIG_HAL_ADC_SYSCALL > 0)
#include "hal_adc_ksyscall.h"
#endif
#if (RHINO_CONFIG_HAL_DAC_SYSCALL > 0)
#include "hal_dac_ksyscall.h"
#endif
#if (RHINO_CONFIG_HAL_FLASH_SYSCALL > 0)
#include "hal_flash_ksyscall.h"
#endif
#if (RHINO_CONFIG_HAL_GPIO_SYSCALL > 0)
#include "hal_gpio_ksyscall.h"
#endif
#if (RHINO_CONFIG_HAL_I2C_SYSCALL > 0)
#include "hal_i2c_ksyscall.h"
#endif
#if (RHINO_CONFIG_HAL_NAND_SYSCALL > 0)
#include "hal_nand_ksyscall.h"
#endif
#if (RHINO_CONFIG_HAL_NOR_SYSCALL > 0)
#include "hal_nor_ksyscall.h"
#endif
#if (RHINO_CONFIG_HAL_RTC_SYSCALL > 0)
#include "hal_rtc_ksyscall.h"
#endif
#if (RHINO_CONFIG_HAL_SPI_SYSCALL > 0)
#include "hal_spi_ksyscall.h"
#endif
#if (RHINO_CONFIG_HAL_SD_SYSCALL > 0)
#include "hal_sd_ksyscall.h"
#endif
#if (RHINO_CONFIG_HAL_UART_SYSCALL > 0)
#include "hal_uart_ksyscall.h"
#endif
#if (RHINO_CONFIG_UCLI_SYSCALL > 0)
#include "cli_ksyscall.h"
#endif
#if (RHINO_CONFIG_LWIP_SYSCALL > 0)
#include "lwip_ksyscall.h"
#endif
#if (RHINO_CONFIG_LINKKIT_SYSCALL > 0)
#include "linkkit_ksyscall.h"
#endif

/**************************************************************
 *   syscall table
 *************************************************************/
#if defined(__CC_ARM) || defined (__GNUC__)
#if (RHINO_CONFIG_DYN_SYSCALL_TBL > 0)
const void *syscall_tbl[] =
#else
const void * const syscall_tbl[] =
#endif
#elif defined (__ICCARM__)
#if (RHINO_CONFIG_DYN_SYSCALL_TBL > 0)
__root void *syscall_tbl[] =
#else
const __root void * const syscall_tbl[] =
#endif
#endif
{
    /* ------------------- task ----------------------*/
    [SYS_KRHINO_CUR_TASK_GET]    = (void*)sys_krhino_cur_task_get_stub,
    [SYS_KRHINO_TASK_SLEEP]      = (void*)sys_krhino_task_sleep_stub,
    [SYS_KRHINO_TASK_SUSPEND]    = (void*)sys_krhino_task_suspend_stub,
    [SYS_KRHINO_TASK_RESUME]     = (void*)sys_krhino_task_resume_stub,
#if (RHINO_CONFIG_TASK_WAIT_ABORT > 0)
    [SYS_KRHINO_TASK_WAIT_ABORT] = (void*)sys_krhino_task_wait_abort_stub,
#endif /* RHINO_CONFIG_TASK_WAIT_ABORT */
    [SYS_KRHINO_UTASK_CREATE]    = (void*)sys_krhino_utask_create_stub,
    [SYS_KRHINO_UTASK_DEL]       = (void*)sys_krhino_utask_del_stub,

    /* ------------------- porcess ----------------------*/
    [SYS_KRHINO_UPROCESS_CREATE]  = (void*)sys_krhino_uprocess_create_stub,
    [SYS_KRHINO_UPROCESS_EXIT]    = (void*)sys_krhino_uprocess_exit_stub,
    [SYS_KRHINO_UPROCESS_RES_GET] = (void*)sys_krhino_uprocess_res_get_stub,

    /* ------------------- time ----------------------*/
    [SYS_KRHINO_SYS_TIME_GET] = (void*)sys_krhino_sys_time_get_stub,
    [SYS_KRHINO_SYS_TICK_GET] = (void*)sys_krhino_sys_tick_get_stub,
    [SYS_KRHINO_MS_TO_TICKS]  = (void*)sys_krhino_ms_to_ticks_stub,
    [SYS_KRHINO_TICKS_TO_MS]  = (void*)sys_krhino_ticks_to_ms_stub,

    /* ------------------- mutex ---------------------*/
    [SYS_KRHINO_MUTEX_DYN_CREATE] = (void*)sys_krhino_mutex_dyn_create_stub,
    [SYS_KRHINO_MUTEX_DYN_DEL]    = (void*)sys_krhino_mutex_dyn_del_stub,
    [SYS_KRHINO_MUTEX_LOCK]       = (void*)sys_krhino_mutex_lock_stub,
    [SYS_KRHINO_MUTEX_UNLOCK]     = (void*)sys_krhino_mutex_unlock_stub,

    /* ------------------ semphore --------------------*/
#if (RHINO_CONFIG_SEM > 0)
    [SYS_KRHINO_SEM_DYN_CREATE] = (void*)sys_krhino_sem_dyn_create_stub,
    [SYS_KRHINO_SEM_DYN_DEL]    = (void*)sys_krhino_sem_dyn_del_stub,
    [SYS_KRHINO_SEM_TAKE]       = (void*)sys_krhino_sem_take_stub,
    [SYS_KRHINO_SEM_GIVE]       = (void*)sys_krhino_sem_give_stub,
#endif /* RHINO_CONFIG_SEM */

    /* --------------------- queue -----------------------*/
#if (RHINO_CONFIG_QUEUE > 0)
    [SYS_KRHINO_QUEUE_DYN_CREATE] = (void*)sys_krhino_queue_dyn_create_stub,
    [SYS_KRHINO_QUEUE_DYN_DEL]    = (void*)sys_krhino_queue_dyn_del_stub,
    [SYS_KRHINO_QUEUE_BACK_SEND]  = (void*)sys_krhino_queue_back_send_stub,
    [SYS_KRHINO_QUEUE_ALL_SEND]   = (void*)sys_krhino_queue_all_send_stub,
    [SYS_KRHINO_QUEUE_RECV]       = (void*)sys_krhino_queue_recv_stub,
    [SYS_KRHINO_QUEUE_FLUSH]      = (void*)sys_krhino_queue_flush_stub,
#endif /*RHINO_CONFIG_QUEUE */

    /* ------------------ buf queue --------------------*/
#if (RHINO_CONFIG_BUF_QUEUE > 0)
    [SYS_KRHINO_BUF_QUEUE_DYN_CREATE]     = (void*)sys_krhino_buf_queue_dyn_create_stub,
    [SYS_KRHINO_FIX_BUF_QUEUE_DYN_CREATE] = (void*)sys_krhino_fix_buf_queue_dyn_create_stub,
    [SYS_KRHINO_BUF_QUEUE_DYN_DEL]        = (void*)sys_krhino_buf_queue_dyn_del_stub,
    [SYS_KRHINO_BUF_QUEUE_SEND]           = (void*)sys_krhino_buf_queue_send_stub,
    [SYS_KRHINO_BUF_QUEUE_RECV]           = (void*)sys_krhino_buf_queue_recv_stub,
    [SYS_KRHINO_BUF_QUEUE_FLUSH]          = (void*)sys_krhino_buf_queue_flush_stub,
#endif /* RHINO_CONFIG_BUF_QUEUE */

    /* ------------------ event -------------------*/
#if (RHINO_CONFIG_EVENT_FLAG > 0)
    [SYS_KRHINO_EVENT_DYN_CREATE] = (void*)sys_krhino_event_dyn_create_stub,
    [SYS_KRHINO_EVENT_DYN_DEL]    = (void*)sys_krhino_event_dyn_del_stub,
    [SYS_KRHINO_EVENT_GET]        = (void*)sys_krhino_event_get_stub,
    [SYS_KRHINO_EVENT_SET]        = (void*)sys_krhino_event_set_stub,
#endif /* RHINO_CONFIG_EVENT_FLAG */

    /* ------------------ u_proc_msg -------------------*/
#if (RHINO_CONFIG_UIPC > 0)
    [SYS_KRHINO_MSG_GET]  = (void*)sys_krhino_msg_get_stub,
    [SYS_KRHINO_MSG_SND]  = (void*)sys_krhino_msg_snd_stub,
    [SYS_KRHINO_MSG_RECV] = (void*)sys_krhino_msg_recv_stub,
    [SYS_KRHINO_MSG_DEL]  = (void*)sys_krhino_msg_del_stub,
#endif /* RHINO_CONFIG_UIPC */

    /* ------------------- hal uart ---------------------*/
#if (RHINO_CONFIG_HAL_UART_SYSCALL > 0)
    [SYS_HAL_UART_INIT]     = (void*)sys_hal_uart_init_stub,
    [SYS_HAL_UART_SEND]     = (void*)sys_hal_uart_send_stub,
    [SYS_HAL_UART_RECV]     = (void*)sys_hal_uart_recv_stub,
    [SYS_HAL_UART_RECV_II]  = (void*)sys_hal_uart_recv_II_stub,
    [SYS_HAL_UART_FINALIZE] = (void*)sys_hal_uart_finalize_stub,
#endif /* RHINO_CONFIG_HAL_UART_SYSCALL */

    /* ------------------ hal_adc -------------------*/
#if (RHINO_CONFIG_HAL_ADC_SYSCALL > 0)
    [SYS_HAL_ADC_INIT]      = (void*)sys_hal_adc_init_stub,
    [SYS_HAL_ADC_VALUE_GET] = (void*)sys_hal_adc_value_get_stub,
    [SYS_HAL_ADC_FINALIZE]  = (void*)sys_hal_adc_finalize_stub,
#endif /* RHINO_CONFIG_HAL_ADC_SYSCALL */

    /* ------------------- hal flash ---------------------*/
#if (RHINO_CONFIG_HAL_FLASH_SYSCALL > 0)
    [SYS_HAL_FLASH_COPY_INFO]     = (void*)sys_hal_flash_copy_info_stub,
    [SYS_HAL_FLASH_ERASE]         = (void*)sys_hal_flash_erase_stub,
    [SYS_HAL_FLASH_WRITE]         = (void*)sys_hal_flash_write_stub,
    [SYS_HAL_FLASH_ERASE_WRITE]   = (void*)sys_hal_flash_erase_write_stub,
    [SYS_HAL_FLASH_READ]          = (void*)sys_hal_flash_read_stub,
    [SYS_HAL_FLASH_ENABLE_SECURE] = (void*)sys_hal_flash_enable_secure_stub,
    [SYS_HAL_FLASH_DIS_SECURE]    = (void*)sys_hal_flash_dis_secure_stub,
#endif /* RHINO_CONFIG_HAL_FLASH_SYSCALL */

    /* ------------------- hal gpio ---------------------*/
#if (RHINO_CONFIG_HAL_GPIO_SYSCALL > 0)
    [SYS_HAL_GPIO_INIT]          = (void*)sys_hal_gpio_init_stub,
    [SYS_HAL_GPIO_OUTPUT_HIGH]   = (void*)sys_hal_gpio_output_high_stub,
    [SYS_HAL_GPIO_OUTPUT_LOW]    = (void*)sys_hal_gpio_output_low_stub,
    [SYS_HAL_GPIO_OUTPUT_TOGGLE] = (void*)sys_hal_gpio_output_toggle_stub,
    [SYS_HAL_GPIO_INPUT_GET]     = (void*)sys_hal_gpio_input_get_stub,
    [SYS_HAL_GPIO_ENABLE_IRQ]    = (void*)sys_hal_gpio_enable_irq_stub,
    [SYS_HAL_GPIO_DISABLE_IRQ]   = (void*)sys_hal_gpio_disable_irq_stub,
    [SYS_HAL_GPIO_CLEAR_IRQ]     = (void*)sys_hal_gpio_clear_irq_stub,
    [SYS_HAL_GPIO_FINALIZE]      = (void*)sys_hal_gpio_finialize_stub,
#endif /* RHINO_CONFIG_HAL_GPIO_SYSCALL */

    /* ----------------- hal i2c -----------------*/
#if (RHINO_CONFIG_HAL_I2C_SYSCALL > 0)
    [SYS_HAL_I2C_INIT]        = (void*)sys_hal_i2c_init_stub,
    [SYS_HAL_I2C_MASTER_SEND] = (void*)sys_hal_i2c_master_send_stub,
    [SYS_HAL_I2C_MASTER_RECV] = (void*)sys_hal_i2c_master_recv_stub,
    [SYS_HAL_I2C_SLAVE_SEND]  = (void*)sys_hal_i2c_slave_send_stub,
    [SYS_HAL_I2C_SLAVE_RECV]  = (void*)sys_hal_i2c_slave_recv_stub,
    [SYS_HAL_I2C_MEM_WRITE]   = (void*)sys_hal_i2c_mem_write_stub,
    [SYS_HAL_I2C_MEM_READ]    = (void*)sys_hal_i2c_mem_read_stub,
    [SYS_HAL_I2C_FINALIZE]    = (void*)sys_hal_i2c_finalize_stub,
#endif /* RHINO_CONFIG_HAL_I2C_SYSCALL */

    /* ----------------- hal nand -----------------*/
#if (RHINO_CONFIG_HAL_NAND_SYSCALL > 0)
    [SYS_HAL_NAND_INIT]        = (void*)sys_hal_nand_init_stub,
    [SYS_HAL_NAND_READ_PAGE]   = (void*)sys_hal_nand_read_page_stub,
    [SYS_HAL_NAND_WRITE_PAGE]  = (void*)sys_hal_nand_write_page_stub,
    [SYS_HAL_NAND_READ_SPARE]  = (void*)sys_hal_nand_read_spare_stub,
    [SYS_HAL_NAND_WRITE_SPARE] = (void*)sys_hal_nand_write_spare_stub,
    [SYS_HAL_NAND_ERASE_BLOCK] = (void*)sys_hal_nand_erase_block_stubb,
    [SYS_HAL_NAND_FINALIZE]    = (void*)sys_hal_nand_finalize_stub,
#endif /* RHINO_CONFIG_HAL_NAND_SYSCALL */

    /* ----------------- hal nor -----------------*/
#if (RHINO_CONFIG_HAL_NOR_SYSCALL > 0)
    [SYS_HAL_NOR_INIT]        = (void*)sys_hal_nor_init_stub,
    [SYS_HAL_NOR_READ]        = (void*)sys_hal_nor_read_stub,
    [SYS_HAL_NOR_WRITE]       = (void*)sys_hal_nor_write_stub,
    [SYS_HAL_NOR_ERASE_BLOCK] = (void*)sys_hal_nor_erase_block_stub,
    [SYS_HAL_NOR_ERASE_CHIP]  = (void*)sys_hal_nor_erase_chip_stub,
    [SYS_HAL_NOR_FINALIZE]    = (void*)sys_hal_nor_finalize_stub,
#endif /* RHINO_CONFIG_HAL_NOR_SYSCALL */

    /* --------------- hal dac -------------------*/
#if (RHINO_CONFIG_HAL_DAC_SYSCALL > 0)
    [SYS_HAL_DAC_INIT]      = (void*)sys_hal_dac_init_stub,
    [SYS_HAL_DAC_START]     = (void*)sys_hal_dac_start_stub,
    [SYS_HAL_DAC_STOP]      = (void*)sys_hal_dac_stop_stub,
    [SYS_HAL_DAC_SET_VALUE] = (void*)sys_hal_dac_set_value_stub,
    [SYS_HAL_DAC_GET_VALUE] = (void*)sys_hal_dac_get_value_stub,
#endif /* RHINO_CONFIG_HAL_DAC_SYSCALL */

    /* --------------- hal rtc -------------------*/
#if (RHINO_CONFIG_HAL_RTC_SYSCALL > 0)
    [SYS_HAL_RTC_INIT]     = (void*)sys_hal_rtc_init_stub,
    [SYS_HAL_RTC_GET_TIME] = (void*)sys_hal_rtc_get_time_stub,
    [SYS_HAL_RTC_SET_TIME] = (void*)sys_hal_rtc_set_time_stub,
    [SYS_HAL_RTC_FINALIZE] = (void*)sys_hal_rtc_finalize_stub,
#endif /* RHINO_CONFIG_HAL_RTC_SYSCALL */

    /* ---------------- hal sd -------------------*/
#if (RHINO_CONFIG_HAL_SD_SYSCALL > 0)
    [SYS_HAL_SD_INIT]       = (void*)sys_hal_sd_init_stub,
    [SYS_HAL_SD_BLKS_READ]  = (void*)sys_hal_sd_blks_read_stub,
    [SYS_HAL_SD_BLKS_WRITE] = (void*)sys_hal_sd_blks_write_stub,
    [SYS_HAL_SD_ERASE]      = (void*)sys_hal_sd_erase_stub,
    [SYS_HAL_SD_STAT_GET]   = (void*)sys_hal_sd_stat_get_stub,
    [SYS_HAL_SD_INFO_GET]   = (void*)sys_hal_sd_info_get_stub,
    [SYS_HAL_SD_FINALIZE]   = (void*)sys_hal_sd_finalize_stub,
#endif /* RHINO_CONFIG_HAL_SD_SYSCALL */

    /* ---------------- hal spi ------------------*/
#if (RHINO_CONFIG_HAL_SPI_SYSCALL > 0)
    [SYS_HAL_SPI_INIT]      = (void*)sys_hal_spi_init_stub,
    [SYS_HAL_SPI_SEND]      = (void*)sys_hal_spi_send_stub,
    [SYS_HAL_SPI_RECV]      = (void*)sys_hal_spi_recv_stub,
    [SYS_HAL_SPI_SEND_RECV] = (void*)sys_hal_spi_send_recv_stub,
    [SYS_HAL_SPI_FINALIZE]  = (void*)sys_hal_spi_finalize_stub,
#endif /* RHINO_CONFIG_HAL_SPI_SYSCALL */

    /* ----------------   cli  ------------------*/
#if (RHINO_CONFIG_UCLI_SYSCALL > 0)
    [SYS_CLI_REGISTER_CMD]    = (void*)sys_cli_register_command_stub,
    [SYS_CLI_UNREGISTER_CMD]  = (void*)sys_cli_unregister_command_stub,
    [SYS_CLI_REGISTER_CMDS]   = (void*)sys_cli_register_commands_stub,
    [SYS_CLI_UNREGISTER_CMDS] = (void*)sys_cli_unregister_commands_stub,
#endif /* RHINO_CONFIG_UCLI_SYSCALL */

    /* ---------------- lwip --------------------*/
#if (RHINO_CONFIG_LWIP_SYSCALL > 0)
    [SYS_LWIP_ACCEPT]      = (void*)sys_lwip_accept_stub,
    [SYS_LWIP_BIND]        = (void*)sys_lwip_bind_stub,
    [SYS_LWIP_SHUTDOWN]    = (void*)sys_lwip_shutdown_stub,
    [SYS_LWIP_GETPEERNAME] = (void*)sys_lwip_getpeername_stub,
    [SYS_LWIP_GETSOCKNAME] = (void*)sys_lwip_getsockname_stub,
    [SYS_LWIP_GETSOCKOPT]  = (void*)sys_lwip_getsockopt_stub,
    [SYS_LWIP_SETSOCKET]   = (void*)sys_lwip_setsockopt_stub,
    [SYS_LWIP_CLOSE]       = (void*)sys_lwip_close_stub,
    [SYS_LWIP_CONNECT]     = (void*)sys_lwip_connect_stub,
    [SYS_LWIP_LISTEN]      = (void*)sys_lwip_listen_stub,
    [SYS_LWIP_RECV]        = (void*)sys_lwip_recv_stub,
    [SYS_LWIP_READ]        = (void*)sys_lwip_read_stub,
    [SYS_LWIP_RECVFROM]    = (void*)sys_lwip_recvfrom_stub,
    [SYS_LWIP_SEND]        = (void*)sys_lwip_send_stub,
    [SYS_LWIP_SENDMSG]     = (void*)sys_lwip_sendmsg_stub,
    [SYS_LWIP_SENDTO]      = (void*)sys_lwip_sendto_stub,
    [SYS_LWIP_SOCKET]      = (void*)sys_lwip_socket,
    [SYS_LWIP_WRITE]       = (void*)sys_lwip_write_stub,
    [SYS_LWIP_WRITEV]      = (void*)sys_lwip_writev_stub,
    [SYS_LWIP_SELECT]      = (void*)sys_lwip_select_stub,
    [SYS_LWIP_IOCTL]       = (void*)sys_lwip_ioctl_stub,
    [SYS_LWIP_FCNTL]       = (void*)sys_lwip_fcntl_stub,
    [SYS_LWIP_EVENTFD]     = (void*)sys_lwip_eventfd_stub,
#if LWIP_PACKET
    [SYS_LWIP_TRY_WAKEUP]  = (void*)sys_lwip_try_wakeup_stub,
#endif
    [SYS_LWIP_GETHOSTBYNAME]      = (void*)sys_lwip_gethostbyname_stub,
    [SYS_LWIP_GETHOSTBYNAME_R]    = (void*)sys_lwip_gethostbyname_r_stub,
    [SYS_LWIP_FREEADDRINFO]       = (void*)sys_lwip_freeaddrinfo_stub,
    [SYS_LWIP_GETADDRINOF]        = (void*)sys_lwip_getaddrinfo_stub,
#endif /* RHINO_CONFIG_LWIP_SYSCALL */

        /* ---------------- linkkit --------------------*/
#if (RHINO_CONFIG_LINKKIT_SYSCALL > 0)
    [SYS_IOT_LINKKIT_OPEN]         = (void*)sys_IOT_Linkkit_Open_stub,
    [SYS_IOT_LINKKIT_CONNECT]      = (void*)sys_IOT_Linkkit_Connect_stub,
    [SYS_IOT_LINKKIT_YIELD]        = (void*)sys_IOT_Linkkit_Yield_stub,
    [SYS_IOT_LINKKIT_CLOSE]        = (void*)sys_IOT_Linkkit_Close_stub,
    [SYS_IOT_LINKKIT_REPORT]       = (void*)sys_IOT_Linkkit_Report_stub,
    [SYS_IOT_LINKKIT_QUERY]        = (void*)sys_IOT_Linkkit_Query_stub,
    [SYS_IOT_lINKKIT_TRIGGEREVENT] = (void*)sys_IOT_Linkkit_TriggerEvent_stub,
    [SYS_IOT_SETLOGLEVEL]          = (void*)sys_IOT_SetLogLevel_stub,
    [SYS_IOT_DUMPMEMORYSTATS]      = (void*)sys_IOT_DumpMemoryStats_stub,
    [SYS_IOT_SETUPCONNINFO]        = (void*)sys_IOT_SetupConnInfo_stub,
    [SYS_IOT_IOCTL]                = (void*)sys_IOT_Ioctl_stub,
    [SYS_IOT_HAL_IOCTL]            = (void*)sys_IOT_HAL_Ioctl_stub,
    [SYS_IOT_REGSITERCALLBACK]     = (void*)sys_IOT_RegisterCallback_stub,
#endif /* RHINO_CONFIG_LINKKIT_SYSCALL */

    /* ---------------- end ------------------*/
};

