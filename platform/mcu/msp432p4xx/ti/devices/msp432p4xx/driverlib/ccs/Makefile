MSP432LIB = msp432p4xx_driverlib.lib
ARFLAGS=$(MSP432LIB)
CFLAGS=-mv7M4 -mt --fp_mode=strict --gcc --endian=little --enum_type=packed \
--elf --sat_reassoc=off --fp_reassoc=off --gen_func_subsections=on --abi=eabi \
--code_state=16 --plain_char=unsigned -eo=.o -mf0 -O2 -oi0 

CFLAGS+=-I$(SDK_ROOT_INCLUDE)/source/
CFLAGS+=-I$(SDK_ROOT_INCLUDE)/source/third_party/CMSIS/Include/
CFLAGS+=-I$(SDK_ROOT_INCLUDE)/source/ti/devices/msp432p4xx/inc/
CFLAGS+=-I$(CGTOOLS)/include/

OBJ = ../adc14.o ../aes256.o ../comp_e.o ../cpu.o ../crc32.o ../cs.o ../dma.o ../fpu.o \
../gpio.o ../i2c.o ../interrupt.o ../mpu.o ../pmap.o ../pcm.o ../pss.o ../ref_a.o ../reset.o ../rtc_c.o \
../spi.o ../systick.o ../timer_a.o ../timer32.o ../uart.o ../wdt_a.o

OBJ_401 = ../flash.o ../sysctl.o
OBJ_4111 = $(OBJ) ../flash_a.o ../sysctl_a.o ../lcd_f.o

CFLAGS_401=$(CFLAGS) -D__MSP432P401R__
CFLAGS_4111=$(CFLAGS) -D__MSP432P4111__

LINKOBJS = adc14.o aes256.o comp_e.o cpu.o crc32.o cs.o dma.o fpu.o gpio.o \
i2c.o interrupt.o mpu.o pmap.o pcm.o pss.o ref_a.o reset.o rtc_c.o flash.o \
sysctl.o sysctl_a.o flash_a.o lcd_f.o

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

clean:
	rm -f *.o
	rm -f $(MSP432LIB)