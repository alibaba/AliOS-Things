NAME := arch_risc_v32I

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION    := 1.0.2
$(NAME)_SUMMARY    := arch for risc_v32I

$(NAME)_SOURCES := common/panic_c.c
$(NAME)_SOURCES += common/port_c.c
$(NAME)_SOURCES += gcc/port_s.S

GLOBAL_INCLUDES += include

