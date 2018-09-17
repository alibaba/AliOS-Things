NAME = e310

HOST_OPENOCD := e310

# Host architecture is RISC-V
HOST_ARCH := RISC-V

$(NAME)_TYPE := kernel

$(NAME)_COMPONENTS += platform/arch/risc-v/risc_v32I
$(NAME)_COMPONENTS += libc rhino hal middleware.common

GLOBAL_INCLUDES += ../../../arch/risc-v/risc_v32I/gcc
GLOBAL_INCLUDES += ../../../../include/hal/soc/
GLOBAL_INCLUDES += ../aos   \
					../freedom-e-sdk/bsp/env    \
					../freedom-e-sdk/bsp/env/freedom-e300-hifive1    \
					../freedom-e-sdk/bsp/include    \
					../freedom-e-sdk/bsp/drivers    \
					../freedom-e-sdk/bsp/drivers/plic    \
					

#-mthumb -mthumb-interwork
GLOBAL_CFLAGS += 

GLOBAL_CFLAGS += -w  

GLOBAL_ASMFLAGS += -w

#--specs=nosys.specs
GLOBAL_LDFLAGS += -Wl,--gc-sections \
				  -nostartfiles \
				  --specs=nosys.specs \
				  -usystem_vectors \

$(NAME)_CFLAGS  += -Wall -Werror -Wno-unused-variable -Wno-unused-parameter -Wno-implicit-function-declaration
$(NAME)_CFLAGS  += -Wno-type-limits -Wno-sign-compare -Wno-pointer-sign -Wno-uninitialized
$(NAME)_CFLAGS  += -Wno-return-type -Wno-unused-function -Wno-unused-but-set-variable
$(NAME)_CFLAGS  += -Wno-unused-value -Wno-strict-aliasing

#../freedom-e-sdk/bsp/drivers/plic/plic_driver.c   \
#../aos/global_interrupts.c \

$(NAME)_SOURCES := 	../freedom-e-sdk/bsp/env/start.S   \
					../e310/entry.S    \
					../aos/init.c   \
					../aos/aos_main.c   \
					../aos/hook_impl.c   \
					../aos/soc_impl.c   \
					../aos/soc_init.c \
					../freedom-e-sdk/bsp/drivers/plic/plic_driver.c   \
					../aos/global_interrupts.c \
					../hal/hal_interpt_hifive1.c \
					../hal/hal_uart_hifive1.c \



GLOBAL_LDS_FILES += platform/mcu/freedom-e/e310/flash.lds

GLOBAL_DEFINES += CONFIG_ARM

GLOBAL_CFLAGS += -DUSE_M_TIME -DUSE_PLIC

#GLOBAL_CFLAGS += -march=rv32imac -mabi=ilp32 -g
#GLOBAL_ASMFLAGS += -march=rv32imac -mabi=ilp32 -g
#GLOBAL_LDFLAGS += -march=rv32imac -mabi=ilp32 -g

