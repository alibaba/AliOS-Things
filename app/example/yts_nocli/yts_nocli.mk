NAME := yts_nocli

#$(NAME)_SOURCES := main.c
#$(NAME)_COMPONENTS := testcase testcase.certificate_test rhino.test log rhino.vfs yloop hal

$(NAME)_SOURCES := main.c
$(NAME)_COMPONENTS := testcase testcase.certificate_test rhino.test log rhino.vfs yloop hal

$(NAME)_CFLAGS += -Wall -Werror -Wno-unused-variable
