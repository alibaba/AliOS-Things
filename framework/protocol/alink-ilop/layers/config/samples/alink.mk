CFLAGS              := $(filter-out -Werror,$(CFLAGS))
TARGET              := config_sample

SRCS_config_sample := config_sample.c

LDFLAGS             += \
    -lilop-sdk \
    -lilop-hal

HDR_REFS            += base sdk-encap layers/config/include
DEPENDS             += \
    hal-impl
