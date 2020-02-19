# component name
NAME := udata
# component information
$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION := 1.0.2
$(NAME)_SUMMARY := Sensoring device processing framework
# source files and the folder of internal include files
$(NAME)_INCLUDES += ./include
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

# the folder of API files
GLOBAL_INCLUDES += ../../../include/service/udata

# armcc & iar without -Wall and -Werror
ifeq ($(COMPILER),)
$(NAME)_CFLAGS      += -Wall -Werror
else ifeq ($(COMPILER),gcc)
$(NAME)_CFLAGS      += -Wall -Werror
endif

ifeq ($(UDATA_CONFIG_DTC_ENABLE),y)
$(NAME)_SOURCES += service/service_data_to_cloud.c
endif

ifeq ($(UDATA_CONFIG_CJSON_FORMAT),y)
EXTRA_TARGET_MAKEFILES +=  $(SOURCE_ROOT)/components/service/udata/gen_cjson_data.mk
endif

$(NAME)_COMPONENTS-$(UDATA_CONFIG_SENSOR) += sensor osal_aos
$(NAME)_COMPONENTS-$(UDATA_CONFIG_CJSON_FORMAT) += cjson
