##
 # Copyright (C) 2017 C-SKY Microsystems Co., Ltd. All rights reserved.
 #
 # Licensed under the Apache License, Version 2.0 (the "License");
 # you may not use this file except in compliance with the License.
 # You may obtain a copy of the License at
 #
 #   http://www.apache.org/licenses/LICENSE-2.0
 #
 # Unless required by applicable law or agreed to in writing, software
 # distributed under the License is distributed on an "AS IS" BASIS,
 # WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 # See the License for the specific language governing permissions and
 # limitations under the License.
##
INCDIR += -I$(CHIPDIR)/include
INCDIR += -I$(DRIVERDIR)/include

ifneq ($(CONFIG_KERNEL_NONE), y)
ifneq ($(CONFIG_HAVE_VIC), y)
DRIVER_CSRC += $(CHIPDIR)/novic_irq_tbl.c
endif
endif

DRIVER_SSRC += $(CHIPDIR)/startup.S
DRIVER_SSRC += $(CHIPDIR)/vectors.S
DRIVER_CSRC += $(CHIPDIR)/system.c
DRIVER_CSRC += $(CHIPDIR)/isr.c
DRIVER_CSRC += $(CHIPDIR)/lib.c
DRIVER_CSRC += $(CHIPDIR)/devices.c
DRIVER_CSRC += $(CHIPDIR)/pinmux.c
DRIVER_CSRC += $(CHIPDIR)/ck_sys_freq.c
DRIVER_CSRC += $(CHIPDIR)/trap_c.c

DRIVER_CSRC += $(DRIVERDIR)/spi/dw_spi.c
DRIVER_CSRC += $(DRIVERDIR)/usart/dw_usart.c
DRIVER_CSRC += $(DRIVERDIR)/eflash/ck_eflash.c
DRIVER_CSRC += $(DRIVERDIR)/timer/dw_timer.c
DRIVER_CSRC += $(DRIVERDIR)/gpio/dw_gpio.c
DRIVER_CSRC += $(DRIVERDIR)/iic/dw_iic.c
DRIVER_CSRC += $(DRIVERDIR)/rtc/ck_rtc.c
DRIVER_CSRC += $(DRIVERDIR)/wdt/dw_wdt.c
DRIVER_CSRC += $(DRIVERDIR)/pwm/ck_pwm.c
DRIVER_CSRC += $(DRIVERDIR)/dmac/dw_dmac.c
DRIVER_CSRC += $(DRIVERDIR)/adc/ck_adc.c
DRIVER_CSRC += $(DRIVERDIR)/sasc/ck_sasc_v1.c
DRIVER_CSRC += $(DRIVERDIR)/trng/ck_trng.c
DRIVER_CSRC += $(DRIVERDIR)/crc/ck_crc.c
DRIVER_CSRC += $(DRIVERDIR)/aes/ck_aes.c
DRIVER_CSRC += $(DRIVERDIR)/rsa/ck_rsa.c
DRIVER_CSRC += $(DRIVERDIR)/sha/ck_sha_v1.c
