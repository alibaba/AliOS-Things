NAME := hmi

$(NAME)_SOURCES    := hmi.c

$(NAME)_COMPONENTS := cli connectivity.mqtt cjson netmgr framework.common
GLOBAL_DEFINES     += ALIOT_DEBUG IOTX_DEBUG USE_LPTHREAD

# for developerkit board
ifeq ($(CONFIG_SYSINFO_DEVICE_NAME),developerkit)
$(NAME)_SOURCES    += devkit_display/AliOS_Things_logo.c
$(NAME)_SOURCES    += devkit_display/chat_display.c
GLOBAL_DEFINES += LITTLEVGL_DISPLAY LITTLEVGL_DEVELOPERKIT
$(NAME)_COMPONENTS += framework.GUI.littlevGL
GLOBAL_INCLUDES     += ../../framework/GUI/littlevGL/ devkit_display/
endif
