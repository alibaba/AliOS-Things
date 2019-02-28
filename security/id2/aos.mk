NAME := id2

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION := 2.0.1
$(NAME)_SUMMARY := ID2 client SDK
#if component's header files under another directory, add RPM_INCLUDE_DIR to indicate where the header file folder is located
RPM_INCLUDE_DIR := ../include/id2

################################################################################
#id2 config

ifeq ($(CONFIG_LS_KM_SE), y)
$(NAME)_DEFINES     += ID2_CRYPTO_TYPE_CONFIG=ID2_CRYPTO_TYPE_3DES
else ifeq ($(CONFIG_LS_KM_TEE), y)
$(NAME)_DEFINES     += ID2_CRYPTO_TYPE_CONFIG=ID2_CRYPTO_TYPE_AES
else
$(NAME)_DEFINES     += ID2_CRYPTO_TYPE_CONFIG=ID2_CRYPTO_TYPE_AES

ifeq ($(CONFIG_LS_ID2_OTP), y)
$(NAME)_DEFINES     += ID2_OTP_SUPPORTED=1
endif
endif

ifeq ($(CONFIG_LS_DEBUG), y)
$(NAME)_DEFINES     += ID2_DEBUG=1
endif

ifeq ($(CONFIG_SYSINFO_DEVICE_NAME), LINUXHOST)
$(NAME)_DEFINES     += ID2_OTP_LOCAL_TEST=1
endif

################################################################################

GLOBAL_INCLUDES   += ../include/id2

$(NAME)_INCLUDES  += ../include/osa
$(NAME)_INCLUDES  += ./src
$(NAME)_INCLUDES  += ../irot/include

$(NAME)_SOURCES   := \
       ./src/core/id2_client.c  \
       ./src/core/km_impl.c     \
       ./src/core/otp.c         \
       ./src/alg/alg.c          \
       ./src/alg/basex64.c      \
       ./src/alg/hashsum.c      \
       ./src/log/log.c          \
       ./src/util/util.c        \
       ./platform/irot_pal.c

$(NAME)_COMPONENTS := ls_osa
$(NAME)_COMPONENTS += alicrypto
$(NAME)_COMPONENTS += irot

