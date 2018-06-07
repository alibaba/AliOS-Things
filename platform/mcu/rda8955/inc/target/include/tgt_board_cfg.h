/* Copyright (C) 2016 RDA Technologies Limited and/or its affiliates("RDA").
* All rights reserved.
*
* This software is supplied "AS IS" without any warranties.
* RDA assumes no responsibility or liability for the use of the software,
* conveys no license or title under any patent, copyright, or mask work
* right to the product. RDA reserves the right to make changes in the
* software without notification.  RDA also make no representation or
* warranty that such application will be suitable for the specified use
* without further testing or modification.
*/


#ifndef _TGT_BOARD_CFG_H_
#define _TGT_BOARD_CFG_H_



// #############################################################################
// #                                                                           #
// #                      CHIP AND MANDATORY DRIVERS                           #
// #                                                                           #
// #############################################################################




// =============================================================================
// TGT_HAL_CONFIG
// =============================================================================
#ifndef TGT_HAL_CONFIG

// =============================================================================
// GPIO Assignments
// -----------------------------------------------------------------------------
// Only the free GPIOs must be used
// Beware of compatibility issues
// ()
// This description is target specific, each GPIO can be assigned
// one of the following values:
/// pin used on it's alternative function
#define AS_ALT_FUNC     0
/// pin used as it's GPIO function
#define AS_GPIO         1
/// GPIO not connected, set to driving 0 for low power
#define NOT_CONNECTED   2
// -----------------------------------------------------------------------------
// GPIO(0)  //  UART1_RXD
#define TGT_HAL_GPIO_0_USED AS_GPIO
// GPIO(1)  //  UART1_TXD
#define TGT_HAL_GPIO_1_USED AS_GPIO
// GPIO(2)  //  UART1_CTS
#define TGT_HAL_GPIO_2_USED AS_GPIO
// GPIO(3)  //  UART1_RTS
#define TGT_HAL_GPIO_3_USED AS_GPIO
// GPIO(4)  // UART2_RXD
#define TGT_HAL_GPIO_4_USED AS_ALT_FUNC
// GPIO(5)  // UART2_TXD
#define TGT_HAL_GPIO_5_USED AS_ALT_FUNC
// GPIO(6)  // UART2_CTS
#define TGT_HAL_GPIO_6_USED AS_ALT_FUNC
// GPIO(7)  // UART2_RTS
#define TGT_HAL_GPIO_7_USED AS_ALT_FUNC
// GPIO(8)  // SSD_CLK
#define TGT_HAL_GPIO_8_USED AS_ALT_FUNC
// GPIO(9)  // SSD_CMD
#define TGT_HAL_GPIO_9_USED AS_ALT_FUNC
// GPIO(10) // SDAT_0
#define TGT_HAL_GPIO_10_USED AS_ALT_FUNC
// GPIO(11) // SDAT_1
#define TGT_HAL_GPIO_11_USED AS_ALT_FUNC
// GPIO(12) // SDAT_2
#define TGT_HAL_GPIO_12_USED AS_ALT_FUNC
// GPIO(13) // SDAT_3
#define TGT_HAL_GPIO_13_USED AS_ALT_FUNC
// GPIO(14) // LCD_RSTB
#define TGT_HAL_GPIO_14_USED AS_ALT_FUNC
// GPIO(15) // SPI_LCD_CS
#define TGT_HAL_GPIO_15_USED AS_ALT_FUNC
// GPIO(16) // SPI_LCD_SCK
#define TGT_HAL_GPIO_16_USED AS_ALT_FUNC
// GPIO(17) // SPI_LCD_DIO
#define TGT_HAL_GPIO_17_USED AS_ALT_FUNC
// GPIO(18) // SPI_LCD_SDC
#define TGT_HAL_GPIO_18_USED AS_ALT_FUNC
// GPIO(19) // CAM_PWDN
#define TGT_HAL_GPIO_19_USED AS_ALT_FUNC
// GPIO(20) // CAM_RSTB
#define TGT_HAL_GPIO_20_USED AS_ALT_FUNC
// GPIO(21) // CAM_CLK
#define TGT_HAL_GPIO_21_USED AS_ALT_FUNC
// GPIO(22) // SPI_CAM_SCK
#define TGT_HAL_GPIO_22_USED AS_ALT_FUNC
// GPIO(23) // SPI_CAM_DI_0
#define TGT_HAL_GPIO_23_USED AS_ALT_FUNC
// GPIO(24) //SPI_CAM_DI_1
#define TGT_HAL_GPIO_24_USED AS_ALT_FUNC
// GPIO(25) // KEYIN_0
#define TGT_HAL_GPIO_25_USED AS_ALT_FUNC
// GPIO(26) // KEYIN_1
#define TGT_HAL_GPIO_26_USED AS_ALT_FUNC
// GPIO(27) // KEYIN_2
#define TGT_HAL_GPIO_27_USED AS_ALT_FUNC
// GPIO(28) // KEYIN_3
#define TGT_HAL_GPIO_28_USED AS_ALT_FUNC
// GPIO(29) // KEYIN_4
#define TGT_HAL_GPIO_29_USED AS_ALT_FUNC
// GPIO(30) // KEYOUT_0
#define TGT_HAL_GPIO_30_USED AS_ALT_FUNC
// GPIO(31) // KEYOUT_1
#define TGT_HAL_GPIO_31_USED AS_ALT_FUNC


