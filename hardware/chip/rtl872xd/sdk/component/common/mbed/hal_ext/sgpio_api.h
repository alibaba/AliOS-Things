/** mbed Microcontroller Library
  ******************************************************************************
  * @file    sgpio_api.h
  * @author
  * @version V1.0.0
  * @brief   This file provides following mbed I2S API
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

#ifndef MBED_EXT_SGPIO_API_EXT_H
#define MBED_EXT_SGPIO_API_EXT_H
#if (defined(CONFIG_PLATFORM_8195BHP) && (CONFIG_PLATFORM_8195BHP == 1))  ||  (defined(CONFIG_PLATFORM_8195BLP) && (CONFIG_PLATFORM_8195BLP == 1))
///@name AmebaPro Only 
///@{
#include "device.h"

#ifdef __cplusplus
extern "C" {
#endif

/** @addtogroup sgpio SGPIO
 *  @ingroup    hal
 *  @brief      sgpio functions
 *  @{
 */

typedef enum {
    RXTC_NO_TRIGGER = Rxtc_None,
    RXTC_RISE_EDGE  = Rxtc_InputRiseEg,
    RXTC_FALL_EDGE  = Rxtc_InputFallEg,
    RXTC_BOTH_EDGE  = Rxtc_InputBothEg,
    RXTC_ACTIVE = 4   
} sgpio_rxtc_input_start;

typedef enum {
    RXTC_MATCH_EVENT0 = Rxtc_MatchEvent0,
    RXTC_MATCH_EVENT1 = Rxtc_MatchEvent1,
    RXTC_MATCH_EVENT2 = Rxtc_MatchEvent2
} sgpio_rxtc_match_event;

typedef enum {
    CAP_NO_TRIGGER         = Cap_None,
    CAP_RISE_EDGE          = Cap_InputRiseEg,
    CAP_FALL_EDGE          = Cap_InputFallEg,
    CAP_BOTH_EDGE          = Cap_InputBothEg,
    CAP_MULTC_MATCH_EVENT0 = Cap_MultcMatchEvent0
} sgpio_capture_type;

typedef enum {
    MULTC_TIMER             = Multc_Timer_Mode,
    MULTC_COUNTER_RISE_EDGE = Multc_Counter_InputRiseEg,
    MULTC_COUNTER_FALL_EDGE = Multc_Counter_InputFallEg,
    MULTC_COUNTER_BOTH_EDGE = Multc_Counter_InputBothEg
} sgpio_multc_mode;

typedef enum {
    INPUT_NO_TRIGGER = Input_None,
    INPUT_RISE_EDGE  = Input_RiseEdge,
    INPUT_FALL_EDGE  = Input_FallEdge,
    INPUT_BOTH_EDGE  = Input_BothEdge
} sgpio_counter_edge;

typedef enum {
    MULTC_MATCH_EVENT0 = Multc_MatchEvent0,
    MULTC_MATCH_EVENT1 = Multc_MatchEvent1,
    MULTC_MATCH_EVENT2 = Multc_MatchEvent2,
    MULTC_MATCH_EVENT3 = Multc_MatchEvent3
} sgpio_multc_match_event;

typedef enum {
    MULTC_MATCH_GROUP0 = Multc_MatchGroup0,
    MULTC_MATCH_GROUP1 = Multc_MatchGroup1
} sgpio_multc_match_group;

typedef enum {
    MATCH_OUTPUT_NONE   = External_None,
    MATCH_OUTPUT_LOW    = External_Low,
    MATCH_OUTPUT_HIGH   = External_High,
    MATCH_OUTPUT_TOGGLE = External_Toggle
} sgpio_external_output;

typedef enum {
    INPUT_ONLY  = Rx_Input_Only,
    BIDIRECTION = Rx_In_And_Output
} sgpio_rx_inoutput;

typedef enum {
    OUTPUT_LOW  = Output_Is_Low,
    OUTPUT_HIGH = Output_Is_High
} sgpio_output_value;

typedef enum {
    FIRST_LSB = First_Data_LSB,
    FIRST_MSB = First_Data_MSB
} sgpio_first_msb_or_lsb;

