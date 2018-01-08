NAME := uDataapp

$(NAME)_SOURCES := uData_sample.c

$(NAME)_COMPONENTS := cli netmgr framework.common device.sensor uData

$(NAME)_INCLUDES := \
    ./include \
    ../../include/aos \
	../../include/hal \
	
GLOBAL_INCLUDES += .


