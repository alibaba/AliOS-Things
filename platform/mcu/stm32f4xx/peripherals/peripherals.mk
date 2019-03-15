
GLOBAL_INCLUDES += ./peripherals

$(NAME)_SOURCES += peripherals/platform_adc.c           \
                   peripherals/platform_rtc.c           \
                   peripherals/platform_gpio.c          \
                   peripherals/platform_i2c.c           \
                   peripherals/platform_rng.c           \
                   peripherals/platform_mcu_powersave.c \
                   peripherals/platform_pwm.c           \
                   peripherals/platform_flash.c         \
                   peripherals/platform_spi.c           \
                   peripherals/platform_uart.c          \
                   peripherals/platform_watchdog.c      \
                   peripherals/RingBufferUtils.c