typedef enum {
    GET_BIT0 = Compare_Result_Bit0,
    GET_BIT1 = Compare_Result_Bit1
} sgpio_cmp_result_bit;

typedef enum {
    UNIT_US = Time_unit_us,
    UNIT_NS = Time_unit_ns
} sgpio_time_unit;

typedef enum {
    TIMER_US    = Timer_mode_unit_us,
    TIMER_NS    = Timer_mode_unit_ns,
    COUNTER_CNT = Countr_mode_unit_cnt
} sgpio_source_unit;

typedef enum {
    RXTC_INACTIVE   = 0x00,
    RXTC_RESET      = 0x01,
    RXTC_STOP       = 0x02,
    RXTC_RESET_STOP = 0x03    
} sgpio_rxtc_ctrl;

typedef enum {
    MULTC_INACTIVE   = 0x00,
    MULTC_RESET      = 0x01,
    MULTC_STOP       = 0x02,
    MULTC_RESET_STOP = 0x03    
} sgpio_multc_ctrl;

typedef void (*sgpio_irq_handler)(void *);

typedef struct sgpio_s sgpio_t;

/**
  * @brief  Initializes the SGPIO device, include clock/function/interrupt/SGPIO registers.
  * @param  obj: SGPIO object define in application software.
  * @param  tx: Tx PinName according to pinmux spec.
  * @param  bi_dir_rx: Bidirectional Rx PinName according to pinmux spec.
  * @retval none
  */
void sgpio_init (sgpio_t *obj, PinName tx, PinName bi_dir_rx);

/**
  * @brief  Deinitializes the SGPIO device, include function/interrupt/SGPIO registers.
  * @param  obj: SGPIO object define in application software.
  * @retval none
  */
void sgpio_deinit (sgpio_t *obj);

/**
  * @brief  Be able to release the SGPIO pins from the pin mux after initializes the SGPIO device.
  * @param  obj: SGPIO object define in application software.
  * @retval none
  */
void sgpio_pin_free (sgpio_t *obj);

/**
  * @brief  Reset SGPIO and registers.
  * @param  obj: SGPIO object define in application software.
  * @retval none
  */
void sgpio_reset (sgpio_t *obj);

/**
  * @brief  Configure SGPIO to become the rxtc timer. Use RXTC.
  * @param  obj: SGPIO object define in application software.
  * @param  timer_once_en: Enable that matching event is executed once.
  *		@arg 0 : Repeat. When the rxtc timer matches match_reset_value_time, only reset the timer.
  *		@arg 1 : Once. When the rxtc timer matches match_reset_value_time, reset and stop the timer.
  * @param  time_unit: Select the time unit.
  *		@arg 0 : The unit is us.
  *		@arg 1 : The unit is ns.
  * @param  match_time1: Matching this time 1 generates the interrupt.
  * @param  match_time1_cb: User defined IRQ callback function. Using "NULL" will not generate this interrupt.
  * @param  pmatch_time1_arg: User defined IRQ callback parameter.
  *
  * @param  match_time2: Matching this time 2 generates the interrupt.
  * @param  match_time2_cb: User defined IRQ callback function.  Using "NULL" will not generate this interrupt.
  * @param  pmatch_time2_arg: User defined IRQ callback parameter.
  *
  * @param  match_time_reset: This match time is able to make the timer reset and stop.
  * @param  time_reset_cb: User defined IRQ callback function. Using "NULL" will not generate this interrupt.
  * @param  ptime_reset_arg: User defined IRQ callback parameter.
  * @retval none
  */
void sgpio_rxtc_timer_mode (sgpio_t *obj, BOOL timer_once_en, sgpio_time_unit time_unit,
                                u32 match_time1, sgpio_irq_handler match_time1_cb, void *pmatch_time1_arg,
                                u32 match_time2, sgpio_irq_handler match_time2_cb, void *pmatch_time2_arg,
                                u32 match_time_reset, sgpio_irq_handler time_reset_cb, void *ptime_reset_arg);

