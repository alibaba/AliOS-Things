/*!
    \file  gd32f4xx_pmu.h
    \brief definitions for the PMU
*/

/*
    Copyright (C) 2016 GigaDevice

    2016-08-15, V1.0.2, firmware for GD32F4xx
*/

#ifndef GD32F4XX_PMU_H
#define GD32F4XX_PMU_H

#include "gd32f4xx.h"

/* PMU definitions */
#define PMU                           PMU_BASE                 /*!< PMU base address */

/* registers definitions */
#define PMU_CTL                       REG32((PMU) + 0x00U)     /*!< PMU control register */
#define PMU_CS                        REG32((PMU) + 0x04U)     /*!< PMU control and status register */

/* bits definitions */
/* PMU_CTL */
#define PMU_CTL_LDOLP                 BIT(0)                   /*!< LDO low power mode */
#define PMU_CTL_STBMOD                BIT(1)                   /*!< standby mode */
#define PMU_CTL_WURST                 BIT(2)                   /*!< wakeup flag reset */
#define PMU_CTL_STBRST                BIT(3)                   /*!< standby flag reset */
#define PMU_CTL_LVDEN                 BIT(4)                   /*!< low voltage detector enable */
#define PMU_CTL_LVDT                  BITS(5,7)                /*!< low voltage detector threshold */
#define PMU_CTL_BKPWEN                BIT(8)                   /*!< backup domain write enable */
#define PMU_CTL_LDLP                  BIT(10)                  /*!< low-driver mode when use low power LDO */
#define PMU_CTL_LDNP                  BIT(11)                  /*!< low-driver mode when use normal power LDO */
#define PMU_CTL_LDOVS                 BITS(14,15)              /*!< LDO output voltage select */
#define PMU_CTL_HDEN                  BIT(16)                  /*!< high-driver mode enable */
#define PMU_CTL_HDS                   BIT(17)                  /*!< high-driver mode switch */
#define PMU_CTL_LDEN                  BITS(18,19)              /*!< low-driver mode enable in deep-sleep mode */

/* PMU_CS */
#define PMU_CS_WUF                    BIT(0)                   /*!< wakeup flag */
#define PMU_CS_STBF                   BIT(1)                   /*!< standby flag */
#define PMU_CS_LVDF                   BIT(2)                   /*!< low voltage detector status flag */
#define PMU_CS_BLDORF                 BIT(3)                   /*!< backup SRAM LDO ready flag */
#define PMU_CS_WUPEN                  BIT(8)                   /*!< wakeup pin enable */
#define PMU_CS_BLDOON                 BIT(9)                   /*!< backup SRAM LDO on */
#define PMU_CS_LDOVSRF                BIT(14)                  /*!< LDO voltage select ready flag */
#define PMU_CS_HDRF                   BIT(16)                  /*!< high-driver ready flag */
#define PMU_CS_HDSRF                  BIT(17)                  /*!< high-driver switch ready flag */
#define PMU_CS_LDRF                   BITS(18,19)              /*!< Low-driver mode ready flag */

/* constants definitions */
/* PMU low voltage detector threshold definitions */
#define CTL_LVDT(regval)              (BITS(5,7)&((uint32_t)(regval)<<5))
#define PMU_LVDT_0                    CTL_LVDT(0)              /*!< voltage threshold is 2.1V */
#define PMU_LVDT_1                    CTL_LVDT(1)              /*!< voltage threshold is 2.3V */
#define PMU_LVDT_2                    CTL_LVDT(2)              /*!< voltage threshold is 2.4V */
#define PMU_LVDT_3                    CTL_LVDT(3)              /*!< voltage threshold is 2.6V */
#define PMU_LVDT_4                    CTL_LVDT(4)              /*!< voltage threshold is 2.7V */
#define PMU_LVDT_5                    CTL_LVDT(5)              /*!< voltage threshold is 2.9V */
#define PMU_LVDT_6                    CTL_LVDT(6)              /*!< voltage threshold is 3.0V */
#define PMU_LVDT_7                    CTL_LVDT(7)              /*!< voltage threshold is 3.1V */

/* PMU LDO output voltage select definitions */
#define CTL_LDOVS(regval)             (BITS(14,15)&((uint32_t)(regval)<<14))
#define PMU_LDOVS_LOW                 CTL_LDOVS(1)             /*!< LDO output voltage low mode */
#define PMU_LDOVS_MID                 CTL_LDOVS(2)             /*!< LDO output voltage mid mode */
#define PMU_LDOVS_HIGH                CTL_LDOVS(3)             /*!< LDO output voltage high mode */

/* PMU low-driver mode enable in deep-sleep mode */
#define CTL_LDEN(regval)              (BITS(18,19)&((uint32_t)(regval)<<18))
#define PMU_LOWDRIVER_DISABLE         CTL_LDEN(0)              /*!< low-driver mode disable in deep-sleep mode */
#define PMU_LOWDRIVER_ENABLE          CTL_LDEN(3)              /*!< low-driver mode enable in deep-sleep mode */

/* PMU high-driver mode switch */
#define CTL_HDS(regval)               (BIT(17)&((uint32_t)(regval)<<17))
#define PMU_HIGHDR_SWITCH_NONE        CTL_HDS(0)               /*!< no high-driver mode switch */
#define PMU_HIGHDR_SWITCH_EN          CTL_HDS(1)               /*!< high-driver mode switch */

