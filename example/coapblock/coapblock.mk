NAME := coapblock

$(NAME)_SOURCES     := coapblock.c
GLOBAL_DEFINES      += ALIOT_DEBUG

#CONFIG_COAP_DTLS_SUPPORT := y
#CONFIG_COAP_ONLINE := y

ifeq ($(CONFIG_COAP_DTLS_SUPPORT), y)
$(NAME)_DEFINES += COAP_DTLS_SUPPORT
endif
ifeq ($(CONFIG_COAP_ONLINE), y)
$(NAME)_DEFINES += COAP_ONLINE
endif


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

$(NAME)_COMPONENTS  += connectivity.coap  netmgr fota framework.common
