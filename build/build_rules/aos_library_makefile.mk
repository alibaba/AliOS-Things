export SOURCE_ROOT ?= ./

include $(SOURCE_ROOT)/build/aos_host_cmd.mk

define TRY_TO_BUILD_LIBRARY
.PHONY : $(1)
all : $(1)
$(1):
	$(QUIET)$(eval board := $(1))
	$(QUIET)$(eval include $(SOURCE_ROOT)/board/$(board)/$(board).mk)
	$(QUIET)$(MAKE) -r $(SILENT) -f $(SOURCE_ROOT)/build/aos_library_config.mk TARGET_BOARD=$(board) BOARD_ARCH=$(HOST_ARCH)
endef

POSSIBLE_BOARD_LIST := $(notdir $(wildcard $(SOURCE_ROOT)/board/*))

$(foreach b, $(POSSIBLE_BOARD_LIST), \
$(if $(wildcard $(SOURCE_ROOT)/board/$(b)/$(b).mk), \
$(eval $(call TRY_TO_BUILD_LIBRARY, $(b)))))

