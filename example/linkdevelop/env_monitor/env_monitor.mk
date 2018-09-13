NAME := env_monitor

$(NAME)_SOURCES    := env_monitor.c

$(NAME)_COMPONENTS := cli sensor connectivity.mqtt cjson netmgr framework.common
GLOBAL_DEFINES     += ALIOT_DEBUG IOTX_DEBUG USE_LPTHREAD

# for developerkit board
ifeq ($(CONFIG_SYSINFO_DEVICE_NAME),developerkit)
$(NAME)_SOURCES    += devkit_display/AliOS_Things_logo.c
$(NAME)_SOURCES    += devkit_display/sensor_display.c
GLOBAL_DEFINES += LITTLEVGL_DISPLAY LITTLEVGL_DEVELOPERKIT
$(NAME)_COMPONENTS += framework.GUI.littlevGL
GLOBAL_INCLUDES     += ../../framework/GUI/littlevGL/ devkit_display/
GLOBAL_DEFINES += DEV_HUMI_TEMP_SUPPORT
endif

# for stm32l476rg-nucleo, lpc54102 msp432p4111launchpad board
ifeq ($(strip $(CONFIG_SYSINFO_DEVICE_NAME)),$(filter $(CONFIG_SYSINFO_DEVICE_NAME),476-nucleo lpc54102 msp432p4111launchpad f412-nucleo))
GLOBAL_DEFINES += AOS_SENSOR_HUMI_SENSIRION_HTS221 AOS_SENSOR_TEMP_SENSIRION_HTS221
GLOBAL_DEFINES += DEV_HUMI_TEMP_SUPPORT
endif