// GPIO(32) // KEYOUT_2
#define TGT_HAL_GPIO_32_USED AS_ALT_FUNC
// GPIO(33) // KEYOUT_3
#define TGT_HAL_GPIO_33_USED AS_ALT_FUNC
// GPIO(34) // KEYOUT_4
#define TGT_HAL_GPIO_34_USED AS_ALT_FUNC
// GPIO(35) // HST_RXD
#define TGT_HAL_GPIO_35_USED AS_ALT_FUNC
// GPIO(36) // HST_TXD
#define TGT_HAL_GPIO_36_USED AS_ALT_FUNC
// =============================================================================
// Chip version
// =============================================================================
#define TGT_HAL_CHIP_VERSION 1


// =============================================================================
// RF CLK FREQUENCY
// =============================================================================
#define TGT_HAL_RF_CLK_FREQ     HAL_SYS_FREQ_26M

// =============================================================================
// TGT_HAL_CAM_CFG
// -----------------------------------------------------------------------------
// This fills the structure HAL_CFG_CAM_T
// =============================================================================
#define TGT_HAL_CAM_CFG                                                 \
{                                                                       \
    .camUsed            = TRUE,                                         \
    .camRstActiveH      = FALSE,                                        \
    .camPdnActiveH      = TRUE,                                         \
    .camPdnRemap        = { .gpioId  = HAL_GPIO_0},                  \
    .camRstRemap        = { .gpioId  = HAL_GPIO_NONE},                  \
    .cam1Used           = FALSE,                                        \
    .cam1RstActiveH     = FALSE,                                        \
    .cam1PdnActiveH     = TRUE,                                         \
    .cam1PdnRemap       = { .gpioId  = HAL_GPIO_NONE},                  \
    .cam1RstRemap       = { .gpioId  = HAL_GPIO_NONE},                  \
    .camSerialIfMode    = FALSE,                                        \
    .camGpio1En         = FALSE,                                        \
    .camGpio2En         = FALSE,                                        \
    .camGpio3En         = FALSE,                                        \
}

// =============================================================================
// TGT_HAL_PWM_CFG
// -----------------------------------------------------------------------------
/// This structure describes the PWM configuration for a given target.
/// The first field identify which PWL is used for GLOW (if any).
/// The lasts fields tell wether the pin corresponding to PWM output
/// is actually used as PWM output and not as something else (for
/// instance as a GPIO).
// =============================================================================
#define TGT_HAL_PWM_CFG                                                 \
{                                                                       \
    .pwlGlowPosition    = HAL_PWL_NONE,                                 \
    .pwl0Used           = TRUE,                                         \
    .pwl1Used           = FALSE,                                        \
    .pwtUsed            = FALSE,                                        \
    .lpgUsed            = FALSE                                         \
}

// =============================================================================
// HAL_CFG_I2C_T
// -----------------------------------------------------------------------------
/// This structure describes the I2C configuration for a given target. The
/// fields tell wether the corresponding I2C pins are actually used
/// for I2C and not as something else (for instance as a GPIO).
// =============================================================================
#define TGT_HAL_I2C_CFG                                                 \
{                                                                       \
    .i2cUsed             = FALSE,                                       \
    .i2cBps              = HAL_I2C_BPS_100K,                            \
    .i2c2Used            = TRUE,                                        \
    .i2c2Bps             = HAL_I2C_BPS_100K,                            \
    .i2c3Used            = TRUE,                                       \
    .i2c3Bps             = HAL_I2C_BPS_100K,                            \
}

// =============================================================================
// TGT_HAL_I2S_CFG
// -----------------------------------------------------------------------------
/// This structure describes the I2S configuration for a given target. The
/// fields tell wether the corresponding I2S pin is actually used
/// for I2S and not as something else (for instance as a GPIO).
// =============================================================================
#define TGT_HAL_I2S_CFG                                                 \
{                                                                       \
    .doUsed             = TRUE,                                         \
    .di0Used            = TRUE,                                         \
    .di1Used            = TRUE,                                         \
    .di2Used            = FALSE,                                        \
}

// =============================================================================
// TGT_HAL_EBC_CFG
// -----------------------------------------------------------------------------
/// This structure describes the EBC configuration for a given target. The first
/// fields tell wether the pin corresponding to chip select is actually used
/// as a chip select and not as something else (for instance as a GPIO).
// =============================================================================
#define TGT_HAL_EBC_CFG                                                 \
{                                                                       \
    .cs2Used            = FALSE,                                        \
    .cs3Used            = FALSE,                                        \
    .cs4Used            = FALSE                                         \
}

// =============================================================================
// TGT_HAL_UART_CFG
// -----------------------------------------------------------------------------
/// Used to describes a configuration for used pin by an UART for a given target.
// =============================================================================
#define TGT_HAL_UART_CFG                                                \
{                                                                       \
    HAL_UART_CONFIG_NONE,                                        \
    HAL_UART_CONFIG_FLOWCONTROL\
}

