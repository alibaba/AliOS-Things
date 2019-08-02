NAME := itls

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION := 1.1.0
$(NAME)_SUMMARY := IoT lightweight SSL/TLS, which implementation is based on mbedtls.
#if component's header files under another directory, add RPM_INCLUDE_DIR to indicate where the header file folder is located
RPM_INCLUDE_DIR := ../include/itls

GLOBAL_INCLUDES     += ../include

ifeq ($(CONFIG_LS_DEBUG), y)
$(NAME)_PREBUILT_LIBRARY := lib/$(HOST_ARCH)/libitls_dbg.a
else
GLOBAL_DEFINES           += CONFIG_SSL_DEBUG_NO
$(NAME)_PREBUILT_LIBRARY := lib/$(HOST_ARCH)/libitls.a
endif

ifeq ($(HOST_ARCH), linux)
ifeq ($(LWIP), 1)
$(NAME)_DEFINES     += LWIP_ENABLED
endif
else
$(NAME)_DEFINES     += LWIP_ENABLED
endif

$(NAME)_COMPONENTS  := ls_osa
$(NAME)_COMPONENTS  += alicrypto
$(NAME)_COMPONENTS  += id2
$(NAME)_COMPONENTS  += irot

$(NAME)_SOURCES     := src/mbedtls_net.c
$(NAME)_SOURCES     += src/mbedtls_alt.c
