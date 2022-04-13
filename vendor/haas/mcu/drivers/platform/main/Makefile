cur_dir := $(dir $(lastword $(MAKEFILE_LIST)))

#obj-y := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)*.c $(cur_dir)*.cpp $(cur_dir)*.S))
obj-y :=

ifeq ($(ROM_BUILD),1)
obj-y += ../../tests/rom/startup_ARMCM.S
else
obj-y += startup_main.S
AFLAGS_startup_main.o += -DRAM_SIZE=$(RAM_SIZE)
ifeq ($(A7_DSP_ENABLE),1)
AFLAGS_startup_main.o += -D__A7_DSP_ENABLE__
endif
endif

ifeq ($(FACTORY_SUITE),1)
obj-y += ../../tests/factory_suite/
else ifeq ($(RELIABILITY_TEST),1)
obj-y += ../../tests/reliability_test/
else ifeq ($(NOSTD),1)
obj-y += nostd_main.c
else ifeq ($(NOAPP),1)
obj-y += noapp_main.cpp
else ifeq ($(MBW_TEST_ONLY),1)
obj-y += ../../tests/mbw_test/mbw.c
ccflags-y += -DMBW_TEST
obj-y += ../../net/utils/vsprintf.c
else
obj-y += main.cpp
endif


ifneq ($(filter 1,$(USB_SERIAL_TEST) $(USB_SERIAL_DIRECT_XFER_TEST) $(USB_AUDIO_TEST) $(I2C_TEST) $(AF_TEST) $(VD_TEST) $(CP_TEST) $(TDM_TEST) $(SEC_ENG_TEST) $(A7_DSP_TEST) $(TRANSQ_TEST) $(PSRAM_TEST) $(PSRAMUHS_TEST) $(WIFI_TEST)),)
ccflags-y += -Itests/hwtest -DHWTEST

ifneq ($(filter 1,$(USB_SERIAL_TEST) $(USB_SERIAL_DIRECT_XFER_TEST) $(USB_AUDIO_TEST)),)
obj-y += ../../tests/hwtest/usb_dev_test.c
ifeq ($(USB_SERIAL_TEST),1)
ccflags-y += -DUSB_SERIAL_TEST
else ifeq ($(USB_SERIAL_DIRECT_XFER_TEST),1)
ccflags-y += -DUSB_SERIAL_DIRECT_XFER_TEST
else
ccflags-y += -DUSB_AUDIO_TEST
endif
endif

ifneq ($(USB_SERIAL_DIRECT_XFER_SIZE),)
ccflags-y += -DUSB_SERIAL_DIRECT_XFER_SIZE=$(USB_SERIAL_DIRECT_XFER_SIZE)
endif

ifeq ($(I2C_TEST),1)
obj-y += ../../tests/hwtest/i2c_test.c
ccflags-y += -DI2C_TEST

ifeq ($(I2C_TEST_DMA_MODE),1)
CFLAGS_i2c_test.o += -DI2C_TEST_DMA_MODE
endif
ifeq ($(I2C_TEST_INT_MODE),1)
CFLAGS_i2c_test.o += -DI2C_TEST_INT_MODE
endif
ifneq ($(I2C_SPEED),)
CFLAGS_i2c_test.o += -DI2C_SPEED=$(I2C_SPEED)
endif
endif # I2C_TEST

ifeq ($(AF_TEST),1)
obj-y += ../../services/audioflinger/  ../../tests/hwtest/af_test.c
ccflags-y += -Iservices/audioflinger -DAF_TEST

ifeq ($(AF_INT_CODEC_TEST),1)
obj-y += ../../platform/drivers/codec/
CFLAGS_af_test.o += -DAF_INT_CODEC_TEST
export AF_DEVICE_INT_CODEC ?= 1
export AF_DEVICE_I2S ?= 0
else
export AF_DEVICE_INT_CODEC ?= 0
export AF_DEVICE_I2S ?= 1
endif

ifneq ($(CHAN_NUM_CAPTURE),)
CFLAGS_af_test.o += -DCHAN_NUM_CAPTURE=$(CHAN_NUM_CAPTURE)
endif
ifneq ($(CHAN_SEP_BUF_CAPTURE),)
CFLAGS_af_test.o += -DCHAN_SEP_BUF_CAPTURE=$(CHAN_SEP_BUF_CAPTURE)
endif
ifeq ($(FIXED_BUF_CAPTURE),1)
CFLAGS_af_test.o += -DFIXED_BUF_CAPTURE
endif
endif # AF_TEST

ifeq ($(VD_TEST),1)
obj-y += ../../apps/voice_detector/voice_detector.c \
    ../../apps/common/app_utils.c \
    ../../services/audioflinger/ \
    ../../services/audio_dump/ \
    ../../platform/drivers/codec/
