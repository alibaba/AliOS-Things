NAME := fota_platform

$(NAME)_SOURCES := ota_platform_os.c
GLOBAL_INCLUDES += ./

ifneq (,$(filter protocol.alink,$(COMPONENTS)))
$(NAME)_CFLAGS += -Wall -Werror
$(NAME)_COMPONENTS += fota.platform.alink
else
ifneq (,$(filter connectivity.mqtt,$(COMPONENTS)))
$(NAME)_COMPONENTS += fota.platform.aliot
else
ifneq (,$(filter connectivity.coap,$(COMPONENTS)))
$(NAME)_COMPONENTS += fota.platform.coap
else
$(NAME)_COMPONENTS += fota.platform.common
endif
endif
endif
