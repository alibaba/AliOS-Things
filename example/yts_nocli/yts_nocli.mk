NAME := yts_nocli

$(NAME)_SOURCES := main.c
$(NAME)_COMPONENTS := testcase testcase.certificate_test rhino.test log vfs yloop hal

$(NAME)_CFLAGS += -Wall -Werror -Wno-unused-variable

CONFIG_SYSINFO_APP_VERSION = APP-1.0.0-$(CURRENT_TIME)
$(info app_version:${CONFIG_SYSINFO_APP_VERSION})
GLOBAL_CFLAGS += -DSYSINFO_APP_VERSION=\"$(CONFIG_SYSINFO_APP_VERSION)\"
