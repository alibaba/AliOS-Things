HOST_OPENOCD := LPC54102
NAME := mcu_lpc54102impl

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION    := 1.0.0
$(NAME)_SUMMARY    := driver & sdk for platform/mcu lpc54102impl

$(NAME)_COMPONENTS += arch_armv7m
$(NAME)_COMPONENTS += newlib_stub rhino netmgr sal

GLOBAL_CFLAGS += -DCORE_M4
GLOBAL_CFLAGS += -fmessage-length=0
GLOBAL_CFLAGS += -fno-builtin -ffunction-sections -fdata-sections -fno-common -std=gnu99 -nostdlib -DSDK_DEBUGCONSOLE=1
#GLOBAL_CFLAGS += $(CPU_CFLAGS) -mlittle-endian
GLOBAL_CFLAGS += -mcpu=cortex-m4 -mlittle-endian -mthumb -mthumb-interwork

GLOBAL_ASMFLAGS += -D__MULTICORE_MASTER -D__MULTICORE_M0SLAVE -DMULTICORE_MASTER_SLAVE_M0SLAVE

GLOBAL_INCLUDES += ./ \
                   CMSIS/Include \
                   drivers \
                   mcuxpresso \
                   utilities \
                   utilities/str \
                   utilities/log \
                   utilities/io

#GLOBAL_LDFLAGS += -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -mlittle-endian -mthumb-interwork -nostartfiles
GLOBAL_LDFLAGS += -mcpu=cortex-m4           \
                  -mthumb -mthumb-interwork \
                  -mlittle-endian           \
                  -nostartfiles             \
                  $(CLIB_LDFLAGS_NANO_FLOAT)

GLOBAL_LDFLAGS += -L$(SOURCE_ROOT)platform/mcu/lpc54102/mcuxpresso
ifeq ($(HOST_ARCH),Cortex-M4F)
GLOBAL_LDFLAGS += -lfsl_power_cm4_hard
else ifeq ($(HOST_ARCH),Cortex-M4)
GLOBAL_LDFLAGS += -lfsl_power_lib_cm4
endif
$(NAME)_CFLAGS  += -Wall -Werror -Wno-unused-variable -Wno-unused-parameter -Wno-implicit-function-declaration
$(NAME)_CFLAGS  += -Wno-type-limits -Wno-sign-compare -Wno-pointer-sign -Wno-uninitialized
$(NAME)_CFLAGS  += -Wno-return-type -Wno-unused-function -Wno-unused-but-set-variable
$(NAME)_CFLAGS  += -Wno-unused-value -Wno-strict-aliasing
GLOBAL_CFLAGS   += -Wno-format -Wno-incompatible-pointer-types
$(NAME)_SOURCES :=

#$(NAME)_SOURCES     += ../../arch/arm/armv7m/gcc/m4/port_c.c
#$(NAME)_SOURCES     += ../../arch/arm/armv7m/gcc/m4/port_s.S

$(NAME)_SOURCES += ./drivers/fsl_adc.c
$(NAME)_SOURCES += ./drivers/fsl_clock.c
$(NAME)_SOURCES += ./drivers/fsl_common.c
$(NAME)_SOURCES += ./drivers/fsl_crc.c
$(NAME)_SOURCES += ./drivers/fsl_ctimer.c
$(NAME)_SOURCES += ./drivers/fsl_dma.c
$(NAME)_SOURCES += ./drivers/fsl_flashiap.c
$(NAME)_SOURCES += ./drivers/fsl_fmeas.c
$(NAME)_SOURCES += ./drivers/fsl_gint.c
$(NAME)_SOURCES += ./drivers/fsl_gpio.c
$(NAME)_SOURCES += ./drivers/fsl_i2c.c
$(NAME)_SOURCES += ./drivers/fsl_i2c_dma.c
$(NAME)_SOURCES += ./drivers/fsl_inputmux.c
$(NAME)_SOURCES += ./drivers/fsl_mrt.c
$(NAME)_SOURCES += ./drivers/fsl_pint.c
$(NAME)_SOURCES += ./drivers/fsl_power.c
$(NAME)_SOURCES += ./drivers/fsl_reset.c
$(NAME)_SOURCES += ./drivers/fsl_rit.c
$(NAME)_SOURCES += ./drivers/fsl_rtc.c
$(NAME)_SOURCES += ./drivers/fsl_sctimer.c
$(NAME)_SOURCES += ./drivers/fsl_spi.c
$(NAME)_SOURCES += ./drivers/fsl_spi_dma.c
$(NAME)_SOURCES += ./drivers/fsl_usart.c
$(NAME)_SOURCES += ./drivers/fsl_usart_dma.c
$(NAME)_SOURCES += ./drivers/fsl_utick.c
$(NAME)_SOURCES += ./drivers/fsl_wwdt.c
$(NAME)_SOURCES += ./system_LPC54102_cm4.c
$(NAME)_SOURCES += ./utilities/log/fsl_log.c
$(NAME)_SOURCES += ./utilities/str/fsl_str.c
$(NAME)_SOURCES += ./utilities/io/fsl_io.c
$(NAME)_SOURCES += ./utilities/fsl_debug_console.c
$(NAME)_SOURCES += ./mcuxpresso/startup_lpc5410x.c
$(NAME)_SOURCES += ./hal/hal_uart.c
$(NAME)_SOURCES += ./hal/hal_gpio.c
$(NAME)_SOURCES += ./hal/hal_i2c.c
$(NAME)_SOURCES += ./hal/hal_spi.c
$(NAME)_SOURCES += ./hal/csp_log.c
$(NAME)_SOURCES += ./hal/hal_flash.c
$(NAME)_SOURCES += ./hal/hal_wifi_wmi.c
$(NAME)_SOURCES += ./aos/aos.c
$(NAME)_SOURCES += ./aos/soc_impl.c
$(NAME)_SOURCES += ./hal/hook_impl.c

