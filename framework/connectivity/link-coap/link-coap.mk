NAME := link-coap

GLOBAL_INCLUDES +=  \
    ./src \
 
$(NAME)_SOURCES := \
    src/CoAPDeserialize.c \
    src/CoAPExport.c \
    src/CoAPMessage.c \
    src/CoAPNetwork.c \
    src/CoAPObserve.c \
    src/CoAPPlatform.c \
    src/CoAPResource.c \
    src/CoAPSerialize.c \
    src/CoAPServer.c 



ifeq ($(LINK_COAP_ALONE),1)
$(NAME)_INCLUDES +=./platform   
$(NAME)_SOURCES :+=  \
    platform/HAL_UDP_rhino.c \
    platform/HAL_OS_rhino.c 
else
$(NAME)_COMPONENTS := protocol.linkkit.iotkit
endif
ifeq ($(COMPILER),)
$(NAME)_CFLAGS := $(filter-out -Werror,$(CFLAGS))
else ifeq ($(COMPILER),gcc)
$(NAME)_CFLAGS := $(filter-out -Werror,$(CFLAGS))
endif

$(NAME)_DEFINES += DEBUG USE_LPTHREAD
# PKG_UPDATE  := 'git@gitlab.alibaba-inc.com:iot-middleware/iot-coap-c.git'
