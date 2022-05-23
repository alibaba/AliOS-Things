#
# Main Makefile. This is basically the same as a component makefile.

COMPONENT_ADD_INCLUDEDIRS := include
COMPONENT_PRIV_INCLUDEDIRS := inc
COMPONENT_SRCDIRS := src smart_config airkiss_config blufi_config

LIBS := airkiss_aes
COMPONENT_ADD_LDFLAGS += -lmain -L $(COMPONENT_PATH)/airkiss_config $(addprefix -l,$(LIBS))
ALL_LIB_FILES += $(patsubst %,$(COMPONENT_PATH)/airkiss_config/lib%.a,$(LIBS))
