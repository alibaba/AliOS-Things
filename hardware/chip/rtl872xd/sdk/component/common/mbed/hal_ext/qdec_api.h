/** mbed Microcontroller Library
  ******************************************************************************
  * @file    qdec_api.h
  * @author
  * @version V1.0.0
  * @brief
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2006-2013 ARM Limited
  *
  * Licensed under the Apache License, Version 2.0 (the "License");
  * you may not use this file except in compliance with the License.
  * You may obtain a copy of the License at
  *
  *     http://www.apache.org/licenses/LICENSE-2.0
  *
  * Unless required by applicable law or agreed to in writing, software
  * distributed under the License is distributed on an "AS IS" BASIS,
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  ******************************************************************************
  */

#ifndef MBED_EXT_QDEC_API_EXT_H
#define MBED_EXT_QDEC_API_EXT_H
#if defined(CONFIG_PLATFORM_8195BHP) && (CONFIG_PLATFORM_8195BHP == 1)
///@name AmebaPro Only 
///@{
#include "device.h"

#ifdef __cplusplus
extern "C" {
#endif

/** @addtogroup qdec QDEC
 *  @ingroup    hal
 *  @brief      qdec functions
 *  @{
 */

typedef enum {
    QDEC_SClk_2M    = QdecSClk_2M,
    QDEC_SClk_32K   = QdecSClk_32K,
    QDEC_SClk_MAXID = QdecSClk_MAXID
} qdec_clk_source;

typedef enum {
    ONE_PHASE_COUNT  = One_Phase,
    TWO_PHASE_COUNT  = Two_Phase
} qdec_resolution;

typedef enum {
    REF_INDEX           = Index_Event,
    PC_OVER_UNDER_FLOW  = PC_Over_Or_Under_Flow
} qdec_rotation_source;

typedef enum {
    INPUT_INIT_LOW  = Input_Low,
    INPUT_INIT_HIGH = Input_High
} qdec_index_level;

typedef enum {
    INDEX_NO_RST      = Disable_Rst,
    INDEX_RST_1ST     = Reset_1st_Index,
    INDEX_RST_ALWAYS  = Reset_Every_Index
} qdec_index_rst;

typedef enum {
    PC_CHANGE_1_CNT = Pos_Chg1,
    PC_CHANGE_2_CNT = Pos_Chg2,
    PC_CHANGE_4_CNT = Pos_Chg4
} qdec_cnt_chg;

typedef enum {
    CAL_PC_ABSOLUTE_CNT     = Absolute_Count,
    CAL_PC_NOT_ABSOLUTE_CNT = Not_Absolute_Count
} qdec_vmuc_mode;

typedef struct qdec_def_setting_s {
    u8                      smp_div;        /*!< Sampling Clock = source clock/(smp_div+1). Value: 0 ~ 31. */
    qdec_resolution         resolution;     /*!< The position accumulation counter. 0: 1 phase, 1: 2 phases. Only phase A edges are counted. */
    u16                     pc_maximum;     /*!< The maximum value of the position counter. Value: 0 ~ 65535. */
    qdec_rotation_source    rotation_mode;  /*!< 0: accumulate the rotation number when the index event occurres with direction(+/-), 1: accumulate the rotation number when the position counter occurres overflow(+)/underflow(-). */
    BOOL                    filter_enable;  /*!< 0: disable de-bounce. 1: enable de-bounce. */
    u16                     filter_time;    /*!< De-bounce timer configuration, Value: 0 ~ 2047, unit is sclk: 0.5us or 31.25us. The time needs a little bigger than the maximum period of noise. */
    BOOL                    index_enable;   /*!< 0: The index pulse signal detection is disabled. 1: enable. */
    qdec_index_level        index_level;    /*!< 0: The index input is low. 1: The index input is lhigh. */
    qdec_index_rst          index_rst;      /*!< The index input can reset the position counter. 0: disable, 1: only reset 1st, 2: reset every index. */
} qdec_def_setting_t, *pqdec_def_setting_t;

typedef void (*qdec_irq_handler)(void *);

typedef struct qdec_s qdec_t;

/**
  * @brief  Initializes the QDEC device, include clock/function/interrupt/QDEC registers.
  * @param  obj: QDEC object define in application software.
  * @param  pha: PHA PinName according to pinmux spec.
  * @param  phb: PHB PinName according to pinmux spec.
  * @param  idx: IDX PinName according to pinmux spec. This pin is not configured when set PinName "NC".
  * @param  source_clk: The clock source of the quadrature decoder.
  *		@arg 0 : 2MHz
  *		@arg 1 : 32KHz
  * @retval none
  */
void qdec_init (qdec_t *obj, PinName pha, PinName phb, PinName idx, qdec_clk_source source_clk);

/**
  * @brief  Deinitializes the QDEC device, include function/interrupt/QDEC registers.
  * @param  obj: QDEC object define in application software.
  * @retval none
  */
void qdec_deinit (qdec_t *obj);

/**
  * @brief  Set the clock source, and the clock divider
  * @param  obj: QDEC object define in application software.
  * @param  smp_div: Sampling Clock = source clock/(smp_div+1). Value: 0 ~ 31.
  * @retval none
  */
void qdec_set_sampling_div (qdec_t *obj, u8 smp_div);

/**
  * @brief  Set the position accumulation counter.
  * @param  obj: QDEC object define in application software.
  * @param  resolution: The resolution of the quadrature decoder.
  *		@arg 0 : 1 phase
  *		@arg 1 : 2 phases. Only phase A edges are counted.
  * @retval none
  */
void qdec_set_resolution (qdec_t *obj, qdec_resolution resolution);

/**
  * @brief  Set the position accumulation counter.
  * @param  obj: QDEC object define in application software.
  * @param  pc_maximum: The maximum value of the position counter is according to the quadrature encoder. Value: 0 ~ 65535.
  * @retval none
  */
void qdec_set_pc_maximum (qdec_t *obj, u16 pc_maximum);

/**
  * @brief  Set the method for calculating the rotation.
  * @param  obj: QDEC object define in application software.
  * @param  rotation_mode: The rotation mode.
  *		@arg 0 : Accumulate the rotation number when the index event occurres with direction(+/-).
  *		@arg 1 : Accumulate the rotation number when the position counter occurres overflow(+)/underflow(-).
  * @retval none
  */
void qdec_set_rotation_mode (qdec_t *obj, qdec_rotation_source rotation_mode);

/**
  * @brief  Set the glitch filter.
  * @param  obj: QDEC object define in application software.
  * @param  filter_enable: To enable or disable.
  * @param  filter_time: Set de-bounce time = filter_time x clock source unit, clock source unit is 0.5us or 31.25us.
  * @retval none
  */
void qdec_set_filter_init (qdec_t *obj, BOOL filter_enable, u16 filter_time);

/**
  * @brief  Configure the function of the index pin.
  * @param  obj: QDEC object define in application software.
  * @param  index_enable: To enable or disable.
  * @param  index_level: Set the used level according to the quadrature encoder.
  * @param  index_rst: The index input can reset the position counter.
  *		@arg 0 : disable.
  *		@arg 1 : only reset 1st.
  *		@arg 2 : always reset.
  * @retval none
  */
void qdec_set_index_init (qdec_t *obj, BOOL index_enable, qdec_index_level index_level, qdec_index_rst index_rst);

/**
  * @brief  Control quadrature decoder.
  * @param  obj: QDEC object define in application software.
  * @param  qdec_en_ctrl: To enable or disable.
  * @retval none
  */
void qdec_set_en (qdec_t *obj, BOOL qdec_en_ctrl);

/**
  * @brief  Control the interrupt event when change the direction.
  * @param  obj: QDEC object define in application software.
  * @param  en: To enable or disable.
  * @param  qdec_cb:  User defined IRQ callback function.
  * @param  pqdec_arg: User defined IRQ callback parameter.
  * @retval none
  */
void qdec_set_position_direction_change_init (qdec_t *obj, BOOL en, qdec_irq_handler qdec_cb, void *pqdec_arg);

/**
  * @brief  Set a value changed on the position counter which will trigger the position changed interrupt.
  * @param  obj: QDEC object define in application software.
  * @param  en: To enable or disable.
  * @param  count_change_value:  This field set a value changed on the position counter which will trigger the position changed interrupt.
  *		@arg 00: +/- 1.
  *		@arg 01: +/- 2.
  *		@arg 10: +/- 4.
  *		@arg 11: reserved.
  * @param  qdec_cb:  User defined IRQ callback function.
  * @param  pqdec_arg:  User defined IRQ callback parameter.
  * @retval none
  */
void qdec_set_position_count_change_init (qdec_t *obj, BOOL en, qdec_cnt_chg count_change_value, qdec_irq_handler qdec_cb, void *pqdec_arg);

/**
  * @brief  Control the interrupt event when the position counter is equal to the compare value.
  * @param  obj: QDEC object define in application software.
  * @param  en: To enable or disable.
  * @param  compare_value:  If the position counter is equal to this value, the corresponding interrupt will be asserted. The value is 0x00~0xFFFF.
  * @param  qdec_cb:  User defined IRQ callback function.
  * @param  pqdec_arg:  User defined IRQ callback parameter.
  * @retval none
  */
void qdec_set_position_compare_init (qdec_t *obj, BOOL en, u16 compare_value, qdec_irq_handler qdec_cb, void *pqdec_arg);

/**
  * @brief  Control the interrupt event when the position counter value is overflow (Max position counter -> 0x0000).
  * @param  obj: QDEC object define in application software.
  * @param  en: To enable or disable.
  * @param  qdec_cb:  User defined IRQ callback function.
  * @param  pqdec_arg:  User defined IRQ callback parameter.
  * @retval none
  */
void qdec_set_position_overflow_init (qdec_t *obj, BOOL en, qdec_irq_handler qdec_cb, void *pqdec_arg);

/**
  * @brief  Control the interrupt event when the position counter value is underflow (0x0000 -> Max position counter).
  * @param  obj: QDEC object define in application software.
  * @param  en: To enable or disable.
  * @param  qdec_cb:  User defined IRQ callback function.
  * @param  pqdec_arg:  User defined IRQ callback parameter.
  * @retval none
  */
void qdec_set_position_underflow_init (qdec_t *obj, BOOL en, qdec_irq_handler qdec_cb, void *pqdec_arg);

/**
  * @brief  Control the interrupt event when the rotation counter is equal to the rotation value.
  * @param  obj: QDEC object define in application software.
  * @param  en: To enable or disable.
  * @param  compare_value:  If the rotation counter is equal to this value, the corresponding interrupt will be asserted. The value is 0x00~0xFFF.
  * @param  qdec_cb:  User defined IRQ callback function.
  * @param  pqdec_arg:  User defined IRQ callback parameter.
  * @retval none
  */
void qdec_set_rotation_compare_init (qdec_t *obj, BOOL en, u16 compare_value, qdec_irq_handler qdec_cb, void *pqdec_arg);

/**
  * @brief  Control the interrupt event when the rotation counter value is overflow (Max rotation counter -> 0x0000).
  * @param  obj: QDEC object define in application software.
  * @param  en: To enable or disable.
  * @param  qdec_cb:  User defined IRQ callback function.
  * @param  pqdec_arg:  User defined IRQ callback parameter.
  * @retval none
  */
void qdec_set_rotation_overflow_init (qdec_t *obj, BOOL en, qdec_irq_handler qdec_cb, void *pqdec_arg);

/**
  * @brief  Control the interrupt event when the rotation counter value is underflow (0x0000 -> Max position counter).
  * @param  obj: QDEC object define in application software.
  * @param  en: To enable or disable.
  * @param  qdec_cb:  User defined IRQ callback function.
  * @param  pqdec_arg:  User defined IRQ callback parameter.
  * @retval none
  */
void qdec_set_rotation_underflow_init (qdec_t *obj, BOOL en, qdec_irq_handler qdec_cb, void *pqdec_arg);

/**
  * @brief  Control the interrupt event when get the velocity by measuring the counts in a period of time.
  * @param  obj: QDEC object define in application software.
  * @param  en: To enable or disable.
  * @param  vmuc_mode:  Control the measurement value.
  *		@arg 0 : the counter value uses the absolute value.
  *		@arg 1 : the counter value does not use the absolute value.
  * @param  time_us:  Measure time. This unit is us.
  * @param  qdec_cb:  User defined IRQ callback function.
  * @param  pqdec_arg:  User defined IRQ callback parameter.
  * @retval none
  */
void qdec_set_velocity_measure_cnt_init (qdec_t *obj, BOOL en, qdec_vmuc_mode vmuc_mode, u32 time_us, qdec_irq_handler qdec_cb, void *pqdec_arg);

/**
  * @brief  Control the interrupt event. The measuring counts is less than the low limit when execute the velocity function.
  * @param  obj: QDEC object define in application software.
  * @param  en: To enable or disable.
  * @param  low_limit_cnt:  The low limit value is 0x00~0xFFFF.
  * @param  qdec_cb:  User defined IRQ callback function.
  * @param  pqdec_arg:  User defined IRQ callback parameter.
  * @retval none
  */
void qdec_set_velocity_measure_cnt_lowlmt_init (qdec_t *obj, BOOL en, u16 low_limit_cnt, qdec_irq_handler qdec_cb, void *pqdec_arg);

/**
  * @brief  Control the interrupt event. The measuring counts is bigger than the up limit when execute the velocity function.
  * @param  obj: QDEC object define in application software.
  * @param  en: To enable or disable.
  * @param  up_limit_cnt:  The up limit value is 0x00~0xFFFF.
  * @param  qdec_cb:  User defined IRQ callback function.
  * @param  pqdec_arg:  User defined IRQ callback parameter.
  * @retval none
  */
void qdec_set_velocity_measure_cnt_uplmt_init (qdec_t *obj, BOOL en, u16 up_limit_cnt, qdec_irq_handler qdec_cb, void *pqdec_arg);

/**
  * @brief  Control the interrupt event for detecting the index.
  * @param  obj: QDEC object define in application software.
  * @param  en: To enable or disable.
  * @param  qdec_cb:  User defined IRQ callback function.
  * @param  pqdec_arg:  User defined IRQ callback parameter.
  * @retval none
  */
void qdec_set_index_present_init (qdec_t *obj, BOOL en, qdec_irq_handler qdec_cb, void *pqdec_arg);

/**
  * @brief  Control the interrupt event for checking that the position counter is zero at the present of index.
  * @param  obj: QDEC object define in application software.
  * @param  en: To enable or disable.
  * @param  qdec_cb:  User defined IRQ callback function.
  * @param  pqdec_arg:  User defined IRQ callback parameter.
  * @retval none
  */
void qdec_set_index_check_position_init (qdec_t *obj, BOOL en, qdec_irq_handler qdec_cb, void *pqdec_arg);

/**
  * @brief  Reset the position counter and machine.
  * @param  obj: QDEC object define in application software.
  * @retval none
  */
void qdec_position_reset (qdec_t *obj);

/**
  * @brief  Reset the rotation counter and machine.
  * @param  obj: QDEC object define in application software.
  * @retval none
  */
void qdec_rotation_reset (qdec_t *obj);

/**
  * @brief  Reset the velocity counter and machine.
  * @param  obj: QDEC object define in application software.
  * @retval none
  */
void qdec_velocity_cnt_reset (qdec_t *obj);

/**
  * @brief  Start the timer to measure the velocity.
  * @param  obj: QDEC object define in application software.
  * @param  en: To enable or disable.
  * @retval none
  */
void qdec_start_velocity_measure_cnt (qdec_t *obj, BOOL en);

/**
  * @brief  Get the velocity counts.
  * @param  obj: QDEC object define in application software.
  * @retval The capture value of the velocity counter.
  */
u16 qdec_get_velocity_measure_cnt (qdec_t *obj);

/**
  * @brief  Calculate RPM (Revolution Per Minute) according to the velocity counts.
  * @param  obj: QDEC object define in application software.
  * @param  velocity_cnt:  The velocity counts.
  * @retval RPM
  */
float qdec_get_velocity_measure_cnt_rpm (qdec_t *obj, u16 velocity_cnt);

/**
  * @brief  Get the position counter of QDEC.
  * @param  obj: QDEC object define in application software.
  * @retval Position counter 16bits.
  */
u16 qdec_get_position (qdec_t *obj);

/**
  * @brief  Get the rotation counter of QDEC.
  * @param  obj: QDEC object define in application software.
  * @retval Rotation counter 12bits.
  */
u16 qdec_get_rotation (qdec_t *obj);

/**
  * @brief  Get the movement direction of QDEC.
  * @param  obj: QDEC object define in application software.
  * @retval 0: decrease, 1: increase
  */
u8 qdec_get_direction (qdec_t *obj);

/**
  * @brief  Get the phase state of QDEC.
  * @param  obj: QDEC object define in application software.
  * @retval Current state of (A, B) phase
  */
u8 qdec_get_phase_state (qdec_t *obj);

/*\@}*/

#ifdef __cplusplus
}
#endif
///@}
#endif  // end of "#if defined(CONFIG_PLATFORM_8195BHP)"
#endif

