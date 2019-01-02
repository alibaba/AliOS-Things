NAME := id2

LIBID2CLIENT := .

################################################################################
#diffrent config for KM, TEE, SE

LINUXHOST_ID2_CFLAGS	= -DID2_OTP_SUPPORTED=1 -DID2_CRYPTO_TYPE_CONFIG=ID2_CRYPTO_TYPE_AES -DID2_OTP_LOCAL_TEST=1 -DID2_DEBUG=1
 KM_ID2_CFLAGS			= -DID2_OTP_SUPPORTED=1 -DID2_CRYPTO_TYPE_CONFIG=ID2_CRYPTO_TYPE_AES
TEE_ID2_CFLAGS			= -DID2_OTP_SUPPORTED=0 -DID2_CRYPTO_TYPE_CONFIG=ID2_CRYPTO_TYPE_AES
 SE_ID2_CFLAGS			= -DID2_OTP_SUPPORTED=0 -DID2_CRYPTO_TYPE_CONFIG=ID2_CRYPTO_TYPE_3DES

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

$(NAME)_INCLUDES     += $(LIBID2CLIENT)/../irot/include
$(NAME)_INCLUDES     += $(LIBID2CLIENT)/include
$(NAME)_INCLUDES     += $(LIBID2CLIENT)/platform/include
$(NAME)_INCLUDES     += $(LIBID2CLIENT)/src
$(NAME)_INCLUDES     += $(LIBID2CLIENT)

GLOBAL_INCLUDES     += $(LIBID2CLIENT)/..
GLOBAL_INCLUDES     += $(LIBID2CLIENT)/platform/include
GLOBAL_INCLUDES     += $(LIBID2CLIENT)/include

$(NAME)_SOURCES     += 							\
    $(LIBID2CLIENT)/src/util/util.c \
    $(LIBID2CLIENT)/src/log/log.c \
    $(LIBID2CLIENT)/src/alg/alg.c \
    $(LIBID2CLIENT)/src/core/otp.c \
    $(LIBID2CLIENT)/src/core/km_impl.c \
    $(LIBID2CLIENT)/src/core/id2_client.c \
    $(LIBID2CLIENT)/platform/aos/irot_pal.c \

$(NAME)_COMPONENTS += base64
$(NAME)_COMPONENTS += alicrypto
$(NAME)_COMPONENTS += irot

