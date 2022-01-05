COMPONENT_ADD_INCLUDEDIRS := esp_tts_chinese/include 


LIB_FILES := $(shell ls $(COMPONENT_PATH)/esp_tts_chinese/lib*.a) 

LIBS := $(patsubst lib%.a,-l%,$(LIB_FILES))

COMPONENT_ADD_LDFLAGS +=  -L$(COMPONENT_PATH)/esp_tts_chinese \
						  $(LIBS)

