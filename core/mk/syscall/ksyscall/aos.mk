NAME := ksyscall

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION := 1.0.1
$(NAME)_SUMMARY := kernel syscall

ifeq ($(COMPILER),)
$(NAME)_CFLAGS += -Wall -Werror
else ifeq ($(COMPILER),gcc)
$(NAME)_CFLAGS += -Wall -Werror
endif

ifeq ($(RHINO_CONFIG_HAL_ADC_SYSCALL),y)
$(NAME)_SOURCES += hal_adc_ksyscall.c
endif

ifeq ($(RHINO_CONFIG_HAL_DAC_SYSCALL),y)
$(NAME)_SOURCES += hal_dac_ksyscall.c
endif

ifeq ($(RHINO_CONFIG_HAL_FLASH_SYSCALL),y)
$(NAME)_SOURCES += hal_flash_ksyscall.c
endif

ifeq ($(RHINO_CONFIG_HAL_GPIO_SYSCALL),y)
$(NAME)_SOURCES += hal_gpio_ksyscall.c
endif

ifeq ($(RHINO_CONFIG_HAL_I2C_SYSCALL),y)
$(NAME)_SOURCES += hal_i2c_ksyscall.c
endif

ifeq ($(RHINO_CONFIG_HAL_NAND_SYSCALL),y)
$(NAME)_SOURCES += hal_nand_ksyscall.c
endif

ifeq ($(RHINO_CONFIG_HAL_NOR_SYSCALL),y)
$(NAME)_SOURCES += hal_nor_ksyscall.c
endif

ifeq ($(RHINO_CONFIG_HAL_RTC_SYSCALL),y)
$(NAME)_SOURCES += hal_rtc_ksyscall.c
endif

ifeq ($(RHINO_CONFIG_HAL_SD_SYSCALL),y)
$(NAME)_SOURCES += hal_sd_ksyscall.c
endif

ifeq ($(RHINO_CONFIG_HAL_SPI_SYSCALL),y)
$(NAME)_SOURCES += hal_spi_ksyscall.c
endif

ifeq ($(RHINO_CONFIG_HAL_UART_SYSCALL),y)
$(NAME)_SOURCES += hal_uart_ksyscall.c
endif

ifeq ($(RHINO_CONFIG_UCLI_SYSCALL),y)
$(NAME)_SOURCES += cli_ksyscall.c
endif

ifeq ($(RHINO_CONFIG_LWIP_SYSCALL),y)
$(NAME)_SOURCES += lwip_ksyscall.c
endif

$(NAME)_SOURCES += rhino_ksyscall.c

ifeq ($(RHINO_CONFIG_LINKKIT_SYSCALL),y)
$(NAME)_SOURCES += linkkit_ksyscall.c
endif

ifeq ($(RHINO_CONFIG_DYN_SYSCALL_TBL),y)
$(NAME)_SOURCES += dyn_syscall.c
endif

$(NAME)_SOURCES += syscall_tbl.c

$(NAME)_INCLUDES += include
$(NAME)_INCLUDES += ../common/include

