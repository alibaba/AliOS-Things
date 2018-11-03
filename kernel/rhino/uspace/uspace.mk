NAME := uspace

$(NAME)_MBINS_TYPE := share

$(NAME)_INCLUDES := .

#default gcc
ifeq ($(COMPILER),)
$(NAME)_CFLAGS      += -Wall -Werror
else ifeq ($(COMPILER),gcc)
$(NAME)_CFLAGS      += -Wall -Werror
endif

ifeq ($(MBINS),kernel)
$(NAME)_SOURCES := u_task.c
else ifeq ($(MBINS),app)
$(NAME)_SOURCES := u_timer.c
$(NAME)_SOURCES += u_mm.c

GLOBAL_DEFINES += AOS_UMM
endif

GLOBAL_INCLUDES += ./include

GLOBAL_DEFINES +=

