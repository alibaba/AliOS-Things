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

MAKEFILE_TARGETS := clean distclean Help # targets used by makefile

# define commands on different hosts
include $(MAKEFILES_PATH)/aos_host_cmd.mk
# kconfig support
include $(MAKEFILES_PATH)/aos_kconfig.mk

define CHECK_UCUBE_VERSION
$(if $(findstring 0.2,$(shell aos --version)),$(error aos-cube 0.3.x or newer is required, please update your aos-cube first! (eg. $$ aos upgrade)),)
endef

$(call CHECK_UCUBE_VERSION)

define NO_CONFIG_ERROR
$(info Please configure AliOS-Things first:)
$(info *********************************************************************)
$(info $$ aos make app@board -c config)
$(info Or)
$(info $$ aos make menuconfig)
$(info *********************************************************************)
$(error No Config Found!)
endef

define TARGET_OBSOLATE_ERROR
$(error Build Target Obsolate!)
endef

all:
	$(QUIET)$(if $(wildcard $(AOS_CONFIG)),,$(call NO_CONFIG_ERROR))

#
# Process build strings
#
BUILD_SUBTARGETS := .config .menuconfig compile .image .export-keil .export-iar .defconfig
export BUILD_STRING ?= $(strip $(filter-out $(MAKEFILE_TARGETS) download run total export_linkkit_sdk restore_linkkit_sdk, $(MAKECMDGOALS)))

# Set IDE if export-* is specified
ifneq ($(filter %.export-keil, $(BUILD_STRING)),)
export IDE := keil
else ifneq ($(filter %.export-iar, $(BUILD_STRING)),)
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
$(info The command "aos make $(BUILD_STRING)" is obselate!)
$(info *********************************************************************)
$(info Step 1: config:)
$(info $$ aos make $(BUILD_STRING) -c config)
$(info --- more configs can use: $$ aos make menuconfig)
$(info Step 2: build:)
$(info $$ aos make [VAR=value])
$(info )
$(info Or run config & build in oneline:)
$(info $$ aos make $(BUILD_STRING) -c config && aos make [VAR=value])
$(info *********************************************************************)
$(call TARGET_OBSOLATE_ERROR)
endif
endif

else
ifneq ($(findstring @, $(MAKECMDGOALS)),)
ifeq ($(filter %.config %.menuconfig, $(MAKECMDGOALS)),)
$(info )
$(info The command "aos make $(BUILD_STRING)" is obselate!)
$(info *********************************************************************)
$(info Please run build with:)
$(info $$ aos make [VAR=value])
$(info *********************************************************************)
$(call TARGET_OBSOLATE_ERROR)
endif
endif
endif

ifeq ($(findstring @,$(BUILD_STRING)),)
$(error Unknown target $(BUILD_STRING) ...)
endif

components := $(subst @, ,$(BUILD_STRING))
ifneq ($(words $(components)),2)
$(error App/Board Unknown, Please check your config!)
endif

# get real app name and check if it available
appname = $(firstword $(components))
appdir = $(subst .,/,$(appname))
appmkfile = $(wildcard $(SOURCE_ROOT)app/*/$(appdir)/aos.mk $(SOURCE_ROOT)test/develop/$(appdir)/aos.mk)
$(if $(appmkfile),\
    $(eval include $(appmkfile)) $(eval real_appname = $(NAME)),\
    $(eval real_appname = $(notdir $(appdir))))

# determine what app type should be enabled: example, profile, test
ifneq ($(findstring profile,$(appmkfile)),)
export AOS_APP_TYPE := AOS_APP_PROFILE
else ifneq ($(findstring example,$(appmkfile)),)
export AOS_APP_TYPE := AOS_APP_EXAMPLE
else
export AOS_APP_TYPE :=
endif

boardname = $(word 2, $(components))
boardmkfile = $(wildcard $(SOURCE_ROOT)board/$(boardname)/aos.mk $(SOURCE_ROOT)board/$(boardname)-*/aos.mk)
$(if $(boardmkfile),\
    $(eval include $(boardmkfile)) $(eval real_boardname = $(NAME)),\
    $(eval real_boardname = $(boardname)))

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
Aborting due to invalid targets

Usage: make <target> [download] [run | debug] [JTAG=xxx] [total] [VERBOSE=1] [MBINS=app/kernel]
       make run

  <target>
    One each of the following mandatory [and optional] components separated by '@'
      * Application (apps in example)
      * Board ($(filter-out common  include README.txt,$(notdir $(wildcard board/*))))
      * [debug | release] Building for debug or release configurations

  [download]
    Download firmware image to target platform

  [run]
    Reset and run an application on the target hardware

  [total]
    Build all targets related to this application and board

  [JTAG=xxx]
    JTAG interface configuration file from the tools/OpenOCD dirctory
    Default option is jlink_swd

  [VERBOSE=1]
    Shows the commands as they are being executed

  [MBINS=app/kernel]
    Build bins: app or kernel

  Notes
    * Component names are case sensitive
    * 'rhino' and 'debug' are reserved component names
    * Component names MUST NOT include space or '@' characters
    * Building for debug is assumed unless '@release' is appended to the target

  Example Usage
    Build for Debug
      $> make helloworld@mk3060

    Build, Download and Run using the default USB-JTAG programming interface
      $> make helloworld@mk3060 download run

    Build for Release
      $> make helloworld@mk3060@release

    Reset and run an application on the target hardware
      $> make run

    Clean output directory
      $> make clean
endef

############################
# Extra options:
#                CHECK_HEADERS=1 : builds header files to test for their completeness
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

.PHONY: $(BUILD_STRING) main_app bootloader clean Help download total run download_bootloader export_linkkit_sdk restore_linkkit_sdk

Help:
	$(TOOLCHAIN_HOOK_TARGETS)
	$(error $(USAGE_TEXT))

clean:
	$(QUIET)$(ECHO) Cleaning...
	$(QUIET)$(CLEAN_COMMAND)
	$(QUIET)rm -rf build/scripts/config_mk.py
ifneq (,$(wildcard build/scripts/*.pyc))
	$(QUIET)rm -rf build/scripts/*.pyc
endif
	$(QUIET)$(ECHO) Done

distclean: clean
	$(QUIET)rm -f .config .config.retry .defconfig

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
main_app: $(addsuffix .export-keil, $(BUILD_STRING))
else ifeq ($(IDE),iar)
main_app: $(addsuffix .export-iar, $(BUILD_STRING))
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

%.export-project: $(CONFIG_FILE)
	$(QUIET)$(MAKE) -r $(SILENT) -f $(MAKEFILES_PATH)/aos_3rd_project.mk $*

%.export-keil: %.export-project
	$(QUIET)$(ECHO) Build Keil project complete

%.export-iar: %.export-project
	$(QUIET)$(ECHO) Build IAR Project complete

#%.compile: %.config
#	$(QUIET)$(MAKE) -r $(JOBSNO) $(SILENT) -f $(MAKEFILES_PATH)/aos_target_build.mk $@

%.defconfig: defconfig
	$(QUIET)$(ECHO) Create .config complete

%.config: $(AOS_CONFIG)
	$(QUIET):

%.menuconfig: $(AOS_CONFIG)
	$(QUIET)$(MAKE) -r $(SILENT) -f $(MAKEFILES_PATH)/aos_kconfig.mk menuconfig
