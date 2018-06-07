
NAME := uDataapp

ifeq ($(dtc),1)

GLOBAL_DEFINES      +=  MQTT_DIRECT  ALIOT_DEBUG IOTX_DEBUG USE_LPTHREAD   COAP_WITH_YLOOP

$(NAME)_SOURCES     := uData-example.c uData_app.c linkkit_export.c lite_queue.c uData_tsl.c

$(NAME)_COMPONENTS := protocol.linkkit.iotkit connectivity.mqtt fota netmgr framework.common  protocol.linkkit.cm protocol.linkkit.dm protocol.linkkit.alcs ywss4linkkit  device.sensor uData

ifeq ($(LWIP),1)
$(NAME)_COMPONENTS  += protocols.net
no_with_lwip := 0
endif

ifneq ($(HOST_MCU_FAMILY),esp8266)
$(NAME)_COMPONENTS  += cli
GLOBAL_DEFINES += CONFIG_AOS_CLI
else
GLOBAL_DEFINES +=FOTA_RAM_LIMIT_MODE AWSS_NEED_REBOOT
endif
#GLOBAL_CFLAGS += -DON_PRE2=1

GLOBAL_DEFINES += DATA_TO_CLOUD

else

$(NAME)_SOURCES := uData-example.c

$(NAME)_COMPONENTS := cli netmgr framework.common device.sensor uData

GLOBAL_INCLUDES += .

endif