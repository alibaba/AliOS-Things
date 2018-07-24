
NAME := alicrypto

ALICRYPTO_TEST := no 

ifneq (,$(BINS))
ifeq ($(MBEDTLS_SHARE),1)
$(NAME)_TYPE := framework&kernel
else
$(NAME)_TYPE := kernel
endif
endif

$(NAME)_INCLUDES     += ./mbedtls/include/mbedtls
$(NAME)_INCLUDES     += ./libalicrypto/mbed/inc
$(NAME)_INCLUDES     += ./libalicrypto/sw
GLOBAL_INCLUDES      += ./libalicrypto/inc

$(NAME)_CFLAGS       += -DCONFIG_CRYPT_MBED=1 -DCONFIG_DBG_CRYPT=1
GLOBAL_DEFINES       += CONFIG_ALICRYPTO

ifeq ($(COMPILER),)
$(NAME)_CFLAGS      += -W -Wdeclaration-after-statement
endif

ifneq (, $(findstring cb2201, $(BUILD_STRING)))
$(NAME)_INCLUDES	+= ./csky/cb2201
$(NAME)_SOURCES     += 							\
           ./csky/ch2201/aes.c 	\
           ./libalicrypto/mbed/hash/hash.c 		\
           ./csky/ch2201/rsa.c 		\
           ./libalicrypto/mbed/mac/hmac.c 		\
           ./csky/ch2201/rand.c 	\
           ./libalicrypto/ali_crypto.c

else

$(NAME)_SOURCES     += 							\
           ./libalicrypto/mbed/cipher/aes.c 	\
           ./libalicrypto/mbed/hash/hash.c 		\
           ./libalicrypto/mbed/asym/rsa.c 		\
           ./libalicrypto/mbed/mac/hmac.c 		\
           ./libalicrypto/sw/ali_crypto_rand.c 	\
           ./libalicrypto/ali_crypto.c
endif

$(NAME)_CFLAGS      += -D_FILE_OFFSET_BITS=64

$(NAME)_INCLUDES 	+= ./mbedtls/include

$(NAME)_SOURCES     += 									\
           ./mbedtls/library/aes.c

$(NAME)_SOURCES     += 									\
           ./mbedtls/library/md5.c 						\
           ./mbedtls/library/sha1.c 					\
           ./mbedtls/library/sha256.c


$(NAME)_SOURCES     += 									\
           ./mbedtls/library/hash_wrap.c 				\
           ./mbedtls/library/bignum.c 					\
           ./mbedtls/library/oid.c 						\
           ./mbedtls/library/rsa.c 						\
           ./mbedtls/library/threading.c 				\
           ./mbedtls/library/mbedtls_alt.c 				\
           ./mbedtls/library/asn1parse.c 				\

$(NAME)_SOURCES     += 									\
           ./mbedtls/library/hmac.c 					\

ifeq ($(ALICRYPTO_TEST), yes)

$(NAME)_INCLUDES     += ./libalicrypto/test/inc
$(NAME)_SOURCES += 												\
				./libalicrypto/test/ali_crypto_test.c 			\
				./libalicrypto/test/ali_crypto_test_comm.c 		\
				./libalicrypto/test/ali_crypto_test_aes.c 		\
				./libalicrypto/test/ali_crypto_test_hash.c 		\
				./libalicrypto/test/ali_crypto_test_rand.c 		\
				./libalicrypto/test/ali_crypto_test_rsa.c 		\
				./libalicrypto/test/ali_crypto_test_hmac.c 		\

endif

