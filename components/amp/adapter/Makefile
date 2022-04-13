TARGET = libadapter.a
MODULE = adapter

MOD_SOURCES := \
		platform/linux/peripheral/amp_hal_adc.c \
		platform/linux/peripheral/amp_hal_dac.c \
		platform/linux/peripheral/amp_hal_can.c \
		platform/linux/peripheral/amp_hal_gpio.c \
		platform/linux/peripheral/amp_hal_i2c.c \
		platform/linux/peripheral/amp_hal_pwm.c \
		platform/linux/peripheral/amp_hal_rtc.c \
		platform/linux/peripheral/amp_hal_spi.c \
		platform/linux/peripheral/amp_hal_timer.c \
		platform/linux/peripheral/amp_hal_uart.c \
		platform/linux/peripheral/amp_hal_wdg.c \
		platform/linux/network/amp_cellular.c \
		platform/linux/network/amp_wifi.c \
		platform/linux/network/amp_tcp.c \
		platform/linux/network/amp_udp.c \
		platform/linux/network/amp_httpc.c \
		platform/linux/advanced/audio/amp_audio.c \
		platform/linux/advanced/audio/amp_tts.c \
		platform/linux/amp_fs.c \
		platform/linux/amp_kv.c \
		platform/linux/amp_pm.c \
		platform/linux/amp_system.c \
		platform/linux/amp_tls_mbedtls.c

MOD_INCLUDES := \
		include \
		include/peripheral \
		../components/linkkit \
		../components/linkkit/infra \
		../components/http/include \
		../components/ulog \
		../main \
		../utils/mbedtls/include \
		./platform/linux

include $(TOOLS_DIR)/rules.mk
