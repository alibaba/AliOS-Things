NAME := certificate_test

$(NAME)_SOURCES := aos_test.c cutest/cut.c posix_test.c
$(NAME)_COMPONENTS := rhino
$(NAME)_COMPONENTS += rhino.fs.kv
$(NAME)_COMPONENTS += yloop

$(NAME)_CFLAGS += -Wall -Werror -Wno-unused-varible
