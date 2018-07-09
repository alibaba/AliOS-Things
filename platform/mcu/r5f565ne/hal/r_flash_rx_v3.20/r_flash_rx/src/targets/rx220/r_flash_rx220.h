/*******************************************************************************
* DISCLAIMER
* This software is supplied by Renesas Electronics Corporation and is only 
* intended for use with Renesas products. No other uses are authorized. This 
* software is owned by Renesas Electronics Corporation and is protected under
* all applicable laws, including copyright laws.
* THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING
* THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING BUT NOT
* LIMITED TO WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE 
* AND NON-INFRINGEMENT. ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED.
* TO THE MAXIMUM EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS 
* ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES SHALL BE LIABLE 
* FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR
* ANY REASON RELATED TO THIS SOFTWARE, EVEN IF RENESAS OR ITS AFFILIATES HAVE
* BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
* Renesas reserves the right, without notice, to make changes to this software
* and to discontinue the availability of this software. By using this software,
* you agree to the additional terms and conditions found by accessing the 
* following link:
* http://www.renesas.com/disclaimer 
*
* Copyright (C) 2013 Renesas Electronics Corporation. All rights reserved.
*******************************************************************************/
/******************************************************************************
* File Name    : r_flash_rx220.h
* Description  : This file has specific information about the ROM and DF on 
*                RX220 Group MCUs.
*******************************************************************************
* History : DD.MM.YYYY Version Description
*         : 06.09.2013 1.00    First Release
*         : 09.08.2016 2.00    Modified for BSPless flash
*         : 17.11.2016 2.10    Added FLASH_xxx size and num block defines
*         : 28.02.2017 2.11    Corrected DF_NUM_BLOCKS value.
*         : 02.08.2017 2.20    Replaced #include "r_mcu_config.h" with "r_flash_targets.h"
******************************************************************************/

#ifndef FLASH_API_RX220_H
#define FLASH_API_RX220_H

/******************************************************************************
Includes   <System Includes> , "Project Includes"
******************************************************************************/
#include <stdint.h>
#include "r_flash_targets.h"


/******************************************************************************
Macro definitions
******************************************************************************/
/* Defines the number of flash areas */
#define NUM_ROM_AREAS           1
/* Defines the start program/erase address for the different flash areas */
#define ROM_AREA_0              (0x00F80000)
   
/* Defines whether this MCU requires the FCU RAM be enabled and initialized.
   If uncommented, then MCU is required to init FCU RAM.
   If commented out, then MCU is not required. */
//#define FCU_RAM_INIT_REQUIRED   (1)

/*  Bottom of DF Area */
#define DF_ADDRESS              0x00100000
/* Used for getting DF block */
#define DF_MASK                 0xFFFFF800
/* Used for getting erase boundary in DF block when doing blank checking */
#define DF_ERASE_BLOCK_SIZE     0x00000080
/* This is used to get the boundary of the 'fake' blocks that are 2KB. */
#define DF_BLOCK_SIZE_LARGE     0x00000800
/* Defines how many DF blocks are on this part */
#define DF_NUM_BLOCKS           64
/* THis is the last address in the last block, not the starting address */
#define FLASH_DF_LAST_BLOCK        ((FLASH_DF_BLOCK_INVALID) - (DF_ERASE_BLOCK_SIZE))

/* Defines how many ROM blocks are on this part */
#if   MCU_ROM_SIZE_BYTES == 32768
    #define ROM_NUM_BLOCKS  16        //32KB part
#elif MCU_ROM_SIZE_BYTES == 65536
    #define ROM_NUM_BLOCKS  32        //64KB part
#elif MCU_ROM_SIZE_BYTES == 131072
    #define ROM_NUM_BLOCKS  64    	  //128KB part
#elif MCU_ROM_SIZE_BYTES == 262144
    #define ROM_NUM_BLOCKS  128  	  //256KB part
#else
    #error "ERROR - Flash API - This ROM size is not defined. Update r_flash_rx220.h with this MCUs information."
#endif

/* Programming size for ROM in bytes. This is being set as 2 to comply with older versions where there was only 1
   size option.  */
#define ROM_PROGRAM_SIZE          2
/* Programming size for ROM in bytes. RX220 supports 128, 8 and 2 byte options. */
#define ROM_PROGRAM_SIZE_LARGE    128
#define ROM_PROGRAM_SIZE_MEDIUM   8
#define ROM_PROGRAM_SIZE_SMALL    2
/* Programming size for data flash in bytes */
/* NOTE: RX220 can program 2 or 8 bytes at a time. */
#define DF_PROGRAM_SIZE_LARGE     8
#define DF_PROGRAM_SIZE_SMALL     2

