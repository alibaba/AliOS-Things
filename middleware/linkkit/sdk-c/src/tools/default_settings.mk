DEFAULT_BLD             ?= $(CONFIG_DIR)/config.ubuntu.x86
POST_ENV_HOOK           := Post_Env
POST_FINAL_OUT_HOOK     := Post_Distro
SUBDIRS                 := directory-not-exist-actually
UTEST_PROG              := sdk-testsuites
ifeq (Darwin,$(shell uname))
POST_FINAL_OUT_HOOK     :=
endif

CONFIG_LIB_EXPORT               ?= static

CFLAGS  += -Iexamples -Iexamples/linkkit -Itests

CFLAGS  += -DFORCE_SSL_VERIFY
CFLAGS  += -DCOAP_SERV_MULTITHREAD

#
# Hacks for cmake sub-system
#
EXTRA_INSTALL_HDRS  := \
    $(EXPORT_DIR)/iot_import.h \
    $(EXPORT_DIR)/iot_export.h \
    $(EXPORT_DIR)/imports \
    $(EXPORT_DIR)/exports \

EXTRA_INCLUDE_DIRS  := \
    $(EXPORT_DIR) \
    src/ref-impl/tls \
    src/security/pro \
    src/tools/linkkit_tsl_convert/include \
    tests/external/utils \
    build-rules \
    src/ref-impl/hal \

WIN32_CMAKE_SKIP    := \
    linkkit_tsl_convert \
    coap-example \

NOEXEC_CMAKE_DIRS   := tests src/services/linkkit/dm

