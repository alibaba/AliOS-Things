NAME := newlib_stub

$(NAME)_VERSION := 1.0.0
$(NAME)_SUMMARY := libc adaptation layer
$(NAME)_MBINS_TYPE := share

$(NAME)_COMPONENTS += vfs

ifeq ($(COMPILER),armcc)
ifeq ($(MBINS),app)
ifeq ($(ENABLE_USPACE),1)
$(NAME)_SOURCES := compilers/armlibc/armcc_libc_uspace.c
else
$(NAME)_SOURCES := compilers/armlibc/armcc_libc.c
endif
else
$(NAME)_SOURCES := compilers/armlibc/armcc_libc.c
endif
GLOBAL_INCLUDES += compilers/armlibc
else ifeq ($(COMPILER),rvct)
$(NAME)_SOURCES := compilers/armlibc/armcc_libc.c
GLOBAL_INCLUDES += compilers/armlibc
else ifeq ($(COMPILER),iar)
ifeq ($(MBINS),app)
ifeq ($(ENABLE_USPACE),1)
$(NAME)_SOURCES := compilers/iar/iar_libc_uspace.c
else
$(NAME)_SOURCES := compilers/iar/iar_libc.c
endif
else
$(NAME)_SOURCES := compilers/iar/iar_libc.c
endif
GLOBAL_INCLUDES += compilers/iar
else ifeq ($(HOST_MCU_FAMILY),mcu_rda8955)
$(NAME)_SOURCES := mips_rda_stub.c
else ifeq ($(HOST_MCU_FAMILY),mcu_e310)
$(NAME)_SOURCES := newlib_riscv_stub.c
else ifneq ($(HOST_MCU_FAMILY),mcu_linuximpl)
ifeq ($(MBINS),app)
ifeq ($(ENABLE_USPACE),1)
$(NAME)_SOURCES := newlib_stub_uspace.c
else
$(NAME)_SOURCES := newlib_stub.c
endif
else
$(NAME)_SOURCES := newlib_stub.c
endif
endif

ifeq ($(IDE),keil)
    $(NAME)_SOURCES += compilers/armlibc/hal_stub.c
endif