// =============================================================================
// TGT_HAL_SPI_CFG
// -----------------------------------------------------------------------------
/// This structure describes the SPI configuration for a given target. The first
/// fields tell wether the pin corresponding to chip select is actually used
/// as a chip select and not as something else (for instance as a GPIO).
/// Then, the polarity of the Chip Select is given. It is only relevant
/// if the corresponding Chip Select is used as a Chip Select.
/// Finally which pin is used as input, Can be none, one or the other.
/// On most chip configuration the input 0 (di0) is on the output pin: SPI_DIO
// =============================================================================
#define TGT_HAL_SPI_CFG                                                 \
    {                                                                   \
        {                                                               \
        .cs0Used        = FALSE,                                        \
        .cs1Used        = FALSE,                                        \
        .cs2Used        = FALSE,                                        \
        .cs3Used        = FALSE,                                        \
        .cs0ActiveLow   = FALSE,                                        \
        .cs1ActiveLow   = FALSE,                                        \
        .cs2ActiveLow   = FALSE,                                        \
        .cs3ActiveLow   = FALSE,                                        \
        .di0Used        = FALSE,                                        \
        .di1Used        = FALSE                                         \
    },                                                                  \
    {                                                                   \
        .cs0Used        = FALSE,                                        \
        .cs1Used        = FALSE,                                        \
        .cs2Used        = FALSE,                                        \
        .cs3Used        = FALSE,                                        \
        .cs0ActiveLow   = FALSE,                                        \
        .cs1ActiveLow   = FALSE,                                        \
        .cs2ActiveLow   = FALSE,                                        \
        .cs3ActiveLow   = FALSE,                                        \
        .di0Used        = FALSE,                                        \
        .di1Used        = FALSE                                         \
        }                                                               \
    }

// =============================================================================
// TGT_HAL_GOUDA_CFG
// -----------------------------------------------------------------------------
/// This structure describes the GOUDA configuration for a given target.
/// The first fields tell wether the pin corresponding to chip select is
/// actually used as a chip select and not as something else (for instance
/// as a GPIO). If none are used, the GOUDA is considered unused.
// =============================================================================
#define TGT_HAL_GOUDA_CFG                                               \
{                                                                       \
    .cs0Used            = TRUE,                                         \
    .cs1Used            = FALSE,                                        \
    .lcdData16Bit       = FALSE,                                        \
    .lcdResetActiveLow  = TRUE,                                         \
    .lcdResetPin        = { .gpoId = HAL_GPIO_NONE},                       \
}


// =============================================================================
//  GPIO pins that can be GPIO but are not connected on the board
//  any pin described here will be driven low all the time for power optimization
//  It's actually computed from the TGT_HAL_GPIO_xx_USED macros above.
//  DO NOT MODIFY !
// =============================================================================
#define TGT_HAL_NO_CONNECT_GPIO         ( \
    ((TGT_HAL_GPIO_0_USED & 2) >> 1)    | \
    (TGT_HAL_GPIO_1_USED & 2)           | \
    ((TGT_HAL_GPIO_2_USED & 2) << 1)    | \
    ((TGT_HAL_GPIO_3_USED & 2) << 2)    | \
    ((TGT_HAL_GPIO_4_USED & 2) << 3)    | \
    ((TGT_HAL_GPIO_5_USED & 2) << 4)    | \
    ((TGT_HAL_GPIO_6_USED & 2) << 5)    | \
    ((TGT_HAL_GPIO_7_USED & 2) << 6)    | \
    ((TGT_HAL_GPIO_8_USED & 2) << 7)    | \
    ((TGT_HAL_GPIO_9_USED & 2) << 8)    | \
    ((TGT_HAL_GPIO_10_USED & 2) << 9)   | \
    ((TGT_HAL_GPIO_11_USED & 2) << 10)  | \
    ((TGT_HAL_GPIO_12_USED & 2) << 11)  | \
    ((TGT_HAL_GPIO_13_USED & 2) << 12)  | \
    ((TGT_HAL_GPIO_14_USED & 2) << 13)  | \
    ((TGT_HAL_GPIO_15_USED & 2) << 14)  | \
    ((TGT_HAL_GPIO_16_USED & 2) << 15)  | \
    ((TGT_HAL_GPIO_17_USED & 2) << 16)  | \
    ((TGT_HAL_GPIO_18_USED & 2) << 17)  | \
    ((TGT_HAL_GPIO_19_USED & 2) << 18)  | \
    ((TGT_HAL_GPIO_20_USED & 2) << 19)  | \
    ((TGT_HAL_GPIO_21_USED & 2) << 20)  | \
    ((TGT_HAL_GPIO_22_USED & 2) << 21)  | \
    ((TGT_HAL_GPIO_23_USED & 2) << 22)  | \
    ((TGT_HAL_GPIO_24_USED & 2) << 23)  | \
    ((TGT_HAL_GPIO_25_USED & 2) << 24)  | \
    ((TGT_HAL_GPIO_26_USED & 2) << 25)  | \
    ((TGT_HAL_GPIO_27_USED & 2) << 26)  | \
    ((TGT_HAL_GPIO_28_USED & 2) << 27)  | \
    ((TGT_HAL_GPIO_29_USED & 2) << 28)  | \
    ((TGT_HAL_GPIO_30_USED & 2) << 29)  | \
    ((TGT_HAL_GPIO_31_USED & 2) << 30)  )


