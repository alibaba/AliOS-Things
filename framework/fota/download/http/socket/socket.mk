NAME := fota_socket

$(NAME)_TYPE := kernel
GLOBAL_INCLUDES += ./

ifeq ($(SPI_WIFI_ENABLED), true)
$(NAME)_CFLAGS += -Wall -Werror
$(NAME)_COMPONENTS += fota.download.http.socket.stm32wifi
else
$(NAME)_CFLAGS += -Wall -Werror
$(NAME)_COMPONENTS += fota.download.http.socket.stand
endif

