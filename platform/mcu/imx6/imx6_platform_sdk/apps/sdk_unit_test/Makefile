#-------------------------------------------------------------------------------
# Copyright (c) 2012 Freescale Semiconductor, Inc.
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without modification,
# are permitted provided that the following conditions are met:
#
# o Redistributions of source code must retain the above copyright notice, this list
#   of conditions and the following disclaimer.
#
# o Redistributions in binary form must reproduce the above copyright notice, this
#   list of conditions and the following disclaimer in the documentation and/or
#   other materials provided with the distribution.
#
# o Neither the name of Freescale Semiconductor, Inc. nor the names of its
#   contributors may be used to endorse or promote products derived from this
#   software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
# ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
# WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
# DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
# ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
# (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
# LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
# ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
# SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#-------------------------------------------------------------------------------

#
# SDK unit test application
#

include $(SDK_ROOT)/mk/common.mk

# Alias 'test' to 'TEST' for easier command line typing.
ifdef test
TEST = $(test)
endif

# Provide default value of TEST if not set.
TEST ?= ALL

# Test selection configuration.
ifeq "$(TEST)" "ALL"
DEFINES +=-DALL_TEST_ENABLE
endif

# Pass the selected test name to the unit test app.
DEFINES +=-DSTANDALONE -DSDK_TEST=$(TEST)_test

# Set up to link application.
export APP_NAME = sdk_unit_test
LINK_APP = 1

APP_ELF = $(APP_OUTPUT_ROOT)/$(APP_NAME)_$(TEST).elf

# Tests included in the build for all targets and boards.
define SUBDIRS
    $(SDK_LIB_ROOT)/drivers/audio/test
    $(SDK_LIB_ROOT)/drivers/caam/test
    $(SDK_LIB_ROOT)/drivers/cpu_utility/test
    $(SDK_LIB_ROOT)/core/test
    $(SDK_LIB_ROOT)/drivers/i2c/test
    $(SDK_LIB_ROOT)/drivers/ocotp/test
    $(SDK_LIB_ROOT)/drivers/pmu/test
    $(SDK_LIB_ROOT)/drivers/rtc/test
    $(SDK_LIB_ROOT)/drivers/sdma/test
    $(SDK_LIB_ROOT)/drivers/tempmon/test
    $(SDK_LIB_ROOT)/drivers/timer/test
    $(SDK_LIB_ROOT)/drivers/uart/test
    $(SDK_LIB_ROOT)/drivers/usdhc/test
    $(SDK_LIB_ROOT)/drivers/pwm/test
    $(SDK_LIB_ROOT)/drivers/wdog/test
    $(SDK_LIB_ROOT)/drivers/gpio/test
endef

# Tests that are not available for CHIP_MX6SL only.
ifneq "$(BOARD)" "evk"
    SUBDIRS += \
    $(SDK_LIB_ROOT)/drivers/hdmi/test \
    $(SDK_LIB_ROOT)/drivers/ipu/test \
    $(SDK_LIB_ROOT)/drivers/vdoa/test \
    $(SDK_LIB_ROOT)/drivers/vpu/test
endif

# Tests that are only available on the evb.
ifeq "$(BOARD)" "evb"
    SUBDIRS += \
        $(SDK_LIB_ROOT)/drivers/enet/test \
        $(SDK_LIB_ROOT)/drivers/gpmi/test \
        $(SDK_LIB_ROOT)/drivers/keypad/test \
        $(SDK_LIB_ROOT)/drivers/pcie/test \
        $(SDK_LIB_ROOT)/drivers/spi/test \
        $(SDK_LIB_ROOT)/drivers/camera/test
endif

# Tests that apply to either evb or sabre_ai.
ifneq "$(filter evb sabre_ai,$(BOARD))" ""
    SUBDIRS += \
        $(SDK_LIB_ROOT)/drivers/mipi/test \
        $(SDK_LIB_ROOT)/drivers/usb/test
endif

