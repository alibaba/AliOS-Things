/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#ifndef SYSCALL_NO_H
#define SYSCALL_NO_H

/* ------------------- task --------------------*/
#define K_TASK_BASE (0)

#define SYS_KRHINO_CUR_TASK_GET    (K_TASK_BASE + 0)
#define SYS_KRHINO_TASK_SLEEP      (K_TASK_BASE + 1)
#define SYS_KRHINO_TASK_SUSPEND    (K_TASK_BASE + 2)
#define SYS_KRHINO_TASK_RESUME     (K_TASK_BASE + 3)
#define SYS_KRHINO_TASK_WAIT_ABORT (K_TASK_BASE + 4)
#define SYS_KRHINO_UTASK_CREATE    (K_TASK_BASE + 5)
#define SYS_KRHINO_UTASK_DEL       (K_TASK_BASE + 6)

#define K_TASK_END (SYS_KRHINO_UTASK_DEL)

/* ------------------- process --------------------*/
#define K_UPROCESS_BASE (K_TASK_END + 1)

#define SYS_KRHINO_UPROCESS_CREATE  (K_UPROCESS_BASE + 0)
#define SYS_KRHINO_UPROCESS_EXIT    (K_UPROCESS_BASE + 1)
#define SYS_KRHINO_UPROCESS_RES_GET (K_UPROCESS_BASE + 2)

#define K_UPROCESS_END (SYS_KRHINO_UPROCESS_RES_GET)

/* ------------------- time --------------------*/
#define K_TIME_BASE (K_UPROCESS_END + 1)

#define SYS_KRHINO_SYS_TIME_GET (K_TIME_BASE + 0)
#define SYS_KRHINO_SYS_TICK_GET (K_TIME_BASE + 1)
#define SYS_KRHINO_MS_TO_TICKS  (K_TIME_BASE + 2)
#define SYS_KRHINO_TICKS_TO_MS  (K_TIME_BASE + 3)

#define K_TIME_END (SYS_KRHINO_TICKS_TO_MS)

/* ------------------- mutex --------------------*/
#define K_MUTEX_BASE (K_TIME_END + 1)

#define SYS_KRHINO_MUTEX_DYN_CREATE (K_MUTEX_BASE + 2)
#define SYS_KRHINO_MUTEX_DYN_DEL    (K_MUTEX_BASE + 3)
#define SYS_KRHINO_MUTEX_LOCK       (K_MUTEX_BASE + 4)
#define SYS_KRHINO_MUTEX_UNLOCK     (K_MUTEX_BASE + 5)

#define K_MUTEXT_END (SYS_KRHINO_MUTEX_UNLOCK)

/* ------------------ semphore -------------------*/
#define K_SEM_BASE (K_MUTEXT_END + 1)

#define SYS_KRHINO_SEM_DYN_CREATE (K_SEM_BASE + 2)
#define SYS_KRHINO_SEM_DYN_DEL    (K_SEM_BASE + 3)
#define SYS_KRHINO_SEM_TAKE       (K_SEM_BASE + 4)
#define SYS_KRHINO_SEM_GIVE       (K_SEM_BASE + 5)

#define K_SEM_END (SYS_KRHINO_SEM_GIVE)

/* -------------------- queue --------------------*/
#define K_QUEUE_BASE (K_SEM_END + 1)

#define SYS_KRHINO_QUEUE_DYN_CREATE (K_QUEUE_BASE + 2)
#define SYS_KRHINO_QUEUE_DYN_DEL    (K_QUEUE_BASE + 3)
#define SYS_KRHINO_QUEUE_BACK_SEND  (K_QUEUE_BASE + 4)
#define SYS_KRHINO_QUEUE_ALL_SEND   (K_QUEUE_BASE + 5)
#define SYS_KRHINO_QUEUE_RECV       (K_QUEUE_BASE + 6)
#define SYS_KRHINO_QUEUE_FLUSH      (K_QUEUE_BASE + 7)

#define K_QUEUE_END (SYS_KRHINO_QUEUE_FLUSH)

/* ------------------ buf queue -------------------*/
#define K_BUF_QUEUE_BASE (K_QUEUE_END + 1)

