.PHONY: FORCE

ifdef TARGET
SRCS ?= $(wildcard *.c *.cpp)
OBJS ?= $(subst .c,.o,$(subst .cpp,.o,$(SRCS)))

ifneq (modinfo,$(MAKECMDGOALS))
ifneq (clean,$(MAKECMDGOALS))
sinclude $(OBJS:.o=.d)
endif
endif

ifdef LIBA_TARGET
$(TARGET): $(LIBA_TARGET)

LDFLAGS := -l$(subst .a,,$(subst lib,,$(LIBA_TARGET))) $(LDFLAGS)
endif

ifdef LIBSO_TARGET
$(TARGET): $(LIBSO_TARGET)

LDFLAGS := -l$(subst .so,,$(subst lib,,$(LIBSO_TARGET))) $(LDFLAGS)
endif

ifneq (,$(filter %.cpp %.cc,$(SRCS)))
CCLD    := $(CXX)
else
CCLD    := $(CC)
endif

ifeq (1,$(words $(TARGET)))

$(TARGET): $(OBJS) FORCE
	$(call Brief_Log,"LD")
	$(call Inspect_Env,$(WATCHED_VARS))
	$(Q)$(CCLD) $(CFLAGS) -o $@ \
	    $(RPATH_CFLAGS) \
	    $(OBJS) \
	    -Wl,--start-group $(filter -l%, $(LDFLAGS)) -Wl,--end-group \
	    $(filter -L%, $(LDFLAGS))
	$(Q)mkdir -p $(OUTPUT_DIR)${bindir}
	$(Q)cp -f $@ $(OUTPUT_DIR)${bindir}

else

$(foreach t,$(sort $(TARGET)),$(t)): FORCE
	$(Q)$(MAKE) TARGET=$@ OBJS="$(SRCS_$@:.c=.o)"

endif

endif   # ifdef TARGET