GLOBAL_CFLAGS += -DA_LITTLE_ENDIAN

GLOBAL_INCLUDES += ./wifi_qca/common_src/include
GLOBAL_INCLUDES += ./wifi_qca/common_src/hcd
GLOBAL_INCLUDES += ./wifi_qca/common_src/stack_common
GLOBAL_INCLUDES += ./wifi_qca/common_src/wmi
GLOBAL_INCLUDES += ./wifi_qca/custom_src/include
GLOBAL_INCLUDES += ./wifi_qca/custom_src/stack_custom
GLOBAL_INCLUDES += ./wifi_qca/port/boards/lpcxpresso54102/alios/gt202
GLOBAL_INCLUDES += ./wifi_qca/port/boards/lpcxpresso54102/alios
GLOBAL_INCLUDES += ./wifi_qca/port/drivers/spi_alios
GLOBAL_INCLUDES += ./wifi_qca/port/shields
GLOBAL_INCLUDES += ./wifi_qca/port/env/alios
GLOBAL_INCLUDES += ./wifi_qca/port
GLOBAL_INCLUDES += ./wifi_qca/include
GLOBAL_INCLUDES += ./wifi_qca/include/AR6002/hw2.0/hw
GLOBAL_INCLUDES += ./wifi_qca/include/AR6002
GLOBAL_INCLUDES += ./wifi_qca

$(NAME)_SOURCES += ./wifi_qca/common_src/api_interface/api_init.c
$(NAME)_SOURCES += ./wifi_qca/common_src/api_interface/api_ioctl.c
$(NAME)_SOURCES += ./wifi_qca/common_src/api_interface/api_txrx.c
$(NAME)_SOURCES += ./wifi_qca/common_src/api_interface/api_wmi_rx.c
$(NAME)_SOURCES += ./wifi_qca/common_src/bmi/bmi.c
$(NAME)_SOURCES += ./wifi_qca/common_src/driver/driver_diag.c
$(NAME)_SOURCES += ./wifi_qca/common_src/driver/driver_init.c
$(NAME)_SOURCES += ./wifi_qca/common_src/driver/driver_main.c
$(NAME)_SOURCES += ./wifi_qca/common_src/driver/driver_netbuf.c
$(NAME)_SOURCES += ./wifi_qca/common_src/driver/driver_txrx.c
$(NAME)_SOURCES += ./wifi_qca/common_src/htc/htc.c
$(NAME)_SOURCES += ./wifi_qca/common_src/hcd/spi_hcd.c
$(NAME)_SOURCES += ./wifi_qca/common_src/hw_interface/hw_api.c
$(NAME)_SOURCES += ./wifi_qca/common_src/stack_common/api_stack_offload.c
$(NAME)_SOURCES += ./wifi_qca/common_src/qapi/qcom_api.c
$(NAME)_SOURCES += ./wifi_qca/common_src/qapi/qcom_legacy.c
$(NAME)_SOURCES += ./wifi_qca/common_src/reorder/rcv_aggr.c
$(NAME)_SOURCES += ./wifi_qca/common_src/storerecall/dset_api.c
$(NAME)_SOURCES += ./wifi_qca/common_src/storerecall/dset.c
$(NAME)_SOURCES += ./wifi_qca/common_src/storerecall/storerecall.c
$(NAME)_SOURCES += ./wifi_qca/common_src/util/util.c
$(NAME)_SOURCES += ./wifi_qca/common_src/wmi/wmi.c
$(NAME)_SOURCES += ./wifi_qca/custom_src/api_interface/cust_api_init.c
$(NAME)_SOURCES += ./wifi_qca/custom_src/api_interface/cust_api_ioctl.c
$(NAME)_SOURCES += ./wifi_qca/custom_src/api_interface/cust_api_txrx.c
$(NAME)_SOURCES += ./wifi_qca/custom_src/api_interface/cust_api_wmi_rx.c
$(NAME)_SOURCES += ./wifi_qca/custom_src/driver/cust_driver_main.c
$(NAME)_SOURCES += ./wifi_qca/custom_src/driver/cust_driver_netbuf.c
$(NAME)_SOURCES += ./wifi_qca/custom_src/hw_interface/cust_spi_hcd.c
$(NAME)_SOURCES += ./wifi_qca/custom_src/qapi/custom_qcom_api.c
$(NAME)_SOURCES += ./wifi_qca/custom_src/stack_custom/cust_api_stack_offload.c
$(NAME)_SOURCES += ./wifi_qca/custom_src/stack_custom/cust_api_stack_txrx.c

$(NAME)_SOURCES += ./wifi_qca/port/boards/lpcxpresso54102/alios/wifi_shield.c
$(NAME)_SOURCES += ./wifi_qca/port/boards/lpcxpresso54102/alios/wlan_qca400x.c
$(NAME)_SOURCES += ./wifi_qca/port/env/alios/wifi_env.c
$(NAME)_SOURCES += ./wifi_qca/port/drivers/spi_alios/wifi_spi.c
$(NAME)_SOURCES += ./wifi_qca/port/drivers/spi_alios/wifi_driver_main.c
$(NAME)_SOURCES += ./wifi_qca/port/pin_mux.c

ifndef OVERRIDE_LD_FILE
GLOBAL_LDFLAGS += -T platform/mcu/lpc54102/lpc54102_flash_96K.ld
endif