#define SYS_KRHINO_BUF_QUEUE_DYN_CREATE     (K_BUF_QUEUE_BASE + 3)
#define SYS_KRHINO_FIX_BUF_QUEUE_DYN_CREATE (K_BUF_QUEUE_BASE + 4)
#define SYS_KRHINO_BUF_QUEUE_DYN_DEL        (K_BUF_QUEUE_BASE + 5)
#define SYS_KRHINO_BUF_QUEUE_SEND           (K_BUF_QUEUE_BASE + 6)
#define SYS_KRHINO_BUF_QUEUE_RECV           (K_BUF_QUEUE_BASE + 7)
#define SYS_KRHINO_BUF_QUEUE_FLUSH          (K_BUF_QUEUE_BASE + 8)

#define K_BUF_QUEUE_END (SYS_KRHINO_BUF_QUEUE_FLUSH)

/* ------------------ event -------------------*/
#define K_EVENT_BASE (K_BUF_QUEUE_END + 1)

#define SYS_KRHINO_EVENT_DYN_CREATE (K_EVENT_BASE + 0)
#define SYS_KRHINO_EVENT_DYN_DEL    (K_EVENT_BASE + 1)
#define SYS_KRHINO_EVENT_GET        (K_EVENT_BASE + 2)
#define SYS_KRHINO_EVENT_SET        (K_EVENT_BASE + 3)

#define K_EVENT_END (SYS_KRHINO_EVENT_SET)

/* ----------------- proc msg -----------------*/
#define U_PROC_MSG_BASE (K_EVENT_END + 1)

#define SYS_KRHINO_MSG_GET  (U_PROC_MSG_BASE + 0)
#define SYS_KRHINO_MSG_SND  (U_PROC_MSG_BASE + 1)
#define SYS_KRHINO_MSG_RECV (U_PROC_MSG_BASE + 2)
#define SYS_KRHINO_MSG_DEL  (U_PROC_MSG_BASE + 3)

#define U_PROC_MSG_END (SYS_KRHINO_MSG_DEL)

/* ------------ reserved for user --------------*/
#define SYS_USER_DEFINE_BASE (U_PROC_MSG_END + 1)

#define SYS_USER_DEFINE0 (SYS_USER_DEFINE_BASE + 0)
#define SYS_USER_DEFINE1 (SYS_USER_DEFINE_BASE + 1)
#define SYS_USER_DEFINE2 (SYS_USER_DEFINE_BASE + 2)
#define SYS_USER_DEFINE3 (SYS_USER_DEFINE_BASE + 3)
#define SYS_USER_DEFINE4 (SYS_USER_DEFINE_BASE + 4)
#define SYS_USER_DEFINE5 (SYS_USER_DEFINE_BASE + 5)
#define SYS_USER_DEFINE6 (SYS_USER_DEFINE_BASE + 6)
#define SYS_USER_DEFINE7 (SYS_USER_DEFINE_BASE + 7)
#define SYS_USER_DEFINE8 (SYS_USER_DEFINE_BASE + 8)
#define SYS_USER_DEFINE9 (SYS_USER_DEFINE_BASE + 9)

#define SYS_USER_DEFINE_END (SYS_USER_DEFINE9)

/* ----------------- hal uart -----------------*/
#define HAL_UART_BASE (SYS_USER_DEFINE_END + 1)

#define SYS_HAL_UART_INIT     (HAL_UART_BASE + 0)
#define SYS_HAL_UART_SEND     (HAL_UART_BASE + 1)
#define SYS_HAL_UART_RECV     (HAL_UART_BASE + 2)
#define SYS_HAL_UART_RECV_II  (HAL_UART_BASE + 3)
#define SYS_HAL_UART_FINALIZE (HAL_UART_BASE + 4)

#define HAL_UART_END (SYS_HAL_UART_FINALIZE)

/* ----------------- hal adc -----------------*/
#define HAL_ADC_BASE (HAL_UART_END + 1)

