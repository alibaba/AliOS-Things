NAME := ota

$(NAME)_TYPE := kernel
$(NAME)_SOURCES := ota.c \
                   ota_file.c \
                   ota_http.c
