/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <syscall_no.h>
#include "rhino_ksyscall.h"
#include "hal_adc_ksyscall.h"
#include "hal_flash_ksyscall.h"
#include "hal_i2c_ksyscall.h"
#include "hal_nand_ksyscall.h"
#include "hal_nor_ksyscall.h"
#include "hal_rtc_ksyscall.h"
#include "hal_spi_ksyscall.h"
#include "hal_dac_ksyscall.h"
#include "hal_gpio_ksyscall.h"
#include "hal_sd_ksyscall.h"
#include "hal_uart_ksyscall.h"
#include "cli_ksyscall.h"
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
    [SYS_KRHINO_UPROCESS_CREATE]  = sys_krhino_uprocess_create_stub,
    [SYS_KRHINO_UPROCESS_EXIT]    = sys_krhino_uprocess_exit_stub,
    [SYS_KRHINO_UPROCESS_RES_GET] = sys_krhino_uprocess_res_get_stub,

    /* ------------------- time ----------------------*/
    [SYS_KRHINO_SYS_TIME_GET] = sys_krhino_sys_time_get_stub,
    [SYS_KRHINO_SYS_TICK_GET] = sys_krhino_sys_tick_get_stub,
    [SYS_KRHINO_MS_TO_TICKS]  = sys_krhino_ms_to_ticks_stub,
    [SYS_KRHINO_TICKS_TO_MS]  = sys_krhino_ticks_to_ms_stub,

    /* ------------------- mutex ---------------------*/
    [SYS_KRHINO_MUTEX_DYN_CREATE] = sys_krhino_mutex_dyn_create_stub,
    [SYS_KRHINO_MUTEX_DYN_DEL]    = sys_krhino_mutex_dyn_del_stub,
    [SYS_KRHINO_MUTEX_LOCK]       = sys_krhino_mutex_lock_stub,
    [SYS_KRHINO_MUTEX_UNLOCK]     = sys_krhino_mutex_unlock_stub,

    /* ------------------ semphore --------------------*/
    [SYS_KRHINO_SEM_DYN_CREATE] = sys_krhino_sem_dyn_create_stub,
    [SYS_KRHINO_SEM_DYN_DEL]    = sys_krhino_sem_dyn_del_stub,
    [SYS_KRHINO_SEM_TAKE]       = sys_krhino_sem_take_stub,
    [SYS_KRHINO_SEM_GIVE]       = sys_krhino_sem_give_stub,

    /* --------------------- queue -----------------------*/
    [SYS_KRHINO_QUEUE_DYN_CREATE] = sys_krhino_queue_dyn_create_stub,
    [SYS_KRHINO_QUEUE_DYN_DEL]    = sys_krhino_queue_dyn_del_stub,
    [SYS_KRHINO_QUEUE_BACK_SEND]  = sys_krhino_queue_back_send_stub,
    [SYS_KRHINO_QUEUE_ALL_SEND]   = sys_krhino_queue_all_send_stub,
    [SYS_KRHINO_QUEUE_RECV]       = sys_krhino_queue_recv_stub,
    [SYS_KRHINO_QUEUE_FLUSH]      = sys_krhino_queue_flush_stub,

    /* ------------------ buf queue --------------------*/
    [SYS_KRHINO_BUF_QUEUE_DYN_CREATE]     = sys_krhino_buf_queue_dyn_create_stub,
    [SYS_KRHINO_FIX_BUF_QUEUE_DYN_CREATE] = sys_krhino_fix_buf_queue_dyn_create_stub,
    [SYS_KRHINO_BUF_QUEUE_DYN_DEL]        = sys_krhino_buf_queue_dyn_del_stub,
    [SYS_KRHINO_BUF_QUEUE_SEND]           = sys_krhino_buf_queue_send_stub,
    [SYS_KRHINO_BUF_QUEUE_RECV]           = sys_krhino_buf_queue_recv_stub,
    [SYS_KRHINO_BUF_QUEUE_FLUSH]          = sys_krhino_buf_queue_flush_stub,

    /* ------------------ event -------------------*/
    [SYS_KRHINO_EVENT_DYN_CREATE] = sys_krhino_event_dyn_create_stub,
    [SYS_KRHINO_EVENT_DYN_DEL]    =  sys_krhino_event_dyn_del_stub,
    [SYS_KRHINO_EVENT_GET]        =  sys_krhino_event_get_stub,
    [SYS_KRHINO_EVENT_SET]        = sys_krhino_event_set_stub,

    /* ------------------ u_proc_msg -------------------*/
    [SYS_KRHINO_MSG_GET]  = sys_krhino_msg_get_stub,
    [SYS_KRHINO_MSG_SND]  = sys_krhino_msg_snd_stub,
    [SYS_KRHINO_MSG_RECV] = sys_krhino_msg_recv_stub,
    [SYS_KRHINO_MSG_DEL]  = sys_krhino_msg_del_stub,

    /* ------------------- hal uart ---------------------*/
    [SYS_HAL_UART_INIT]     = sys_hal_uart_init_stub,
    [SYS_HAL_UART_SEND]     = sys_hal_uart_send_stub,
    [SYS_HAL_UART_RECV]     = sys_hal_uart_recv_stub,
    [SYS_HAL_UART_RECV_II]  = sys_hal_uart_recv_II_stub,
    [SYS_HAL_UART_FINALIZE] = sys_hal_uart_finalize_stub,

    /* ------------------ hal_adc -------------------*/
