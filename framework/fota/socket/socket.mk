NAME := fota_socket

GLOBAL_INCLUDES += ./

ifeq ($(findstring b_l475e, $(BUILD_STRING)), b_l475e)
$(NAME)_CFLAGS += -Wall -Werror
$(NAME)_COMPONENTS += fota.socket.stm32wifi
else
$(NAME)_CFLAGS += -Wall -Werror
$(NAME)_COMPONENTS += fota.socket.stand
endif

