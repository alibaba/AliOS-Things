
NAME := alicrypto
ALICRYPTO_TEST := yes

ifneq (,$(BINS))
ifeq ($(MBEDTLS_SHARE),1)
$(NAME)_TYPE := share
else
$(NAME)_TYPE := kernel
endif
endif

$(NAME)_SOURCES :=
$(NAME)_COMPONENTS :=

GLOBAL_INCLUDES     += inc
GLOBAL_LDFLAGS      +=
GLOBAL_DEFINES      += CONFIG_ALICRYPTO
GLOBAL_CFLAGS       +=

#$(NAME)_SOURCES += test/ali_crypto_test_weak.c
PLATFORM := linuxhost

ifeq ($(HOST_ARCH), linux)
PLATFORM := linuxhost
$(NAME)_PREBUILT_LIBRARY := lib/$(PLATFORM)/libmbedcrypto.a  \
		lib/$(PLATFORM)/libalicrypto.a
ifeq ($(ALICRYPTO_TEST), yes)
GLOBAL_INCLUDES     += test
GLOBAL_LDFLAGS      +=
$(NAME)_SOURCES += \
				test/ali_crypto_test.c \
				test/ali_crypto_test_comm.c \
				test/ali_crypto_test_aes.c \
				test/ali_crypto_test_hash.c \
				test/ali_crypto_test_rand.c \
				test/ali_crypto_test_rsa.c \
				test/ali_crypto_test_hmac.c \
				
endif

else ifeq ($(HOST_ARCH), armhflinux)
PLATFORM := armhflinux
$(NAME)_PREBUILT_LIBRARY := lib/$(PLATFORM)/libmbedcrypto.a  \
		lib/$(PLATFORM)/libalicrypto.a

ifeq ($(ALICRYPTO_TEST), yes)
GLOBAL_INCLUDES     += test
GLOBAL_LDFLAGS      +=
$(NAME)_SOURCES += \
				test/ali_crypto_test.c \
				test/ali_crypto_test_comm.c \
				test/ali_crypto_test_aes.c \
				test/ali_crypto_test_hash.c \
				test/ali_crypto_test_rand.c \
				test/ali_crypto_test_rsa.c \
				test/ali_crypto_test_hmac.c \
				
endif # end ALICRYPTO_TEST=yes

else ifeq ($(HOST_ARCH), Cortex-M4)
PLATFORM := b_l475e
$(NAME)_PREBUILT_LIBRARY := lib/$(PLATFORM)/libmbedcrypto.a  \
                lib/$(PLATFORM)/libalicrypto.a

ifeq ($(ALICRYPTO_TEST), yes)
GLOBAL_INCLUDES     += test
GLOBAL_LDFLAGS      +=
$(NAME)_SOURCES += \
                                test/ali_crypto_test.c \
                                test/ali_crypto_test_comm.c \
                                test/ali_crypto_test_aes.c \
                                test/ali_crypto_test_hash.c \
                                test/ali_crypto_test_rand.c \
                                test/ali_crypto_test_rsa.c \
                                test/ali_crypto_test_hmac.c \

endif # end ALICRYPTO_TEST=yes

else ifeq ($(HOST_ARCH), ARM968E-S)
PLATFORM := mk108
$(NAME)_PREBUILT_LIBRARY := lib/mk108/thumb/libmbedcrypto.a  \
		lib/$(PLATFORM)/thumb/libalicrypto.a

ifeq ($(ALICRYPTO_TEST), yes)
GLOBAL_INCLUDES     += test
GLOBAL_LDFLAGS      +=
$(NAME)_SOURCES += \
				test/ali_crypto_test.c \
				test/ali_crypto_test_comm.c \
				test/ali_crypto_test_aes.c \
				test/ali_crypto_test_hash.c \
				test/ali_crypto_test_rand.c \
				test/ali_crypto_test_rsa.c \
				test/ali_crypto_test_hmac.c \
				
endif # end ALICRYPTO_TEST=yes

endif # !linuxapp@linuxhost
