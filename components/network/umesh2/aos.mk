NAME := umesh2

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION := 1.0.0
$(NAME)_SUMMARY := uMesh provides the ability to form local area mesh network.

#GLOBAL_DEFINES += 

$(NAME)_DEFINES += UTILS_AES_BUILDIN


$(NAME)_DEFINES += USE_ACTION_FRAME
$(NAME)_COMPONENTS += yloop

$(NAME)_INCLUDES += include/core   \
                    include/hal    \
                    include/utils  


$(NAME)_SOURCES := src/core/*.c \
                   src/hal/*.c \
                   src/utils/*.c                   


#default gcc
ifeq ($(COMPILER),)
$(NAME)_CFLAGS      += -Wall -Werror
else ifeq ($(COMPILER),gcc)
$(NAME)_CFLAGS      += -Wall -Werror
endif
