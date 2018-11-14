NAME := libiot_dm

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION := 2.3.0
$(NAME)_SUMMARY :=

# $(NAME)_SOURCES := ./dm_api.c \
# ./dm_cota.c \
# ./dm_fota.c \
# ./dm_ipc.c \
# ./dm_manager.c \
# ./dm_message.c \
# ./dm_message_cache.c \
# ./dm_msg_process.c \
# ./dm_opt.c \
# ./dm_ota.c \
# ./dm_shadow.c \
# ./dm_tsl_alink.c \
# ./dm_utils.c \
# ./client/dm_client_adapter.c \
# ./client/dm_client.c \
# ./server/dm_server_adapter.c \
# ./server/dm_server.c \

LINKKIT_MODULE  := middleware/linkkit/sdk-c/src/services/linkkit/dm

$(NAME)_SOURCES := $(wildcard $(SOURCE_ROOT)/$(LINKKIT_MODULE)/*.c)
$(NAME)_SOURCES += $(wildcard $(SOURCE_ROOT)/$(LINKKIT_MODULE)/*/*.c)

$(NAME)_SOURCES := $(foreach S,$($(NAME)_SOURCES),$(subst $(SOURCE_ROOT)/$(LINKKIT_MODULE),.,$(S)))

 
$(NAME)_COMPONENTS := middleware/linkkit/sdk-c/src/infra/utils \
middleware/linkkit/sdk-c/src/infra/log \
middleware/linkkit/sdk-c/src/infra/system \
middleware/linkkit/sdk-c/src/sdk-impl \


$(NAME)_INCLUDES := ./server \
./client \
../../../../src/infra \
../../../../src/infra/log \
../../../../src/infra/utils \
../../../../src/infra/utils/misc \
../../../../src/infra/utils/digest \
../../../../src/infra/system \
../../../../src/services/linkkit/cm \
../../../../src/services/linkkit/cm/include \
./example \

