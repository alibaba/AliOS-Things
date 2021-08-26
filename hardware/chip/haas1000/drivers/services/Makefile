ifneq ($(REL_SDK),1)
ifneq ($(GEN_SDK_LIB),1)
LIB_NAME := libservices
obj-y :=
ifneq ($(SDK),1)
src_obj := wifi_app/
$(LIB_NAME)-y := $(src_obj)
endif
obj-y += $(LIB_NAME).a

ifeq ($(BT_CLOSE),1)
obj-y += audio_dump/ audioflinger/ audio_process/ hw_dsp/ resources/ \
			../utils/crc32/ ../utils/heap/ norflash_api/ overlay/
else
obj-y += audio_dump/ \
         audioflinger/ \
         audio_process/ \
         hw_dsp/ \
         bt_app/ \
         overlay/ \
         resources/ \
         ../utils/crc32/ \
         ../utils/heap/ \
         osif/ \
         norflash_api/ \
         app_ai/
endif
ifeq ($(BONE_SENSOR_TDM),1)
obj-y += bone_sensor/
endif

ifeq ($(USE_BT_ADAPTER),1)
obj-y += bt_service/
endif

ifneq ($(INTERCONNECTION)_$(AI_VOICE),0_0)
obj-y += interconnection/
endif
ifeq ($(BT_CLOSE),0)
ifeq ($(ENHANCED_STACK),1)
obj-y += bt_profiles_enhanced/
obj-y += bt_if_enhanced/
else
obj-y += bt_profiles/
obj-y += bt_if/
endif
endif
ifeq ($(MBED),1)
obj-y += fs/
endif

ifeq ($(SERVICE_ONE_LIB),1)
obj-y += multimedia/
endif

ifneq ($(MBED)_$(AI_VOICE),0_0)
obj-y +=../utils/kfifo/
endif

ifeq ($(UTILS_INTERSYSHCI),1)
obj-y +=../utils/intersyshci/
endif
ifeq ($(UTILS_BES_KV),1)
obj-y +=../utils/bes_kv/
endif

ifeq ($(VOICE_DATAPATH_ENABLED),1)
obj-y += voicepath/
obj-y += voicepath/$(VOICE_DATAPATH_TYPE)/
endif
ifeq ($(BT_CLOSE),0)
ifeq ($(BLE),1)
obj-y += ble_app/ \
         bridge/ \
         ble_stack/ \
         ../utils/retention_ram/ \
         ble_profiles/
endif
endif

ifeq ($(AUTO_TEST),1)
obj-y += auto_test/
endif

obj-y += nv_section/

obj-y += sys_time/

ifneq ($(FPGA),1)
#ifneq ($(WIFI_APP_TEST),1)
obj-y += nvrecord/
#endif
endif

ifeq ($(VOICE_RECOGNITION),1)
obj-y += voice_recognition/
endif

ifeq ($(APP_ANC_TEST),1)
obj-y += anc_spp_tool/
endif
ifeq ($(TEST_OVER_THE_AIR),1)
obj-y += tota/
endif

ifeq ($(BES_OTA_BASIC),1)
ifeq ($(IBRT_OTA),1)
ifeq ($(FPGA_IBRT_OTA),1)
obj-y += fpga_ibrt_ota/
else
obj-y += ibrt_ota/
endif
else
obj-y += ota/
endif
endif

ifeq ($(TILE_DATAPATH_ENABLED),1)
obj-y += ../thirdparty/tile/
endif
ifeq ($(AI_VOICE),1)
obj-y += ai_voice/ \
         ../utils/crc16/
endif

ifeq ($(CHIP_HAS_CP),1)
obj-y += cp_accel/
ifeq ($(CP_ENABLE), 1)
obj-y += cp_server/
endif
endif

ifeq ($(IBRT),1)
obj-y += ibrt_core/
obj-y += app_ibrt/
obj-y += ibrt_ui/
endif

ifeq ($(TWS_SYSTEM_ENABLED),1)
obj-y += app_tws/
endif

ifeq ($(RSA_SHA),1)
obj-y +=../utils/sha256/ ../utils/rsa2048/
endif

ifeq ($(AES256_FOR_BIN),1)
obj-y +=../utils/aes256/
endif

obj-y += communication/

ifeq ($(A7_DSP_ENABLE),1)
obj-y += a7_dsp/
endif

