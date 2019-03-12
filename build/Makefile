default: all

unexport NAME # For compatibility with Windows Subsystem Linux

export SOURCE_ROOT ?= ./
export BUILD_DIR   ?= out
export MAKEFILES_PATH := $(SOURCE_ROOT)/build/build_rules
export SCRIPTS_PATH := $(SOURCE_ROOT)/build/scripts

# Specify build jobs
JOBS ?=4
ifeq (,$(SUB_BUILD))
JOBSNO := -j$(JOBS)
endif

MAKEFILE_TARGETS := clean distclean help # targets used by makefile

# define commands on different hosts
include $(MAKEFILES_PATH)/aos_host_cmd.mk
# kconfig support
include $(MAKEFILES_PATH)/aos_kconfig.mk

define CHECK_UCUBE_VERSION
$(if $(findstring 0.2,$(shell aos --version)),$(error aos-cube 0.3.x or newer is required, please update your aos-cube first! (eg. $$ aos upgrade)),)
endef

$(call CHECK_UCUBE_VERSION)

define NO_CONFIG_ERROR
$(info Please configure first:)
$(info *********************************************************************)
$(info $$ aos make <app>@<board> -c config)
$(info ---> Or more config: $$ aos make menuconfig)
$(info *********************************************************************)
$(error No Config Found!)
endef

define TARGET_DEPRECATED_ERROR
$(error Build Target Deprecated!)
endef

define BUILD_INCOMPATIBLE_ERROR
$(info *********************************************************************)
$(info The $(1) is incompatible with "aos make")
$(info )
$(info Please refer to its README and build it with proper tools)
$(info *********************************************************************)
$(error Build Incompatible!)
endef

all:
	$(QUIET)$(if $(wildcard $(AOS_CONFIG)),,$(call NO_CONFIG_ERROR))

#
# Process build strings
#
BUILD_SUBTARGETS := .config .menuconfig
MAKEFILE_TARGETS += export_linkkit_sdk restore_linkkit_sdk export-keil export-iar
export BUILD_STRING ?= $(strip $(filter-out $(MAKEFILE_TARGETS), $(MAKECMDGOALS)))

# Set IDE if export-* is specified
ifneq ($(filter export-keil, $(MAKECMDGOALS)),)
export IDE := keil
else ifneq ($(filter export-iar, $(MAKECMDGOALS)),)
export IDE := iar
endif

ifneq ($(filter %.config %.menuconfig, $(MAKECMDGOALS)),)
$(shell rm -rf .config .defconfig out/config)
endif

