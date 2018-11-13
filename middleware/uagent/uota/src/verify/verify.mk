NAME := ota_verify

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION := 0.0.1
$(NAME)_SUMMARY :=

$(NAME)_SOURCES := ota_verify.c \
                   ota_rsa_verify.c \
                   hash/ota_hash.c \
                   hash/ota_md5.c \
                   hash/ota_sha256.c \
                   crc/ota_crc.c \
                   rsa/ota_bignum.c \
                   rsa/ota_rsa.c \
                   rsa/ota_hash_rsa.c \
                   base64/ota_base64.c 

GLOBAL_INCLUDES += . ../../inc ../../hal ../verify ./crc ./hash ./rsa
