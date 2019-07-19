/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#ifndef U_DEFAULT_CONFIG_H
#define U_DEFAULT_CONFIG_H

#ifndef RHINO_CONFIG_PROTECTED
#define RHINO_CONFIG_PROTECTED 1
#endif

/**
 * load elf file from ram
 * default: load file from file system
 */
#ifndef RHINO_CONFIG_RAM_LOAD
#define RHINO_CONFIG_RAM_LOAD 0
#endif

#ifndef RHINO_CONFIG_DYN_SYSCALL_TBL
#define RHINO_CONFIG_DYN_SYSCALL_TBL 0
#endif

/* Support user space task delete, kernel can
 * malloc, free buffer to user space heap */
#ifndef RHINO_CONFIG_URES
#define RHINO_CONFIG_URES 1
#endif

/* Support user space CLI */
#ifndef RHINO_CONFIG_UCLI
#define RHINO_CONFIG_UCLI 0
#endif

#ifndef RHINO_CONFIG_UIPC
#define RHINO_CONFIG_UIPC 0
#endif

/* Support user space file system */
#ifndef RHINO_CONFIG_UFS
#define RHINO_CONFIG_UFS 0
#endif

/* Support user space TCPIP utility */
#ifndef RHINO_CONFIG_UTCPIP
#define RHINO_CONFIG_UTCPIP 0
#endif

/*----------------- LWIP ----------------*/
#if (RHINO_CONFIG_UTCPIP > 0)
#define RHINO_CONFIG_LWIP_SYSCALL 1
#endif

/* Support user space linkkit */
#ifndef RHINO_CONFIG_ULINKKIT
#define RHINO_CONFIG_ULINKKIT 0
#endif

/*--------------- linkkit ---------------*/
#if (RHINO_CONFIG_ULINKKIT > 0)
#define RHINO_CONFIG_LINKKIT_SYSCALL 1
#endif

/**
 * Support user space callback, kernel sends
 * callback msg to user space agent(callback task),
 * the callback is executed in user space
 */
#ifndef RHINO_CONFIG_UCALLBACK
#define RHINO_CONFIG_UCALLBACK 0
#endif

/*----------------- CLI ----------------*/
#if (RHINO_CONFIG_UCLI > 0)
#define RHINO_CONFIG_UCLI_SYSCALL 1
#endif

/*----------------- HAL ----------------*/
#ifndef RHINO_CONFIG_HAL_ADC_SYSCALL
#define RHINO_CONFIG_HAL_ADC_SYSCALL 0
#endif

#ifndef RHINO_CONFIG_HAL_DAC_SYSCALL
#define RHINO_CONFIG_HAL_DAC_SYSCALL 0
#endif

#ifndef RHINO_CONFIG_HAL_FLASH_SYSCALL
#define RHINO_CONFIG_HAL_FLASH_SYSCALL 0
#endif

#ifndef RHINO_CONFIG_HAL_GPIO_SYSCALL
#define RHINO_CONFIG_HAL_GPIO_SYSCALL 0
#endif

#ifndef RHINO_CONFIG_HAL_I2C_SYSCALL
#define RHINO_CONFIG_HAL_I2C_SYSCALL 0
#endif

#ifndef RHINO_CONFIG_HAL_NAND_SYSCALL
#define RHINO_CONFIG_HAL_NAND_SYSCALL 0
#endif

#ifndef RHINO_CONFIG_HAL_NOR_SYSCALL
#define RHINO_CONFIG_HAL_NOR_SYSCALL 0
#endif

#ifndef RHINO_CONFIG_HAL_RTC_SYSCALL
#define RHINO_CONFIG_HAL_RTC_SYSCALL 0
#endif

#ifndef RHINO_CONFIG_HAL_SD_SYSCALL
#define RHINO_CONFIG_HAL_SD_SYSCALL 0
#endif

#ifndef RHINO_CONFIG_HAL_SPI_SYSCALL
#define RHINO_CONFIG_HAL_SPI_SYSCALL 1
#endif

#ifndef RHINO_CONFIG_HAL_UART_SYSCALL
#define RHINO_CONFIG_HAL_UART_SYSCALL 1
#endif

#endif /* U_DEFAULT_CONFIG_H */

