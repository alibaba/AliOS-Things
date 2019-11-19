NAME := websoc_yts_test

$(NAME)_MBINS_TYPE := app
$(NAME)_VERSION    := 0.0.1
$(NAME)_SUMMARY    := YTS testcase for websoc
$(NAME)_COMPONENTS += websoc

$(NAME)_SOURCES += websoc_yts_test.c

$(NAME)_CFLAGS += -Wall -Werror

