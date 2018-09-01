NAME := sal

$(NAME)_MBINS_TYPE := kernel
ifneq (1,$(at_adapter))
GLOBAL_DEFINES += WITH_SAL # for sal general use
$(NAME)_SOURCES := sal_sockets.c sal_err.c sal_arch.c ip4_addr.c sal.c sal_device.c
GLOBAL_INCLUDES += ./include
endif

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
