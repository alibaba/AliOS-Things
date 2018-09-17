
NAME := RDA8955

HOST_OPENOCD := rda8955

$(NAME)_TYPE := kernel

$(NAME)_COMPONENTS += platform/arch/mips/mips-I
$(NAME)_COMPONENTS := libc rhino hal netmgr middleware.common cjson digest_algorithm network.lwip

#dual_os ?= no

#GLOBAL_LDFLAGS += -DSXR_RHINO_DUAL_OS

GLOBAL_DEFINES += CONFIG_AOS_KV_PTN=6
GLOBAL_DEFINES += CONFIG_AOS_KV_BUFFER_SIZE=8192
GLOBAL_DEFINES += DEVICE_SERIAL=1
GLOBAL_DEFINES += DEVICE_STDIO_MESSAGES=1
GLOBAL_DEFINES += DEVICE_SPI=1
GLOBAL_DEFINES += DEVICE_FLASH=1
GLOBAL_DEFINES += TARGET_FLASH_CMSIS_ALGO=1

GLOBAL_DEFINES += RHINO_CONFIG_TASK_STACK_CUR_CHECK=1

GLOBAL_INCLUDES += ../../arch/mips/mips-I/gcc \
				   ../../../network/lwip/include

GLOBAL_INCLUDES += inc/target/include \
GLOBAL_INCLUDES += include \
GLOBAL_INCLUDES += inc \
GLOBAL_INCLUDES += inc/chip/bcpu/spc/src \
GLOBAL_INCLUDES += inc/chip/bcpu/include \
GLOBAL_INCLUDES += inc/include \
GLOBAL_INCLUDES += inc/base/common/include \
GLOBAL_INCLUDES += inc/chip/regs/include \
GLOBAL_INCLUDES += inc/chip/defs/include \
GLOBAL_INCLUDES += inc/chip/patch/include \
GLOBAL_INCLUDES += inc/chip/bcpu/include \
GLOBAL_INCLUDES += inc/chip/bcpu/spc/include \
GLOBAL_INCLUDES += inc/chip/bcpu/spp/include \
GLOBAL_INCLUDES += inc/chip/bcpu/spal/include \
GLOBAL_INCLUDES += inc/chip/hal/include \
GLOBAL_INCLUDES += inc/system/vpp/include \
GLOBAL_INCLUDES += ../../../osal/aos/ \
GLOBAL_INCLUDES += aos/test/ \

GLOBAL_INCLUDES += base/common/include  \
					base/fdlibm/include  \
					base/sap/include  \
					base/std/include  \
					base/sx/include \

GLOBAL_INCLUDES += gcc-mips/include/   \
					gcc-mips/lib/gcc/mips-elf/4.4.2/include/ \
					gcc-mips/lib/gcc/mips-elf/4.4.2/include-fixed/ \
					gcc-mips/lib/gcc/mips-elf/4.4.2/include-tools/include
					
					
GLOBAL_CFLAGS += -w -EL -DCT_ASM=

GLOBAL_ASMFLAGS += -DCT_ASM=

GLOBAL_LDFLAGS += -EL 


#GLOBAL_LDFLAGS += -nostdlib   --oformat=elf32-littlemips  platform/mcu/rda8955/lib/mem_bridge_rom_CHIP.elf   platform/mcu/rda8955/lib/stripped_bcpu_elf_file.elf --script #platform/mcu/rda8955/8955.ld --gc-sections  -Lplatform/mcu/rda8955/lib  --start-group    -lat_debug -lplatform_debug -lstd_debug  -lbcpu_debug  -lpatch_debug  --end-group  #-Lbuild/compiler/gcc-mips/lib/gcc/mips-elf/4.4.2 -lgcc  --warn-common   --error-unresolved-symbols  \

#GLOBAL_LDFLAGS += -nostdlib   --oformat=elf32-littlemips  platform/mcu/rda8955/lib/mem_bridge_rom_CHIP.elf   platform/mcu/rda8955/lib/stripped_bcpu_elf_file.elf --gc-sections  #-Lplatform/mcu/rda8955/lib  -Wl,--start-group    platform/mcu/rda8955/lib/libat_debug.a   platform/mcu/rda8955/lib/libplatform_debug.a platform/mcu/rda8955/lib/libstd_debug.a  #platform/mcu/rda8955/lib/libbcpu_debug.a  platform/mcu/rda8955/lib/libpatch_debug.a   -Lbuild/compiler/gcc-mips/lib/gcc/mips-elf/4.4.2 -lgcc  -Wl,--end-group --warn-common   #--error-unresolved-symbols  \

GLOBAL_LDFLAGS += -nostdlib   --oformat=elf32-littlemips  platform/mcu/rda8955/lib/mem_bridge_rom_CHIP.elf   platform/mcu/rda8955/lib/stripped_bcpu_elf_file.elf --gc-sections  -Lplatform/mcu/rda8955/lib    -lat_debug -lplatform_debug -lstd_debug  -lbcpu_debug  -lpatch_debug   -Lbuild/compiler/gcc-mips/lib/gcc/mips-elf/4.4.2 -lgcc  -Wl,--end-group   --no-whole-archive --warn-common   --error-unresolved-symbols  \

$(NAME)_CFLAGS  += -Wall -Werror -Wno-unused-variable -Wno-unused-parameter -Wno-implicit-function-declaration
$(NAME)_CFLAGS  += -Wno-type-limits -Wno-sign-compare -Wno-pointer-sign -Wno-uninitialized
$(NAME)_CFLAGS  += -Wno-return-type -Wno-unused-function -Wno-unused-but-set-variable
$(NAME)_CFLAGS  += -Wno-unused-value -Wno-strict-aliasing

GLOBAL_LDFLAGS += -T platform/mcu/rda8955/rda8955.ld

$(NAME)_SOURCES := aos/aos_adapter.c \
                   aos/aos_adapter_ipc.c \
				   aos/hook_impl.c \
				   aos/soc_impl.c \
				   aos/aos_adapter_timeout.c \
                   hal/flash.c \
				   ../../arch/mips/mips-I/gcc/port_c.c    \
				   ../../arch/mips/mips-I/gcc/port_s.S    \
				   
ifeq ($(vcall), rhino_sxr)
$(NAME)_SOURCES += aos/test/aos_test.c aos/test/cut.c
else ifeq ($(vcall), rhino_sxr_mutios)
#GLOBAL_DEFINES += SXR_RHINO_DUAL_OS 
#$(NAME)_SOURCES += test/src/aos_test.c
#$(NAME)_SOURCES += test/src/cut.c

#GLOBAL_INCLUDES += test/include
endif	   


EXTRA_TARGET_MAKEFILES +=  $(SOURCE_ROOT)/platform/mcu/$(HOST_MCU_FAMILY)/gen_crc_bin.mk
