/*
 * Copyright (C) 2016 YunOS Project. All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef _HCI_API_
#define _HCI_API_
#include <stdint.h>

struct hci_debug_counter_t {
    uint32_t acl_in_count;
    uint32_t acl_out_count;
    uint32_t hci_in_is_null_count;
    uint32_t cmd_out_count;
    uint32_t event_in_count;
    uint32_t event_discard_count;
    uint32_t event_in_is_null_count;
    uint32_t att_in_count;
    uint32_t att_out_count;
    uint32_t hci_in_err_count;
};
extern struct hci_debug_counter_t g_hci_debug_counter;
int hci_api_init();
int hci_api_le_scan_enable(uint8_t enable, uint8_t filter_dup);
int hci_api_le_scan_param_set(uint8_t scan_type, uint16_t interval, uint16_t window, uint8_t addr_type, uint8_t filter_policy);
int hci_api_le_get_max_data_len(uint16_t *tx_octets, uint16_t *tx_time);
int hci_api_le_get_default_data_len(uint16_t *tx_octets, uint16_t *tx_time);
int hci_api_le_set_default_data_len(uint16_t tx_octets, uint16_t tx_time);
int hci_api_le_set_data_len(int16_t conn_handle, uint16_t tx_octets, uint16_t tx_time);

int hci_api_le_adv_enable(uint8_t enable);
int hci_api_le_set_random_addr(const uint8_t addr[6]);
int hci_api_le_create_conn(uint16_t scan_interval,
                           uint16_t scan_window,
                           uint8_t filter_policy,
                           uint8_t peer_addr_type,
                           const uint8_t peer_addr[6],
                           uint8_t own_addr_type,
                           uint16_t conn_interval_min,
                           uint16_t conn_interval_max,
                           uint16_t conn_latency,
                           uint16_t supervision_timeout,
                           uint16_t min_ce_len,
                           uint16_t max_ce_len);
int hci_api_set_host_buffer_size(uint16_t acl_mtu, uint8_t  sco_mtu,
                                 uint16_t acl_pkts, uint16_t sco_pkts);
int hci_api_set_host_flow_enable(uint8_t enable);
int hci_api_le_set_bdaddr(uint8_t bdaddr[6]);
int hci_api_reset();
int hci_api_read_local_feature(uint8_t feature[8]);
int hci_api_read_local_version_info(uint8_t   *hci_version,
                                    uint8_t            *lmp_version,
                                    uint16_t               *hci_revision,
                                    uint16_t               *lmp_subversion,
                                    uint16_t           *manufacturer);
int hci_api_read_bdaddr(uint8_t bdaddr[6]);
int hci_api_read_local_support_command(uint8_t supported_commands[64]);
int hci_api_le_set_event_mask(uint64_t mask);
int hci_api_le_read_local_feature(uint8_t feature[8]);
int hci_api_le_read_buffer_size_complete(uint16_t *le_max_len, uint8_t *le_max_num);
int hci_api_le_read_support_states(uint64_t *states);
int hci_api_le_read_rl_size(uint8_t *rl_size);
int hci_api_set_event_mask(uint64_t mask);
int hci_api_vs_init();
int hci_api_le_set_ad_data(uint8_t len, uint8_t data[31]);
int hci_api_le_set_sd_data(uint8_t len, uint8_t data[31]);
int hci_api_le_adv_param(uint16_t        min_interval,
                         uint16_t        max_interval,
                         uint8_t         type,
                         uint8_t         own_addr_type,
                         uint8_t  direct_addr_type,
                         uint8_t  direct_addr[6],
                         uint8_t         channel_map,
                         uint8_t         filter_policy);

int hci_api_le_create_conn_cancel();
int hci_api_le_read_remote_features(uint16_t conn_handle);
int hci_api_le_read_remote_version(uint16_t conn_handle);

int hci_api_le_disconnect(uint16_t conn_handle, uint8_t reason);

struct handle_count {
    uint16_t handle;
    uint16_t count;
};

int hci_api_host_num_complete_packets(uint8_t num_handles, struct handle_count *phc);

int hci_api_le_conn_updata(uint16_t conn_handle,
                           uint16_t conn_interval_min,
                           uint16_t conn_interval_max,
                           uint16_t conn_latency,
                           uint16_t supervision_timeout,
                           uint16_t min_ce_len,
                           uint16_t max_ce_len);

int hci_api_le_start_encrypt(uint16_t conn_handle,
                             uint8_t rand[8],
                             uint8_t ediv[2],
                             uint8_t  ltk[16]);

int hci_api_le_enctypt_ltk_req_reply(uint16_t conn_handle, uint8_t  ltk[16]);

int hci_api_le_enctypt_ltk_req_neg_reply(uint16_t conn_handle);

int hci_api_le_rand(uint8_t random_data[8]);

int hci_api_le_enc(uint8_t key[16], uint8_t plaintext[16], uint8_t ciphertext[16]);

int hci_api_le_set_phy(uint16_t  handle,
                       uint8_t   all_phys,
                       uint8_t   tx_phys,
                       uint8_t   rx_phys,
                       uint16_t  phy_opts);

int hci_api_le_conn_param_req_reply(uint16_t handle,
                                    uint16_t interval_min,
                                    uint16_t interval_max,
                                    uint16_t latency,
                                    uint16_t timeout,
                                    uint16_t min_ce_len,
                                    uint16_t max_ce_len);


int hci_api_le_conn_param_neg_reply(uint16_t handle, uint8_t reason);

int hci_api_le_add_dev_to_rl(uint8_t type,
                             uint8_t peer_id_addr[6],
                             uint8_t peer_irk[16],
                             uint8_t local_irk[16]);

int hci_api_le_remove_dev_from_rl(uint8_t type, const uint8_t peer_id_addr[6]);

int hci_api_le_clear_rl();


int hci_api_le_set_addr_res_enable(uint8_t enable);


int hci_api_le_set_privacy_mode(uint8_t type, uint8_t id_addr[6], uint8_t mode);



int hci_api_le_gen_p256_pubkey();

int hci_api_le_gen_dhkey(uint8_t remote_pk[64]);
int hci_api_read_buffer_size(uint16_t *acl_max_len,
                             uint8_t  *sco_max_len,
                             uint16_t *acl_max_num,
                             uint16_t *sco_max_num);
int hci_api_le_write_host_supp(uint8_t le, uint8_t simul);
int hci_api_num_complete_packets(uint8_t num_handles,
                                 uint16_t   *handles,
                                 uint16_t   *counts);
int hci_api_white_list_size(uint8_t *size);

int hci_api_white_list_add(uint8_t peer_addr_type, uint8_t peer_addr[6]);

int hci_api_white_list_remove(uint8_t peer_addr_type, uint8_t peer_addr[6]);

int hci_api_white_list_clear();

int hci_api_le_event_dhkey_complete(uint8_t status, uint8_t dhkey[32]);

int hci_api_le_event_pkey_complete(uint8_t status, uint8_t key[64]);

int hci_api_le_ext_adv_param_set (uint8_t         handle,
                                    	uint16_t        props,
                                    	uint32_t         prim_min_interval,
                                    	uint32_t         prim_max_interval,
                                    	uint8_t         prim_channel_map,
                                    	uint8_t         own_addr_type,
                                    	uint8_t         peer_addr_type,
                                    	uint8_t         peer_addr[6],
                                    	uint8_t         filter_policy,
                                    	int8_t          tx_power,
                                    	uint8_t         prim_adv_phy,
                                    	uint8_t         sec_adv_max_skip,
                                    	uint8_t         sec_adv_phy,
                                    	uint8_t         sid,
                                    	uint8_t         scan_req_notify_enable,
                                    	int8_t         *ret_txpower);

struct ext_adv_set_t {
    uint8_t  adv_handle;
    uint16_t duration;
    uint8_t  max_ext_adv_evts;
};

int hci_api_le_ext_adv_enable(	uint8_t       enable, uint16_t set_num, struct ext_adv_set_t adv_sets[]);

int hci_api_le_set_adv_random_addr(uint8_t handle, const uint8_t addr[6]);

int hci_api_le_ext_scan_enable(uint8_t enable,uint8_t filter_dup,uint16_t duration, uint16_t period);

struct ext_scan_param_t {
	uint8_t  type;
	uint16_t interval;
	uint16_t window;
};

int hci_api_le_ext_scan_param_set(uint8_t own_addr_type,uint8_t filter_policy,uint8_t scan_phys, struct ext_scan_param_t params[]);

struct ext_conn_phy_params_t {
	uint16_t scan_interval;
	uint16_t scan_window;
	uint16_t conn_interval_min;
	uint16_t conn_interval_max;
	uint16_t conn_latency;
	uint16_t supervision_timeout;
	uint16_t min_ce_len;
	uint16_t max_ce_len;
};

int hci_api_le_create_conn_ext(uint8_t filter_policy,
                                      uint8_t own_addr_type,                           
                                      uint8_t peer_addr_type,
                                      uint8_t peer_addr[6], 
                                      uint8_t init_phys,
                                      struct ext_conn_phy_params_t params[]);

int hci_api_le_read_phy(uint16_t conn_handle, 	uint8_t  *tx_phy , uint8_t *rx_phy);

int hci_api_le_rpa_timeout_set(uint16_t timeout);

int hci_api_le_set_ext_ad_data(uint8_t handle, uint8_t op, uint8_t  frag_pref, uint8_t  len, uint8_t  data[251]);
int hci_api_le_set_ext_sd_data(uint8_t handle, uint8_t op, uint8_t  frag_pref, uint8_t  len, uint8_t  data[251]);

int hci_api_le_remove_adv_set(uint8_t handle);

int hci_api_le_set_host_chan_classif(uint8_t ch_map[5]);

#endif

