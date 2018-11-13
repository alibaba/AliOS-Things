NAME := itls

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION := 0.0.1
$(NAME)_SUMMARY :=
DEBUG := no

GLOBAL_INCLUDES += include

ifeq ($(COMPILER),armcc)
else
$(NAME)_CFLAGS      += -Wall -Werror -Os
endif

$(NAME)_DEFINES     += CONFIG_PLAT_AOS

ifneq ($(HOST_ARCH), ck802)
$(NAME)_DEFINES     += CONFIG_KEY_OTP_ENABLED
endif

ifeq ($(HOST_ARCH), linux)
ifeq ($(LWIP), 1)
$(NAME)_DEFINES     += LWIP_ENABLED
endif
else
$(NAME)_DEFINES     += LWIP_ENABLED
endif

ifeq ($(DEBUG), no)
$(NAME)_PREBUILT_LIBRARY := lib/$(HOST_ARCH)/libitls.a
else
GLOBAL_DEFINES           += CONFIG_SSL_DEBUG
$(NAME)_PREBUILT_LIBRARY := lib/$(HOST_ARCH)/libitls.a.dbg
endif

$(NAME)_COMPONENTS  := alicrypto
$(NAME)_COMPONENTS  += id2

$(NAME)_SOURCES     := src/mbedtls_net.c
$(NAME)_SOURCES     += src/mbedtls_alt.c
$(NAME)_SOURCES     += src/mbedtls_auth.c
