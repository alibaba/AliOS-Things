/*
** ###################################################################
**     Version:             rev. 1.2, 2017-06-08
**     Build:               b180327
**
**     Abstract:
**         Chip specific module features.
**
**     The Clear BSD License
**     Copyright 2016 Freescale Semiconductor, Inc.
**     Copyright 2016-2018 NXP
**     All rights reserved.
**
**     Redistribution and use in source and binary forms, with or without
**     modification, are permitted (subject to the limitations in the
**     disclaimer below) provided that the following conditions are met:
**
**     * Redistributions of source code must retain the above copyright
**       notice, this list of conditions and the following disclaimer.
**
**     * Redistributions in binary form must reproduce the above copyright
**       notice, this list of conditions and the following disclaimer in the
**       documentation and/or other materials provided with the distribution.
**
**     * Neither the name of the copyright holder nor the names of its
**       contributors may be used to endorse or promote products derived from
**       this software without specific prior written permission.
**
**     NO EXPRESS OR IMPLIED LICENSES TO ANY PARTY'S PATENT RIGHTS ARE
**     GRANTED BY THIS LICENSE. THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT
**     HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED
**     WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
**     MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
**     DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
**     LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
**     CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
**     SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
**     BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
**     WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
**     OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
**     IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
**
**     http:                 www.nxp.com
**     mail:                 support@nxp.com
**
**     Revisions:
**     - rev. 1.0 (2016-08-12)
**         Initial version.
**     - rev. 1.1 (2016-11-25)
**         Update CANFD and Classic CAN register.
**         Add MAC TIMERSTAMP registers.
**     - rev. 1.2 (2017-06-08)
**         Remove RTC_CTRL_RTC_OSC_BYPASS.
**         SYSCON_ARMTRCLKDIV rename to SYSCON_ARMTRACECLKDIV.
**         Remove RESET and HALT from SYSCON_AHBCLKDIV.
**
** ###################################################################
*/

#ifndef _LPC54018_FEATURES_H_
#define _LPC54018_FEATURES_H_

/* SOC module features */

