NAME := ota_download

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION := 0.0.1
$(NAME)_SUMMARY := Download firmware form OTA server.

$(NAME)_SOURCES := ota_download_http.c
                   
GLOBAL_INCLUDES += . ../../inc ../../hal ../verify


