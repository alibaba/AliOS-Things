LIBA_TARGET := libilop-tls.a
CFLAGS      := $(filter-out -ansi,$(CFLAGS))

LIB_HEADERS := $(wildcard $(TOP_DIR)/$(MODULE_NAME)/mbedtls/include/mbedtls/*.h)

PKG_SOURCE  := mbedtls.git
