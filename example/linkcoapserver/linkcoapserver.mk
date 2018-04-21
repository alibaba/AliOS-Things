NAME := coapserverapp

$(NAME)_SOURCES     := serverapp.c CoAPServer.c
GLOBAL_DEFINES      += ALIOT_DEBUG  USE_LPTHREAD


$(NAME)_COMPONENTS  := cli
#ifeq ($(LWIP),1)
#$(NAME)_COMPONENTS  += protocols.net
#endif

ifneq ($(findstring linuxhost, $(BUILD_STRING)), linuxhost)
$(NAME)_DEFINES += PLATFORM_NOT_LINUX
endif

$(NAME)_CFLAGS += \
    -Wno-unused-function \
    -Wno-implicit-function-declaration \
    -Wno-unused-function \
#    -Werror

with_yloop := 0
ifeq ($(with_yloop),1)
GLOBAL_DEFINES += COAP_WITH_YLOOP
endif

$(NAME)_COMPONENTS  += connectivity.link-coap  netmgr  framework.common framework.protocol.alink-ilop
