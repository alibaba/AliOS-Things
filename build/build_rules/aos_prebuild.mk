# Run tasks before build starting

export AOS_ALL_COMPONENTS := $(OUTPUT_DIR)/aos_all_components.mk

##########
# Recurse directories to find valid AOS component Makefiles
# $(1) = starting directory
# $(2) = name of variable to which to add Makefiles that are found
define RECURSE_DIR_COMPMK_SEARCH
$(foreach file, $(wildcard $(1)/*), $(if $(wildcard $(file)/*), $(if $(wildcard $(file)/aos.mk), $(eval $(2) += $(wildcard $(file)/aos.mk)),) $(call RECURSE_DIR_COMPMK_SEARCH,$(file),$(2)),))
endef

#$(call RECURSE_DIR_COMPMK_SEARCH, $(patsubst %/,%,$(SOURCE_ROOT)),VALID_COMPONENT_MAKEFILES)

$(AOS_ALL_COMPONENTS): $(VALID_COMPONENT_MAKEFILES)
	$(QUIET)$(call MKDIR, $(OUTPUT_DIR))
	$(QUIET)$(PYTHON) $(SCRIPTS_PATH)/aos_parse_components.py $(SOURCE_ROOT) $@

AOS_PRE_BUILD_TARGETS += $(AOS_ALL_COMPONENTS)
