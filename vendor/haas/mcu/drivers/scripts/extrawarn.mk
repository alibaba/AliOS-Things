cur_makefile := $(lastword $(MAKEFILE_LIST))

$(cur_makefile): ;

# ==========================================================================
#
# make W=... settings
#
# W=1 - warnings that may be relevant and does not occur too often
# W=2 - warnings that occur quite often but may still be relevant
# W=3 - the more obscure warnings, can most likely be ignored
#
# $(call cc-option, -W...) handles gcc -W.. options which
# are not supported by all versions of the compiler
# ==========================================================================

ifeq ("$(origin W)","command line")
  export KBUILD_ENABLE_EXTRA_GCC_CHECKS := $(W)
endif

ifdef KBUILD_ENABLE_EXTRA_GCC_CHECKS
warning-  := $(empty)

warning-1 := -Wextra -Wunused -Wno-unused-parameter
warning-1 += -Wmissing-declarations
warning-1 += -Wmissing-format-attribute
warning-1 += -Wmissing-include-dirs
warning-1 += -Wunused-but-set-variable
warning-1 += -Wno-missing-field-initializers
c_warning-1 := -Wmissing-prototypes -Wold-style-definition

warning-2 := -Waggregate-return
warning-2 += -Wcast-align
warning-2 += -Wdisabled-optimization
warning-2 += -Wshadow
warning-2 += -Wlogical-op
warning-2 += -Wmissing-field-initializers
c_warning-2 := -Wnested-externs

warning-3 := -Wcast-qual
warning-3 += -Wconversion
warning-3 += -Wpacked
warning-3 += -Wpadded
warning-3 += -Wpointer-arith
warning-3 += -Wredundant-decls
warning-3 += -Wswitch-default
warning-3 += -Wpacked-bitfield-compat
warning-3 += -Wvla
c_warning-3 := -Wbad-function-cast

warning := $(warning-$(findstring 1, $(KBUILD_ENABLE_EXTRA_GCC_CHECKS)))
warning += $(warning-$(findstring 2, $(KBUILD_ENABLE_EXTRA_GCC_CHECKS)))
warning += $(warning-$(findstring 3, $(KBUILD_ENABLE_EXTRA_GCC_CHECKS)))

c_warning := $(warning-$(findstring 1, $(KBUILD_ENABLE_EXTRA_GCC_CHECKS)))
c_warning += $(warning-$(findstring 2, $(KBUILD_ENABLE_EXTRA_GCC_CHECKS)))
c_warning += $(warning-$(findstring 3, $(KBUILD_ENABLE_EXTRA_GCC_CHECKS)))

ifeq ("$(strip $(warning) $(c_warning))","")
        $(error W=$(KBUILD_ENABLE_EXTRA_GCC_CHECKS) is unknown)
endif

KBUILD_CFLAGS += $(warning)
C_ONLY_FLAGS += $(c_warning)
else

ifeq ($(COMPILER),clang)
KBUILD_CFLAGS += -Wno-initializer-overrides
KBUILD_CFLAGS += -Wno-unused-value
KBUILD_CFLAGS += -Wno-format
KBUILD_CFLAGS += -Wno-unknown-warning-option
KBUILD_CFLAGS += -Wno-sign-compare)
KBUILD_CFLAGS += -Wno-format-zero-length
KBUILD_CFLAGS += -Wno-uninitialized
endif
endif
