NAME := uagent_test

$(NAME)_MBINS_TYPE := app
$(NAME)_VERSION    := 0.0.1
$(NAME)_SUMMARY    := testcase for uagent
$(NAME)_COMPONENTS += uagent netmgr cli cjson libiot_mqtt

$(NAME)_SOURCES += uagent_test.c

#$(NAME)_CFLAGS += -Wall -Werror

