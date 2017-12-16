/**
  ******************************************************************************
  * @file    mx25lm51245g.h
  * @author  MCD Application Team
  * @brief   This file contains all the description of the MX25LM51245G Octal memory.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2017 STMicroelectronics</center></h2>
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */ 

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MX25LM51245G_H
#define __MX25LM51245G_H

#ifdef __cplusplus
 extern "C" {
#endif 

/* Includes ------------------------------------------------------------------*/

/** @addtogroup BSP
  * @{
  */ 

/** @addtogroup Components
  * @{
  */ 
  
/** @addtogroup MX25LM51245G
  * @{
  */

/** @defgroup MX25LM51245G_Exported_Types
  * @{
  */
   
/**
  * @}
  */ 

/** @defgroup MX25LM51245G_Exported_Constants
  * @{
  */
   
/** 
  * @brief  MX25LM51245G Configuration  
  */  
#define MX25LM51245G_FLASH_SIZE                   0x4000000 /* 512 MBits => 64 MBytes */
#define MX25LM51245G_BLOCK_SIZE                   0x10000   /* 1024 blocks of 64 KBytes */
#define MX25LM51245G_SECTOR_SIZE                  0x1000    /* 16384 sectors of 4 kBytes */
#define MX25LM51245G_PAGE_SIZE                    0x100     /* 262144 pages of 256 bytes */

#define MX25LM51245G_DUMMY_CYCLES_READ            8
#define MX25LM51245G_DUMMY_CYCLES_READ_OCTAL_66M  6
#define MX25LM51245G_DUMMY_CYCLES_READ_OCTAL_84M  8
#define MX25LM51245G_DUMMY_CYCLES_READ_OCTAL_104M 10
#define MX25LM51245G_DUMMY_CYCLES_READ_OCTAL_133M 14

#define MX25LM51245G_CR2_DC_66M                   MX25LM51245G_CR2_DC_6_CYCLES
#define MX25LM51245G_CR2_DC_84M                   MX25LM51245G_CR2_DC_8_CYCLES
#define MX25LM51245G_CR2_DC_104M                  MX25LM51245G_CR2_DC_10_CYCLES
#define MX25LM51245G_CR2_DC_133M                  MX25LM51245G_CR2_DC_14_CYCLES

#define MX25LM51245G_CHIP_ERASE_MAX_TIME          300000
#define MX25LM51245G_BLOCK_ERASE_MAX_TIME         2000
#define MX25LM51245G_SECTOR_ERASE_MAX_TIME        400
#define MX25LM51245G_WRITE_REG_MAX_TIME           40

/** 
  * @brief  MX25LM51245G Commands  
  */  
/* Read Operations */
#define READ_CMD                                  0x03
#define READ_4_BYTE_ADDR_CMD                      0x13

#define FAST_READ_CMD                             0x0B
#define FAST_READ_4_BYTE_ADDR_CMD                 0x0C

#define OCTAL_IO_READ_CMD                         0xEC13
#define OCTAL_IO_DTR_READ_CMD                     0xEE11

/* Program Operations */
#define PAGE_PROG_CMD                             0x02
#define PAGE_PROG_4_BYTE_ADDR_CMD                 0x12

#define OCTAL_PAGE_PROG_CMD                       0x12ED

/* Erase Operations */
#define SECTOR_ERASE_CMD                          0x20
#define SECTOR_ERASE_4_BYTE_ADDR_CMD              0x21

#define OCTAL_SECTOR_ERASE_CMD                    0x21DE

#define BLOCK_ERASE_CMD                           0xD8
#define BLOCK_ERASE_4_BYTE_ADDR_CMD               0xDC

#define OCTAL_BLOCK_ERASE_CMD                     0xDC23

#define CHIP_ERASE_CMD                            0x60
#define CHIP_ERASE_CMD_2                          0xC7

#define OCTAL_CHIP_ERASE_CMD                      0x609F
#define OCTAL_CHIP_ERASE_CMD_2                    0xC738

#define PROG_ERASE_RESUME_CMD                     0x30
#define PROG_ERASE_SUSPEND_CMD                    0xB0

#define OCTAL_PROG_ERASE_RESUME_CMD               0x30CF
#define OCTAL_PROG_ERASE_SUSPEND_CMD              0xB04F

/* Identification Operations */
#define READ_ID_CMD                               0x9F
#define READ_SERIAL_FLASH_DISCO_PARAM_CMD         0x5A

#define OCTAL_READ_ID_CMD                         0x9F60
#define OCTAL_READ_SERIAL_FLASH_DISCO_PARAM_CMD   0x5AA5  

/* Write Operations */
#define WRITE_ENABLE_CMD                          0x06
#define WRITE_DISABLE_CMD                         0x04

#define OCTAL_WRITE_ENABLE_CMD                    0x06F9
#define OCTAL_WRITE_DISABLE_CMD                   0x04FB

/* Register Operations */
#define READ_STATUS_REG_CMD                       0x05
#define READ_CFG_REG_CMD                          0x15
#define WRITE_STATUS_CFG_REG_CMD                  0x01

#define OCTAL_READ_STATUS_REG_CMD                 0x05FA
#define OCTAL_READ_CFG_REG_CMD                    0x15EA
#define OCTAL_WRITE_STATUS_CFG_REG_CMD            0x01FE

#define READ_CFG_REG_2_CMD                        0x71
#define WRITE_CFG_REG_2_CMD                       0x72

#define OCTAL_READ_CFG_REG_2_CMD                  0x718E
#define OCTAL_WRITE_CFG_REG_2_CMD                 0x728D

#define READ_FAST_BOOT_REG_CMD                    0x16
#define WRITE_FAST_BOOT_REG                       0x17
#define ERASE_FAST_BOOT_REG                       0x18

#define OCTAL_READ_FAST_BOOT_REG_CMD              0x16E9
#define OCTAL_WRITE_FAST_BOOT_REG                 0x17E8
#define OCTAL_ERASE_FAST_BOOT_REG                 0x18E7

#define READ_SEC_REG_CMD                          0x2B
#define WRITE_SEC_REG_CMD                         0x2F

#define OCTAL_READ_SECURITY_REG_CMD               0x2BD4
#define OCTAL_WRITE_SECURITY_REG_CMD              0x2FD0

#define READ_LOCK_REG_CMD                         0x2D
#define WRITE_LOCK_REG_CMD                        0x2C

#define OCTAL_READ_LOCK_REG_CMD                   0x2DD2
#define OCTAL_WRITE_LOCK_REG_CMD                  0x2CD3

#define READ_SPB_STATUS_CMD                       0xE2
#define PROG_SPB_BIT_CMD                          0xE3
#define ERASE_ALL_SPB_BIT_CMD                     0xE4

#define OCTAL_READ_SPB_STATUS_CMD                 0xE21D
#define OCTAL_PROG_SPB_BIT_CMD                    0xE31C
#define OCTAL_ERASE_ALL_SPB_BIT_CMD               0xE41B

#define READ_DPB_REG_CMD                          0xE0
#define WRITE_DPB_REG_CMD                         0xE1

#define OCTAL_READ_DPB_REG_CMD                    0xE01F
#define OCTAL_WRITE_DPB_REG_CMD                   0xE11E

/* Power Down Operations */
#define DEEP_POWER_DOWN_CMD                       0xB9
#define RELEASE_DEEP_POWER_DOWN_CMD               0xAB

#define OCTAL_ENTER_DEEP_POWER_DOWN_CMD           0xB946

/* Burst Operations */
#define SET_BURST_LENGTH_CMD                      0xC0

#define OCTAL_SET_BURST_LENGTH_CMD                0xC03F

/* One-Time Programmable Operations */
#define ENTER_SECURED_OTP_CMD                     0xB1
#define EXIT_SECURED_OTP_CMD                      0xC1

#define OCTAL_ENTER_SECURED_OTP_CMD               0xB14E
#define OCTAL_EXIT_SECURED_OTP_CMD                0xC13E

/* No Operation */
#define NO_OPERATION_CMD                          0x00

#define OCTAL_NO_OPERATION_CMD                    0x00FF

/* Reset Operations */
#define RESET_ENABLE_CMD                          0x66
#define RESET_MEMORY_CMD                          0x99

#define OCTAL_RESET_ENABLE_CMD                    0x6699
#define OCTAL_RESET_MEMORY_CMD                    0x9966

/* Protection Operations */
#define WRITE_PROTECT_SEL_CMD                     0x68

#define OCTAL_WRITE_PROTECT_SEL_CMD               0x6897

#define GANG_BLOCK_LOCK_CMD                       0x7E
#define GANG_BLOCK_UNLOCK_CMD                     0x98

#define OCTAL_GANG_BLOCK_LOCK_CMD                 0x7E81
#define OCTAL_GANG_BLOCK_UNLOCK_CMD               0x9867

/** 
  * @brief  MX25LM51245G Registers  
  */ 
/* Status Register */
#define MX25LM51245G_SR_WIP                       ((uint8_t)0x01)        /*!< Write in progress */
#define MX25LM51245G_SR_WEL                       ((uint8_t)0x02)        /*!< Write enable latch */
#define MX25LM51245G_SR_BP                        ((uint8_t)0x3C)        /*!< Block protect */

/* Configuration Register 1 */
#define MX25LM51245G_CR1_ODS                      ((uint8_t)0x07)        /*!< Output driver strength */
#define MX25LM51245G_CR1_TB                       ((uint8_t)0x08)        /*!< Top / bottom */

/* Configuration Register 2 */
/* Address : 0x00000000 */
#define MX25LM51245G_CR2_REG1_ADDR                ((uint32_t)0x00000000) /*!< CR2 register address 0x00000000 */
#define MX25LM51245G_CR2_SOPI                     ((uint8_t)0x01)        /*!< STR OPI Enable */
#define MX25LM51245G_CR2_DOPI                     ((uint8_t)0x02)        /*!< DTR OPI Enable */
/* Address : 0x00000200 */
#define MX25LM51245G_CR2_REG2_ADDR                ((uint32_t)0x00000200) /*!< CR2 register address 0x00000200 */
#define MX25LM51245G_CR2_DQSPRC                   ((uint8_t)0x01)        /*!< DTR DQS pre-cycle */
#define MX25LM51245G_CR2_DOS                      ((uint8_t)0x02)        /*!< DQS on STR mode */
/* Address : 0x00000300 */
#define MX25LM51245G_CR2_REG3_ADDR                ((uint32_t)0x00000300) /*!< CR2 register address 0x00000300 */
#define MX25LM51245G_CR2_DC                       ((uint8_t)0x07)        /*!< Dummy cycle */
#define MX25LM51245G_CR2_DC_20_CYCLES             ((uint8_t)0x00)        /*!< 20 Dummy cycles */
#define MX25LM51245G_CR2_DC_18_CYCLES             ((uint8_t)0x01)        /*!< 18 Dummy cycles */
#define MX25LM51245G_CR2_DC_16_CYCLES             ((uint8_t)0x02)        /*!< 16 Dummy cycles */
#define MX25LM51245G_CR2_DC_14_CYCLES             ((uint8_t)0x03)        /*!< 14 Dummy cycles */
#define MX25LM51245G_CR2_DC_12_CYCLES             ((uint8_t)0x04)        /*!< 12 Dummy cycles */
#define MX25LM51245G_CR2_DC_10_CYCLES             ((uint8_t)0x05)        /*!< 10 Dummy cycles */
#define MX25LM51245G_CR2_DC_8_CYCLES              ((uint8_t)0x06)        /*!< 8 Dummy cycles */
#define MX25LM51245G_CR2_DC_6_CYCLES              ((uint8_t)0x07)        /*!< 6 Dummy cycles */
/* Address : 0x00000500 */
#define MX25LM51245G_CR2_REG4_ADDR                ((uint32_t)0x00000500) /*!< CR2 register address 0x00000500 */
#define MX25LM51245G_CR2_PPTSEL                   ((uint8_t)0x01)        /*!< Preamble pattern selection */
/* Address : 0x40000000 */
#define MX25LM51245G_CR2_REG5_ADDR                ((uint32_t)0x40000000) /*!< CR2 register address 0x40000000 */
#define MX25LM51245G_CR2_DEFSOPI                  ((uint8_t)0x01)        /*!< Enable SOPI after power on reset */
#define MX25LM51245G_CR2_DEFDOPI                  ((uint8_t)0x02)        /*!< Enable DOPI after power on reset */

/* Security Register */
#define MX25LM51245G_SECR_SOI                     ((uint8_t)0x01)        /*!< Secured OTP indicator */
#define MX25LM51245G_SECR_LDSO                    ((uint8_t)0x02)        /*!< Lock-down secured OTP */
#define MX25LM51245G_SECR_PSB                     ((uint8_t)0x04)        /*!< Program suspend bit */
#define MX25LM51245G_SECR_ESB                     ((uint8_t)0x08)        /*!< Erase suspend bit */
#define MX25LM51245G_SECR_P_FAIL                  ((uint8_t)0x20)        /*!< Program fail flag */
#define MX25LM51245G_SECR_E_FAIL                  ((uint8_t)0x40)        /*!< Erase fail flag */
#define MX25LM51245G_SECR_WPSEL                   ((uint8_t)0x40)        /*!< Write protection selection */
/**
  * @}
  */
  
/** @defgroup MX25LM51245G_Exported_Functions
  * @{
  */ 
/**
  * @}
  */ 
      
#ifdef __cplusplus
}
#endif

#endif /* __MX25LM51245G_H */

/**
  * @}
  */ 

/**
  * @}
  */ 

/**
  * @}
  */
  
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
