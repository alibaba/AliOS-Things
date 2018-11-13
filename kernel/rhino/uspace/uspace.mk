NAME := uspace

$(NAME)_MBINS_TYPE := share
$(NAME)_VERSION := 0.0.1
$(NAME)_SUMMARY :=

ifeq ($(COMPILER),)
$(NAME)_CFLAGS += -Wall -Werror
else ifeq ($(COMPILER),gcc)
$(NAME)_CFLAGS += -Wall -Werror
endif

ifeq ($(ENABLE_USPACE),1)
ifeq ($(MBINS),kernel)
$(NAME)_SOURCES    := u_task.c
$(NAME)_SOURCES    += u_proc_msg.c
$(NAME)_COMPONENTS += rhino.uspace.syscall.ksyscall
else ifeq ($(MBINS),app)
$(NAME)_SOURCES    := u_timer.c
$(NAME)_SOURCES    += u_mm.c
$(NAME)_COMPONENTS += rhino.uspace.syscall.usyscall
GLOBAL_DEFINES     += AOS_UMM
endif
GLOBAL_INCLUDES += ./include
endif

