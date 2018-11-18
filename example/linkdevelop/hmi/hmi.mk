NAME := hmi

$(NAME)_SOURCES    := hmi.c

$(NAME)_COMPONENTS := cli connectivity.mqtt cjson netmgr framework.common
GLOBAL_DEFINES     += ALIOT_DEBUG IOTX_DEBUG USE_LPTHREAD

# for developerkit board
ifeq ($(CONFIG_SYSINFO_DEVICE_NAME),developerkit)
$(NAME)_SOURCES    += devkit_display/AliOS_Things_logo.c
$(NAME)_SOURCES    += devkit_display/chat_display.c
GLOBAL_DEFINES += LITTLEVGL_DISPLAY LITTLEVGL_DEVELOPERKIT
$(NAME)_COMPONENTS += framework.GUI.littlevGL sensor
GLOBAL_INCLUDES     += ../../framework/GUI/littlevGL/ devkit_display/
endif

ifeq ($(CONFIG_SYSINFO_DEVICE_NAME),ATSAME54-XPRO)
$(NAME)_SOURCES    += brd_same54_press_report.c
$(NAME)_DEFINES += ATSAME54_XPRO_BOARD
endif