#define FLASH_CF_BLOCK_SIZE             (2048)
#define FLASH_NUM_BLOCKS_CF             ROM_NUM_BLOCKS
#define FLASH_CF_MIN_PGM_SIZE           ROM_PROGRAM_SIZE
#define FLASH_DF_BLOCK_SIZE             DF_ERASE_BLOCK_SIZE
#define FLASH_NUM_BLOCKS_DF             DF_NUM_BLOCKS
#define FLASH_DF_MIN_PGM_SIZE           DF_PROGRAM_SIZE_SMALL
#define FLASH_DF_HIGHEST_VALID_BLOCK    FLASH_DF_LAST_BLOCK


/* User ROM Block Area        Size: Start Addr -   End Addr */
typedef enum _flash_block_address
{
    FLASH_CF_BLOCK_0     = 0x00FFF800,    /*   2KB: 0xFFFFF800 - 0xFFFFFFFF */
    FLASH_CF_BLOCK_1     = 0x00FFF000,    /*   2KB: 0xFFFFF000 - 0xFFFFF7FF */
    FLASH_CF_BLOCK_2     = 0x00FFE800,    /*   2KB: 0xFFFFE800 - 0xFFFFEFFF */
    FLASH_CF_BLOCK_3     = 0x00FFE000,    /*   2KB: 0xFFFFE000 - 0xFFFFE7FF */
    FLASH_CF_BLOCK_4     = 0xFFFFD800,    /*   2KB: 0xFFFFD800 - 0xFFFFDFFF */
    FLASH_CF_BLOCK_5     = 0x00FFD000,    /*   2KB: 0xFFFFD000 - 0xFFFFD7FF */
    FLASH_CF_BLOCK_6     = 0x00FFC800,    /*   2KB: 0xFFFFC800 - 0xFFFFCFFF */
    FLASH_CF_BLOCK_7     = 0x00FFC000,    /*   2KB: 0xFFFFC000 - 0xFFFFC7FF */
    FLASH_CF_BLOCK_8     = 0x00FFB800,    /*   2KB: 0xFFFFB800 - 0xFFFFBFFF */
    FLASH_CF_BLOCK_9     = 0x00FFB000,    /*   2KB: 0xFFFFB000 - 0xFFFFB7FF */
    FLASH_CF_BLOCK_10    = 0x00FFA800,    /*   2KB: 0xFFFFA800 - 0xFFFFAFFF */
    FLASH_CF_BLOCK_11    = 0x00FFA000,    /*   2KB: 0xFFFFA000 - 0xFFFFA7FF */
    FLASH_CF_BLOCK_12    = 0x00FF9800,    /*   2KB: 0xFFFF9800 - 0xFFFF9FFF */
    FLASH_CF_BLOCK_13    = 0x00FF9000,    /*   2KB: 0xFFFF9000 - 0xFFFF97FF */
    FLASH_CF_BLOCK_14    = 0x00FF8800,    /*   2KB: 0xFFFF8800 - 0xFFFF8FFF */
    FLASH_CF_BLOCK_15    = 0x00FF8000,    /*   2KB: 0xFFFF8000 - 0xFFFF87FF */
#if   MCU_CFG_PART_MEMORY_SIZE == 0x1   //   32KB ROM
    FLASH_CF_BLOCK_INVALID = 0x00FEFFFF,
#else
    FLASH_CF_BLOCK_16    = 0x00FF7800,    /*   2KB: 0xFFFF7800 - 0xFFFF7FFF */
    FLASH_CF_BLOCK_17    = 0x00FF7000,    /*   2KB: 0xFFFF7000 - 0xFFFF77FF */
    FLASH_CF_BLOCK_18    = 0x00FF6800,    /*   2KB: 0xFFFF6800 - 0xFFFF6FFF */
    FLASH_CF_BLOCK_19    = 0x00FF6000,    /*   2KB: 0xFFFF6000 - 0xFFFF67FF */
    FLASH_CF_BLOCK_20    = 0x00FF5800,    /*   2KB: 0xFFFF5800 - 0xFFFF5FFF */
    FLASH_CF_BLOCK_21    = 0x00FF5000,    /*   2KB: 0xFFFF5000 - 0xFFFF57FF */
    FLASH_CF_BLOCK_22    = 0x00FF4800,    /*   2KB: 0xFFFF4800 - 0xFFFF4FFF */
    FLASH_CF_BLOCK_23    = 0x00FF4000,    /*   2KB: 0xFFFF4000 - 0xFFFF47FF */
    FLASH_CF_BLOCK_24    = 0x00FF3800,    /*   2KB: 0xFFFF3800 - 0xFFFF3FFF */
    FLASH_CF_BLOCK_25    = 0x00FF3000,    /*   2KB: 0xFFFF3000 - 0xFFFF37FF */
    FLASH_CF_BLOCK_26    = 0x00FF2800,    /*   2KB: 0xFFFF2800 - 0xFFFF2FFF */
    FLASH_CF_BLOCK_27    = 0x00FF2000,    /*   2KB: 0xFFFF2000 - 0xFFFF27FF */
    FLASH_CF_BLOCK_28    = 0x00FF1800,    /*   2KB: 0xFFFF1800 - 0xFFFF1FFF */
    FLASH_CF_BLOCK_29    = 0x00FF1000,    /*   2KB: 0xFFFF1000 - 0xFFFF17FF */
    FLASH_CF_BLOCK_30    = 0x00FF0800,    /*   2KB: 0xFFFF0800 - 0xFFFF0FFF */
    FLASH_CF_BLOCK_31    = 0x00FF0000,    /*   2KB: 0xFFFF0000 - 0xFFFF07FF */
#if   MCU_CFG_PART_MEMORY_SIZE == 0x3   //   64KB ROM
    FLASH_CF_BLOCK_INVALID = 0x00FEFFFF,
#else
    FLASH_CF_BLOCK_32    = 0x00FEF800,    /*   2KB: 0xFFFEF800 - 0xFFFEFFFF */
    FLASH_CF_BLOCK_33    = 0x00FEF000,    /*   2KB: 0xFFFEF000 - 0xFFFEF7FF */
    FLASH_CF_BLOCK_34    = 0x00FEE800,    /*   2KB: 0xFFFEE800 - 0xFFFEEFFF */
    FLASH_CF_BLOCK_35    = 0x00FEE000,    /*   2KB: 0xFFFEE000 - 0xFFFEE7FF */
    FLASH_CF_BLOCK_36    = 0x00FED800,    /*   2KB: 0xFFFED800 - 0xFFFEDFFF */
    FLASH_CF_BLOCK_37    = 0x00FED000,    /*   2KB: 0xFFFED000 - 0xFFFED7FF */
    FLASH_CF_BLOCK_38    = 0x00FEC800,    /*   2KB: 0xFFFEC800 - 0xFFFECFFF */
    FLASH_CF_BLOCK_39    = 0x00FEC000,    /*   2KB: 0xFFFEC000 - 0xFFFEC7FF */
    FLASH_CF_BLOCK_40    = 0x00FEB800,    /*   2KB: 0xFFFEB800 - 0xFFFEBFFF */
    FLASH_CF_BLOCK_41    = 0x00FEB000,    /*   2KB: 0xFFFEB000 - 0xFFFEB7FF */
    FLASH_CF_BLOCK_42    = 0x00FEA800,    /*   2KB: 0xFFFEA800 - 0xFFFEAFFF */
    FLASH_CF_BLOCK_43    = 0x00FEA000,    /*   2KB: 0xFFFEA000 - 0xFFFEA7FF */
    FLASH_CF_BLOCK_44    = 0x00FE9800,    /*   2KB: 0xFFFE9800 - 0xFFFE9FFF */
    FLASH_CF_BLOCK_45    = 0x00FE9000,    /*   2KB: 0xFFFE9000 - 0xFFFE97FF */
    FLASH_CF_BLOCK_46    = 0x00FE8800,    /*   2KB: 0xFFFE8800 - 0xFFFE8FFF */
    FLASH_CF_BLOCK_47    = 0x00FE8000,    /*   2KB: 0xFFFE8000 - 0xFFFE87FF */
    FLASH_CF_BLOCK_48    = 0x00FE7800,    /*   2KB: 0xFFFE7800 - 0xFFFE7FFF */
    FLASH_CF_BLOCK_49    = 0x00FE7000,    /*   2KB: 0xFFFE7000 - 0xFFFE77FF */
    FLASH_CF_BLOCK_50    = 0x00FE6800,    /*   2KB: 0xFFFE6800 - 0xFFFE6FFF */
    FLASH_CF_BLOCK_51    = 0x00FE6000,    /*   2KB: 0xFFFE6000 - 0xFFFE67FF */
    FLASH_CF_BLOCK_52    = 0x00FE5800,    /*   2KB: 0xFFFE5800 - 0xFFFE5FFF */
    FLASH_CF_BLOCK_53    = 0x00FE5000,    /*   2KB: 0xFFFE5000 - 0xFFFE57FF */
    FLASH_CF_BLOCK_54    = 0x00FE4800,    /*   2KB: 0xFFFE4800 - 0xFFFE4FFF */
    FLASH_CF_BLOCK_55    = 0x00FE4000,    /*   2KB: 0xFFFE4000 - 0xFFFE47FF */
    FLASH_CF_BLOCK_56    = 0x00FE3800,    /*   2KB: 0xFFFE3800 - 0xFFFE3FFF */
    FLASH_CF_BLOCK_57    = 0x00FE3000,    /*   2KB: 0xFFFE3000 - 0xFFFE37FF */
    FLASH_CF_BLOCK_58    = 0x00FE2800,    /*   2KB: 0xFFFE2800 - 0xFFFE2FFF */
    FLASH_CF_BLOCK_59    = 0x00FE2000,    /*   2KB: 0xFFFE2000 - 0xFFFE27FF */
    FLASH_CF_BLOCK_60    = 0x00FE1800,    /*   2KB: 0xFFFE1800 - 0xFFFE1FFF */
    FLASH_CF_BLOCK_61    = 0x00FE1000,    /*   2KB: 0xFFFE1000 - 0xFFFE17FF */
    FLASH_CF_BLOCK_62    = 0x00FE0800,    /*   2KB: 0xFFFE0800 - 0xFFFE0FFF */
    FLASH_CF_BLOCK_63    = 0x00FE0000,    /*   2KB: 0xFFFE0000 - 0xFFFE07FF */
#if   MCU_CFG_PART_MEMORY_SIZE == 0x5   //   128KB ROM
    FLASH_CF_BLOCK_INVALID = 0x00FDFFFF,
#else
    FLASH_CF_BLOCK_64    = 0x00FDF800,    /*   2KB: 0xFFFDF800 - 0xFFFDFFFF */
    FLASH_CF_BLOCK_65    = 0x00FDF000,    /*   2KB: 0xFFFDF000 - 0xFFFDF7FF */
    FLASH_CF_BLOCK_66    = 0x00FDE800,    /*   2KB: 0xFFFDE800 - 0xFFFDEFFF */
    FLASH_CF_BLOCK_67    = 0x00FDE000,    /*   2KB: 0xFFFDE000 - 0xFFFDE7FF */
    FLASH_CF_BLOCK_68    = 0x00FDD800,    /*   2KB: 0xFFFDD800 - 0xFFFDDFFF */
    FLASH_CF_BLOCK_69    = 0x00FDD000,    /*   2KB: 0xFFFDD000 - 0xFFFDD7FF */
    FLASH_CF_BLOCK_70    = 0x00FDC800,    /*   2KB: 0xFFFDC800 - 0xFFFDCFFF */
    FLASH_CF_BLOCK_71    = 0x00FDC000,    /*   2KB: 0xFFFDC000 - 0xFFFDC7FF */
    FLASH_CF_BLOCK_72    = 0x00FDB800,    /*   2KB: 0xFFFDB800 - 0xFFFDBFFF */
    FLASH_CF_BLOCK_73    = 0x00FDB000,    /*   2KB: 0xFFFDB000 - 0xFFFDB7FF */
    FLASH_CF_BLOCK_74    = 0x00FDA800,    /*   2KB: 0xFFFDA800 - 0xFFFDAFFF */
    FLASH_CF_BLOCK_75    = 0x00FDA000,    /*   2KB: 0xFFFDA000 - 0xFFFDA7FF */
    FLASH_CF_BLOCK_76    = 0x00FD9800,    /*   2KB: 0xFFFD9800 - 0xFFFD9FFF */
    FLASH_CF_BLOCK_77    = 0x00FD9000,    /*   2KB: 0xFFFD9000 - 0xFFFD97FF */
    FLASH_CF_BLOCK_78    = 0x00FD8800,    /*   2KB: 0xFFFD8800 - 0xFFFD8FFF */
    FLASH_CF_BLOCK_79    = 0x00FD8000,    /*   2KB: 0xFFFD8000 - 0xFFFD87FF */
    FLASH_CF_BLOCK_80    = 0x00FD7800,    /*   2KB: 0xFFFD7800 - 0xFFFD7FFF */
    FLASH_CF_BLOCK_81    = 0x00FD7000,    /*   2KB: 0xFFFD7000 - 0xFFFD77FF */
    FLASH_CF_BLOCK_82    = 0x00FD6800,    /*   2KB: 0xFFFD6800 - 0xFFFD6FFF */
    FLASH_CF_BLOCK_83    = 0x00FD6000,    /*   2KB: 0xFFFD6000 - 0xFFFD67FF */
    FLASH_CF_BLOCK_84    = 0x00FD5800,    /*   2KB: 0xFFFD5800 - 0xFFFD5FFF */
    FLASH_CF_BLOCK_85    = 0x00FD5000,    /*   2KB: 0xFFFD5000 - 0xFFFD57FF */
    FLASH_CF_BLOCK_86    = 0x00FD4800,    /*   2KB: 0xFFFD4800 - 0xFFFD4FFF */
    FLASH_CF_BLOCK_87    = 0x00FD4000,    /*   2KB: 0xFFFD4000 - 0xFFFD47FF */
    FLASH_CF_BLOCK_88    = 0x00FD3800,    /*   2KB: 0xFFFD3800 - 0xFFFD3FFF */
    FLASH_CF_BLOCK_89    = 0x00FD3000,    /*   2KB: 0xFFFD3000 - 0xFFFD37FF */
    FLASH_CF_BLOCK_90    = 0x00FD2800,    /*   2KB: 0xFFFD2800 - 0xFFFD2FFF */
    FLASH_CF_BLOCK_91    = 0x00FD2000,    /*   2KB: 0xFFFD2000 - 0xFFFD27FF */
    FLASH_CF_BLOCK_92    = 0x00FD1800,    /*   2KB: 0xFFFD1800 - 0xFFFD1FFF */
    FLASH_CF_BLOCK_93    = 0x00FD1000,    /*   2KB: 0xFFFD1000 - 0xFFFD17FF */
    FLASH_CF_BLOCK_94    = 0x00FD0800,    /*   2KB: 0xFFFD0800 - 0xFFFD0FFF */
    FLASH_CF_BLOCK_95    = 0x00FD0000,    /*   2KB: 0xFFFD0000 - 0xFFFD07FF */
    FLASH_CF_BLOCK_96    = 0x00FCF800,    /*   2KB: 0xFFFCF800 - 0xFFFCFFFF */
    FLASH_CF_BLOCK_97    = 0x00FCF000,    /*   2KB: 0xFFFCF000 - 0xFFFCF7FF */
    FLASH_CF_BLOCK_98    = 0x00FCE800,    /*   2KB: 0xFFFCE800 - 0xFFFCEFFF */
    FLASH_CF_BLOCK_99    = 0x00FCE000,    /*   2KB: 0xFFFCE000 - 0xFFFCE7FF */
    FLASH_CF_BLOCK_100   = 0x00FCD800,    /*   2KB: 0xFFFCD800 - 0xFFFCDFFF */
    FLASH_CF_BLOCK_101   = 0x00FCD000,    /*   2KB: 0xFFFCD000 - 0xFFFCD7FF */
    FLASH_CF_BLOCK_102   = 0x00FCC800,    /*   2KB: 0xFFFCC800 - 0xFFFCCFFF */
    FLASH_CF_BLOCK_103   = 0x00FCC000,    /*   2KB: 0xFFFCC000 - 0xFFFCC7FF */
    FLASH_CF_BLOCK_104   = 0x00FCB800,    /*   2KB: 0xFFFCB800 - 0xFFFCBFFF */
    FLASH_CF_BLOCK_105   = 0x00FCB000,    /*   2KB: 0xFFFCB000 - 0xFFFCB7FF */
    FLASH_CF_BLOCK_106   = 0x00FCA800,    /*   2KB: 0xFFFCA800 - 0xFFFCAFFF */
    FLASH_CF_BLOCK_107   = 0x00FCA000,    /*   2KB: 0xFFFCA000 - 0xFFFCA7FF */
    FLASH_CF_BLOCK_108   = 0x00FC9800,    /*   2KB: 0xFFFC9800 - 0xFFFC9FFF */
    FLASH_CF_BLOCK_109   = 0x00FC9000,    /*   2KB: 0xFFFC9000 - 0xFFFC97FF */
    FLASH_CF_BLOCK_110   = 0x00FC8800,    /*   2KB: 0xFFFC8800 - 0xFFFC8FFF */
    FLASH_CF_BLOCK_111   = 0x00FC8000,    /*   2KB: 0xFFFC8000 - 0xFFFC87FF */
    FLASH_CF_BLOCK_112   = 0x00FC7800,    /*   2KB: 0xFFFC7800 - 0xFFFC7FFF */
    FLASH_CF_BLOCK_113   = 0x00FC7000,    /*   2KB: 0xFFFC7000 - 0xFFFC77FF */
    FLASH_CF_BLOCK_114   = 0x00FC6800,    /*   2KB: 0xFFFC6800 - 0xFFFC6FFF */
    FLASH_CF_BLOCK_115   = 0x00FC6000,    /*   2KB: 0xFFFC6000 - 0xFFFC67FF */
    FLASH_CF_BLOCK_116   = 0x00FC5800,    /*   2KB: 0xFFFC5800 - 0xFFFC5FFF */
    FLASH_CF_BLOCK_117   = 0x00FC5000,    /*   2KB: 0xFFFC5000 - 0xFFFC57FF */
    FLASH_CF_BLOCK_118   = 0x00FC4800,    /*   2KB: 0xFFFC4800 - 0xFFFC4FFF */
    FLASH_CF_BLOCK_119   = 0x00FC4000,    /*   2KB: 0xFFFC4000 - 0xFFFC47FF */
    FLASH_CF_BLOCK_120   = 0x00FC3800,    /*   2KB: 0xFFFC3800 - 0xFFFC3FFF */
    FLASH_CF_BLOCK_121   = 0x00FC3000,    /*   2KB: 0xFFFC3000 - 0xFFFC37FF */
    FLASH_CF_BLOCK_122   = 0x00FC2800,    /*   2KB: 0xFFFC2800 - 0xFFFC2FFF */
    FLASH_CF_BLOCK_123   = 0x00FC2000,    /*   2KB: 0xFFFC2000 - 0xFFFC27FF */
    FLASH_CF_BLOCK_124   = 0x00FC1800,    /*   2KB: 0xFFFC1800 - 0xFFFC1FFF */
    FLASH_CF_BLOCK_125   = 0x00FC1000,    /*   2KB: 0xFFFC1000 - 0xFFFC17FF */
    FLASH_CF_BLOCK_126   = 0x00FC0800,    /*   2KB: 0xFFFC0800 - 0xFFFC0FFF */
    FLASH_CF_BLOCK_127   = 0x00FC0000,    /*   2KB: 0xFFFC0000 - 0xFFFC07FF */
#if   MCU_CFG_PART_MEMORY_SIZE == 0x6   //   256KB ROM
    FLASH_CF_BLOCK_INVALID = 0x00FBFFFF,
#else
    FLASH_CF_BLOCK_INVALID = 0x0000FFF7FF,
#endif
#endif
#endif
#endif



    FLASH_DF_BLOCK_0       = 0x00100000,    /*   128: 0x00100000 - 0x0010007F */
    FLASH_DF_BLOCK_1       = 0x00100080,    /*   128: 0x00100080 - 0x001000FF */
    FLASH_DF_BLOCK_2       = 0x00100100,    /*   128: 0x00100100 - 0x0010017F */
    FLASH_DF_BLOCK_3       = 0x00100180,    /*   128: 0x00100180 - 0x001001FF */
    FLASH_DF_BLOCK_4       = 0x00100200,    /*   128: 0x00100200 - 0x0010027F */
    FLASH_DF_BLOCK_5       = 0x00100280,    /*   128: 0x00100280 - 0x001002FF */
    FLASH_DF_BLOCK_6       = 0x00100300,    /*   128: 0x00100300 - 0x0010037F */
    FLASH_DF_BLOCK_7       = 0x00100380,    /*   128: 0x00100380 - 0x001003FF */
    FLASH_DF_BLOCK_8       = 0x00100400,    /*   128: 0x00100400 - 0x0010047F */
    FLASH_DF_BLOCK_9       = 0x00100480,    /*   128: 0x00100480 - 0x001004FF */
    FLASH_DF_BLOCK_10      = 0x00100500,    /*   128: 0x00100500 - 0x0010057F */
    FLASH_DF_BLOCK_11      = 0x00100580,    /*   128: 0x00100580 - 0x001005FF */
    FLASH_DF_BLOCK_12      = 0x00100600,    /*   128: 0x00100600 - 0x0010067F */
    FLASH_DF_BLOCK_13      = 0x00100680,    /*   128: 0x00100680 - 0x001006FF */
    FLASH_DF_BLOCK_14      = 0x00100700,    /*   128: 0x00100700 - 0x0010077F */
    FLASH_DF_BLOCK_15      = 0x00100780,    /*   128: 0x00100780 - 0x001007FF */
    FLASH_DF_BLOCK_16      = 0x00100800,    /*   128: 0x00100800 - 0x0010087F */
    FLASH_DF_BLOCK_17      = 0x00100880,    /*   128: 0x00100880 - 0x001008FF */
    FLASH_DF_BLOCK_18      = 0x00100900,    /*   128: 0x00100900 - 0x0010097F */
    FLASH_DF_BLOCK_19      = 0x00100980,    /*   128: 0x00100980 - 0x001009FF */
    FLASH_DF_BLOCK_20      = 0x00100A00,    /*   128: 0x00100A00 - 0x00100A7F */
    FLASH_DF_BLOCK_21      = 0x00100A80,    /*   128: 0x00100A80 - 0x00100AFF */
    FLASH_DF_BLOCK_22      = 0x00100B00,    /*   128: 0x00100B00 - 0x00100B7F */
    FLASH_DF_BLOCK_23      = 0x00100B80,    /*   128: 0x00100B80 - 0x00100BFF */
    FLASH_DF_BLOCK_24      = 0x00100C00,    /*   128: 0x00100C00 - 0x00100C7F */
    FLASH_DF_BLOCK_25      = 0x00100C80,    /*   128: 0x00100C80 - 0x00100CFF */
    FLASH_DF_BLOCK_26      = 0x00100D00,    /*   128: 0x00100D00 - 0x00100D7F */
    FLASH_DF_BLOCK_27      = 0x00100D80,    /*   128: 0x00100D80 - 0x00100DFF */
    FLASH_DF_BLOCK_28      = 0x00100E00,    /*   128: 0x00100E00 - 0x00100E7F */
    FLASH_DF_BLOCK_29      = 0x00100E80,    /*   128: 0x00100E80 - 0x00100EFF */
    FLASH_DF_BLOCK_30      = 0x00100F00,    /*   128: 0x00100F00 - 0x00100F7F */
    FLASH_DF_BLOCK_31      = 0x00100F80,    /*   128: 0x00100F80 - 0x00100FFF */
    FLASH_DF_BLOCK_32      = 0x00101000,    /*   128: 0x00101000 - 0x0010107F */
    FLASH_DF_BLOCK_33      = 0x00101080,    /*   128: 0x00101080 - 0x001010FF */
    FLASH_DF_BLOCK_34      = 0x00101100,    /*   128: 0x00101100 - 0x0010117F */
    FLASH_DF_BLOCK_35      = 0x00101180,    /*   128: 0x00101180 - 0x001011FF */
    FLASH_DF_BLOCK_36      = 0x00101200,    /*   128: 0x00101200 - 0x0010127F */
    FLASH_DF_BLOCK_37      = 0x00101280,    /*   128: 0x00101280 - 0x001012FF */
    FLASH_DF_BLOCK_38      = 0x00101300,    /*   128: 0x00101300 - 0x0010137F */
    FLASH_DF_BLOCK_39      = 0x00101380,    /*   128: 0x00101380 - 0x001013FF */
    FLASH_DF_BLOCK_40      = 0x00101400,    /*   128: 0x00101400 - 0x0010147F */
    FLASH_DF_BLOCK_41      = 0x00101480,    /*   128: 0x00101480 - 0x001014FF */
    FLASH_DF_BLOCK_42      = 0x00101500,    /*   128: 0x00101500 - 0x0010157F */
    FLASH_DF_BLOCK_43      = 0x00101580,    /*   128: 0x00101580 - 0x001015FF */
    FLASH_DF_BLOCK_44      = 0x00101600,    /*   128: 0x00101600 - 0x0010167F */
    FLASH_DF_BLOCK_45      = 0x00101680,    /*   128: 0x00101680 - 0x001016FF */
    FLASH_DF_BLOCK_46      = 0x00101700,    /*   128: 0x00101700 - 0x0010177F */
    FLASH_DF_BLOCK_47      = 0x00101780,    /*   128: 0x00101780 - 0x001017FF */
    FLASH_DF_BLOCK_48      = 0x00101800,    /*   128: 0x00101800 - 0x0010187F */
    FLASH_DF_BLOCK_49      = 0x00101880,    /*   128: 0x00101880 - 0x001018FF */
    FLASH_DF_BLOCK_50      = 0x00101900,    /*   128: 0x00101900 - 0x0010197F */
    FLASH_DF_BLOCK_51      = 0x00101980,    /*   128: 0x00101980 - 0x001019FF */
    FLASH_DF_BLOCK_52      = 0x00101A00,    /*   128: 0x00101A00 - 0x00101A7F */
    FLASH_DF_BLOCK_53      = 0x00101A80,    /*   128: 0x00101A80 - 0x00101AFF */
    FLASH_DF_BLOCK_54      = 0x00101B00,    /*   128: 0x00101B00 - 0x00101B7F */
    FLASH_DF_BLOCK_55      = 0x00101B80,    /*   128: 0x00101B80 - 0x00101BFF */
    FLASH_DF_BLOCK_56      = 0x00101C00,    /*   128: 0x00101C00 - 0x00101C7F */
    FLASH_DF_BLOCK_57      = 0x00101C80,    /*   128: 0x00101C80 - 0x00101CFF */
    FLASH_DF_BLOCK_58      = 0x00101D00,    /*   128: 0x00101D00 - 0x00101D7F */
    FLASH_DF_BLOCK_59      = 0x00101D80,    /*   128: 0x00101D80 - 0x00101DFF */
    FLASH_DF_BLOCK_60      = 0x00101E00,    /*   128: 0x00101E00 - 0x00101E7F */
    FLASH_DF_BLOCK_61      = 0x00101E80,    /*   128: 0x00101E80 - 0x00101EFF */
    FLASH_DF_BLOCK_62      = 0x00101F00,    /*   128: 0x00101F00 - 0x00101F7F */
    FLASH_DF_BLOCK_63      = 0x00101F80,    /*   128: 0x00101F80 - 0x00101FFF */
    FLASH_DF_BLOCK_INVALID = 0x00102000     /*   2KB: Can't write beyond 0x00101FFF */

} flash_block_address_t;

