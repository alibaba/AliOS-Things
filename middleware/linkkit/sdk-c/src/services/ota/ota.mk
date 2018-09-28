NAME := libiot_ota

$(NAME)_SOURCES := ota.c

$(NAME)_INCLUDES := .

$(NAME)_COMPONENTS := middleware/linkkit/sdk-c/src/infra/utils \
                      middleware/linkkit/sdk-c/src/infra/log \
                      middleware/linkkit/sdk-c/src/infra/system \
                      middleware/linkkit/sdk-c/src/services/ota \
