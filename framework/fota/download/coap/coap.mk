NAME := download_coap

$(NAME)_TYPE := kernel
$(NAME)_SOURCES := ota_download.c
$(NAME)_INCLUDES := ./ \
		   ../
