.PHONY: comp-lib

ifdef COMP_LIB
ifeq (dynamic,$(CONFIG_LIB_EXPORT))
define Finalize_CompLib
( \
    $(CC) -shared -Os -o $(2)/lib$(3).so $(1) $(LDFLAGS); \
)
endef
define Info_CompLib
( \
    echo -ne "\e[1;32m"; \
    printf "\r%-32s%s\n" "[LD] lib$(1).so" "<= $(firstword $(2))"; \
    for i in $(wordlist 2,100,$(2)); do \
        printf "%-32s%s\n" "" "   $${i}"; \
    done; \
    echo -ne "\e[0m"; \
)
endef
else
define Finalize_CompLib
( \
	EXIST_OBJS="$$(ls $(1) 2>/dev/null)"; \
\
	if [ "$${EXIST_OBJS}" != "" ]; then \
	    $(AR) -rcs $(2)/lib$(3).a $${EXIST_OBJS}; \
	fi \
)
endef
define Info_CompLib
( \
    echo -ne "\e[1;35m"; \
    printf "\r%-32s%s\n" "[AR] lib$(1).a" "<= $(firstword $(2))"; \
    for i in $(wordlist 2,100,$(2)); do \
        printf "%-32s%s\n" "" "   $${i}"; \
    done; \
    echo -ne "\e[0m"; \
)
endef
endif # dynamic
endif # COMP_LIB

comp-lib: LIB_NAME = $(subst lib,,$(subst .so,,$(subst .a,,$(COMP_LIB))))
comp-lib: LIB_OBJS = $(foreach d,$(COMP_LIB_COMPONENTS),$(LIBOBJ_TMPDIR)/$(d)/*.o)
comp-lib: toolchain
	$(Q) \
	if [ -f $(STAMP_PRJ_CFG) ]; then true; else \
	    $(call Info_CompLib,$(LIB_NAME),$(COMP_LIB_COMPONENTS)); \
	    $(call Finalize_CompLib,$(LIB_OBJS),$(SYSROOT_LIB),$(LIB_NAME)); \
	fi