ifeq ($(USE_BT_ADAPTER),1)
obj-y += bt_service/
endif

obj-y += transq_msg/

else


cur_dir := $(dir $(lastword $(MAKEFILE_LIST)))

LIB_NAME := libservices
obj-y :=
ifneq ($(SDK),1)
ifeq ($(BT_CLOSE),1)
src_obj := audio_dump/ audioflinger/ audio_process/ hw_dsp/ resources/ \
			../utils/crc32/ ../utils/heap/ norflash_api/ overlay/
else
src_obj := audio_dump/ \
         audioflinger/ \
         audio_process/ \
         hw_dsp/ \
         bt_app/ \
         overlay/ \
         resources/ \
         ../utils/crc32/ \
         ../utils/heap/ \
         osif/ \
         norflash_api/ \
         app_ai/
endif
ifeq ($(BONE_SENSOR_TDM),1)
src_obj += bone_sensor/
endif

ifneq ($(INTERCONNECTION)_$(AI_VOICE),0_0)
src_obj += interconnection/
endif

ifeq ($(BT_CLOSE),0)
ifeq ($(ENHANCED_STACK),1)
src_obj += bt_profiles_enhanced/
src_obj += bt_if_enhanced/
else
src_obj += bt_profiles/
src_obj += bt_if/
endif
endif

ifeq ($(MBED),1)
src_obj += fs/
endif

ifeq ($(SERVICE_ONE_LIB),1)
src_obj += multimedia/
endif

ifneq ($(MBED)_$(AI_VOICE),0_0)
src_obj +=../utils/kfifo/
endif

ifeq ($(UTILS_INTERSYSHCI),1)
src_obj +=../utils/intersyshci/
endif
ifeq ($(UTILS_BES_KV),1)
src_obj +=../utils/bes_kv/
endif

ifeq ($(VOICE_DATAPATH_ENABLED),1)
src_obj += voicepath/
src_obj += voicepath/$(VOICE_DATAPATH_TYPE)/
endif

ifeq ($(BT_CLOSE),0)
ifeq ($(BLE),1)
src_obj += ble_app/ \
         bridge/ \
         ble_stack/ \
         ../utils/retention_ram/ \
         ble_profiles/
endif
endif

ifeq ($(AUTO_TEST),1)
src_obj += auto_test/
endif

src_obj += nv_section/

src_obj += sys_time/

ifneq ($(FPGA),1)
#ifneq ($(WIFI_APP_TEST),1)
src_obj += nvrecord/
#endif
endif

ifeq ($(VOICE_RECOGNITION),1)
src_obj+= voice_recognition/
endif

ifeq ($(TEST_OVER_THE_AIR),1)
src_obj += tota/
endif

ifeq ($(BES_OTA_BASIC),1)
src_obj += ota/
endif

ifeq ($(AI_VOICE),1)
src_obj += ai_voice/
endif

ifeq ($(CHIP_HAS_CP),1)
src_obj += cp_accel/
ifeq ($(CP_ENABLE), 1)
src_obj += cp_server/
endif
endif

ifeq ($(IBRT),1)
src_obj += tws_ibrt/
src_obj += app_ibrt/
endif

ifeq ($(TWS_SYSTEM_ENABLED),1)
src_obj += app_tws/
endif

ifeq ($(RSA_SHA),1)
src_obj +=../utils/sha256/ ../utils/rsa2048/
endif

ifeq ($(AES256_FOR_BIN),1)
src_obj +=../utils/aes256/
endif

src_obj += communication/
src_obj += wifi_app/

ifeq ($(A7_DSP_ENABLE),1)
ifeq ($(A7_INTO_LIB),1)
src_obj += a7_dsp/
endif
endif

ifeq ($(USE_BT_ADAPTER),1)
src_obj += bt_service/
endif

src_obj += transq_msg/

$(LIB_NAME)-y := $(src_obj)
endif
obj-y :=

ifeq ($(A7_DSP_ENABLE),1)
ifneq ($(A7_INTO_LIB),1)
obj-y += a7_dsp/
endif
endif

obj-y += $(LIB_NAME).a
endif

subdir-ccflags-y += \
	-Inet

ifeq ($(RPC_SUPPORT),1)
obj-y += rpc/
endif
else
cur_dir := $(dir $(lastword $(MAKEFILE_LIST)))

obj-y += libservices.a
endif