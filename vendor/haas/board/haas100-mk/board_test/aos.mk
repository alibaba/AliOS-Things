NAME := board_test

$(NAME)_MBINS_TYPE := app
$(NAME)_VERSION := 1.0.1
$(NAME)_SUMMARY := haas100 board test
$(NAME)_SOURCES := board_test.c led_test.c key_test.c sd_test.c watchdog_test.c uart_test.c rs485_test.c ch395_test.c i2c_test.c pwm_test.c adc_test.c

$(NAME)_COMPONENTS += osal_aos fatfs device_sal_ch395 littlefs netmgr cli

GLOBAL_DEFINES += CONFIG_AOS_FATFS_SUPPORT_MMC

$(NAME)_INCLUDES += ./
