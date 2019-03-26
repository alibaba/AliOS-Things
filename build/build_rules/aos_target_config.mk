include $(MAKEFILES_PATH)/aos_host_cmd.mk
include $(MAKEFILES_PATH)/aos_kconfig.mk

# include .config
ifeq ($(NOCONFIG),)
$(if $(wildcard $(AOS_CONFIG)), \
    $(eval include $(AOS_CONFIG)), $(error No such file: $(AOS_CONFIG)))
endif

# <comp>_LOCATION and <comp>_MAKEFILE defined in aos_all_components.mk
$(if $(wildcard $(OUTPUT_DIR)/aos_all_components.mk), \
    $(eval include $(OUTPUT_DIR)/aos_all_components.mk), \
    $(error No such file: $(OUTPUT_DIR)/aos_all_components.mk) )

APPDIR ?=
CONFIG_FILE_DIR := $(OUTPUT_DIR)
CONFIG_FILE := $(CONFIG_FILE_DIR)/config.mk
DEPENDENCY_DICT :=

COMPONENT_DIRECTORIES := . \
                         app/example   \
                         app/profile   \
                         board     \
                         kernel    \
                         platform  \
                         utility   \
                         middleware \
                         network   \
                         tools     \
                         test      \
                         test/develop      \
                         drivers    \
                         security

COMPONENT_DIRECTORIES += $(OUTPUT_DIR)
COMPONENT_DIRECTORIES += $(APPDIR)

##################################
# Macros
##################################

#####################################################################################
# Macro GET_BARE_LOCATION get relative path from $(SOURCE_ROOT) to component
# $(1) is component name
GET_BARE_LOCATION =$(patsubst $(call ESCAPE_BACKSLASHES,$(SOURCE_ROOT))%,%,$(strip $($(1)_LOCATION)))

#####################################################################################
# Macro PREPROCESS_TEST_COMPONENT get test components from command line:
#   aos make yts@boardname test="comp1,comp2..."
define PREPROCESS_TEST_COMPONENT
$(if $(filter yts,$(COMPONENTS)), \
$(if $(test), $(eval TEST_COMPONENTS := $(foreach tmpcomp,$(strip $(subst $(COMMA),$(SPACE),$(test))),$(if $(findstring _test,$(tmpcomp)),$(tmpcomp),$(addsuffix _test,$(tmpcomp))))),) \
$(eval COMPONENTS += $(TEST_COMPONENTS)))
endef

#####################################################################################
# Macro FIND_VARIOUS_COMPONENT use breadth traversal to search components
# $(1) is the list of components left to process.
# $(COMP) is set as the first element in the list
define FIND_VARIOUS_COMPONENT

$(eval COMP := $(word 1,$(1)))
$(call FIND_ONE_COMPONENT, $(COMP))

$(eval PROCESSED_COMPONENTS_LOCS += $(COMP))
$(if $(strip $(filter-out $(PROCESSED_COMPONENTS_LOCS),$(COMPONENTS))),\
     $(call FIND_VARIOUS_COMPONENT,$(filter-out $(PROCESSED_COMPONENTS_LOCS),$(COMPONENTS))),\
)

endef

#####################################################################################
# Macro FIND_ONE_COMPONENT search component with name
# $(1) is the name of component
define FIND_ONE_COMPONENT

$(eval COMP := $(1))
$(eval COMP_LOCATION := $(subst .,/,$(COMP)))
$(eval COMP_MAKEFILE_NAME := $(notdir $(COMP_LOCATION)))
# Find the component makefile in directory list

$(eval TEMP_MAKEFILE := $(if $($(COMP)_MAKEFILE),$($(COMP)_MAKEFILE),$(strip $(wildcard $(foreach dir, $(if $(APPDIR),$(APPDIR),) $(if $(CUBE_AOS_DIR),$(CUBE_AOS_DIR) $(CUBE_AOS_DIR)/remote,) $(addprefix $(SOURCE_ROOT),$(COMPONENT_DIRECTORIES)), $(dir)/$(COMP_LOCATION)/$(COMP_MAKEFILE_NAME).mk $(dir)/$(COMP_LOCATION)/aos.mk)))))
# Check if component makefile was found - if not try downloading it and re-doing the makefile search
$(if $(TEMP_MAKEFILE),,\
    $(info Unknown component: $(COMP) - directory or makefile for component not found. Ensure the $(COMP_LOCATION) directory contains aos.mk) \
    $(info Below is a list of valid local components (Some are internal): ) \
    $(call FIND_VALID_COMPONENTS, VALID_COMPONENT_LIST,$(COMPONENT_DIRECTORIES)) \
     $(foreach comp,$(VALID_COMPONENT_LIST),$(info $(comp))) \
     $(info Below is a list of valid components from the internet: ) \
     $(info $(call DOWNLOAD_COMPONENT_LIST)) \
     $(error Unknown component: $(COMP) - directory or makefile for component not found. Ensure the $(COMP_LOCATION) directory contains aos.mk))
$(eval TEMP_MAKEFILE := $(if $(filter %aos.mk,$(TEMP_MAKEFILE)),$(filter %aos.mk,$(TEMP_MAKEFILE)),$(TEMP_MAKEFILE)))
$(if $(filter 1,$(words $(TEMP_MAKEFILE))),,$(error More than one component with the name "$(COMP)". See $(TEMP_MAKEFILE)))

$(eval TEMP_MAKEFILE := $(subst ././,./,$(TEMP_MAKEFILE)))
$(eval include $(TEMP_MAKEFILE))
$(eval deps :=)
$(eval NAME := $(strip $(NAME)))
$(eval $(NAME)_COMPONENTS += $($(NAME)_COMPONENTS-y))
$(eval deps_src := $($(NAME)_COMPONENTS))
$(eval components_cube := $(subst .,/,$(COMPONENTS)))
$(eval deps_cube := $(subst .,/,$($(NAME)_COMPONENTS)))

