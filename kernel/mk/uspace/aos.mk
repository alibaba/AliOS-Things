NAME := uspace

$(NAME)_MBINS_TYPE := app
$(NAME)_VERSION := 1.0.0
$(NAME)_SUMMARY := user space basic function

ifeq ($(COMPILER),)
$(NAME)_CFLAGS += -Wall -Werror
else ifeq ($(COMPILER),gcc)
$(NAME)_CFLAGS += -Wall -Werror
endif

# armcc compiler
ifeq ($(COMPILER),armcc)
ifeq ($(HOST_ARCH),Cortex-M0)
$(NAME)_SOURCES += ../syscall/usyscall/svc_asm/armv6m_svc_armcc.s
$(NAME)_PREBUILT_LIBRARY := lib/armcc/armv6m/libuspace.a
else ifeq ($(HOST_ARCH),Cortex-M3)
$(NAME)_PREBUILT_LIBRARY := lib/armcc/armv7m/libuspace.a
$(NAME)_SOURCES += ../syscall/usyscall/svc_asm/armv7m_svc_armcc.s
else ifeq ($(HOST_ARCH),Cortex-M4)
$(NAME)_PREBUILT_LIBRARY := lib/armcc/armv7m/libuspace.a
$(NAME)_SOURCES += ../syscall/usyscall/svc_asm/armv7m_svc_armcc.s
else ifeq ($(HOST_ARCH),Cortex-M7)
$(NAME)_PREBUILT_LIBRARY := lib/armcc/armv7m/libuspace.a
else ifeq ($(HOST_ARCH),Cortex-A5)
$(NAME)_PREBUILT_LIBRARY := lib/armcc/armv7a/libuspace.a
else ifeq ($(HOST_ARCH),Cortex-A7)
$(NAME)_PREBUILT_LIBRARY := lib/armcc/armv7a/libuspace.a
else ifeq ($(HOST_ARCH),Cortex-A9)
$(NAME)_PREBUILT_LIBRARY := lib/armcc/armv7a/libuspace.a
endif

# iar compiler
else ifeq ($(COMPILER),iar)
ifeq ($(HOST_ARCH),Cortex-M0)
$(NAME)_PREBUILT_LIBRARY := lib/iccarm/armv6m/libuspace.a
$(NAME)_SOURCES += ../syscall/usyscall/svc_asm/armv6m_svc_iar.s
else ifeq ($(HOST_ARCH),Cortex-M3)
$(NAME)_PREBUILT_LIBRARY := lib/iccarm/armv7m/libuspace.a
$(NAME)_SOURCES += ../syscall/usyscall/svc_asm/armv7m_svc_iar.s
else ifeq ($(HOST_ARCH),Cortex-M4)
$(NAME)_PREBUILT_LIBRARY := lib/iccarm/armv7m/libuspace.a
$(NAME)_SOURCES += ../syscall/usyscall/svc_asm/armv7m_svc_iar.s
else ifeq ($(HOST_ARCH),Cortex-M7)
$(NAME)_PREBUILT_LIBRARY := lib/iccarm/armv7m/libuspace.a
else ifeq ($(HOST_ARCH),Cortex-A5)
$(NAME)_PREBUILT_LIBRARY := lib/iccarm/armv7a/libuspace.a
else ifeq ($(HOST_ARCH),Cortex-A7)
$(NAME)_PREBUILT_LIBRARY := lib/iccarm/armv7a/libuspace.a
else ifeq ($(HOST_ARCH),Cortex-A9)
$(NAME)_PREBUILT_LIBRARY := lib/iccarm/armv7a/libuspace.a

endif