/**
  * @brief  Generate the external output when happen the match events of the rxtc timer. 
  * @param  obj: SGPIO object define in application software.
  * @param  match_time1_output: Set the output value when match the time 1.
  *		@arg 0 : Inactive.
  *		@arg 1 : The output is low.
  *		@arg 2 : The output is high.
  *		@arg 3 : Toggle.
  * @param  match_time2_output: Set the output value when match the time 2.
  * @param  match_reset_time_output: Set the output value when match the reset time.
  * @retval none
  */
void sgpio_rxtc_timer_match_output (sgpio_t *obj, sgpio_external_output match_time1_output, sgpio_external_output match_time2_output,
                                        sgpio_external_output match_reset_time_output);

/**
  * @brief  Control to start the rxtc timer. Use RXTC.
  * @param  obj: SGPIO object define in application software.
  * @param  start_en: To enable or disable.
  * @retval none
  */
void sgpio_rxtc_start_en (sgpio_t *obj, BOOL start_en);

/**
  * @brief  Configure SGPIO to become the multc timer. Use MULTC.
  * @param  obj: SGPIO object define in application software.
  * @param  timer_once_en: Enable that matching event is executed once.
  *		@arg 0 : Repeat. When the multc timer matches match_reset_value_time, only reset the timer.
  *		@arg 1 : Once. When the multc timer matches match_reset_value_time, reset and stop the timer.
  * @param  time_unit: Select the time unit.
  *		@arg 0 : The unit is us.
  *		@arg 1 : The unit is ns.
  * @param  match_time_reset: This match time is able to make the timer reset and stop.
  * @param  time_reset_cb: User defined IRQ callback function. Using "NULL" will not generate this interrupt.
  * @param  ptime_reset_arg: User defined IRQ callback parameter.
  * @retval none
  */
void sgpio_multc_timer_mode (sgpio_t *obj, BOOL timer_once_en, sgpio_time_unit time_unit,
                                u32 match_time_reset, sgpio_irq_handler time_reset_cb, void *ptime_reset_arg);

/**
  * @brief  Configure SGPIO to count input triggers. Use MULTC. When use the timeout function, use RXTC.
  *         Setting the monitor time can monitor the number of triggers in a period.
  *         Reset the counter when the monitor time is timeout.
  * @param  obj: SGPIO object define in application software.
  * @param  counter_en: To enable or disable.
  * @param  input_edge: Select the edge of the trigger event.
  *		@arg 0 : Inactive.
  *		@arg 1 : Count on the rising edge.
  *		@arg 2 : Count on the falling edge.
  *		@arg 3 : Count on the both edge.
  * @param  match_value: When the counter value is equal to match_value, generate an interrupt.
  * @param  match_cb: User defined IRQ callback function. Using "NULL" will not generate this interrupt.
  * @param  pmatch_arg: User defined IRQ callback parameter.
  * @param  multc_ctrl: When the counter value is equal to match_value, control multc reset and stop.
  *		@arg 0 : Inactive
  *		@arg 1 : Reset MULTC.  
  *		@arg 2 : Stop MULTC.  
  *		@arg 3 : Reset and stop MULTC.     
  * @param  timeout_unit: Select the timeout unit.
  *		@arg 0 : The unit is us.
  *		@arg 1 : The unit is ns.
  * @param  counter_timeout: If this value is not 0, make RXTC count time. When RXTC matches the timeout value, make MULTC reset and stop.
  * @param  timeout_cb: User defined IRQ callback function. Using "NULL" will not generate this interrupt.
  * @param  ptimeout_arg: User defined IRQ callback parameter.
  * @retval none
  */
void sgpio_multc_counter_mode (sgpio_t *obj, BOOL counter_en, sgpio_counter_edge input_edge, u16 match_value, 
                                    sgpio_irq_handler match_cb, void *pmatch_arg, sgpio_multc_ctrl multc_ctrl,                                    
                                    sgpio_time_unit timeout_unit, u32 counter_timeout, sgpio_irq_handler timeout_cb, void *ptimeout_arg);


