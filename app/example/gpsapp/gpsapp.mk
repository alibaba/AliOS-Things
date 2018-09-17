NAME := gpsapp

$(NAME)_SOURCES := gpsapp_sample.c

$(NAME)_COMPONENTS := cli network.sal.atparser cjson middleware.uagent.uota netmgr middleware.common device.gps middleware.udata connectivity.mqtt

$(NAME)_INCLUDES := \
    ./include \
    ../../include/aos \
    ../../kernel/hal/include \
    ../../utility/iotx-utils/sdk-impl
	
GLOBAL_INCLUDES += .

GLOBAL_DEFINES      += ALIOT_DEBUG IOTX_DEBUG

GLOBAL_INCLUDES += ./
