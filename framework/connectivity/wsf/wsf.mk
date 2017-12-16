NAME := wsf

$(NAME)_TYPE := framework
$(NAME)_SOURCES := wsf.c wsf_list.c wsf_network.c wsf_client.c wsf_msg.c \
	           wsf_ssl_cert.c wsf_config.c wsf_msg_queue.c wsf_worker.c \
                   connectivity_manager.c

#default gcc
ifeq ($(COMPILER),)
$(NAME)_CFLAGS  += -Wall -Werror -Wno-unused-variable -Wno-unused-parameter -Wno-implicit-function-declaration
$(NAME)_CFLAGS  += -Wno-type-limits -Wno-sign-compare -Wno-pointer-sign -Wno-uninitialized
$(NAME)_CFLAGS  += -Wno-return-type -Wno-unused-function -Wno-unused-but-set-variable
$(NAME)_CFLAGS  += -Wno-unused-value -Wno-strict-aliasing
else ifeq ($(COMPILER),gcc)
$(NAME)_CFLAGS  += -Wall -Werror -Wno-unused-variable -Wno-unused-parameter -Wno-implicit-function-declaration
$(NAME)_CFLAGS  += -Wno-type-limits -Wno-sign-compare -Wno-pointer-sign -Wno-uninitialized
$(NAME)_CFLAGS  += -Wno-return-type -Wno-unused-function -Wno-unused-but-set-variable
$(NAME)_CFLAGS  += -Wno-unused-value -Wno-strict-aliasing
endif

$(NAME)_DEFINES += DEBUG
GLOBAL_INCLUDES += .
