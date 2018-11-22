# filters for resources
TEXT_FILTERS   := %.html %.htm %.txt %.eml %.js %.css %.dat %.cer %.pem %.json %.xml %.py %.key
BINARY_FILTERS := %.jpg %.jpeg %.png %.ico %.gif %.bin %.flac

STAGING_DIR ?= $(OUTPUT_DIR)/resources/Staging/
FS_IMAGE    ?= $(OUTPUT_DIR)/resources/filesystem.bin


###############################################################################
# MACRO: RESOURCE_FILENAME
# Makes an output path C file for a resource file, converting dots to underscores
# $(1) is resource filename in the resource directory
RESOURCE_FILENAME      =$(addprefix $(OUTPUT_DIR)/resources/,$(addsuffix .c,$(subst .,_,$(notdir $(1)))))

###############################################################################
# MACRO: RESOURCE_VARIABLE_NAME
# Creates a variable name that will be used for a resource from it's filename
# slashes are converted to _DIR_ and dots to underscores
# $(1) is resource filename in the resource directory
RESOURCE_VARIABLE_NAME =$(if $(findstring firmware,$(1)),wifi_firmware_image,$(addprefix resources_,$(subst /,_DIR_,$(subst .,_,$(subst $(SOURCE_ROOT)resources/,,$(1))))))

BIN_TO_RES_SCRIPT  := $(TOOLS_ROOT)/text_to_c/bin_to_resource_c.pl
TEXT_TO_RES_SCRIPT := $(TOOLS_ROOT)/text_to_c/text_to_resource_c.pl
###############################################################################
# MACRO: BUILD_RESOURCE_RULES
# Creates targets to build a resource file
# the first target converts the text resource file to a C file
# the second target compiles the C resource file into an object file
# $(1) is the name of a resource
# $(2) should be MEM or FILESYSTEM - indication location of resource
define BUILD_RESOURCE_RULES
$(call RESOURCE_FILENAME, $(1)): $(1) $(STAGING_DIR).d | $(EXTRA_PRE_BUILD_TARGETS) $(TEXT_TO_RES_SCRIPT) $(BIN_TO_RES_SCRIPT)
	$$(if $(RESOURCES_START_PRINT),,$(eval RESOURCES_START_PRINT:=1) $(QUIET)$(ECHO) Processing resources)
	$$(if $(filter $(TEXT_FILTERS),$(1)),$(QUIET)$(PERL) $(TEXT_TO_RES_SCRIPT)  $(2) $(call RESOURCE_VARIABLE_NAME, $(1)) $(1) > $$@)
	$$(if $(filter $(BINARY_FILTERS),$(1)),$(QUIET)$(PERL) $(BIN_TO_RES_SCRIPT) $(2) $(call RESOURCE_VARIABLE_NAME,$(1)) $(1) > $$@)
	$(QUIET)$(CP) $(1) $(STAGING_DIR)$(notdir $(1))


$(patsubst %.c,%.o,$(call RESOURCE_FILENAME, $(1))): $(call RESOURCE_FILENAME, $(1))
ifeq (IAR,$(TOOLCHAIN_NAME))
	$(QUIET)$(CC) $(COMPILER_SPECIFIC_COMP_ONLY_FLAG) $(COMPILER_SPECIFIC_DEPS_FLAG) $(RESOURCE_CFLAGS) $(call ADD_COMPILER_SPECIFIC_STANDARD_CFLAGS, ) $($(1)_CFLAGS) -I$(SOURCE_ROOT)include/ -I$(SOURCE_ROOT)AOS/WWD/include -o $$@ $$< >> $$(IAR_BUILD_RESULTS_FILE)
else
	$(QUIET)$(CC) $(COMPILER_SPECIFIC_COMP_ONLY_FLAG) $(COMPILER_SPECIFIC_DEPS_FLAG) $(RESOURCE_CFLAGS) $(call ADD_COMPILER_SPECIFIC_STANDARD_CFLAGS, ) $($(1)_CFLAGS) -I$(SOURCE_ROOT)include/ -I$(SOURCE_ROOT)AOS/WWD/include -o $$@ $$<
endif

$(eval RESOURCE_OBJS += $(patsubst %.c,%.o,$(call RESOURCE_FILENAME, $(1))))

endef

###############################################################################
# MACRO: CREATE_ALL_RESOURCE_TARGETS
# Create build targets which convert resources from binary to C files and build
# the C files
# Also creates a target for the overall resources variables header file
# $(1) is a list of resources
define CREATE_ALL_RESOURCE_TARGETS

