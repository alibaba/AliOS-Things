NAME := kspace

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION := 1.0.1
$(NAME)_SUMMARY := syscall micro kernel

ifeq ($(COMPILER),)
$(NAME)_CFLAGS += -Wall -Werror
else ifeq ($(COMPILER),gcc)
$(NAME)_CFLAGS += -Wall -Werror
endif

ifeq ($(COMPILER),armcc)
ifeq ($(HOST_ARCH),Cortex-M0)
$(NAME)_PREBUILT_LIBRARY := lib/armcc/armv6m/libkspace.a
else ifeq ($(HOST_ARCH),Cortex-M3)
$(NAME)_PREBUILT_LIBRARY := lib/armcc/armv7m/libkspace.a
else ifeq ($(HOST_ARCH),Cortex-M4)
$(NAME)_PREBUILT_LIBRARY := lib/armcc/armv7m/libkspace.a
else ifeq ($(HOST_ARCH),Cortex-M7)
$(NAME)_PREBUILT_LIBRARY := lib/armcc/armv7m/libkspace.a
else ifeq ($(HOST_ARCH),Cortex-A5)
$(NAME)_PREBUILT_LIBRARY := lib/armcc/armv7a/libkspace.a
else ifeq ($(HOST_ARCH),Cortex-A7)
$(NAME)_PREBUILT_LIBRARY := lib/armcc/armv7a/libkspace.a
else ifeq ($(HOST_ARCH),Cortex-A9)
$(NAME)_PREBUILT_LIBRARY := lib/armcc/armv7a/libkspace.a
endif

else ifeq ($(COMPILER),iar)
ifeq ($(HOST_ARCH),Cortex-M0)
$(NAME)_PREBUILT_LIBRARY := lib/iccarm/armv6m/libkspace.a
else ifeq ($(HOST_ARCH),Cortex-M3)
$(NAME)_PREBUILT_LIBRARY := lib/iccarm/armv7m/libkspace.a
else ifeq ($(HOST_ARCH),Cortex-M4)
$(NAME)_PREBUILT_LIBRARY := lib/iccarm/armv7m/libkspace.a
else ifeq ($(HOST_ARCH),Cortex-M7)
$(NAME)_PREBUILT_LIBRARY := lib/iccarm/armv7m/libkspace.a
else ifeq ($(HOST_ARCH),Cortex-A5)
$(NAME)_PREBUILT_LIBRARY := lib/iccarm/armv7a/libkspace.a
else ifeq ($(HOST_ARCH),Cortex-A7)
$(NAME)_PREBUILT_LIBRARY := lib/iccarm/armv7a/libkspace.a
else ifeq ($(HOST_ARCH),Cortex-A9)
$(NAME)_PREBUILT_LIBRARY := lib/iccarm/armv7a/libkspace.a
endif

else
ifeq ($(HOST_ARCH),Cortex-M0)
$(NAME)_PREBUILT_LIBRARY := lib/gcc/armv6m/libkspace.a
else ifeq ($(HOST_ARCH),Cortex-M3)
$(NAME)_PREBUILT_LIBRARY := lib/gcc/armv7m/libkspace.a
else ifeq ($(HOST_ARCH),Cortex-M4)
$(NAME)_PREBUILT_LIBRARY := lib/gcc/armv7m/libkspace.a
else ifeq ($(HOST_ARCH),Cortex-M7)
$(NAME)_PREBUILT_LIBRARY := lib/gcc/armv7m/libkspace.a
else ifeq ($(HOST_ARCH),Cortex-A5)
$(NAME)_PREBUILT_LIBRARY := lib/gcc/armv7a/libkspace.a
else ifeq ($(HOST_ARCH),Cortex-A7)
$(NAME)_PREBUILT_LIBRARY := lib/gcc/armv7a/libkspace.a
else ifeq ($(HOST_ARCH),Cortex-A9)
$(NAME)_PREBUILT_LIBRARY := lib/gcc/armv7a/libkspace.a
endif

endif

ifeq ($(RHINO_CONFIG_HAL_ADC_SYSCALL),y)
$(NAME)_SOURCES += ../syscall/ksyscall/hal_adc_ksyscall.c
endif

ifeq ($(RHINO_CONFIG_HAL_DAC_SYSCALL),y)
$(NAME)_SOURCES += ../syscall/ksyscall/hal_dac_ksyscall.c
endif

ifeq ($(RHINO_CONFIG_HAL_FLASH_SYSCALL),y)
$(NAME)_SOURCES += ../syscall/ksyscall/hal_flash_ksyscall.c
endif

ifeq ($(RHINO_CONFIG_HAL_GPIO_SYSCALL),y)
$(NAME)_SOURCES += ../syscall/ksyscall/hal_gpio_ksyscall.c
endif

ifeq ($(RHINO_CONFIG_HAL_I2C_SYSCALL),y)
$(NAME)_SOURCES += ../syscall/ksyscall/hal_i2c_ksyscall.c
endif

ifeq ($(RHINO_CONFIG_HAL_NAND_SYSCALL),y)
$(NAME)_SOURCES += ../syscall/ksyscall/hal_nand_ksyscall.c
endif

ifeq ($(RHINO_CONFIG_HAL_NOR_SYSCALL),y)
$(NAME)_SOURCES += ../syscall/ksyscall/hal_nor_ksyscall.c
endif

ifeq ($(RHINO_CONFIG_HAL_RTC_SYSCALL),y)
$(NAME)_SOURCES += ../syscall/ksyscall/hal_rtc_ksyscall.c
endif

ifeq ($(RHINO_CONFIG_HAL_SD_SYSCALL),y)
$(NAME)_SOURCES += ../syscall/ksyscall/hal_sd_ksyscall.c
endif

ifeq ($(RHINO_CONFIG_HAL_SPI_SYSCALL),y)
$(NAME)_SOURCES += ../syscall/ksyscall/hal_spi_ksyscall.c
endif

ifeq ($(RHINO_CONFIG_HAL_UART_SYSCALL),y)
$(NAME)_SOURCES += ../syscall/ksyscall/hal_uart_ksyscall.c
endif

ifeq ($(RHINO_CONFIG_UCLI_SYSCALL),y)
$(NAME)_SOURCES += ../syscall/ksyscall/cli_ksyscall.c
endif

ifeq ($(RHINO_CONFIG_LWIP_SYSCALL),y)
$(NAME)_SOURCES += ../syscall/ksyscall/lwip_ksyscall.c
endif

$(NAME)_SOURCES += ../syscall/ksyscall/rhino_ksyscall.c

ifeq ($(RHINO_CONFIG_LINKKIT_SYSCALL),y)
$(NAME)_SOURCES += ../syscall/ksyscall/linkkit_ksyscall.c
endif

ifeq ($(RHINO_CONFIG_DYN_SYSCALL_TBL),y)
$(NAME)_SOURCES += ../syscall/ksyscall/dyn_syscall.c
endif

$(NAME)_SOURCES += ../syscall/ksyscall/syscall_tbl.c

$(NAME)_INCLUDES += ../syscall/ksyscall/include
$(NAME)_INCLUDES += ../syscall/common/include

GLOBAL_INCLUDES += ./include
GLOBAL_INCLUDES += ../syscall/include

