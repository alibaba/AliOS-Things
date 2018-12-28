NAME := arch_cskyv2-l

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION    := 1.0.0
$(NAME)_SUMMARY    := arch for cskyv2-l

GLOBAL_ASMFLAGS += -ffunction-sections -fdata-sections
GLOBAL_CFLAGS   += -c -Wa,-melrw

ifeq ($(CONFIG_HARD_FLOAT), y)
GLOBAL_CFLAGS += -mhard-float
endif
BAL_CFLAGS    += $(CONFIG_DEBUG_LEVEL) $(CONFIG_OPTIMIZE_LEVEL) $(CONFIG_ENDIAN_MODE)
GLOBAL_CFLAGS += -Wall
GLOBAL_CFLAGS += -ffunction-sections
GLOBAL_CFLAGS += -fdata-sections
ifeq ($(CONFIG_HAVE_VIC), y)
GLOBAL_CFLAGS += -mistack
endif

GLOBAL_INCLUDES += .

$(NAME)_SOURCES += cpu_impl.c     \
                   port_s_novic.S \
                   port_c.c       \
                   entry.S
