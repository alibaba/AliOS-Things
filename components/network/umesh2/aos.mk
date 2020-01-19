NAME := umesh2

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION := 1.0.0
$(NAME)_SUMMARY := uMesh provides the ability to form local area mesh network.

#GLOBAL_DEFINES += 

$(NAME)_DEFINES += UTILS_AES_BUILDIN


$(NAME)_DEFINES += USE_ACTION_FRAME
$(NAME)_COMPONENTS += yloop

$(NAME)_INCLUDES += include/core       \
                    include/adapter    \
                    include/utils  

$(NAME)_SOURCES := src/core/*.c        \
                   src/adapter/*.c     \
                   src/utils/*.c                   



GLOBAL_DEFINES += LWIP_IPV6=0

#default gcc
ifeq ($(COMPILER),)
$(NAME)_CFLAGS      += -Wall -Werror
else ifeq ($(COMPILER),gcc)
$(NAME)_CFLAGS      += -Wall -Werror
endif
