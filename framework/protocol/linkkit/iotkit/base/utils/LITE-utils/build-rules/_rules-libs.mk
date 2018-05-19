VPATH    := $(TOP_DIR)/$(MODULE_NAME)
LIB_SRCS ?= $(foreach M,*.c */*.c */*/*.c,$(wildcard $(TOP_DIR)/$(MODULE_NAME)/$(M))) $(wildcard *.c)
LIB_OBJS := $(LIB_SRCS:.c=.o)
LIB_OBJS := $(subst $(TOP_DIR)/$(MODULE_NAME)/,,$(LIB_OBJS))

ifdef LIB_SRCS_PATTERN
SRC_LIST := $(foreach M,$(LIB_SRCS_PATTERN),$(shell ls $(TOP_DIR)/$(MODULE_NAME)/$(M) 2>/dev/null))
LIB_SRCS := $(SRC_LIST)
LIB_OBJS := $(SRC_LIST:.c=.o)
LIB_OBJS := $(subst $(TOP_DIR)/$(MODULE_NAME)/,,$(LIB_OBJS))
endif

sinclude $(LIB_OBJS:.o=.d)

ifdef LIBA_TARGET
.PHONY: StaticLib_Install

ifeq (1,$(words $(LIBA_TARGET)))

$(LIBA_TARGET) :: $(LIB_OBJS)
	@$(call Brief_Log,"AR")
	$(call Inspect_Env,$(WATCHED_VARS))
	$(Q)rm -f $@
ifdef CONFIG_LIBOBJ_STRIP
	@$(call Brief_Log,"ST")
	$(TOP_Q)$(STRIP) --strip-debug $(LIB_OBJS)
endif
	$(TOP_Q)$(AR) -rcs $@ $(LIB_OBJS)

$(LIBA_TARGET) :: StaticLib_Install
	$(Q)mkdir -p $(LIBOBJ_TMPDIR)/$(MODULE_NAME)
ifneq (,$(strip $(LIB_OBJS)))
ifneq ($(LIBA_TARGET),$(LIBA_SKIP_COMBO))
	$(Q)cp -f $(LIB_OBJS) $(LIBOBJ_TMPDIR)/$(MODULE_NAME)
endif
endif
	$(Q)mkdir -p $(SYSROOT_LIB)
	$(Q)cp -f $@ $(SYSROOT_LIB)
	$(call Copy_Headers, $(LIB_HEADERS),$(SYSROOT_INC),$(LIB_HDRS_DIR))

else

$(foreach t,$(sort $(LIBA_TARGET)),$(t)): FORCE
	$(Q) \
	$(MAKE) LIBA_TARGET=$@ \
	    LIB_SRCS="$(LIB_SRCS_$(subst .a,,$(subst lib,,$@)))" \
	    LIB_SRCS_PATTERN="$(LIB_SRCS_PATTERN_$(subst .a,,$(subst lib,,$@)))" \


endif   # ifeq (1,$(words $(LIBA_TARGET)))

endif   # ifdef LIBA_TARGET

ifdef LIBSO_TARGET
.PHONY: DynamicLib_Install

$(LIBSO_TARGET) :: SELF_LIBNAME = $(subst lib,,$(subst .so,,$(LIBSO_TARGET)))
$(LIBSO_TARGET) :: LDFLAGS := $(filter-out -l$(SELF_LIBNAME), $(LDFLAGS))
$(LIBSO_TARGET) :: $(LIB_OBJS) $(foreach d,$(DEPENDS_$(MODULE_NAME)),$(SYSROOT_LIB)/$(LIBA_TARGET_$(d)))
	@$(call Brief_Log,"CC")
	$(call Inspect_Env,$(WATCHED_VARS))
	$(Q)$(CC) -shared -Os \
	    $(CFLAGS) \
	    $(RPATH_CFLAGS) \
	    -o $@ \
	    $(LIB_OBJS) \
	    -Wl,--start-group $(LDFLAGS) -Wl,--end-group

$(LIBSO_TARGET) :: DynamicLib_Install
	$(Q)mkdir -p $(LIBOBJ_TMPDIR)/$(shell $(SHELL_DBG) basename $(CURDIR))
	$(Q)cp -f $(LIB_OBJS) $(LIBOBJ_TMPDIR)/$(shell $(SHELL_DBG) basename $(CURDIR))
	$(Q)mkdir -p $(SYSROOT_LIB)
	$(Q)install -m 0755 $@ $(SYSROOT_LIB)
	$(call Copy_Headers, $(LIB_HEADERS),$(SYSROOT_INC),$(LIB_HDRS_DIR))

endif   # ifdef LIBSO_TARGET

