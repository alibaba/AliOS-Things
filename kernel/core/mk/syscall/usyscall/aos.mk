NAME := usyscall

$(NAME)_MBINS_TYPE := app
$(NAME)_VERSION := 1.0.1
$(NAME)_SUMMARY := user space syscall

ifeq ($(COMPILER),)
$(NAME)_CFLAGS += -Wall
else ifeq ($(COMPILER),gcc)
$(NAME)_CFLAGS += -Wall
endif

# armcc compiler
ifeq ($(COMPILER),armcc)
ifeq ($(HOST_ARCH),Cortex-M0)
$(NAME)_SOURCES += svc_asm/armv6m_svc_armcc.s
else ifeq ($(HOST_ARCH),Cortex-M3)
$(NAME)_SOURCES += svc_asm/armv7m_svc_armcc.s
else ifeq ($(HOST_ARCH),Cortex-M4)
$(NAME)_SOURCES += svc_asm/armv7m_svc_armcc.s
endif

# iar compiler
else ifeq ($(COMPILER),iar)
ifeq ($(HOST_ARCH),Cortex-M0)
$(NAME)_SOURCES += svc_asm/armv6m_svc_iar.s
else ifeq ($(HOST_ARCH),Cortex-M3)
$(NAME)_SOURCES += svc_asm/armv7m_svc_iar.s
else ifeq ($(HOST_ARCH),Cortex-M4)
$(NAME)_SOURCES += svc_asm/armv7m_svc_iar.s
endif

# default gcc compiler
else
ifeq ($(HOST_ARCH),Cortex-M0)
$(NAME)_SOURCES += svc_asm/armv6m_svc_gcc.S
else ifeq ($(HOST_ARCH),Cortex-M3)
$(NAME)_SOURCES += svc_asm/armv7m_svc_gcc.S
else ifeq ($(HOST_ARCH),Cortex-M4)
$(NAME)_SOURCES += svc_asm/armv7m_svc_gcc.S
else ifeq ($(HOST_ARCH),Cortex-M33)
$(NAME)_SOURCES += svc_asm/armv8m_svc_gcc.S
else ifeq ($(HOST_ARCH),Cortex-A7)
$(NAME)_SOURCES += svc_asm/armv7a_svc_gcc.S
else ifeq ($(HOST_ARCH),Cortex-A9)
$(NAME)_SOURCES += svc_asm/armv7a_svc_gcc.S
endif

endif

$(NAME)_SOURCES-$(!AOS_COMP_LWIP_IN_SERVICE)    += lwip_usyscall.c
$(NAME)_SOURCES-$(!AOS_COMP_NETMGR_IN_SERVICE)  += netmgr_usyscall.c

$(NAME)_SOURCES += network_usyscall.c
$(NAME)_SOURCES += signal_usyscall.c
$(NAME)_SOURCES += cli_usyscall.c
$(NAME)_SOURCES += kv_usyscall.c
$(NAME)_SOURCES += vfs_usyscall.c
$(NAME)_SOURCES += epoll_usyscall.c
$(NAME)_SOURCES += select_usyscall.c
$(NAME)_SOURCES += irq_usyscall.c
$(NAME)_SOURCES += aos_iomap_usyscall.c
$(NAME)_SOURCES += aos_obj_usyscall.c
$(NAME)_SOURCES += rhino_usyscall.c
$(NAME)_SOURCES += aos_usyscall.c
$(NAME)_SOURCES += debug_usyscall.c
#$(NAME)_SOURCES += hal_uart_usyscall.c
$(NAME)_SOURCES-$(CONFIG_HAL_FLASH_SYSCALL) += hal_flash_usyscall.c
$(NAME)_SOURCES += hal_wdg_usyscall.c
$(NAME)_INCLUDES += ../include
GLOBAL_INCLUDES  += include
