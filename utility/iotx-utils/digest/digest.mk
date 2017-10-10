NAME := libdigest

#$(NAME)_COMPONENTS := src
$(NAME)_INCLUDES :=  \
    ./ \
    ../LITE-log \
    ../sdk-impl
    

$(NAME)_SOURCES := \
    utils_base64.c \
    utils_hmac.c \
    utils_md5.c \
    utils_sha1.c

# TODO: fix warnings
$(NAME)_CFLAGS := $(filter-out -Werror,$(CFLAGS))

$(NAME)_DEFINES += DEBUG