# Tests that are only available on the sabre_ai board.
ifeq "$(BOARD)" "sabre_ai"
    SUBDIRS += \
        $(SDK_LIB_ROOT)/drivers/flexcan/test \
        $(SDK_LIB_ROOT)/drivers/eim/test \
        $(SDK_LIB_ROOT)/drivers/pcie/test \
        $(SDK_LIB_ROOT)/drivers/spi/test \
        $(SDK_LIB_ROOT)/drivers/camera/test \
        $(SDK_LIB_ROOT)/drivers/enet/test \
        $(SDK_LIB_ROOT)/drivers/accelerometer/test
endif

# Tests only available on the smart_device board.
ifeq "$(BOARD)" "smart_device"
    SUBDIRS += \
        $(SDK_LIB_ROOT)/drivers/camera/test \
        $(SDK_LIB_ROOT)/drivers/keypad/test \
        $(SDK_LIB_ROOT)/drivers/usb/test \
        $(SDK_LIB_ROOT)/drivers/pcie/test \
        $(SDK_LIB_ROOT)/drivers/spi/test \
        $(SDK_LIB_ROOT)/drivers/enet/test \
        $(SDK_LIB_ROOT)/drivers/keypad/test \
        $(SDK_LIB_ROOT)/drivers/accelerometer/test
endif

ifeq "$(BOARD)" "evk"
    SUBDIRS += \
	$(SDK_LIB_ROOT)/drivers/usb/test \
       	$(SDK_LIB_ROOT)/drivers/spi/test 
endif

# SATA is only available on the mx6dq.
ifeq "$(TARGET)" "mx6dq"
    SUBDIRS += $(SDK_LIB_ROOT)/drivers/sata/test
else # for mx6sdl and mx6sl
    SUBDIRS += $(SDK_LIB_ROOT)/drivers/epdc/test
endif

# available test only on the mx6sl.
ifeq "$(TARGET)" "mx6sl"
    SUBDIRS += $(SDK_LIB_ROOT)/drivers/lcdif/test
    SUBDIRS += $(SDK_LIB_ROOT)/drivers/spdc/test
    SUBDIRS += $(SDK_LIB_ROOT)/drivers/camera/test
    SUBDIRS += $(SDK_LIB_ROOT)/drivers/dcp/test
    SUBDIRS += $(SDK_LIB_ROOT)/drivers/fec/test	
endif

# Local source files.
define SOURCES
    $(APPS_ROOT)/common/platform_init.c
    $(APPS_ROOT)/common/print_clock_info.c
    $(APPS_ROOT)/common/print_version.c
    $(APPS_ROOT)/common/ivt.c
    src/unit_test_main.c
    src/all_test.c
endef

# Subdirectories will archive objects into libsdk_unit_test.a
# but not try to link the application executable since LINK_APP is not set.
$(SUBDIRS):: export ARCHIVE_APP_OBJECTS=1

# Need to include the SDK library!
LIBRARIES = \
    $(LIBSDK) \
    $(LIBBOARD) \
    $(LIBAPP)

# Specify our linker script.
LD_FILE = $(APPS_ROOT)/common/basic_sdk_app.ld.S

# Add common to include paths.
INCLUDES += -I$(APPS_ROOT)/common

# Standard targets.
include $(SDK_ROOT)/mk/targets.mk

# Special rules to force rebuilding of the unit test app when the TEST variable is
# changed. These rules must come after the inclusion of targets.mk because they
# depend upon variables defined in that file.

# The most recent value of the TEST variable is written to a current_test.txt file
# in the app's output directory. Here we construct the filename and read its current
# contents.
current_test_file := $(APP_OUTPUT_ROOT)/current_test.txt
current_test_name := $(shell [ -f $(current_test_file) ] && cat $(current_test_file))

# Delete the current test name file if the currently selected test doesn't match the
# one recorded in it.
ifneq "$(TEST)" "$(current_test_name)"
#$(info Deleting $(current_test_file); last test was $(current_test_name))
$(shell rm -f $(current_test_file))
#else
#$(info Last test $(current_test_name) matches current test $(TEST))
endif

# Object files and the application executable depend on the current test file. This
# forces rebuilding if the file doesn't exist, i.e. if we deleted it above.
$(OBJECTS_ALL) $(APP_ELF): $(current_test_file)

# To rebuild the current test file, just write the value of TEST into it.
$(current_test_file):
	echo "$(TEST)" > $(current_test_file)
#	echo "Updated current test to $(TEST)"