ccflags-y += -DVD_TEST \
    -Iapps/voice_detector \
    -Iapps/common \
    -Iservices/audio_dump/include \
    -Iservices/multimedia/speech/inc
subdir-ccflags-y += -Iservices/audioflinger
endif

ifeq ($(CP_TEST),1)
obj-y += ../../tests/hwtest/cp_test.c
ccflags-y += -DCP_TEST
endif

ifeq ($(TDM_TEST),1)
obj-y += ../../tests/hwtest/tdm_test.c
ccflags-y += -DTDM_TEST
ccflags-y += -DCHIP_HAS_I2S=$(CHIP_HAS_I2S)
ifneq ($(CHIP_HAS_TDM),)
ccflags-y += -DCHIP_HAS_TDM=$(CHIP_HAS_TDM)
endif

ifeq ($(CHIP_HAS_BCM),1)
CFLAGS_se_test.o += -DCHIP_HAS_BCM
CFLAGS_se_aes_test.o += -DCHIP_HAS_BCM
endif

ifeq ($(LIS25_TEST), 1)
ccflags-y += -DLIS25_TEST
obj-y += ../../tests/codec_vad/lis25ba.c
ccflags-y += -Itests/codec_vad/
endif
ifeq ($(TDM_LOOPBACK_TEST), 1)
ccflags-y += -DTDM_LOOPBACK_TEST
endif
ifeq ($(TDM_PLAYBACK_ONLY_TEST), 1)
ccflags-y += -DTDM_PLAYBACK_ONLY_TEST
endif
endif #TDM_TEST

ifeq ($(SEC_ENG_TEST),1)
obj-y += ../../tests/hwtest/sec_eng_test.c ../../utils/hexdump/
ccflags-y += -DSEC_ENG_TEST -Iutils/hexdump
ifneq ($(filter 1,$(AES_LARGE_DATA) $(HASH_LARGE_DATA) $(HMAC_LARGE_DATA)),)
obj-y += ../../tests/hwtest/asm_test.S
asflags-y += -Itests/hwtest
ifeq ($(AES_LARGE_DATA),1)
asflags-y += -DAES_LARGE_DATA
ccflags-y += -DAES_LARGE_DATA
endif
ifeq ($(HASH_LARGE_DATA),1)
asflags-y += -DHASH_LARGE_DATA
ccflags-y += -DHASH_LARGE_DATA
endif
ifeq ($(HMAC_LARGE_DATA),1)
asflags-y += -DHMAC_LARGE_DATA
ccflags-y += -DHMAC_LARGE_DATA
endif
endif
endif

$(call echo-help,INFO    Please make sure DSP bin is up to date: out/$(DSP_BIN_NAME)/$(DSP_BIN_NAME).bin)
ifeq ($(A7_DSP_TEST),1)
obj-y += ../../tests/hwtest/a7_dsp_test.c
CFLAGS_a7_dsp_test.o += -Iservices/transq_msg
ifeq ($(CHIP_HAS_PSRAMUHS), 1)
CFLAGS_a7_dsp_test.o += -DCHIP_HAS_PSRAMUHS
ifeq ($(PSRAMUHS_ENABLE), 1)
CFLAGS_a7_dsp_test.o += -DPSRAMUHS_ENABLE
endif
endif
obj-y += ../../tests/hwtest/asm_test.S
ccflags-y += -DA7_DSP_TEST
AFLAGS_asm_test.o += -DA7_DSP_TEST -DDSP_BIN_NAME=$(DSP_BIN_NAME).bin -Iout/$(T)
DSP_BIN_NAME ?= hwtest_a7_dsp
$(obj)/../../tests/hwtest/asm_test.o : out/$(T)/$(DSP_BIN_NAME).bin
out/$(T)/$(DSP_BIN_NAME).bin : $(srctree)/out/$(DSP_BIN_NAME)/$(DSP_BIN_NAME).bin
	$(call CMDCPFILE,$(srctree)/out/$(DSP_BIN_NAME)/$(DSP_BIN_NAME).bin,$(srctree)/$@)
	$(call CMDCPFILE,$(srctree)/out/$(DSP_BIN_NAME)/$(DSP_BIN_NAME).elf,$(srctree)/$(@:.bin=.elf))
	$(call CMDCPFILE,$(srctree)/out/$(DSP_BIN_NAME)/$(DSP_BIN_NAME).map,$(srctree)/$(@:.bin=.map))
	$(call CMDCPFILE,$(srctree)/out/$(DSP_BIN_NAME)/$(DSP_BIN_NAME).lst,$(srctree)/$(@:.bin=.lst))
$(srctree)/out/$(DSP_BIN_NAME)/$(DSP_BIN_NAME).bin : FORCE
	$(call echo-help,)
	$(call echo-help,INFO    Please make sure DSP bin is up to date: out/$(DSP_BIN_NAME)/$(DSP_BIN_NAME).bin)
