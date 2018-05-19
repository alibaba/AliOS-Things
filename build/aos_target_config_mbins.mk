include $(MAKEFILES_PATH)/aos_host_cmd.mk

APPDIR ?=
CONFIG_FILE_DIR := $(OUTPUT_DIR)
CONFIG_FILE := $(CONFIG_FILE_DIR)/config.mk


COMPONENT_DIRECTORIES := . \
                         example   \
                         board     \
                         kernel    \
                         platform  \
                         utility   \
                         framework \
                         tools     \
                         test      \
                         device    \
                         security  \
			 app

TEST_COMPONENT_DIRECTORIES := test

ifneq ($(ONLY_BUILD_LIBRARY), yes)
COMPONENT_DIRECTORIES += $(OUTPUT_DIR)/syscall
COMPONENT_DIRECTORIES += $(OUTPUT_DIR)
endif

COMPONENT_DIRECTORIES += $(APPDIR)

AOS_SDK_VERSION ?= $(AOS_SDK_VERSION_MAJOR).$(AOS_SDK_VERSION_MINOR).$(AOS_SDK_VERSION_REVISION)

##################################
# Macros
##################################

# $(1) is component
GET_BARE_LOCATION =$(patsubst $(call ESCAPE_BACKSLASHES,$(SOURCE_ROOT))%,%,$(strip $($(1)_LOCATION)))

define PREPROCESS_TEST_COMPONENT
$(if $(filter yts,$(COMPONENTS)), \
$(if $(test), $(eval TEST_COMPONENTS := $(strip $(subst $(COMMA), $(SPACE), $(test)))),) \
$(if $(TEST_COMPONENTS), $(call RECURSE_DIR_COMPONENT_SEARCH, $(patsubst %/,%,$(TEST_COMPONENT_DIRECTORIES)), TEST_COMPONENT_LIST) \
$(eval TEST_COMPONENTS := $(addprefix %., $(addsuffix _test, $(TEST_COMPONENTS)))) \
$(eval COMPONENTS += $(filter $(TEST_COMPONENTS),  $(subst /,.,$(strip $(TEST_COMPONENT_LIST)))))))
endef

#####################################################################################
# Macro FIND_COMPONENT  use breadth traversal to search component
# $(1) is the list of components left to process. $(COMP) is set as the first element in the list
define FIND_COMPONENT

$(eval COMP := $(word 1,$(1)))
$(eval COMP_LOCATION := $(subst .,/,$(COMP)))
$(eval COMP_MAKEFILE_NAME := $(notdir $(COMP_LOCATION)))
# Find the component makefile in directory list
$(eval TEMP_MAKEFILE := $(strip $(wildcard $(foreach dir, $(if $(filter-out out, $(BUILD_DIR)),$(OUTPUT_DIR) $(OUTPUT_DIR)/syscall,) $(if $(APPDIR),$(APPDIR),) $(if $(CUBE_AOS_DIR),$(CUBE_AOS_DIR) $(CUBE_AOS_DIR)/remote,) $(addprefix $(SOURCE_ROOT),$(COMPONENT_DIRECTORIES)), $(dir)/$(COMP_LOCATION)/$(COMP_MAKEFILE_NAME).mk))))
# Check if component makefile was found - if not try downloading it and re-doing the makefile search
$(if $(TEMP_MAKEFILE),,\
	 $(info Unknown component: $(COMP) - directory or makefile for component not found. Ensure the $(COMP_LOCATION) directory contains $(COMP_MAKEFILE_NAME).mk) \
	 $(info Below is a list of valid local components (Some are internal): ) \
	 $(call FIND_VALID_COMPONENTS, VALID_COMPONENT_LIST,$(COMPONENT_DIRECTORIES)) \
     $(foreach comp,$(VALID_COMPONENT_LIST),$(info $(comp))) \
     $(info Below is a list of valid components from the internet: ) \
     $(info $(call DOWNLOAD_COMPONENT_LIST)) \
     $(error Unknown component: $(COMP) - directory or makefile for component not found. Ensure the $(COMP_LOCATION) directory contains $(COMP_MAKEFILE_NAME).mk))
