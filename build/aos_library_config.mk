export AOS_SDK_VERSION_MAJOR    :=  3
export AOS_SDK_VERSION_MINOR    :=  2
export AOS_SDK_VERSION_REVISION :=  3

export MAKEFILES_PATH := $(SOURCE_ROOT)/build
export SCRIPTS_PATH := $(SOURCE_ROOT)/build/scripts

include $(SOURCE_ROOT)/build/aos_host_cmd.mk

ifeq ($(COMPILER),armcc)
include $(SOURCE_ROOT)/build/aos_toolchain_armcc.mk
else ifeq ($(COMPILER),rvct)
include $(SOURCE_ROOT)build/aos_toolchain_rvct.mk
else ifeq ($(COMPILER),iar)
include $(SOURCE_ROOT)build/aos_toolchain_iar.mk
else
include $(SOURCE_ROOT)/build/aos_toolchain_gcc.mk
endif

OLD_MAKECMDGOALS := $(MAKECMDGOALS)
OLD_CURDIR := $(CURDIR)

ifeq ($(TARGET_ARCH), $(filter $(TARGET_ARCH), Cortex-M4 Cortex-M3 Cortex-M4F))
ifeq ($(TARGET_BOARD), b_l475e)
HOST_ARCH := $(TARGET_ARCH)
endif
else
HOST_ARCH := $(BOARD_ARCH)
endif


ifeq ($(TARGET_ARCH), $(HOST_ARCH))
LIB_NAME := $(word $(words $(subst /, , $(LIB_DIR))), $(subst /, , $(LIB_DIR)))

ifneq ($(notdir $(LIB_DIR)), )
LIB_OUT_DIR := $(LIB_DIR)/
else
LIB_OUT_DIR := $(LIB_DIR)
endif

ALWAYS_OPTIMISE := 1

BYPASS_LIBRARY_POISON_CHECK=1

ONLY_BUILD_LIBRARY := yes

POSSIBLE_APP_NAME := $(LIB_NAME)app
APP := $(strip $(filter $(POSSIBLE_APP_NAME), $(foreach app, $(SOURCE_ROOT)/example, $(notdir $(wildcard $(app)/*)))))

ifeq ($(APP),)
ifeq ($(TARGET_BOARD), b_l475e)
APP := mqttapp
else
APP := alinkapp
endif
endif

MAKECMDGOALS += $(APP)@$(TARGET_BOARD)

include $(LIB_OUT_DIR)$(LIB_NAME).mk
TARGET_CFLAGS := $(addprefix -I$(LIB_OUT_DIR),$(GLOBAL_INCLUDES)) $(addprefix -D,$(GLOBAL_DEFINES)) $(addprefix -I$(LIB_OUT_DIR),$($(NAME)_INCLUDES)) $(addprefix -D,$($(NAME)_DEFINES)) $($(NAME)_CFLAGS)

SOURCES := $(addprefix $(LIB_OUT_DIR),$($(NAME)_SOURCES))

CFLAGS :=
AOS_SDK_CFLAGS :=
AOS_SDK_INCLUDES :=
AOS_SDK_DEFINES :=

include $(SOURCE_ROOT)/build/aos_target_config.mk

CFLAGS += -c -MD -ggdb -Os $(strip $(AOS_SDK_CFLAGS)) $($(LIB_NAME)_CFLAGS_ALL) 
CFLAGS += -Wall -fsigned-char -ffunction-sections -fdata-sections -fno-common -std=gnu11
CFLAGS += $(strip $(AOS_SDK_INCLUDES)) $(strip $(addprefix -D,$(AOS_SDK_DEFINES)))

MAKECMDGOALS := $(OLD_MAKECMDGOALS)
CURDIR := $(OLD_CURDIR)
CFLAGS += $(TARGET_CFLAGS)
NAME := $(LIB_NAME)

include $(SOURCE_ROOT)/build/aos_library_build.mk

else

.PHONY : all
all :

endif
