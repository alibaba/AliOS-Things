LIBA_TARGET := libiot_hal.a
CFLAGS      := $(filter-out -ansi,$(CFLAGS))

HDR_REFS    += src/ref-impl/tls
HDR_REFS    += src/infra
HDR_REFS    += src/security/pro

LIB_SRCS_PATTERN    += os/$(CONFIG_VENDOR)/*.c
ifneq (,$(filter -DSUPPORT_TLS,$(CFLAGS)))
LIB_SRCS_PATTERN    += ssl/mbedtls/*.c
endif
ifneq (,$(filter -DSUPPORT_ITLS,$(CFLAGS)))
LIB_SRCS_PATTERN    += ssl/itls/*.c
endif

define Extra_CMake_Head
    echo 'ADD_DEFINITIONS (-DDLL_HAL_EXPORTS)' $(1)
    echo '' $(1)
    echo 'IF (WIN32)' $(1)
    echo '    SET (OS_DIR win7)' $(1)
    echo 'ELSE (WIN32)' $(1)
    echo '    SET (OS_DIR ubuntu)' $(1)
    echo 'ENDIF (WIN32)' $(1)
    echo 'FILE (GLOB EXTRA_C_SOURCES $${PROJECT_SOURCE_DIR}/$(MODULE_NAME)/os/$${OS_DIR}/*.c)' $(1)
    echo '' $(1)
endef

define Extra_CMake_Foot
    echo 'TARGET_LINK_LIBRARIES (iot_hal iot_tls)' $(1)
    echo '' $(1)
    echo 'SET (LIBRARY_OUTPUT_PATH ../../../out)' $(1)
    echo '' $(1)
endef
