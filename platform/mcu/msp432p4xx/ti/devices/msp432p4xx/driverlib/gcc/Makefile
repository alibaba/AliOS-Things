SDK_ROOT_INCLUDE = ../../../../../../
include $(SDK_ROOT_INCLUDE)/imports.mak

CC = $(GCC_ARMCOMPILER)/bin/arm-none-eabi-gcc
AR = $(GCC_ARMCOMPILER)/bin/arm-none-eabi-ar

MSP432LIB = msp432p4xx_driverlib.a
ARFLAGS=$(MSP432LIB)
CFLAGS=-mthumb -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mcpu=cortex-m4 -O4 -ffunction-sections -fdata-sections -MD -std=c99 -c 

CFLAGS+=-I$(SDK_ROOT_INCLUDE)/source/
CFLAGS+=-I$(SDK_ROOT_INCLUDE)/source/third_party/CMSIS/Include/
CFLAGS+=-I$(SDK_ROOT_INCLUDE)/source/ti/devices/msp432p4xx/inc/
CFLAGS+=-I$(GCC_ARMCOMPILER)/arm-none-eabi/include/
CFLAGS_401=$(CFLAGS) -D__MSP432P401R__
CFLAGS_4111=$(CFLAGS) -D__MSP432P4111__

OBJ = ../adc14.o ../aes256.o ../comp_e.o ../cpu.o ../crc32.o ../cs.o ../dma.o ../fpu.o \
../gpio.o ../i2c.o ../interrupt.o ../mpu.o ../pmap.o ../pcm.o ../pss.o ../ref_a.o ../reset.o ../rtc_c.o \
../spi.o ../systick.o ../timer_a.o ../timer32.o ../uart.o ../wdt_a.o
OBJ_401 = ../flash.o ../sysctl.o
OBJ_4111 = $(OBJ) ../flash_a.o ../sysctl_a.o ../lcd_f.o

LINKOBJS = adc14.o aes256.o comp_e.o cpu.o crc32.o cs.o dma.o fpu.o gpio.o \
i2c.o interrupt.o mpu.o pmap.o pcm.o pss.o ref_a.o reset.o rtc_c.o flash.o \
sysctl.o sysctl_a.o flash_a.o lcd_f.o

.PHONY: all
all: $(MSP432LIB)

$(OBJ_401): %.o: %.c
	@echo "  CC      ${<}"
	@${CC} ${CFLAGS_401} $<

$(OBJ_4111): %.o: %.c
	@echo "  CC      ${<}"
	@${CC} ${CFLAGS_4111} ${<}

$(MSP432LIB): $(OBJ_401) $(OBJ_4111)
	@echo "  AR      $(MSP432LIB)"
	@$(AR) rs $(MSP432LIB) $(LINKOBJS)  > /dev/null

.PHONY: clean
clean:
	@rm -f *.o
	@rm -f *.d
	@rm -f $(MSP432LIB)