$(if $(filter 1,$(words $(TEMP_MAKEFILE))),,$(error More than one component with the name "$(COMP)". See $(TEMP_MAKEFILE)))

$(eval TEMP_MAKEFILE := $(subst ././,./,$(TEMP_MAKEFILE)))
$(eval include $(TEMP_MAKEFILE))
$(eval deps :=)
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
	$(call PREPROCESS_TEST_COMPONENT, $(COMPONENTS), $(TEST_COMPONENTS)) \
	DEPENDENCY += '$(NAME)': '$($(NAME)_COMPONENTS)',)

$(eval PROCESSED_COMPONENTS_LOCS += $(COMP))
$(if $(strip $(filter-out $(PROCESSED_COMPONENTS_LOCS),$(COMPONENTS))),\
     $(call FIND_COMPONENT,$(filter-out $(PROCESSED_COMPONENTS_LOCS),$(COMPONENTS))),\
)
endef

#####################################################################################
# Macro PROCESS_ONE_COMPONENT
# $(1) is one component
define PROCESS_ONE_COMPONENT
$(eval COMP := $(1))
$(eval COMP_LOCATION := $(subst .,/,$(COMP)))
$(eval COMP_MAKEFILE_NAME := $(notdir $(COMP_LOCATION)))
# Find the component makefile in directory list
$(eval TEMP_MAKEFILE := $(strip $(wildcard $(foreach dir, $(if $(filter-out out, $(BUILD_DIR)),$(OUTPUT_DIR) $(OUTPUT_DIR)/syscall,) $(if $(APPDIR),$(APPDIR)/$(comp),) $(if $(CUBE_AOS_DIR),$(CUBE_AOS_DIR) $(CUBE_AOS_DIR)/remote) $(addprefix $(SOURCE_ROOT),$(COMPONENT_DIRECTORIES)), $(dir)/$(COMP_LOCATION)/$(COMP_MAKEFILE_NAME).mk))))

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

# Expand the list of resources to point to the full location (either component local or the common resources directory)
$(eval $(NAME)_RESOURCES_EXPANDED := $(foreach res,$($(NAME)_RESOURCES),$(word 1,$(wildcard $(addsuffix $(res),$(CURDIR) $(SOURCE_ROOT)resources/)))))

$(eval CURDIR := $(OLD_CURDIR))

$(eval $(NAME)_LOCATION := $(dir $(TEMP_MAKEFILE)))
$(eval $(NAME)_MAKEFILE := $(TEMP_MAKEFILE))
AOS_SDK_MAKEFILES     += $($(NAME)_MAKEFILE)

# Set debug/release specific options
$(eval $(NAME)_BUILD_TYPE := $(BUILD_TYPE))
$(eval $(NAME)_BUILD_TYPE := $(if $($(NAME)_NEVER_OPTIMISE),  debug,   $($(NAME)_BUILD_TYPE)))
$(eval $(NAME)_BUILD_TYPE := $(if $($(NAME)_ALWAYS_OPTIMISE), release, $($(NAME)_BUILD_TYPE)))

$(NAME)_ASMFLAGS += $(if $(findstring debug,$($(NAME)_BUILD_TYPE)), $(COMPILER_SPECIFIC_DEBUG_ASFLAGS),  $(COMPILER_SPECIFIC_RELEASE_ASFLAGS))
$(NAME)_LDFLAGS  += $(if $(findstring debug,$($(NAME)_BUILD_TYPE)), $(COMPILER_SPECIFIC_DEBUG_LDFLAGS),  $(COMPILER_SPECIFIC_RELEASE_LDFLAGS))

$(NAME)_OPTIM_CFLAGS   ?= $(if $(findstring debug,$($(NAME)_BUILD_TYPE)), $(COMPILER_SPECIFIC_DEBUG_CFLAGS), $(if $(findstring release_log,$($(NAME)_BUILD_TYPE)), $(COMPILER_SPECIFIC_RELEASE_LOG_CFLAGS), $(COMPILER_SPECIFIC_RELEASE_CFLAGS)))

