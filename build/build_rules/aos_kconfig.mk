#
# Support for menuconfig
#

ifneq (,$(APPDIR))
CONFIG_BASEDIR := $(APPDIR)/
export AOS_CONFIG_IN := $(APPDIR)/Config.in
else
CONFIG_BASEDIR :=
export AOS_CONFIG_IN := $(SOURCE_ROOT)build/Config.in
endif

export AOS_CONFIG := $(CONFIG_BASEDIR).config
export AOS_DEFCONFIG ?= $(CONFIG_BASEDIR).defconfig
export TMP_DEFCONFIG := $(CONFIG_BASEDIR).defconfig

export AOS_DEFCONFIG_DIR := $(SOURCE_ROOT)build/configs
export AOS_CONFIG_DIR := $(BUILD_DIR)/config

KCONFIG_TOOLPATH ?=
KCONFIG_DIR := $(SOURCE_ROOT)/build/kconfig/$(HOST_OS)/

CONVERT_AUTOCONF_AOSCONFIG := 1
CONVERT_AOSCONFIG_CONFIG := 2
CONVERT_AOSCONFIG_ASM := 3

ifneq (,$(wildcard $(KCONFIG_DIR)COPYING))
KCONFIG_TOOLPATH := $(KCONFIG_DIR)
endif

ifeq (,$(KCONFIG_TOOLPATH))
ifeq ($(HOST_OS),Win32)
SYSTEM_KCONFIG_TOOLPATH = $(shell where kconfig-mconf.exe)
else
SYSTEM_KCONFIG_TOOLPATH = $(shell which kconfig-mconf)
endif

ifeq (,$(findstring kconfig-mconf,$(SYSTEM_KCONFIG_TOOLPATH)))
KCONFIG_TOOLPATH := $(KCONFIG_DIR)
DOWNLOAD_KCONFIG = yes
endif

# Fix for which output: "no kconfig-mconf in ..."
ifneq (,$(findstring no kconfig-mconf in,$(SYSTEM_KCONFIG_TOOLPATH)))
KCONFIG_TOOLPATH := $(KCONFIG_DIR)
DOWNLOAD_KCONFIG = yes
endif
endif

KCONFIG_MCONF := $(KCONFIG_TOOLPATH)kconfig-mconf
KCONFIG_CONF := $(KCONFIG_TOOLPATH)kconfig-conf

ifeq ($(HOST_OS),Linux64)
KCONFIG_URL := https://gitee.com/alios-things/kconfig-frontends-linux.git
else ifeq ($(HOST_OS),OSX)
KCONFIG_URL := https://gitee.com/alios-things/kconfig-frontends-mac.git
else ifeq ($(HOST_OS),Win32)
export KCONFIG_MCONF := $(subst /,\,$(KCONFIG_MCONF)).bat
export KCONFIG_CONF := $(subst /,\,$(KCONFIG_CONF)).bat
export AOS_CONFIG_IN := $(subst ./,,$(AOS_CONFIG_IN))
export AOS_DEFCONFIG := $(subst ./,,$(AOS_DEFCONFIG))
export TMP_DEFCONFIG := $(subst ./,,$(TMP_DEFCONFIG))
KCONFIG_URL := https://gitee.com/alios-things/kconfig-frontends-win32.git
endif

export SYSCONFIG_H := $(SOURCE_ROOT)/build/configs/sysconfig.h

# Don't read in .config for these targets
noconfig_targets := menuconfig oldconfig silentoldconfig olddefconfig \
    defconfig savedefconfig %_defconfig list-defconfig alldefconfig

.PHONY: $(noconfig_targets)

