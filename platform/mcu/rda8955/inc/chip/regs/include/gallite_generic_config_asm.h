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


#ifndef _GALLITE_GENERIC_CONFIG_ASM_H_
#define _GALLITE_GENERIC_CONFIG_ASM_H_

//THIS FILE HAS BEEN GENERATED WITH COOLWATCHER. PLEASE EDIT WITH CARE !

#ifndef CT_ASM
#error "You are trying to use in a normal C code the assembly H description of 'gallite_generic_config'."
#endif


/// 0= chip option; 1= FPGA option
#define FPGA_OPTION                             (0)
/// 0= no baseband; 1= baseband included
#define BB_OPTION                               (1)
/// 0= Nothing; 1= BIST; 2= TEST MASTER
#define USE_TEST_MASTER                         (0)
/// 0= no monitor; 1=monitor included
#define USE_SYS_AHBC_MON                        (0)
/// 0= no debug host sel register as on test chip; 1=debug host sel register
/// included
#define HAVE_DEBUG_HOST_SEL                     (0)
/// 0= No VOC ; 1= VOC included
#define VOC_OPTION                              (1)
/// 0= No aif channels (0,1) ; 1= All 3 channels
#define AIF_OPTION                              (1)
/// 0= No MMI ; 1= MMI included : keypad, PWL/PWT, calendar
#define MMI_OPTION                              (1)
/// GPIO/GPO OPTIONS: numbers
#define NB_GPIO                                 (37)
#define NB_GPIO_INT                             (37)
#define NB_GPO                                  (10)
/// 0= No DMA ; 1= DMA included
#define DMA_OPTION                              (1)
/// 0 = no SPI, no GPADC, no UART1; 1 = SPI, GPADC, UART1 included
#define SYS_PERIPH_OPTION                       (1)
/// 0 = no USB; 1 = USB included
#define USB_OPTION                              (1)
/// System Ifc Number of generic channel (range 2 to 7)
#define SYS_IFC_NB_STD_CHANNEL                  (7)
/// 0 = no UART2; 1 = UART2 included
#define UART2_OPTION                            (1)
/// number of SPI1 CS
#define SPI1_NB_CS                              (2)
/// number of SPI1 DI
#define SPI1_NB_DI                              (2)
/// size of SPI1 DATA
#define SPI1_DATA_SIZE                          (8)
/// 0 = no SPI2; 1 = SPI2 included
#define SPI2_OPTION                             (1)
/// number of SPI2 CS
#define SPI2_NB_CS                              (2)
/// number of SPI DI
#define SPI2_NB_DI                              (2)
/// size of SPI2 DATA
#define SPI2_DATA_SIZE                          (8)
/// 0 = no SPI3; 1 = SPI3 included
#define SPI3_OPTION                             (1)
/// number of SPI3 CS
#define SPI3_NB_CS                              (2)
/// number of SPI DI
#define SPI3_NB_DI                              (2)
/// size of SPI3 DATA
#define SPI3_DATA_SIZE                          (32)
/// 0 = no SDMMC; 1 = SDMMC controller included
#define SDMMC_OPTION                            (1)
/// 0 = no Camera; 1 = Camera controller included
#define CAMERA_OPTION                           (1)
/// 0 = no Gouda; 1 = Gouda included
#define GOUDA_OPTION                            (1)
/// 0 = EBC, 1 = AHBM
#define MEMBRIDGE_OPTION                        (0)
/// for membridge internal ram: number of 32k blocks
#define MEMBRIDGE_RAM_NB_BLOCK                  (2)
/// for EBC option only
#define EBC_NB_BITS_ADDR                        (27)
/// for AHBM option only: address bus size
#define AHBM_NB_BITS_ADDR                       (28)
#define AHBM_INIT_SB_0                          (0)
#define AHBM_INIT_SB_1                          (0)
#define AHBM_INIT_SB_2                          (0)
#define AHBM_INIT_SB_3                          (0)
#define AHBM_INIT_SB_4                          (0)


#endif
