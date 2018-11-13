
NAME := alicrypto

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION := 0.0.1
$(NAME)_SUMMARY :=

GLOBAL_INCLUDES      += ./libalicrypto/inc

$(NAME)_CFLAGS       += -DCONFIG_CRYPT_MBED=1 -DCONFIG_DBG_CRYPT=1
GLOBAL_DEFINES       += CONFIG_ALICRYPTO

ifeq ($(COMPILER),)
$(NAME)_CFLAGS      += -W -Wdeclaration-after-statement
endif

ifneq (, $(findstring cb2201, $(BUILD_STRING)))
$(NAME)_SOURCES     +=                          \
           ./tee/aes.c                          \
           ./libalicrypto/mbed/hash/hash.c      \
           ./libalicrypto/mbed/asym/rsa.c       \
           ./libalicrypto/mbed/mac/hmac.c       \
           ./tee/rand.c                         \
           ./libalicrypto/ali_crypto.c          \
           ./tee/tee_crypto_client.c

else

$(NAME)_SOURCES     +=                          \
           ./libalicrypto/mbed/cipher/aes.c     \
           ./libalicrypto/mbed/hash/hash.c      \
           ./libalicrypto/mbed/asym/rsa.c       \
           ./libalicrypto/mbed/mac/hmac.c       \
           ./libalicrypto/sw/ali_crypto_rand.c  \
           ./libalicrypto/ali_crypto.c
endif

$(NAME)_CFLAGS      += -D_FILE_OFFSET_BITS=64

$(NAME)_SOURCES     +=                                  \
           ./mbedtls/library/aes.c

$(NAME)_SOURCES     +=                                  \
           ./mbedtls/library/md5.c                      \
           ./mbedtls/library/sha1.c                     \
           ./mbedtls/library/sha256.c


$(NAME)_SOURCES     +=                                  \
           ./mbedtls/library/hash_wrap.c                \
           ./mbedtls/library/bignum.c                   \
           ./mbedtls/library/oid.c                      \
           ./mbedtls/library/threading.c                \
           ./mbedtls/library/mbedtls_alt.c              \
           ./mbedtls/library/asn1parse.c

ifneq ($(IDE),iar)
$(NAME)_SOURCES     += ./mbedtls/library/rsa.c
endif

$(NAME)_SOURCES     +=                                  \
           ./mbedtls/library/hmac.c



