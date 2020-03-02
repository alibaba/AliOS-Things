NAME := gatewayapp

$(NAME)_MBINS_TYPE := app
$(NAME)_VERSION := 1.0.0
$(NAME)_SUMMARY := uData sensor framework demo

$(NAME)_SOURCES :=   gateway.c
$(NAME)_COMPONENTS := cli sensor udata

$(NAME)_INCLUDES += ./

ifeq ($(GATEWAYAPP_CONFIG_PT_SCANNER),y)
GLOBAL_DEFINES     += PT_SCANNER
endif

ifeq ($(GATEWAYAPP_CONFIG_PT_SENSOR),y)
AOS_SENSOR_MODBUS_ENABLE = y
AOS_SENSOR_RTC_MAXIM_DS1307 = y
AOS_SENSOR_CANBUS_INV_MPU9250 = y
GLOBAL_DEFINES     += PT_SENSOR
endif

ifeq ($(GATEWAYAPP_CONFIG_DTC_ENABLE),y)
AOS_CONFIG_DTC_LINKKIT = y
endif

ifeq ($(AOS_CONFIG_DTC_LINKKIT),y)

$(NAME)_SOURCES += linkkit/app_entry.c
$(NAME)_SOURCES += linkkit/linkkit_example_solo.c

$(NAME)_COMPONENTS += yloop netmgr cjson libiot_devmodel

$(NAME)_INCLUDES += ./
GLOBAL_DEFINES += DTC_LINKKIT

else ifeq ($(AOS_CONFIG_DTC_MQTT),y)

$(NAME)_SOURCES    += mqtt/mqtt_example.c

$(NAME)_COMPONENTS += netmgr yloop
GLOBAL_DEFINES     += USE_LPTHREAD

GLOBAL_DEFINES     += DTC_MQTT
endif
