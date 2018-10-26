
#####################################################################################
# Macro PARSE_FEATURE_DEFCONFIG extend feature defconfig
# $(1): path to feature's defconfig
# $(2): var name of components
# $(3): DEFCONFIG_LIST

define PARSE_FEATURE_DEFCONFIG
$(eval _defconfig := $(1))
$(eval include $(_defconfig))
$(eval $(3) += $(_defconfig))
$(eval $(NAME)_DEFCONFIG := $(_defconfig))
$(eval $(2) += $($(NAME)_COMPONENTS) $($(NAME)_COMPONENTS-y))
endef

#####################################################################################
# Macro PARSE_FEATURE check feature defconfig and extend it
# $(1) feature name: feature.xxx, feature/xxx
# $(2) COMPONENTS
# $(3) DEFCONFIG_LIST

define PARSE_FEATURE
$(eval FEATURE = $(1))
$(eval FEATURE_LOCATION := $(FEATURE_DIR)/$(subst .,/,$(FEATURE)))
$(eval feature_found := no)

$(if $(wildcard $(FEATURE_LOCATION)), \
    $(eval FEATURE_DEFCONFIG := $(FEATURE_LOCATION)/defconfig) \
    $(if $(wildcard $(FEATURE_DEFCONFIG)), $(eval feature_found := yes),), \
    $(eval FEATURE_DEFCONFIG := $(FEATURE_LOCATION)_defconfig) \
    $(if $(wildcard $(FEATURE_DEFCONFIG)), $(eval feature_found := yes),) \
)

$(eval FEATURE_DEFCONFIG_FILES += $(FEATURE_DEFCONFIG))

$(if $(filter yes, $(feature_found)),, \
    $(error Unknown feature $(FEATURE), can not find feature configs from $(dir $(FEATURE_LOCATION))) \
)

$(call PARSE_FEATURE_DEFCONFIG, $(FEATURE_DEFCONFIG), $(2), $(3))

endef
