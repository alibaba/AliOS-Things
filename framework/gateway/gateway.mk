NAME := gateway

GLOBAL_INCLUDES += ./

GLOBAL_CFLAGS += -DMESH_GATEWAY_SERVICE -DGATEWAY_SDK

$(NAME)_CFLAGS += -Wall -Werror

$(NAME)_SOURCES += gateway_service.c

$(NAME)_CFLAGS  += -Wall -Werror -Wno-unused-variable -Wno-unused-parameter -Wno-implicit-function-declaration
$(NAME)_CFLAGS  += -Wno-type-limits -Wno-sign-compare -Wno-pointer-sign -Wno-uninitialized
$(NAME)_CFLAGS  += -Wno-return-type -Wno-unused-function -Wno-unused-but-set-variable
$(NAME)_CFLAGS  += -Wno-unused-value -Wno-strict-aliasing

$(NAME)_COMPONENTS += cjson protocols.net protocols.mesh protocol.alink gateway.devmgr gateway.msdp
