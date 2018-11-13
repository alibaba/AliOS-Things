NAME := i2c_hts221_test

$(NAME)_MBINS_TYPE := app
$(NAME)_VERSION := 0.0.1
$(NAME)_SUMMARY :=
$(NAME)_SOURCES := i2c_hts221_test.c

GLOBAL_DEFINES += AOS_NO_WIFI
GLOBAL_DEFINES += AOS_SENSOR_HUMI_SENSIRION_HTS221
GLOBAL_DEFINES += UDATA_YLOOP

$(NAME)_COMPONENTS := yloop cli  netmgr middleware.common device.sensor middleware.udata utility.cjson

ifeq ($(BENCHMARKS),1)
$(NAME)_COMPONENTS  += benchmarks
GLOBAL_DEFINES      += CONFIG_CMD_BENCHMARKS
endif

GLOBAL_INCLUDES += ./