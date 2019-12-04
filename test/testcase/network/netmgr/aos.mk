NAME := netmgr_yts_test

$(NAME)_MBINS_TYPE := app
$(NAME)_VERSION    := 0.0.1
$(NAME)_SUMMARY    := YTS testcase for netmgr
$(NAME)_COMPONENTS += netmgr

$(NAME)_SOURCES += netmgr_yts_test.c

$(NAME)_CFLAGS += -Wall -Werror
