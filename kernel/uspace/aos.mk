NAME := uspace

$(NAME)_MBINS_TYPE := share
$(NAME)_VERSION := 1.0.0
$(NAME)_SUMMARY := syscall uprocess utask utimer IPC

ifeq ($(COMPILER),)
$(NAME)_CFLAGS += -Wall -Werror
else ifeq ($(COMPILER),gcc)
$(NAME)_CFLAGS += -Wall -Werror
endif

ifeq ($(ENABLE_USPACE),1)
ifeq ($(MBINS),kernel)
$(NAME)_SOURCES    := u_task.c
$(NAME)_SOURCES    += u_proc_msg.c
$(NAME)_SOURCES    += syscall/ksyscall/rhino_ksyscall.c
$(NAME)_SOURCES    += syscall/ksyscall/hal_uart_ksyscall.c
$(NAME)_SOURCES    += syscall/ksyscall/lwip_ksyscall.c
$(NAME)_SOURCES    += syscall/ksyscall/syscall_tbl.c

$(NAME)_INCLUDES   += syscall/ksyscall
else ifeq ($(MBINS),app)
$(NAME)_SOURCES    := u_timer.c
$(NAME)_SOURCES    += u_res_task.c
$(NAME)_SOURCES    += u_mm.c
$(NAME)_SOURCES    += syscall/usyscall/rhino_usyscall.c
$(NAME)_SOURCES    += syscall/usyscall/hal_uart_usyscall.c
$(NAME)_SOURCES    += syscall/usyscall/lwip_usyscall.c

GLOBAL_DEFINES += AOS_USPACE

AOS_VFS := 0

endif
GLOBAL_INCLUDES    += syscall/include
GLOBAL_INCLUDES    += ./include
endif

