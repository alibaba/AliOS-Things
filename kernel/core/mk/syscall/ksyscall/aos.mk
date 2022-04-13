NAME := ksyscall

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION := 1.0.1
$(NAME)_SUMMARY := kernel syscall

ifeq ($(COMPILER),)
$(NAME)_CFLAGS += -Wall
else ifeq ($(COMPILER),gcc)
$(NAME)_CFLAGS += -Wall
endif

ifeq ($(AOS_COMP_IRQ),y)
$(NAME)_SOURCES += irq_ksyscall.c
ifeq ($(HOST_ARCH),Cortex-M3)
$(NAME)_SOURCES += irq_ksyscall_nvic.c
else ifeq ($(HOST_ARCH),Cortex-M4)
$(NAME)_SOURCES += irq_ksyscall_nvic.c
else ifeq ($(HOST_ARCH),Cortex-M33)
$(NAME)_SOURCES += irq_ksyscall_nvic.c
else ifeq ($(HOST_ARCH),Cortex-M7)
$(NAME)_SOURCES += irq_ksyscall_nvic.c
endif

ifeq ($(HOST_ARCH),Cortex-A7)
$(NAME)_SOURCES += irq_ksyscall_gic.c
else ifeq ($(HOST_ARCH),Cortex-A9)
$(NAME)_SOURCES += irq_ksyscall_gic.c
endif
endif

$(NAME)_SOURCES-$(RHINO_CONFIG_DYN_SYSCALL_TBL) += dyn_syscall.c

$(NAME)_SOURCES-$(AOS_COMP_CLI)    += cli_ksyscall.c
$(NAME)_SOURCES-$(AOS_COMP_KV)     += kv_ksyscall.c
$(NAME)_SOURCES-$(AOS_COMP_KV)     += vfs_ksyscall.c
$(NAME)_SOURCES-$(AOS_COMP_EPOLL)  += epoll_ksyscall.c
$(NAME)_SOURCES-$(AOS_COMP_SELECT) += select_ksyscall.c
$(NAME)_SOURCES-$(AOS_COMP_NETMGR) += netmgr_ksyscall.c
$(NAME)_SOURCES-$(RHINO_CONFIG_USIGNAL) += signal_ksyscall.c

$(NAME)_SOURCES += network_ksyscall.c
$(NAME)_SOURCES += aos_iomap_ksyscall.c
$(NAME)_SOURCES += aos_obj_ksyscall.c
$(NAME)_SOURCES += rhino_ksyscall.c
$(NAME)_SOURCES += aos_ksyscall.c
$(NAME)_SOURCES += syscall_tbl.c
$(NAME)_SOURCES += debug_ksyscall.c
$(NAME)_SOURCES += hal_uart_ksyscall.c
$(NAME)_SOURCES += hal_flash_ksyscall.c
$(NAME)_SOURCES += hal_wdg_ksyscall.c

$(NAME)_INCLUDES += ../include include