// =============================================================================
//  GPIO pins that are actually used as GPIO
//  It's actually computed from the TGT_HAL_GPIO_xx_USED macros above.
//  DO NOT MODIFY !
// =============================================================================
#define TGT_HAL_USED_GPIO               ( \
    (TGT_HAL_GPIO_0_USED & 1)           | \
    ((TGT_HAL_GPIO_1_USED & 1) << 1)    | \
    ((TGT_HAL_GPIO_2_USED & 1) << 2)    | \
    ((TGT_HAL_GPIO_3_USED & 1) << 3)    | \
    ((TGT_HAL_GPIO_4_USED & 1) << 4)    | \
    ((TGT_HAL_GPIO_5_USED & 1) << 5)    | \
    ((TGT_HAL_GPIO_6_USED & 1) << 6)    | \
    ((TGT_HAL_GPIO_7_USED & 1) << 7)    | \
    ((TGT_HAL_GPIO_8_USED & 1) << 8)    | \
    ((TGT_HAL_GPIO_9_USED & 1) << 9)    | \
    ((TGT_HAL_GPIO_10_USED & 1) << 10)  | \
    ((TGT_HAL_GPIO_11_USED & 1) << 11)  | \
    ((TGT_HAL_GPIO_12_USED & 1) << 12)  | \
    ((TGT_HAL_GPIO_13_USED & 1) << 13)  | \
    ((TGT_HAL_GPIO_14_USED & 1) << 14)  | \
    ((TGT_HAL_GPIO_15_USED & 1) << 15)  | \
    ((TGT_HAL_GPIO_16_USED & 1) << 16)  | \
    ((TGT_HAL_GPIO_17_USED & 1) << 17)  | \
    ((TGT_HAL_GPIO_18_USED & 1) << 18)  | \
    ((TGT_HAL_GPIO_19_USED & 1) << 19)  | \
    ((TGT_HAL_GPIO_20_USED & 1) << 20)  | \
    ((TGT_HAL_GPIO_21_USED & 1) << 21)  | \
    ((TGT_HAL_GPIO_22_USED & 1) << 22)  | \
    ((TGT_HAL_GPIO_23_USED & 1) << 23)  | \
    ((TGT_HAL_GPIO_24_USED & 1) << 24)  | \
    ((TGT_HAL_GPIO_25_USED & 1) << 25)  | \
    ((TGT_HAL_GPIO_26_USED & 1) << 26)  | \
    ((TGT_HAL_GPIO_27_USED & 1) << 27)  | \
    ((TGT_HAL_GPIO_28_USED & 1) << 28)  | \
    ((TGT_HAL_GPIO_29_USED & 1) << 29)  | \
    ((TGT_HAL_GPIO_30_USED & 1) << 30)  | \
    ((TGT_HAL_GPIO_31_USED & 1) << 31)  )

// -----------------------------------------------------------------------------
/// This structure describes the IO Drive configuration for a given target.
// =============================================================================
#define TGT_HAL_IO_DRIVE                                                \
{                                                                       \
    {                                                                   \
        .vMem0Domain    = 0,                                            \
        .vMem1Domain    = 0,                                            \
        .vMemSpiDomain  = 0,                                            \
        .vLcdDomain     = 1,                                            \
        .vCamDomain     = 3,                                            \
        .vPadDomain     = 3,                                            \
        .keyInKeyOut    = 3,                                            \
        .vSdmmcDomain   = 0,                                            \
    }                                                                   \
}
// =============================================================================
// TGT_HAL_CONFIG
// =============================================================================
#define TGT_HAL_CONFIG                                                  \
{                                                                       \
    .chipVersion        = TGT_HAL_CHIP_VERSION,                         \
    .rfClkFreq          = TGT_HAL_RF_CLK_FREQ,                          \
    .useLpsCo1          = FALSE,                                        \
    .keyInMask          = 0x1f,                                         \
    .keyOutMask         = 0x1f,                                         \
    .pwmCfg             = TGT_HAL_PWM_CFG,                              \
    .useUsbBackup       = FALSE,                                        \
    .useClk32k          = FALSE,                                        \
    .i2cCfg             = TGT_HAL_I2C_CFG,                              \
    .i2c2UseCamPins     = TRUE,                                        \
    .i2sCfg             = TGT_HAL_I2S_CFG,                              \
    .ebcCfg             = TGT_HAL_EBC_CFG,                              \
    .uartCfg            = TGT_HAL_UART_CFG,                             \
    .spiCfg             = TGT_HAL_SPI_CFG,                              \
    .useSdmmc           = TRUE,                                         \
    .useSdmmc2          = HAL_CFG_SDIO2_NONE,                           \
    .spi2UseCamPins     = FALSE,                                        \
    .camCfg             = TGT_HAL_CAM_CFG,                              \
    .goudaCfg           = TGT_HAL_GOUDA_CFG,                            \
    .noConnectGpio      = TGT_HAL_NO_CONNECT_GPIO,                      \
    .usedGpio           = TGT_HAL_USED_GPIO,                            \
    .usedGpo            = 0,                             \
    .usedTco            = 0,                             \
    .ioDrive            = TGT_HAL_IO_DRIVE                              \
}

#endif // TGT_HAL_CONFIG


// =============================================================================
// KEY Mapping
// =============================================================================
#ifndef KEY_MAP

#define KEY_ROW_NUM 8
#define KEY_COL_NUM 8

#define TGT_KEY_NB (KEY_ROW_NUM * KEY_COL_NUM)

