NAME := yloop

$(NAME)_COMPONENTS := log vfs

$(NAME)_TYPE := kernel
$(NAME)_MBINS_TYPE := kernel

$(NAME)_SOURCES     := yloop.c
$(NAME)_SOURCES     += local_event.c

#default gcc
ifeq ($(COMPILER),)
$(NAME)_CFLAGS      += -Wall -Werror
else ifeq ($(COMPILER),gcc)
$(NAME)_CFLAGS      += -Wall -Werror
endif

GLOBAL_DEFINES      += AOS_LOOP

ifeq ($(COMPILER),armcc)
	$(NAME)_LINK_FILES := local_event.o
endif	