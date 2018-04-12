ifneq ($(TOP_DIR),$(CURDIR))
INTERNAL_INCLUDES += -I$(SYSROOT_INC)
INTERNAL_INCLUDES += $(foreach d, $(shell find $(SYSROOT_INC) -type d), -I$(d))
INTERNAL_INCLUDES += -I$(TOP_DIR)
INTERNAL_INCLUDES += -I$(IMPORT_DIR)
INTERNAL_INCLUDES += -I$(IMPORT_DIR)/include
INTERNAL_INCLUDES += \
$(foreach d, \
    $(shell [ -d $(IMPORT_DIR)/$(CONFIG_VENDOR) ] && find -L $(IMPORT_DIR)/$(CONFIG_VENDOR)/include -type d), \
    -I$(d) \
)
INTERNAL_INCLUDES += $(foreach mod, $(MODULE_NAME) $(HDR_REFS), \
    $(foreach d, \
        $(shell find $(TOP_DIR)/$(mod)/ -type d -a -name "[^.]*"), \
        -I$(d) \
    ) \
)
INTERNAL_INCLUDES += \
    $(foreach d, \
        $(shell find $(OUTPUT_DIR)/$(MODULE_NAME) -type d -a -name "[^.]*"), \
        -I$(d) \
    )
INTERNAL_INCLUDES := $(strip $(sort $(INTERNAL_INCLUDES)))

EXTERNAL_INCLUDES += $(foreach mod, $(DEPENDS), \
    $(foreach d, \
        $(shell $(SHELL_DBG) find \
            $(SYSROOT_INC)/$(mod)/ -maxdepth 2 -type d 2>/dev/null) \
        $(shell $(SHELL_DBG) find \
            $(IMPORT_VDRDIR)/include/$(mod)/ -maxdepth 2 -type d 2>/dev/null), \
        -I$(d) \
    ) \
)
EXTERNAL_INCLUDES := $(strip $(EXTERNAL_INCLUDES))
endif   # ifneq ($(TOP_DIR),$(CURDIR))

ifeq (dynamic,$(strip $(CONFIG_LIB_EXPORT)))
CFLAGS  += -fPIC
endif

CFLAGS  := $(sort $(strip $(CFLAGS)))

LDFLAGS += -L$(SYSROOT_LIB)
LDFLAGS += -L$(IMPORT_VDRDIR)/$(PREBUILT_LIBDIR)

LDFLAGS += $(foreach d,$(DEPENDS_$(MODULE_NAME)),$(REF_LDFLAGS_$(d)))

WATCHED_VARS = \
    TARGET \
    CFLAGS \
    CC \
    LDFLAGS \
    CURDIR \
    INTERNAL_INCLUDES \
    DEPENDS \
    MAKECMDGOALS \
    EXTERNAL_INCLUDES \
    LIBA_TARGET \
    LIBSO_TARGET \

ALL_TARGETS := $(TARGET) $(LIBSO_TARGET) $(LIBA_TARGET) $(firstword $(KMOD_TARGET))

ifneq (,$(strip $(PKG_SWITCH)))
all: $(ALL_TARGETS)
else
all:
	$(Q)true
endif

clean:
	$(Q)rm -f \
        $(strip \
            $(ALL_TARGETS) $(OBJS) $(LIB_OBJS) \
            $(OBJS:.o=.d) $(LIB_OBJS:.o=.d) \
            $(LIB_OBJS:.o=.gcno) $(LIB_OBJS:.o=.gcda) \
        ) \
        *.o.e *.d *.o *.a *.so *.log *.gc*

%.o: %.c $(HD_MAKEFILE)
	$(call Brief_Log,"CC")
	$(call Inspect_Env,$(WATCHED_VARS))
	$(Q) \
	set -o pipefail; \
	$(CC) -I$(CURDIR) \
	    $(INTERNAL_INCLUDES) \
	    $(EXTERNAL_INCLUDES) \
	    $(CFLAGS) \
	    -c -o $@ $<
ifneq (,$(OBJCOPY_FLAGS))
	$(Q)$(OBJCOPY) $(OBJCOPY_FLAGS) $@
endif

NODEP_LIST = \
    $(SYSROOT_INC)/git_version.h \
    $(SYSROOT_INC)/platform.h \
    $(SYSROOT_INC)/product.h \
    $(SYSROOT_INC)/product_config.h \

ifneq (,$(findstring gcc,$(CC)))
%.d: %.c
	$(Q) \
	$(CC) -MM -I$(CURDIR) \
	    $(INTERNAL_INCLUDES) \
	    $(EXTERNAL_INCLUDES) \
	    $(CFLAGS) \
	$< > $@.$$$$ 2>/dev/null; \
	sed -i 's!$(shell basename $*)\.o[ :]!$*.o:!1' $@.$$$$; \
	mv $@.$$$$ $@;
endif

%.o: %.cpp
	$(call Brief_Log,"CC")
	$(call Inspect_Env,$(WATCHED_VARS))
	$(Q)$(CXX) -I$(CURDIR) \
	    $(INTERNAL_INCLUDES) \
	    $(EXTERNAL_INCLUDES) \
	    $(CFLAGS) \
	    -c -o $@ $<

ifneq (,$(findstring gcc,$(CC)))
%.d: %.cpp
	$(Q) \
	$(CXX) -MM -I$(CURDIR) \
	    $(INTERNAL_INCLUDES) \
	    $(EXTERNAL_INCLUDES) \
	    $(CFLAGS) \
	$< > $@.$$$$; \
	$(foreach D,$(NODEP_LIST),sed -i 's:$(D)::g' $@.$$$$;) \
	sed 's,\($*\)\.o[ :]*,\1.o $@: ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$;
endif

include $(RULE_DIR)/_rules-libs.mk
include $(RULE_DIR)/_rules-prog.mk
include $(RULE_DIR)/_rules-kmod.mk
