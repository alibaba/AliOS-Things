NAME := u_drivers_ut

$(NAME)_MBINS_TYPE := share
$(NAME)_VERSION    := 1.0.0
$(NAME)_SUMMARY    := Drivers unit test program

$(NAME)_SOURCES := drv_unit_test.c

#$(NAME)_CFLAGS += -Werror

$(NAME)_COMPONENTS +=

$(NAME)_DEFINES +=

GLOBAL_INCLUDES += include include/devicevfs
