/**
 ******************************************************************************
 * @file    lps22hb.h
 * @author  MCD Application Team
 * @version V1.0.0
 * @date    14-February-2017
 * @brief   LPS22HB header driver file
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
#ifndef __LPS22HB__H
#define __LPS22HB__H

#ifdef __cplusplus
extern "C" {
#endif
  
/* Includes ------------------------------------------------------------------*/
#include "../Common/psensor.h"
#include "../Common/tsensor.h"  

/** @addtogroup BSP
  * @{
  */

/** @addtogroup Component
  * @{
  */

/** @addtogroup LPS22HB
  * @{
  */

/** @defgroup LPS22HB_Exported_Macros LPS22HB Exported Macros
  * @{
  */
  
/**
* @brief  Bitfield positioning.
*/
#define LPS22HB_BIT(x) ((uint8_t)x)
/**
  * @}
  */ 


/** @defgroup LPS22HB_Exported_Constants LPS22HB Exported Constants
  * @{
  */

/**
  * @brief Device Identification register.
  *        Read
  *        Default value: 0xB1
  *        7:0 This read-only register contains the device identifier that, for LPS22HB, is set to B1h.
  */

#define LPS22HB_WHO_AM_I_REG         (uint8_t)0x0F

/**
  * @brief Device Identification value.
  */
#define LPS22HB_WHO_AM_I_VAL         (uint8_t)0xB1

/**
  * @brief Reference Pressure  Register(LSB data)
  *        Read/write
  *        Default value: 0x00
  *        7:0 REFL7-0: Lower part of the reference pressure value that
  *             is sum to the sensor output pressure.
  */
#define LPS22HB_REF_P_XL_REG         (uint8_t)0x15

/**
  * @brief Reference Pressure Register (Middle data)
  *        Read/write
  *        Default value: 0x00
  *        7:0 REFL15-8: Middle part of the reference pressure value that
  *             is sum to the sensor output pressure.
  */
#define LPS22HB_REF_P_L_REG          (uint8_t)0x16

/**
  * @brief Reference Pressure Register (MSB data)
  *        Read/write
  *        Default value: 0x00
  *        7:0 REFL23-16 Higest part of the reference pressure value that
  *             is sum to the sensor output pressure.
  */
#define LPS22HB_REF_P_H_REG          (uint8_t)0x17

/**
  * @brief Pressure and temperature resolution mode Register
  *        Read/write
  *        Default value: 0x05
  *        7:2 These bits must be set to 0 for proper operation of the device
  *        1: Reserved
  *        0 LC_EN: Low Current Mode Enable. Default 0
  */
#define LPS22HB_RES_CONF_REG     (uint8_t)0x1A
#define LPS22HB_LCEN_MASK        (uint8_t)0x01

/**
  * @brief Control Register 1
  *        Read/write
  *        Default value: 0x00
  *        7: This bit must be set to 0 for proper operation of the device
  *        6:4 ODR2, ODR1, ODR0: output data rate selection.Default 000
  *            ODR2  | ODR1  | ODR0  | Pressure output data-rate(Hz)  | Pressure output data-rate(Hz)
  *          ----------------------------------------------------------------------------------
  *             0    |  0    |  0    |         one shot               |         one shot
  *             0    |  0    |  1    |            1                   |            1
  *             0    |  1    |  0    |            10                  |           10
  *             0    |  1    |  1    |            25                  |           25
  *             1    |  0    |  0    |            50                  |           50
  *             1    |  0    |  1    |            75                  |         75
  *             1    |  1    |  0    |         Reserved               |         Reserved
  *             1    |  1    |  1    |         Reserved               |         Reserved
  *        
  *        3 EN_LPFP: Enable Low Pass filter on Pressure data. Default value:0
  *        2:LPF_CFG Low-pass configuration register. (0: Filter cutoff is ODR/9; 1: filter cutoff is ODR/20)
  *        1 BDU: block data update. 0 - continuous update; 1 - output registers not updated until MSB and LSB reading.
  *        0 SIM: SPI Serial Interface Mode selection. 0 - SPI 4-wire; 1 - SPI 3-wire
  */
#define LPS22HB_CTRL_REG1      (uint8_t)0x10

#define LPS22HB_ODR_MASK                (uint8_t)0x70
#define LPS22HB_LPFP_MASK               (uint8_t)0x08
#define LPS22HB_LPFP_CUTOFF_MASK        (uint8_t)0x04
#define LPS22HB_BDU_MASK                (uint8_t)0x02
#define LPS22HB_SIM_MASK                (uint8_t)0x01

