NAME := libmisc

$(NAME)_INCLUDES += \
    ./ \
    ../sdk-impl \
    ../LITE-log \
    ../LITE-utils
# don't modify to L_CFLAGS, because CONFIG_CJSON_WITHOUT_DOUBLE should enable global

$(NAME)_SOURCES := \
    utils_httpc.c \
    utils_epoch_time.c \
    utils_list.c \
    utils_net.c \
    utils_timer.c

ifeq ($(findstring linuxhost, $(BUILD_STRING)), linuxhost)
PLATFORM_MISC := linux
else ifeq ($(findstring mk3060, $(BUILD_STRING)), mk3060)
PLATFORM_MISC := rhino
else ifeq ($(findstring b_l475e, $(BUILD_STRING)), b_l475e)
PLATFORM_MISC := rhino
endif

ifneq (,$(filter connectivity.coap,$(COMPONENTS)))
$(NAME)_SOURCES += ../hal/$(PLATFORM_MISC)/HAL_TCP_$(PLATFORM_MISC).c
ifneq ($(CONFIG_COAP_DTLS_SUPPORT), y)
$(NAME)_SOURCES += ../hal/$(PLATFORM_MISC)/HAL_OS_$(PLATFORM_MISC).c
#GLOBAL_DEFINES += IOTX_WITHOUT_TLS
$(NAME)_COMPONENTS += iotx-utils.mbedtls-hal
endif
endif
