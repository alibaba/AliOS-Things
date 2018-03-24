NAME := gps

$(NAME)_TYPE := kernel
$(NAME)_COMPONENTS += sal atparser

$(NAME)_SOURCES += hal/gps_hal.c \
                   parse/gps_parse.c
				   
				   
ifeq (gps.sim868,$(module))
$(NAME)_COMPONENTS += sal.gprs.sim800
GLOBAL_DEFINES     += AOS_GPS_SIM868

$(NAME)_SOURCES += drv/drv_sim868/gps_drv_simcom_sim868.c
$(NAME)_SOURCES += drv/drv_sim868/gps_parse_simcom_sim868.c
endif

GLOBAL_DEFINES     += AOS_GPS
$(NAME)_CFLAGS     += -Wall -Werror

$(NAME)_INCLUDES := ../../utility/iotx-utils/sdk-impl ../../framework/atparser
$(NAME)_INCLUDES += ./hal ./parse
GLOBAL_INCLUDES  += .

