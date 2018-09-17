NAME := modbus_test

$(NAME)_SOURCES := modbus_test.c
$(NAME)_SOURCES += modbus_demo.c

GLOBAL_DEFINES += AOS_NO_WIFI

$(NAME)_COMPONENTS := yloop cli kernel/rhino/bus/mbmaster

GLOBAL_DEFINES += RHINO_CONFIG_MODBUS=1

ifeq ($(BENCHMARKS),1)
$(NAME)_COMPONENTS  += benchmarks
GLOBAL_DEFINES      += CONFIG_CMD_BENCHMARKS
endif

GLOBAL_INCLUDES += ./