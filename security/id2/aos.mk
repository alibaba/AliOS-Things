NAME := id2

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION := 2.0.1
$(NAME)_SUMMARY := ID2 client SDK

$(NAME)_CFLAGS  := -DPLATFORM_ALIOS

################################################################################
#diffrent config for KM, TEE, SE

LINUXHOST_ID2_CFLAGS = -DID2_OTP_SUPPORTED=1 -DID2_CRYPTO_TYPE_CONFIG=ID2_CRYPTO_TYPE_AES -DID2_OTP_LOCAL_TEST=1 -DID2_DEBUG=1
KM_ID2_CFLAGS	     = -DID2_OTP_SUPPORTED=1 -DID2_CRYPTO_TYPE_CONFIG=ID2_CRYPTO_TYPE_AES
TEE_ID2_CFLAGS	     = -DID2_OTP_SUPPORTED=0 -DID2_CRYPTO_TYPE_CONFIG=ID2_CRYPTO_TYPE_AES
SE_ID2_CFLAGS	     = -DID2_OTP_SUPPORTED=0 -DID2_CRYPTO_TYPE_CONFIG=ID2_CRYPTO_TYPE_3DES

ifeq ($(findstring linuxhost, $(BUILD_STRING)), linuxhost)
	$(NAME)_CFLAGS      += $(LINUXHOST_ID2_CFLAGS)
else ifeq ($(findstring mk3060, $(BUILD_STRING)), mk3060)
	$(NAME)_CFLAGS      += $(KM_ID2_CFLAGS)
else ifeq ($(findstring mk3080, $(BUILD_STRING)), mk3080)
	$(NAME)_CFLAGS      += $(KM_ID2_CFLAGS)
else ifeq ($(findstring cb2201, $(BUILD_STRING)), cb2201)
	$(NAME)_CFLAGS      += $(TEE_ID2_CFLAGS)
endif

################################################################################

GLOBAL_INCLUDES   += ../include/id2

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

$(NAME)_COMPONENTS += alicrypto
$(NAME)_COMPONENTS += irot