$(foreach RESOURCE,$(1),$(eval $(call BUILD_RESOURCE_RULES,$(RESOURCE),$(if $(filter $(1),$(INTERNAL_MEMORY_RESOURCES)),MEM,FILESYSTEM))))

$(STAGING_DIR).d:
	$(QUIET)$$(call MKDIR, $$(dir $$@))
	$(QUIET)$(TOUCH) $$(@)

# Target for build-from-source
# The repeated lines avoid line-too-long errors in windows
$(OUTPUT_DIR)/libraries/resources.a: $$(RESOURCE_OBJS)
	$(QUIET)$(RM) $$@
	$$(if $$(wordlist 1,50,     $$(RESOURCE_OBJS)),$(QUIET)$(AR) $(AOS_SDK_ARFLAGS) $(COMPILER_SPECIFIC_ARFLAGS_CREATE) $$@ $$(wordlist 1,50,     $$(RESOURCE_OBJS)))
	$$(if $$(wordlist 51,100,   $$(RESOURCE_OBJS)),$(QUIET)$(AR) $(AOS_SDK_ARFLAGS) $(COMPILER_SPECIFIC_ARFLAGS_ADD) $$@ $$(wordlist 51,100,   $$(RESOURCE_OBJS)))
	$$(if $$(wordlist 101,150,  $$(RESOURCE_OBJS)),$(QUIET)$(AR) $(AOS_SDK_ARFLAGS) $(COMPILER_SPECIFIC_ARFLAGS_ADD) $$@ $$(wordlist 101,150,  $$(RESOURCE_OBJS)))
	$$(if $$(wordlist 151,200,  $$(RESOURCE_OBJS)),$(QUIET)$(AR) $(AOS_SDK_ARFLAGS) $(COMPILER_SPECIFIC_ARFLAGS_ADD) $$@ $$(wordlist 151,200,  $$(RESOURCE_OBJS)))
	$$(if $$(wordlist 201,250,  $$(RESOURCE_OBJS)),$(QUIET)$(AR) $(AOS_SDK_ARFLAGS) $(COMPILER_SPECIFIC_ARFLAGS_ADD) $$@ $$(wordlist 201,250,  $$(RESOURCE_OBJS)))
	$$(if $$(wordlist 251,300,  $$(RESOURCE_OBJS)),$(QUIET)$(AR) $(AOS_SDK_ARFLAGS) $(COMPILER_SPECIFIC_ARFLAGS_ADD) $$@ $$(wordlist 251,300,  $$(RESOURCE_OBJS)))
	$$(if $$(wordlist 301,350,  $$(RESOURCE_OBJS)),$(QUIET)$(AR) $(AOS_SDK_ARFLAGS) $(COMPILER_SPECIFIC_ARFLAGS_ADD) $$@ $$(wordlist 301,350,  $$(RESOURCE_OBJS)))
	$$(if $$(wordlist 351,400,  $$(RESOURCE_OBJS)),$(QUIET)$(AR) $(AOS_SDK_ARFLAGS) $(COMPILER_SPECIFIC_ARFLAGS_ADD) $$@ $$(wordlist 351,400,  $$(RESOURCE_OBJS)))
	$$(if $$(wordlist 401,450,  $$(RESOURCE_OBJS)),$(QUIET)$(AR) $(AOS_SDK_ARFLAGS) $(COMPILER_SPECIFIC_ARFLAGS_ADD) $$@ $$(wordlist 401,450,  $$(RESOURCE_OBJS)))
	$$(if $$(wordlist 451,500,  $$(RESOURCE_OBJS)),$(QUIET)$(AR) $(AOS_SDK_ARFLAGS) $(COMPILER_SPECIFIC_ARFLAGS_ADD) $$@ $$(wordlist 451,500,  $$(RESOURCE_OBJS)))
	$$(if $$(wordlist 501,550,  $$(RESOURCE_OBJS)),$(QUIET)$(AR) $(AOS_SDK_ARFLAGS) $(COMPILER_SPECIFIC_ARFLAGS_ADD) $$@ $$(wordlist 501,550,  $$(RESOURCE_OBJS)))
	$$(if $$(wordlist 551,600,  $$(RESOURCE_OBJS)),$(QUIET)$(AR) $(AOS_SDK_ARFLAGS) $(COMPILER_SPECIFIC_ARFLAGS_ADD) $$@ $$(wordlist 551,600,  $$(RESOURCE_OBJS)))
	$$(if $$(wordlist 601,650,  $$(RESOURCE_OBJS)),$(QUIET)$(AR) $(AOS_SDK_ARFLAGS) $(COMPILER_SPECIFIC_ARFLAGS_ADD) $$@ $$(wordlist 601,650,  $$(RESOURCE_OBJS)))
	$$(if $$(wordlist 651,700,  $$(RESOURCE_OBJS)),$(QUIET)$(AR) $(AOS_SDK_ARFLAGS) $(COMPILER_SPECIFIC_ARFLAGS_ADD) $$@ $$(wordlist 651,700,  $$(RESOURCE_OBJS)))
	$$(if $$(wordlist 701,750,  $$(RESOURCE_OBJS)),$(QUIET)$(AR) $(AOS_SDK_ARFLAGS) $(COMPILER_SPECIFIC_ARFLAGS_ADD) $$@ $$(wordlist 701,750,  $$(RESOURCE_OBJS)))
	$$(if $$(wordlist 751,1000, $$(RESOURCE_OBJS)),$(QUIET)$(AR) $(AOS_SDK_ARFLAGS) $(COMPILER_SPECIFIC_ARFLAGS_ADD) $$@ $$(wordlist 751,1000, $$(RESOURCE_OBJS)))


