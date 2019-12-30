NAME := yloop

$(NAME)_COMPONENTS := vfs

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION := 1.0.1
$(NAME)_SUMMARY := yloop provides the event schedule mechanism for resource constrained devices.

$(NAME)_SOURCES     := src/yloop.c
$(NAME)_SOURCES     += src/local_event.c
$(NAME)_SOURCES     += src/select.c
$(NAME)_SOURCES     += src/device.c
$(NAME)_SOURCES     += src/workqueue.c

#default gcc
ifeq ($(COMPILER),)
$(NAME)_CFLAGS      += -Wall -Werror
else ifeq ($(COMPILER),gcc)
$(NAME)_CFLAGS      += -Wall -Werror
endif

GLOBAL_DEFINES      += AOS_LOOP
$(NAME)_INCLUDES     += include
$(NAME)_INCLUDES     += hal

ifeq ($(COMPILER),armcc)
	$(NAME)_LINK_FILES := src/local_event.o
endif

ifeq ($(COMPILER),rvct)
	$(NAME)_LINK_FILES := src/local_event.o
endif

ifeq ($(osal), rhino)
$(NAME)_SOURCES += hal/loop_hal_alios.c
endif
