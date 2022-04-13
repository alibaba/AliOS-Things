cur_dir := $(dir $(lastword $(MAKEFILE_LIST)))

obj-y :=

ifeq ($(DSP_ENABLE),1)
obj-y += mcu_slave_code.o
obj-y += mcu_slave_init.o
obj-y += mcu_audio.o
obj-y += mcu_cmd.o
obj-y += a7_cmd.o
obj-y += data_dump.o
obj-y += aud_dump.o
obj-y += heart_beat.o
obj-y += audio_dump_single_channel.o
ifeq ($(DSP_IMAGE_COMP),1)
obj-y += decom/
endif
ifeq ($(DSP_AUDIO_TASK),1)
CFLAGS_mcu_slave_init.o += -D__A7_DSP_AUDIO_TASK__
endif

export DSP_BIN_NAME ?= $(CHIP)_a7
$(obj)/mcu_slave_code.o : out/$(T)/$(DSP_BIN_NAME).bin
AFLAGS_mcu_slave_code.o += -DDSP_BIN_NAME=$(DSP_BIN_NAME).bin -Iout/$(T)
out/$(T)/$(DSP_BIN_NAME).bin : $(srctree)/out/$(DSP_BIN_NAME)/$(DSP_BIN_NAME).bin
	$(call CMDCPFILE,$(srctree)/out/$(DSP_BIN_NAME)/$(DSP_BIN_NAME).bin,$(srctree)/$@)
#	$(call CMDCPFILE,$(srctree)/out/$(DSP_BIN_NAME)/$(DSP_BIN_NAME).elf,$(srctree)/$(@:.bin=.elf))
#	$(call CMDCPFILE,$(srctree)/out/$(DSP_BIN_NAME)/$(DSP_BIN_NAME).map,$(srctree)/$(@:.bin=.map))
#	$(call CMDCPFILE,$(srctree)/out/$(DSP_BIN_NAME)/$(DSP_BIN_NAME).lst,$(srctree)/$(@:.bin=.lst))
$(srctree)/out/$(DSP_BIN_NAME)/$(DSP_BIN_NAME).bin : FORCE
	$(call echo-help,)
	$(call echo-help,INFO    Please make sure DSP bin is up to date: out/$(DSP_BIN_NAME)/$(DSP_BIN_NAME).bin)
# We cannot build $(DSP_BIN_NAME) automatically here, because the env inherited by sub-make cannot be cleaned:
# E.g., KBUILD_SRC, TC, CHIP_HAS_CP, ...
	$(call echo-help,)
endif

ifeq ($(CHIP_SUBSYS), dsp)
obj-y += a7_main.o
obj-y += a7_audio.o
obj-y += data_dump.o
obj-y += aud_dump.o
obj-y += a7_cmd.o
ifeq ($(RTOS),1)
obj-y += my_a7_test.o
obj-y += heart_beat.o
endif
# solo SRAM for alg
ifeq ($(A7_SRAM),1)
obj-y += ../../../net/net_os/net_memory.o
endif

ifneq ($(DEBUG_PORT),)
CFLAGS_a7_main.o += -DDEBUG_PORT=$(DEBUG_PORT)
endif

ifeq ($(NO_CLOCK_INIT),1)
CFLAGS_a7_main.o += -DNO_CLOCK_INIT
endif
ifeq ($(NO_PMU),1)
CFLAGS_a7_main.o += -DNO_PMU
endif
ifeq ($(NO_LPU_26M),1)
CFLAGS_a7_main.o += -DNO_LPU_26M
endif
ifeq ($(NO_PLL),1)
CFLAGS_a7_main.o += -DNO_PLL
endif
ifeq ($(NO_SPI),1)
CFLAGS_a7_main.o += -DNO_SPI
endif
ifeq ($(MBW_TEST),1)
CFLAGS_a7_main.o += -DMBW_TEST
endif
ifeq ($(DSP_AUDIO_TASK),1)
CFLAGS_a7_main.o += -D__A7_DSP_AUDIO_TASK__
endif
endif

ifeq ($(A7_DSP_HEARTBEAT_CHECK), 1)
CFLAGS_heart_beat.o += -DA7_DSP_HEARTBEAT_CHECK
endif

ccflags-y += \
             -Iservices/audioflinger \
             -Iservices/transq_msg \
             -Iservices/sys_time/ \
             -Iutils/hwtimer_list \
             -Iutils/ \
             -Iapps/key \
             -Inet \
             -Inet/net_os \
             -Iapps/common