/**
  * @brief  Generate the external output when happen the match events of the multc timer counter. 
  * @param  obj: SGPIO object define in application software.
  * @param  source_unit: Select the time unit.
  *		@arg 0 : The unit is us in the timer mode.
  *		@arg 1 : The unit is ns in the timer mode.
  *		@arg 2 : The unit is count in the counter mode.
  * @param  match_value1_output: Set the output value when happen the multc match event 1.
  *		@arg 0 : Inactive.
  *		@arg 1 : The output is low.
  *		@arg 2 : The output is high.
  *		@arg 3 : Toggle.
  * @param  match_value1: The match value 1 of the multc timer counter .
  * @param  match_value2_output: Set the output value when happen the multc match event 2.
  * @param  match_value2: The match value 2 of the multc timer counter .
  * @param  match_value3_output: Set the output value when happen the multc match event 3.
  * @param  match_value3: The match value 3 of the multc timer counter .
  * @retval none
  */
void sgpio_multc_timer_counter_match_output (sgpio_t *obj, sgpio_source_unit source_unit,
                                                    sgpio_external_output match_value1_output, u32 match_value1,
                                                    sgpio_external_output match_value2_output, u32 match_value2,
                                                    sgpio_external_output match_value3_output, u32 match_value3);

/**
  * @brief  Control to start the multc timer. Use MULTC.
  * @param  obj: SGPIO object define in application software.
  * @param  start_en: To enable or disable.
  * @retval none
  */
void sgpio_multc_start_en (sgpio_t *obj, BOOL start_en);

/**
  * @brief  Configure SGPIO to become the capture mode for measuring the pulse width. Use RXTC.
  * @param  obj: SGPIO object define in application software.
  * @param  capture_en: To enable or disable.
  * @param  start_timer_edge: Select the trigger edge for starting the rxtc timer.
  * @param  input_capture_edge: Select the capture edge for capturing the time.
  *		@arg 0 : Inactive.
  *		@arg 1 : Capture on the rising edge.
  *		@arg 2 : Capture on the falling edge.
  *		@arg 3 : Capture on the both edge.
  * @param  rxtc_ctrl: When happen the capture event, control rxtc reset and stop.
  *		@arg 0 : Inactive
  *		@arg 1 : Reset RXTC.  
  *		@arg 2 : Stop RXTC.  
  *		@arg 3 : Reset and stop RXTC.     
  * @param  max_capture_range_us: Set the maximum possible measurement value for making the prescale of the timer automatically.
  *         This setting will affect the accuracy.
  * @param  capture_cb: User defined IRQ callback function. Using "NULL" will not generate this interrupt.
  * @param  pcapture_arg: User defined IRQ callback parameter.
  * @retval none
  */
void sgpio_rxtc_capture_mode (sgpio_t *obj, BOOL capture_en, sgpio_rxtc_input_start start_timer_edge, sgpio_capture_type input_capture_edge,
                                    sgpio_rxtc_ctrl rxtc_ctrl, u32 max_capture_range_us, sgpio_irq_handler capture_cb, void *pcapture_arg);

/**
  * @brief  Make the capture timer reset and stop in the capture mode when the timer value is equal to timeout_value. Use RXTC.
  * @param  obj: SGPIO object define in application software.
  * @param  capture_timeout_en: To enable or disable.
  * @param  rxtc_ctrl: When happen the capture timeout, control rxtc reset and stop.
  *		@arg 0 : Inactive
  *		@arg 1 : Reset RXTC.  
  *		@arg 2 : Stop RXTC.  
  *		@arg 3 : Reset and stop RXTC.     
  * @param  time_unit: Select the time unit.
  *		@arg 0 : The unit is us.
  *		@arg 1 : The unit is ns.
  * @param  timeout_value:  Matching this timeout value generates the interrupt.
  * @param  capture_timeout_cb: User defined IRQ callback function. Using "NULL" will not generate this interrupt.
  * @param  pcapture_timeout_arg: User defined IRQ callback parameter.
  * @retval none
  */
void sgpio_rxtc_capture_timeout(sgpio_t *obj, BOOL capture_timeout_en, sgpio_rxtc_ctrl rxtc_ctrl,
                                    sgpio_time_unit time_unit, u32 timeout_value, sgpio_irq_handler capture_timeout_cb, void *pcapture_timeout_arg);

