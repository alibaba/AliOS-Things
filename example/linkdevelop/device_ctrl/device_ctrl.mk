NAME := device_ctrl

$(NAME)_SOURCES    := device_ctrl.c

$(NAME)_COMPONENTS := cli connectivity.mqtt cjson netmgr framework.common
GLOBAL_DEFINES     += ALIOT_DEBUG IOTX_DEBUG USE_LPTHREAD

# for developerkit board
ifeq ($(CONFIG_SYSINFO_DEVICE_NAME),developerkit)
GLOBAL_DEFINES += CLD_CMD_LED_REMOTE_CTRL_SUPPORT
endif

# for stm32l476rg-nucleo, stm32f412-nucleo, NXP lpc54102, TI msp432p4111launchpad
ifeq ($(strip $(CONFIG_SYSINFO_DEVICE_NAME)),$(filter $(CONFIG_SYSINFO_DEVICE_NAME),476-nucleo lpc54102 msp432p4111launchpad f412-nucleo))
GLOBAL_DEFINES += CLD_CMD_LED_REMOTE_CTRL_SUPPORT
endif



