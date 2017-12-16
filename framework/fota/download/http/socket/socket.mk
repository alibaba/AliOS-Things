NAME := fota_socket

$(NAME)_TYPE := kernel
GLOBAL_INCLUDES += ./

#default gcc
ifeq ($(COMPILER),)
$(NAME)_CFLAGS      += -Wall -Werror
else ifeq ($(COMPILER),gcc)
$(NAME)_CFLAGS      += -Wall -Werror
endif

ifeq ($(STM32_NONSTD_SOCKET), true)
$(NAME)_COMPONENTS += fota.download.http.socket.stm32wifi
else
$(NAME)_COMPONENTS += fota.download.http.socket.stand
endif