#define LPS22HB_LPFP_BIT    LPS22HB_BIT(3)

/**
  * @brief Control  Register 2
  *        Read/write
  *        Default value: 0x10
  *        7 BOOT:  Reboot memory content. 0: normal mode; 1: reboot memory content. Self-clearing upon completation
  *        6 FIFO_EN: FIFO Enable. 0: disable; 1:  enable
  *        5 STOP_ON_FTH: Stop on FIFO Threshold  FIFO Watermark level use. 0: disable; 1: enable
  *        4 IF_ADD_INC: Register address automatically incrementeed during a multiple byte access with a serial interface (I2C or SPI). 
  *                      Default value 1.( 0: disable; 1: enable)
  *        3 I2C DIS:  Disable I2C interface 0: I2C Enabled; 1: I2C disabled
  *        2 SWRESET: Software reset. 0: normal mode; 1: SW reset. Self-clearing upon completation
  *        1 AUTO_ZERO: Autozero enable. 0: normal mode; 1: autozero enable.
  *        0 ONE_SHOT: One shot enable. 0: waiting for start of conversion; 1: start for a new dataset
  */
#define LPS22HB_CTRL_REG2      (uint8_t)0x11

#define LPS22HB_BOOT_BIT       LPS22HB_BIT(7)
#define LPS22HB_FIFO_EN_BIT    LPS22HB_BIT(6)
#define LPS22HB_WTM_EN_BIT     LPS22HB_BIT(5)
#define LPS22HB_ADD_INC_BIT    LPS22HB_BIT(4)
#define LPS22HB_I2C_BIT        LPS22HB_BIT(3)
#define LPS22HB_SW_RESET_BIT   LPS22HB_BIT(2)

#define LPS22HB_FIFO_EN_MASK   (uint8_t)0x40
#define LPS22HB_WTM_EN_MASK    (uint8_t)0x20
#define LPS22HB_ADD_INC_MASK   (uint8_t)0x10
#define LPS22HB_I2C_MASK       (uint8_t)0x08
#define LPS22HB_ONE_SHOT_MASK  (uint8_t)0x01


/**
  * @brief CTRL Reg3 Interrupt Control Register
  *        Read/write
  *        Default value: 0x00
  *        7 INT_H_L: Interrupt active high, low. 0:active high; 1: active low.
  *        6 PP_OD: Push-Pull/OpenDrain selection on interrupt pads. 0: Push-pull; 1: open drain.
  *        5 F_FSS5: FIFO full flag on INT_DRDY pin. Defaul value: 0. (0: Diasable; 1 : Enable).
  *        4 F_FTH: FIFO threshold (watermark) status on INT_DRDY pin. Defaul value: 0. (0: Diasable; 1 : Enable).
  *        3 F_OVR: FIFO overrun interrupt on INT_DRDY pin. Defaul value: 0. (0: Diasable; 1 : Enable).
  *        2 DRDY: Data-ready signal on INT_DRDY pin. Defaul value: 0. (0: Diasable; 1 : Enable).
  *        1:0 INT_S2, INT_S1: data signal on INT pad control bits.
  *           INT_S2  | INT_S1  | INT pin
  *          ------------------------------------------------------
  *               0       |      0      |     Data signal( in order of priority:PTH_DRDY or F_FTH or F_OVR_or F_FSS5
  *               0       |      1      |     Pressure high (P_high)
  *               1       |      0      |     Pressure low (P_low)
  *               1       |      1      |     P_low OR P_high
  */
#define LPS22HB_CTRL_REG3      (uint8_t)0x12

#define LPS22HB_PP_OD_BIT       LPS22HB_BIT(6)
#define LPS22HB_FIFO_FULL_BIT   LPS22HB_BIT(5)
#define LPS22HB_FIFO_FTH_BIT    LPS22HB_BIT(4)
#define LPS22HB_FIFO_OVR_BIT    LPS22HB_BIT(3)
#define LPS22HB_DRDY_BIT        LPS22HB_BIT(2)


#define LPS22HB_INT_H_L_MASK            (uint8_t)0x80
#define LPS22HB_PP_OD_MASK              (uint8_t)0x40
#define LPS22HB_FIFO_FULL_MASK          (uint8_t)0x20
#define LPS22HB_FIFO_FTH_MASK           (uint8_t)0x10
#define LPS22HB_FIFO_OVR_MASK           (uint8_t)0x08
#define LPS22HB_DRDY_MASK               (uint8_t)0x04
#define LPS22HB_INT_S12_MASK            (uint8_t)0x03


