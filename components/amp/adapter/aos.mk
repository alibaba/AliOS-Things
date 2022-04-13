NAME = libamp_adapter

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION := 0.0.1
$(NAME)_SUMMARY := amp adapter

$(NAME)_SOURCES := \
		platform/aos/peripheral/amp_hal_adc.c \
		platform/aos/peripheral/amp_hal_dac.c \
		platform/aos/peripheral/amp_hal_can.c \
		platform/aos/peripheral/amp_hal_gpio.c \
		platform/aos/peripheral/amp_hal_i2c.c \
		platform/aos/peripheral/amp_hal_pwm.c \
		platform/aos/peripheral/amp_hal_rtc.c \
		platform/aos/peripheral/amp_hal_spi.c \
		platform/aos/peripheral/amp_hal_timer.c \
		platform/aos/peripheral/amp_hal_uart.c \
		platform/aos/peripheral/amp_hal_wdg.c \
		platform/aos/network/amp_tcp.c \
		platform/aos/network/amp_udp.c \
		platform/aos/network/amp_dns.c \
		platform/aos/network/amp_wifi.c \
		platform/aos/network/amp_cellular.c \
		platform/aos/network/amp_httpc.c \
		platform/aos/amp_fs.c \
		platform/aos/amp_kv.c \
		platform/aos/amp_pm.c \
		platform/aos/amp_system.c \
		platform/aos/amp_tls_mbedtls.c

$(NAME)_INCLUDES := \
		include \
		include/peripheral \
		platform/aos \
		platform/aos/network \
		../components/linkkit \
		../components/linkkit/infra

ifeq ($(CONFIG_SYSINFO_DEVICE_NAME),ESP32)
GLOBAL_DEFINES += CONFIG_SPIFFS_PHYS_SZ=524288
GLOBAL_DEFINES += CONFIG_SPIFFS_LOG_BLOCK_SZ=65536
GLOBAL_DEFINES += CONFIG_SPIFFS_LOG_PAGE_SZ=256
else ifeq ($(CONFIG_SYSINFO_DEVICE_NAME),f429-nucleo)
GLOBAL_DEFINES += CONFIG_SPIFFS_PHYS_SZ=262144
GLOBAL_DEFINES += CONFIG_SPIFFS_LOG_BLOCK_SZ=16384
GLOBAL_DEFINES += CONFIG_SPIFFS_LOG_PAGE_SZ=2048
else
GLOBAL_DEFINES += CONFIG_SPIFFS_PHYS_SZ=262144
GLOBAL_DEFINES += CONFIG_SPIFFS_LOG_BLOCK_SZ=16384
GLOBAL_DEFINES += CONFIG_SPIFFS_LOG_PAGE_SZ=2048
endif

GLOBAL_DEFINES += JSENGINE BE_OS_AOS
GLOBAL_DEFINES += SPIFFS_OBJ_NAME_LEN=96
GLOBAL_DEFINES += SPIFFS_CACHE