
NAME := alicrypto

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION := 2.0.2
$(NAME)_SUMMARY := Crypto algorithms and APIs for AliOS Things.
#if component's header files under another directory, add RPM_INCLUDE_DIR to indicate where the header file folder is located
RPM_INCLUDE_DIR    := ../include/icrypto

GLOBAL_INCLUDES     += ../include/icrypto

$(NAME)_DEFINES     += CONFIG_RSA_ENCRYPT
$(NAME)_DEFINES     += CONFIG_RSA_DECRYPT
$(NAME)_DEFINES     += CONFIG_RSA_SIGN
$(NAME)_DEFINES     += CONFIG_RSA_VERIFY
$(NAME)_DEFINES     += RSA_PADDING_PKCS1_V21
$(NAME)_DEFINES     += RSA_PADDING_PKCS1_V15

$(NAME)_SOURCES     := ./src/api_aes.c
$(NAME)_SOURCES     += ./src/api_hash.c
$(NAME)_SOURCES     += ./src/api_hmac.c
$(NAME)_SOURCES     += ./src/api_init.c
$(NAME)_SOURCES     += ./src/api_rand.c
$(NAME)_SOURCES     += ./src/api_rsa.c
$(NAME)_SOURCES     += ./src/api_sm2.c
$(NAME)_SOURCES     += ./src/api_sm4.c

$(NAME)_COMPONENTS  := ls_osa
$(NAME)_COMPONENTS  += ls_hal