/**
  * @brief Interrupt Differential configuration Register
  *        Read/write
  *        Default value: 0x00.
  *        7 AUTORIFP: AutoRifP Enable
  *        6 RESET_ARP: Reset AutoRifP function
  *        4 AUTOZERO: Autozero enabled
  *        5 RESET_AZ: Reset Autozero Function
  *        3 DIFF_EN: Interrupt generation enable
  *        2 LIR: Latch Interrupt request into INT_SOURCE register. 0 - interrupt request not latched
  *                                                                 1 - interrupt request latched
  *        1 PL_E: Enable interrupt generation on differential pressure low event. 0 - disable; 1 - enable
  *        0 PH_E: Enable interrupt generation on differential pressure high event. 0 - disable; 1 - enable
  */
#define LPS22HB_INTERRUPT_CFG_REG  (uint8_t)0x0B

#define LPS22HB_DIFF_EN_BIT       LPS22HB_BIT(3)
#define LPS22HB_LIR_BIT           LPS22HB_BIT(2)
#define LPS22HB_PLE_BIT           LPS22HB_BIT(1)
#define LPS22HB_PHE_BIT           LPS22HB_BIT(0)

#define LPS22HB_AUTORIFP_MASK     (uint8_t)0x80
#define LPS22HB_RESET_ARP_MASK    (uint8_t)0x40
#define LPS22HB_AUTOZERO_MASK     (uint8_t)0x20
#define LPS22HB_RESET_AZ_MASK     (uint8_t)0x10
#define LPS22HB_DIFF_EN_MASK      (uint8_t)0x08
#define LPS22HB_LIR_MASK          (uint8_t)0x04
#define LPS22HB_PLE_MASK          (uint8_t)0x02
#define LPS22HB_PHE_MASK          (uint8_t)0x01



/**
  * @brief Interrupt source Register (It is cleared by reading it)
  *        Read
  *        7 BOOT_STATUS:  If 1 indicates that the Boot (Reboot) phase is running.
  *        6:3 Reserved: Keep these bits at 0
  *        2 IA: Interrupt Active.0: no interrupt has been generated
  *                               1: one or more interrupt events have been generated.
  *        1 PL: Differential pressure Low. 0: no interrupt has been generated
  *                                         1: Low differential pressure event has occurred.
  *        0 PH: Differential pressure High. 0: no interrupt has been generated
  *                                          1: High differential pressure event has occurred.
  */
#define LPS22HB_INTERRUPT_SOURCE_REG   (uint8_t)0x25

#define LPS22HB_BOOT_STATUS_BIT        LPS22HB_BIT(7)
#define LPS22HB_IA_BIT                 LPS22HB_BIT(2)
#define LPS22HB_PL_BIT                 LPS22HB_BIT(1)
#define LPS22HB_PH_BIT                 LPS22HB_BIT(0)

#define LPS22HB_BOOT_STATUS_MASK      (uint8_t)0x80
#define LPS22HB_IA_MASK               (uint8_t)0x04
#define LPS22HB_PL_MASK               (uint8_t)0x02
#define LPS22HB_PH_MASK               (uint8_t)0x01


/**
  * @brief  Status Register
  *         Read
  *         7:6 Reserved: 0
  *         5 T_OR: Temperature data overrun. 0: no overrun has occurred
  *                                           1: a new data for temperature has overwritten the previous one.
  *         4 P_OR: Pressure data overrun. 0: no overrun has occurred
  *                                        1: new data for pressure has overwritten the previous one.
  *         3:2 Reserved: 0
  *         1 T_DA: Temperature data available. 0: new data for temperature is not yet available
  *                                             1: new data for temperature is available.
  *         0 P_DA: Pressure data available. 0: new data for pressure is not yet available
  *                                          1: new data for pressure is available.
  */
#define LPS22HB_STATUS_REG         (uint8_t)0x27

#define LPS22HB_TOR_BIT            LPS22HB_BIT(5)
#define LPS22HB_POR_BIT            LPS22HB_BIT(4)
#define LPS22HB_TDA_BIT            LPS22HB_BIT(1)
#define LPS22HB_PDA_BIT            LPS22HB_BIT(0)

