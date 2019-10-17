NAME := usyscall

$(NAME)_MBINS_TYPE := app
$(NAME)_VERSION := 1.0.1
$(NAME)_SUMMARY := user space syscall

ifeq ($(COMPILER),)
$(NAME)_CFLAGS += -Wall -Werror
else ifeq ($(COMPILER),gcc)
$(NAME)_CFLAGS += -Wall -Werror
endif

# armcc compiler
ifeq ($(COMPILER),armcc)
ifeq ($(HOST_ARCH),Cortex-M0)
$(NAME)_SOURCES += svc_asm/armv6m_svc_armcc.s
else ifeq ($(HOST_ARCH),Cortex-M3)
$(NAME)_SOURCES += svc_asm/armv7m_svc_armcc.s
else ifeq ($(HOST_ARCH),Cortex-M4)
$(NAME)_SOURCES += svc_asm/armv7m_svc_armcc.s
endif

# iar compiler
else ifeq ($(COMPILER),iar)
ifeq ($(HOST_ARCH),Cortex-M0)
$(NAME)_SOURCES += svc_asm/armv6m_svc_iar.s
else ifeq ($(HOST_ARCH),Cortex-M3)
$(NAME)_SOURCES += svc_asm/armv7m_svc_iar.s
else ifeq ($(HOST_ARCH),Cortex-M4)
$(NAME)_SOURCES += svc_asm/armv7m_svc_iar.s
endif

# default gcc compiler
else
ifeq ($(HOST_ARCH),Cortex-M0)
$(NAME)_SOURCES += svc_asm/armv6m_svc_gcc.S
else ifeq ($(HOST_ARCH),Cortex-M3)
$(NAME)_SOURCES += svc_asm/armv7m_svc_gcc.S
else ifeq ($(HOST_ARCH),Cortex-M4)
$(NAME)_SOURCES += svc_asm/armv7m_svc_gcc.S
endif

endif


ifeq ($(RHINO_CONFIG_HAL_ADC_SYSCALL),y)
$(NAME)_SOURCES += hal_adc_usyscall.c
endif

ifeq ($(RHINO_CONFIG_HAL_DAC_SYSCALL),y)
$(NAME)_SOURCES += hal_dac_usyscall.c
endif

ifeq ($(RHINO_CONFIG_HAL_FLASH_SYSCALL),y)
$(NAME)_SOURCES += hal_flash_usyscall.c
endif

ifeq ($(RHINO_CONFIG_HAL_GPIO_SYSCALL),y)
$(NAME)_SOURCES += hal_gpio_usyscall.c
endif

ifeq ($(RHINO_CONFIG_HAL_I2C_SYSCALL),y)
$(NAME)_SOURCES += hal_i2c_usyscall.c
endif

ifeq ($(RHINO_CONFIG_HAL_NAND_SYSCALL),y)
$(NAME)_SOURCES += hal_nand_usyscall.c
endif

ifeq ($(RHINO_CONFIG_HAL_NOR_SYSCALL),y)
$(NAME)_SOURCES += hal_nor_usyscall.c
endif

ifeq ($(RHINO_CONFIG_HAL_RTC_SYSCALL),y)
$(NAME)_SOURCES += hal_rtc_usyscall.c
endif

ifeq ($(RHINO_CONFIG_HAL_SD_SYSCALL),y)
$(NAME)_SOURCES += hal_sd_usyscall.c
endif

ifeq ($(RHINO_CONFIG_HAL_SPI_SYSCALL),y)
$(NAME)_SOURCES += hal_spi_usyscall.c
endif

ifeq ($(RHINO_CONFIG_HAL_UART_SYSCALL),y)
$(NAME)_SOURCES += hal_uart_usyscall.c
endif

ifeq ($(RHINO_CONFIG_UCLI_SYSCALL),y)
$(NAME)_SOURCES += cli_usyscall.c
endif

ifeq ($(RHINO_CONFIG_LWIP_SYSCALL),y)
$(NAME)_SOURCES += lwip_usyscall.c
endif

$(NAME)_SOURCES += rhino_usyscall.c

ifeq ($(RHINO_CONFIG_LINKKIT_SYSCALL),y)
$(NAME)_SOURCES += linkkit_usyscall.c
endif

$(NAME)_INCLUDES += ../common/include

