NAME := ssl_client

$(NAME)_SOURCES     := ssl_client.c
$(NAME)_SOURCES     += dtls_client.c

$(NAME)_COMPONENTS  := mbedtls alicrypto

ifeq ($(findstring b_l475e, $(BUILD_STRING)), b_l475e)
$(NAME)_DEFINES     := MBEDTLS_NET_ALT_UART
endif

ifneq (,${BINS})
GLOBAL_CFLAGS += -DSYSINFO_OS_BINS
endif
CURRENT_TIME = $(shell /bin/date +%Y%m%d.%H%M)
CONFIG_SYSINFO_APP_VERSION = APP-1.0.0-$(CURRENT_TIME)
$(info app_version:${CONFIG_SYSINFO_APP_VERSION})
GLOBAL_CFLAGS += -DSYSINFO_APP_VERSION=\"$(CONFIG_SYSINFO_APP_VERSION)\"

