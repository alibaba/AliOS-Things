
NAME := uDataapp

ifeq ($(dtc),1)

GLOBAL_DEFINES      +=  MQTT_DIRECT  ALIOT_DEBUG IOTX_DEBUG USE_LPTHREAD   COAP_WITH_YLOOP

$(NAME)_SOURCES     := uData-example.c uData_app.c linkkit_export.c lite_queue.c uData_tsl.c

$(NAME)_COMPONENTS := connectivity.mqtt middleware.uagent.uota netmgr middleware.common device.sensor middleware.udata

ifeq ($(LWIP),1)
$(NAME)_COMPONENTS  += network.lwip
no_with_lwip := 0
endif

ifneq ($(HOST_MCU_FAMILY),esp8266)
$(NAME)_COMPONENTS  += cli
GLOBAL_DEFINES += CONFIG_AOS_CLI
else
GLOBAL_DEFINES +=UOTA_RAM_LIMIT_MODE AWSS_NEED_REBOOT
endif
#GLOBAL_CFLAGS += -DON_PRE2=1

GLOBAL_DEFINES += DATA_TO_CLOUD

else

$(NAME)_SOURCES := uData-example.c

ifeq ($(CONFIG_SYSINFO_DEVICE_NAME),developerkit)
$(NAME)_SOURCES    += house.c
$(NAME)_SOURCES    += weather.c
$(NAME)_SOURCES    += AliOS_Things_logo.c
$(NAME)_SOURCES    += sensor_display.c
GLOBAL_DEFINES += LITTLEVGL_DISPLAY
GLOBAL_DEFINES += BONE_ENGINE_NOT_NEED
endif

$(NAME)_COMPONENTS := cli netmgr middleware.common device.sensor middleware.udata 3rdparty.experimental.gui.littlevGL

GLOBAL_DEFINES += LITTLEVGL_DEVELOPERKIT

GLOBAL_INCLUDES += .

endif

GLOBAL_INCLUDES := ../../../device/sensor/include