/**
  * @brief  Monitor the capture value. When the capture value is bigger than monitor time, generate the interrupt. Use RXTC.
  * @param  obj: SGPIO object define in application software.
  * @param  capture_monitor_en: To enable or disable.
  * @param  time_unit: Select the time unit.
  *		@arg 0 : The unit is us.
  *		@arg 1 : The unit is ns.
  * @param  monitor_time: The monitor value.
  * @param  monitor_count: Set the continuous numbers of the successful condition. Value: 1 ~ 32, 0: Inactive.
  * @param  capture_monitor_cb: User defined IRQ callback function. Using "NULL" will not generate this interrupt.
  * @param  pcapture_monitor_arg: User defined IRQ callback parameter.
  * @retval none
  */
void sgpio_rxtc_capture_monitor(sgpio_t *obj, BOOL capture_monitor_en, sgpio_time_unit time_unit, u32 monitor_time, u8 monitor_count,
                                    sgpio_irq_handler capture_monitor_cb, void *pcapture_monitor_arg);

/**
  * @brief  Get the capture value. Use RXTC.
  * @param  obj: SGPIO object define in application software.
  * @param  time_unit: Select the time unit.
  *		@arg 0 : The unit is us.
  *		@arg 1 : The unit is ns.
  * @retval The capture value.
  */
u32 sgpio_get_rxtc_capture_time (sgpio_t *obj, sgpio_time_unit time_unit);

/**
  * @brief  Set the trigger edge to start the timer, and sample the input value according to the sampling time.
  *         The sampling data is put in the 32-bit register. Use RXTC.
  * @param  obj: SGPIO object define in application software.
  * @param  sampling_en: To enable or disable.
  * @param  start_timer_edge: Select the trigger edge for starting the rxtc timer.
  * @param  time_unit: Select the time unit.
  *		@arg 0 : The unit is us.
  *		@arg 1 : The unit is ns.
  * @param  sampling_time1: The sampling time 1 needs less than the sampling end time. Set to 0 when not in use.
  * @param  sampling_time2: The sampling time 2 needs less than the sampling end time. Set to 0 when not in use.
  * @param  sampling_end_time: The sampling end time. Matching this time is to make the timer reset and stop.
  * @param  sampling_bit_length: Set the bit length generated the interrupt. Value: 1 ~ 32, 0: Inactive.
  * @param  first_msb_or_lsb: Putting in the register is LSB or MSB.
  * @param  sampling_finish_cb: User defined IRQ callback function. Using "NULL" will not generate this interrupt.
  * @param  psampling_finish_arg: User defined IRQ callback parameter.
  * @retval none
  */
void sgpio_sampling_rxdata (sgpio_t *obj, BOOL sampling_en, sgpio_rxtc_input_start start_timer_edge, sgpio_time_unit time_unit,
                                u32 sampling_time1, u32 sampling_time2, u32 sampling_end_time, u8 sampling_bit_length,
                                sgpio_first_msb_or_lsb first_msb_or_lsb, sgpio_irq_handler sampling_finish_cb, void *psampling_finish_arg);

/**
  * @brief  Set the trigger edge to start the timer, and translate the capture time into "0" or "1" according to the result of the comparison.
  *         When the capture time is bigger than the compare time, the bit result is put in the 32-bit register.
  *         The bit resut is "0" or "1" is controlled by compare_result_bit. Use RXTC.
  * @param  obj: SGPIO object define in application software.
  * @param  sampling_en: To enable or disable.
  * @param  capture_en: Select the trigger edge for starting the rxtc timer.
  * @param  input_capture_edge: Select the capture edge for capturing the time.
  *		@arg 0 : Inactive.
  *		@arg 1 : Capture on the rising edge.
  *		@arg 2 : Capture on the falling edge.
  *		@arg 3 : Capture on the both edge.
  * @param  max_capture_range_us: Set the maximum possible measurement value for making the prescale of the timer automatically.
  *         This setting will affect the accuracy.
  * @param  time_unit: Select the time unit.
  *		@arg 0 : The unit is us.
  *		@arg 1 : The unit is ns.
  * @param  capture_compare_value_time: Set the compare time.
  * @param  compare_result_bit: Deciding the value of the result bit is "0" or "1" when the capture time is bigger than the compare time.
  * @param  compare_bit_length:  Set the bit length generated the interrupt. Value: 1 ~ 32, 0: Inactive.
  * @param  first_msb_or_lsb: Putting in the register is LSB or MSB.
  * @param  sampling_finish_cb: User defined IRQ callback function. Using "NULL" will not generate this interrupt.
  * @param  psampling_finish_arg: User defined IRQ callback parameter.
  * @retval none
  */
