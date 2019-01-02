COMPILER_PATH ?= 

ifeq ($(COMPILER),armcc)
CONFIG_ENV_CFLAGS += -DSTM32L496xx \
					 --cpu=Cortex-M4 \
					 --apcs=interwork \
					 --split_sections
TOOLCHAIN_PATH :=
				 
else ifeq ($(COMPILER),iar)
CONFIG_ENV_CFLAGS += -DSTM32L496xx \
					--cpu=Cortex-M4 \
                    --cpu_mode=thumb \
                    --endian=little \
				    
TOOLCHAIN_PATH :=

else
CONFIG_ENV_CFLAGS += -DSTM32L496xx \
				     -mcpu=cortex-m4 \
					 -mlittle-endian \
					 -mthumb \
					 -mthumb-interwork
TOOLCHAIN_PATH :=
TOOLCHAIN_PREFIX:=arm-none-eabi-
endif