NAME := yloop

$(NAME)_COMPONENTS := log rhino.vfs

$(NAME)_TYPE := kernel
$(NAME)_MBINS_TYPE := kernel

$(NAME)_SOURCES     := yloop.c
$(NAME)_SOURCES     += local_event.c
$(NAME)_SOURCES     += select.c
$(NAME)_SOURCES     += device.c

#default gcc
ifeq ($(COMPILER),)
$(NAME)_CFLAGS      += -Wall -Werror
else ifeq ($(COMPILER),gcc)
$(NAME)_CFLAGS      += -Wall -Werror
endif

GLOBAL_DEFINES      += AOS_LOOP
GLOBAL_INCLUDES     += include

ifeq ($(COMPILER),armcc)
	$(NAME)_LINK_FILES := local_event.o
endif

ifeq ($(COMPILER),rvct)
	$(NAME)_LINK_FILES := local_event.o
endif