void sgpio_capture_compare_rxdata (sgpio_t *obj, BOOL capture_en, sgpio_rxtc_input_start start_timer_edge, sgpio_capture_type input_capture_edge,
                                        u32 max_capture_range_us, sgpio_time_unit time_unit, u32 capture_compare_value_time, sgpio_cmp_result_bit compare_result_bit,
                                        u8 compare_bit_length, sgpio_first_msb_or_lsb first_msb_or_lsb, sgpio_irq_handler compare_finish_cb, void *pcompare_finish_arg);

/**
  * @brief  Get the register of the RX data by sample or capture.
  * @param  obj: SGPIO object define in application software.
  * @retval The result register.
  */
u32 sgpio_get_input_rxdata (sgpio_t *obj);

/**
  * @brief  Configure the waveforms of the bit 0 and bit 1 for the output.
  * @param  obj: SGPIO object define in application software.
  * @param  initial_output_value: Set the initial output value.
  * @param  time_unit: Select the time unit.
  *		@arg 0 : The unit is us.
  *		@arg 1 : The unit is ns.
  * @param  bit0_middle_change_time: The time is to change the output value for the bit 0.
  * @param  bit0_duration_time: The duration of the bit 0.
  * @param  bit1_middle_change_time: The time is to change the output value for the bit 1.
  * @param  bit1_duration_time: The duration of the bit 1.
  * @retval none
  */
void sgpio_set_bit_symbol_of_txdata (sgpio_t *obj, sgpio_output_value_t initial_output_value, sgpio_time_unit time_unit,
                                        u32 bit0_middle_change_time, u32 bit0_duration_time,
                                        u32 bit1_middle_change_time, u32 bit1_duration_time);

/**
  * @brief  Set the output data according to the waveforms of the bit 0 and bit 1. Use MULTC.
  * @param  obj: SGPIO object define in application software.
  * @param  txdata_en: To enable or disable.
  * @param  rx_output_en: Control the output behavior of the bidirectional Rx.
  * @param  output_bit_counts: Set the output bit number.
  * @param  ptxdata_pointer: Set the output data pointer.
  * @param  txdata_finish_cb: When finish the TX output, generate the interrupt.
  *          User defined IRQ callback function. Using "NULL" will not generate this interrupt.
  * @param  ptxdata_finish_arg: User defined IRQ callback parameter.
  * @retval none
  */
void sgpio_set_txdata (sgpio_t *obj, BOOL txdata_en, BOOL rx_output_en, u16 output_bit_counts, u32 *ptxdata_pointer,
                        sgpio_irq_handler txdata_finish_cb, void *ptxdata_finish_arg);

/**
  * @brief  Start to output the TX data. Use MULTC.
  * @param  obj: SGPIO object define in application software.
  * @retval none
  */
void sgpio_start_send_txdata (sgpio_t *obj);


/**
  * @brief  Monitor the register of the sample or capture data. When the register value is equal to the monitor data, generate the interrupt.
  * @param  obj: SGPIO object define in application software.
  * @param  monitor_en: To enable or disable.
  * @param  monitor_data: Set the monitor data.
  * @param  monitor_input_data_mask: Set the bit mask of the monitor data.
  * @param  monitor_rxdata_cb: User defined IRQ callback function. Using "NULL" will not generate this interrupt.
  * @param  pmonitor_rxdata_arg: User defined IRQ callback parameter.
  * @retval none
  */
void sgpio_set_rxdata_monitor (sgpio_t *obj, BOOL monitor_en, u32 monitor_data, u32 monitor_input_data_mask,
                                    sgpio_irq_handler monitor_rxdata_cb, void *pmonitor_rxdata_arg);

/**
  * @brief  Disable the monitor mode.
  * @param  obj: SGPIO object define in application software.
  * @retval none
  */
void sgpio_disable_rxdata_monitor (sgpio_t *obj);

