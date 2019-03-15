TOUT := out

define inc-imm
	include $(1)
endef



define inc-module
$(eval MYDIR := $(1))
LIBRARY_NAME :=
LOCAL_CFLAGS :=
LOCAL_INC := -I$(1)
LIBRARIES_DIR += -L$(TOUT)/$(1)
CLEAN_RULE += clean_$(TOUT)/$(1)
include $(1)/module.mk
endef
#LIBRARIES += $(LIBRARY_NAME)
#LIB_DEP += $(TOUT)/$(1)/lib$(LIBRARY_NAME)$(LIBSUFFIX)

define foreach-dir
	$(foreach d,$(1), $(eval $(call inc-module,$(d))))
endef

define build-lib
ifeq ($(filter clean distclean setup list_projects bootloader bootloader_clean flashtool flashtool_clean,$(MAKECMDGOALS)),)
-include $$(addprefix $(TOUT)/$(7)/,$$(patsubst %.c,%.d,$(2)))
-include $$(addprefix $(TOUT)/$(7)/,$$(patsubst %.S,%.d,$(3)))
endif

ifeq ($(strip $(REDIRECT_BUILDLOG)), 1)
$(TOUT)/$(7)/%.d: $(TOUT)/$(7)/%.c
	$(Q)$(CC) $(CFLAGS) $(INC) $(4) $(5) -MM -MT $$(patsubst %.d,%.c,$$@) -MF $$@ $$(patsubst $(TOUT)/$(7)%,$(7)%,$$<) 2>&1 | tee -a build_error.log

$(TOUT)/$(7)/%.d: $(TOUT)/$(7)/%.S
	$(Q)$(CC) $(CFLAGS) $(INC) $(4) $(5) -MM -MT $$(patsubst %.d,%.S,$$@) -MF $$@ $$(patsubst $(TOUT)/$(7)%,$(7)%,$$<) 2>&1 | tee -a build_error.log

$(TOUT)/$(7)/%.c: $(TOPDIR)/$(7)/%.c
	$(Q)if [ ! -d $$(dir $$@) ]; then \
		$(MKDIR) $$(dir $$@); \
	fi
	$(Q)$(CC) $(CFLAGS) $(INC) $(4) $(5) -E $$< -o $$@ 2>&1 | tee -a build_error.log

$(TOUT)/$(7)/%.S: $(TOPDIR)/$(7)/%.S
	$(Q)if [ ! -d $$(dir $$@) ]; then \
		$(MKDIR) $$(dir $$@); \
	fi
	$(Q)$(CC) $(AFLAGS) $(INC) $(6) $(5) -E $$< -o $$@ 2>&1 | tee -a build_error.log

$(TOUT)/$(7)/%.o: $(TOUT)/$(7)/%.c
	$(Q)$(CC) $(CFLAGS) $(INC) $(4) $(5) -c $$< -o $$@ 2>&1 | tee -a build_error.log
	$(Q)$(SIZE) $$@

$(TOUT)/$(7)/%.o: $(TOUT)/$(7)/%.S
	$(Q)$(CC) $(AFLAGS) $(INC) $(6) $(5) -c $$< -o $$@ 2>&1 | tee -a build_error.log
	$(Q)$(SIZE) $$@
else
$(TOUT)/$(7)/%.d: $(TOUT)/$(7)/%.c
	$(Q)$(CC) $(CFLAGS) $(INC) $(4) $(5) -MM -MT $$(patsubst %.d,%.c,$$@) -MF $$@ $$(patsubst $(TOUT)/$(7)%,$(7)%,$$<)

$(TOUT)/$(7)/%.d: $(TOUT)/$(7)/%.S
	$(Q)$(CC) $(CFLAGS) $(INC) $(4) $(5) -MM -MT $$(patsubst %.d,%.S,$$@) -MF $$@ $$(patsubst $(TOUT)/$(7)%,$(7)%,$$<)

