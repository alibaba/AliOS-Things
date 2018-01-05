NAME := gateway

$(NAME)_TYPE := framework
GLOBAL_INCLUDES += ./

GLOBAL_CFLAGS += -DMESH_GATEWAY_SERVICE -DGATEWAY_SDK


$(NAME)_SOURCES += gateway_service.c

$(NAME)_COMPONENTS += cjson protocols.mesh protocol.alink gateway.devmgr gateway.msdp

GLOBAL_DEFINES += CONFIG_GATEWAY
