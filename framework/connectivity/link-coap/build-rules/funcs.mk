define Dump_Var
	NUM=`echo "$(strip $($(1)))"|awk '{ print NF }'`; \
	if expr $${NUM} \> 1 >/dev/null; then \
	    printf -- "-----------------------------------------------------------------\n"; \
	    printf "%-24s| %s\n" ". $(1)" `echo "$(strip $($(1)))"|cut -d' ' -f1|sed 's/^ *//'`; \
	    for i in `echo "$(strip $($(1)))"|cut -d' ' -f2-`; do \
	        printf "%-24s| %s\n" "" "$${i}"; \
	    done; \
	    printf -- "-----------------------------------------------------------------\n"; \
	else \
	    printf "%-24s| %s\n" ". $(1)" "$(strip $($(1)))"; \
	fi;
endef

ifneq (,$(Q))
define Inspect_Env
endef
else
define Inspect_Env
	@printf -- "-----------------------------------------------------------------\n"
	@printf "%-20s| %s\n" ". BUILDING_TARGET" "$@"
	@printf -- "-----------------------------------------------------------------\n"
	@printf "%-20s| %s\n" ". BUILDING_DEPEND" "$(filter-out FORCE,$^)"
	@printf -- "-----------------------------------------------------------------\n"
	@$(foreach var,$(1),$(call Dump_Var,$(var)))
	@printf -- "-----------------------------------------------------------------\n"
endef
endif

# 31, red. 32, green. 33, yellow. 34, blue. 35, magenta. 36, cyan. 37, white.
define Brief_Log
	@if [ "$1" = "CC" ]; then \
	    if echo "$@"|grep -q "\.so$$"; then \
	        COLOR_MARK="\e[1;32m"; \
	    elif echo "$@"|grep -q "\.ko$$"; then \
	        COLOR_MARK="\e[1;35m"; \
	    else \
	        COLOR_MARK="\e[1;36m"; \
	    fi \
	elif [ "$1" = "AR" ]; then \
	    COLOR_MARK="\e[1;33m"; \
	elif [ "$1" = "LD" ]; then \
	    COLOR_MARK="\e[1;31m"; \
	fi; \
	echo -ne "$${COLOR_MARK}"
	@if [ "$2" = "" ]; then \
	    FIRST_DEP="$(firstword $(filter-out FORCE,$?))"; \
	    SPACE_BAR="                                   "; \
	    if [ "$${FIRST_DEP}" != "" ]; then \
	        FIRST_DEP="$$(basename $${FIRST_DEP})"; \
	    fi; \
	    printf "\r%-32s%s%s\n" "[$1] $$(expr substr $$(basename $@) 1 20)" "<= $${FIRST_DEP} $${SPACE_BAR}"; \
	else \
	    printf "\r%-32s%s%s\n" "[$1] $$(expr substr $(2) 1 20)" "<= $${FIRST_DEP} $${SPACE_BAR}"; \
	fi
	@for i in $(wordlist 2,100,$(filter-out FORCE,$?)); do \
	    if [ "$$(echo $${i}|cut -c1)" != "/" ]; then \
	        printf "%-32s%s\n" "" "   $$(basename $${i})"; \
	    fi \
	done
	@echo -ne "\e[0m"
endef

define Copy_Headers
	$(Q) \
	if [ "$(strip $(1))" != "" ]; then \
	    mkdir -p $(2)/$(3); \
	    for hdr in $(1); do \
	        if [ $${PWD}/$${hdr} -nt $(2)/$(3)/$${hdr} ]; then \
	            mkdir -p $(2)/$(3)/$$(dirname $${hdr}); \
	            cp -f $${hdr} $(2)/$(3)/$$(dirname $${hdr}); \
	        fi; \
	    done \
	fi
endef

define Update_Extra_Srcs
( \
	for ELEM in $(strip $(1)); do \
	    DST=$(OUTPUT_DIR)/$(2)/$$(basename $${ELEM}); \
	    if [ $${ELEM} -nt $${DST} ]; then \
	        cp -Lf $${ELEM} $${DST}; \
	    fi; \
	done \
)
endef

#
#	    ($(foreach d,$(COMP_LIB_COMPONENTS), \
#
#	        $(RECURSIVE_MAKE) pre-build target-$(d) && \
#	        $(MAKE) --no-print-directory -C $(OUTPUT_DIR)/$(d) $(LIBA_TARGET_$(d)) \
#	            $(if $(Q),,2>&1|tee -a $(OUTPUT_DIR)/$(d)/$(COMPILE_LOG)) \
#	            $(if $(Q),,2>&1|tee -a $(OUTPUT_DIR)/$(COMPILE_LOG)) \
#	        ; \
#
#	        if [ $$? != 0 ]; then \
#

ifdef COMP_LIB
define Build_CompLib
( \
	if  [ "$(strip $(1))" = "FORCE" ] || \
	    [ "$$(echo $(LDFLAGS_$(strip $(1)))|grep -wo -- '-l$(COMP_LIB_NAME)')" != "" ]; then \
	    ($(foreach d,$(COMP_LIB_COMPONENTS), \
	        $(MAKE) --no-print-directory $(d); \
	        if [ $$? != 0 ]; then \
	            echo -e "\rFailed to build $(LIBA_TARGET_$(d)) in $(d)"; \
	            exit 10; \
	        else \
	            if [ "$(LIBA_TARGET_$(d))" != "" ]; then \
	                rm -f $(SYSROOT_LIB)/$(LIBA_TARGET_$(d)); \
	            fi; \
	        fi; \
	    )); \
	    if  [ $$? != 0 ]; then \
	        echo -e "\rFailed to build components for '$(COMP_LIB)'. Abort!"; \
	        exit 11; \
	    else \
	        $(RECURSIVE_MAKE) comp-lib; \
	    fi; \
	fi \
)
endef
else
define Build_CompLib
true
endef
endif