$(foreach dep, $(deps_cube),\
	$(eval comp_dep := $(firstword $(deps_src))) \
	$(eval find := 0) \
	$(foreach component, $(components_cube) $(CUBE_REMOVE_COMPONENTS), \
		$(if $(filter $(notdir $(dep)),$(notdir $(component))), \
			$(if $(findstring $(dep), $(component)),$(eval find := 1))))\
	$(if $(filter 0, $(find)), $(eval deps += $(comp_dep))) \
	$(eval deps_src := $(filter-out $(comp_dep),$(deps_src))))

$(if $(findstring $(TEMP_MAKEFILE),$(ALL_MAKEFILES)),,\
	$(eval ALL_MAKEFILES += $(TEMP_MAKEFILE)) \
	$(eval COMPONENTS += $(deps)) \
	$(eval REAL_COMPONENTS_LOCS += $(COMP)) \
	$(eval iotx_check_RET:=0)\
	$(call PREPROCESS_TEST_COMPONENT, $(COMPONENTS), $(TEST_COMPONENTS)) \
	DEPENDENCY_DICT += '$(NAME)': '$($(NAME)_COMPONENTS)',)

endef

#####################################################################################
# Macro PROCESS_ONE_COMPONENT
# $(1) is one component
define PROCESS_ONE_COMPONENT
$(eval COMP := $(1))
$(eval COMP_LOCATION := $(subst .,/,$(COMP)))
$(eval COMP_MAKEFILE_NAME := $(notdir $(COMP_LOCATION)))

# Find the component makefile in directory list
$(eval TEMP_MAKEFILE := $(if $($(COMP)_MAKEFILE),$($(COMP)_MAKEFILE),$(strip $(wildcard $(foreach dir,  $(if $(CUBE_AOS_DIR),$(CUBE_AOS_DIR) $(CUBE_AOS_DIR)/remote) $(addprefix $(SOURCE_ROOT),$(COMPONENT_DIRECTORIES)), $(dir)/$(COMP_LOCATION)/$(COMP_MAKEFILE_NAME).mk $(dir)/$(COMP_LOCATION)/aos.mk)))))
$(eval TEMP_MAKEFILE := $(if $(filter %aos.mk,$(TEMP_MAKEFILE)),$(filter %aos.mk,$(TEMP_MAKEFILE)),$(TEMP_MAKEFILE)))

# Clear all the temporary variables
$(eval GLOBAL_INCLUDES:=)
$(eval GLOBAL_LINK_SCRIPT:=)
$(eval DEFAULT_LINK_SCRIPT:=)
$(eval DCT_LINK_SCRIPT:=)
$(eval GLOBAL_DEFINES:=)
$(eval GLOBAL_CFLAGS:=)
$(eval GLOBAL_CXXFLAGS:=)
$(eval GLOBAL_ASMFLAGS:=)
$(eval GLOBAL_LDFLAGS:=)
$(eval GLOBAL_LDS_FILES:=)
$(eval GLOBAL_LDS_INCLUDES:=)
$(eval GLOBAL_CERTIFICATES:=)

$(eval GLOBAL_INCLUDES-y:=)
$(eval GLOBAL_LINK_SCRIPT-y:=)
$(eval DEFAULT_LINK_SCRIPT-y:=)
$(eval DCT_LINK_SCRIPT-y:=)
$(eval GLOBAL_DEFINES-y:=)
$(eval GLOBAL_CFLAGS-y:=)
$(eval GLOBAL_CXXFLAGS-y:=)
$(eval GLOBAL_ASMFLAGS-y:=)
$(eval GLOBAL_LDFLAGS-y:=)
$(eval GLOBAL_LDS_FILES-y:=)
$(eval GLOBAL_LDS_INCLUDES-y:=)
$(eval GLOBAL_CERTIFICATES-y:=)

$(eval WIFI_CONFIG_DCT_H:=)
$(eval BT_CONFIG_DCT_H:=)
$(eval APPLICATION_DCT:=)
$(eval CERTIFICATE:=)
$(eval PRIVATE_KEY:=)
$(eval CHIP_SPECIFIC_SCRIPT:=)
$(eval CONVERTER_OUTPUT_FILE:=)
$(eval BIN_OUTPUT_FILE:=)
$(eval OLD_CURDIR := $(CURDIR))
$(eval CURDIR := $(CURDIR)$(dir $(TEMP_MAKEFILE)))
$(eval TEST_COMPONENTS :=)
$(eval CPLUSPLUS_FLAGS:=)
$(eval AOS_2BOOT_SUPPORT :=)
$(eval AOS_IMG1_XIP1_LD_FILE :=)
$(eval AOS_IMG2_XIP2_LD_FILE :=)
# Cache the last valid RTOS/NS combination for iterative filtering.
$(eval TEMP_VALID_OSNS_COMBOS := $(VALID_OSNS_COMBOS))

# Include the component makefile - This defines the NAME variable
$(eval include $(TEMP_MAKEFILE))

# Filter the valid RTOS/NS combination to the least-common set.
$(eval VALID_OSNS_COMBOS :=\
  $(if $(VALID_OSNS_COMBOS),\
    $(filter $(VALID_OSNS_COMBOS),$(TEMP_VALID_OSNS_COMBOS)),\
    $(TEMP_VALID_OSNS_COMBOS)\
  )\
)

$(eval $(NAME)_MAKEFILE :=$(TEMP_MAKEFILE))

$(eval CURDIR := $(OLD_CURDIR))

$(eval $(NAME)_LOCATION := $(dir $(TEMP_MAKEFILE)))
$(eval $(NAME)_MAKEFILE := $(TEMP_MAKEFILE))
AOS_SDK_MAKEFILES     += $($(NAME)_MAKEFILE)

# Set debug/release specific options
$(eval $(NAME)_BUILD_TYPE := $(BUILD_TYPE))
$(eval $(NAME)_BUILD_TYPE := $(if $($(NAME)_NEVER_OPTIMISE),  debug,   $($(NAME)_BUILD_TYPE)))
$(eval $(NAME)_BUILD_TYPE := $(if $($(NAME)_ALWAYS_OPTIMISE), release, $($(NAME)_BUILD_TYPE)))

