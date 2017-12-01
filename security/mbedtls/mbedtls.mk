NAME := mbedtls

DEBUG := no

ifneq (,$(BINS))
MBEDTLS_SHARE := 0
ifeq ($(MBEDTLS_SHARE),1)
$(NAME)_TYPE := framework&kernel
else
$(NAME)_TYPE := kernel
GLOBAL_DEFINES     += MBEDTLS_IN_KERNEL
endif
endif

GLOBAL_INCLUDES     += include

$(NAME)_CFLAGS      += -Wall -Werror -Os

ifeq ($(DEBUG), yes)
$(NAME)_DEFINES     += CONFIG_SSL_DEBUG
endif

$(NAME)_COMPONENTS  := alicrypto

ifeq ($(HOST_ARCH), linux)
ifeq ($(LWIP), 1)
$(NAME)_DEFINES     += LWIP_ENABLED
endif
else
$(NAME)_DEFINES     += LWIP_ENABLED
endif

ifeq ($(HOST_ARCH), linux)
$(NAME)_PREBUILT_LIBRARY := lib/$(HOST_ARCH)/libmbedtls.a
ifeq (1,$(with_lwip))
$(info using lwip version mbedtls)
$(NAME)_PREBUILT_LIBRARY := lib/$(HOST_ARCH)/libmbedtls.a.lwip
endif
else ifeq ($(HOST_ARCH), xtensa)
$(NAME)_PREBUILT_LIBRARY := lib/$(HOST_ARCH)/libmbedtls.a
ifeq ($(DEBUG), yes)
$(info using debug version mbedtls)
$(NAME)_PREBUILT_LIBRARY := lib/$(HOST_ARCH)/libmbedtls.a.dbg
endif
else
$(NAME)_PREBUILT_LIBRARY := lib/$(HOST_ARCH)/libmbedtls.a
endif

$(NAME)_SOURCES     := mbedtls_net.c
$(NAME)_SOURCES     += mbedtls_ssl.c

