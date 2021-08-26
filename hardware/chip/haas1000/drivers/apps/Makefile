ifneq ($(GEN_SDK_LIB),1)
obj-y := audioplayers/ common/ main/ key/ pwl/ battery/ factory/ cmd/ mic/

ifeq ($(HAAS_OTA_ENABLED),1)
obj-y += ota/
endif

ifeq ($(MASTER_APP_SUPPORT),1)
obj-y += master/
endif

ifeq ($(APP_TEST_AUDIO),1)
obj-y += apptester/
endif

ifeq ($(BTUSB_AUDIO_MODE),1)
obj-y += usbaudio/
endif
ifeq ($(BT_USB_AUDIO_DUAL_MODE),1)
obj-y += btusbaudio/
obj-y += usbaudio/
endif

ifeq ($(APP_TEST_SDMMC),1)
obj-y += sdmmc/
endif

ifeq ($(ANC_APP),1)
obj-y += anc/
endif

ifeq ($(VOICE_DETECTOR_EN),1)
obj-y += voice_detector/
endif

ifeq ($(ALIOS_APP_SUPPORT),1)
obj-y += alios/
endif

ifeq ($(AIS_APP_SUPPORT),1)
obj-y += ais/
endif

ifeq ($(AIA_APP_SUPPORT),1)
obj-y += aia/
endif

ifeq ($(MASTER_APP_SUPPORT),1)
obj-y += master/
endif

else

cur_dir := $(dir $(lastword $(MAKEFILE_LIST)))

LIB_NAME := libapps
obj-y :=
ifneq ($(SDK),1)

src_obj := audioplayers/ common/ main/ key/ pwl/ battery/ factory/ cmd/ mic/

ifeq ($(APP_TEST_AUDIO),1)
src_obj += apptester/
endif

ifeq ($(BTUSB_AUDIO_MODE),1)
src_obj += usbaudio/
endif

ifeq ($(APP_TEST_SDMMC),1)
src_obj += sdmmc/
endif

ifeq ($(ANC_APP),1)
src_obj += anc/
endif

ifeq ($(VOICE_DETECTOR_EN),1)
src_obj += voice_detector/
endif

ifeq ($(BT_USB_AUDIO_DUAL_MODE),1)
src_obj += btusbaudio/
endif

ifeq ($(ALIOS_APP_SUPPORT),1)
#src_obj += alios/
endif

ifeq ($(AIS_APP_SUPPORT),1)
#src_obj += ais/
endif

ifeq ($(AIA_APP_SUPPORT),1)
#src_obj += aia/
endif

$(LIB_NAME)-y := $(src_obj)
endif

ifeq ($(ALIOS_APP_SUPPORT),1)
obj-y += alios/
endif

ifeq ($(AIS_APP_SUPPORT),1)
obj-y += ais/
endif

ifeq ($(AIA_APP_SUPPORT),1)
obj-y += aia/
endif

ifeq ($(MASTER_APP_SUPPORT),1)
obj-y += master/
endif

obj-y += $(LIB_NAME).a

endif

subdir-ccflags-y += -Iapps/apptester \
					-Iapps/audioplayers \
					-Iapps/common \
					-Iapps/sdmmc \
					-Iapps/main \
					-Iapps/cmd \
					-Iapps/key \
					-Iapps/pwl \
					-Iapps/ota \
					-Iapps/master \
					-Iapps/battery \
					-Iservices/voicepath \
					-Iservices/ble_app/app_datapath \
					-Iutils/list \
					-Iutils/heap \
					-Iservices/cp_server \
					-Iservices/multimedia/audio/process/filters/include
ifeq ($(BT_USB_AUDIO_DUAL_MODE),1)
subdir-ccflags-y += -Iapps/btusbaudio
endif

ifeq ($(A2DP_LDAC_ON),1)
subdir-ccflags-y += -Ithirdparty/audio_codec_lib/ldac/inc
endif

subdir-ccflags-y += -Iplatform/hal -Iplatform/cmsis -Iplatform/cmsis/inc -Inet/include/  -Inet/include/byteorder  -Inet/include/os  -Inet/include/unaligned
subdir-ccflags-y += -Inet -Inet/net80211 -Inet/core -Inet/lib -Inet/net_os -Iutils/hwtimer_list -Iplatform/drivers/ana
subdir-ccflags-y += -Inet/lwip/src/include/
subdir-ccflags-y += -Iservices/wifi_app/wifi_voice
ifeq ($(SUPPORT_IR),1)
subdir-ccflags-y += -Iplatform/hal/ir/inc
endif