/* @brief ADC availability on the SoC. */
#define FSL_FEATURE_SOC_ADC_COUNT (1)
/* @brief ASYNC_SYSCON availability on the SoC. */
#define FSL_FEATURE_SOC_ASYNC_SYSCON_COUNT (1)
/* @brief CAN availability on the SoC. */
#define FSL_FEATURE_SOC_LPC_CAN_COUNT (2)
/* @brief CRC availability on the SoC. */
#define FSL_FEATURE_SOC_CRC_COUNT (1)
/* @brief CTIMER availability on the SoC. */
#define FSL_FEATURE_SOC_CTIMER_COUNT (5)
/* @brief DMA availability on the SoC. */
#define FSL_FEATURE_SOC_DMA_COUNT (1)
/* @brief DMIC availability on the SoC. */
#define FSL_FEATURE_SOC_DMIC_COUNT (1)
/* @brief EMC availability on the SoC. */
#define FSL_FEATURE_SOC_EMC_COUNT (1)
/* @brief ENET availability on the SoC. */
#define FSL_FEATURE_SOC_LPC_ENET_COUNT (1)
/* @brief FLEXCOMM availability on the SoC. */
#define FSL_FEATURE_SOC_FLEXCOMM_COUNT (11)
/* @brief GINT availability on the SoC. */
#define FSL_FEATURE_SOC_GINT_COUNT (2)
/* @brief GPIO availability on the SoC. */
#define FSL_FEATURE_SOC_GPIO_COUNT (1)
/* @brief I2C availability on the SoC. */
#define FSL_FEATURE_SOC_I2C_COUNT (10)
/* @brief I2S availability on the SoC. */
#define FSL_FEATURE_SOC_I2S_COUNT (2)
/* @brief INPUTMUX availability on the SoC. */
#define FSL_FEATURE_SOC_INPUTMUX_COUNT (1)
/* @brief IOCON availability on the SoC. */
#define FSL_FEATURE_SOC_IOCON_COUNT (1)
/* @brief LCD availability on the SoC. */
#define FSL_FEATURE_SOC_LCD_COUNT (1)
/* @brief MRT availability on the SoC. */
#define FSL_FEATURE_SOC_MRT_COUNT (1)
/* @brief PINT availability on the SoC. */
#define FSL_FEATURE_SOC_PINT_COUNT (1)
/* @brief RIT availability on the SoC. */
#define FSL_FEATURE_SOC_RIT_COUNT (1)
/* @brief RNG availability on the SoC. */
#define FSL_FEATURE_SOC_LPC_RNG_COUNT (1)
/* @brief RTC availability on the SoC. */
#define FSL_FEATURE_SOC_RTC_COUNT (1)
/* @brief SCT availability on the SoC. */
#define FSL_FEATURE_SOC_SCT_COUNT (1)
/* @brief SDIF availability on the SoC. */
#define FSL_FEATURE_SOC_SDIF_COUNT (1)
/* @brief SHA availability on the SoC. */
#define FSL_FEATURE_SOC_SHA_COUNT (1)
/* @brief SMARTCARD availability on the SoC. */
#define FSL_FEATURE_SOC_SMARTCARD_COUNT (2)
/* @brief SPI availability on the SoC. */
#define FSL_FEATURE_SOC_SPI_COUNT (11)
/* @brief SPIFI availability on the SoC. */
#define FSL_FEATURE_SOC_SPIFI_COUNT (1)
/* @brief SYSCON availability on the SoC. */
#define FSL_FEATURE_SOC_SYSCON_COUNT (1)
/* @brief USART availability on the SoC. */
#define FSL_FEATURE_SOC_USART_COUNT (10)
/* @brief USB availability on the SoC. */
#define FSL_FEATURE_SOC_USB_COUNT (1)
/* @brief USBFSH availability on the SoC. */
#define FSL_FEATURE_SOC_USBFSH_COUNT (1)
/* @brief USBHSD availability on the SoC. */
#define FSL_FEATURE_SOC_USBHSD_COUNT (1)
/* @brief USBHSH availability on the SoC. */
#define FSL_FEATURE_SOC_USBHSH_COUNT (1)
/* @brief UTICK availability on the SoC. */
#define FSL_FEATURE_SOC_UTICK_COUNT (1)
/* @brief WWDT availability on the SoC. */
#define FSL_FEATURE_SOC_WWDT_COUNT (1)

/* ADC module features */

/* @brief Has Calibration register. */
#define FSL_FEATURE_ADC_HAS_CALIB_REG (1)

/* CAN module features */

/* @brief Support CANFD or not */
#define FSL_FEATURE_CAN_SUPPORT_CANFD (1)

/* DMA module features */

/* @brief Number of channels */
#define FSL_FEATURE_DMA_NUMBER_OF_CHANNELS (30)

/* FLEXCOMM module features */