$(NAME)_OPTIM_CXXFLAGS ?= $(if $(findstring debug,$($(NAME)_BUILD_TYPE)), $(COMPILER_SPECIFIC_DEBUG_CXXFLAGS), $(if $(findstring release_log,$($(NAME)_BUILD_TYPE)), $(COMPILER_SPECIFIC_RELEASE_LOG_CXXFLAGS), $(COMPILER_SPECIFIC_RELEASE_CXXFLAGS)))

AOS_SDK_INCLUDES           +=$(addprefix -I$($(NAME)_LOCATION),$(GLOBAL_INCLUDES))
AOS_SDK_LINK_SCRIPT        +=$(if $(GLOBAL_LINK_SCRIPT),$(GLOBAL_LINK_SCRIPT),)
AOS_SDK_DEFAULT_LINK_SCRIPT+=$(if $(DEFAULT_LINK_SCRIPT),$(addprefix $($(NAME)_LOCATION),$(DEFAULT_LINK_SCRIPT)),)
AOS_SDK_DEFINES            +=$(GLOBAL_DEFINES)
AOS_SDK_CFLAGS             +=$(GLOBAL_CFLAGS)
AOS_SDK_CXXFLAGS           +=$(GLOBAL_CXXFLAGS)
AOS_SDK_ASMFLAGS           +=$(GLOBAL_ASMFLAGS)
AOS_SDK_LDFLAGS            +=$(GLOBAL_LDFLAGS)
AOS_SDK_LDS_FILES          +=$(GLOBAL_LDS_FILES)
AOS_SDK_LDS_INCLUDES       +=$(GLOBAL_LDS_INCLUDES)
AOS_SDK_CHIP_SPECIFIC_SCRIPT += $(CHIP_SPECIFIC_SCRIPT)
AOS_SDK_CONVERTER_OUTPUT_FILE += $(CONVERTER_OUTPUT_FILE)
AOS_SDK_FINAL_OUTPUT_FILE += $(BIN_OUTPUT_FILE)

$(eval PROCESSED_COMPONENTS += $(NAME))

$(eval $(NAME)_SOURCES := $(sort $($(NAME)_SOURCES)) )

endef


#####################################################################################
# Macro PROCESS_COMPONENT
# $(1) is the list of components left to process. $(COMP) is set as the first element in the list
define PROCESS_COMPONENT
$(foreach TMP_COMP, $(COMPONENTS),$(call PROCESS_ONE_COMPONENT, $(TMP_COMP)))
endef

##################################
# Start of processing
##################################

# Separate the build string into components
COMPONENTS := $(subst @, ,$(MAKECMDGOALS))


