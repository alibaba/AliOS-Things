NAME := ota_service

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION := 0.0.1
$(NAME)_SUMMARY := OTA serice of user interface.

$(NAME)_SOURCES := ota_service.c ota_verify.c ota_hash.c ota_rsa_verify.c
                   
GLOBAL_INCLUDES += . ../../inc ../../hal ../verify


