NAME := ota_download

$(NAME)_TYPE := uota

$(NAME)_SOURCES := ota_download_http.c ota_download_coap.c
                   
GLOBAL_INCLUDES += . ../../inc ../../hal ../verify