#define KEY_MAP                                                           \
{                                                                         \
    KP_STAR, KP_7, KP_4, KP_1, KP_UP , KP_UNMAPPED, KP_UNMAPPED, KP_UNMAPPED, \
    KP_0, KP_8, KP_5, KP_2, KP_DW, KP_UNMAPPED, KP_UNMAPPED, KP_UNMAPPED, \
    KP_POUND,KP_9, KP_6, KP_3, KP_RT , KP_UNMAPPED, KP_UNMAPPED, KP_UNMAPPED, \
    KP_SND, KP_SR, KP_BACK ,KP_OK ,KP_LT , KP_UNMAPPED, KP_UNMAPPED, KP_UNMAPPED, \
    KP_UNMAPPED, KP_SL, KP_UNMAPPED, KP_UNMAPPED, KP_UNMAPPED, KP_UNMAPPED, KP_UNMAPPED, KP_UNMAPPED, \
    KP_UNMAPPED, KP_UNMAPPED, KP_UNMAPPED, KP_UNMAPPED, KP_UNMAPPED, KP_UNMAPPED, KP_UNMAPPED, KP_UNMAPPED, \
    KP_UNMAPPED, KP_UNMAPPED, KP_UNMAPPED, KP_UNMAPPED, KP_UNMAPPED, KP_UNMAPPED, KP_UNMAPPED, KP_UNMAPPED, \
    KP_UNMAPPED, KP_UNMAPPED, KP_UNMAPPED, KP_UNMAPPED, KP_UNMAPPED, KP_UNMAPPED, KP_UNMAPPED, KP_UNMAPPED, \
}
#endif // KEY_MAP

#ifndef KEY_USB_BOOT_DOWNLOAD
#define KEY_USB_BOOT_DOWNLOAD {KP_OK,}
#endif

#ifndef KEY_BOOT_DOWNLOAD

#define KEY_BOOT_DOWNLOAD { KP_0, KP_OK, }

#endif // KEY_BOOT_DOWNLOAD

// =============================================================================
// RFD config
// =============================================================================
#ifndef TGT_RFD_CONFIG
#define TGT_RFD_CONFIG

#include "hal_tcu.h"
#define TGT_XCV_CONFIG    {.RST = TCO_UNUSED, .PDN  = TCO(11)              }
#define TGT_PA_CONFIG     {.ENA = TCO_UNUSED, .TXEN = TCO_UNUSED, .BS  = TCO_UNUSED }
#define TGT_SW_CONFIG     {.SW1 = TCO(2), .SW2  = TCO(3), .SW3 = TCO(4) }

// Note: Some XCV maybe have different control pin names, so someone who develop
//       the target configuration should explain the pin usage as below.
//
// FIXME Fix that with proper knowledge !
// PA->ENA is VLOGIC pin for SKY77518, MODEN for TQM4002, MOD for RDA6216
// PA-TXEN is BIAS for RDA6216 ?
//
#endif // TGT_RFD_CONFIG


// =============================================================================
// PMD config
// -----------------------------------------------------------------------------
/// This fills the structure PMD_CONFIG_T
// =============================================================================
#ifndef TGT_PMD_CONFIG

