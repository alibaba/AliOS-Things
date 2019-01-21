


LIB_SRC := \
	Source/list.c \
	Source/event_groups.c \
	Source/list.c \
	Source/queue.c \
	Source/tasks.c \
	Source/timers.c \
	Source/timers_ext.c

LIB_ASRC :=

LIBRARY_NAME := freertos
LOCAL_CFLAGS +=
LOCAL_AFLAGS +=
LOCAL_INC +=


RELEASE_SRC := 2
$(eval $(call build-lib,$(LIBRARY_NAME),$(LIB_SRC),$(LIB_ASRC),$(LOCAL_CFLAGS),$(LOCAL_INC),$(LOCAL_AFLAGS),$(MYDIR)))

