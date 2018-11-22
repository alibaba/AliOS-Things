NAME := ota_download

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION := 0.0.1
$(NAME)_SUMMARY :=

$(NAME)_SOURCES := ota_download_http.c ota_download_coap.c
                   
GLOBAL_INCLUDES += . ../../inc ../../hal ../verify