#define SYS_HAL_ADC_INIT      (HAL_ADC_BASE + 0)
#define SYS_HAL_ADC_VALUE_GET (HAL_ADC_BASE + 1)
#define SYS_HAL_ADC_FINALIZE  (HAL_ADC_BASE + 2)

#define HAL_ADC_END (SYS_HAL_ADC_FINALIZE)

/* ----------------- hal dac -----------------*/
#define HAL_DAC_BASE (HAL_ADC_END + 1)

#define SYS_HAL_DAC_INIT      (HAL_DAC_BASE + 0)
#define SYS_HAL_DAC_START     (HAL_DAC_BASE + 1)
#define SYS_HAL_DAC_STOP      (HAL_DAC_BASE + 2)
#define SYS_HAL_DAC_SET_VALUE (HAL_DAC_BASE + 3)
#define SYS_HAL_DAC_GET_VALUE (HAL_DAC_BASE + 4)
#define SYS_HAL_DAC_FINALIZE  (HAL_DAC_BASE + 5)

#define HAL_DAC_END (SYS_HAL_DAC_FINALIZE)

/* ----------------- hal flash -----------------*/
#define HAL_FLASH_BASE (HAL_DAC_END + 1)

#define SYS_HAL_FLASH_INFO_GET      (HAL_FLASH_BASE + 0)
#define SYS_HAL_FLASH_ERASE         (HAL_FLASH_BASE + 1)
#define SYS_HAL_FLASH_WRITE         (HAL_FLASH_BASE + 2)
#define SYS_HAL_FLASH_ERASE_WRITE   (HAL_FLASH_BASE + 3)
#define SYS_HAL_FLASH_READ          (HAL_FLASH_BASE + 4)
#define SYS_HAL_FLASH_ENABLE_SECURE (HAL_FLASH_BASE + 5)
#define SYS_HAL_FLASH_DIS_SECURE    (HAL_FLASH_BASE + 6)

#define HAL_FLASH_END (SYS_HAL_FLASH_DIS_SECURE)

/* ----------------- hal gpio -----------------*/
#define HAL_GPIO_BASE (HAL_FLASH_END + 1)

#define SYS_HAL_GPIO_INIT          (HAL_GPIO_BASE + 0)
#define SYS_HAL_GPIO_OUTPUT_HIGH   (HAL_GPIO_BASE + 1)
#define SYS_HAL_GPIO_OUTPUT_LOW    (HAL_GPIO_BASE + 2)
#define SYS_HAL_GPIO_OUTPUT_TOGGLE (HAL_GPIO_BASE + 3)
#define SYS_HAL_GPIO_INPUT_GET     (HAL_GPIO_BASE + 4)
#define SYS_HAL_GPIO_ENABLE_IRQ    (HAL_GPIO_BASE + 5)
#define SYS_HAL_GPIO_DISABLE_IRQ   (HAL_GPIO_BASE + 6)
#define SYS_HAL_GPIO_CLEAR_IRQ     (HAL_GPIO_BASE + 7)
#define SYS_HAL_GPIO_FINALIZE      (HAL_GPIO_BASE + 8)

#define HAL_GPIO_END (SYS_HAL_GPIO_FINALIZE)

/* ----------------- hal i2c -----------------*/
#define HAL_I2C_BASE (HAL_GPIO_END + 1)

#define SYS_HAL_I2C_INIT        (HAL_I2C_BASE + 0)
#define SYS_HAL_I2C_MASTER_SEND (HAL_I2C_BASE + 1)
#define SYS_HAL_I2C_MASTER_RECV (HAL_I2C_BASE + 2)
#define SYS_HAL_I2C_SLAVE_SEND  (HAL_I2C_BASE + 3)
#define SYS_HAL_I2C_SLAVE_RECV  (HAL_I2C_BASE + 4)
#define SYS_HAL_I2C_MEM_WRITE   (HAL_I2C_BASE + 5)
#define SYS_HAL_I2C_MEM_READ    (HAL_I2C_BASE + 6)
#define SYS_HAL_I2C_FINALIZE    (HAL_I2C_BASE + 7)

#define HAL_I2C_END (SYS_HAL_I2C_FINALIZE)

