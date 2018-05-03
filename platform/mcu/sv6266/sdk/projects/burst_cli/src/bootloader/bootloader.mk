#################################################################
#																#
# Source code to each application								#
#																#
#################################################################
APP := $(TOPDIR)/$(PROJ_DIR)/src/bootloader/bootloader

LDSCRIPT_S := $(TOPDIR)/$(PROJ_DIR)/src/bootloader/bootloader.lds.S
LINKSCRIPT := $(TOPDIR)/$(PROJ_DIR)/src/bootloader/linkscript.lds

.PHONY: all clean distclean

all: ${APP}.elf ${APP}.bin

$(LINKSCRIPT): $(LDSCRIPT_S)
	@$(CC) -E -P $< $(INC) $(CFLAGS) -o $@

clean:
	@$(RM) -f ${APP}.elf ${APP}.bin ${APP}.map $(LINKSCRIPT)

distclean: clean

${APP}.elf: ${APP}.a $(LINKSCRIPT)
	@$(CC) -Wl,--start-group $< -Wl,--end-group $(INC) $(CFLAGS) $(LDFLAGS) -Xlinker -Map=${APP}.map -T $(LINKSCRIPT) -o $@

${APP}.bin: ${APP}.elf
	@$(OBJCOPY) $< -O binary $@

