cur_dir := $(dir $(lastword $(MAKEFILE_LIST)))

obj-y := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)*.c $(cur_dir)*.cpp $(cur_dir)*.S))

ifneq ($(wildcard $(cur_dir)../../config/$(T)/app_status_ind.c),)
obj-y += ../../config/$(T)/app_status_ind.c
else
obj-y += ../../config/$(DEFAULT_CFG_SRC)/app_status_ind.c
endif

obj-y := $(obj-y:.c=.o)
obj-y := $(obj-y:.cpp=.o)
obj-y := $(obj-y:.S=.o)

ccflags-y +=  \
    $(BT_IF_INCLUDES) \
    -Iservices/bt_app \
    -Iservices/audioflinger \
    -Iservices/norflash_api \
    -Iservices/nvrecord \
    -Iservices/nv_section/factory_section \
    -Iservices/nv_section/log_section \
    -Iservices/nv_section/customparam_section \
    -Iservices/overlay \
    -Iservices/resources \
    -Iservices/audio_process \
    -Iapps/apptester \
    -Iapps/factory \
    -Iutils/crc32 \
    -Iplatform/drivers/bt \
    -Iplatform/drivers/ana \
    -Iapps/audioplayers/rbplay \
    -Itests/anc_usb \
    -Iapps/anc/inc \
    -Iapps/ota \
    -Ithirdparty/userapi \
    -Iservices/voicepath \
    -Iservices/wifi_app \
    -Iservices/wifi_app/alsa_adapter \
    -Iservices/ble_app \
    -Iservices/ble_app/app_main \
    -Iservices/ble_app/app_gfps \
    -Iservices/voicepath/gsound/gsound_service \
    -Iservices/voicepath/gsound/gsound_target \
    -Iservices/voicepath/gsound/gsound_custom/inc \
    -Iservices/voicepath/gsound/gsound_target_api_read_only \
    -Iservices/communication \
    -Iutils/cqueue \
    -Iservices/ai_voice/audio \
    -Iservices/ai_voice/manager \
    -Iservices/ai_voice/transport \
    -Iservices/app_ai/inc \
    -Iservices/interconnection/red \
    -Iservices/interconnection/green \
    -Iservices/ble_stack/ble_ip \
    -Iservices/ble_stack/hl/api \
    -Iservices/ble_stack/hl/inc/ \
    -Iservices/ble_stack/ke/api \
    -Iservices/bridge/ \
    $(BT_PROFILES_INCLUDES) \
    -Iservices/ble_stack/common/api/ \
    -Iservices/ble_stack/hl/src/gap/ \
    -Iservices/ble_stack/hl/src/gap/gapc/ \
    -Iservices/ble_stack/hl/src/gap/gapm/ \
    -Iservices/ble_stack/hl/src/gap/smpc/ \
    -Iservices/ble_stack/ke/src/ \
    -Iservices/ble_stack/hl/src/gatt/attc/ \
    -Iservices/ble_stack/hl/src/gatt/attm/ \
    -Iservices/ble_stack/hl/src/gatt/atts/ \
    -Iservices/ble_stack/hl/src/gatt/gattc/ \
    -Iservices/ble_stack/hl/src/gatt/gattm/ \
    -Iservices/ble_stack/hl/src/gatt/ \
    -Iservices/ble_stack/hl/src/l2c/l2cc/ \
    -Iservices/ble_stack/hl/src/l2c/l2cm/ \
    -Iservices/ble_stack/hci/api/ \
    -Iservices/ble_stack/hci/src/ \
    -Iservices/ble_stack/app/api/ \
    -Iservices/ble_stack/app/src/ \
    -Iservices/multimedia/audio/codec/sbc/inc \
    -Iservices/multimedia/audio/codec/sbc/src/inc \
    -Iservices/bt_app/a2dp_codecs/include \
    -Ithirdparty/tile \
    -Iservices/app_tws/inc \
    -Ithirdparty/tile/tile_target

ifeq ($(SUPPORT_IR),1)
ccflags-y += -Iplatform/hal/ir/inc -Iplatform/drivers/ir/inc
endif

ifeq ($(VOICE_DATAPATH_ENABLED),1)
ccflags-y += -Iservices/voicepath/$(VOICE_DATAPATH_TYPE)
endif

ifeq ($(IBRT),1)
ccflags-y += -Iservices/ibrt_core/inc
ccflags-y += -Iservices/ibrt_ui/inc
ccflags-y += -Iservices/app_ibrt/inc
endif

ifeq ($(APP_TEST_AUDIO),1)
CFLAGS_apps_tester.o += -DAPP_TEST_AUDIO
endif

ifeq ($(BES_OTA_BASIC),1)
ccflags-y += -Iservices/ota
endif

ifeq ($(BES_OTA_BASIC),1)
ifeq ($(IBRT_OTA),1)
ccflags-y += -Iservices/ibrt_ota
else
ccflags-y += -Iservices/ota
endif
endif

CFLAGS_apps.o += $(LDS_SECTION_FLAGS)

ifeq ($(ANC_APP),1)
CFLAGS_apps_tester.o += -DANC_APP
CFLAGS_apps.o += -DANC_APP
endif

ifeq ($(RB_CODEC),1)
CFLAGS_apps.o += -DRB_CODEC
endif

ifeq ($(VOICE_PROMPT),1)
CFLAGS_apps.o += -DMEDIA_PLAYER_SUPPORT
endif

ifeq ($(ENGINEER_MODE),1)
CFLAGS_apps.o += -D__ENGINEER_MODE_SUPPORT__
endif

ifeq ($(MCU_HIGH_PERFORMANCE_MODE),1)
CFLAGS_apps.o += -DMCU_HIGH_PERFORMANCE_MODE
endif

ifeq ($(BTADDR_FOR_DEBUG),1)
CFLAGS_apps.o += -DBTADDR_FOR_DEBUG
endif

ifeq ($(FORCE_SIGNALINGMODE),1)
CFLAGS_apps.o += -DFORCE_SIGNALINGMODE
endif

ifeq ($(FORCE_NOSIGNALINGMODE),1)
CFLAGS_apps.o += -DFORCE_NOSIGNALINGMODE
endif

ifeq ($(POWERKEY_I2C_SWITCH),1)
CFLAGS_apps.o += -DPOWERKEY_I2C_SWITCH
endif