# default gcc compiler
else
ifeq ($(HOST_ARCH),Cortex-M0)
$(NAME)_PREBUILT_LIBRARY := lib/gcc/armv6m/libuspace.a
$(NAME)_SOURCES += ../syscall/usyscall/svc_asm/armv6m_svc_gcc.S
else ifeq ($(HOST_ARCH),Cortex-M3)
$(NAME)_PREBUILT_LIBRARY := lib/gcc/armv7m/libuspace.a
$(NAME)_SOURCES += ../syscall/usyscall/svc_asm/armv7m_svc_gcc.S
else ifeq ($(HOST_ARCH),Cortex-M4)
$(NAME)_PREBUILT_LIBRARY := lib/gcc/armv7m/libuspace.a
$(NAME)_SOURCES += ../syscall/usyscall/svc_asm/armv7m_svc_gcc.S
else ifeq ($(HOST_ARCH),Cortex-M7)
$(NAME)_PREBUILT_LIBRARY := lib/gcc/armv7m/libuspace.a
else ifeq ($(HOST_ARCH),Cortex-A5)
$(NAME)_PREBUILT_LIBRARY := lib/gcc/armv7a/libuspace.a
else ifeq ($(HOST_ARCH),Cortex-A7)
$(NAME)_PREBUILT_LIBRARY := lib/gcc/armv7a/libuspace.a
else ifeq ($(HOST_ARCH),Cortex-A9)
$(NAME)_PREBUILT_LIBRARY := lib/gcc/armv7a/libuspace.a
endif

endif


ifeq ($(RHINO_CONFIG_HAL_ADC_SYSCALL),y)
$(NAME)_SOURCES += ../syscall/usyscall/hal_adc_usyscall.c
endif

ifeq ($(RHINO_CONFIG_HAL_DAC_SYSCALL),y)
$(NAME)_SOURCES += ../syscall/usyscall/hal_dac_usyscall.c
endif

ifeq ($(RHINO_CONFIG_HAL_FLASH_SYSCALL),y)
$(NAME)_SOURCES += ../syscall/usyscall/hal_flash_usyscall.c
endif

ifeq ($(RHINO_CONFIG_HAL_GPIO_SYSCALL),y)
$(NAME)_SOURCES += ../syscall/usyscall/hal_gpio_usyscall.c
endif

ifeq ($(RHINO_CONFIG_HAL_I2C_SYSCALL),y)
$(NAME)_SOURCES += ../syscall/usyscall/hal_i2c_usyscall.c
endif

ifeq ($(RHINO_CONFIG_HAL_NAND_SYSCALL),y)
$(NAME)_SOURCES += ../syscall/usyscall/hal_nand_usyscall.c
endif

ifeq ($(RHINO_CONFIG_HAL_NOR_SYSCALL),y)
$(NAME)_SOURCES += ../syscall/usyscall/hal_nor_usyscall.c
endif

ifeq ($(RHINO_CONFIG_HAL_RTC_SYSCALL),y)
$(NAME)_SOURCES += ../syscall/usyscall/hal_rtc_usyscall.c
endif

ifeq ($(RHINO_CONFIG_HAL_SD_SYSCALL),y)
$(NAME)_SOURCES += ../syscall/usyscall/hal_sd_usyscall.c
endif

ifeq ($(RHINO_CONFIG_HAL_SPI_SYSCALL),y)
$(NAME)_SOURCES += ../syscall/usyscall/hal_spi_usyscall.c
endif

ifeq ($(RHINO_CONFIG_HAL_UART_SYSCALL),y)
$(NAME)_SOURCES += ../syscall/usyscall/hal_uart_usyscall.c
endif

ifeq ($(RHINO_CONFIG_UCLI_SYSCALL),y)
$(NAME)_SOURCES += ../syscall/usyscall/cli_usyscall.c
endif

ifeq ($(RHINO_CONFIG_LWIP_SYSCALL),y)
$(NAME)_SOURCES += ../syscall/usyscall/lwip_usyscall.c
endif

$(NAME)_SOURCES += ../syscall/usyscall/rhino_usyscall.c

ifeq ($(RHINO_CONFIG_LINKKIT_SYSCALL),y)
$(NAME)_SOURCES += ../syscall/usyscall/linkkit_usyscall.c
endif

$(NAME)_INCLUDES += src/umm
$(NAME)_INCLUDES += ../syscall/common/include

GLOBAL_INCLUDES += ./include
GLOBAL_INCLUDES += ../kspace/include
GLOBAL_INCLUDES += ../syscall/include