ifeq ($($(NAME)_BUILD_TYPE),debug)
$(NAME)_ASMFLAGS += $(COMPILER_SPECIFIC_DEBUG_ASFLAGS) $($(NAME)_ASMFLAGS-y)
$(NAME)_LDFLAGS  += $(COMPILER_SPECIFIC_DEBUG_LDFLAGS) $($(NAME)_LDFLAGS-y)
$(NAME)_OPTIM_CFLAGS ?= $(COMPILER_SPECIFIC_DEBUG_CFLAGS) $($(NAME)_OPTIM_CFLAGS-y)
$(NAME)_OPTIM_CXXFLAGS ?= $(COMPILER_SPECIFIC_DEBUG_CXXFLAGS) $($(NAME)_OPTIM_CXXFLAGS-y)
else ifeq ($($(NAME)_BUILD_TYPE),inspect)
$(NAME)_ASMFLAGS += $(COMPILER_SPECIFIC_INSPECT_ASFLAGS) $($(NAME)_ASMFLAGS-y)
$(NAME)_LDFLAGS  += $(COMPILER_SPECIFIC_INSPECT_LDFLAGS) $($(NAME)_LDFLAGS-y)
$(NAME)_OPTIM_CFLAGS ?= $(COMPILER_SPECIFIC_INSPECT_CFLAGS) $($(NAME)_OPTIM_CFLAGS-y)
$(NAME)_OPTIM_CXXFLAGS ?= $(COMPILER_SPECIFIC_INSPECT_CXXFLAGS) $($(NAME)_OPTIM_CXXFLAGS-y)
else ifeq ($($(NAME)_BUILD_TYPE),release_log)
$(NAME)_ASMFLAGS += $(COMPILER_SPECIFIC_RELEASE_LOG_ASFLAGS) $($(NAME)_ASMFLAGS-y)
$(NAME)_LDFLAGS  += $(COMPILER_SPECIFIC_RELEASE_LOG_LDFLAGS) $($(NAME)_LDFLAGS-y)
$(NAME)_OPTIM_CFLAGS ?= $(COMPILER_SPECIFIC_RELEASE_LOG_CFLAGS) $($(NAME)_OPTIM_CFLAGS-y)
$(NAME)_OPTIM_CXXFLAGS ?= $(COMPILER_SPECIFIC_RELEASE_LOG_CXXFLAGS) $($(NAME)_OPTIM_CXXFLAGS-y)
else ifeq ($($(NAME)_BUILD_TYPE),release)
$(NAME)_ASMFLAGS += $(COMPILER_SPECIFIC_RELEASE_ASFLAGS) $($(NAME)_ASMFLAGS-y)
$(NAME)_LDFLAGS  += $(COMPILER_SPECIFIC_RELEASE_LDFLAGS) $($(NAME)_LDFLAGS-y)
$(NAME)_OPTIM_CFLAGS ?= $(COMPILER_SPECIFIC_RELEASE_CFLAGS) $($(NAME)_OPTIM_CFLAGS-y)
$(NAME)_OPTIM_CXXFLAGS ?= $(COMPILER_SPECIFIC_RELEASE_CXXFLAGS) $($(NAME)_OPTIM_CXXFLAGS-y)
endif

AOS_SDK_LINK_SCRIPT        +=$(if $(GLOBAL_LINK_SCRIPT),$(GLOBAL_LINK_SCRIPT),) $(if $(GLOBAL_LINK_SCRIPT-y),$(GLOBAL_LINK_SCRIPT-y),)
AOS_SDK_DEFAULT_LINK_SCRIPT+=$(if $(DEFAULT_LINK_SCRIPT),$(addprefix $($(NAME)_LOCATION),$(DEFAULT_LINK_SCRIPT)),) $(if $(DEFAULT_LINK_SCRIPT-y),$(addprefix $($(NAME)_LOCATION),$(DEFAULT_LINK_SCRIPT-y)),)
$(eval AOS_SDK_DEFINES            +=$(GLOBAL_DEFINES) $(GLOBAL_DEFINES-y))
AOS_SDK_CFLAGS             +=$(GLOBAL_CFLAGS) $(GLOBAL_CFLAGS-y)
AOS_SDK_CXXFLAGS           +=$(GLOBAL_CXXFLAGS) $(GLOBAL_CXXFLAGS-y)
AOS_SDK_ASMFLAGS           +=$(GLOBAL_ASMFLAGS) $(GLOBAL_ASMFLAGS-y)
AOS_SDK_LDFLAGS            +=$(GLOBAL_LDFLAGS) $(GLOBAL_LDFLAGS-y)
AOS_SDK_LDS_FILES          +=$(GLOBAL_LDS_FILES) $(GLOBAL_LDS_FILES-y)
AOS_SDK_LDS_INCLUDES       +=$(GLOBAL_LDS_INCLUDES) $(GLOBAL_LDS_INCLUDES-y)
AOS_SDK_CHIP_SPECIFIC_SCRIPT += $(CHIP_SPECIFIC_SCRIPT)
AOS_SDK_CONVERTER_OUTPUT_FILE += $(CONVERTER_OUTPUT_FILE)
AOS_SDK_FINAL_OUTPUT_FILE += $(BIN_OUTPUT_FILE)
AOS_SDK_2BOOT_SUPPORT +=$(AOS_2BOOT_SUPPORT)
AOS_SDK_IMG1_XIP1_LD_FILE +=$(AOS_IMG1_XIP1_LD_FILE)
AOS_SDK_IMG2_XIP2_LD_FILE +=$(AOS_IMG2_XIP2_LD_FILE)
AOS_CPLUSPLUS_FLAGS += $(CPLUSPLUS_FLAGS)
$(eval PROCESSED_COMPONENTS += $(NAME))
$(eval $(NAME)_SOURCES := $(sort $(subst $($(NAME)_LOCATION),,$(wildcard $(addprefix $($(NAME)_LOCATION),$($(NAME)_SOURCES) $($(NAME)_SOURCES-y)) ))))
$(eval $(NAME)_POPULATE_INCLUDES := $(if $(filter 1,$(STRICT)),$(addprefix $(subst $(SOURCE_ROOT),,$($(NAME)_LOCATION)),$(GLOBAL_INCLUDES) $(GLOBAL_INCLUDES-y))))

