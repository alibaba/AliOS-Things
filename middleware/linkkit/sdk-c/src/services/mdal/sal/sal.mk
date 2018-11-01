NAME := sal

GLOBAL_DEFINES += WITH_SAL

$(NAME)_SOURCES := ./src/sal_sockets.c \
                   ./src/sal_err.c     \
                   ./src/ip4_addr.c

ifeq (y,$(FEATURE_SAL_USE_AOS_HAL))
$(NAME)_SOURCES += ./src/sal_arch_aos.c
else
$(NAME)_SOURCES += ./src/sal_arch.c    \
                   ./src/sal_util.c
endif

GLOBAL_INCLUDES += ./                  \
                   ./include

ifeq (wifi.gt202,$(module))
$(NAME)_COMPONENTS += sal.wifi.gt202
else ifeq (wifi.mk3060,$(module))
$(NAME)_COMPONENTS += sal.wifi.mk3060
else ifeq (wifi.bk7231,$(module))
$(NAME)_COMPONENTS += sal.wifi.bk7231
else ifeq (gprs.sim800,$(module))
$(NAME)_COMPONENTS += sal.gprs.sim800
else ifeq (wifi.esp8266,$(module))
$(NAME)_COMPONENTS += sal.wifi.esp8266
else ifeq (wifi.athost,$(module))
$(NAME)_COMPONENTS += sal.wifi.athost
endif
