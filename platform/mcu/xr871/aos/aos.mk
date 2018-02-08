NAME := aos

$(NAME)_TYPE := kernel
$(NAME)_SOURCES := aos.c \
                   aos_impl.c \
                   soc_impl.c \
                   hal/soc/adc.c \
                   hal/soc/flash.c \
                   hal/soc/gpio.c \
                   hal/soc/i2c.c \
                   hal/soc/pwm.c \
                   hal/soc/rtc.c \
                   hal/soc/sd.c \
                   hal/soc/timer.c \
                   hal/soc/uart.c \
                   hal/soc/wdg.c \
                   hal/fota_port.c \
                   hal/wifi_port.c \
                   hal/wifi.c \
                   hal/hal.c \
                   cli_ext/cli_ext.c \
                   cli_ext/cli_mem.c \
                   cli_ext/cli_upgrade.c