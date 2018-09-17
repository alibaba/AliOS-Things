NAME := newlib_stub

ifeq ($(COMPILER),armcc)
$(NAME)_TYPE := share
$(NAME)_SOURCES := compilers/armlibc/armcc_libc.c
GLOBAL_INCLUDES += compilers/armlibc 
else ifeq ($(COMPILER),rvct)
$(NAME)_TYPE := share
$(NAME)_SOURCES := compilers/armlibc/armcc_libc.c
GLOBAL_INCLUDES += compilers/armlibc 
else ifeq ($(COMPILER),iar)
$(NAME)_TYPE := share
GLOBAL_INCLUDES += compilers/iar
$(NAME)_SOURCES := compilers/iar/iar_libc.c
else ifeq ($(HOST_MCU_FAMILY),rda8955)
$(NAME)_TYPE := share
$(NAME)_MBINS_TYPE := share
$(NAME)_SOURCES := mips_rda_stub.c
else ifeq ($(HOST_MCU_FAMILY),freedom-e.e310)
$(NAME)_TYPE := share
$(NAME)_MBINS_TYPE := share
$(NAME)_SOURCES := newlib_riscv_stub.c
else ifneq ($(HOST_MCU_FAMILY),linux)
$(NAME)_TYPE := share
$(NAME)_MBINS_TYPE := share
$(NAME)_SOURCES := newlib_stub.c
endif

ifeq ($(IDE),keil)
    $(NAME)_SOURCES += compilers/armlibc/hal_stub.c
endif

