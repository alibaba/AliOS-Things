LIBA_TARGET := libiot_platform.a
HDR_REFS    += src/sdk-impl
HDR_REFS    += src/thirdparty/tls
HDR_REFS    += src/thirdparty/lib/common
HDR_REFS    += src/utils
HDR_REFS    += src/platform
CFLAGS      := $(filter-out -ansi,$(CFLAGS))
DEPENDS     += src/thirdparty/tls

ifneq ($(CONFIG_VENDOR),rhino)
 LIB_SRCS_PATTERN    += os/$(CONFIG_VENDOR)/*.c
endif
LIB_SRCS_PATTERN    += ssl/mbedtls/*.c
LIB_SRCS_PATTERN    += ssl/itls/*.c

define Extra_CMake_Head
    echo 'IF (WIN32)' $(1)
    echo '    SET (OS_DIR win7)' $(1)
    echo 'ELSE (WIN32)' $(1)
    echo '    SET (OS_DIR ubuntu)' $(1)
    echo 'ENDIF (WIN32)' $(1)
    echo 'FILE (GLOB EXTRA_C_SOURCES $${PROJECT_SOURCE_DIR}/$(MODULE_NAME)/os/$${OS_DIR}/*.c)' $(1)
    echo '' $(1)
endef
