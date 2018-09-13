CONFIG_ENV_CFLAGS   += \
    -Os -Wall \
    -DCONFIG_HTTP_AUTH_TIMEOUT=500 \
    -DCONFIG_MID_HTTP_TIMEOUT=500 \
    -DCONFIG_GUIDER_AUTH_TIMEOUT=500 \

CONFIG_src/services/uota :=
CONFIG_src/services/awss :=

CONFIG_ENV_CFLAGS   += -Werror

LDFLAGS             += -lpthread

OVERRIDE_STRIP      := strip

