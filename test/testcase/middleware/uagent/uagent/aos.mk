NAME := uagent_test

$(NAME)_MBINS_TYPE := app
$(NAME)_VERSION    := 0.0.1
$(NAME)_SUMMARY    := testcase for uagent
$(NAME)_COMPONENTS += uagent cjson linkkit_sdk_c

$(NAME)_SOURCES += uagent_test.c

#$(NAME)_CFLAGS += -Wall -Werror

