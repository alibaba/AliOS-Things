
NAME := dm
$(NAME)_TYPE := framework
GLOBAL_INCLUDES += ./include ./include/interface
$(NAME)_INCLUDES += ../../protocol/cm/

$(NAME)_SOURCES     := src/dm_cm_impl.c src/dm_cm_msg_info.c src/dm_impl.c src/dm_thing.c src/dm_thing_manager.c src/dm_logger.c src/dm_slist.c

$(NAME)_COMPONENTS := framework.common 
GLOBAL_DEFINES += USING_UTILS_JSON LITE_THING_MODEL NDEBUG LOCAL_CONN_ENABLE