#define TGT_PMD_CONFIG                                                  \
    {                                                                   \
        .power =                                                        \
        {                                                               \
            { /* PMD_POWER_MIC          : Micro bias enable */          \
                .ldo            = {  .opal = PMD_LDO_MIC},              \
                .polarity       = TRUE,                                 \
                .shared         = TRUE, /* with PMD_POWER_EARPIECE */   \
                .powerOnState   = FALSE,                                \
            },                                                          \
            { /* PMD_POWER_CAMERA       : Camera LDO enable */          \
                .ldo            = { .opal = PMD_LDO_CAM|PMD_LDO_RF},    \
                .polarity       = TRUE,                                 \
                .shared         = FALSE,                                \
                .powerOnState   = FALSE,                                \
            },                                                          \
            { /* PMD_POWER_AUDIO        : Audio LDO enable */           \
                .ldo            = { .opal = PMD_LDO_ABB},               \
                .polarity       = TRUE,                                 \
                .shared         = FALSE,                                \
                .powerOnState   = FALSE,                                \
            },                                                          \
            { /* PMD_POWER_STEREO_DAC   : Stereo DAC LDO enable */      \
                .ldo            = { .opal = PMD_LDO_NONE},              \
                .polarity       = TRUE,                                 \
                .shared         = FALSE,                                \
                .powerOnState   = FALSE,                                \
            },                                                          \
            { /* PMD_POWER_LOUD_SPEAKER : Loud Speaker enable */        \
                .ldo            = { .pin = {.gpioId = HAL_GPIO_NONE}},  \
                .polarity       = TRUE,                                 \
                .shared         = FALSE,                                \
                .powerOnState   = FALSE,                                \
            },                                                          \
            { /* PMD_POWER_PA           : RF Power Amplifier */         \
                .ldo            = { .opal = PMD_LDO_NONE},              \
                .polarity       = TRUE,                                 \
                .shared         = FALSE,                                \
                .powerOnState   = FALSE,                                \
            },                                                          \
            { /* PMD_POWER_USB          : USB LDOs enable */            \
                .ldo            = { .opal = PMD_LDO_USB},               \
                .polarity       = TRUE,                                 \
                .shared         = FALSE,                                \
                .powerOnState   = FALSE,                                \
            },                                                          \
            { /* PMD_POWER_SDMMC        : SD/MMC LDO enable */          \
                .ldo            = { .opal = PMD_LDO_MMC},               \
                .polarity       = TRUE,                                 \
                .shared         = FALSE,                                \
                .powerOnState   = FALSE,                                \
            },                                                          \
            { /* PMD_POWER_FM           : FM LDO enable */              \
                .ldo            = { .opal = PMD_LDO_ABB},              \
                .polarity       = TRUE,                                 \
                .shared         = FALSE,                                \
                .powerOnState   = FALSE,                                \
            },                                                          \
            { /* PMD_POWER_EARPIECE     : Ear Piece Micro bias enable */\
                .ldo            = { .opal = PMD_LDO_MIC},               \
                .polarity       = TRUE,                                 \
                .shared         = TRUE, /* with PMD_POWER_MIC */        \
                .powerOnState   = FALSE,                                \
            },                                                          \
            { /* PMD_POWER_BT           : BlueTooth LDOs enable */      \
                .ldo            = { .opal = PMD_LDO_NONE},              \
                .polarity       = TRUE,                                 \
                .shared         = FALSE,                                \
                .powerOnState   = FALSE,                                \
            },                                                          \
            { /* PMD_POWER_CAMERA_FLASH : Camera Flash Light enable */  \
                .ldo            = { .pin = {.gpioId = HAL_GPIO_NONE}},   \
                .polarity       = TRUE,                                 \
                .shared         = FALSE,                                \
                .powerOnState   = FALSE,                                \
            },                                                          \
            { /* PMD_POWER_LCD          : (main) LCD LDO enable */      \
                .ldo            = { .opal = PMD_LDO_LCD},               \
                .polarity       = TRUE,                                 \
                .shared         = FALSE,                                \
                .powerOnState   = FALSE,                                \
            },                                                          \
            { /* PMD_POWER_I2C          : I2C LDO enable */             \
                 /* Inside this chip, PMD_LDO_CAM also supplies     */  \
                 /*   power to I2C1 I/O. It must be specified here  */  \
                 /*   if I2C1 is used on this board.                */  \
                 /* On this board, PMD_LDO_LCD also supplies power  */  \
                 /*   to I2C2 pull-up resistor.                     */  \
                .ldo            = { .opal = PMD_LDO_LCD|PMD_LDO_CAM},   \
                .polarity       = TRUE,                                 \
                .shared         = FALSE,                                \
                .powerOnState   = FALSE,                                \
            },                                                          \
        },                                                              \
        .level =                                                        \
        {                                                               \
            { /* PMD_LEVEL_SIM          : Sim class voltage */          \
                .type           = PMD_LEVEL_TYPE_NONE,                  \
                .ldo            = { .opal = PMD_LDO_NONE},              \
                .powerOnState   = 0,                                    \
            },                                                          \
            { /* PMD_LEVEL_KEYPAD       : KeyPad Back Light level */    \
                .type           = PMD_LEVEL_TYPE_OPAL,                  \
                .ldo            = { .pin = { .gpoId  = HAL_GPO_NONE}},  \
                .powerOnState   = 0,                                    \
            },                                                          \
            { /* PMD_LEVEL_LCD          : (main) LCD Back Light level*/ \
                .type           = PMD_LEVEL_TYPE_BACKLIGHT,             \
                .ldo            = { .opal = PMD_LDO_NONE},              \
                .powerOnState   = 0,                                    \
            },                                                          \
            { /* PMD_LEVEL_SUB_LCD      : Sub LCD Back Light level */   \
                .type           = PMD_LEVEL_TYPE_NONE,                  \
                .ldo            = { .opal = PMD_LDO_NONE},              \
                .powerOnState   = 0,                                    \
            },                                                          \
            { /* PMD_LEVEL_LED0         : LED0 Light level */           \
                .type           = PMD_LEVEL_TYPE_NONE,                  \
                .ldo            = { .opal = PMD_LDO_NONE},              \
                .powerOnState   = 0,                                    \
            },                                                          \
            { /* PMD_LEVEL_LED1         : LED1 Light level */           \
                .type           = PMD_LEVEL_TYPE_NONE,                  \
                .ldo            = { .opal = PMD_LDO_NONE},              \
                .powerOnState   = 0,                                    \
            },                                                          \
            { /* PMD_LEVEL_LED2         : LED2 Light level */           \
                .type           = PMD_LEVEL_TYPE_NONE,                  \
                .ldo            = { .opal = PMD_LDO_NONE},              \
                .powerOnState   = 0,                                    \
            },                                                          \
            { /* PMD_LEVEL_LED3         : LED3 Light level */           \
                .type           = PMD_LEVEL_TYPE_OPAL,                   \
                .ldo            = { .pin = { .gpioId  = HAL_GPIO_NONE}},   \
                .powerOnState   = 0,                                    \
            },                                                          \
            { /* PMD_LEVEL_VIBRATOR     : Vibrator control level */     \
                .type           = PMD_LEVEL_TYPE_LDO,                   \
                .ldo            = { .opal = PMD_LDO_VIBR},              \
                .powerOnState   = 0,                                    \
            },                                                          \
            { /* PMD_LEVEL_LOUD_SPEAKER : loudspeaker gain */           \
                .type           = PMD_LEVEL_TYPE_NONE,                  \
                .ldo            = { .opal = PMD_LDO_NONE},              \
                .powerOnState   = 0,                                    \
            },                                                          \
        },                                                              \
        .lightLevelToBacklight = {   0, 16, 48, 64, 64, 64, 64, 64},    \
        .lightLevelToPwm0 = {   0,  8,  8,  8,  8,  8,  8,  8},         \
        .lightLevelToPwm1 = {   0,   0,   0,   0,   0, 224, 224, 224},  \
        /* If any LDO is configured in ldoEnableNormal, it cannot   */  \
        /*   be controlled as a POWER or LEVEL LDO any more.        */  \
        .ldoEnableNormal = 0,                                           \
        .ldoEnableLowPower = 0,                                         \
        .ldoCamIs2_8 = TRUE,                                            \
        .ldoLcdIs2_8 = TRUE,                                            \
        .ldoMMCIs2_8 = TRUE,                                            \
        .ldoIbrIs2_8 = FALSE,                                           \
        .ldoRfIs2_8  = FALSE,                                           \
        .batteryGpadcChannel = HAL_ANA_GPADC_CHAN_7,                    \
        .batteryLevelChargeTermMV    = 4200,                            \
        .batteryLevelRechargeMV      = 4150,                            \
        .batteryLevelFullMV          = 4200,                            \
        .batteryChargeTimeout        = 4 HOURS,                         \
        .batteryOffsetHighActivityMV = 30,                              \
        .batteryOffsetPowerDownChargeMV = -150,                              \
        .powerOnVoltageMV            = 3400,                            \
        .powerDownVoltageMV          = 3400,                            \
        .batteryChargeCurrent        = PMD_CHARGER_400MA,               \
        .chargerGpadcChannel         = HAL_ANA_GPADC_CHAN_6,            \
        .chargerLevelUpperLimit      = 6500,                            \
        .chargerOffsetBackToNormal   = -300,                            \
        .batteryMVScreenAntiFlicker  = 3600,                            \
        .batteryOffsetScreenNormal   = 100,                             \
        .earpieceDetectGpio          = HAL_GPIO_2,                      \
        .earpieceGpadcChannel        = HAL_ANA_GPADC_CHAN_1,            \
    }