typedef struct _rom_block_sizes
{
 uint32_t num_blocks;                     // number of blocks at this size
 uint32_t block_size;                     // Size of each block
}rom_block_sizes_t;

typedef struct _rom_block_info
{
 uint32_t start_addr;                   // starting address for this block section
 uint32_t end_addr;                     // ending (up to and including this) address
 uint16_t block_number;                 // the rom block number for this address queried
 uint32_t thisblock_stAddr;             // the starting address for the above block #
 uint32_t block_size;                   // Size of this block

}rom_block_info_t;

#define NUM_BLOCK_TABLE_ENTRIES 2
static rom_block_sizes_t g_flash_RomBlockSizes[NUM_BLOCK_TABLE_ENTRIES] = {
         512,  2048,    /* 512 blocks of 2K   */
        0,  0
};


/*  According to HW Manual the Max Programming Time for 128 bytes (ROM)
    is 6ms.  This is with a FCLK of 32MHz. The calculation below
    calculates the number of ICLK ticks needed for the timeout delay.
    The 6ms number is adjusted linearly depending on the FCLK frequency.
*/
#define WAIT_MAX_ROM_WRITE \
        ((int32_t)(6000 * (32.0/(MCU_CFG_FCLK_HZ/1000000)))*(MCU_CFG_ICLK_HZ/1000000))