ifeq ($(STRICT),1)
AOS_SDK_INCLUDES += $(addprefix -I$(OUTPUT_DIR)/includes/,$($(NAME)_POPULATE_INCLUDES))
else
AOS_SDK_INCLUDES +=$(addprefix -I$($(NAME)_LOCATION),$(GLOBAL_INCLUDES) $(GLOBAL_INCLUDES-y))
endif

endef

#####################################################################################
# Macro PROCESS_COMPONENT
# $(1) is the list of components left to process. $(COMP) is set as the first element in the list
define PROCESS_COMPONENT
AOS_SDK_DEFINES += MCU_FAMILY=\"$(HOST_MCU_FAMILY)\"
$(eval CONFIG_IN_COMPONENTS := $(sort $(subst board_,,$(OBJ-y))))
$(eval ONLY_IN_AOSMK := $(filter-out $(CONFIG_IN_COMPONENTS), $(REAL_COMPONENTS_LOCS)))
$(eval ONLY_IN_CONFIGIN := $(filter-out $(REAL_COMPONENTS_LOCS), $(CONFIG_IN_COMPONENTS)))
$(eval ALL_COMPONENTS := $(REAL_COMPONENTS_LOCS) $(ONLY_IN_CONFIGIN))
$(info *** All Components: $(ALL_COMPONENTS))
$(if $(DEBUG_CONFIG), \
    $(info *** MKFile Enabled: $(REAL_COMPONENTS_LOCS)) \
    $(info *** Config Enabled: $(CONFIG_IN_COMPONENTS)) \
    $(if $(ONLY_IN_AOSMK), $(info *** MKFile Enabled Only: $(ONLY_IN_AOSMK)),) \
    $(if $(ONLY_IN_CONFIGIN), $(info *** Config Enabled Only: $(ONLY_IN_CONFIGIN)),), \
    $(if $(ONLY_IN_CONFIGIN), $(info *** Config Enabled: $(ONLY_IN_CONFIGIN)),) \
)
$(foreach TMP_COMP, $(REAL_COMPONENTS_LOCS) $(ONLY_IN_CONFIGIN),$(call PROCESS_ONE_COMPONENT, $(TMP_COMP)))

endef

##################################
# Start of processing
##################################

# Separate the build string into components
COMPONENTS := $(subst @, ,$(MAKECMDGOALS))

# Valid build types
BUILD_TYPE_LIST := debug inspect release_log release

ifeq ($(BUILD_TYPE),)
BUILD_TYPE := release_log
else
$(if $(filter $(BUILD_TYPE_LIST),$(BUILD_TYPE)),,$(error BUILD_TYPE Unknown: "$(BUILD_TYPE)"! Please choice one from: "$(BUILD_TYPE_LIST)"))
endif

COMPONENTS := $(filter-out $(BUILD_TYPE_LIST), $(COMPONENTS))

# Set LDFLAGS according to BUILD_TYPE
ifeq ($(BUILD_TYPE),release_log)
AOS_SDK_LDFLAGS  += $(COMPILER_SPECIFIC_RELEASE_LOG_LDFLAGS)
else ifeq ($(BUILD_TYPE),debug)
AOS_SDK_LDFLAGS  += $(COMPILER_SPECIFIC_DEBUG_LDFLAGS)
else ifeq ($(BUILD_TYPE),inspect)
AOS_SDK_LDFLAGS  += $(COMPILER_SPECIFIC_INSPECT_LDFLAGS)
else ifeq ($(BUILD_TYPE),release)
AOS_SDK_LDFLAGS  += $(COMPILER_SPECIFIC_RELEASE_LDFLAGS)
endif

# Check if there are any unknown components; output error if so.
$(foreach comp, $(COMPONENTS), $(if $(wildcard $(APPDIR)/$(comp) $(CUBE_AOS_DIR)/$(comp) $(foreach dir, $(addprefix $(SOURCE_ROOT),$(COMPONENT_DIRECTORIES)), $(dir)/$(subst .,/,$(comp)) ) $($(comp)_LOCATION)),,$(error Unknown component: $(comp))))

# Find the matching platform and application from the build string components
PLATFORM_FULL   :=$(strip $(foreach comp,$(subst .,/,$(COMPONENTS)),$(if $(wildcard $(SOURCE_ROOT)board/$(comp)),$(comp),)))

