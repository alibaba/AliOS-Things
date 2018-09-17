NAME := vfs_device

$(NAME)_MBINS_TYPE  := kernel

$(NAME)_SOURCES     += vfs_adc.c
$(NAME)_SOURCES     += vfs_uart.c
$(NAME)_SOURCES     += vfs_gpio.c
$(NAME)_SOURCES     += vfs_spi.c
$(NAME)_SOURCES     += vfs_pwm.c
$(NAME)_SOURCES     += vfs_rtc.c
$(NAME)_SOURCES     += vfs_wdg.c
$(NAME)_SOURCES     += vfs_i2c.c

$(NAME)_INCLUDES    += ../include/device/ \
                       ../include/  \
                       ../../hal/soc/