# We cannot build $(DSP_BIN_NAME) automatically here, because the env inherited by sub-make cannot be cleaned:
# E.g., KBUILD_SRC, TC, CHIP_HAS_CP, ...
	$(call echo-help,)
ifeq ($(A7_DSP_TRANSQ_AF), 1)
CFLAGS_a7_dsp_test.o += -DA7_DSP_TRANSQ_AF -Iservices/audioflinger
obj-y += ../../services/audioflinger/
obj-y += ../../platform/drivers/codec/
export AF_DEVICE_INT_CODEC ?= 1
export AF_DEVICE_I2S ?= 1
endif
endif #A7_DSP_TEST

ifeq ($(TRANSQ_TEST),1)
obj-y += ../../tests/hwtest/transq_test.c
ccflags-y += -DTRANSQ_TEST
ifneq ($(RX_ROLE),)
CFLAGS_transq_test.o += -DRX_ROLE
endif
ifneq  ($(TRANSQ_ID),)
CFLAGS_transq_test.o += -DTRANSQ_ID=$(TRANSQ_ID)
endif
endif #TRANSQ_TEST

ifeq ($(PSRAM_TEST),1)
obj-y += ../../tests/hwtest/psram_test.c
ccflags-y += -DPSRAM_TEST -DCHIP_PSRAM_CTRL_VER=$(CHIP_PSRAM_CTRL_VER)
endif #PSRAM_TEST

ifeq ($(PSRAMUHS_TEST),1)
obj-y += ../../tests/hwtest/psram_test.c
ccflags-y += -DPSRAMUHS_TEST
endif #PSRAMUHS_TEST

ifeq ($(WIFI_TEST),1)
obj-y += main_wifi_test.c
ccflags-y += -DWIFI_TEST
endif

ifeq ($(MBW_TEST),1)
obj-y += ../../tests/mbw_test/mbw.c
ccflags-y += -DMBW_TEST
endif



endif # hwtest cases

ifeq ($(RTOS),1)
ifeq ($(NOAPP),1)
obj-y += test-sntp.c
endif
endif
obj-y := $(obj-y:.c=.o)
obj-y := $(obj-y:.cpp=.o)
obj-y := $(obj-y:.S=.o)

obj-y += ../../utils/hwtimer_list/

ccflags-y +=  \
	-Iutils/hwtimer_list \
	-Iplatform/drivers/ana \
	-Iplatform/drivers/security_engine \
	-Iplatform/drivers/usb/usb_dev/inc \
	-Iapps/main \
	-Iapps/factory \
	-Iutils/list  \
	-Iapps/audioplayers  \
	-Iservices/bt_app  \
	-Iapps/common  \
        -Inet/include/\
        -Inet/include/byteorder/\
        -Inet/include/linux/\
        -Inet/include/unaligned/\
	-Inet/\
	-Inet/wpa_supplicant_l/wpa_supplicant/ \
		-Inet/wpa_supplicant_l/src/ \
	-Inet/wpa_supplicant_l/src/utils/  \
	-Inet/net_os/  \
	-Inet/include/os/  \
	-Inet/lwip/src/include/\
	-Inet/lwip/src/include/lwip/\
	-Inet/lwip/src/include/lwip/arch\
	-Inet/lwip/src/include/ipv4/\
	-Inet/lwip/src/include/lwip/apps \
	-Inet/include/linux/\
	-Iservices/nvrecord/\
	-Iservices/norflash_api  \
	 -Iplatform/drivers/wifi/$(CHIP)\
    -Iservices/nv_section/factory_section/ \
	-Iservices/nv_section/log_section

ifneq ($(FLASH_FILL),)
ccflags-y += -DFLASH_FILL=$(FLASH_FILL)
endif

ifneq ($(DEBUG_PORT),)
ccflags-y += -DDEBUG_PORT=$(DEBUG_PORT)
endif

ifneq ($(FLASH_SIZE),)
ccflags-y += -DFLASH_SIZE=$(FLASH_SIZE)
endif

ifeq ($(ROM_UTILS_ON),1)
ccflags-y += -DROM_UTILS_ON
endif

ifeq ($(NO_PMU),1)
ccflags-y += -DNO_PMU
endif

ifeq ($(NO_TIMER),1)
ccflags-y += -DNO_TIMER
endif

ifeq ($(SLEEP_TEST),1)
ccflags-y += -DSLEEP_TEST
endif

ifeq ($(RTOS),1)
ifeq ($(KERNEL),RTX)
ccflags-y += -Irtos/rtx/TARGET_CORTEX_M
endif
endif

ifeq ($(DEBUG_MODE_USB_DOWNLOAD),1)
ccflags-y += -DDEBUG_MODE_USB_DOWNLOAD
endif

ccflags-y += -Iservices/wifi_app/wifi_voice/