MAKEFILE_TARGETS += $(noconfig_targets)
DEFCONFIG_FILES := $(notdir $(wildcard $(AOS_DEFCONFIG_DIR)/*_defconfig))

ifeq (yes,$(DOWNLOAD_KCONFIG))
menuconfig: $(KCONFIG_MCONF)
$(filter-out menuocnfig %_defconfig, $(noconfig_targets)): $(KCONFIG_CONF)
$(AOS_CONFIG) $(AOS_CONFIG_DIR)/auto.conf $(AOS_CONFIG_DIR)/autoconf.h: | $(KCONFIG_CONF)
$(DEFCONFIG_FILES): | $(KCONFIG_CONF)
endif

# Use -include for GCC and --preinclude for other ARM compilers
GCC_INCLUDE_AUTOCONF_H = $(if $(wildcard $(CONFIG_BASEDIR)aos_config.h), -include $(CONFIG_BASEDIR)aos_config.h)
ARMCC_INCLUDE_AUTOCONF_H = $(if $(wildcard $(CONFIG_BASEDIR)aos_config.h), --preinclude $(CONFIG_BASEDIR)aos_config.h)
# Use --cpreproc --cpreproc_opts=--preinclude,autoconf.h for armasm
ARMASM_INCLUDE_AUTOCONF_H = $(if $(wildcard $(CONFIG_BASEDIR)aos_config.h), --cpreproc --cpreproc_opts=--preinclude$(COMMA)$(CONFIG_BASEDIR)aos_config.h)
# Use -Dsymbol=\"value\" for iasmarm
IASMARM_INCLUDE_AUTOCONF_H = $(if $(wildcard $(CONFIG_BASEDIR)aos_config.h), $(shell $(PYTHON) $(SCRIPTS_PATH)/aos_autoconf_convert.py $(CONVERT_AOSCONFIG_ASM) $(CONFIG_BASEDIR)aos_config.h))

GCC_INCLUDE_SYSCONFIG_H = $($(if $(wildcard $(SYSCONFIG_H)), -include $(SYSCONFIG_H)))
ARMCC_INCLUDE_SYSCONFIG_H = $($(if $(wildcard $(SYSCONFIG_H)), --preinclude $(SYSCONFIG_H)))

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
	$(QUIET)$(PYTHON) $(SCRIPTS_PATH)/aos_download_tools.py $(KCONFIG_URL) $(KCONFIG_DIR)

menuconfig:
	$(QUIET)$(COMMON_CONFIG_ENV) $(KCONFIG_MCONF) $(AOS_CONFIG_IN)

oldconfig silentoldconfig olddefconfig:
	$(QUIET)$(call MKDIR, $(BUILD_DIR)/config)
	$(QUIET)$(COMMON_CONFIG_ENV) $(KCONFIG_CONF) --$@ $(AOS_CONFIG_IN)
	$(QUIET)$(PYTHON) $(SCRIPTS_PATH)/aos_autoconf_convert.py $(CONVERT_AUTOCONF_AOSCONFIG) $(AOS_CONFIG_DIR)/autoconf.h $(CONFIG_BASEDIR)aos_config.h

# Create .defconfig
$(TMP_DEFCONFIG):
	$(QUIET)$(ECHO) Creating $@ ...
	$(QUIET)$(if $(BOARD_DEFCONFIG),\
	$(info Reading defconfig from $(BOARD_DEFCONFIG) ...) $(CPDIR) $(BOARD_DEFCONFIG) $(TMP_DEFCONFIG))

	$(QUIET)$(if $(BOARD_DEFCONFIG),\
	$(if $(AOS_APP_TYPE),$(call WRITE_FILE_APPEND,$@,$(AOS_APP_TYPE)=y)),\
	$(if $(AOS_APP_TYPE),$(call WRITE_FILE_CREATE,$@,$(AOS_APP_TYPE)=y)))

	$(QUIET)$(call WRITE_FILE_APPEND,$@,$(AOS_APP_CONFIG)=y)
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
$(AOS_CONFIG):
	$(QUIET)$(call LOAD_DEFCONFIG,$(KCONFIG_CONF),$(TMP_DEFCONFIG))
	$(QUIET)$(PYTHON) $(SCRIPTS_PATH)/aos_check_config.py $(AOS_CONFIG) $(BUILD_STRING)

ifneq ($(filter %.config %.menuconfig, $(MAKECMDGOALS)),)
$(AOS_CONFIG): $(TMP_DEFCONFIG)
endif

defconfig:
	$(QUIET)$(call LOAD_DEFCONFIG,$(KCONFIG_CONF),$(AOS_DEFCONFIG))

alldefconfig:
	$(QUIET)$(COMMON_CONFIG_ENV) $(KCONFIG_CONF) --alldefconfig $(AOS_CONFIG_IN)

savedefconfig:
	$(QUIET)$(COMMON_CONFIG_ENV) $(KCONFIG_CONF) \
		--savedefconfig=$(if $(AOS_DEFCONFIG),$(AOS_DEFCONFIG),$(AOS_CONFIG_DIR)/defconfig) \
		$(AOS_CONFIG_IN)

$(DEFCONFIG_FILES):
	$(QUIET)$(COMMON_CONFIG_ENV) $(KCONFIG_CONF) --defconfig=$(AOS_DEFCONFIG_DIR)/$@ $(AOS_CONFIG_IN)

ECHO_DEFCONFIG = " $(1)\n"
list-defconfig:
	$(QUIET)$(ECHO) ""
	$(QUIET)$(ECHO) "Valid defconfigs:"
	$(QUIET)$(ECHO) " "$(foreach defconfig,$(DEFCONFIG_FILES),$(call ECHO_DEFCONFIG,$(defconfig)))

$(AOS_CONFIG_DIR)/auto.conf $(AOS_CONFIG_DIR)/autoconf.h: $(AOS_CONFIG)
	$(QUIET)$(ECHO) Creating $@ ...
	$(QUIET)$(call MKDIR, $(BUILD_DIR)/config)
	$(QUIET)$(COMMON_CONFIG_ENV) $(KCONFIG_CONF) --silentoldconfig $(AOS_CONFIG_IN)
	$(QUIET)$(PYTHON) $(SCRIPTS_PATH)/aos_autoconf_convert.py $(CONVERT_AUTOCONF_AOSCONFIG) $(AOS_CONFIG_DIR)/autoconf.h $(CONFIG_BASEDIR)aos_config.h
