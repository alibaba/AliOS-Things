
NAME := RDA5981x

HOST_OPENOCD := rda5981x

$(NAME)_TYPE := kernel

$(NAME)_COMPONENTS += platform/arch/arm/armv7m
$(NAME)_COMPONENTS := libc rhino hal netmgr framework.common cjson cli digest_algorithm protocols.net

GLOBAL_DEFINES += CONFIG_AOS_KV_MULTIPTN_MODE
GLOBAL_DEFINES += CONFIG_AOS_KV_PTN=6
GLOBAL_DEFINES += CONFIG_AOS_KV_SECOND_PTN=7
GLOBAL_DEFINES += CONFIG_AOS_KV_PTN_SIZE=4096
GLOBAL_DEFINES += CONFIG_AOS_KV_BUFFER_SIZE=8192
GLOBAL_DEFINES += DEVICE_SERIAL=1
GLOBAL_DEFINES += DEVICE_STDIO_MESSAGES=1
GLOBAL_DEFINES += DEVICE_SPI=1
GLOBAL_DEFINES += DEVICE_FLASH=1
GLOBAL_DEFINES += TARGET_FLASH_CMSIS_ALGO=1
GLOBAL_DEFINES += RDA5991H_HW_VER=4

GLOBAL_DEFINES += RHINO_CONFIG_TASK_STACK_CUR_CHECK=1
GLOBAL_INCLUDES += ../../arch/arm/armv7m/gcc/m4 \
				   ../../../kernel/protocols/net/include
GLOBAL_INCLUDES += include \
                   startup \
                   driver \
                   cmsis \
                   middleware \
				   wifi/inc \
				   wifi/inc/arch \
				   wifi/inc/driver \
				   wifi/inc/lib \
				   wifi/inc/wpa


GLOBAL_CFLAGS += -DRDA5981x

GLOBAL_CFLAGS += -mcpu=cortex-m4 \
                 -march=armv7-m \
                 -mthumb -mthumb-interwork \
                 -mlittle-endian

GLOBAL_CFLAGS += -w

GLOBAL_LDFLAGS += -mcpu=cortex-m4        \
                  -mthumb -mthumb-interwork \
                  -mlittle-endian \
                  -nostartfiles \
                  --specs=nosys.specs \
                  $(CLIB_LDFLAGS_NANO_FLOAT)
				  #-libwifi_sta_ap_gcc
GLOBAL_LDFLAGS += platform/mcu/rda5981x/wifi/lib/TOOLCHAIN_GCC_ARM/libwifi_sta_ap.a

$(NAME)_CFLAGS  += -Wall -Werror -Wno-unused-variable -Wno-unused-parameter -Wno-implicit-function-declaration
$(NAME)_CFLAGS  += -Wno-type-limits -Wno-sign-compare -Wno-pointer-sign -Wno-uninitialized
$(NAME)_CFLAGS  += -Wno-return-type -Wno-unused-function -Wno-unused-but-set-variable
$(NAME)_CFLAGS  += -Wno-unused-value -Wno-strict-aliasing

GLOBAL_LDFLAGS += -T platform/mcu/rda5981x/rda5981x.ld

$(NAME)_SOURCES := startup/startup_rda5981x.s \
                   startup/soc_init.c \
                   ../../arch/arm/armv7m/gcc/m4/port_c.c \
                   ../../arch/arm/armv7m/gcc/m4/port_s.S \
                   aos/aos.c \
                   aos/soc_impl.c \
                   hal/system_rda5981x.c \
                   hal/rda_ccfg_api.c \
                   hal/hw.c \
                   hal/uart.c \
                   hal/gpio.c \
                   hal/spi.c \
				   hal/flash.c \
                   hal/rda_trng_api.c \
				   hal/wifi_port.c

$(NAME)_SOURCES += cmsis/cmsis_nvic.c \
                   driver/serial_api.c \
                   driver/gpio_api.c \
                   driver/mbed_pinmap_common.c \
                   driver/mbed_gpio.c \
                   driver/pinmap.c \
                   driver/spi_api.c \
				   driver/flash_api.c \
				   driver/rda_flash.c \
				   driver/flash_common_algo.c \
                   middleware/mbed_assert.c \
                   middleware/mbed_board.c \
                   middleware/mbed_critical.c \
                   middleware/mbed_error.c 
				   
$(NAME)_SOURCES += wifi/rda_sys_wrapper.c \
				   wifi/rda59xx_daemon.c \
				   wifi/rda59xx_lwip.c	\

                   
