NAME := umesh2

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION := 1.0.0
$(NAME)_SUMMARY := uMesh provides the ability to form local area mesh network.

$(NAME)_DEFINES += UTILS_AES_BUILDIN


$(NAME)_DEFINES += USE_ACTION_FRAME

$(NAME)_INCLUDES += include/core       \
                    include/adapter    \
                    include/utils  

$(NAME)_SOURCES := src/core/*.c        \
                   src/adapter/*.c     \
                   src/utils/*.c                   

$(NAME)_DEFINES += LWIP_IPV6=1

$(NAME)_COMPONENTS-$(UMESH_USE_YLOOP) := yloop
