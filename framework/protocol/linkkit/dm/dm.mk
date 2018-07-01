
NAME := dm
$(NAME)_TYPE := framework
GLOBAL_INCLUDES += ./include ./include/interface
$(NAME)_INCLUDES += ../../protocol/cm/

$(NAME)_SOURCES     := src/dm_cm_impl.c src/dm_cm_msg_info.c src/dm_impl.c src/dm_thing.c src/dm_thing_manager.c src/dm_logger.c src/dm_slist.c

$(NAME)_COMPONENTS := framework.common 
GLOBAL_DEFINES += USING_UTILS_JSON LITE_THING_MODEL NDEBUG

FLAG_ESP8266_CHIPSET := $(findstring ESP8266_CHIPSET, $(AOS_SDK_DEFINES))
#FLAG_SUPPORT_SINGAPORE_DOMAIN := $(findstring SUPPORT_SINGAPORE_DOMAIN, $(AOS_SDK_DEFINES))

#ifneq ($(FLAG_ESP8266_CHIPSET),ESP8266_CHIPSET)
GLOBAL_DEFINES += LOCAL_CONN_ENABLE
#endif

#ifneq ($(FLAG_ESP8266_CHIPSET)_$(FLAG_SUPPORT_SINGAPORE_DOMAIN),ESP8266_CHIPSET_SUPPORT_SINGAPORE_DOMAIN)
#GLOBAL_DEFINES += LOCAL_CONN_ENABLE
#endif
