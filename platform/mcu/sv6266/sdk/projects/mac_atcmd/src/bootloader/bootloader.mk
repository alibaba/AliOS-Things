#################################################################
#																#
# Source code to each application								#
#																#
#################################################################
APP := $(TOPDIR)/$(PROJ_DIR)/src/bootloader/bootloader

LDSCRIPT_S := $(TOPDIR)/$(PROJ_DIR)/src/bootloader/bootloader.lds.S
LINKSCRIPT := $(TOPDIR)/$(PROJ_DIR)/src/bootloader/linkscript.lds

CUST_SRCS += $(TOPDIR)/$(PROJ_DIR)/src/bootloader/custom_ota/custom_ota.c
CUST_SRCS += $(TOPDIR)/$(PROJ_DIR)/src/bootloader/custom_ota/boot_init.c

CUST_INC := -I$(TOPDIR)/$(PROJ_DIR)/src/bootloader/lib
CUST_INC += -I$(TOPDIR)/$(PROJ_DIR)/src/bootloader/tiny_printf

INC += -I$(TOPDIR)/components/bsp/soc/$(CHIP_NAME)

ifeq ($(strip $(VERBOSE)), 1)
Q := @
else
Q :=
endif

.PHONY: all clean distclean

all: ${APP}.elf ${APP}.bin

$(LINKSCRIPT): $(LDSCRIPT_S)
	@$(CC) -E -P $< $(INC) $(CFLAGS) -o $@

clean:
	@$(RM) -f ${APP}.elf ${APP}.bin ${APP}.map $(LINKSCRIPT)

distclean: clean

${APP}.elf: ${APP}.a $(LINKSCRIPT) ${CUST_SRCS}
	$(Q)$(CC) ${CUST_SRCS} -Wl,--start-group $< -Wl,--end-group $(INC) $(CUST_INC) $(CFLAGS) $(LDFLAGS) -Xlinker -Map=${APP}.map -T $(LINKSCRIPT) -o $@

${APP}.bin: ${APP}.elf
	@$(OBJCOPY) $< -O binary $@