/*  According to HW Manual the Max Programming Time for 8 bytes
    (Data Flash) is 3.2ms.  This is with a FCLK of 32MHz. The calculation
    below calculates the number of ICLK ticks needed for the timeout delay.
    The 3.2ms number is adjusted linearly depending on the FCLK frequency.
*/
#define WAIT_MAX_DF_WRITE \
        ((int32_t)(3200 * (32.0/(MCU_CFG_FCLK_HZ/1000000)))*(MCU_CFG_ICLK_HZ/1000000))

/*  According to HW Manual the Max Blank Check time for 2k bytes
    (Data Flash) is 2.5ms.  This is with a FCLK of 32MHz. The calculation
    below calculates the number of ICLK ticks needed for the timeout delay.
    The 2.5ms number is adjusted linearly depending on the FCLK frequency.
*/
#define WAIT_MAX_BLANK_CHECK \
        ((int32_t)(2500 * (32.0/(MCU_CFG_FCLK_HZ/1000000)))*(MCU_CFG_ICLK_HZ/1000000))
        
/*  According to HW Manual the max timeout value when using the peripheral
    clock notification command is 60us. This is with a FCLK of 32MHz. The 
    calculation below calculates the number of ICLK ticks needed for the 
    timeout delay. The 60us number is adjusted linearly depending on 
    the FCLK frequency.
*/
#define WAIT_MAX_NOTIFY_FCU_CLOCK \
        ((int32_t)(60 * (32.0/(MCU_CFG_FCLK_HZ/1000000)))*(MCU_CFG_ICLK_HZ/1000000))

/*  According to HW Manual the Max Erasure Time for a 2kB block is
    around 60ms.  This is with a FCLK of 32MHz. The calculation below
    calculates the number of ICLK ticks needed for the timeout delay.
    The 60ms number is adjusted linearly depending on the FCLK frequency.
*/
#define WAIT_MAX_ERASE \
        ((int32_t)(60000 * (32.0/(MCU_CFG_FCLK_HZ/1000000)))*(MCU_CFG_ICLK_HZ/1000000))

/******************************************************************************
Error checking
******************************************************************************/
/* FCLK must be between 4MHz and 32MHz. */
#if (MCU_CFG_FCLK_HZ > 32000000) || (MCU_CFG_FCLK_HZ < 4000000)
    #error "ERROR - Flash API - FCLK on RX220 must be between 4MHz and 32MHz!"
#endif

#endif /* FLASH_API_RX220_H */