ifneq ($(filter menuconfig, $(MAKECMDGOALS)),)
export BUILD_STRING :=
else ifeq ($(BUILD_STRING),)
# Get BUILD_STRING from AOS_CONFIG
ifneq ($(wildcard $(AOS_CONFIG)),)
include $(AOS_CONFIG)
BUILD_STRING = $(strip $(subst @board_,@,$(subst ",,$(AOS_BUILD_APP)@$(AOS_BUILD_BOARD))))
endif
endif

# Strip subtargets from BUILD_STRING
$(foreach subtarget, $(BUILD_SUBTARGETS), $(eval BUILD_STRING := $(subst $(subtarget),,$(BUILD_STRING))))
subtarget := $(lastword $(subst @, ,$(BUILD_STRING)))
$(if $(findstring .,$(subtarget)),$(error Parameter for "-c" option Unknown "$(lastword $(subst ., ,$(subtarget)))"!),)
export CLEANED_BUILD_STRING := $(BUILD_STRING)

export OUTPUT_DIR  := $(BUILD_DIR)/$(CLEANED_BUILD_STRING)$(MBINS)
export CONFIG_FILE := $(OUTPUT_DIR)/config.mk
export AUTO_COMPONENT_DIR := $(OUTPUT_DIR)/auto_component

#
# PreCheck for build configs
#

# Enable .config and autoconf.h in build
ifeq ($(NOCONFIG),)

# Get AOS_BUILD_APP, AOS_BUILD_BOARD from BUILD_STRING for creating
# AOS_CONFIG if it is not existing
ifneq ($(BUILD_STRING),)

ifeq ($(wildcard $(AOS_CONFIG)),)
ifneq ($(findstring @, $(MAKECMDGOALS)),)
ifeq ($(filter %.config %.menuconfig, $(MAKECMDGOALS)),)
$(info )
$(info Build command updated, please follow steps:)
$(info *********************************************************************)
$(info Step 1: config:)
$(info $$ aos make $(BUILD_STRING) -c config)
$(info ---> Or more config: $$ aos make menuconfig)
$(info )
$(info Step 2: build:)
$(info $$ aos make    # VAR=value if needed)
$(info )
$(info ---> Or oneline command for config and build:)
$(info $$ aos make $(BUILD_STRING) -c config && aos make)
$(info *********************************************************************)
$(call TARGET_DEPRECATED_ERROR)
endif
endif

else
ifneq ($(findstring @, $(MAKECMDGOALS)),)
ifeq ($(filter %.config %.menuconfig, $(MAKECMDGOALS)),)
$(info )
$(info Config completed, please run build directly: $$ aos make)
$(info )
$(call TARGET_DEPRECATED_ERROR)
endif
endif
endif

ifeq ($(findstring @,$(BUILD_STRING)),)
$(error Unknown Target "$(BUILD_STRING)"!)
endif

components := $(subst @, ,$(BUILD_STRING))
ifneq ($(words $(components)),2)
$(error App/Board Unknown, Please check your config!)
endif

# get real app name and check if it available
appname = $(firstword $(components))
appdir = $(subst .,/,$(appname))
appmkfile = $(wildcard $(SOURCE_ROOT)app/*/$(appdir)/aos.mk $(SOURCE_ROOT)app/$(appdir)/aos.mk $(SOURCE_ROOT)test/develop/$(appdir)/aos.mk)
$(if $(appmkfile),\
    $(eval include $(appmkfile)) $(eval real_appname = $(NAME)),\
    $(error App Unknown, Please make sure the "$(appname)" is existing!))

# determine what app type should be enabled: example, profile, test
ifneq ($(findstring profile/,$(appmkfile)),)
export AOS_APP_TYPE := AOS_APP_PROFILE
else ifneq ($(findstring example/,$(appmkfile)),)
export AOS_APP_TYPE := AOS_APP_EXAMPLE
else ifneq ($(findstring develop/,$(appmkfile)),)
export AOS_APP_TYPE :=
else
export AOS_APP_TYPE := AOS_APP_LOCALAPP
endif

boardname = $(word 2, $(components))
boardmkfile = $(wildcard $(SOURCE_ROOT)board/$(boardname)/aos.mk $(SOURCE_ROOT)board/$(boardname)-*/aos.mk)
$(if $(boardmkfile),\
    $(eval include $(boardmkfile)) $(eval real_boardname = $(NAME)),\
    $(eval real_boardname = $(boardname)))

$(if $(filter no,$(AOS_BUILD_COMPATIBLE)), $(call BUILD_INCOMPATIBLE_ERROR,$(boardname)))
defconfig = $(wildcard $(SOURCE_ROOT)board/$(boardname)/defconfig)
ifneq ($(defconfig),)
export BOARD_DEFCONFIG := $(defconfig)
AOS_DEFCONFIG := $(BOARD_DEFCONFIG)
endif

export AOS_BOARD_CONFIG = AOS_BOARD_$(shell $(PYTHON) $(TR) $(subst -,_,$(subst board_,,$(real_boardname))))
export AOS_APP_CONFIG = AOS_APP_$(shell $(PYTHON) $(TR) $(subst -,_,$(subst app_,,$(real_appname))))

endif # $(BUILD_STRING)
endif # $(NOCONFIG)

# prebuild support
include $(MAKEFILES_PATH)/aos_prebuild.mk

# Support for MBINS build
export UNDERLINE
export RADIXPOINT

export MBINSTYPE_UPPER
export MBINSTYPE_LOWER
export BIN2BOOTTYPE_LOWER

MBINS ?=
MBINS_APP ?=
ifeq (,$(MBINS))
MBINSTYPE_UPPER :=
MBINSTYPE_LOWER :=
UNDERLINE :=
RADIXPOINT :=
else ifeq (app,$(MBINS))
MBINSTYPE_UPPER :=APP
MBINSTYPE_LOWER :=app
UNDERLINE :=_
RADIXPOINT :=.
else ifeq (kernel,$(MBINS))
MBINSTYPE_UPPER :=KERNEL
MBINSTYPE_LOWER :=kernel
UNDERLINE :=_
RADIXPOINT :=.
else
$(error ***** MBINS Error, Valid MBINS: [MBINS=app MBINS=kernel] ***** ***)
endif

define MBINS_EXIT
$(error ***** This Platform Not Support Multi-Bins! [SUPPORT_MBINS:=$(SUPPORT_MBINS)] Not Use Option:[MBINs=*] ***** ***)
endef

# Support for 2nd bootloader
BIN2BOOTTYPE_LOWER := .2boot

# Define help message
define USAGE_TEXT

Usage: aos make <target> -c <[config|menuconfig]>
       aos make menuconfig
       aos make [VERBOSE=1] [MBINS=app|kernel] [BUILD_TYPE=debug|release|release_log]
                [clean|distclean] [export-keil|export-iar] ...

  <target>
    One each of the following mandatory components separated by '@'
      * Application (apps in app/example/, app/profile/ ...)
      * Board (boards in board/)

  [VERBOSE=1]
    Shows the commands as they are being executed

  [MBINS=app/kernel]
    Build bins: app or kernel

  [BUILD_TYPE=debug|release|release_log]
    Build for debug, release or release_log (optimize but log enable) configurations

  Notes
    * Component names are case sensitive
    * Component names MUST NOT include space or '@' characters
    * BUILD_TYPE is set as release_log by default

  Example Usage:
    Configure and build with default configs:
      $> aos make helloworld@mk3060 -c config
      $> aos make

    Configure and build with customize configs:
      $> aos make menuconfig
      $> aos make

    Customize configs from default and build:
      $> aos make helloworld@mk3060 -c menuconfig
      $> aos make

    Clean output directory (without configs)
      $> make clean

    Clean output directory and configs
      $> aos make distclean
endef

############################
# Extra options:
#   CHECK_HEADERS=1 : builds header files to test for their completeness
############################

OPENOCD_LOG_FILE ?= $(BUILD_DIR)/openocd.log
DOWNLOAD_LOG := >> $(OPENOCD_LOG_FILE)

BOOTLOADER_LOG_FILE ?= $(BUILD_DIR)/bootloader.log
export HOST_OS
export VERBOSE
export SUB_BUILD
export OPENOCD_LOG_FILE

COMPONENT_DEPENDENCY_SCRIPT := $(SCRIPTS_PATH)/component_dependencies.py
COMPONENT_DEPENDENCY = $(PYTHON) $(COMPONENT_DEPENDENCY_SCRIPT)
export COMPONENT_DEPENDENCY

.PHONY: $(BUILD_STRING) main_app clean help export_linkkit_sdk restore_linkkit_sdk

help:
	$(info $(USAGE_TEXT))
	$(QUIET)$(ECHO)

clean:
	$(QUIET)$(ECHO) Cleaning...
	$(QUIET)$(CLEAN_COMMAND)
	$(QUIET)rm -rf build/scripts/config_mk.py
ifneq (,$(wildcard build/scripts/*.pyc))
	$(QUIET)rm -rf build/scripts/*.pyc
endif
	$(QUIET)$(ECHO) Done

distclean: clean
	$(QUIET)rm -f .config .config.retry .config.old .config.cmd .defconfig

ifneq ($(BUILD_STRING),)
ifeq ($(findstring @,$(BUILD_STRING)),)
$(error Unknown target $(BUILD_STRING) ...)
endif

# Include toolchain
ifeq ($(IDE),iar)
export COMPILER := iar
else ifeq ($(IDE),keil)
export COMPILER := armcc
else ifeq ($(IDE),rvct)
export COMPILER := rvct
endif

ifeq ($(COMPILER),armcc)
export TOOLCHAIN_MAKEFILE := $(MAKEFILES_PATH)/toolchain/aos_toolchain_armcc.mk
else ifeq ($(COMPILER),rvct)
export TOOLCHAIN_MAKEFILE := $(MAKEFILES_PATH)/toolchain/aos_toolchain_rvct.mk
else ifeq ($(COMPILER),iar)
export TOOLCHAIN_MAKEFILE := $(MAKEFILES_PATH)/toolchain/aos_toolchain_iar.mk
else
export TOOLCHAIN_MAKEFILE := $(MAKEFILES_PATH)/toolchain/aos_toolchain_gcc.mk
endif

# Support for test suite - create auto_component and register init functions for test components
AUTO_COMPONENT = $(AUTO_COMPONENT_DIR)/auto_component.mk
ifneq ($(test), )
TEST_COMPONENT_COLLECTION = $(AUTO_COMPONENT_DIR)/test_collection.$(test)
else
TEST_COMPONENT_COLLECTION = $(AUTO_COMPONENT_DIR)/test_collection.default
endif

$(AUTO_COMPONENT): $(TEST_COMPONENT_COLLECTION)
	$(QUIET)$(PYTHON) $(SCRIPTS_PATH)/auto_component.py $(AUTO_COMPONENT_DIR)

$(TEST_COMPONENT_COLLECTION):
	$(QUIET)$(PYTHON) $(SCRIPTS_PATH)/gen_test_collection.py $(AUTO_COMPONENT_DIR) $(TEST_COMPONENT_COLLECTION)

# Target to create config.mk file
$(OUTPUT_DIR)/config.mk: $(MAKEFILES_PATH)/aos_target_config.mk $(MAKEFILES_PATH)/aos_host_cmd.mk $(TOOLCHAIN_MAKEFILE) $(AOS_SDK_MAKEFILES) $(AUTO_COMPONENT) $(AOS_PRE_BUILD_TARGETS)
	$(QUIET)$(ECHO) $(if $(AOS_SDK_MAKEFILES),Applying changes made to: $?,Making config file for first time)
	$(QUIET)$(MAKE) -r $(SILENT) -f $(MAKEFILES_PATH)/aos_target_config.mk $(CLEANED_BUILD_STRING)

ifeq ($(NOCONFIG),)
$(OUTPUT_DIR)/config.mk: $(AOS_CONFIG_DIR)/autoconf.h
endif

endif # ($(BUILD_STRING),)

ifneq ($(MBINS),)
ifneq ($(SUPPORT_MBINS),yes)
MBINS_ERROR := error
endif
endif

all: $(BUILD_STRING)

$(BUILD_STRING): $(AOS_PRE_BUILD_TARGETS) main_app 2boot

ifeq ($(IDE),keil)
main_app: export-keil
else ifeq ($(IDE),iar)
main_app: export-iar
endif

.PHONY: show-build-info
show-build-info:
	$(QUIET)$(ECHO)
	$(QUIET)$(ECHO) Build Configuration:
	$(QUIET)$(ECHO) ====================
	$(QUIET)$(ECHO) App:   $(appname)
	$(QUIET)$(ECHO) Board: $(boardname)
	$(QUIET)$(ECHO) ====================
	$(QUIET)$(ECHO)

.PHONY: prebuild
ifeq ($(NOCONFIG),)
prebuild: show-build-info
endif

prebuild: $(AOS_PRE_BUILD_TARGETS)

init-build-env: $(OUTPUT_DIR)/config.mk
	$(QUIET)$(call MKDIR, $(OUTPUT_DIR)/binary)
	$(QUIET)$(call MKDIR, $(OUTPUT_DIR)/libraries)
	$(QUIET)$(call MKDIR, $(OUTPUT_DIR)/includes)
	$(QUIET)$(PYTHON) $(SCRIPTS_PATH)/gen_auto_code.py $(OUTPUT_DIR)/config.mk $(AUTO_COMPONENT_DIR)

main_app: prebuild $(OUTPUT_DIR)/config.mk $(YOS_SDK_PRE_APP_BUILDS) $(MAKEFILES_PATH)/aos_target_build.mk init-build-env
	$(if $(MBINS_ERROR), $(call MBINS_EXIT))
	$(QUIET)$(ECHO) Build AOS Now
	$(QUIET)$(ECHO) TOOLCHAIN_PATH=$(TOOLCHAIN_PATH)
	$(QUIET)$(MAKE) -r $(JOBSNO) $(SILENT) -f $(MAKEFILES_PATH)/aos_target_build.mk $(CLEANED_BUILD_STRING) $(PASSDOWN_TARGETS)
	$(QUIET)$(ECHO) Build complete: $(BUILD_STRING)

ifeq (yes,$(AOS_SDK_2BOOT_SUPPORT))
$(OUTPUT_DIR)/config_2boot.mk: $(MAKEFILES_PATH)/aos_target_config_2boot.mk $(MAKEFILES_PATH)/aos_host_cmd.mk $(TOOLCHAIN_MK) $(AOS_SDK_2BOOT_MAKEFILES) $(AUTO_COMPONENT)
	$(QUIET)$(ECHO) $(if $(AOS_SDK_2BOOT_MAKEFILES),Applying changes made to: $?,Making config file for first time)
	$(QUIET)$(MAKE) -r $(SILENT) -f $(MAKEFILES_PATH)/aos_target_config_2boot.mk $(CLEANED_BUILD_STRING)

2boot: $(OUTPUT_DIR)/config_2boot.mk $(MAKEFILES_PATH)/aos_target_build_2boot.mk
	$(if $(MBINS_ERROR), $(call MBINS_EXIT))
	$(QUIET)$(ECHO) Build AOS Second Boot Now
	$(QUIET)$(ECHO) TOOLCHAIN_PATH=$(TOOLCHAIN_PATH)
	$(QUIET)$(call MKDIR, $(OUTPUT_DIR)/binary)
	$(QUIET)$(call MKDIR, $(OUTPUT_DIR)/libraries)
	$(QUIET)$(PYTHON) $(SCRIPTS_PATH)/gen_auto_code.py $(OUTPUT_DIR)/config_2boot.mk $(AUTO_COMPONENT_DIR)
	$(QUIET)$(MAKE) -r $(SILENT) $(JOBSNO) -f $(MAKEFILES_PATH)/aos_target_build_2boot.mk $(CLEANED_BUILD_STRING) $(PASSDOWN_TARGETS)
	$(QUIET)$(ECHO) Build 2nd boot complete: $(BUILD_STRING)
	$(QUIET)$(PYTHON) $(SCRIPTS_PATH)/gen_output.py $(OUTPUT_DIR)/binary $(OUTPUT_DIR)/config.mk
else
2boot:
	$(QUIET)$(PYTHON) $(SCRIPTS_PATH)/gen_output.py $(OUTPUT_DIR)/binary $(OUTPUT_DIR)/config.mk
endif

ifeq (littlevgl_simulate@linuxhost,$(CLEANED_BUILD_STRING))
	./out/littlevgl_simulate@linuxhost/binary/littlevgl_simulate@linuxhost.elf
endif

export_linkkit_sdk:
	$(QUIET)$(call MKDIR, $(BUILD_DIR))
	$(QUIET)$(PYTHON) $(SCRIPTS_PATH)/gen_linkkit_sdk.py $(SOURCE_ROOT) $(BUILD_DIR)/export

restore_linkkit_sdk:
	$(QUIET)$(call MKDIR, $(BUILD_DIR))
	$(QUIET)$(PYTHON) $(SCRIPTS_PATH)/restore_linkkit_sdk.py $(SOURCE_ROOT) $(BUILD_DIR)/export

#%.config: $(CONFIG_FILE)
#	$(QUIET)$(ECHO) Build configs complete: $(CONFIG_FILE)

export-project: $(CONFIG_FILE)
	$(QUIET)$(MAKE) -r $(SILENT) -f $(MAKEFILES_PATH)/aos_3rd_project.mk $(BUILD_STRING)

export-keil: export-project
	$(QUIET)$(ECHO) Build Keil project complete

export-iar: export-project
	$(QUIET)$(ECHO) Build IAR Project complete

%.config: $(AOS_CONFIG)
	$(QUIET):

%.menuconfig: $(AOS_CONFIG)
	$(QUIET)$(MAKE) -r $(SILENT) -f $(MAKEFILES_PATH)/aos_kconfig.mk menuconfig
