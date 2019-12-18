NAME := lwm2m_yts_test

$(NAME)_MBINS_TYPE := app
$(NAME)_VERSION    := 0.0.1
$(NAME)_SUMMARY    := YTS testcase for lwm2m
$(NAME)_COMPONENTS += lwm2m

$(NAME)_SOURCES += lwm2m_yts_test.c

$(NAME)_CFLAGS += -Wall -Werror