ifneq (,$(filter mk3060,$(COMPONENTS)))
ifneq (,$(filter bootloader,$(COMPONENTS)))
$(error mk3060 doesn't support bootlaoder option)
endif
endif

#Dependency python dict start
DEPENDENCY := "{

BUILD_TYPE_LIST := debug \
                   release_log \
                   release

# Extract out: the debug/release option, OTA option, and the lint option
BUILD_TYPE          := $(if $(filter $(BUILD_TYPE_LIST),$(COMPONENTS)),$(firstword $(filter $(BUILD_TYPE_LIST),$(COMPONENTS))),release_log)
COMPONENTS          := $(filter-out $(BUILD_TYPE_LIST), $(COMPONENTS))

# Set debug/release specific options
ifeq ($(BUILD_TYPE),release)
AOS_SDK_LDFLAGS  += $(COMPILER_SPECIFIC_RELEASE_LDFLAGS)
else
AOS_SDK_LDFLAGS  += $(COMPILER_SPECIFIC_DEBUG_LDFLAGS)
endif


# Check if there are any unknown components; output error if so.
$(foreach comp, $(COMPONENTS), $(if $(wildcard $(APPDIR)/$(comp) $(CUBE_AOS_DIR)/$(comp) $(foreach dir, $(addprefix $(SOURCE_ROOT),$(COMPONENT_DIRECTORIES)), $(dir)/$(subst .,/,$(comp)) ) ),,$(error Unknown component: $(comp))))

# Find the matching platform and application from the build string components
PLATFORM_FULL   :=$(strip $(foreach comp,$(subst .,/,$(COMPONENTS)),$(if $(wildcard $(SOURCE_ROOT)board/$(comp)),$(comp),)))
APP_FULL        :=$(strip $(foreach comp,$(subst .,/,$(COMPONENTS)),$(if $(wildcard $(APPDIR)/$(comp) $(SOURCE_ROOT)example/$(comp) $(SOURCE_ROOT)$(comp)),$(comp),)))

PLATFORM    :=$(notdir $(PLATFORM_FULL))
APP         :=$(notdir $(APP_FULL))

PLATFORM_DIRECTORY := $(PLATFORM_FULL)

EXTRA_CFLAGS :=    -DAOS_SDK_VERSION_MAJOR=$(AOS_SDK_VERSION_MAJOR) \
                   -DAOS_SDK_VERSION_MINOR=$(AOS_SDK_VERSION_MINOR) \
                   -DAOS_SDK_VERSION_REVISION=$(AOS_SDK_VERSION_REVISION) \
                   -I$(OUTPUT_DIR)/resources/  \
                   -DPLATFORM=$(SLASH_QUOTE_START)$$(PLATFORM)$(SLASH_QUOTE_END)

# Load platform makefile to make variables like WLAN_CHIP, HOST_OPENOCD & HOST_ARCH available to all makefiles
$(eval CURDIR := $(SOURCE_ROOT)board/$(PLATFORM_DIRECTORY)/)

include $(SOURCE_ROOT)board/$(PLATFORM_DIRECTORY)/$(notdir $(PLATFORM_DIRECTORY)).mk

PLATFORM_MCU_BOARD	:=$(subst .,/,$(HOST_MCU_FAMILY))
PLATFORM_MCU_BD :=$(subst ., ,$(HOST_MCU_FAMILY))
PLATFORM_MCU_MK :=$(if ($(words $(PLATFORM_MCU_BD)):1= $(PLATFORM_MCU_BD)),$(word $(words $(PLATFORM_MCU_BD)),$(PLATFORM_MCU_BD)))

$(eval CURDIR := $(SOURCE_ROOT)/platform/mcu/$(PLATFORM_MCU_BOARD)/)
include $(SOURCE_ROOT)platform/mcu/$(PLATFORM_MCU_BOARD)/$(PLATFORM_MCU_MK).mk
MAIN_COMPONENT_PROCESSING :=1

# Now we know the target architecture - include all toolchain makefiles and check one of them can handle the architecture
CC :=

ifeq ($(COMPILER),armcc)
include $(MAKEFILES_PATH)/aos_toolchain_armcc.mk
else ifeq ($(COMPILER),iar)
include $(MAKEFILES_PATH)/aos_toolchain_iar.mk
else
include $(MAKEFILES_PATH)/aos_toolchain_gcc.mk
endif

ifndef CC
$(error No matching toolchain found for architecture $(HOST_ARCH))
endif



# Process all the components + AOS

COMPONENTS += platform/mcu/$(PLATFORM_MCU_BOARD) vcall

ifneq ($(ONLY_BUILD_LIBRARY), yes)
#COMPONENTS += auto_component
endif

ifeq ($(MBINS),app)
COMPONENTS += usyscall.umbins platform.mcu.pca10056.aos.app_runtime
AOS_SDK_DEFINES += BUILD_APP
AOS_SDK_LDFLAGS += -Wl,-wrap,vprintf -Wl,-wrap,fflush -nostartfiles
else ifeq ($(MBINS),kernel)
COMPONENTS += ksyscall.kmbins
AOS_SDK_DEFINES += BUILD_KERNEL
else ifeq (,$(MBINS))
AOS_SDK_DEFINES += BUILD_BIN
endif

ALL_MAKEFILES :=
ifneq ($(CUBE_MAKEFILE), )
include $(CUBE_MAKEFILE)
COMPONENTS += $(CUBE_ADD_COMPONENTS)
COMPONENT_DIRECTORIES += $(CUBE_AOS_DIR)
endif

CURDIR :=
$(info processing components: $(COMPONENTS))
$(eval $(call FIND_COMPONENT, $(COMPONENTS)))
# remove repeat component
$(eval COMPONENTS := $(sort $(COMPONENTS)) )
$(eval $(call PROCESS_COMPONENT, $(COMPONENTS)))

PLATFORM    :=$(notdir $(PLATFORM_FULL))

# Add some default values
AOS_SDK_INCLUDES += -I$(SOURCE_ROOT)include -I$(SOURCE_ROOT)example/$(APP_FULL)
AOS_SDK_DEFINES += $(EXTERNAL_AOS_GLOBAL_DEFINES)

ALL_RESOURCES := $(sort $(foreach comp,$(PROCESSED_COMPONENTS),$($(comp)_RESOURCES_EXPANDED)))

# Make sure the user has specified a component from each category
$(if $(PLATFORM),,$(error No platform specified. Options are: $(notdir $(wildcard board/*))))
$(if $(APP),,$(error No application specified. Options are: $(notdir $(wildcard example/*))))

# Make sure a WLAN_CHIP, WLAN_CHIP_REVISION, WLAN_CHIP_FAMILY and HOST_OPENOCD have been defined
#$(if $(WLAN_CHIP),,$(error No WLAN_CHIP has been defined))
#$(if $(WLAN_CHIP_REVISION),,$(error No WLAN_CHIP_REVISION has been defined))
#$(if $(WLAN_CHIP_FAMILY),,$(error No WLAN_CHIP_FAMILY has been defined))
$(if $(HOST_OPENOCD),,$(error No HOST_OPENOCD has been defined))

$(eval VALID_PLATFORMS := $(call EXPAND_WILDCARD_PLATFORMS,$(VALID_PLATFORMS)))
$(eval INVALID_PLATFORMS := $(call EXPAND_WILDCARD_PLATFORMS,$(INVALID_PLATFORMS)))

# Check for valid platform, OSNS combination, build type, image type and bus
$(eval $(if $(VALID_PLATFORMS), $(if $(filter $(VALID_PLATFORMS),$(PLATFORM)),,$(error $(APP) application does not support $(PLATFORM) platform)),))
$(eval $(if $(INVALID_PLATFORMS), $(if $(filter $(INVALID_PLATFORMS),$(PLATFORM)),$(error $(APP) application does not support $(PLATFORM) platform)),))
$(eval $(if $(VALID_BUILD_TYPES), $(if $(filter $(VALID_BUILD_TYPES),$(BUILD_TYPE)),,$(error $(APP) application does not support $(BUILD_TYPE) build)),))

ifneq ($(ONLY_BUILD_LIBRARY), yes)
#Dependency python dict end
DEPENDENCY += }"
#Call python script
$(eval DEPENDENCY = $(shell $(COMPONENT_DEPENDENCY) $(OUTPUT_DIR) $(DEPENDENCY)))

REMOVE_FIRST = $(wordlist 2,$(words $(1)),$(1))

EXTRA_TARGET_MAKEFILES :=$(call unique,$(EXTRA_TARGET_MAKEFILES))
$(foreach makefile_name,$(EXTRA_TARGET_MAKEFILES),$(eval include $(makefile_name)))

$(CONFIG_FILE_DIR):
	$(QUIET)$(call MKDIR, $@)

endif
# Summarize all the information into the config file


# Fill out full CFLAGS - done here to allow late expansion of macros
$(foreach comp,$(PROCESSED_COMPONENTS), $(eval $(comp)_CFLAGS_ALL := $(call ADD_COMPILER_SPECIFIC_STANDARD_CFLAGS,$($(comp)_OPTIM_CFLAGS))) )
$(foreach comp,$(PROCESSED_COMPONENTS), $(eval $(comp)_CFLAGS_ALL += $(EXTRA_CFLAGS)) )
$(foreach comp,$(PROCESSED_COMPONENTS), $(eval $(comp)_CFLAGS_ALL += $($(comp)_CFLAGS)) )

$(foreach comp,$(PROCESSED_COMPONENTS), $(eval $(comp)_CXXFLAGS_ALL := $(call ADD_COMPILER_SPECIFIC_STANDARD_CXXFLAGS,$($(comp)_OPTIM_CXXFLAGS))) )
$(foreach comp,$(PROCESSED_COMPONENTS), $(eval $(comp)_CXXFLAGS_ALL += $(EXTRA_CFLAGS)) )
$(foreach comp,$(PROCESSED_COMPONENTS), $(eval $(comp)_CXXFLAGS_ALL += $($(comp)_CXXFLAGS)) )

ifneq ($(ONLY_BUILD_LIBRARY), yes)
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

$(CONFIG_FILE): $(AOS_SDK_MAKEFILES) | $(CONFIG_FILE_DIR)
	$(QUIET)$(call WRITE_FILE_CREATE, $(CONFIG_FILE) ,AOS_SDK_MAKEFILES           		+= $(AOS_SDK_MAKEFILES))
	$(QUIET)$(call WRITE_FILE_APPEND, $(CONFIG_FILE) ,TOOLCHAIN_NAME            		:= $(TOOLCHAIN_NAME))
	$(QUIET)$(call WRITE_FILE_APPEND, $(CONFIG_FILE) ,AOS_SDK_LDFLAGS             		+= $(strip $(AOS_SDK_LDFLAGS)))
	$(QUIET)$(call WRITE_FILE_APPEND, $(CONFIG_FILE) ,AOS_SDK_LDS_FILES                     += $(strip $(AOS_SDK_LDS_FILES)))
	$(QUIET)$(call WRITE_FILE_APPEND, $(CONFIG_FILE) ,AOS_SDK_LDS_INCLUDES                  += $(strip $(AOS_SDK_LDS_INCLUDES)))
	$(QUIET)$(call WRITE_FILE_APPEND, $(CONFIG_FILE) ,RESOURCE_CFLAGS					+= $(strip $(AOS_SDK_CFLAGS)))
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
	$(QUIET)$(foreach comp,$(PROCESSED_COMPONENTS), $(call WRITE_FILE_APPEND, $(CONFIG_FILE) ,$(comp)_ASMFLAGS         := $(AOS_SDK_ASMFLAGS) $($(comp)_ASMFLAGS)))
	$(QUIET)$(foreach comp,$(PROCESSED_COMPONENTS), $(call WRITE_FILE_APPEND, $(CONFIG_FILE) ,$(comp)_RESOURCES        := $($(comp)_RESOURCES_EXPANDED)))
	$(QUIET)$(foreach comp,$(PROCESSED_COMPONENTS), $(call WRITE_FILE_APPEND, $(CONFIG_FILE) ,$(comp)_MAKEFILE         := $($(comp)_MAKEFILE)))
	$(QUIET)$(foreach comp,$(PROCESSED_COMPONENTS), $(call WRITE_FILE_APPEND, $(CONFIG_FILE) ,$(comp)_PRE_BUILD_TARGETS:= $($(comp)_PRE_BUILD_TARGETS)))
	$(QUIET)$(foreach comp,$(PROCESSED_COMPONENTS), $(call WRITE_FILE_APPEND, $(CONFIG_FILE) ,$(comp)_PREBUILT_LIBRARY := $(addprefix $($(comp)_LOCATION),$($(comp)_PREBUILT_LIBRARY))))
	$(QUIET)$(foreach comp,$(PROCESSED_COMPONENTS), $(call WRITE_FILE_APPEND, $(CONFIG_FILE) ,$(comp)_MBINS_TYPE             := $($(comp)_MBINS_TYPE)))
	$(QUIET)$(call WRITE_FILE_APPEND, $(CONFIG_FILE) ,AOS_SDK_UNIT_TEST_SOURCES   		:= $(AOS_SDK_UNIT_TEST_SOURCES))
	$(QUIET)$(call WRITE_FILE_APPEND, $(CONFIG_FILE) ,ALL_RESOURCES             		:= $(call unique,$(ALL_RESOURCES)))
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
endif

CONFIG_PY_FILE := build/scripts/config_mk.py

# write a component name in python format
define WRITE_COMPOENT_PY
$(call WRITE_FILE_APPEND, $(CONFIG_PY_FILE) ,{'name':'$(comp)'$(COMMA) )
$(call WRITE_FILE_APPEND, $(CONFIG_PY_FILE) ,'src':[ )
$(eval SOURCES_FULLPATH := $(addprefix $($(comp)_LOCATION), $($(comp)_SOURCES)))
$(foreach src,$(SOURCES_FULLPATH), $(call WRITE_FILE_APPEND, $(CONFIG_PY_FILE) ,'$(src)'$(COMMA)))
$(call WRITE_FILE_APPEND, $(CONFIG_PY_FILE) ,]$(COMMA))

$(call WRITE_FILE_APPEND, $(CONFIG_PY_FILE) ,'include':[ )
$(eval INCLUDE_FULLPATH := $(addprefix $($(comp)_LOCATION),$($(comp)_INCLUDES)) )
$(eval INCLUDE_FULLPATH += $(subst -I.,.,$(call unique,$(AOS_SDK_INCLUDES))) )
$(foreach inc,$(INCLUDE_FULLPATH), $(call WRITE_FILE_APPEND, $(CONFIG_PY_FILE) ,'$(inc)'$(COMMA)))
$(call WRITE_FILE_APPEND, $(CONFIG_PY_FILE) ,]$(COMMA))
$(call WRITE_FILE_APPEND, $(CONFIG_PY_FILE) ,}$(COMMA))
endef

PROJ_GEN_DIR   := projects/autogen/$(CLEANED_BUILD_STRING)

ifeq ($(IDE),iar)
PROJECT_GEN := $(PROJ_GEN_DIR)/iar_project/$(CLEANED_BUILD_STRING).ewp
$(MAKECMDGOALS): $(PROJECT_GEN)
$(PROJECT_GEN): build/scripts/iar.py build/aos_target_config.mk $(CONFIG_FILE)
	$(QUIET)echo Making $(IDE) Project
	$(QUIET)$(call WRITE_FILE_CREATE, $(CONFIG_PY_FILE) ,Projects = [)
	$(QUIET)$(foreach comp,$(PROCESSED_COMPONENTS), $(call WRITE_COMPOENT_PY ))
	$(QUIET)$(call WRITE_FILE_APPEND, $(CONFIG_PY_FILE) ,])
	$(QUIET)$(call MKDIR, $(PROJ_GEN_DIR)/iar_project)
	$(QUIET)cp -f  build/scripts/template.ewd $(PROJ_GEN_DIR)/iar_project/$(CLEANED_BUILD_STRING).ewd
	python build/scripts/iar.py $(CLEANED_BUILD_STRING)
	$(QUIET)echo ----------- iar project has generated in $(PROJ_GEN_DIR)/iar_project ----------- 
endif

ifeq ($(IDE),keil)
PROJECT_GEN := $(PROJ_GEN_DIR)/keil_project/$(CLEANED_BUILD_STRING).uvprojx
$(MAKECMDGOALS): $(PROJECT_GEN)
$(PROJECT_GEN): build/scripts/keil.py build/aos_target_config.mk $(CONFIG_FILE)
	$(QUIET)echo Making $(IDE) Project
	$(QUIET)$(call WRITE_FILE_CREATE, $(CONFIG_PY_FILE) ,Projects = [)
	$(QUIET)$(foreach comp,$(PROCESSED_COMPONENTS), $(call WRITE_COMPOENT_PY ))
	$(QUIET)$(call WRITE_FILE_APPEND, $(CONFIG_PY_FILE) ,])
	$(QUIET)$(call MKDIR, $(PROJ_GEN_DIR)/keil_project)
	python build/scripts/keil.py $(CLEANED_BUILD_STRING)
	$(QUIET)echo ----------- keil project has generated in $(PROJ_GEN_DIR)/keil_project ----------- 
endif