RESOURCE_HEADER_TARGET_CREATED := 1

$(eval RESOURCE_C_FILES += $(call RESOURCE_FILENAME, $(1)))

$(OUTPUT_DIR)/resources/resources.h: $$(RESOURCE_C_FILES)

endef


# resources header target - creates a header file of all the resource variables.
$(OUTPUT_DIR)/resources/resources.h:  $(CONFIG_FILE)
	$(QUIET)$(ECHO) $(QUOTES_FOR_ECHO)/* Automatically generated file - this comment ensures resources.h file creation */$(QUOTES_FOR_ECHO) > $@
	$(if $(RESOURCE_C_FILES), $(QUIET)$(PERL) $(TOOLS_ROOT)/text_to_c/resources_header.pl $(RESOURCE_C_FILES) >> $@)


RESOURCES_DEPENDENCY = $(OUTPUT_DIR)/resources/resources.h $(if $(RESOURCE_HEADER_TARGET_CREATED), $(OUTPUT_DIR)/libraries/resources.a, )
RESOURCES_LIBRARY = $(if $(RESOURCE_HEADER_TARGET_CREATED),$(OUTPUT_DIR)/libraries/resources.a)

# Expand the list of resources to point to the full location (either component local or the common resources directory)
# $(1) is the resource name, $(2) is the current directory
RESOURCE_EXPAND_DIRECTORY = $(foreach res,$($(1)_RESOURCES),$(word 1,$(wildcard $(addsuffix $(res),$(2) $(SOURCE_ROOT)resources/))))
RESOURCE_EXPAND_DIRECTORY = $$($(1)_RESOURCES)


ifneq ($(filter SFLASH_FILESYSTEM,$(RESOURCES_LOCATION)),)

RESOURCE_DOWNLOADER_TARGET := $(if $(findstring sflash_write,$(APP)),,external_resource_downloader)
RESOURCE_DOWNLOAD = download_all_resources

download_all_resources: $(RESOURCES_DEPENDENCY) $(RESOURCE_DOWNLOADER_TARGET)
	 $(OPENOCD_FULL_NAME) -f $(OPENOCD_PATH)$(JTAG).cfg -f $(OPENOCD_PATH)$(HOST_OPENOCD).cfg -f $($(SOURCE_ROOT)apps/waf/sflash_write/sflash_write.tcl -c "sflash_write_file $(FS_IMAGE) 0x0 $(PLATFORM)-$(BUS) 1 0" -c shutdown

$(FS_IMAGE): $(RESOURCES_DEPENDENCY)
	$(COMMON_TOOLS_PATH)mk_aosfs32 $(FS_IMAGE) $(STAGING_DIR)



SFLASH_WRITE_TARGET := waf.sflash_write-NoOS-$(PLATFORM)-$(BUS)
SFLASH_WRITE_LOG_FILE ?= out/sflash_write.log
SFLASH_WRITE_OUTFILE := $(BUILD_DIR)/$(SFLASH_WRITE_TARGET)/binary/$(SFLASH_WRITE_TARGET)
ifneq ($(VERBOSE),1)
SFLASH_WRITE_REDIRECT	= > $(SFLASH_WRITE_LOG_FILE)
endif

external_resource_downloader:
	$(QUIET)$(ECHO) Building serial flash downloader
	$(QUIET)$(ECHO_BLANK_LINE)
	$(QUIET)$(MAKE) -r $(SILENT) -f $(SOURCE_ROOT)Makefile $(SFLASH_WRITE_TARGET) NO_BUILD_BOOTLOADER=1 -I$(OUTPUT_DIR)  SFLASH= $(SFLASH_WRITE_REDIRECT)

endif
