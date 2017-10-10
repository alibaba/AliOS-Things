NAME := yloop

$(NAME)_COMPONENTS := log vfs

$(NAME)_TYPE := kernel
$(NAME)_SOURCES     := yloop.c
$(NAME)_SOURCES     += local_event.c

$(NAME)_CFLAGS      += -Wall -Werror
GLOBAL_DEFINES      += AOS_LOOP
