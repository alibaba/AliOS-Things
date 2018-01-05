/*!
    \file  gd32f4xx_ipa.h
    \brief definitions for the IPA
*/

/*
    Copyright (C) 2016 GigaDevice

    2016-08-15, V1.0.0, firmware for GD32F4xx
*/

#ifndef GD32F4XX_IPA_H
#define GD32F4XX_IPA_H

#include "gd32f4xx.h"

/* TLI definitions */
#define IPA                               IPA_BASE               /*!< IPA base address */

/* bits definitions */
/* registers definitions */
#define IPA_CTL                           REG32(IPA + 0x00U)     /*!< IPA control register */
#define IPA_INTF                          REG32(IPA + 0x04U)     /*!< IPA interrupt flag register */
#define IPA_INTC                          REG32(IPA + 0x08U)     /*!< IPA interrupt flag clear register */
#define IPA_FMADDR                        REG32(IPA + 0x0CU)     /*!< IPA foreground memory base address register */
#define IPA_FLOFF                         REG32(IPA + 0x10U)     /*!< IPA foreground line offset register */
#define IPA_BMADDR                        REG32(IPA + 0x14U)     /*!< IPA background memory base address register  */
#define IPA_BLOFF                         REG32(IPA + 0x18U)     /*!< IPA background line offset register */
#define IPA_FPCTL                         REG32(IPA + 0x1CU)     /*!< IPA foreground pixel control register */
#define IPA_FPV                           REG32(IPA + 0x20U)     /*!< IPA foreground pixel value register */
#define IPA_BPCTL                         REG32(IPA + 0x24U)     /*!< IPA background pixel control register */
#define IPA_BPV                           REG32(IPA + 0x28U)     /*!< IPA background pixel value register */
#define IPA_FLMADDR                       REG32(IPA + 0x2CU)     /*!< IPA foreground LUT memory base address register */
#define IPA_BLMADDR                       REG32(IPA + 0x30U)     /*!< IPA background LUT memory base address register */
#define IPA_DPCTL                         REG32(IPA + 0x34U)     /*!< IPA destination pixel control register */
#define IPA_DPV                           REG32(IPA + 0x38U)     /*!< IPA destination pixel value register  */
#define IPA_DMADDR                        REG32(IPA + 0x3CU)     /*!< IPA destination memory base address register */
#define IPA_DLOFF                         REG32(IPA + 0x40U)     /*!< IPA destination line offset register */
#define IPA_IMS                           REG32(IPA + 0x44U)     /*!< IPA image size register */
#define IPA_LM                            REG32(IPA + 0x48U)     /*!< IPA line mark register  */
#define IPA_ITCTL                         REG32(IPA + 0x4CU)     /*!< IPA inter-timer control register */

/* IPA_CTL */
#define IPA_CTL_TEN                       BIT(0)           /*!< transfer enable */
#define IPA_CTL_THU                       BIT(1)           /*!< transfer hang up */
#define IPA_CTL_TST                       BIT(2)           /*!< transfer stop */
#define IPA_CTL_TAEIE                     BIT(8)           /*!< enable bit for transfer access error interrupt */
#define IPA_CTL_FTFIE                     BIT(9)           /*!< enable bit for full transfer finish interrup */
#define IPA_CTL_TLMIE                     BIT(10)          /*!< enable bit for transfer line mark interrupt */
#define IPA_CTL_LACIE                     BIT(11)          /*!< enable bit for LUT access conflict interrupt */
#define IPA_CTL_LLFIE                     BIT(12)          /*!< enable bit for LUT loading finish interrupt */
#define IPA_CTL_WCFIE                     BIT(13)          /*!< enable bit for wrong configuration interrupt */
#define IPA_CTL_PFCM                      BITS(16,17)      /*!< pixel format convert mode */

/* IPA_INTF */
#define IPA_INTF_TAEIF                    BIT(0)           /*!< transfer access error interrupt flag */
#define IPA_INTF_FTFIF                    BIT(1)           /*!< full transfer finish interrupt flag */
#define IPA_INTF_TLMIF                    BIT(2)           /*!< transfer line mark interrupt flag */
#define IPA_INTF_LACIF                    BIT(3)           /*!< LUT access conflict interrupt flag */
#define IPA_INTF_LLFIF                    BIT(4)           /*!< LUT loading finish interrupt flag */
#define IPA_INTF_WCFIF                    BIT(5)           /*!< wrong configuration interrupt flag */

