NAME := certificate_test

$(NAME)_SOURCES := certificate_test.c cutest/cut.c
$(NAME)_COMPONENTS := rhino
$(NAME)_COMPONENTS += modules.fs.kv
$(NAME)_COMPONENTS += yloop

$(NAME)_CFLAGS += -Wall -Werror -Wno-unused-varible
$(NAME)_CFLAGS += -DTEST_CONFIG_MALLOC_FREE_TIMES=100000
$(NAME)_CFLAGS += -DTEST_CONFIG_MAX_TASK_COUNT=10
$(NAME)_CFLAGS += -DTEST_CONFIG_CREATE_TASK_TIMES=10000
$(NAME)_CFLAGS += -DTEST_CONFIG_SYNC_TIMES=100000

ifneq (, $(filter modules.fs.kv, $($(NAME)_COMPONENTS)))
    $(NAME)_CFLAGS += -DTEST_CONFIG_KV_ENABLED
    $(NAME)_CFLAGS += -DTEST_CONFIG_KV_TIMES=10000
    $(info kv is enabled!)
endif

ifneq (, $(filter yloop, $($(NAME)_COMPONENTS)))
    $(NAME)_CFLAGS += -DTEST_CONFIG_YLOOP_ENABLED
    $(NAME)_CFLAGS += -DTEST_CONFIG_YLOOP_EVENT_COUNT=1000
    $(NAME)_CFLAGS += -DTEST_CONFIG_YLOOP_LOOP_COUNT=10
    $(info yloop is enabled!)
endif
