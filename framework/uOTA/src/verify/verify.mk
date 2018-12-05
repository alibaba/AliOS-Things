NAME := ota_verify

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION := 0.0.1
$(NAME)_SUMMARY := Verify OTA firmware with Hash or RSA.

$(NAME)_SOURCES := ota_verify.c \
                   ota_rsa_verify.c \
                   hash/ota_hash.c \
                   crc/ota_crc.c \
                   base64/ota_base64.c 

GLOBAL_INCLUDES += . ../../inc ../../hal ../verify ./crc ./hash ./rsa
