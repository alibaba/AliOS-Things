#
# Support for menuconfig
#

export AOS_CONFIG := $(SOURCE_ROOT).config
export AOS_DEFCONFIG ?= $(SOURCE_ROOT).defconfig
export TMP_DEFCONFIG := $(SOURCE_ROOT).defconfig
export AOS_DEFCONFIG_DIR := $(SOURCE_ROOT)build/configs
export AOS_CONFIG_IN := $(SOURCE_ROOT)build/Config.in
export AOS_CONFIG_DIR := $(BUILD_DIR)/config

export KCONFIG_DIR := $(SOURCE_ROOT)build/kconfig
export KCONFIG_MCONF := $(KCONFIG_DIR)/kconfig-mconf
export KCONFIG_CONF := $(KCONFIG_DIR)/kconfig-conf

export SYSCONFIG_H := $(SOURCE_ROOT)build/configs/sysconfig.h

ifeq ($(HOST_OS),Linux64)
KCONFIG_URL := https://gitee.com/alios-things/kconfig-frontends-linux.git
else ifeq ($(HOST_OS),OSX)
KCONFIG_URL := https://gitee.com/alios-things/kconfig-frontends-mac.git
else ifeq ($(HOST_OS),Win32)
export KCONFIG_MCONF := $(subst /,\,$(KCONFIG_MCONF)).bat
export KCONFIG_CONF := $(subst /,\,$(KCONFIG_CONF)).bat
export AOS_CONFIG_IN := $(subst ./,,$(AOS_CONFIG_IN))
export AOS_DEFCONFIG := $(subst ./,,$(AOS_DEFCONFIG))
KCONFIG_URL := https://gitee.com/alios-things/kconfig-frontends-win32.git
endif

# Don't read in .config for these targets
noconfig_targets := menuconfig oldconfig silentoldconfig olddefconfig \
    defconfig savedefconfig %-defconfig list-defconfigs alldefconfig

.PHONY: $(noconfig_targets)

MAKEFILE_TARGETS += $(noconfig_targets)

# Pull in user's configuration file
#ifeq ($(filter $(noconfig_targets),$(MAKECMDGOALS)),)
#-include $(AOS_CONFIG)
#endif

# Use -include for GCC and --preinclude for other ARM compilers
INCLUDE_OPTS = -include
ifneq (, $(filter iar keil rvct, $(IDE)))
INCLUDE_OPTS = --preinclude
endif

INCLUDE_AUTOCONF_H = $(if $(wildcard $(AOS_CONFIG_DIR)/autoconf.h), $(INCLUDE_OPTS) $(AOS_CONFIG_DIR)/autoconf.h)
INCLUDE_SYSCONFIG_H = $($(if $(wildcard $(SYSCONFIG_H)), $(INCLUDE_OPTS) $(SYSCONFIG_H)))

ifneq ($(HOST_OS),Win32)
COMMON_CONFIG_ENV = \
	KCONFIG_CONFIG=$(AOS_CONFIG) \
	KCONFIG_AUTOCONFIG=$(AOS_CONFIG_DIR)/auto.conf \
	KCONFIG_AUTOHEADER=$(AOS_CONFIG_DIR)/autoconf.h \
	KCONFIG_TRISTATE=$(AOS_CONFIG_DIR)/tristate.conf
else
COMMON_CONFIG_ENV =
endif

$(KCONFIG_MCONF) $(KCONFIG_CONF):
	$(QUIET)git clone $(KCONFIG_URL) $(KCONFIG_DIR)

menuconfig: $(KCONFIG_MCONF)
	$(QUIET)$(COMMON_CONFIG_ENV) $< $(AOS_CONFIG_IN)

oldconfig silentoldconfig olddefconfig: $(KCONFIG_CONF)
	$(QUIET)$(call MKDIR, $(BUILD_DIR)/config)
	$(QUIET)$(COMMON_CONFIG_ENV) $< --$@ $(AOS_CONFIG_IN)

# Create .defconfig
$(TMP_DEFCONFIG):
	$(QUIET)$(ECHO) Creating $@ ...
	$(QUIET)$(if $(BOARD_DEFCONFIG),\
	$(info Reading defconfig from $(BOARD_DEFCONFIG) ...) $(CPDIR) $(BOARD_DEFCONFIG) $(TMP_DEFCONFIG))

	$(QUIET)$(if $(BOARD_DEFCONFIG),\
	$(if $(AOS_APP_TYPE),$(call WRITE_FILE_APPEND,$@,$(AOS_APP_TYPE)=y)),\
	$(if $(AOS_APP_TYPE),$(call WRITE_FILE_CREATE,$@,$(AOS_APP_TYPE)=y)))

	$(QUIET)$(call WRITE_FILE_APPEND,$@,AOS_app_$(AOS_APP_CONFIG)=y)
	$(QUIET)$(call WRITE_FILE_APPEND,$@,$(AOS_BOARD_CONFIG)=y)

ifneq ($(TMP_DEFCONFIG),$(AOS_DEFCONFIG))
$(TMP_DEFCONFIG): $(AOS_DEFCONFIG)
endif

#####################################################################################
# Macro LOAD_DEFCONFIG load default configs from TMP_DEFCONFIG:
# $(1) is command kconfig-conf
# $(2) is defconfig file
define LOAD_DEFCONFIG
$(QUIET)$(ECHO) Creating $(AOS_CONFIG) ...
$(QUIET)$(COMMON_CONFIG_ENV) $(1) --defconfig$(if $(2),=$(2)) $(AOS_CONFIG_IN)
endef

# Create .config
$(AOS_CONFIG): $(KCONFIG_CONF) $(TMP_DEFCONFIG)
	$(QUIET)$(call LOAD_DEFCONFIG, $<,$(TMP_DEFCONFIG))

defconfig: $(KCONFIG_CONF)
	$(QUIET)$(call LOAD_DEFCONFIG, $<,$(AOS_DEFCONFIG))

alldefconfig: $(KCONFIG_CONF)
	$(QUIET)$(COMMON_CONFIG_ENV) $< --alldefconfig $(AOS_CONFIG_IN)

savedefconfig: $(KCONFIG_CONF)
	$(QUIET)$(COMMON_CONFIG_ENV) $< \
		--savedefconfig=$(if $(AOS_DEFCONFIG),$(AOS_DEFCONFIG),$(AOS_CONFIG_DIR)/defconfig) \
		$(AOS_CONFIG_IN)

%-defconfig: $(KCONFIG_CONF)
	$(QUIET)$(COMMON_CONFIG_ENV) $< --defconfig=$(AOS_DEFCONFIG_DIR)/$@ $(AOS_CONFIG_IN)

ECHO_DEFCONFIG = " $(notdir $(1)):\tBuild for $(subst -defconfig,,$(notdir $(1)))\n"
list-defconfigs:
	$(QUIET)$(ECHO) ""
	$(QUIET)$(ECHO) "Valid defconfigs:"
	$(QUIET)$(ECHO) " "$(foreach defconfig,$(wildcard $(AOS_DEFCONFIG_DIR)/*-defconfig),$(call ECHO_DEFCONFIG,$(defconfig)))

$(AOS_CONFIG_DIR)/auto.conf $(AOS_CONFIG_DIR)/autoconf.h: $(KCONFIG_CONF) $(AOS_CONFIG)
	$(QUIET)$(ECHO) Creating $@ ...
	$(QUIET)$(call MKDIR, $(BUILD_DIR)/config)
	$(QUIET)$(COMMON_CONFIG_ENV) $< --silentoldconfig $(AOS_CONFIG_IN)
