NAME := cryptotest

$(NAME)_SOURCES := cryptotest.c

TESTPATH := ../../../security/alicrypto

$(NAME)_INCLUDES     += $(TESTPATH)/libalicrypto/test/inc

$(NAME)_SOURCES += $(TESTPATH)/libalicrypto/test/ali_crypto_test.c           \
                   $(TESTPATH)/libalicrypto/test/ali_crypto_test_comm.c      \
                   $(TESTPATH)/libalicrypto/test/ali_crypto_test_aes.c       \
                   $(TESTPATH)/libalicrypto/test/ali_crypto_test_hash.c      \
                   $(TESTPATH)/libalicrypto/test/ali_crypto_test_rand.c      \
                   $(TESTPATH)/libalicrypto/test/ali_crypto_test_rsa.c       \
                   $(TESTPATH)/libalicrypto/test/ali_crypto_test_hmac.c

GLOBAL_DEFINES += AOS_NO_WIFI

$(NAME)_COMPONENTS := yloop cli alicrypto

ifeq ($(BENCHMARKS),1)
$(NAME)_COMPONENTS  += benchmarks
GLOBAL_DEFINES      += CONFIG_CMD_BENCHMARKS
endif

GLOBAL_INCLUDES += ./