/* ----------------- hal nand -----------------*/
#define HAL_NAND_BASE (HAL_I2C_END + 1)

#define SYS_HAL_NAND_INIT        (HAL_NAND_BASE + 0)
#define SYS_HAL_NAND_READ_PAGE   (HAL_NAND_BASE + 1)
#define SYS_HAL_NAND_WRITE_PAGE  (HAL_NAND_BASE + 2)
#define SYS_HAL_NAND_READ_SPARE  (HAL_NAND_BASE + 3)
#define SYS_HAL_NAND_WRITE_SPARE (HAL_NAND_BASE + 4)
#define SYS_HAL_NAND_ERASE_BLOCK (HAL_NAND_BASE + 5)
#define SYS_HAL_NAND_FINALIZE    (HAL_NAND_BASE + 6)

#define HAL_NAND_END (SYS_HAL_NAND_FINALIZE)

/* ----------------- hal nor ------------------*/
#define HAL_NOR_BASE (HAL_NAND_END + 1)

#define SYS_HAL_NOR_INIT        (HAL_NOR_BASE + 0)
#define SYS_HAL_NOR_READ        (HAL_NOR_BASE + 1)
#define SYS_HAL_NOR_WRITE       (HAL_NOR_BASE + 2)
#define SYS_HAL_NOR_ERASE_BLOCK (HAL_NOR_BASE + 3)
#define SYS_HAL_NOR_ERASE_CHIP  (HAL_NOR_BASE + 4)
#define SYS_HAL_NOR_FINALIZE    (HAL_NOR_BASE + 5)

#define HAL_NOR_END (SYS_HAL_NOR_FINALIZE)

/* ----------------- hal  rtc-----------------*/
#define HAL_RTC_BASE (HAL_NOR_END + 1)

#define SYS_HAL_RTC_INIT     (HAL_RTC_BASE + 0)
#define SYS_HAL_RTC_GET_TIME (HAL_RTC_BASE + 1)
#define SYS_HAL_RTC_SET_TIME (HAL_RTC_BASE + 2)
#define SYS_HAL_RTC_FINALIZE (HAL_RTC_BASE + 3)

#define HAL_RTC_END (SYS_HAL_RTC_FINALIZE)

/* ----------------- hal sd -----------------*/
#define HAL_SD_BASE (HAL_RTC_END + 1)

#define SYS_HAL_SD_INIT       (HAL_SD_BASE + 0)
#define SYS_HAL_SD_BLKS_READ  (HAL_SD_BASE + 1)
#define SYS_HAL_SD_BLKS_WRITE (HAL_SD_BASE + 2)
#define SYS_HAL_SD_ERASE      (HAL_SD_BASE + 3)
#define SYS_HAL_SD_STAT_GET   (HAL_SD_BASE + 4)
#define SYS_HAL_SD_INFO_GET   (HAL_SD_BASE + 5)
#define SYS_HAL_SD_FINALIZE   (HAL_SD_BASE + 6)

#define HAL_SD_END (SYS_HAL_SD_FINALIZE)

/* ----------------- hal spi -----------------*/
#define HAL_SPI_BASE (HAL_SD_END + 1)

#define SYS_HAL_SPI_INIT      (HAL_SPI_BASE + 0)
#define SYS_HAL_SPI_SEND      (HAL_SPI_BASE + 1)
#define SYS_HAL_SPI_RECV      (HAL_SPI_BASE + 2)
#define SYS_HAL_SPI_SEND_RECV (HAL_SPI_BASE + 3)
#define SYS_HAL_SPI_FINALIZE  (HAL_SPI_BASE + 4)

#define HAL_SPI_END (SYS_HAL_SPI_FINALIZE)

/* -----------------  cli  -----------------*/
#define CLI_BASE (HAL_SPI_END + 1)

#define SYS_CLI_REGISTER_CMD    (CLI_BASE + 0)
#define SYS_CLI_UNREGISTER_CMD  (CLI_BASE + 1)
#define SYS_CLI_REGISTER_CMDS   (CLI_BASE + 2)
#define SYS_CLI_UNREGISTER_CMDS (CLI_BASE + 3)

