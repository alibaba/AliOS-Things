NAME := fota_download

$(NAME)_TYPE := kernel
GLOBAL_INCLUDES += ./

#default gcc
ifeq ($(COMPILER),)
$(NAME)_CFLAGS      += -Wall -Werror
else ifeq ($(COMPILER),gcc)
$(NAME)_CFLAGS      += -Wall -Werror
endif

$(NAME)_SOURCES := download_common.c


ifneq (,$(filter connectivity.coap,$(COMPONENTS)))
ifneq (,$(FOTA_DL_COAP))
$(NAME)_COMPONENTS += fota.download.coap
else
$(NAME)_COMPONENTS += fota.download.http
endif
else
$(NAME)_COMPONENTS += fota.download.http
endif
