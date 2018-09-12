NAME := breeze_otaapp

$(NAME)_SOURCES := breeze_otaapp.c

GLOBAL_DEFINES += BLE_APP_RECONFIG_AISILOP

ble = 1

$(NAME)_COMPONENTS := yloop bluetooth.breeze cli

$(NAME)_COMPONENTS += middleware/uagent/uota_ble

GLOBAL_DEFINES += DEBUG
GLOBAL_DEFINES += CONFIG_BLE_LINK_PARAMETERS
#GLOBAL_DEFINES += CONFIG_BT_SMP

GLOBAL_DEFINES += BUILD_AOS

ifeq ($(continue_ble_adv),1)
GLOBAL_DEFINES += CONTINUE_BEL_ADV
endif

ifeq ($(append_user_adv),1)
GLOBAL_DEFINES += APPEND_USER_ADV
endif

GLOBAL_INCLUDES += ../
