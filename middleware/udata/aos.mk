NAME := udata

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION := 1.0.0
$(NAME)_SUMMARY := Sensoring device processing framework
GLOBAL_DEFINES += AOS_UDATA

$(NAME)_COMPONENTS += osal_aos

$(NAME)_SOURCES += \
    udata_main.c \
    udata_interface.c \
    service_mgr/service_mgr.c \
    cali_data/calibrated_example_algo.c \
    abs_data_model/abs_data_model.c \
    service/service_process.c \
    udata_queue.c \
    udata_service_task.c \
    udata_parse.c

ifeq ($(AOS_CONFIG_DTC_ENABLE),y)
$(NAME)_SOURCES += service/service_data_to_cloud.c
GLOBAL_DEFINES  += DATA_TO_CLOUD
endif

ifeq ($(UDATA_CONFIG_CJSON_FORMAT),y)
$(NAME)_COMPONENTS += cjson
EXTRA_TARGET_MAKEFILES +=  $(SOURCE_ROOT)/middleware/udata/gen_cjson_data.mk
GLOBAL_DEFINES += UDATA_CJSON_SUPPORTED
endif

$(NAME)_INCLUDES += ./include

ifeq ($(COMPILER),)
$(NAME)_CFLAGS      += -Wall -Werror
else ifeq ($(COMPILER),gcc)
$(NAME)_CFLAGS      += -Wall -Werror
endif
