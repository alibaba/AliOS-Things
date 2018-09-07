NAME := ldapp

$(NAME)_SOURCES    := mqtt-example.c

$(NAME)_COMPONENTS := cli sensor connectivity.mqtt cjson fota netmgr framework.common
GLOBAL_DEFINES     += ALIOT_DEBUG IOTX_DEBUG USE_LPTHREAD

# for developerkit board
ifeq ($(CONFIG_SYSINFO_DEVICE_NAME),developerkit)
$(NAME)_SOURCES    += AliOS_Things_logo.c
$(NAME)_SOURCES    += sensor_display.c
GLOBAL_DEFINES += LITTLEVGL_DISPLAY
$(NAME)_COMPONENTS += framework.GUI.littlevGL
GLOBAL_INCLUDES     += ../framework/GUI/littlevGL/
GLOBAL_DEFINES += LITTLEVGL_DEVELOPERKIT
GLOBAL_DEFINES += DEV_BOARD_DEVELOPERKIT
endif

# for stm32l476rg-nucleo, lpc54102 msp432p4111launchpad board
ifeq ($(strip $(CONFIG_SYSINFO_DEVICE_NAME)),$(filter $(CONFIG_SYSINFO_DEVICE_NAME),476-nucleo lpc54102 msp432p4111launchpad f412-nucleo))
GLOBAL_DEFINES += AOS_SENSOR_HUMI_SENSIRION_HTS221 AOS_SENSOR_TEMP_SENSIRION_HTS221
GLOBAL_DEFINES += CLD_CMD_LED_REMOTE_CTRL_SUPPORT DEV_HUMI_TEMP_SUPPORT
endif