$(TOUT)/$(7)/%.c: $(TOPDIR)/$(7)/%.c
	$(Q)if [ ! -d $$(dir $$@) ]; then \
		$(MKDIR) $$(dir $$@); \
	fi
	$(Q)$(CC) $(CFLAGS) $(INC) $(4) $(5) -E $$< -o $$@

$(TOUT)/$(7)/%.S: $(TOPDIR)/$(7)/%.S
	$(Q)if [ ! -d $$(dir $$@) ]; then \
		$(MKDIR) $$(dir $$@); \
	fi
	$(Q)$(CC) $(AFLAGS) $(INC) $(6) $(5) -E $$< -o $$@
$(TOUT)/$(7)/%.o: $(TOUT)/$(7)/%.c
	$(Q)$(CC) $(CFLAGS) $(INC) $(4) $(5) -c $$< -o $$@
	$(Q)$(SIZE) $$@

$(TOUT)/$(7)/%.o: $(TOUT)/$(7)/%.S
	$(Q)$(CC) $(AFLAGS) $(INC) $(6) $(5) -c $$< -o $$@
	$(Q)$(SIZE) $$@
endif

$(TOUT)/$(7)/lib$(1)$(LIBSUFFIX): $(addprefix $(TOUT)/$(7)/,$(patsubst %.c,%.o,$(2))) $(addprefix $(TOUT)/$(7)/,$(patsubst %.S,%.o,$(3)))
	$(Q)echo "const char $(1)lib_version[] = \"#VERSION_START#" $(LIB_VERSION) "#VERSION_END#\";" > $$@.c
	$(Q)echo "const char $(1)lib_customer[] = \"#CUSTOMER_START#" $(CUSTOMER) "#CUSTOMER_END#\";" >> $$@.c
	$(Q)$(CC) -c $$@.c -o $$@.o
	$(Q)$(AR) -rsc $$@ $$^ $$@.o

libs/lib$(1)$(LIBSUFFIX): $(TOUT)/$(7)/lib$(1)$(LIBSUFFIX)
	$(Q)if [ ! -d $$(dir $$@) ]; then \
		$(MKDIR) $$(dir $$@); \
	fi
	$(Q)$(CP) $$^ $$@

lib$(1)$(LIBSUFFIX): libs/lib$(1)$(LIBSUFFIX)


clean_$(TOUT)/$(7):
	$(Q)$(RM) $(addprefix $(TOUT)/$(7)/,$(patsubst %.c,%.o,$(2))) $(addprefix $(TOUT)/$(7)/,$(patsubst %.S,%.o,$(3))) 2>/dev/null | true
	$(Q)$(RM) $(addprefix $(TOUT)/$(7)/,$(patsubst %.c,%.d,$(2))) $(addprefix $(TOUT)/$(7)/,$(patsubst %.S,%.d,$(3))) 2>/dev/null | true
	$(Q)$(RM) $(TOUT)/$(7)/lib$(1)$(LIBSUFFIX).c $(TOUT)/$(7)/lib$(1)$(LIBSUFFIX).o 2>/dev/null | true
	$(Q)$(RM) $(TOUT)/$(7)/lib$(1)$(LIBSUFFIX) 2>/dev/null | true

$(eval LLNAME := $(TOUT)/$(7)/lib$(1)$(LIBSUFFIX))
LIBRARIES += $(1)
LIB_DEP += $(TOUT)/$(7)/lib$(1)$(LIBSUFFIX)
TMP_SRC += $(addprefix $(TOUT)/$(7)/,$(2)) $(addprefix $(TOUT)/$(7)/,$(3))
endef

define clear-lib-internal
endef

###$(eval $(call build-lib-internal,$(1),$(2)))
# $(1) = LIBRARY_NAME
# $(2) = LIB_SRC
# $(3) = LIB_ASRC
# $(4) = LOCAL_CFLAGS
# $(5) = LOCAL_INC
# $(6) = LOCAL_AFLAGS
# $(7) = MYDIR
#define build-lib
#	$(eval $(call build-lib-internal,$(1),$(2),$(3),$(4),$(5),$(6)))
#endef