#define LPS22HB_TOR_MASK           (uint8_t)0x20
#define LPS22HB_POR_MASK           (uint8_t)0x10
#define LPS22HB_TDA_MASK           (uint8_t)0x02
#define LPS22HB_PDA_MASK           (uint8_t)0x01


/**
  * @brief  Pressure data (LSB) register.
  *         Read
  *         Default value: 0x00.(To be verified)
  *         POUT7 - POUT0: Pressure data LSB (2's complement).
  *         Pressure output data: Pout(hPA)=(PRESS_OUT_H & PRESS_OUT_L &
  *         PRESS_OUT_XL)[dec]/4096.
  */
#define LPS22HB_PRESS_OUT_XL_REG        (uint8_t)0x28

/**
* @brief  Pressure data (Middle part) register.
*         Read
*         Default value: 0x80.
*         POUT15 - POUT8: Pressure data middle part (2's complement).
*         Pressure output data: Pout(hPA)=(PRESS_OUT_H & PRESS_OUT_L &
*         PRESS_OUT_XL)[dec]/4096.
*/
#define LPS22HB_PRESS_OUT_L_REG        (uint8_t)0x29

/**
  * @brief  Pressure data (MSB) register.
  *         Read
  *         Default value: 0x2F.
  *         POUT23 - POUT16: Pressure data MSB (2's complement).
  *         Pressure output data: Pout(hPA)=(PRESS_OUT_H & PRESS_OUT_L &
  *         PRESS_OUT_XL)[dec]/4096.
  */
#define LPS22HB_PRESS_OUT_H_REG        (uint8_t)0x2A

/**
  * @brief  Temperature data (LSB) register.
  *         Read
  *         Default value: 0x00.
  *         TOUT7 - TOUT0: temperature data LSB.
  *         Tout(degC)=TEMP_OUT/100
  */
#define LPS22HB_TEMP_OUT_L_REG         (uint8_t)0x2B

/**
  * @brief  Temperature data (MSB) register.
  *         Read
  *         Default value: 0x00.
  *         TOUT15 - TOUT8: temperature data MSB.
  *         Tout(degC)=TEMP_OUT/100
  */
#define LPS22HBH_TEMP_OUT_H_REG         (uint8_t)0x2C

/**
  * @brief Threshold pressure (LSB) register.
  *        Read/write
  *        Default value: 0x00.
  *        7:0 THS7-THS0: LSB Threshold pressure Low part of threshold value for pressure interrupt
  *        generation. The complete threshold value is given by THS_P_H & THS_P_L and is
  *        expressed as unsigned number. P_ths(hPA)=(THS_P_H & THS_P_L)[dec]/16.
  */
#define LPS22HB_THS_P_LOW_REG           (uint8_t)0x0C

/**
  * @brief Threshold pressure (MSB)
  *        Read/write
  *        Default value: 0x00.
  *        7:0 THS15-THS8: MSB Threshold pressure. High part of threshold value for pressure interrupt
  *        generation. The complete threshold value is given by THS_P_H & THS_P_L and is
  *        expressed as unsigned number. P_ths(mbar)=(THS_P_H & THS_P_L)[dec]/16.
  */
#define LPS22HB_THS_P_HIGH_REG         (uint8_t)0x0D

/**
  * @brief FIFO control register
  *        Read/write
  *        Default value: 0x00
  *        7:5 F_MODE2, F_MODE1, F_MODE0: FIFO mode selection.
  *            FM2   | FM1   | FM0   |    FIFO MODE
  *          ---------------------------------------------------
  *             0    |  0    |  0    | BYPASS MODE
  *             0    |  0    |  1    | FIFO MODE. Stops collecting data when full
  *             0    |  1    |  0    | STREAM MODE: Keep the newest measurements in the FIFO
  *             0    |  1    |  1    | STREAM MODE until trigger deasserted, then change to FIFO MODE
  *             1    |  0    |  0    | BYPASS MODE until trigger deasserted, then STREAM MODE
  *             1    |  0    |  1    | Reserved for future use
  *             1    |  1    |  0    | Reserved
  *             1    |  1    |  1    | BYPASS mode until trigger deasserted, then FIFO MODE
  *       
  *        4:0 WTM_POINT4-0 : FIFO Watermark level selection (0-31)
  */
#define LPS22HB_CTRL_FIFO_REG          (uint8_t)0x14

#define LPS22HB_FIFO_MODE_MASK        (uint8_t)0xE0
#define LPS22HB_WTM_POINT_MASK        (uint8_t)0x1F


