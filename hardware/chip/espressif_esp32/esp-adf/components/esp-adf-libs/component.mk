
# "main" pseudo-component makefile.

# (Uses default behaviour of compiling all source files in directory, adding 'include' to include path.)

COMPONENT_ADD_INCLUDEDIRS :=    esp_audio/include \
                                esp_codec/include/codec \
                                esp_codec/include/processing \
                                recorder_engine/include \
                                esp_ssdp/include \
                                esp_upnp/include \
                                esp_sip/include  \
                                audio_misc/include

COMPONENT_SRCDIRS := . esp_codec audio_misc

LIBS := esp_processing esp_audio esp_codec esp_upnp 

ifdef CONFIG_REC_ENG_ENABLE_VAD_ONLY
 LIBS += recorder_engine_vad
endif

ifdef CONFIG_REC_ENG_ENABLE_VAD_WWE
 LIBS += recorder_engine_vad_wwe
endif

ifdef CONFIG_REC_ENG_ENABLE_VAD_WWE_AMR
 LIBS += recorder_engine
endif

COMPONENT_ADD_LDFLAGS +=  -L$(COMPONENT_PATH)/esp_audio/lib/esp32 \
                          -L$(COMPONENT_PATH)/esp_codec/lib/esp32 \
                          -L$(COMPONENT_PATH)/recorder_engine/lib/esp32 \
                          -L$(COMPONENT_PATH)/esp_ssdp/lib/esp32 \
                          -L$(COMPONENT_PATH)/esp_upnp/lib/esp32 \
                           $(addprefix -l,$(LIBS)) \

ifdef IDF_VERSION_MAJOR
ifeq ($(IDF_VERSION_MAJOR),4)
COMPONENT_ADD_LDFLAGS += -L$(COMPONENT_PATH)/esp_sip/lib/esp32 -lesp_sip
COMPONENT_ADD_LDFLAGS += -L$(COMPONENT_PATH)/esp_ssdp/lib/esp32 -lesp_ssdp
else
COMPONENT_ADD_LDFLAGS += -L$(COMPONENT_PATH)/esp_sip/lib/esp32 -lesp_sip-v33
COMPONENT_ADD_LDFLAGS += -L$(COMPONENT_PATH)/esp_ssdp/lib/esp32 -lesp_ssdp-v33
endif
else
COMPONENT_ADD_LDFLAGS += -L$(COMPONENT_PATH)/esp_sip/lib/esp32 -lesp_sip-v33
COMPONENT_ADD_LDFLAGS += -L$(COMPONENT_PATH)/esp_ssdp/lib/esp32 -lesp_ssdp-v33
endif

ALL_LIB_FILES += $(patsubst %,$(COMPONENT_PATH)/%/lib/esp32/lib%.a,$(LIBS))
