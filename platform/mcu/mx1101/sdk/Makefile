# ------------------------------------------------
# Author  : Snow Yang
# Date    : 2017-12-04
# Version : V0.0.1
# ------------------------------------------------

######################################
# target
######################################
TARGET = mx1101


######################################
# building variables
######################################
# debug build?
DEBUG = 1
# optimization
OPT = -Og


#######################################
# paths
#######################################

# firmware library path
PERIFLIB_PATH = 

# Build path
BUILD_DIR = build

######################################
# source
######################################
# C sources
C_SOURCES =  \
main.c 

# ASM sources
ASM_SOURCES = \
startup.s


######################################
# firmware library
######################################
PERIFLIB_SOURCES = 


#######################################
# binaries
#######################################
BINPATH = /Users/yangshiwei/work/gcc-arm-none-eabi-5_4-2016q3/bin
PREFIX = arm-none-eabi-
CC = $(BINPATH)/$(PREFIX)gcc
AS = $(BINPATH)/$(PREFIX)gcc -x assembler-with-cpp
CP = $(BINPATH)/$(PREFIX)objcopy
AR = $(BINPATH)/$(PREFIX)ar
SZ = $(BINPATH)/$(PREFIX)size
HEX = $(CP) -O ihex
BIN = $(CP) -O binary -S
 
#######################################
# CFLAGS
#######################################
# cpu
CPU = -mcpu=cortex-m3

# fpu
# NONE for Cortex-M0/M0+/M3

# float-abi


# mcu
MCU = $(CPU) -mthumb $(FPU) $(FLOAT-ABI)

# macros for gcc
# AS defines
AS_DEFS = 

# C defines
C_DEFS =


# AS includes
AS_INCLUDES = 

# C includes
C_INCLUDES =  \
-ICMSIS/Include \
-IO18B_SDK_GCC/MVs_new/MVs18_SDK/Libraries/driver/inc

# compile gcc flags
ASFLAGS = $(MCU) $(AS_DEFS) $(AS_INCLUDES) $(OPT) -Wall -fdata-sections -ffunction-sections

CFLAGS = $(MCU) $(C_DEFS) $(C_INCLUDES) $(OPT) -Wall -fdata-sections -ffunction-sections

ifeq ($(DEBUG), 1)
CFLAGS += -g -gdwarf-2
endif


# Generate dependency information
CFLAGS += -MMD


#######################################
# LDFLAGS
#######################################
# link script
LDSCRIPT = link.ld

# libraries
LIBS = -lc -lm -lnosys
LIBDIR =
LDFLAGS = $(MCU) -specs=nano.specs -T$(LDSCRIPT) $(LIBDIR) $(LIBS) -Wl,-Map=$(BUILD_DIR)/$(TARGET).map,--cref -Wl,--gc-sections

# default action: build all
all: $(BUILD_DIR)/$(TARGET).elf $(BUILD_DIR)/$(TARGET).hex $(BUILD_DIR)/$(TARGET).bin


#######################################
# build the application
#######################################
# list of objects
OBJECTS = $(addprefix $(BUILD_DIR)/,$(notdir $(C_SOURCES:.c=.o)))
vpath %.c $(sort $(dir $(C_SOURCES)))
# list of ASM program objects
OBJECTS += $(addprefix $(BUILD_DIR)/,$(notdir $(ASM_SOURCES:.s=.o)))
vpath %.s $(sort $(dir $(ASM_SOURCES)))

OBJECTS += O18B_SDK_GCC/lib/libdriver.a

$(BUILD_DIR)/%.o: %.c Makefile | $(BUILD_DIR) 
	@echo compiling $<
	@$(CC) -c $(CFLAGS) -Wa,-a,-ad,-alms=$(BUILD_DIR)/$(notdir $(<:.c=.lst)) $< -o $@

$(BUILD_DIR)/%.o: %.s Makefile | $(BUILD_DIR)
	@echo compiling $<
	@$(AS) -c $(CFLAGS) $< -o $@

$(BUILD_DIR)/$(TARGET).elf: $(OBJECTS) Makefile
	@echo generating $@
	@$(CC) $(OBJECTS) $(LDFLAGS) -o $@
	@$(SZ) $@

$(BUILD_DIR)/%.hex: $(BUILD_DIR)/%.elf | $(BUILD_DIR)
	@echo generating $@
	@$(HEX) $< $@
	
$(BUILD_DIR)/%.bin: $(BUILD_DIR)/%.elf | $(BUILD_DIR)
	@echo generating $@
	@$(BIN) $< $@	
	
$(BUILD_DIR):
	mkdir $@		

#######################################
# clean up
#######################################
clean:
	@echo cleaning...
	@-rm -fR .dep $(BUILD_DIR)
	@echo done
  
#######################################
# dependencies
#######################################
-include $(addprefix $(BUILD_DIR)/,$(notdir $(C_SOURCES:.c=.d)))

# *** EOF ***