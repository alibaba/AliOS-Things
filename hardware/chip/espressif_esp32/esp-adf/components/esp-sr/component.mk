COMPONENT_ADD_INCLUDEDIRS := lib/include \
                             wake_word_engine/include \
                             speech_command_recognition/include \
                             acoustic_algorithm/include \
                             esp-tts/esp_tts_chinese/include \

COMPONENT_SRCDIRS := speech_command_recognition


LIB_FILES := $(shell ls $(COMPONENT_PATH)/wake_word_engine/lib*.a) \
             $(shell ls $(COMPONENT_PATH)/lib/lib*.a) \
             $(shell ls $(COMPONENT_PATH)/acoustic_algorithm/lib*.a) \
             $(shell ls $(COMPONENT_PATH)/speech_command_recognition/lib*.a) \
             $(shell ls $(COMPONENT_PATH)/esp-tts/esp_tts_chinese/lib*.a) \

LIBS := $(patsubst lib%.a,-l%,$(LIB_FILES))

COMPONENT_ADD_LDFLAGS +=  -L$(COMPONENT_PATH)/lib \
                          -L$(COMPONENT_PATH)/wake_word_engine \
                          -L$(COMPONENT_PATH)/speech_command_recognition \
                          -L$(COMPONENT_PATH)/acoustic_algorithm \
                          -L$(COMPONENT_PATH)/esp-tts \
                          $(LIBS)

