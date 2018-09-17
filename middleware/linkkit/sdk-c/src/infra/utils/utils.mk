NAME := libiot_utils

$(NAME)_SOURCES := ./misc/linked_list.c \
./misc/utils_epoch_time.c \
./misc/utils_event.c \
./misc/utils_httpc.c \
./misc/mem_stats.c \
./misc/utils_list.c \
./misc/string_utils.c \
./misc/json_parser.c \
./misc/json_token.c \
./misc/utils_timer.c \
./misc/lite-cjson.c \
./misc/utils_net.c \
./digest/utils_sha1.c \
./digest/utils_base64.c \
./digest/utils_sha256.c \
./digest/utils_md5.c \
./digest/utils_hmac.c \
 
$(NAME)_COMPONENTS := 

$(NAME)_INCLUDES := ../../../src/infra/log \
./misc \
./digest \