/* @brief FLEXCOMM0 USART INDEX 0 */
#define FSL_FEATURE_FLEXCOMM0_USART_INDEX  (0)
/* @brief FLEXCOMM0 SPI INDEX 0 */
#define FSL_FEATURE_FLEXCOMM0_SPI_INDEX  (0)
/* @brief FLEXCOMM0 I2C INDEX 0 */
#define FSL_FEATURE_FLEXCOMM0_I2C_INDEX  (0)
/* @brief FLEXCOMM1 USART INDEX 1 */
#define FSL_FEATURE_FLEXCOMM1_USART_INDEX  (1)
/* @brief FLEXCOMM1 SPI INDEX 1 */
#define FSL_FEATURE_FLEXCOMM1_SPI_INDEX  (1)
/* @brief FLEXCOMM1 I2C INDEX 1 */
#define FSL_FEATURE_FLEXCOMM1_I2C_INDEX  (1)
/* @brief FLEXCOMM2 USART INDEX 2 */
#define FSL_FEATURE_FLEXCOMM2_USART_INDEX  (2)
/* @brief FLEXCOMM2 SPI INDEX 2 */
#define FSL_FEATURE_FLEXCOMM2_SPI_INDEX  (2)
/* @brief FLEXCOMM2 I2C INDEX 2 */
#define FSL_FEATURE_FLEXCOMM2_I2C_INDEX  (2)
/* @brief FLEXCOMM3 USART INDEX 3 */
#define FSL_FEATURE_FLEXCOMM3_USART_INDEX  (3)
/* @brief FLEXCOMM3 SPI INDEX 3 */
#define FSL_FEATURE_FLEXCOMM3_SPI_INDEX  (3)
/* @brief FLEXCOMM3 I2C INDEX 3 */
#define FSL_FEATURE_FLEXCOMM3_I2C_INDEX  (3)
/* @brief FLEXCOMM4 USART INDEX 4 */
#define FSL_FEATURE_FLEXCOMM4_USART_INDEX  (4)
/* @brief FLEXCOMM4 SPI INDEX 4 */
#define FSL_FEATURE_FLEXCOMM4_SPI_INDEX  (4)
/* @brief FLEXCOMM4 I2C INDEX 4 */
#define FSL_FEATURE_FLEXCOMM4_I2C_INDEX  (4)
/* @brief FLEXCOMM5 USART INDEX 5 */
#define FSL_FEATURE_FLEXCOMM5_USART_INDEX  (5)
/* @brief FLEXCOMM5 SPI INDEX 5 */
#define FSL_FEATURE_FLEXCOMM5_SPI_INDEX  (5)
/* @brief FLEXCOMM5 I2C INDEX 5 */
#define FSL_FEATURE_FLEXCOMM5_I2C_INDEX  (5)
/* @brief FLEXCOMM6 USART INDEX 6 */
#define FSL_FEATURE_FLEXCOMM6_USART_INDEX  (6)
/* @brief FLEXCOMM6 SPI INDEX 6 */
#define FSL_FEATURE_FLEXCOMM6_SPI_INDEX  (6)
/* @brief FLEXCOMM6 I2C INDEX 6 */
#define FSL_FEATURE_FLEXCOMM6_I2C_INDEX  (6)
/* @brief FLEXCOMM7 I2S INDEX 0 */
#define FSL_FEATURE_FLEXCOMM6_I2S_INDEX  (0)
/* @brief FLEXCOMM7 USART INDEX 7 */
#define FSL_FEATURE_FLEXCOMM7_USART_INDEX  (7)
/* @brief FLEXCOMM7 SPI INDEX 7 */
#define FSL_FEATURE_FLEXCOMM7_SPI_INDEX  (7)
/* @brief FLEXCOMM7 I2C INDEX 7 */
#define FSL_FEATURE_FLEXCOMM7_I2C_INDEX  (7)
/* @brief FLEXCOMM7 I2S INDEX 1 */
#define FSL_FEATURE_FLEXCOMM7_I2S_INDEX  (1)
/* @brief FLEXCOMM4 USART INDEX 8 */
#define FSL_FEATURE_FLEXCOMM8_USART_INDEX  (8)
/* @brief FLEXCOMM4 SPI INDEX 8 */
#define FSL_FEATURE_FLEXCOMM8_SPI_INDEX  (8)
/* @brief FLEXCOMM4 I2C INDEX 8 */
#define FSL_FEATURE_FLEXCOMM8_I2C_INDEX  (8)
/* @brief FLEXCOMM5 USART INDEX 9 */
#define FSL_FEATURE_FLEXCOMM9_USART_INDEX  (9)
/* @brief FLEXCOMM5 SPI INDEX 9 */
#define FSL_FEATURE_FLEXCOMM9_SPI_INDEX  (9)
/* @brief FLEXCOMM5 I2C INDEX 9 */
#define FSL_FEATURE_FLEXCOMM9_I2C_INDEX  (9)

/* IOCON module features */

/* @brief Func bit field width */
#define FSL_FEATURE_IOCON_FUNC_FIELD_WIDTH (4)

/* MRT module features */

/* @brief number of channels. */
#define FSL_FEATURE_MRT_NUMBER_OF_CHANNELS  (4)

/* interrupt module features */

