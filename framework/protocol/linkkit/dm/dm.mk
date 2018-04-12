
NAME := dm
$(NAME)_TYPE := framework
GLOBAL_INCLUDES += ./include ./include/interface
$(NAME)_INCLUDES += ../../protocol/alink-ilop/sdk-encap  ../../protocol/alink-ilop/iotkit-system ../../protocol/cm/

$(NAME)_SOURCES     := src/dm_cm_impl.c src/dm_cm_msg_info.c src/dm_impl.c src/dm_thing.c src/dm_thing_manager.c src/dm_logger.c src/dm_slist.c

#defalut gcc
ifeq ($(COMPILER),)
$(NAME)_CFLAGS  += -Wall -Werror -Wno-unused-variable -Wno-unused-parameter -Wno-implicit-function-declaration
$(NAME)_CFLAGS  += -Wno-type-limits -Wno-sign-compare -Wno-pointer-sign -Wno-uninitialized
$(NAME)_CFLAGS  += -Wno-return-type -Wno-unused-function -Wno-unused-but-set-variable
$(NAME)_CFLAGS  += -Wno-unused-value -Wno-strict-aliasing
else ifeq ($(COMPILER),gcc)
$(NAME)_CFLAGS  += -Wall -Werror -Wno-unused-variable -Wno-unused-parameter -Wno-implicit-function-declaration
$(NAME)_CFLAGS  += -Wno-type-limits -Wno-sign-compare -Wno-pointer-sign -Wno-uninitialized
$(NAME)_CFLAGS  += -Wno-return-type -Wno-unused-function -Wno-unused-but-set-variable
$(NAME)_CFLAGS  += -Wno-unused-value -Wno-strict-aliasing
endif

$(NAME)_COMPONENTS := framework.common 
GLOBAL_DEFINES += USING_UTILS_JSON LITE_THING_MODEL NDEBUG LOCAL_CONN_ENABLE
