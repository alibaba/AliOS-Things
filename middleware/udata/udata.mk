NAME := udata

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

ifneq ($(dtc),)
$(NAME)_SOURCES += service/service_data_to_cloud.c
GLOBAL_DEFINES  += DATA_TO_CLOUD
endif

ifeq ($(udata_cjson_support),1)
$(NAME)_PRE_PROCESS := @python build/scripts/cjson_format.py
endif

$(NAME)_INCLUDES := ../../device/sensor/include

GLOBAL_INCLUDES += . include \
                   ./include/aos

ifeq ($(COMPILER),)
$(NAME)_CFLAGS      += -Wall -Werror
else ifeq ($(COMPILER),gcc)
$(NAME)_CFLAGS      += -Wall -Werror
endif

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION := 0.0.1
$(NAME)_SUMMARY :=
GLOBAL_DEFINES += AOS_UDATA