/* IPA_INTC */
#define IPA_INTC_TAEIFC                   BIT(0)           /*!< clear bit for transfer access error interrupt flag */
#define IPA_INTC_FTFIFC                   BIT(1)           /*!< clear bit for full transfer finish interrupt flag */
#define IPA_INTC_TLMIFC                   BIT(2)           /*!< clear bit for transfer line mark interrupt flag */
#define IPA_INTC_LACIFC                   BIT(3)           /*!< clear bit for LUT access conflict interrupt flag */
#define IPA_INTC_LLFIFC                   BIT(4)           /*!< clear bit for LUT loading finish interrupt flag */
#define IPA_INTC_WCFIFC                   BIT(5)           /*!< clear bit for wrong configuration interrupt flag */

/* IPA_FMADDR */
#define IPA_FMADDR_FMADDR                 BITS(0,31)       /*!< foreground memory base address */

/* IPA_FLOFF */
#define IPA_FLOFF_FLOFF                   BITS(0,13)       /*!< foreground line offset */

/* IPA_BMADDR */
#define IPA_BMADDR_BMADDR                 BITS(0,31)       /*!< background memory base address */

/* IPA_BLOFF */
#define IPA_BLOFF_BLOFF                   BITS(0,13)       /*!< background line offset */

/* IPA_FPCTL */
#define IPA_FPCTL_FPF                     BITS(0,3)        /*!< foreground pixel format */
#define IPA_FPCTL_FLPF                    BIT(4)           /*!< foreground LUT pixel format */
#define IPA_FPCTL_FLLEN                   BIT(5)           /*!< foreground LUT loading enable */
#define IPA_FPCTL_FCNP                    BITS(8,15)       /*!< foreground LUT number of pixel */
#define IPA_FPCTL_FAVCA                   BITS(16,17)      /*!< foreground alpha value calculation algorithm */
#define IPA_FPCTL_FPDAV                   BITS(24,31)      /*!< foreground pre- defined alpha value */

/* IPA_FPV */
#define IPA_FPV_FPDBV                     BITS(0,7)        /*!< foreground pre-defined red value */
#define IPA_FPV_FPDGV                     BITS(8,15)       /*!< foreground pre-defined green value */
#define IPA_FPV_FPDRV                     BITS(16,23)      /*!< foreground pre-defined red value */

/* IPA_BPCTL */
#define IPA_BPCTL_BPF                     BITS(0,3)        /*!< background pixel format */
#define IPA_BPCTL_BLPF                    BIT(4)           /*!< background LUT pixel format */
#define IPA_BPCTL_BLLEN                   BIT(5)           /*!< background LUT loading enable */
#define IPA_BPCTL_BCNP                    BITS(8,15)       /*!< background LUT number of pixel */
#define IPA_BPCTL_BAVCA                   BITS(16,17)      /*!< background alpha value calculation algorithm */
#define IPA_BPCTL_BPDAV                   BITS(24,31)      /*!< background pre- defined alpha value */

/* IPA_BPV */
#define IPA_BPV_BPDBV                     BITS(0,7)        /*!< background pre-defined blue value */
#define IPA_BPV_BPDGV                     BITS(8,15)       /*!< background pre-defined green value */
#define IPA_BPV_BPDRV                     BITS(16,23)      /*!< background pre-defined red value */

/* IPA_FLMADDR */
#define IPA_FLMADDR_FLMADDR               BITS(0,31)       /*!< foreground LUT memory base address */

/* IPA_BLMADDR */
#define IPA_BLMADDR_BLMADDR               BITS(0,31)       /*!< background LUT memory base address */

/* IPA_DPCTL */
#define IPA_DPCTL_DPF                     BITS(0,2)       /*!< destination pixel control register */

/* IPA_DPV */
/* destination pixel format ARGB8888 */
#define IPA_DPV_DPDBV_0                   BITS(0,7)        /*!< destination pre-defined blue value */
#define IPA_DPV_DPDGV_0                   BITS(8,15)       /*!< destination pre-defined green value */
#define IPA_DPV_DPDRV_0                   BITS(16,23)      /*!< destination pre-defined red value */
#define IPA_DPV_DPDAV_0                   BITS(24,31)      /*!< destination pre-defined alpha value */

