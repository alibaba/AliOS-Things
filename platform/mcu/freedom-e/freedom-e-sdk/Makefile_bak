#############################################################
# Configuration
#############################################################

# Allows users to create Makefile.local or ../Makefile.project with
# configuration variables, so they don't have to be set on the command-line
# every time.
extra_configs := $(wildcard Makefile.local ../Makefile.project)
ifneq ($(extra_configs),)
$(info Obtaining additional make variables from $(extra_configs))
include $(extra_configs)
endif

# Default target
BOARD ?= freedom-e300-hifive1
PROGRAM ?= demo_gpio
LINK_TARGET ?= flash
GDB_PORT ?= 3333

# Variables the user probably shouldn't override.
builddir := work/build
installdir := work/install
toolchain_srcdir := riscv-gnu-toolchain
openocd_srcdir := openocd

#############################################################
# BSP Loading
#############################################################

# Finds the directory in which this BSP is located, ensuring that there is
# exactly one.
board_dir := $(wildcard bsp/env/$(BOARD))
ifeq ($(words $(board_dir)),0)
$(error Unable to find BSP for $(BOARD), expected to find either "bsp/$(BOARD)" or "bsp-addons/$(BOARD)")
endif
ifneq ($(words $(board_dir)),1)
$(error Found multiple BSPs for $(BOARD): "$(board_dir)")
endif

# There must be a settings makefile fragment in the BSP's board directory.
ifeq ($(wildcard $(board_dir)/settings.mk),)
$(error Unable to find BSP for $(BOARD), expected to find $(board_dir)/settings.mk)
endif
include $(board_dir)/settings.mk

ifeq ($(RISCV_ARCH),)
$(error $(board_dir)/board.mk must set RISCV_ARCH, the RISC-V ISA string to target)
endif

ifeq ($(RISCV_ABI),)
$(error $(board_dir)/board.mk must set RISCV_ABI, the ABI to target)
endif

# Determines the XLEN from the toolchain tuple
ifeq ($(patsubst rv32%,rv32,$(RISCV_ARCH)),rv32)
RISCV_XLEN := 32
else ifeq ($(patsubst rv64%,rv64,$(RISCV_ARCH)),rv64)
RISCV_XLEN := 64
else
$(error Unable to determine XLEN from $(RISCV_ARCH))
endif

#############################################################
# Prints help message
#############################################################
.PHONY: help
help:
	@echo "  SiFive Freedom E Software Development Kit "
	@echo "  Makefile targets:"
	@echo ""
	@echo " tools [BOARD = $(BOARD)]:"
	@echo "    Install compilation & debugging tools to target your desired board."
	@echo ""
	@echo " toolchain-clean:"
	@echo "    Removes the installed toolchain."
	@echo ""
	@echo " openocd-clean:"
	@echo "    Removes the locally built instance of OpenOCD."
	@echo ""
	@echo " uninstall:"
	@echo "    Uninstall the compilation & debugging tools."
	@echo ""
	@echo " software [PROGRAM=$(PROGRAM) BOARD=$(BOARD)]:"
	@echo "    Build a software program to load with the"
	@echo "    debugger."
	@echo ""
	@echo " clean [PROGRAM=$(PROGRAM) BOARD=$(BOARD)]:"
	@echo "    Clean compiled objects for a specified "
	@echo "    software program."
	@echo ""
	@echo " upload [PROGRAM=$(PROGRAM) BOARD=$(BOARD)]:"
	@echo "    Launch OpenOCD to flash your program to the"
	@echo "    on-board Flash."
	@echo ""
	@echo " run_openocd [BOARD=$(BOARD)]:"
	@echo " run_gdb     [PROGRAM=$(PROGRAM) BOARD=$(BOARD)]:"
	@echo "     Launch OpenOCD or GDB seperately. Allows Ctrl-C to halt running"
	@echo "     programs."
	@echo ""
	@echo " dasm [PROGRAM=$(PROGRAM)]:"
	@echo "     Generates the dissassembly output of 'objdump -D' to stdout."
	@echo ""
	@echo " For more information, visit dev.sifive.com"

#############################################################
# This section is for tool installation
#############################################################
.PHONY: tools
tools: riscv-gnu-toolchain openocd

# Pointers to various important tools in the toolchain.
toolchain_builddir := $(builddir)/riscv-gnu-toolchain/riscv64-unknown-elf
toolchain_prefix := $(toolchain_builddir)/prefix

RISCV_PATH ?= $(toolchain_prefix)

RISCV_GCC     := $(abspath $(RISCV_PATH)/bin/riscv64-unknown-elf-gcc)
RISCV_GXX     := $(abspath $(RISCV_PATH)/bin/riscv64-unknown-elf-g++)
RISCV_OBJDUMP := $(abspath $(RISCV_PATH)/bin/riscv64-unknown-elf-objdump)
RISCV_GDB     := $(abspath $(RISCV_PATH)/bin/riscv64-unknown-elf-gdb)
RISCV_AR      := $(abspath $(RISCV_PATH)/bin/riscv64-unknown-elf-ar)

PATH := $(abspath $(RISCV_PATH)/bin):$(PATH)

$(RISCV_GCC) $(RISCV_GXX) $(RISCV_OBJDUMP) $(RISCV_GDB) $(RISCV_AR): $(toolchain_builddir)/install.stamp
	touch -c $@

