NAME := gatewayapp

$(NAME)_MBINS_TYPE := app
$(NAME)_VERSION := 1.0.0
$(NAME)_SUMMARY := gatewayapp


ifeq ($(PT_SCANNER),1)
GLOBAL_DEFINES += PT_SCANNER=1
endif

ifeq ($(PT_SENSOR),1)
GLOBAL_DEFINES += PT_SENSOR=1
GLOBAL_DEFINES += AOS_SENSOR_ACC_GYRO_INV_MPU9250
GLOBAL_DEFINES += AOS_SENSOR_RTC_MAXIM_DS1307
modbus_sensor_enable=1
GLOBAL_DEFINES += RHINO_CONFIG_MODBUS=1
GLOBAL_DEFINES += UDATA_TASK_STACK_SIZE=6144
endif



ifeq ($(dtc),1)

GLOBAL_DEFINES      +=  MQTT_DIRECT  ALIOT_DEBUG IOTX_DEBUG USE_LPTHREAD COAP_WITH_YLOOP DEPRECATED_LINKKIT __DEMO__

$(NAME)_SOURCES     := gateway.c gateway_app.c gateway_tsl.c

$(NAME)_COMPONENTS := feature.linkkit-noawss sensor cjson netmgr udata mbmaster

ifeq ($(LWIP),1)
$(NAME)_COMPONENTS  += lwip
no_with_lwip := 0
endif

ifneq ($(HOST_MCU_FAMILY),mcu_esp8266)
$(NAME)_COMPONENTS  += cli
GLOBAL_DEFINES += CONFIG_AOS_CLI
else
GLOBAL_DEFINES +=FOTA_RAM_LIMIT_MODE AWSS_NEED_REBOOT
endif

GLOBAL_DEFINES += DATA_TO_CLOUD

ifeq ($(GW_LINKEDGE_WS),1)
LINKEDGE_SERVER ?= "192.168.1.100"
GLOBAL_DEFINES += GW_LINKEDGE_WS=1 LINKEDGE_SERVER="\"${LINKEDGE_SERVER}\""
$(NAME)_SOURCES += linkedge_ws.c
$(NAME)_COMPONENTS += network.websockets
else

endif

else

$(NAME)_SOURCES := gateway.c

$(NAME)_COMPONENTS := netmgr udata sensor mbmaster cjson

GLOBAL_INCLUDES += .

endif
