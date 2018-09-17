NAME := usb_host

$(NAME)_TYPE        := kernel

$(NAME)_SOURCES     +=  core/src/usb_host_core.c \
                        core/src/usb_host_device.c \
                        core/src/usb_host_os.c \
                        core/src/usb_host_pipe.c \
                        core/src/usb_host_request.c \
                        core/src/usb_host_transfer.c

$(NAME)_SOURCES     +=  class/msd/src/usb_host_msd.c \
                        class/rndis/src/usb_host_rndis.c

#default gcc
ifeq ($(COMPILER),)
$(NAME)_CFLAGS      += -Wall -Werror
else ifeq ($(COMPILER),gcc)
$(NAME)_CFLAGS      += -Wall -Werror
endif

GLOBAL_INCLUDES     +=  . core/include \
                        class/msd/include \
                        class/rndis/include

GLOBAL_DEFINES      += AOS_USB_HOST
