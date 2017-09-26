NAME := alink

GLOBAL_INCLUDES += .
$(NAME)_SOURCES  := service_manager.c

include framework/protocol/alink/accs/accs.mk
include framework/protocol/alink/json/json.mk
include framework/protocol/alink/system/system.mk
include framework/protocol/alink/cota/cota.mk
include framework/protocol/alink/os/os.mk

$(NAME)_CFLAGS  += -Wall -Werror -Wno-unused-variable -Wno-unused-parameter -Wno-implicit-function-declaration
$(NAME)_CFLAGS  += -Wno-type-limits -Wno-sign-compare -Wno-pointer-sign -Wno-uninitialized
$(NAME)_CFLAGS  += -Wno-return-type -Wno-unused-function -Wno-unused-but-set-variable
$(NAME)_CFLAGS  += -Wno-unused-value -Wno-strict-aliasing

$(NAME)_COMPONENTS := connectivity.wsf digest_algorithm cjson base64 hashtable log yloop modules.fs.kv cloud
