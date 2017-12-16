NAME := download_http

$(NAME)_TYPE := kernel
$(NAME)_SOURCES := ota_download.c
$(NAME)_INCLUDES := ./ \
		   ../ \
	     ./socket/ 

$(NAME)_COMPONENTS += fota.download.http.socket
