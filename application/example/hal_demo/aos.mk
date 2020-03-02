NAME := hal_demo

$(NAME)_MBINS_TYPE := app
$(NAME)_VERSION := 1.0.1
$(NAME)_SUMMARY := hal demo
$(NAME)_SOURCES := halappdemo.c maintask.c

$(NAME)_SOURCES += halapp_gpio.c  \
                   halapp_pwm.c   \
                   halapp_timer.c \
                   halapp_adc.c   \
                   halapp_dac.c   \
                   halapp_i2c.c   \
                   halapp_spi.c   \
                   halapp_watchdog.c \
                   halapp_rtc.c \
                   halapp_uart.c \

$(NAME)_COMPONENTS += osal_aos

GLOBAL_DEFINES += AOS_NO_WIFI

$(NAME)_INCLUDES += ./
