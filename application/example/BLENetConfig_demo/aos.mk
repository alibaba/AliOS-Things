NAME := BLENetConfig_demo

$(NAME)_MBINS_TYPE := app
$(NAME)_VERSION := 1.0.1
$(NAME)_SUMMARY := BLE NetConfig demo
$(NAME)_SOURCES := maintask.c appdemo.c

$(NAME)_COMPONENTS += osal_aos cli ble_host_comp netmgr cjson network littlefs

GLOBAL_DEFINES += CONFIG_BT AOS_NET_WITH_WIFI

$(NAME)_INCLUDES += ./
