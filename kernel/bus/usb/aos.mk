NAME := usb

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION    := 1.0.0
$(NAME)_SUMMARY    := USB Host & Device Stack

USB_MODE ?= host

ifeq ($(USB_MODE), host)
$(NAME)_SOURCES += usb_host/core/src/usb_host_core.c \
                   usb_host/core/src/usb_host_device.c \
                   usb_host/core/src/usb_host_os.c \
                   usb_host/core/src/usb_host_pipe.c \
                   usb_host/core/src/usb_host_request.c \
                   usb_host/core/src/usb_host_transfer.c

$(NAME)_SOURCES += usb_host/class/msd/src/usb_host_msd.c \
                   usb_host/class/rndis/src/usb_host_rndis.c

$(NAME)_INCLUDES += usb_host \
                   usb_host/core/include \
                   usb_host/class/msd/include \
                   usb_host/class/rndis/include


else ifeq ($(USB_MODE), device)


$(NAME)_SOURCES += usb_device/core/src/usb_device.c

$(NAME)_SOURCES += usb_device/class/msd/src/usb_device_msd.c

$(NAME)_SOURCES += usb_device/class/hid/src/usb_device_hid.c \
                   usb_device/class/hid/src/usb_device_mouse.c

$(NAME)_INCLUDES += usb_device \
                   usb_device/core/include \
                   usb_device/class/msd/include \
                   usb_device/class/hid/include

endif

#default gcc
ifeq ($(COMPILER),)
$(NAME)_CFLAGS += -Wall -Werror
else ifeq ($(COMPILER),gcc)
$(NAME)_CFLAGS += -Wall -Werror
endif

GLOBAL_DEFINES += AOS_COMP_USB
