NAME := modbus_demo

$(NAME)_MBINS_TYPE := app
$(NAME)_VERSION := 1.0.0
$(NAME)_SUMMARY :=
$(NAME)_SOURCES := modbus_test.c
$(NAME)_SOURCES += modbus_demo.c

GLOBAL_DEFINES += AOS_NO_WIFI

$(NAME)_COMPONENTS := yloop cli kernel/bus/mbmaster

GLOBAL_INCLUDES += ./
