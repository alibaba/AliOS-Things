NAME := wifidev

ifeq (y,$(AOS_COMP_WIFIDEV_IN_SERVICE))
$(NAME)_MBINS_TYPE := share
else
$(NAME)_MBINS_TYPE := kernel
endif

$(NAME)_VERSION    := 1.0.0
$(NAME)_SUMMARY    := Wi-Fi Device

$(NAME)_SOURCES += vfs_wifi_drv.c \
	               wifi.c

#$(NAME)_CFLAGS += -Werror

$(NAME)_COMPONENTS +=

$(NAME)_DEFINES +=

GLOBAL_INCLUDES += ./