#endif // TGT_PMD_CONFIG



// =============================================================================
// MEMD config
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// note note note note note note note note note note note note note note note
// note note note note note note note note note note note note note note note
// mcp flash :not modify, still use memd/flsh_xxxx/include/memd_tgt_params_gallite.h
// spi flash :delete memd/flsh_xxxx/include/memd_tgt_params_gallite.h
// add memd/include/memd_tgt_params_8806.h for 8806
// add memd/include/memd_tgt_params_8808.h for 8808
// add memd/include/memd_tgt_params_8809.h for 8809
// note note note note note note note note note note note note note note note
// note note note note note note note note note note note note note note note
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// =============================================================================
#include "memd_tgt_params_8809.h"



// =============================================================================
// TGT_AUD_CONFIG
// -----------------------------------------------------------------------------
/// Audio interface configuration
// =============================================================================
#ifndef TGT_AUD_CONFIG
#define TGT_AUD_CONFIG

#define TGT_AUD_CONFIG_RECEIVER_DRIVER             CodecGallite
#define TGT_AUD_CONFIG_RECEIVER_PARAM              0
#define TGT_AUD_CONFIG_RECEIVER_OUTPUT_PATH        AUD_SPK_LOUD_SPEAKER////  AUD_SPK_LOUD_SPEAKERModefied By tian 20140505
#define TGT_AUD_CONFIG_RECEIVER_OUTPUT_TYPE        AUD_SPEAKER_STEREO
#define TGT_AUD_CONFIG_RECEIVER_INPUT_PATH         AUD_MIC_RECEIVER

#define TGT_AUD_CONFIG_EAR_PIECE_DRIVER            CodecGallite
#define TGT_AUD_CONFIG_EAR_PIECE_PARAM             0
#define TGT_AUD_CONFIG_EAR_PIECE_OUTPUT_PATH       AUD_SPK_EAR_PIECE
#define TGT_AUD_CONFIG_EAR_PIECE_OUTPUT_TYPE       AUD_SPEAKER_STEREO
#define TGT_AUD_CONFIG_EAR_PIECE_INPUT_PATH        AUD_MIC_EAR_PIECE

#define TGT_AUD_CONFIG_LOUD_SPEAKER_DRIVER         CodecGallite
#define TGT_AUD_CONFIG_LOUD_SPEAKER_PARAM          0
#define TGT_AUD_CONFIG_LOUD_SPEAKER_OUTPUT_PATH    AUD_SPK_LOUD_SPEAKER
#define TGT_AUD_CONFIG_LOUD_SPEAKER_OUTPUT_TYPE    AUD_SPEAKER_STEREO
#define TGT_AUD_CONFIG_LOUD_SPEAKER_INPUT_PATH     AUD_MIC_LOUD_SPEAKER

