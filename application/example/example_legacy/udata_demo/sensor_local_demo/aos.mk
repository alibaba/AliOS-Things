NAME := sensor_local_demo

$(NAME)_MBINS_TYPE := app
$(NAME)_VERSION    := 1.0.0
$(NAME)_SUMMARY    := sensor hal demo

$(NAME)_SOURCES    := sensor_local_demo.c
$(NAME)_COMPONENTS := cli sensor

$(NAME)_INCLUDES += ./