/**
  * @brief  Reset the register of the RX data.
  * @param  obj: SGPIO object define in application software.
  * @retval none
  */
void sgpio_reset_receive_rxdata(sgpio_t *obj);

/**
  * @brief  Initializes the GDMA of SGPIO. Schedule the output to make the waveform by GDMA. Use MULTC.
  * @param  obj: SGPIO object define in application software.
  * @param  pmatch_value_ptr: The pointer is the memory address of setting the match time.
  *          Memory allocation: reset time(a), match time1(a), match time2(a), match time3(a)
  *                                     reset time(b), match time1(b), match time2(b), match time3(b)
  *                                     reset time(c), ...
  * @param  reset_time_num: Set the number of reset times. An interrupt will be generated when the number of times is reached.
  * @param  time_unit: Select the time unit.
  *		@arg 0 : The unit is us.
  *		@arg 1 : The unit is ns.
  * @param  match_time1_output: Set the output value when match the time 1.
  *		@arg 0 : Inactive.
  *		@arg 1 : The output is low.
  *		@arg 2 : The output is high.
  *		@arg 3 : Toggle.
  * @param  match_time2_output: Set the output value when match the time 2.
  * @param  match_time3_output: Set the output value when match the time 3.
  * @param  counter_finish_cb: User defined IRQ callback function. Using "NULL" will not generate this interrupt.
  * @param  pcounter_finish_arg: User defined IRQ callback parameter.
  * @retval none
  */
void sgpio_init_dma_match_output (sgpio_t *obj, u16 *pmatch_value_ptr, u8 reset_time_num, sgpio_time_unit time_unit,
                                                sgpio_external_output match_time1_output, sgpio_external_output match_time2_output, sgpio_external_output match_time3_output,
                                                sgpio_irq_handler counter_finish_cb, void *pcounter_finish_arg);

/**
  * @brief  Deinitializes the GDMA of SGPIO.
  * @param  obj: SGPIO object define in application software.
  * @retval none
  */
void sgpio_deinit_dma_match_output (sgpio_t *obj);

/**
  * @brief  Start the GDMA to send the TX waveform.
  * @param  obj: SGPIO object define in application software.
  * @retval none
  */
void sgpio_start_dma (sgpio_t *obj);

/**
  * @brief  Set the TX output value.
  * @param  obj: SGPIO object define in application software.
  * @param  value: Set the output value.
  *		@arg 1 : The output is low.
  *		@arg 2 : The output is high.
  * @retval none
  */
void sgpio_set_output_value (sgpio_t *obj, sgpio_output_value value);

/**
  * @brief  Make the inverse output
  * @param  obj: SGPIO object define in application software.
  * @retval none
  */
void sgpio_set_inverse_output (sgpio_t *obj);

/**
* @brief  Get the register value of the rxtc timer.
* @param  obj: SGPIO object define in application software.
* @retval The register value of the rxtc timer.
*/
u16 sgpio_get_rxtc_value (sgpio_t *obj);

/**
* @brief Reset the rxtc timer.
* @param  obj: SGPIO object define in application software.
* @retval none
*/
void sgpio_reset_rxtc (sgpio_t *obj);

/**
* @brief  Get the register value of the multc timer.
* @param  obj: SGPIO object define in application software.
* @retval The register value of the multc timer.
*/
u16 sgpio_get_multc_value (sgpio_t *obj);

/**
* @brief Reset the multc timer.
* @param  obj: SGPIO object define in application software.
* @retval none
*/
void sgpio_reset_multc (sgpio_t *obj);

/**
* @brief Control the rx to become the bi-direction.
* @param  obj: SGPIO object define in application software.
* @param  rx_in_out: Select the input or bi-direction.
*     @arg 0 : Only input.
*     @arg 1 : Bi-direction.
* @retval none
*/
void sgpio_set_rx_in_out_ctrl (sgpio_t *obj, sgpio_rx_inoutput rx_in_out);


/*\@}*/

#ifdef __cplusplus
}
#endif
///@}
#endif  // end of "#if defined(CONFIG_PLATFORM_8195BHP) || defined(CONFIG_PLATFORM_8195BLP)"
#endif

