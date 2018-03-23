NAME := gpsapp

$(NAME)_SOURCES := gpsapp_sample.c

$(NAME)_COMPONENTS := cli atparser cjson fota netmgr framework.common device.gps uData connectivity.mqtt

$(NAME)_INCLUDES := \
    ./include \
    ../../include/aos \
    ../../include/hal \
    ../../utility/iotx-utils/sdk-impl
	
GLOBAL_INCLUDES += .

GLOBAL_DEFINES      += ALIOT_DEBUG IOTX_DEBUG


