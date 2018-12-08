NAME := ota_verify

#default gcc
ifeq ($(COMPILER),)
$(NAME)_CFLAGS      += -Wall -Werror
else ifeq ($(COMPILER),gcc)
$(NAME)_CFLAGS      += -Wall -Werror
endif

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION := 0.0.1
$(NAME)_SUMMARY := Verify OTA firmware with Hash or RSA.

$(NAME)_SOURCES := ota_sign.c \
                   ota_hash.c \
                   ota_verify.c

ifeq ($(VERIFY),RSA)
GLOBAL_DEFINES += AOS_OTA_RSA
$(NAME)_SOURCES += hash/ota_sha256.c \
                   hash/ota_md5.c \
                   rsa/ota_bignum.c \
                   rsa/ota_rsa.c \
                   rsa/ota_hash_rsa.c
endif
ifeq ($(MD5_CHECK),0)
GLOBAL_DEFINES += AOS_OTA_DISABLE_MD5
endif

GLOBAL_INCLUDES += . ../../inc ../../hal ../verify ./hash ./rsa
