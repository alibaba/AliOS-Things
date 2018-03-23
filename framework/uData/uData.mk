NAME := uData

$(NAME)_SOURCES += \
    uData_main.c \
    uData_interface.c \
    service_mgr/service_mgr.c \
    cali_data/calibrated_example_algo.c \
    abs_data_model/abs_data_model.c \
    service_algo/udata_baro_service.c \
    service_algo/udata_gps_service.c


$(NAME)_INCLUDES := \
    ./include \
    ../../include/aos \
	../../include/hal \
	
GLOBAL_INCLUDES += .

$(NAME)_TYPE := framework

GLOBAL_DEFINES += AOS_UDATA

GLOBAL_DEFINES += AOS_UDATA_SERVICE_BARO
#GLOBAL_DEFINES += AOS_UDATA_SERVICE_ALS
#GLOBAL_DEFINES += AOS_UDATA_SERVICE_TEMP
#GLOBAL_DEFINES += AOS_UDATA_SERVICE_HUMI
#GLOBAL_DEFINES += UDATA_SERVICE_PROXIMITY
#GLOBAL_DEFINES += AOS_UDATA_SERVICE_GPS



