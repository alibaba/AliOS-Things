NAME := BLE_demo

$(NAME)_MBINS_TYPE := app
$(NAME)_VERSION := 1.0.1
$(NAME)_SUMMARY := BLE demo
$(NAME)_SOURCES := maintask.c appdemo.c

$(NAME)_COMPONENTS += osal_aos cli ble_host_comp

GLOBAL_DEFINES += AOS_NO_WIFI CONFIG_BT

$(NAME)_INCLUDES += ./
