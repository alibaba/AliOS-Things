NAME := libiot_mal

$(NAME)_SOURCES := ./mal.c \
 
$(NAME)_COMPONENTS := middleware/linkkit/sdk-c/src/infra/utils \
middleware/linkkit/sdk-c/src/infra/log \
middleware/linkkit/sdk-c/src/infra/system \
middleware/linkkit/sdk-c/src/sdk-impl \
middleware/linkkit/sdk-c/src/services/mdal/mal/ica

$(NAME)_INCLUDES := . \
    middleware/linkkit/sdk-c/src/infra/system \

