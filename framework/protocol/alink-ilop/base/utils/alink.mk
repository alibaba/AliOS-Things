LIBA_TARGET                 := libilop-utils.a libilop-utils-test.a
LIBA_SKIP_COMBO             := libilop-utils-test.a

HDR_REFS                    := sdk-encap base/log
PKG_SOURCE                  := LITE-utils.git
PKG_UPSTREAM                := git@gitlab.alibaba-inc.com:iot-middleware/LITE-utils.git
EXTRA_SRCS                  := base/utils/ilop_utils_config.h

CFLAGS += \
    -DLITE_UTILS_CONFIG_H=\"ilop_utils_config.h\" \
    -DUTILS_printf=HAL_Printf \

LIB_SRCS_PATTERN_ilop-utils         := */*.c
LIB_SRCS_PATTERN_ilop-utils         += LITE-utils/src/*.c

LIB_SRCS_PATTERN_ilop-utils-test    := LITE-utils/testsuites/ut_*.c