APP_FULL        :=$(strip $(foreach comp,$(subst .,/,$(COMPONENTS)),$(if $(wildcard $(APPDIR)/$(comp) $(SOURCE_ROOT)app/$(comp) $(SOURCE_ROOT)app/*/$(comp) $(SOURCE_ROOT)$(comp) $(SOURCE_ROOT)test/develop/$(comp)),$(comp),)))

PLATFORM    :=$(notdir $(PLATFORM_FULL))
APP         :=$(notdir $(APP_FULL))

PLATFORM_DIRECTORY := $(PLATFORM_FULL)

# Set APP_VERSION used by OTA
CONFIG_SYSINFO_APP_VERSION = app-1.0.0-$(CURRENT_TIME)
$(info app_version:${CONFIG_SYSINFO_APP_VERSION})

# Append PLATFORM (Board) and APP_VERSION to EXTRA_CFLAGS
EXTRA_CFLAGS := \
                -DPLATFORM=$(SLASH_QUOTE_START)$$(PLATFORM)$(SLASH_QUOTE_END) \
                -DSYSINFO_APP_VERSION=\"$(CONFIG_SYSINFO_APP_VERSION)\"

# Append "-include/--preinclude autoconf.h" to EXTRA_CFLAGS
ifneq ($(filter armcc iar rvct, $(COMPILER)),)
EXTRA_CFLAGS += $(call ARMCC_INCLUDE_AUTOCONF_H) $(call ARMCC_INCLUDE_SYSCONFIG_H)
else
EXTRA_CFLAGS += $(call GCC_INCLUDE_AUTOCONF_H) $(call GCC_INCLUDE_SYSCONFIG_H)
endif

# For .S files that used macros defined by autoconf.h
ifneq ($(filter armcc rvct, $(COMPILER)),)
EXTRA_ASMFLAGS += $(call ARMASM_INCLUDE_AUTOCONF_H)
else ifeq ($(COMPILER),iar)
EXTRA_ASMFLAGS += $(call IASMARM_INCLUDE_AUTOCONF_H)
else
EXTRA_ASMFLAGS += $(call GCC_INCLUDE_AUTOCONF_H)
endif

# Load platform makefile to make variables like WLAN_CHIP, HOST_OPENOCD & HOST_ARCH available to all makefiles
$(eval CURDIR := $(SOURCE_ROOT)board/$(PLATFORM_DIRECTORY)/)

include $(SOURCE_ROOT)board/$(PLATFORM_DIRECTORY)/aos.mk

$(eval CURDIR := $($(HOST_MCU_FAMILY)_LOCATION)/)
include $($(HOST_MCU_FAMILY)_LOCATION)/aos.mk
MAIN_COMPONENT_PROCESSING :=1

# Now we know the target architecture - include all toolchain makefiles and check one of them can handle the architecture
CC :=

ifeq ($(COMPILER),armcc)
include $(MAKEFILES_PATH)/toolchain/aos_toolchain_armcc.mk
else ifeq ($(COMPILER),rvct)
include $(MAKEFILES_PATH)/toolchain/aos_toolchain_rvct.mk
else ifeq ($(COMPILER),iar)
include $(MAKEFILES_PATH)/toolchain/aos_toolchain_iar.mk
else
include $(MAKEFILES_PATH)/toolchain/aos_toolchain_gcc.mk
endif

ifndef CC
$(error No matching toolchain found for architecture $(HOST_ARCH))
endif

# MBINS build support
ifeq ($(MBINS),app)
ifeq ($(ENABLE_USPACE),1)
COMPONENTS += mm
COMPONENTS += uspace
else
COMPONENTS += mbins.umbins
AOS_SDK_LDFLAGS += -Wl,-wrap,vprintf -Wl,-wrap,fflsuh -nostartfiles
endif
AOS_SDK_DEFINES += BUILD_APP
AOS_SDK_LDFLAGS += -nostartfiles
else ifeq ($(MBINS),kernel)
ifeq ($(ENABLE_USPACE),1)
COMPONENTS += uspace
else
COMPONENTS += mbins.kmbins
endif
AOS_SDK_DEFINES += BUILD_KERNEL
else ifeq (,$(MBINS))
AOS_SDK_DEFINES += BUILD_BIN
endif

# Process component metadata
ALL_MAKEFILES :=
CURDIR :=
$(info processing components: $(COMPONENTS))
$(eval $(call FIND_VARIOUS_COMPONENT, $(COMPONENTS)))
$(eval COMPONENTS := $(sort $(COMPONENTS)) )
$(eval $(call PROCESS_COMPONENT, $(PROCESSED_COMPONENTS_LOCS)))

AOS_SDK_INCLUDES += -I$(SOURCE_ROOT)include \
                    -I$(SOURCE_ROOT)include/hal \
                    -I$(SOURCE_ROOT)include/hal/soc \
                    -I$(SOURCE_ROOT)include/network

AOS_SDK_DEFINES += $(EXTERNAL_AOS_GLOBAL_DEFINES)

# Make sure the user has specified a component from each category
$(if $(PLATFORM),,$(error No platform specified. Options are: $(notdir $(wildcard board/*))))
$(if $(APP),,$(error No application specified. Options are: $(notdir $(wildcard app/example/*))))

# Make sure a WLAN_CHIP, WLAN_CHIP_REVISION, WLAN_CHIP_FAMILY and HOST_OPENOCD have been defined
#$(if $(WLAN_CHIP),,$(error No WLAN_CHIP has been defined))
#$(if $(WLAN_CHIP_REVISION),,$(error No WLAN_CHIP_REVISION has been defined))
#$(if $(WLAN_CHIP_FAMILY),,$(error No WLAN_CHIP_FAMILY has been defined))
$(if $(HOST_OPENOCD),,$(error No HOST_OPENOCD has been defined))

VALID_PLATFORMS :=
INVALID_PLATFORMS :=

$(eval VALID_PLATFORMS := $(call EXPAND_WILDCARD_PLATFORMS,$(VALID_PLATFORMS)))
$(eval INVALID_PLATFORMS := $(call EXPAND_WILDCARD_PLATFORMS,$(INVALID_PLATFORMS)))

# Check for valid platform, OSNS combination, build type, image type and bus
$(eval $(if $(VALID_PLATFORMS), $(if $(filter $(VALID_PLATFORMS),$(PLATFORM)),,$(error $(APP) application does not support $(PLATFORM) platform)),))
$(eval $(if $(INVALID_PLATFORMS), $(if $(filter $(INVALID_PLATFORMS),$(PLATFORM)),$(error $(APP) application does not support $(PLATFORM) platform)),))
$(eval $(if $(VALID_BUILD_TYPES), $(if $(filter $(VALID_BUILD_TYPES),$(BUILD_TYPE)),,$(error $(APP) application does not support $(BUILD_TYPE) build)),))

# Write python dict DEPENDENCY_DICT to json file:
#   DEPENDENCY_DICT = "{ comp: [deplist], ... }"
dependency_file:
	$(eval DEPENDENCY_DICT = "{ $(DEPENDENCY_DICT) }")
	$(PYTHON) $(SCRIPTS_PATH)/component_dependencies.py $(OUTPUT_DIR) $(DEPENDENCY_DICT)

# Support for extra targets defined by component
EXTRA_TARGET_MAKEFILES :=$(call unique,$(EXTRA_TARGET_MAKEFILES))
$(foreach makefile_name,$(EXTRA_TARGET_MAKEFILES),$(eval include $(makefile_name)))

# Summarize all the information into the config file

# Fill out full CFLAGS - done here to allow late expansion of macros
$(foreach comp,$(PROCESSED_COMPONENTS), $(eval $(comp)_CFLAGS_ALL := $(call ADD_COMPILER_SPECIFIC_STANDARD_CFLAGS,$($(comp)_OPTIM_CFLAGS))) )
$(foreach comp,$(PROCESSED_COMPONENTS), $(eval $(comp)_CFLAGS_ALL += $(EXTRA_CFLAGS)) )
$(foreach comp,$(PROCESSED_COMPONENTS), $(eval $(comp)_CFLAGS_ALL += $($(comp)_CFLAGS)) )

$(foreach comp,$(PROCESSED_COMPONENTS), $(eval $(comp)_CXXFLAGS_ALL := $(call ADD_COMPILER_SPECIFIC_STANDARD_CXXFLAGS,$($(comp)_OPTIM_CXXFLAGS))) )
$(foreach comp,$(PROCESSED_COMPONENTS), $(eval $(comp)_CXXFLAGS_ALL += $(EXTRA_CFLAGS)) )
$(foreach comp,$(PROCESSED_COMPONENTS), $(eval $(comp)_CXXFLAGS_ALL += $($(comp)_CXXFLAGS)) )

# select the prebuilt libraries
ifeq (app, $(MBINS))
AOS_SDK_PREBUILT_LIBRARIES +=$(foreach comp,$(PROCESSED_COMPONENTS), $(if $($(comp)_MBINS_TYPE), $(if $(filter app share, $($(comp)_MBINS_TYPE)),$(addprefix $($(comp)_LOCATION),$($(comp)_PREBUILT_LIBRARY))), $(addprefix $($(comp)_LOCATION),$($(comp)_PREBUILT_LIBRARY))))
else ifeq (kernel, $(MBINS))
AOS_SDK_PREBUILT_LIBRARIES +=$(foreach comp,$(PROCESSED_COMPONENTS), $(if $(filter kernel share, $($(comp)_MBINS_TYPE)), $(addprefix $($(comp)_LOCATION),$($(comp)_PREBUILT_LIBRARY))))
else ifeq (, $(MBINS))
AOS_SDK_PREBUILT_LIBRARIES +=$(foreach comp,$(PROCESSED_COMPONENTS), $(addprefix $($(comp)_LOCATION),$($(comp)_PREBUILT_LIBRARY)))
endif

AOS_SDK_LINK_FILES         +=$(foreach comp,$(PROCESSED_COMPONENTS), $(addprefix $$(OUTPUT_DIR)/Modules/$(call GET_BARE_LOCATION,$(comp)),$($(comp)_LINK_FILES)))
AOS_SDK_UNIT_TEST_SOURCES  +=$(foreach comp,$(PROCESSED_COMPONENTS), $(addprefix $($(comp)_LOCATION),$($(comp)_UNIT_TEST_SOURCES)))

ifeq ($(ADD_UNIT_TESTS_TO_LINK_FILES),1)
AOS_SDK_LINK_FILES         += $(patsubst %.cpp,%.o,$(patsubst %.cc,%.o,$(patsubst %.c,%.o, $(foreach comp,$(PROCESSED_COMPONENTS), $(addprefix $$(OUTPUT_DIR)/Modules/$(call GET_BARE_LOCATION,$(comp)),$($(comp)_UNIT_TEST_SOURCES))) )))
endif

# Build target, generate config file
.PHONY: $(MAKECMDGOALS)
$(MAKECMDGOALS): $(CONFIG_FILE) $(TOOLCHAIN_HOOK_TARGETS)

$(CONFIG_FILE_DIR):
	$(QUIET)$(call MKDIR, $@)

ifeq ($(NOCONFIG),)
$(CONFIG_FILE): $(AOS_CONFIG_DIR)/autoconf.h
endif

# Create config.mk
$(CONFIG_FILE): $(AOS_SDK_MAKEFILES) | $(CONFIG_FILE_DIR)
	$(QUIET)$(call WRITE_FILE_CREATE, $(CONFIG_FILE) ,AOS_SDK_MAKEFILES           		+= $(AOS_SDK_MAKEFILES))
	$(QUIET)$(call WRITE_FILE_APPEND, $(CONFIG_FILE) ,TOOLCHAIN_NAME            		:= $(TOOLCHAIN_NAME))
	$(QUIET)$(call WRITE_FILE_APPEND, $(CONFIG_FILE) ,AOS_SDK_LDFLAGS             		+= $(strip $(AOS_SDK_LDFLAGS)))
	$(QUIET)$(call WRITE_FILE_APPEND, $(CONFIG_FILE) ,AOS_SDK_LDS_FILES                     += $(strip $(AOS_SDK_LDS_FILES)))
	$(QUIET)$(call WRITE_FILE_APPEND, $(CONFIG_FILE) ,AOS_SDK_LDS_INCLUDES                  += $(strip $(AOS_SDK_LDS_INCLUDES)))
	$(QUIET)$(call WRITE_FILE_APPEND, $(CONFIG_FILE) ,AOS_SDK_2BOOT_SUPPORT                 += $(strip $(AOS_SDK_2BOOT_SUPPORT)))
	$(QUIET)$(call WRITE_FILE_APPEND, $(CONFIG_FILE) ,AOS_SDK_IMG1_XIP1_LD_FILE                 += $(strip $(AOS_SDK_IMG1_XIP1_LD_FILE)))
	$(QUIET)$(call WRITE_FILE_APPEND, $(CONFIG_FILE) ,AOS_SDK_IMG2_XIP2_LD_FILE                 += $(strip $(AOS_SDK_IMG2_XIP2_LD_FILE)))
	$(QUIET)$(call WRITE_FILE_APPEND, $(CONFIG_FILE) ,AOS_SDK_CFLAGS			+= $(strip $(AOS_SDK_CFLAGS)))
	$(QUIET)$(call WRITE_FILE_APPEND, $(CONFIG_FILE) ,AOS_SDK_ASMFLAGS			+= $(strip $(AOS_SDK_ASMFLAGS)))
	$(QUIET)$(call WRITE_FILE_APPEND, $(CONFIG_FILE) ,AOS_SDK_LINK_SCRIPT         		+= $(strip $(if $(strip $(AOS_SDK_LINK_SCRIPT)),$(AOS_SDK_LINK_SCRIPT),$(AOS_SDK_DEFAULT_LINK_SCRIPT))))
	$(QUIET)$(call WRITE_FILE_APPEND, $(CONFIG_FILE) ,AOS_SDK_LINK_SCRIPT_CMD    	 	+= $(call COMPILER_SPECIFIC_LINK_SCRIPT,$(strip $(if $(strip $(AOS_SDK_LINK_SCRIPT)),$(AOS_SDK_LINK_SCRIPT),$(AOS_SDK_DEFAULT_LINK_SCRIPT)))))
	$(QUIET)$(call WRITE_FILE_APPEND, $(CONFIG_FILE) ,AOS_SDK_PREBUILT_LIBRARIES 	 	+= $(strip $(AOS_SDK_PREBUILT_LIBRARIES)))
	$(QUIET)$(call WRITE_FILE_APPEND, $(CONFIG_FILE) ,AOS_SDK_CERTIFICATES       	 	+= $(strip $(AOS_SDK_CERTIFICATES)))
	$(QUIET)$(call WRITE_FILE_APPEND, $(CONFIG_FILE) ,AOS_SDK_PRE_APP_BUILDS      		+= $(strip $(PRE_APP_BUILDS)))
	$(QUIET)$(call WRITE_FILE_APPEND, $(CONFIG_FILE) ,AOS_SDK_LINK_FILES          		+= $(AOS_SDK_LINK_FILES))
	$(QUIET)$(call WRITE_FILE_APPEND, $(CONFIG_FILE) ,AOS_SDK_INCLUDES           	 	+= $(call unique,$(AOS_SDK_INCLUDES)))
	$(QUIET)$(call WRITE_FILE_APPEND, $(CONFIG_FILE) ,AOS_SDK_DEFINES             		+= $(call unique,$(strip $(addprefix -D,$(AOS_SDK_DEFINES)))))
	$(QUIET)$(call WRITE_FILE_APPEND, $(CONFIG_FILE) ,COMPONENTS                		:= $(PROCESSED_COMPONENTS))
	$(QUIET)$(call WRITE_FILE_APPEND, $(CONFIG_FILE) ,PLATFORM_DIRECTORY        		:= $(PLATFORM_DIRECTORY))
	$(QUIET)$(call WRITE_FILE_APPEND, $(CONFIG_FILE) ,APP_FULL                  		:= $(APP_FULL))
	$(QUIET)$(call WRITE_FILE_APPEND, $(CONFIG_FILE) ,PLATFORM                  		:= $(PLATFORM))
	$(QUIET)$(call WRITE_FILE_APPEND, $(CONFIG_FILE) ,HOST_MCU_FAMILY                  	:= $(HOST_MCU_FAMILY))
	$(QUIET)$(call WRITE_FILE_APPEND, $(CONFIG_FILE) ,SUPPORT_MBINS                          := $(SUPPORT_MBINS))
	$(QUIET)$(call WRITE_FILE_APPEND, $(CONFIG_FILE) ,APP                       		:= $(APP))
	$(QUIET)$(call WRITE_FILE_APPEND, $(CONFIG_FILE) ,HOST_OPENOCD              		:= $(HOST_OPENOCD))
	$(QUIET)$(call WRITE_FILE_APPEND, $(CONFIG_FILE) ,JTAG              		        := $(JTAG))
	$(QUIET)$(call WRITE_FILE_APPEND, $(CONFIG_FILE) ,HOST_ARCH                 		:= $(HOST_ARCH))
	$(QUIET)$(call WRITE_FILE_APPEND, $(CONFIG_FILE) ,NO_BUILD_BOOTLOADER           	:= $(NO_BUILD_BOOTLOADER))
	$(QUIET)$(call WRITE_FILE_APPEND, $(CONFIG_FILE) ,NO_BOOTLOADER_REQUIRED         	:= $(NO_BOOTLOADER_REQUIRED))
	$(QUIET)$(foreach comp,$(PROCESSED_COMPONENTS), $(call WRITE_FILE_APPEND, $(CONFIG_FILE) ,$(comp)_LOCATION         := $($(comp)_LOCATION)))
	$(QUIET)$(foreach comp,$(PROCESSED_COMPONENTS), $(call WRITE_FILE_APPEND, $(CONFIG_FILE) ,$(comp)_SOURCES          += $($(comp)_SOURCES)))
	$(QUIET)$(foreach comp,$(PROCESSED_COMPONENTS), $(call WRITE_FILE_APPEND, $(CONFIG_FILE) ,$(comp)_CHECK_HEADERS    += $($(comp)_CHECK_HEADERS)))
	$(QUIET)$(foreach comp,$(PROCESSED_COMPONENTS), $(call WRITE_FILE_APPEND, $(CONFIG_FILE) ,$(comp)_INCLUDES         := $(addprefix -I$($(comp)_LOCATION),$($(comp)_INCLUDES))))
	$(QUIET)$(foreach comp,$(PROCESSED_COMPONENTS), $(call WRITE_FILE_APPEND, $(CONFIG_FILE) ,$(comp)_DEFINES          := $(addprefix -D,$($(comp)_DEFINES))))
	$(QUIET)$(foreach comp,$(PROCESSED_COMPONENTS), $(call WRITE_FILE_APPEND, $(CONFIG_FILE) ,$(comp)_CFLAGS           := $(AOS_SDK_CFLAGS) $($(comp)_CFLAGS_ALL)))
	$(QUIET)$(foreach comp,$(PROCESSED_COMPONENTS), $(call WRITE_FILE_APPEND, $(CONFIG_FILE) ,$(comp)_CXXFLAGS         := $(AOS_SDK_CXXFLAGS) $($(comp)_CXXFLAGS_ALL)))
	$(QUIET)$(foreach comp,$(PROCESSED_COMPONENTS), $(call WRITE_FILE_APPEND, $(CONFIG_FILE) ,$(comp)_ASMFLAGS         := $(strip $(AOS_SDK_ASMFLAGS) $(EXTRA_ASMFLAGS) $($(comp)_ASMFLAGS))))
	$(QUIET)$(foreach comp,$(PROCESSED_COMPONENTS), $(call WRITE_FILE_APPEND, $(CONFIG_FILE) ,$(comp)_MAKEFILE         := $($(comp)_MAKEFILE)))
	$(QUIET)$(foreach comp,$(PROCESSED_COMPONENTS), $(call WRITE_FILE_APPEND, $(CONFIG_FILE) ,$(comp)_PRE_BUILD_TARGETS:= $($(comp)_PRE_BUILD_TARGETS)))
	$(QUIET)$(foreach comp,$(PROCESSED_COMPONENTS), $(call WRITE_FILE_APPEND, $(CONFIG_FILE) ,$(comp)_PREBUILT_LIBRARY := $(addprefix $($(comp)_LOCATION),$($(comp)_PREBUILT_LIBRARY))))
	$(QUIET)$(foreach comp,$(PROCESSED_COMPONENTS), $(call WRITE_FILE_APPEND, $(CONFIG_FILE) ,$(comp)_MBINS_TYPE             := $($(comp)_MBINS_TYPE)))
	$(QUIET)$(foreach comp,$(PROCESSED_COMPONENTS), $(call WRITE_FILE_APPEND, $(CONFIG_FILE) ,$(comp)_POPULATE_INCLUDES      := $($(comp)_POPULATE_INCLUDES)))
	$(QUIET)$(foreach var,$(sort $(FEATURE_SHOW_VARS)), $(call WRITE_FILE_APPEND, $(CONFIG_FILE) ,$(var) := $($(var))))
	$(QUIET)$(call WRITE_FILE_APPEND, $(CONFIG_FILE) ,OBJ-y := $(sort $(OBJ-y)))
	$(QUIET)$(call WRITE_FILE_APPEND, $(CONFIG_FILE) ,AOS_SDK_UNIT_TEST_SOURCES   		:= $(AOS_SDK_UNIT_TEST_SOURCES))
	$(QUIET)$(call WRITE_FILE_APPEND, $(CONFIG_FILE) ,INTERNAL_MEMORY_RESOURCES 		:= $(call unique,$(INTERNAL_MEMORY_RESOURCES)))
	$(QUIET)$(call WRITE_FILE_APPEND, $(CONFIG_FILE) ,EXTRA_TARGET_MAKEFILES 			:= $(EXTRA_TARGET_MAKEFILES))
	$(QUIET)$(call WRITE_FILE_APPEND, $(CONFIG_FILE) ,APPS_START_SECTOR 				:= $(APPS_START_SECTOR) )
	$(QUIET)$(call WRITE_FILE_APPEND, $(CONFIG_FILE) ,BOOTLOADER_FIRMWARE				:= $(BOOTLOADER_FIRMWARE) )
	$(QUIET)$(call WRITE_FILE_APPEND, $(CONFIG_FILE) ,ATE_FIRMWARE				        := $(ATE_FIRMWARE) )
	$(QUIET)$(call WRITE_FILE_APPEND, $(CONFIG_FILE) ,APPLICATION_FIRMWARE				:= $(APPLICATION_FIRMWARE) )
	$(QUIET)$(call WRITE_FILE_APPEND, $(CONFIG_FILE) ,PARAMETER_1_IMAGE					:= $(PARAMETER_1_IMAGE) )
	$(QUIET)$(call WRITE_FILE_APPEND, $(CONFIG_FILE) ,PARAMETER_2_IMAGE					:= $(PARAMETER_2_IMAGE) )
	$(QUIET)$(call WRITE_FILE_APPEND, $(CONFIG_FILE) ,FILESYSTEM_IMAGE					:= $(FILESYSTEM_IMAGE) )
	$(QUIET)$(call WRITE_FILE_APPEND, $(CONFIG_FILE) ,WIFI_FIRMWARE						:= $(WIFI_FIRMWARE) )
	$(QUIET)$(call WRITE_FILE_APPEND, $(CONFIG_FILE) ,BT_PATCH_FIRMWARE					:= $(BT_PATCH_FIRMWARE) )
	$(QUIET)$(call WRITE_FILE_APPEND, $(CONFIG_FILE) ,AOS_ROM_SYMBOL_LIST_FILE 		:= $(AOS_ROM_SYMBOL_LIST_FILE))
	$(QUIET)$(call WRITE_FILE_APPEND, $(CONFIG_FILE) ,AOS_SDK_CHIP_SPECIFIC_SCRIPT		:= $(AOS_SDK_CHIP_SPECIFIC_SCRIPT))
	$(QUIET)$(call WRITE_FILE_APPEND, $(CONFIG_FILE) ,AOS_SDK_CONVERTER_OUTPUT_FILE	:= $(AOS_SDK_CONVERTER_OUTPUT_FILE))
	$(QUIET)$(call WRITE_FILE_APPEND, $(CONFIG_FILE) ,AOS_SDK_FINAL_OUTPUT_FILE 		:= $(AOS_SDK_FINAL_OUTPUT_FILE))
	$(QUIET)$(call WRITE_FILE_APPEND, $(CONFIG_FILE) ,AOS_RAM_STUB_LIST_FILE 			:= $(AOS_RAM_STUB_LIST_FILE))
	$(QUIET)$(call WRITE_FILE_APPEND, $(CONFIG_FILE) ,PING_PONG_OTA 					:= $(PING_PONG_OTA))
	$(QUIET)$(call WRITE_FILE_APPEND, $(CONFIG_FILE) ,AOS_CPLUSPLUS_FLAGS:= $(AOS_CPLUSPLUS_FLAGS))
	$(QUIET)$(call WRITE_FILE_APPEND, $(CONFIG_FILE) ,KEIL_DEVICE := $(foreach comp,$(PROCESSED_COMPONENTS),$(if $($(comp)_KEIL_DEVICE),$($(comp)_KEIL_DEVICE),)))
	$(QUIET)$(call WRITE_FILE_APPEND, $(CONFIG_FILE) ,KEIL_VENDOR := $(foreach comp,$(PROCESSED_COMPONENTS),$(if $($(comp)_KEIL_VENDOR),$($(comp)_KEIL_VENDOR),)))
	$(QUIET)$(call WRITE_FILE_APPEND, $(CONFIG_FILE) ,IAR_OGCMENU := $(foreach comp,$(PROCESSED_COMPONENTS),$(if $($(comp)_IAR_OGCMENU),$($(comp)_IAR_OGCMENU),)))
