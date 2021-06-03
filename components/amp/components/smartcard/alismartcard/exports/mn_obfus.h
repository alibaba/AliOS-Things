/*
 * Copyright (c) 2019 Alibaba Group. All rights reserved.
 * License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * Description:
 * mn_obfus.h: function obfus file
 *
 */
#ifndef __MN_OBFUS_H__
#define __MN_OBFUS_H__

//#define OBFUSCATION_CODE

#ifdef OBFUSCATION_CODE

// sim func
#define sim_get_common_sim_cb l1l0
#define sim_get_cur_slot l1l1
#define sim_get_slot_cnt l1l2
#define sim_info_init l1l3
#define sim_init l1l4
#define sim_is_switch_support l1l5
#define sim_switch_to_next_card l1l6
#define sim_alicard_detect l1l7
#define sim_card_init_cb l1l8
#define sim_get_alicard_cb l1l9
#define sim_get_cur_slot_cb l1l10
#define sim_get_slot_cnt_cb l1l11
#define sim_is_switch_support_cb l1l12
#define sim_switch_to_next_card_cb l1l13
#define _sim_convert_number_to_iccid l1l14
#define _sim_parse_alibaba_iccid l1l15
#define _sim_power l1l16
#define _sim_remove_quote l1l17
#define _sim_select_next_card_id l1l18
#define _sim_shrink_cpbr_result l1l19
// log func
#define log_destroy l2l0
#define log_file_destroy l2l1
#define log_file_init l2l2
#define log_file_print l2l3
#define log_init l2l4
#define log_print l2l5
// hal func
#define module_proc_at_resp l3l0
#define module_serial_deinit l3l1
#define module_serial_get_name l3l2
#define module_serial_init l3l3
#define _module_check_simstat l3l4
#define _module_is_asr l3l5
#define _module_is_asr1802 l3l6
#define _module_is_bc l3l7
#define _module_is_ec_cat1 l3l8
#define _module_is_m5311 l3l9
#define _module_is_nbiot l3l10
#define _module_parse_asr_cpbr_result l3l11
#define _module_parse_cpbr_result l3l12
#define _module_parse_crsm_result l3l13
#define _module_parse_ec_cat1_cpbr_result l3l14
// mn func
#define mn_hal_switch l4l0
#define mn_parse_carrier_from_iccid l4l1
#define mn_get_log_dir l4l2
#define mn_parse_configs l4l3
#define _mn_switch_card l4l4
#define _mn_exec_extra_reset_cmd l4l5
// var
#define g_carrier_iccid v0l0
#define g_mn_card_switch_time v0l1
#define g_mn_log_dir v0l2
#define g_mn_module_startup_time v0l3
#define g_module_at v0l4
#define g_module_at_baudrate v0l5
#define g_module_reset_while_switch v0l6
#define g_module_type v0l7
#define g_reset_extra_cmd v0l8
#define g_conf_file_path v0l9
#define g_mn_init v0l10
#define g_local_iccid v0l11
#define g_ali_card v0l12
#define g_card_num v0l13
#define g_argc v0l14
#define g_argv v0l15
#define g_mn_extra_reset_cmd v0l16
#define g_str_resp v0l17
#define g_at_table v0l18

#endif

#endif /* MN_OBFUS_H */
