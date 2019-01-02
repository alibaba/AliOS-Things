default: Help

unexport NAME # For compatibility with Windows Subsystem Linux

export SOURCE_ROOT ?= ./

export MAKEFILES_PATH := $(SOURCE_ROOT)/build/build_rules
export SCRIPTS_PATH := $(SOURCE_ROOT)/build/scripts

export UNDERLINE
export RADIXPOINT

export MBINSTYPE_UPPER
export MBINSTYPE_LOWER
export BIN2BOOTTYPE_LOWER

MAKEFILE_TARGETS := clean Help # targets used by makefile

MBINS ?=
MBINS_APP ?=

#define BUILD_STRING, AOS toolchain commands on different hosts
include $(MAKEFILES_PATH)/aos_host_cmd.mk

# kconfig support
include $(MAKEFILES_PATH)/aos_kconfig.mk
include $(MAKEFILES_PATH)/aos_prebuild.mk

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

BIN2BOOTTYPE_LOWER := .2boot

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

.PHONY: $(BUILD_STRING) main_app bootloader clean Help download total run download_bootloader .gdbinit .gdbinit$(MBINSTYPE_LOWER) export_linkkit_sdk restore_linkkit_sdk

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

ifneq ($(BUILD_STRING),)
-include $(OUTPUT_DIR)/config.mk

ifeq ($(IDE),iar)
COMPILER := iar
CLEANED_BUILD_STRING += COMPILER=iar
else ifeq ($(IDE),keil)
COMPILER := armcc
CLEANED_BUILD_STRING += COMPILER=armcc
else ifeq ($(IDE),rvct)
COMPILER := rvct
CLEANED_BUILD_STRING += COMPILER=rvct
endif

# Now we know the target architecture - include all toolchain makefiles and check one of them can handle the architecture
ifeq ($(COMPILER),armcc)
 TOOLCHAIN_MK = $(MAKEFILES_PATH)/toolchain/aos_toolchain_armcc.mk
else ifeq ($(COMPILER),rvct)
 TOOLCHAIN_MK = $(MAKEFILES_PATH)/toolchain/aos_toolchain_rvct.mk
else ifeq ($(COMPILER),iar)
 TOOLCHAIN_MK = $(MAKEFILES_PATH)/toolchain/aos_toolchain_iar.mk
else
 TOOLCHAIN_MK = $(MAKEFILES_PATH)/toolchain/aos_toolchain_gcc.mk
endif

include $(TOOLCHAIN_MK)

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

$(OUTPUT_DIR)/config.mk: $(MAKEFILES_PATH)/aos_target_config.mk $(MAKEFILES_PATH)/aos_host_cmd.mk $(TOOLCHAIN_MK) $(AOS_SDK_MAKEFILES) $(AUTO_COMPONENT) $(AOS_PRE_BUILD_TARGETS)
	$(QUIET)$(if $(PARSE_RESOURSE_TO_SYSCALL_FILE), $(call MKDIR, $(OUTPUT_DIR)/precompile))
	$(QUIET)$(if $(PARSE_RESOURSE_TO_SYSCALL_FILE), $(call MKDIR, $(OUTPUT_DIR)/syscall/syscall_kapi))
	$(QUIET)$(if $(PARSE_RESOURSE_TO_SYSCALL_FILE), $(call MKDIR, $(OUTPUT_DIR)/syscall/syscall_fapi))
	$(QUIET)$(if $(PARSE_RESOURSE_TO_SYSCALL_FILE), $(call PARSE_RESOURSE_TO_SYSCALL_FILE, $(OUTPUT_DIR), pre-create))
	$(QUIET)$(ECHO) $(if $(AOS_SDK_MAKEFILES),Applying changes made to: $?,Making config file for first time)
	$(QUIET)$(MAKE) -r $(SILENT) -f $(MAKEFILES_PATH)/aos_target_config.mk $(CLEANED_BUILD_STRING)

endif

define MBINS_EXIT
$(error ***** This Platform Not Support Multi-Bins! [SUPPORT_MBINS:=$(SUPPORT_MBINS)] Not Use Option:[MBINs=*] ***** ***)
endef

ifneq ($(MBINS),)
ifneq ($(SUPPORT_MBINS),yes)
MBINS_ERROR := error
endif
endif

