NAME := udata

$(NAME)_SOURCES += \
    udata_main.c \
    udata_interface.c \
    service_mgr/service_mgr.c \
    cali_data/calibrated_example_algo.c \
    abs_data_model/abs_data_model.c \
    service/service_process.c \
    udata_queue.c \
    udata_service_task.c

ifeq ($(dtc),1)
$(NAME)_SOURCES += service/service_data_to_cloud.c
endif


$(NAME)_INCLUDES := \
    ./include \
    ../../include/aos \
    ../../kernel/hal/include \
    ../../device/sensor/include

GLOBAL_INCLUDES += . include

ifeq ($(COMPILER),)
$(NAME)_CFLAGS      += -Wall -Werror
else ifeq ($(COMPILER),gcc)
$(NAME)_CFLAGS      += -Wall -Werror
endif

$(NAME)_TYPE := framework
#GLOBAL_DEFINES += UDATA_YLOOP
GLOBAL_DEFINES += AOS_UDATA

GLOBAL_DEFINES += AOS_UDATA_SERVICE_ACC
GLOBAL_DEFINES += AOS_UDATA_SERVICE_MAG
GLOBAL_DEFINES += AOS_UDATA_SERVICE_GYRO

GLOBAL_DEFINES += AOS_UDATA_SERVICE_ALS
GLOBAL_DEFINES += AOS_UDATA_SERVICE_PS
GLOBAL_DEFINES += AOS_UDATA_SERVICE_BARO
GLOBAL_DEFINES += AOS_UDATA_SERVICE_TEMP
GLOBAL_DEFINES += AOS_UDATA_SERVICE_UV
GLOBAL_DEFINES += AOS_UDATA_SERVICE_HUMI
GLOBAL_DEFINES += AOS_UDATA_SERVICE_NOISE
GLOBAL_DEFINES += AOS_UDATA_SERVICE_PM25
GLOBAL_DEFINES += AOS_UDATA_SERVICE_CO2
GLOBAL_DEFINES += AOS_UDATA_SERVICE_HCHO
GLOBAL_DEFINES += AOS_UDATA_SERVICE_TVOC

GLOBAL_DEFINES += AOS_UDATA_SERVICE_HALL
GLOBAL_DEFINES += AOS_UDATA_SERVICE_HR

GLOBAL_DEFINES += AOS_UDATA_SERVICE_GPS

GLOBAL_DEFINES += AOS_UDATA_SERVICE_RGB
GLOBAL_DEFINES += AOS_UDATA_SERVICE_GS
GLOBAL_DEFINES += AOS_UDATA_SERVICE_IR
GLOBAL_DEFINES += AOS_UDATA_SERVICE_RTC

