CHIP		?= haas1000

DEBUG		?= 1

NOSTD		?= 0

SIMU		?= 1

ROM_BUILD	?= 1

FLASH_CHIP	?= ALL

PSRAM_ENABLE ?= 1

KBUILD_CPPFLAGS += -DMIC_SAMPLERATE=48000 -DMIC_PERIOD_MS=48

export A7_UART ?= 4
#default transq, support 1(uart0), 3(uart2)
ifeq ($(A7_UART),1)
DEBUG_PORT ?= 1
DSP_USE_AUDMA ?= 1
else ifeq ($(A7_UART),3)
DEBUG_PORT ?= 3
DSP_USE_AUDMA ?= 1
else
DEBUG_PORT	?= 4
DSP_USE_AUDMA ?= 1
endif
KBUILD_CPPFLAGS += -DA7_UART=$(A7_UART)

export A7_SRAM ?= 2
# a7 sram mode:
# 0: none, use m33 share sync flags for transq, stack/ttb use PSRAM
# 1: solo, use m33 share sync flags for transq, stack/ttb use SRAM, solo 512K, for alg test
# 2: default, solo share sync flags for transq, stack/ttb use SRAM
ifeq ($(A7_SRAM),0)
export DSP_RAM_SIZE ?= 0
export DSP_RAM_SHARE_SIZE ?= 0x15000
KBUILD_CPPFLAGS += -DDSP_RAM_SHARE_SIZE=$(DSP_RAM_SHARE_SIZE)
else ifeq ($(A7_SRAM),1)
export DSP_RAM_SIZE ?= 0x80000
KBUILD_CPPFLAGS += -DDSP_SRAM_SOLO
else
export DSP_RAM_SIZE ?= 0x80000
endif

export A7_PROJECT ?= dummy

export A7_DSP_ENABLE ?= 1
ifeq ($(A7_DSP_ENABLE),1)
KBUILD_CFLAGS += -D__A7_DSP_ENABLE__
endif

export TRACE_BAUD_RATE ?= 1500000

#KERNEL      ?= RHINO
ifneq ($(KERNEL),)
RTOS		?= 1
MBED		?= 0
else
RTOS		?= 0
endif

export CHIP_SUBSYS ?= dsp

export EXEC_IN_RAM ?= 1

export NO_CLOCK_INIT ?= 1

export NO_PMU ?= 1

export NO_SPI ?= 1

export ALLOW_WARNING ?= 1

export DSP_RAM_SIZE ?= 0x80000

export PSRAMUHS_ENABLE ?= 1

export TRACE_BUF_SIZE := 8192

export MBW_TEST ?= 0

export DSP_AUDIO_TASK ?= 1

export TRACE_CRLF ?= 1

export A7_ALI_LIB ?= 0
ifeq ($(A7_ALI_LIB), 1)
KBUILD_CPPFLAGS += -DA7_ALI_LIB
endif

export DIGMIC_EN ?= 0
ifeq ($(DIGMIC_EN),1)
KBUILD_CFLAGS += -D__DIGMIC_USED__
endif

init-y		:=
core-y		:= platform/cmsis/ platform/hal/ services/a7_dsp/ services/transq_msg/ utils/hwtimer_list/ utils/kfifo/  

core-y		+= services/a7_dsp/prj/$(A7_PROJECT)/

ifeq ($(RTOS), 1)
core-y		+= services/sys_time/ utils/bes_kv/
endif

ifeq ($(MBW_TEST), 1)
core-y += tests/mbw_test/
KBUILD_CPPFLAGS += -DMBW_TEST_NOOS -DMBW_ARRAY_SIZE_KB=256 -DMBW_BLOCK_SIZE_KB=16
endif

ifeq ($(PSRAMUHS_ENABLE), 1)
export EXEC_IN_RAM := 0
ifeq ($(PSRAM_ENABLE), 1)
export PSRAMUHS_SIZE ?= 0x800000
export DSP_PSRAMUHS_SIZE ?= 0x800000
export MCU_PSRAMUHS_SIZE ?= 0x000000
else
export PSRAMUHS_SIZE ?= 0x1000000
export DSP_PSRAMUHS_SIZE ?= 0x800000
export MCU_PSRAMUHS_SIZE ?= 0x800000
endif
KBUILD_CPPFLAGS += -DCHIP_HAS_PSRAMUHS -DPSRAMUHS_ENABLE -DMCU_PSRAMUHS_SIZE=$(MCU_PSRAMUHS_SIZE) -DDSP_PSRAMUHS_SIZE=$(DSP_PSRAMUHS_SIZE)
endif

KBUILD_CPPFLAGS += -Iplatform/cmsis/inc -Iplatform/hal -Iplatform/drivers/usb/usb_dev/inc

ifeq ($(EXEC_IN_RAM), 1)
KBUILD_CPPFLAGS += -DEXEC_IN_RAM
endif

export DSP_LIB ?= 1

KBUILD_CPPFLAGS += -DAOS_COMP_OSAL_POSIX -DAOS_COMP_CPLUSPLUS

LIB_LDFLAGS += -lstdc++ -lsupc++

CFLAGS_IMAGE +=

LDFLAGS_IMAGE +=

C_ONLY_FLAGS    := -std=gnu11
C++_ONLY_FLAGS  := -std=gnu++11

CFLAGS_IMAGE += -u _printf_float -u _scanf_float

LDFLAGS_IMAGE += --wrap malloc --wrap calloc --wrap free --wrap realloc