/* destination pixel format RGB8888 */
#define IPA_DPV_DPDBV_1                   BITS(0,7)        /*!< destination pre-defined blue value */
#define IPA_DPV_DPDGV_1                   BITS(8,15)       /*!< destination pre-defined green value */
#define IPA_DPV_DPDRV_1                   BITS(16,23)      /*!< destination pre-defined red value */

/* destination pixel format RGB565 */
#define IPA_DPV_DPDBV_2                   BITS(0,4)        /*!< destination pre-defined blue value */
#define IPA_DPV_DPDGV_2                   BITS(5,10)       /*!< destination pre-defined green value */
#define IPA_DPV_DPDRV_2                   BITS(11,15)      /*!< destination pre-defined red value */

/* destination pixel format ARGB1555 */
#define IPA_DPV_DPDBV_3                   BITS(0,4)        /*!< destination pre-defined blue value */
#define IPA_DPV_DPDGV_3                   BITS(5,9)        /*!< destination pre-defined green value */
#define IPA_DPV_DPDRV_3                   BITS(10,14)      /*!< destination pre-defined red value */
#define IPA_DPV_DPDAV_3                   BIT(15)          /*!< destination pre-defined alpha value */

/* destination pixel format ARGB4444 */
#define IPA_DPV_DPDBV_4                   BITS(0,3)        /*!< destination pre-defined blue value */
#define IPA_DPV_DPDGV_4                   BITS(4,7)        /*!< destination pre-defined green value */
#define IPA_DPV_DPDRV_4                   BITS(8,11)       /*!< destination pre-defined red value */
#define IPA_DPV_DPDAV_4                   BITS(12,15)      /*!< destination pre-defined alpha value */

/* IPA_DMADDR */
#define IPA_DMADDR_DMADDR                 BITS(0,31)       /*!< destination memory base address */

/* IPA_DLOFF */
#define IPA_DLOFF_DLOFF                   BITS(0,13)       /*!< destination line offset */

/* IPA_IMS */
#define IPA_IMS_HEIGHT                    BITS(0,15)       /*!< height of the image to be processed */
#define IPA_IMS_WIDTH                     BITS(16,29)      /*!< width of the image to be processed */

/* IPA_LM */
#define IPA_LM_LM                         BITS(0,15)       /*!< line mark */

/* IPA_ITCTL */
#define IPA_ITCTL_ITEN                    BIT(0)           /*!< inter-timer enable */
#define IPA_ITCTL_NCCI                    BITS(8,15)       /*!< number of clock cycles interval */


/* constants definitions */
/* IPA foreground parameter struct definitions */
typedef struct
{   
    uint32_t foreground_memaddr;                          /*!< foreground memory base address */
    uint32_t foreground_lineoff;                          /*!< foreground line offset */
    uint32_t foreground_prealpha;                         /*!< foreground pre-defined alpha value */
    uint32_t foreground_alpha_algorithm;                  /*!< foreground alpha value calculation algorithm */
    uint32_t foreground_pf;                               /*!< foreground pixel format */
    uint32_t foreground_prered;                           /*!< foreground pre-defined red value */
    uint32_t foreground_pregreen;                         /*!< foreground pre-defined green value */
    uint32_t foreground_preblue;                          /*!< foreground pre-defined blue value */
}ipa_foreground_parameter_struct; 

/* IPA background parameter struct definitions */
typedef struct
{   
    uint32_t background_memaddr;                          /*!< background memory base address */
    uint32_t background_lineoff;                          /*!< background line offset */
    uint32_t background_prealpha;                         /*!< background pre-defined alpha value */
    uint32_t background_alpha_algorithm;                  /*!< background alpha value calculation algorithm */                                          
    uint32_t background_pf;                               /*!< background pixel format */
    uint32_t background_prered;                           /*!< background pre-defined red value */
    uint32_t background_pregreen;                         /*!< background pre-defined green value */
    uint32_t background_preblue;                          /*!< background pre-defined blue value */
}ipa_background_parameter_struct; 