/**
  * @brief FIFO Status register
  *        Read
  *        7 FTH_FIFO: FIFO threshold status. 0:FIFO filling is lower than FTH level
  *                                           1: FIFO is equal or higher than FTH level.
  *        6 OVR: Overrun bit status. 0 - FIFO not full
  *                                   1 - FIFO is full and at least one sample in the FIFO has been overwritten.
  *        5:0 FSS: FIFO Stored data level. 000000: FIFO empty, 100000: FIFO is full and has 32 unread samples.
  */
#define LPS22HB_STATUS_FIFO_REG        (uint8_t)0x26

#define LPS22HB_FTH_FIFO_BIT          LPS22HB_BIT(7)
#define LPS22HB_OVR_FIFO_BIT          LPS22HB_BIT(6)

#define LPS22HB_FTH_FIFO_MASK         (uint8_t)0x80
#define LPS22HB_OVR_FIFO_MASK         (uint8_t)0x40
#define LPS22HB_LEVEL_FIFO_MASK       (uint8_t)0x3F
#define LPS22HB_FIFO_EMPTY            (uint8_t)0x00
#define LPS22HB_FIFO_FULL             (uint8_t)0x20



/**
  * @brief Pressure offset register  (LSB)
  *        Read/write
  *        Default value: 0x00
  *        7:0 RPDS7-0:Pressure Offset for 1 point calibration (OPC) after soldering.
  *        This register contains the low part of the pressure offset value after soldering,for
  *        differential pressure computing. The complete value is given by RPDS_L & RPDS_H
  *        and is expressed as signed 2 complement value.
  */
#define LPS22HB_RPDS_L_REG        (uint8_t)0x18

/**
  * @brief Pressure offset register (MSB)
  *        Read/write
  *        Default value: 0x00
  *        7:0 RPDS15-8:Pressure Offset for 1 point calibration  (OPC) after soldering.
  *        This register contains the high part of the pressure offset value after soldering (see description RPDS_L)
  */
#define LPS22HB_RPDS_H_REG        (uint8_t)0x19


/**
  * @brief Clock Tree Configuration register
  *        Read/write
  *        Default value: 0x00
  *        7:6 Reserved.
  *        5: CTE: Clock Tree Enhancement
  */
#define LPS22HB_CLOCK_TREE_CONFIGURATION        (uint8_t)0x43
#define LPS22HB_CTE_MASK                        (uint8_t)0x20

/**
* @}
*/


/** @defgroup LPS22HB_Pressure_Exported_Functions LPS22HB Pressure Exported Functions
  * @{
  */  
/* PRESSURE functions */
void    LPS22HB_P_Init(uint16_t DeviceAddr);
uint8_t LPS22HB_P_ReadID(uint16_t DeviceAddr);
float   LPS22HB_P_ReadPressure(uint16_t DeviceAddr);
/**
  * @}
  */

/** @defgroup HTS221_PressImported_Globals  PRESSURE Imported Globals
  * @{
  */
/* PRESSURE driver structure */
extern PSENSOR_DrvTypeDef LPS22HB_P_Drv;
/**
  * @}
  */

/** @defgroup LPS22HB_Temperature_Exported_Functions LPS22HB Temperature Exported Functions
  * @{
  */
/* TEMPERATURE functions */
void  LPS22HB_T_Init(uint16_t DeviceAddr, TSENSOR_InitTypeDef *pInitStruct);
float LPS22HB_T_ReadTemp(uint16_t DeviceAddr);
/**
  * @}
  */

/** @defgroup HTS221_TempImported_Globals  Temperature Imported Globals
  * @{
  */
/* Temperature driver structure */
extern TSENSOR_DrvTypeDef LPS22HB_T_Drv;
/**
  * @}
  */

/** @defgroup LPS22HB_Imported_Functions LPS22HB Imported Functions
  * @{
  */
/* IO functions */
extern void     SENSOR_IO_Init(void);
extern void     SENSOR_IO_Write(uint8_t Addr, uint8_t Reg, uint8_t Value);
extern uint8_t  SENSOR_IO_Read(uint8_t Addr, uint8_t Reg);
extern uint16_t SENSOR_IO_ReadMultiple(uint8_t Addr, uint8_t Reg, uint8_t *Buffer, uint16_t Length);
extern void     SENSOR_IO_WriteMultiple(uint8_t Addr, uint8_t Reg, uint8_t *Buffer, uint16_t Length);

/**
  * @}
  */

#ifdef __cplusplus
}
#endif

#endif /* __LPS22HB__H */

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