#if (CONFIG_HAL_ADC_SYSCALL > 0)
    [SYS_HAL_ADC_INIT]      = sys_hal_adc_init_stub,
    [SYS_HAL_ADC_VALUE_GET] = sys_hal_adc_value_get_stub,
    [SYS_HAL_ADC_FINALIZE]  = sys_hal_adc_finalize_stub,
#endif

    /* ------------------- hal flash ---------------------*/
#if (CONFIG_HAL_FLASH_SYSCALL > 0)
    [SYS_HAL_FLASH_GET_INFO]      = sys_hal_flash_get_info_stub,
    [SYS_HAL_FLASH_ERASE]         = sys_hal_flash_erase_stub,
    [SYS_HAL_FLASH_WRITE]         = sys_hal_flash_write_stub,
    [SYS_HAL_FLASH_ERASE_WRITE]   = sys_hal_flash_erase_write_stub,
    [SYS_HAL_FLASH_READ]          = sys_hal_flash_read_stub,
    [SYS_HAL_FLASH_ENABLE_SECURE] = sys_hal_flash_enable_secure_stub,
    [SYS_HAL_FLASH_DIS_SECURE]    = sys_hal_flash_dis_secure_stub,
#endif /* CONFIG_HAL_FLASH_SYSCALL */

    /* ------------------- hal gpio ---------------------*/
#if (CONFIG_HAL_GPIO_SYSCALL > 0)
    [SYS_HAL_GPIO_INIT]          = sys_hal_gpio_init_stub,
    [SYS_HAL_GPIO_OUTPUT_HIGH]   = sys_hal_gpio_output_high_stub,
    [SYS_HAL_GPIO_OUTPUT_LOW]    = sys_hal_gpio_output_low_stub,
    [SYS_HAL_GPIO_OUTPUT_TOGGLE] = sys_hal_gpio_output_toggle_stub,
    [SYS_HAL_GPIO_INPUT_GET]     = sys_hal_gpio_input_get_stub,
    [SYS_HAL_GPIO_ENABLE_IRQ]    = sys_hal_gpio_enable_irq_stub,
    [SYS_HAL_GPIO_DISABLE_IRQ]   = sys_hal_gpio_disable_irq_stub,
    [SYS_HAL_GPIO_CLEAR_IRQ]     = sys_hal_gpio_clear_irq_stub,
    [SYS_HAL_GPIO_FINALIZE]      = sys_hal_gpio_finialize_stub,
#endif /* CONFIG_HAL_GPIO_SYSCALL */

    /* ----------------- hal i2c -----------------*/
#if (CONFIG_HAL_I2C_SYSCALL > 0)
    [SYS_HAL_I2C_INIT]        = sys_hal_i2c_init_stub,
    [SYS_HAL_I2C_MASTER_SEND] = sys_hal_i2c_master_send_stub,
    [SYS_HAL_I2C_MASTER_RECV] = sys_hal_i2c_master_recv_stub,
    [SYS_HAL_I2C_SLAVE_SEND]  = sys_hal_i2c_slave_send_stub,
    [SYS_HAL_I2C_SLAVE_RECV]  = sys_hal_i2c_slave_recv_stub,
    [SYS_HAL_I2C_MEM_WRITE]   = sys_hal_i2c_mem_write_stub,
    [SYS_HAL_I2C_MEM_READ]   = sys_hal_i2c_mem_read_stub,
    [SYS_HAL_I2C_FINALIZE]    = sys_hal_i2c_finalize_stub,
#endif /* CONFIG_HAL_I2C_SYSCALL */

    /* ----------------- hal nand -----------------*/
#if (CONFIG_HAL_NAND_SYSCALL > 0)
    [SYS_HAL_NAND_INIT]        = sys_hal_nand_init_stub,
    [SYS_HAL_NAND_READ_PAGE]   = sys_hal_nand_read_page_stub,
    [SYS_HAL_NAND_WRITE_PAGE]  = sys_hal_nand_write_page_stub,
    [SYS_HAL_NAND_READ_SPARE]  = sys_hal_nand_read_spare_stub,
    [SYS_HAL_NAND_WRITE_SPARE] = sys_hal_nand_write_spare_stub,
    [SYS_HAL_NAND_ERASE_BLOCK] = sys_hal_nand_erase_block_stubb,
    [SYS_HAL_NAND_FINALIZE]    = sys_hal_nand_finalize_stub,