/* IPA destination parameter struct definitions */
typedef struct
{
    uint32_t destination_memaddr;                         /*!< destination memory base address */    
    uint32_t destination_lineoff;                         /*!< destination line offset */
    uint32_t destination_prealpha;                        /*!< destination pre-defined alpha value */
    uint32_t destination_pf;                              /*!< destination pixel format */
    uint32_t destination_prered;                          /*!< destination pre-defined red value */
    uint32_t destination_pregreen;                        /*!< destination pre-defined green value */
    uint32_t destination_preblue;                         /*!< destination pre-defined blue value */
    uint32_t image_width;                                 /*!< width of the image to be processed */
    uint32_t image_height;                                /*!< height of the image to be processed */                                          
}ipa_destination_parameter_struct; 

/* destination pixel format */
typedef enum 
{
    IPA_DPF_ARGB8888,                                         /*!< destination pixel format ARGB8888 */
    IPA_DPF_RGB888,                                           /*!< destination pixel format RGB888 */
    IPA_DPF_RGB565,                                           /*!< destination pixel format RGB565 */
    IPA_DPF_ARGB1555,                                         /*!< destination pixel format ARGB1555 */
    IPA_DPF_ARGB4444                                          /*!< destination pixel format ARGB4444 */
} ipa_dpf_enum;

/* LUT pixel format */
#define IPA_LUT_PF_ARGB8888             ((uint8_t)0x00U)                 /*!< LUT pixel format ARGB8888 */
#define IPA_LUT_PF_RGB888               ((uint8_t)0x01U)                 /*!< LUT pixel format RGB888 */

/* Inter-timer */
#define IPA_INTER_TIMER_DISABLE         ((uint8_t)0x00U)                 /*!< Inter-timer disable */
#define IPA_INTER_TIMER_ENABLE          ((uint8_t)0x01U)                 /*!< Inter-timer enable */

/* IPA pixel format convert mode */
#define CTL_PFCM(regval)                (BITS(16,17) & ((regval) << 16))
#define IPA_FGTODE                      CTL_PFCM(0)                      /*!< foreground memory to destination memory without pixel format convert */
#define IPA_FGTODE_PF_CONVERT           CTL_PFCM(1)                      /*!< foreground memory to destination memory with pixel format convert */
#define IPA_FGBGTODE                    CTL_PFCM(2)                      /*!< blending foreground and background memory to destination memory */
#define IPA_FILL_UP_DE                  CTL_PFCM(3)                      /*!< fill up destination memory with specific color */

/* foreground alpha value calculation algorithm */
#define FPCTL_FAVCA(regval)             (BITS(16,17) & ((regval) << 16))
#define IPA_FG_ALPHA_MODE_0             FPCTL_FAVCA(0)                   /*!< no effect */
#define IPA_FG_ALPHA_MODE_1             FPCTL_FAVCA(1)                   /*!< FPDAV[7:0] is selected as the foreground alpha value */
#define IPA_FG_ALPHA_MODE_2             FPCTL_FAVCA(2)                   /*!< FPDAV[7:0] multiplied by read alpha value */

/* background alpha value calculation algorithm */
#define BPCTL_BAVCA(regval)             (BITS(16,17) & ((regval) << 16))
#define IPA_BG_ALPHA_MODE_0             BPCTL_BAVCA(0)                   /*!< no effect */
#define IPA_BG_ALPHA_MODE_1             BPCTL_BAVCA(1)                   /*!< BPDAV[7:0] is selected as the background alpha value */
#define IPA_BG_ALPHA_MODE_2             BPCTL_BAVCA(2)                   /*!< BPDAV[7:0] multiplied by read alpha value */

/* foreground pixel format */
#define FPCTL_PPF(regval)               (BITS(0,3) & ((regval)))
#define FOREGROUND_PPF_ARGB8888         FPCTL_PPF(0)                     /*!< foreground pixel format ARGB8888 */
#define FOREGROUND_PPF_RGB888           FPCTL_PPF(1)                     /*!< foreground pixel format RGB888 */
#define FOREGROUND_PPF_RGB565           FPCTL_PPF(2)                     /*!< foreground pixel format RGB565 */
#define FOREGROUND_PPF_ARG1555          FPCTL_PPF(3)                     /*!< foreground pixel format ARGB1555 */
#define FOREGROUND_PPF_ARGB4444         FPCTL_PPF(4)                     /*!< foreground pixel format ARGB4444 */
#define FOREGROUND_PPF_L8               FPCTL_PPF(5)                     /*!< foreground pixel format L8 */
#define FOREGROUND_PPF_AL44             FPCTL_PPF(6)                     /*!< foreground pixel format AL44 */
#define FOREGROUND_PPF_AL88             FPCTL_PPF(7)                     /*!< foreground pixel format AL88 */
#define FOREGROUND_PPF_L4               FPCTL_PPF(8)                     /*!< foreground pixel format L4 */
#define FOREGROUND_PPF_A8               FPCTL_PPF(9)                     /*!< foreground pixel format A8 */
#define FOREGROUND_PPF_A4               FPCTL_PPF(10)                    /*!< foreground pixel format A4 */

