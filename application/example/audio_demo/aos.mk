NAME := audio_demo

$(NAME)_MBINS_TYPE := app
$(NAME)_VERSION := 1.0.2
$(NAME)_SUMMARY := audio_demo

$(NAME)_INCLUDES += ./
$(NAME)_SOURCES :=   app_entry.c maintask.c audio_demo.c
$(NAME)_COMPONENTS += libiot_devmodel libiot_dynreg littlefs fatfs uvoice

$(NAME)_COMPONENTS += netmgr cjson network

ifeq ($(AOS_COMP_UND),y)
$(NAME)_COMPONENTS += und
endif

$(NAME)_COMPONENTS-$(ENABLE_AOS_OTA) += ota
$(NAME)_COMPONENTS-$(AOS_COMP_SDK_AWSS) += libiot_awss

ifeq ($(AUDIO_DEMO_CONFIG_CASE_SOLO),y)
$(NAME)_SOURCES += linkkit_example_solo.c
endif

ifeq ($(AUDIO_DEMO_CONFIG_PRINT_HEAP),y)
$(NAME)_DEFINES += CONFIG_PRINT_HEAP
endif

$(NAME)_COMPONENTS-$(!AOS_BOARD_ESP8266)  += cli
ifneq ($(HOST_MCU_FAMILY),mcu_esp8266)
GLOBAL_DEFINES += CLI_CONFIG_STACK_SIZE=3072
else
GLOBAL_DEFINES += ESP8266_CHIPSET
endif

ifeq ($(AWSS_SUPPORT_BLE_PROVISION),y)
ifneq ($(EN_COMBO_NET), y)
$(error need enable AWSS in breeze settings first)
endif

$(NAME)_SOURCES += combo/combo_net.c
$(NAME)_INCLUDES += combo
bz_en_auth := 1
bz_en_awss := 1
endif

# uVoice config
GLOBAL_DEFINES     += SOUND_APP
GLOBAL_DEFINES     += OSAL_RHINO
GLOBAL_DEFINES     += CLI_CONFIG_STACK_SIZE=8192 CONFIG_AOS_FATFS_SUPPORT_MMC
uvoice_project_build ?= 1
uvoice_build_type ?= debug
uvoice_tts_enable ?= 1
uvoice_player_enable ?= 1
uvoice_recorder_enable ?= 1
uvoice_file_enable ?= 1
uvoice_mlist_enable ?= 1
uvoice_http_enable ?= 1
uvoice_https_enable ?= 1
uvoice_event_enable ?= 1
dec_pvmp3_enable ?= 1
dec_pvaac_enable ?= 1

A7_DSP_ENABLE := 1
GLOBAL_DEFINES += HAAS_AUDIO_DEMO

