NAME := ldapp

$(NAME)_SOURCES    := mqtt-example.c

ifeq ($(CONFIG_SYSINFO_DEVICE_NAME),developerkit)
$(NAME)_SOURCES    += AliOS_Things_logo.c
$(NAME)_SOURCES    += sensor_display.c
GLOBAL_DEFINES += LITTLEVGL_DISPLAY
endif

$(NAME)_COMPONENTS := cli sensor connectivity.mqtt cjson fota netmgr framework.common
GLOBAL_DEFINES     += ALIOT_DEBUG IOTX_DEBUG USE_LPTHREAD

$(NAME)_COMPONENTS += framework.GUI.littlevGL
GLOBAL_INCLUDES     += ../framework/GUI/littlevGL/
GLOBAL_DEFINES += LITTLEVGL_DEVELOPERKIT