/* background pixel format */
#define BPCTL_PPF(regval)               (BITS(0,3) & ((regval)))
#define BACKGROUND_PPF_ARGB8888         BPCTL_PPF(0)                     /*!< background pixel format ARGB8888 */
#define BACKGROUND_PPF_RGB888           BPCTL_PPF(1)                     /*!< background pixel format RGB888 */
#define BACKGROUND_PPF_RGB565           BPCTL_PPF(2)                     /*!< background pixel format RGB565 */
#define BACKGROUND_PPF_ARG1555          BPCTL_PPF(3)                     /*!< background pixel format ARGB1555 */
#define BACKGROUND_PPF_ARGB4444         BPCTL_PPF(4)                     /*!< background pixel format ARGB4444 */
#define BACKGROUND_PPF_L8               BPCTL_PPF(5)                     /*!< background pixel format L8 */
#define BACKGROUND_PPF_AL44             BPCTL_PPF(6)                     /*!< background pixel format AL44 */
#define BACKGROUND_PPF_AL88             BPCTL_PPF(7)                     /*!< background pixel format AL88 */
#define BACKGROUND_PPF_L4               BPCTL_PPF(8)                     /*!< background pixel format L4 */
#define BACKGROUND_PPF_A8               BPCTL_PPF(9)                     /*!< background pixel format A8 */
#define BACKGROUND_PPF_A4               BPCTL_PPF(10)                    /*!< background pixel format A4 */


/* function declarations */

/* deinitialize IPA */
void ipa_deinit(void);
/* IPA transfer enable */
void ipa_transfer_enable(void);
/* IPA transfer hang up enable */
void ipa_transfer_hangup_enable(void);
/* IPA transfer hang up disable */
void ipa_transfer_hangup_disable(void);
/* IPA transfer stop enable */
void ipa_transfer_stop_enable(void);
/* IPA transfer stop disable */
void ipa_transfer_stop_disable(void);
/* IPA foreground LUT loading enable */
void ipa_foreground_lut_loading_enable(void);
/* IPA background LUT loading enable */
void ipa_background_lut_loading_enable(void);
/* IPA transfer enable */
void ipa_pixel_format_convert_mod(uint32_t pfcm);

/* initialize foreground parameters */
void ipa_foreground_init(ipa_foreground_parameter_struct* foreground_struct);
/* initialize background parameters */
void ipa_background_init(ipa_background_parameter_struct* background_struct);
/* initialize destination parameters */
void ipa_destination_init(ipa_destination_parameter_struct* destination_struct);
/* initialize IPA foreground LUT parameters */
void ipa_foreground_lut_init(uint32_t fg_lut_num,uint8_t fg_lut_pf, uint32_t fg_lut_addr);
/* initialize IPA background LUT parameters */
void ipa_background_lut_init(uint32_t bg_lut_num,uint8_t bg_lut_pf, uint32_t bg_lut_addr);

/* configure line mark */
void ipa_line_mark_config(uint32_t linenum);
/* Inter-timer enable or disable */
void ipa_inter_timer_config(uint8_t timercfg);
/* number of clock cycles interval set */
void ipa_interval_clock_num_config(uint32_t clk_num );

/* IPA interrupt enable */
void ipa_interrupt_enable(uint32_t inttype);
/* IPA interrupt disable */
void ipa_interrupt_disable(uint32_t inttype);
/* get IPA interrupt flag */
FlagStatus ipa_interrupt_flag_get(uint32_t intflag);
/* clear IPA interrupt flag */
void ipa_interrupt_flag_clear(uint32_t intflag);

#endif /* GD32F4XX_IPA_H */