#define CLI_END (SYS_CLI_UNREGISTER_CMDS)

/* ------------------- lwip ------------------*/
#define LWIP_BASE (CLI_END + 1)

#define SYS_LWIP_ACCEPT      (LWIP_BASE + 0)
#define SYS_LWIP_BIND        (LWIP_BASE + 1)
#define SYS_LWIP_SHUTDOWN    (LWIP_BASE + 2)
#define SYS_LWIP_GETPEERNAME (LWIP_BASE + 3)
#define SYS_LWIP_GETSOCKNAME (LWIP_BASE + 4)
#define SYS_LWIP_GETSOCKOPT  (LWIP_BASE + 5)
#define SYS_LWIP_SETSOCKET   (LWIP_BASE + 6)
#define SYS_LWIP_CLOSE       (LWIP_BASE + 7)
#define SYS_LWIP_CONNECT     (LWIP_BASE + 8)
#define SYS_LWIP_LISTEN      (LWIP_BASE + 9)
#define SYS_LWIP_RECV        (LWIP_BASE + 10)
#define SYS_LWIP_READ        (LWIP_BASE + 11)
#define SYS_LWIP_RECVFROM    (LWIP_BASE + 12)
#define SYS_LWIP_SEND        (LWIP_BASE + 13)
#define SYS_LWIP_SENDMSG     (LWIP_BASE + 14)
#define SYS_LWIP_SENDTO      (LWIP_BASE + 15)
#define SYS_LWIP_SOCKET      (LWIP_BASE + 16)
#define SYS_LWIP_WRITE       (LWIP_BASE + 17)
#define SYS_LWIP_WRITEV      (LWIP_BASE + 18)
#define SYS_LWIP_SELECT      (LWIP_BASE + 19)
#define SYS_LWIP_IOCTL       (LWIP_BASE + 20)
#define SYS_LWIP_FCNTL       (LWIP_BASE + 21)
#define SYS_LWIP_EVENTFD     (LWIP_BASE + 22)
#define SYS_LWIP_TRY_WAKEUP  (LWIP_BASE + 23)

#define SYS_LWIP_GETHOSTBYNAME      (LWIP_BASE + 24)
#define SYS_LWIP_GETHOSTBYNAME_R    (LWIP_BASE + 25)
#define SYS_LWIP_FREEADDRINFO       (LWIP_BASE + 26)
#define SYS_LWIP_GETADDRINOF        (LWIP_BASE + 27)

#define LWIP_END (SYS_LWIP_GETADDRINOF)

/* ------------------- linkkit -------------------*/
#define LINKKIT_BASE (LWIP_END + 1)

#define SYS_IOT_LINKKIT_OPEN         (LINKKIT_BASE + 0)
#define SYS_IOT_LINKKIT_CONNECT      (LINKKIT_BASE + 1)
#define SYS_IOT_LINKKIT_YIELD        (LINKKIT_BASE + 2)
#define SYS_IOT_LINKKIT_CLOSE        (LINKKIT_BASE + 3)
#define SYS_IOT_LINKKIT_REPORT       (LINKKIT_BASE + 4)
#define SYS_IOT_LINKKIT_QUERY        (LINKKIT_BASE + 5)
#define SYS_IOT_lINKKIT_TRIGGEREVENT (LINKKIT_BASE + 6)
#define SYS_IOT_SETLOGLEVEL          (LINKKIT_BASE + 7)
#define SYS_IOT_DUMPMEMORYSTATS      (LINKKIT_BASE + 8)
#define SYS_IOT_SETUPCONNINFO        (LINKKIT_BASE + 9)
#define SYS_IOT_IOCTL                (LINKKIT_BASE + 10)
#define SYS_IOT_HAL_IOCTL            (LINKKIT_BASE + 11)
#define SYS_IOT_REGSITERCALLBACK     (LINKKIT_BASE + 12)

#define LINKKIT_END (SYS_IOT_REGSITERCALLBACK)

/* ------------------- end -------------------*/
#define MAX_SYSCALL_NO (LINKKIT_END)

#endif /* SYSCALL_NO_H */

