#NAME := iot-utils

include framework/protocol/alink-ilop/base/utils/LITE-utils/LITE-utils.mk

UTIL_SOURCE_DIR = base/utils/

$(NAME)_SOURCES  += $(UTIL_SOURCE_DIR)/misc/utils_epoch_time.c
$(NAME)_SOURCES  += $(UTIL_SOURCE_DIR)/misc/utils_httpc.c
$(NAME)_SOURCES  += $(UTIL_SOURCE_DIR)/misc/utils_list.c
$(NAME)_SOURCES  += $(UTIL_SOURCE_DIR)/misc/utils_net.c
$(NAME)_SOURCES  += $(UTIL_SOURCE_DIR)/misc/utils_timer.c
$(NAME)_SOURCES  += $(UTIL_SOURCE_DIR)/misc/work_queue.c

$(NAME)_SOURCES  += $(UTIL_SOURCE_DIR)/digest/utils_base64.c
$(NAME)_SOURCES  += $(UTIL_SOURCE_DIR)/digest/utils_hmac.c
$(NAME)_SOURCES  += $(UTIL_SOURCE_DIR)/digest/utils_md5.c
$(NAME)_SOURCES  += $(UTIL_SOURCE_DIR)/digest/utils_sha1.c

ifeq ($(COMPILER),gcc)
$(NAME)_CFLAGS += \
    -Wno-unused-function \
    -Wno-implicit-function-declaration \
    -Wno-unused-function \
#    -Werror
endif

$(NAME)_CFLAGS += \
    -DLITE_UTILS_CONFIG_H=\"ilop_utils_config.h\" \
    -DUTILS_printf=HAL_Printf \
  
  
  
GLOBAL_INCLUDES    += \
                    base/utils/misc/ \
                    base/utils/digest
                      
                     
$(NAME)_INCLUDES  += \
                    ./ilop_utils_config.h \
                     ./exports \
                    ../../sdk-encap \
                    ../log  