# Builds riscv-gnu-toolchain, which contains GCC and all the supporting
# software for C code.
.PHONY: riscv-gnu-toolchain
riscv-gnu-toolchain: $(RISCV_GCC) $(RISCV_GXX) $(RISCV_OBJDUMP) $(RISCV_GDB) $(RISCV_AR)

$(builddir)/riscv-gnu-toolchain/%/install.stamp: $(builddir)/riscv-gnu-toolchain/%/build.stamp
	$(MAKE) -C $(dir $@) install
	date > $@

$(builddir)/riscv-gnu-toolchain/%/build.stamp: $(builddir)/riscv-gnu-toolchain/%/configure.stamp
	$(MAKE) -C $(dir $@)
	date > $@

$(builddir)/riscv-gnu-toolchain/%-elf/configure.stamp:
	$(eval $@_TUPLE := $(patsubst $(builddir)/riscv-gnu-toolchain/%-elf/configure.stamp,%,$@))
	rm -rf $(dir $@)
	mkdir -p $(dir $@)
	cd $(dir $@); $(abspath $(toolchain_srcdir)/configure) \
		--prefix=$(abspath $(dir $@)/prefix) \
		--disable-linux \
		--enable-multilib \
		--with-cmodel=medany \
		--with-libgcc-cmodel
	date > $@

.PHONY: toolchain-clean
toolchain-clean:
	rm -rf $(toolchain_builddir)

# Builds and installs OpenOCD, which translates GDB into JTAG for debugging and
# initializing the target.
openocd_builddir := $(builddir)/openocd
openocd_prefix := $(openocd_builddir)/prefix

RISCV_OPENOCD_PATH ?= $(openocd_prefix)
RISCV_OPENOCD ?= $(RISCV_OPENOCD_PATH)/bin/openocd

.PHONY: openocd
openocd: $(RISCV_OPENOCD)

$(RISCV_OPENOCD): $(openocd_builddir)/install.stamp
	touch -c $@

$(openocd_builddir)/install.stamp: $(openocd_builddir)/build.stamp
	$(MAKE) -C $(dir $@) install
	date > $@

$(openocd_builddir)/build.stamp: $(openocd_builddir)/configure.stamp
	$(MAKE) -C $(dir $@)
	date > $@

$(openocd_builddir)/configure.stamp:
	rm -rf $(dir $@)
	mkdir -p $(dir $@)
	cd $(abspath $(openocd_srcdir)); autoreconf -i
	cd $(dir $@); $(abspath $(openocd_srcdir)/configure) \
		--prefix=$(abspath $(dir $@)/prefix) \
		--disable-werror
	date > $@

.PHONY: openocd-clean
openocd-clean:
	rm -rf $(openocd_builddir)

#############################################################
# This Section is for Software Compilation
#############################################################
PROGRAM_DIR = software/$(PROGRAM)
PROGRAM_ELF = software/$(PROGRAM)/$(PROGRAM)

.PHONY: software_clean
clean: software_clean
software_clean:
	$(MAKE) -C $(PROGRAM_DIR) CC=$(RISCV_GCC) RISCV_ARCH=$(RISCV_ARCH) RISCV_ABI=$(RISCV_ABI) AR=$(RISCV_AR) BSP_BASE=$(abspath bsp) BOARD=$(BOARD) LINK_TARGET=$(LINK_TARGET) clean

.PHONY: software
software: software_clean
	$(MAKE) -C $(PROGRAM_DIR) CC=$(RISCV_GCC) RISCV_ARCH=$(RISCV_ARCH) RISCV_ABI=$(RISCV_ABI) AR=$(RISCV_AR) BSP_BASE=$(abspath bsp) BOARD=$(BOARD) LINK_TARGET=$(LINK_TARGET)

dasm: software $(RISCV_OBJDUMP)
	$(RISCV_OBJDUMP) -D $(PROGRAM_ELF)

#############################################################
# This Section is for uploading a program to SPI Flash
#############################################################
OPENOCDCFG ?= bsp/env/$(BOARD)/openocd.cfg
OPENOCDARGS += -f $(OPENOCDCFG)

GDB_UPLOAD_ARGS ?= --batch

GDB_UPLOAD_CMDS += -ex "set remotetimeout 240"
GDB_UPLOAD_CMDS += -ex "target extended-remote localhost:$(GDB_PORT)"
GDB_UPLOAD_CMDS += -ex "monitor reset halt"
GDB_UPLOAD_CMDS += -ex "monitor flash protect 0 64 last off"
GDB_UPLOAD_CMDS += -ex "load"
GDB_UPLOAD_CMDS += -ex "monitor resume"
GDB_UPLOAD_CMDS += -ex "monitor shutdown"
GDB_UPLOAD_CMDS += -ex "quit"

upload:
	$(RISCV_OPENOCD) $(OPENOCDARGS) & \
	$(RISCV_GDB) $(PROGRAM_DIR)/$(PROGRAM) $(GDB_UPLOAD_ARGS) $(GDB_UPLOAD_CMDS) && \
	echo "Successfully uploaded '$(PROGRAM)' to $(BOARD)."

#############################################################
# This Section is for launching the debugger
#############################################################

run_openocd:
	$(RISCV_OPENOCD) $(OPENOCDARGS)

GDBCMDS += -ex "set remotetimeout 240"
GDBCMDS += -ex "target extended-remote localhost:$(GDB_PORT)"

run_gdb:
	$(RISCV_GDB) $(PROGRAM_DIR)/$(PROGRAM) $(GDBARGS) $(GDBCMDS)
