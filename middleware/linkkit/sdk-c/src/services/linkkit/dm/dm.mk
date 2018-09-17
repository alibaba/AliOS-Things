NAME := libiot_dm

$(NAME)_SOURCES := ./dm_utils.c \
./dm_conn.c \
./dm_cm_wrapper.c \
./dm_message_cache.c \
./dm_ipc.c \
./dm_tsl_alink.c \
./linkkit_solo_legacy.c \
./dm_shadow.c \
./dm_cota.c \
./dm_manager.c \
./dm_api.c \
./dm_message.c \
./dm_opt.c \
./dm_ota.c \
./dm_dispatch.c \
./dm_subscribe.c \
./dm_fota.c \
./linkkit_gateway_legacy.c \
 
$(NAME)_COMPONENTS := middleware/linkkit/sdk-c/src/infra/utils \
middleware/linkkit/sdk-c/src/infra/log \
middleware/linkkit/sdk-c/src/infra/system \
middleware/linkkit/sdk-c/src/sdk-impl \


$(NAME)_INCLUDES := ../../../../src/infra \
../../../../src/infra/log \
../../../../src/infra/utils \
../../../../src/infra/utils/misc \
../../../../src/infra/utils/digest \
../../../../src/infra/system \
q../../../../src/services/linkkit/cm \
../../../../src/services/linkkit/cm/example \
 q. \
./example \