#define TGT_AUD_CONFIG_BT_DRIVER                   Bt
#define TGT_AUD_CONFIG_BT_PARAM                    0
#define TGT_AUD_CONFIG_BT_OUTPUT_PATH              AUD_SPK_EAR_PIECE
#define TGT_AUD_CONFIG_BT_OUTPUT_TYPE              AUD_SPEAKER_STEREO
#define TGT_AUD_CONFIG_BT_INPUT_PATH               AUD_MIC_RECEIVER

#define TGT_AUD_CONFIG_FM_DRIVER                   Fm
#define TGT_AUD_CONFIG_FM_PARAM                    0
#define TGT_AUD_CONFIG_FM_OUTPUT_PATH              AUD_SPK_EAR_PIECE
#define TGT_AUD_CONFIG_FM_OUTPUT_TYPE              AUD_SPEAKER_STEREO
#define TGT_AUD_CONFIG_FM_INPUT_PATH               AUD_MIC_RECEIVER

//atv aud config
#define TGT_AUD_CONFIG_TV_DRIVER                   Tv
#define TGT_AUD_CONFIG_TV_PARAM                    0
#define TGT_AUD_CONFIG_TV_OUTPUT_PATH              AUD_SPK_EAR_PIECE
#define TGT_AUD_CONFIG_TV_OUTPUT_TYPE              AUD_SPEAKER_STEREO
#define TGT_AUD_CONFIG_TV_INPUT_PATH               AUD_MIC_RECEIVER


#endif // TGT_AUD_CONFIG



// #############################################################################
// #                                                                           #
// #                           OPTIONNAL DRIVERS                               #
// #                                                                           #
// #############################################################################



#ifdef TGT_WITH_MCD_IF_sdmmc
// =============================================================================
// MCD config
// -----------------------------------------------------------------------------
/// @elem cardDetectGpio
/// Gpio Connected to socket to detect card insertion/removal .
/// (set to #HAL_GPIO_NONE) if not available.
/// @elem gpioCardDetectHigh
/// Define the polarity of the above GPIO: \c TRUE when active high,
/// \c FALSE when active low.
/// @elem dat3HasPullDown
/// Wether or not the DAT3/CD line has a pull-down and can be used for
/// minimalist new card detection (no insertion/removal interrupt.)
// =============================================================================
#ifndef TGT_MCD_CONFIG
#define TGT_MCD_CONFIG                                                  \
    {                                                                   \
        .cardDetectGpio     = HAL_GPIO_NONE,                            \
        .gpioCardDetectHigh = TRUE,                                     \
        .dat3HasPullDown    = FALSE                                     \
    }
#endif // TGT_MCD_CONFIG
#endif




#ifdef TGT_WITH_FM_MODEL_rdafm_8955
// =============================================================================
#ifndef TGT_FMD_CONFIG
#define TGT_FMD_CONFIG                                                  \
{                                                                   \
     .i2cBusId  = HAL_I2C_BUS_ID_3,     \
        .i2cBps             = HAL_I2C_BPS_100K,                         \
        .powerOnTime        = 1600,                                     \
        .volumeVal          = {0,0x2,0x04,0x06,0x08,0x0a,0x0c,0x0f},    \
        .channelSpacing     = FMD_CHANNEL_SPACE_100,                    \
        .seekRSSIThreshold  = 0x08,                                     \
}
#endif // TGT_FMD_CONFIG 
#endif //TGT_WITH_FM_MODEL_rdafm_8955



// =============================================================================
// TGT_SIMD_CONFIG
// -----------------------------------------------------------------------------
/// Configuration for SIMD
// =============================================================================
#ifndef TGT_SIMD_CONFIG
#define TGT_SIMD_CONFIG                                                 \
{                                                                       \
}
#endif // TGT_SIMD_CONFIG


// =============================================================================
// DSIM config
// -----------------------------------------------------------------------------
/// This fills the dual sim configuration
// =============================================================================
#ifndef TGT_DUALSIM_CONFIG
#define TGT_DUALSIM_CONFIG                                              \
    {                                                                   \
        HAL_SPI,                                                        \
        HAL_SPI_CS1                                                     \
    }
#endif // TGT_DUALSIM_CONFIG


// =============================================================================
// TGT_CAMD_CONFIG
// -----------------------------------------------------------------------------
/// Configuration for CAMD
// =============================================================================
#ifndef TGT_CAMD_CONFIG
#define TGT_CAMD_CONFIG                                                 \
{                                                                       \
    .i2cBusId          = HAL_I2C_BUS_ID_2,            \
    .camera_flash       =PMD_LEVEL_LED1,        \
}
#endif // TGT_CAMD_CONFIG


// =============================================================================
// TGT_CAMS_CONFIG
// -----------------------------------------------------------------------------
/// Configuration for CAMS
// =============================================================================
#ifndef TGT_CAMS_CONFIG
#define TGT_CAMS_CONFIG                                                 \
{                                                                       \
    .sensorOrientation = 1                                              \
}
#endif // TGT_CAMS_CONFIG


// =============================================================================
// ?d config
// -----------------------------------------------------------------------------
/// @todo add other driver configuration here if needed
// =============================================================================



// #############################################################################
// #                                                                           #
// #                                 SERVICES                                  #
// #                                                                           #
// #############################################################################



// =============================================================================
// ?s config
// -----------------------------------------------------------------------------
/// @todo add other service configuration here if needed
// =============================================================================




#endif //_TGT_BOARD_CFG_H_

