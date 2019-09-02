NAME := rbtree_test

$(NAME)_MBINS_TYPE := app
$(NAME)_VERSION    := 0.0.1
$(NAME)_SUMMARY    := testcase for rbtree
$(NAME)_COMPONENTS += lib_rbtree

$(NAME)_SOURCES += rbtree_test.c

$(NAME)_CFLAGS += -Wall -Werror

