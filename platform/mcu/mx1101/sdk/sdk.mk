NAME = mx1101_sdk

$(NAME)_INCLUDES := \
CMSIS/Include \
O18B_SDK_GCC/MVs_new/MVs18_SDK/Libraries/driver/inc

$(NAME)_SOURCES := \
startup.s \
main.c 

GLOBAL_LDFLAGS += platform/mcu/mx1101/sdk/O18B_SDK_GCC/lib/libdriver.a

GLOBAL_LDFLAGS += -T platform/mcu/mx1101/sdk/link.ld
