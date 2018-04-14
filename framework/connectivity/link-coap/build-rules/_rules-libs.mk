ifdef LIBA_TARGET
.PHONY: StaticLib_Install

LIB_SRCS ?= $(wildcard *.c */*.c)
LIB_OBJS ?= $(LIB_SRCS:.c=.o)

sinclude $(LIB_SRCS:.c=.d)

$(LIBA_TARGET) :: $(LIB_OBJS)
	$(call Brief_Log,"AR")
	$(call Inspect_Env,$(WATCHED_VARS))
	$(Q)rm -f $@
	$(Q)$(AR) -rcs $@ $(LIB_OBJS)

$(LIBA_TARGET) :: StaticLib_Install
	$(Q)mkdir -p $(LIBOBJ_TMPDIR)/$(MODULE_NAME)
ifneq (,$(strip $(LIB_OBJS)))
	$(Q)cp -f $(LIB_OBJS) $(LIBOBJ_TMPDIR)/$(MODULE_NAME)
endif
	$(Q)mkdir -p $(SYSROOT_LIB)
	$(Q)cp -f $@ $(SYSROOT_LIB)
	$(call Copy_Headers, $(LIB_HEADERS),$(SYSROOT_INC),$(LIB_HDRS_DIR))

endif   # ifdef LIBA_TARGET

ifdef LIBSO_TARGET
.PHONY: DynamicLib_Install

LIB_SRCS ?= $(wildcard *.c */*.c)
LIB_OBJS ?= $(LIB_SRCS:.c=.o)

sinclude $(LIB_SRCS:.c=.d)

$(LIBSO_TARGET) :: SELF_LIBNAME = $(subst lib,,$(subst .so,,$(LIBSO_TARGET)))
$(LIBSO_TARGET) :: LDFLAGS := $(filter-out -l$(SELF_LIBNAME), $(LDFLAGS))
$(LIBSO_TARGET) :: $(LIB_OBJS) $(foreach d,$(DEPENDS_$(MODULE_NAME)),$(SYSROOT_LIB)/$(LIBA_TARGET_$(d)))
	$(call Brief_Log,"CC")
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

