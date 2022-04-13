NAME := drivers

$(NAME)_MBINS_TYPE := share
$(NAME)_VERSION    := 1.0.0
$(NAME)_SUMMARY    := Drivers 

$(NAME)_SOURCES := test/drv_unit_test.c

#$(NAME)_CFLAGS += -Werror

$(NAME)_COMPONENTS +=

$(NAME)_DEFINES +=

GLOBAL_INCLUDES += include include/devicevfs
