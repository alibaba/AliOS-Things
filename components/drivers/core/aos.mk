NAME := udriver

$(NAME)_MBINS_TYPE := share
$(NAME)_VERSION    := 1.0.0
$(NAME)_SUMMARY    := Device-Driver Model Layer

$(NAME)_SOURCES := u_bus.c
$(NAME)_SOURCES += u_device.c
$(NAME)_SOURCES += u_driver.c
#$(NAME)_SOURCES += u_interrupt.c
$(NAME)_SOURCES += u_ld.c

GLOBAL_CFLAGS += -DENABLE_UBUS_MODEL -DENABLE_UDRV_MODEL -DENABLE_UDEV_MODEL

GLOBAL_CFLAGS += -DDEBUG_MT8153

#$(NAME)_CFLAGS += -Werror

GLOBAL_DEFINES +=

#GLOBAL_LDFLAGS +=  -Wl,--wrap=disable_irq
#GLOBAL_LDFLAGS +=  -Wl,--wrap=enable_irq
#GLOBAL_LDFLAGS +=  -Wl,--wrap=free_irq




$(NAME)_INCLUDES +=
