NAME := kspace

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION := 1.0.1
$(NAME)_SUMMARY := syscall micro kernel

$(NAME)_COMPONENTS += ksyscall
$(NAME)_COMPONENTS += mksh
$(NAME)_COMPONENTS += core_ipc

ifeq ($(COMPILER),)
$(NAME)_CFLAGS += -Wall
else ifeq ($(COMPILER),gcc)
$(NAME)_CFLAGS += -Wall
endif

$(NAME)_SOURCES := core_src/utask.c
$(NAME)_SOURCES += core_src/task_group.c
$(NAME)_SOURCES += core_src/ucli.c
$(NAME)_SOURCES += core_src/res.c
$(NAME)_SOURCES += core_src/fsfd.c
$(NAME)_SOURCES += core_src/ipc.c
$(NAME)_SOURCES += core_src/socketfd.c
$(NAME)_SOURCES += core_src/callback.c
$(NAME)_SOURCES += core_src/uspace_init.c
$(NAME)_SOURCES += core_src/ash.c
ifeq ($(HOST_ARCH),Cortex-M33)
$(NAME)_PREBUILT_LIBRARY += lib/cortex-m33/usignal.a
else ifeq ($(HOST_ARCH),Cortex-A9)
$(NAME)_PREBUILT_LIBRARY += lib/cortex-a9/usignal.a
endif

$(NAME)_INCLUDES += ../uspace/include
GLOBAL_INCLUDES += ./include
GLOBAL_INCLUDES += ../syscall/include

$(NAME)_CFLAGS += -Werror