/* @brief Lowest interrupt request number. */
#define FSL_FEATURE_INTERRUPT_IRQ_MIN (-14)
/* @brief Highest interrupt request number. */
#define FSL_FEATURE_INTERRUPT_IRQ_MAX (105)

/* PINT module features */

/* @brief Number of connected outputs */
#define FSL_FEATURE_PINT_NUMBER_OF_CONNECTED_OUTPUTS (8)

/* SCT module features */

/* @brief Number of events */
#define FSL_FEATURE_SCT_NUMBER_OF_EVENTS (10)
/* @brief Number of states */
#define FSL_FEATURE_SCT_NUMBER_OF_STATES (10)
/* @brief Number of match capture */
#define FSL_FEATURE_SCT_NUMBER_OF_MATCH_CAPTURE (10)
/* @brief Number of outputs */
#define FSL_FEATURE_SCT_NUMBER_OF_OUTPUTS (10)

/* SDIF module features */

/* @brief FIFO depth, every location is a WORD */
#define FSL_FEATURE_SDIF_FIFO_DEPTH_64_32BITS  (64)
/* @brief Max DMA buffer size */
#define FSL_FEATURE_SDIF_INTERNAL_DMA_MAX_BUFFER_SIZE  (4096)
/* @brief Max source clock in HZ */
#define FSL_FEATURE_SDIF_MAX_SOURCE_CLOCK  (52000000)

/* SPIFI module features */

/* @brief SPIFI start address */
#define FSL_FEATURE_SPIFI_START_ADDR (0x10000000)
/* @brief SPIFI end address */
#define FSL_FEATURE_SPIFI_END_ADDR (0x17FFFFFF)

/* SYSCON module features */

/* @brief Pointer to ROM IAP entry functions */
#define FSL_FEATURE_SYSCON_IAP_ENTRY_LOCATION (0x03000205)

/* SysTick module features */

/* @brief Systick has external reference clock. */
#define FSL_FEATURE_SYSTICK_HAS_EXT_REF (0)
/* @brief Systick external reference clock is core clock divided by this value. */
#define FSL_FEATURE_SYSTICK_EXT_REF_CORE_DIV (0)

/* USB module features */

/* @brief Size of the USB dedicated RAM */
#define FSL_FEATURE_USB_USB_RAM (0x00002000)
/* @brief Base address of the USB dedicated RAM */
#define FSL_FEATURE_USB_USB_RAM_BASE_ADDRESS (0x40100000)
/* @brief USB version */
#define FSL_FEATURE_USB_VERSION (200)
/* @brief Number of the endpoint in USB FS */
#define FSL_FEATURE_USB_EP_NUM (5)

/* USBFSH module features */

/* @brief Size of the USB dedicated RAM */
#define FSL_FEATURE_USBFSH_USB_RAM (0x00002000)
/* @brief Base address of the USB dedicated RAM */
#define FSL_FEATURE_USBFSH_USB_RAM_BASE_ADDRESS (0x40100000)
/* @brief USBFSH version */
#define FSL_FEATURE_USBFSH_VERSION (200)

/* USBHSD module features */

/* @brief Size of the USB dedicated RAM */
#define FSL_FEATURE_USBHSD_USB_RAM (0x00002000)
/* @brief Base address of the USB dedicated RAM */
#define FSL_FEATURE_USBHSD_USB_RAM_BASE_ADDRESS (0x40100000)
/* @brief USBHSD version */
#define FSL_FEATURE_USBHSD_VERSION (300)
/* @brief Number of the endpoint in USB HS */
#define FSL_FEATURE_USBHSD_EP_NUM (6)

/* USBHSH module features */

/* @brief Size of the USB dedicated RAM */
#define FSL_FEATURE_USBHSH_USB_RAM (0x00002000)
/* @brief Base address of the USB dedicated RAM */
#define FSL_FEATURE_USBHSH_USB_RAM_BASE_ADDRESS (0x40100000)
/* @brief USBHSH version */
#define FSL_FEATURE_USBHSH_VERSION (300)

#endif /* _LPC54018_FEATURES_H_ */

