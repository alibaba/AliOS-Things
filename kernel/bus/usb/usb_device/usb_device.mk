NAME := usb_device

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION    := 0.0.1
$(NAME)_SUMMARY    := USB Device Stack

$(NAME)_SOURCES += core/src/usb_device.c

$(NAME)_SOURCES += class/msd/src/usb_device_msd.c

$(NAME)_SOURCES += class/hid/src/usb_device_hid.c \
			       class/hid/src/usb_device_mouse.c

#default gcc
ifeq ($(COMPILER),)
$(NAME)_CFLAGS += -Wall -Werror
else ifeq ($(COMPILER),gcc)
$(NAME)_CFLAGS += -Wall -Werror
endif

GLOBAL_INCLUDES += . core/include \
                     class/msd/include \
                     class/hid/include

GLOBAL_DEFINES += AOS_USB_DEVICE
