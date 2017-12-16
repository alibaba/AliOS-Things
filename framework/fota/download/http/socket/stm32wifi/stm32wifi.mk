NAME := socket_stm32wifi

GLOBAL_INCLUDES += ./
$(NAME)_SOURCES := ota_socket.c
$(NAME)_INCLUDES := ./ \
		   ../
