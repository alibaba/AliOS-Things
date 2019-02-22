NAME := ulocation

$(NAME)_TYPE := framework
$(NAME)_VERSION := 1.0.0
$(NAME)_SUMMARY := locating framework

$(NAME)_SOURCES += \
	uloc_north.c \
	uloc_main.c  \
	uloc_south.c \
	hal/uloc_hal_gprs.c \
	hal/uloc_hal_wifi.c \
	hal/uloc_hal_gps.c
	
$(NAME)_COMPONENTS :=
	
GLOBAL_INCLUDES  += . include 
GLOBAL_INCLUDES  += ./hal

GLOBAL_DEFINES  += AOS_ULOCATION