#endif /* CONFIG_HAL_NAND_SYSCALL */

    /* ----------------- hal nor -----------------*/
#if (CONFIG_HAL_NOR_SYSCALL > 0)
    [SYS_HAL_NOR_INIT]        = sys_hal_nor_init_stub,
    [SYS_HAL_NOR_READ]        = sys_hal_nor_read_stub,
    [SYS_HAL_NOR_WRITE]       = sys_hal_nor_write_stub,
    [SYS_HAL_NOR_ERASE_BLOCK] = sys_hal_nor_erase_block_stub,
    [SYS_HAL_NOR_ERASE_CHIP]  = sys_hal_nor_erase_chip_stub,
    [SYS_HAL_NOR_FINALIZE]    = sys_hal_nor_finalize_stub,
#endif /* CONFIG_HAL_NOR_SYSCALL */

    /* --------------- hal dac -------------------*/
#if (CONFIG_HAL_DAC_SYSCALL > 0)
    [SYS_HAL_DAC_INIT]      = sys_hal_dac_init_stub,
    [SYS_HAL_DAC_START]     = sys_hal_dac_start_stub,
    [SYS_HAL_DAC_STOP]      = sys_hal_dac_stop_stub,
    [SYS_HAL_DAC_SET_VALUE] = sys_hal_dac_set_value_stub,
    [SYS_HAL_DAC_GET_VALUE] = sys_hal_dac_get_value_stub,
#endif /* CONFIG_HAL_DAC_SYSCALL */

    /* --------------- hal rtc -------------------*/
#if (CONFIG_HAL_RTC_SYSCALL > 0)
    [SYS_HAL_RTC_INIT]     = sys_hal_rtc_init_stub,
    [SYS_HAL_RTC_GET_TIME] = sys_hal_rtc_get_time_stub,
    [SYS_HAL_RTC_SET_TIME] = sys_hal_rtc_set_time_stub,
    [SYS_HAL_RTC_FINALIZE] = sys_hal_rtc_finalize_stub,
#endif /* CONFIG_HAL_RTC_SYSCALL */

    /* ---------------- hal sd -------------------*/
#if (CONFIG_HAL_SD_SYSCALL > 0)
    [SYS_HAL_SD_INIT]       = sys_hal_sd_init_stub,
    [SYS_HAL_SD_BLKS_READ]  = sys_hal_sd_blks_read_stub,
    [SYS_HAL_SD_BLKS_WRITE] = sys_hal_sd_blks_write_stub,
    [SYS_HAL_SD_ERASE]      = sys_hal_sd_erase_stub,
    [SYS_HAL_SD_STAT_GET]   = sys_hal_sd_stat_get_stub,
    [SYS_HAL_SD_INFO_GET]   = sys_hal_sd_info_get_stub,
    [SYS_HAL_SD_FINALIZE]   = sys_hal_sd_finalize_stub,
#endif /* CONFIG_HAL_SD_SYSCALL */

    /* ---------------- hal spi ------------------*/
#if (CONFIG_HAL_SPI_SYSCALL > 0)
    [SYS_HAL_SPI_INIT]      = sys_hal_spi_init_stub,
    [SYS_HAL_SPI_SEND]      = sys_hal_spi_send_stub,
    [SYS_HAL_SPI_RECV]      = sys_hal_spi_recv_stub,
    [SYS_HAL_SPI_SEND_RECV] = sys_hal_spi_send_recv_stub,
    [SYS_HAL_SPI_FINALIZE]  = sys_hal_spi_finalize_stub,
#endif /* CONFIG_HAL_SPI_SYSCALL */

    /* ----------------   cli  ------------------*/
#if (CONFIG_CLI_SYSCALL > 0)
    [SYS_CLI_REGISTER_CMD] = sys_cli_register_command_stub,
#endif /* CONFIG_CLI_SYSCALL */

    /* ---------------- lwip --------------------*/
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
    [SYS_LWIP_GETHOSTBYNAME]      = sys_lwip_gethostbyname_stub,
    [SYS_LWIP_GETHOSTBYNAME_R]    = sys_lwip_gethostbyname_r_stub,
    [SYS_LWIP_FREEADDRINFO]       = sys_lwip_freeaddrinfo_stub,
    [SYS_LWIP_GETADDRINOF]        = sys_lwip_getaddrinfo_stub,
#endif /* CONFIG_LWIP_SYSCALL */

    /* ---------------- end ------------------*/
    NULL
};

