NAME := amp_adapter

$(NAME)_MBINS_TYPE := app
$(NAME)_VERSION    := 1.0.0
$(NAME)_SUMMARY    := amp adapter

$(NAME)_SOURCES += \
    platform/aos/aos_fs.c \
    platform/aos/haas700/amp_ota_port.c \
    platform/aos/aos_system.c \
    platform/aos/network/aos_cellular.c \
    platform/aos/network/aos_httpc.c \
    platform/aos/network/aos_netdev.c \
    platform/aos/network/aos_tcp.c \
    platform/aos/network/aos_udp.c \
    platform/aos/network/aos_wifi.c \
    platform/aos/peripheral/aos_hal_adc.c \
    platform/aos/peripheral/aos_hal_can.c \
    platform/aos/peripheral/aos_hal_flash.c \
    platform/aos/peripheral/aos_hal_gpio.c \
    platform/aos/peripheral/aos_hal_i2c.c \
    platform/aos/peripheral/aos_hal_lcd.c \
    platform/aos/peripheral/aos_hal_pwm.c \
    platform/aos/peripheral/aos_hal_rtc.c \
    platform/aos/peripheral/aos_hal_spi.c \
    platform/aos/peripheral/aos_hal_timer.c \
    platform/aos/peripheral/aos_hal_uart.c \
    platform/aos/peripheral/aos_hal_wdg.c \

$(NAME)_INCLUDES += \
    platform/aos/network

GLOBAL_INCLUDES += \
    include \
    platform/aos \
    platform/aos/haas700/ \
    include/peripheral