JOBS ?=4
ifeq (,$(SUB_BUILD))
JOBSNO := -j$(JOBS)
endif

PASSDOWN_TARGETS := $(strip $(filter-out $(MAKEFILE_TARGETS) $(BUILD_STRING) export_linkkit_sdk restore_linkkit_sdk,$(MAKECMDGOALS))) #download total
$(PASSDOWN_TARGETS):
	@:

$(BUILD_STRING): $(AOS_PRE_BUILD_TARGETS) main_app 2boot

main_app: $(OUTPUT_DIR)/config.mk $(YOS_SDK_PRE_APP_BUILDS) $(MAKEFILES_PATH)/aos_target_build.mk
	$(if $(MBINS_ERROR), $(call MBINS_EXIT))
	$(QUIET)$(ECHO) Build AOS Now
	$(QUIET)$(ECHO) TOOLCHAIN_PATH=$(TOOLCHAIN_PATH)
	$(QUIET)$(call MKDIR, $(OUTPUT_DIR)/binary)
	$(QUIET)$(call MKDIR, $(OUTPUT_DIR)/modules)
	$(QUIET)$(call MKDIR, $(OUTPUT_DIR)/libraries)
	$(QUIET)$(call MKDIR, $(OUTPUT_DIR)/resources)
	$(QUIET)$(call MKDIR, $(OUTPUT_DIR)/includes)
	$(QUIET)$(PYTHON) $(SCRIPTS_PATH)/gen_auto_code.py $(OUTPUT_DIR)/config.mk $(AUTO_COMPONENT_DIR)
	$(QUIET)$(MAKE) -r $(JOBSNO) $(SILENT) -f $(MAKEFILES_PATH)/aos_target_build.mk $(CLEANED_BUILD_STRING) $(PASSDOWN_TARGETS)
	$(QUIET)$(ECHO) Build complete

ifeq (yes,$(AOS_SDK_2BOOT_SUPPORT))
$(OUTPUT_DIR)/config_2boot.mk: $(MAKEFILES_PATH)/aos_target_config_2boot.mk $(MAKEFILES_PATH)/aos_host_cmd.mk $(TOOLCHAIN_MK) $(AOS_SDK_2BOOT_MAKEFILES) $(AUTO_COMPONENT)
	$(QUIET)$(if $(PARSE_RESOURSE_TO_SYSCALL_FILE), $(call MKDIR, $(OUTPUT_DIR)/precompile))
	$(QUIET)$(if $(PARSE_RESOURSE_TO_SYSCALL_FILE), $(call PARSE_RESOURSE_TO_SYSCALL_FILE, $(OUTPUT_DIR), pre-create))
	$(QUIET)$(ECHO) $(if $(AOS_SDK_2BOOT_MAKEFILES),Applying changes made to: $?,Making config file for first time)
	$(QUIET)$(MAKE) -r $(SILENT) -f $(MAKEFILES_PATH)/aos_target_config_2boot.mk $(CLEANED_BUILD_STRING)

2boot: $(OUTPUT_DIR)/config_2boot.mk $(MAKEFILES_PATH)/aos_target_build_2boot.mk
	$(if $(MBINS_ERROR), $(call MBINS_EXIT))
	$(QUIET)$(ECHO) Build AOS Second Boot Now
	$(QUIET)$(ECHO) TOOLCHAIN_PATH=$(TOOLCHAIN_PATH)
	$(QUIET)$(call MKDIR, $(OUTPUT_DIR)/binary)
	$(QUIET)$(call MKDIR, $(OUTPUT_DIR)/modules)
	$(QUIET)$(call MKDIR, $(OUTPUT_DIR)/libraries)
	$(QUIET)$(call MKDIR, $(OUTPUT_DIR)/resources)
	$(QUIET)$(PYTHON) $(SCRIPTS_PATH)/gen_auto_code.py $(OUTPUT_DIR)/config_2boot.mk $(AUTO_COMPONENT_DIR)
	$(QUIET)$(MAKE) -r $(SILENT) $(JOBSNO) -f $(MAKEFILES_PATH)/aos_target_build_2boot.mk $(CLEANED_BUILD_STRING) $(PASSDOWN_TARGETS)
	$(QUIET)$(ECHO) Build 2nd boot complete
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


