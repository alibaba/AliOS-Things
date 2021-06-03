TARGET = libadapter.a
MODULE = adapter

MOD_SOURCES := \
		platform/linux/peripheral/amp_hal_adc.c \
		platform/linux/peripheral/amp_hal_can.c \
		platform/linux/peripheral/amp_hal_gpio.c \
		platform/linux/peripheral/amp_hal_i2c.c \
		platform/linux/peripheral/amp_hal_pwm.c \
		platform/linux/peripheral/amp_hal_rtc.c \
		platform/linux/peripheral/amp_hal_spi.c \
		platform/linux/peripheral/amp_hal_timer.c \
		platform/linux/peripheral/amp_hal_uart.c \
		platform/linux/peripheral/amp_hal_vuart.c \
		platform/linux/peripheral/amp_hal_wdg.c \
		platform/linux/network/amp_cellular.c \
		platform/linux/network/amp_wifi.c \
		platform/linux/network/amp_tcp.c \
		platform/linux/network/amp_udp.c \
		platform/linux/network/amp_httpc.c \
		platform/linux/amp_flash.c \
		platform/linux/amp_fs.c \
		platform/linux/amp_kv.c \
		platform/linux/amp_pm.c \
		platform/linux/amp_system.c \
		platform/linux/amp_socket.c \

MOD_INCLUDES := \
		include \
		include/peripheral \
		../components/http/include \
		../components/ulog \
		../main \
		../utils/mbedtls/include \
		../utils/mbedtls/platform/include \
		../utils/mbedtls/platform/amp/include \
		./platform/linux \
		portfiles \

ifeq ($(ADDON), ui)
MOD_SOURCES += \
		platform/linux/aui_drivers/indev/FT5406EE8.c \
		platform/linux/aui_drivers/indev/keyboard.c \
		platform/linux/aui_drivers/indev/mouse.c \
		platform/linux/aui_drivers/indev/mousewheel.c \
		platform/linux/aui_drivers/indev/evdev.c \
		platform/linux/aui_drivers/indev/XPT2046.c \
		platform/linux/aui_drivers/display/fbdev.c \
		platform/linux/aui_drivers/display/scaler.c \
		platform/linux/aui_drivers/display/monitor.c \
		platform/linux/aui_drivers/display/R61581.c \
		platform/linux/aui_drivers/display/SSD1963.c \
		platform/linux/aui_drivers/display/ST7565.c

MOD_INCLUDES += \
		platform/linux/aui_drivers \
		platform/linux/aui_drivers/display \
		platform/linux/aui_drivers/indev \
		../ui/aui/include \
		../ui/aui/libs \
		../ui/aui/libs/lvgl\
		../ui/aui/libs/lvgl/lv_misc \
		../ui/aui/libs/lvgl/lv_font \
		../ui/aui/libs/lvgl/lv_core \
		../ui/aui/libs/lvgl/lv_draw \
		../ui/aui/libs/lvgl/lv_hal \
		../ui/aui/libs/lvgl/lv_objx \
		../ui/aui/libs/lvgl/lv_themes  \
		/usr/include/SDL2
endif

include $(TOOLS_DIR)/rules.mk
