#
# Support for menuconfig
#

export AOS_CONFIG := $(SOURCE_ROOT).config
export AOS_CONFIG_IN := build/Config.in
export AOS_CONFIG_DIR := $(BUILD_DIR)/config

export KCONFIG_DIR := $(SOURCE_ROOT)build/kconfig
export KCONFIG_MCONF := $(KCONFIG_DIR)/kconfig-mconf
export KCONFIG_CONF := $(KCONFIG_DIR)/kconfig-conf

export SYSCONFIG_INTERNAL_H := $(SOURCE_ROOT)build/configs/sysconfig.internal.h
export SYSCONFIG_H := $(SOURCE_ROOT)build/configs/sysconfig.h

ifeq ($(HOST_OS),Linux64)
KCONFIG_URL := https://gitee.com/alios-things/kconfig-frontends-linux.git
endif

# Don't read in .config for these targets
noconfig_targets := menuconfig oldconfig silentoldconfig olddefconfig
.PHONY: $(noconfig_targets)

MAKEFILE_TARGETS += $(noconfig_targets)

# Pull in user's configuration file
ifeq ($(filter $(noconfig_targets),$(MAKECMDGOALS)),)
-include $(AOS_CONFIG)
endif

# Use -include for GCC and --preinclude for other ARM compilers
INCLUDE_OPTS = -include
ifneq (, $(filter iar keil rvct, $(IDE)))
INCLUDE_OPTS = --preinclude
endif

INCLUDE_AUTOCONF_H = $(if $(wildcard $(AOS_CONFIG_DIR)/autoconf.h), $(INCLUDE_OPTS) $(AOS_CONFIG_DIR)/autoconf.h)
INCLUDE_SYSCONFIG_H = $(if $(wildcard $(SYSCONFIG_INTERNAL_H)), $(INCLUDE_OPTS) $(SYSCONFIG_INTERNAL_H), $(if $(wildcard $(SYSCONFIG_H)), $(INCLUDE_OPTS) $(SYSCONFIG_H)))

COMMON_CONFIG_ENV = \
	KCONFIG_CONFIG=$(AOS_CONFIG) \
	KCONFIG_AUTOCONFIG=$(AOS_CONFIG_DIR)/auto.conf \
	KCONFIG_AUTOHEADER=$(AOS_CONFIG_DIR)/autoconf.h \
	KCONFIG_TRISTATE=$(AOS_CONFIG_DIR)/tristate.conf

$(KCONFIG_MCONF) $(KCONFIG_CONF):
	@git clone $(KCONFIG_URL) $(KCONFIG_DIR)

menuconfig: $(KCONFIG_MCONF)
	@$(COMMON_CONFIG_ENV) $< $(AOS_CONFIG_IN)

oldconfig silentoldconfig olddefconfig: $(KCONFIG_CONF)
	@$(QUIET)$(call MKDIR, $(BUILD_DIR)/config)
	@$(COMMON_CONFIG_ENV) $< --$@ $(AOS_CONFIG_IN)

$(AOS_CONFIG_DIR)/auto.conf $(AOS_CONFIG_DIR)/autoconf.h: $(AOS_CONFIG)
	@$(MAKE) silentoldconfig
