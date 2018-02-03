NAME := ble_app_framework

ifneq ($(no_ble_app_framework),1)
$(NAME)_SOURCES := ble_app_framework.c
endif

GLOBAL_INCLUDES += .