/* PMU low-driver mode when use low power LDO */
#define CTL_LDLP(regval)              (BIT(10)&((uint32_t)(regval)<<10))
#define PMU_NORMALDR_LOWPWR           CTL_LDLP(0)              /*!< normal driver when use low power LDO */
#define PMU_LOWDR_LOWPWR              CTL_LDLP(1)              /*!< low-driver mode enabled when LDEN is 11 and use low power LDO */

/* PMU low-driver mode when use normal power LDO */
#define CTL_LDNP(regval)              (BIT(11)&((uint32_t)(regval)<<11))
#define PMU_NORMALDR_NORMALPWR        CTL_LDNP(0)              /*!< normal driver when use normal power LDO */
#define PMU_LOWDR_NORMALPWR           CTL_LDNP(1)              /*!< low-driver mode enabled when LDEN is 11 and use normal power LDO */

/* PMU low power mode ready flag definitions */
#define CS_LDRF(regval)               (BITS(18,19)&((uint32_t)(regval)<<18))
#define PMU_LDRF_NORMAL               CS_LDRF(0)               /*!< normal driver in deep-sleep mode */
#define PMU_LDRF_LOWDRIVER            CS_LDRF(3)               /*!< low-driver mode in deep-sleep mode */

/* PMU backup SRAM LDO on or off */
#define CS_BLDOON(regval)             (BIT(9)&((uint32_t)(regval)<<9))
#define PMU_BLDOON_OFF                CS_BLDOON(0)             /*!< backup SRAM LDO off */
#define PMU_BLDOON_ON                 CS_BLDOON(1)             /*!< the backup SRAM LDO on */

/* PMU flag definitions */
#define PMU_FLAG_WAKEUP               PMU_CS_WUF               /*!< wakeup flag status */
#define PMU_FLAG_STANDBY              PMU_CS_STBF              /*!< standby flag status */
#define PMU_FLAG_LVD                  PMU_CS_LVDF              /*!< lvd flag status */
#define PMU_FLAG_BLDORF               PMU_CS_BLDORF            /*!< backup SRAM LDO ready flag */
#define PMU_FLAG_LDOVSRF              PMU_CS_LDOVSRF           /*!< LDO voltage select ready flag */
#define PMU_FLAG_HDRF                 PMU_CS_HDRF              /*!< high-driver ready flag */
#define PMU_FLAG_HDSRF                PMU_CS_HDSRF             /*!< high-driver switch ready flag */
#define PMU_FLAG_LDRF                 PMU_CS_LDRF              /*!< low-driver mode ready flag */

/* PMU ldo definitions */
#define PMU_LDO_NORMAL                ((uint32_t)0x00000000U)  /*!< LDO normal work when PMU enter deepsleep mode */
#define PMU_LDO_LOWPOWER              PMU_CTL_LDOLP            /*!< LDO work at low power status when PMU enter deepsleep mode */

/* PMU flag reset definitions */
#define PMU_FLAG_RESET_WAKEUP         ((uint8_t)0x00U)         /*!< wakeup flag reset */
#define PMU_FLAG_RESET_STANDBY        ((uint8_t)0x01U)         /*!< standby flag reset */

/* PMU command constants definitions */
#define WFI_CMD                       ((uint8_t)0x00U)         /*!< use WFI command */
#define WFE_CMD                       ((uint8_t)0x01U)         /*!< use WFE command */

/* function declarations */
/* reset PMU register */
void pmu_deinit(void);

/* select low voltage detector threshold */
void pmu_lvd_select(uint32_t lvdt_n);
/* LDO output voltage select */
void pmu_ldo_output_select(uint32_t ldo_output);
/* PMU lvd disable */
void pmu_lvd_disable(void);

/* functions of low-driver mode and high-driver mode in deep-sleep mode */
/* high-driver mode switch */
void pmu_highdriver_switch_select(uint32_t highdr_switch);
/* high-driver mode enable */
void pmu_highdriver_mode_enable(void);
/* high-driver mode disable */
void pmu_highdriver_mode_disable(void);
/* low-driver mode enable in deep-sleep mode */
void pmu_low_driver_mode_enable(uint32_t lowdr_mode);
/* in deep-sleep mode, low-driver mode when use low power LDO */
void pmu_lowdriver_lowpower_config(uint32_t mode);
/* in deep-sleep mode, low-driver mode when use normal power LDO */
void pmu_lowdriver_normalpower_config(uint32_t mode);

/* set PMU mode */
/* PMU work at sleep mode */
void pmu_to_sleepmode(uint8_t sleepmodecmd);
/* PMU work at deepsleep mode */
void pmu_to_deepsleepmode(uint32_t ldo, uint8_t deepsleepmodecmd);
/* PMU work at standby mode */
void pmu_to_standbymode(uint8_t standbymodecmd);
/* PMU wakeup pin enable */
void pmu_wakeup_pin_enable(void);
/* PMU wakeup pin disable */
void pmu_wakeup_pin_disable(void);

/* backup related functions */
/* backup SRAM LDO on */
void pmu_backup_ldo_config(uint32_t bkp_ldo);
/* backup domain write enable */
void pmu_backup_write_enable(void);
/* backup domain write disable */
void pmu_backup_write_disable(void);

/* flag functions */
/* reset flag bit */
void pmu_flag_reset(uint32_t flag_reset);
/* get flag status */
FlagStatus pmu_flag_get(uint32_t pmu_flag);

#endif /* GD32F4XX_PMU_H */
