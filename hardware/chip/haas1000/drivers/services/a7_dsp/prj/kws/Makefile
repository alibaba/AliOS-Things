cur_dir := $(dir $(lastword $(MAKEFILE_LIST)))

obj-y :=

ifeq ($(DSP_ENABLE),1)
obj-y += mcu_$(A7_PROJECT)_main.o
endif

ifeq ($(CHIP_SUBSYS), dsp)
obj-y += a7_$(A7_PROJECT)_main.o
ifeq ($(A7_ALI_LIB), 1)
obj-y += lib_a7_$(A7_PROJECT).a
else
obj-y += $(A7_PROJECT)/
endif
endif

ccflags-y += \
             -Iservices/transq_msg \
             -Iservices/sys_time/ \
             -Iservices/a7_dsp/common/ \
             -Iservices/a7_dsp/prj/$(A7_PROJECT)/ \
             -Iutils/hwtimer_list \
             -Iutils/ \
             -Inet \
             -Inet/net_os \
             -Iapps/common \
             -Iservices/multimedia/speech/inc/ \
             -Iutils/heap/ \
             -Iservices/audio_dump/include/
