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
 * std_ali_card.c: alibaba smart card impl
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "aliyun_iot_smartcard_hal.h"
#include "mn_common.h"
#include "sim.h"
#include "sim_internal.h"

typedef struct {
    aliyun_iot_smartcard_carrier_type_t carrier;
    int slot;
    char iccid[21];
} ali_triple_card_slot_st;

typedef enum {
    MFR_SIMHUB = 1,
    MFR_TIANYU,
    MFR_HUAHONG
} ali_triple_card_manufacturer_et;

typedef struct {
    bool simLoad;
    char ali_iccid[21];
    ali_triple_card_manufacturer_et mfr;
    int slot_count;
    int cmcc_slot;
    int cucc_slot;
    int ctcc_slot;
    ali_triple_card_slot_st slot[3];
} ali_triple_card_st;

static ali_triple_card_st g_ali_card;

static aliyun_iot_smartcard_slot_num_t g_card_num = SIM_NULL;

static int sim_get_cur_slot_cb(aliyun_iot_smartcard_slot_num_t *slot_num);

static int dummy_slot[MAX_SIM_SLOT_CNT];

static int _sim_select_next_card_id(aliyun_iot_smartcard_type_t type)
{
    switch (type) {
        case SWITCH_TO_NEXT:
            if (g_card_num == SIM1) {
                return g_ali_card.cucc_slot + 1;
            } else if (g_card_num == SIM2) {
                return g_ali_card.ctcc_slot + 1;
            } else {
                return g_ali_card.cmcc_slot + 1;
            }

        case SWITCH_TO_SIM1:
            return g_ali_card.cmcc_slot + 1;

        case SWITCH_TO_SIM2:
            return g_ali_card.cucc_slot + 1;

        case SWITCH_TO_SIM3:
            return g_ali_card.ctcc_slot + 1;

        default:
            return -1;
    }
}

#define SIM_SWITCH_RETRY_CNT (0)
static int sim_switch_to_next_card_cb(aliyun_iot_smartcard_type_t switch_type)
{
    int ret;
    int id;
    int retry_cnt = 0;
    char pb_str[16];

    aliyun_iot_smartcard_slot_num_t before_slot_num = SIM_NULL;
    aliyun_iot_smartcard_slot_num_t after_slot_num = SIM_NULL;

    sim_get_cur_slot_cb(&before_slot_num);

    if ((aliyun_iot_smartcard_type_t) before_slot_num == switch_type) {
        mn_log_info("switch type %d is same, no need to switch", before_slot_num);
        return 0;
    }

    mn_log_info("before switch, current slot is %d", before_slot_num);

    while (1) {
        mn_log_dbg("<switch to next card>");

        id = _sim_select_next_card_id(switch_type);

        if (id <= 0) {
            mn_log_err("target slot error!");
            return MN_ERROR_GENERIC;
        }

        // add default pb str value
        snprintf(pb_str, sizeof(pb_str), "01011901%d0", id);

        ret = aliyun_iot_smartcard_hal_cpbw(id + 1, pb_str);

        if (ret != 0) {
            mn_log_err("aliyun_iot_smartcard_hal_cpbw id=%d fail!", id);
            return ret;
        }

        ret = aliyun_iot_smartcard_hal_reset_module();

        if (ret != 0) {
            mn_log_err("aliyun_iot_smartcard_hal_reset_module fail!");
            return ret;
        }

        sim_get_cur_slot_cb(&after_slot_num);
        mn_log_info("after switch, current slot is %d", after_slot_num);

        if (after_slot_num == before_slot_num && retry_cnt < SIM_SWITCH_RETRY_CNT) {
            mn_log_warn("current slot is same, retry again");
            retry_cnt++;
            continue;
        }

        break;
    }

    if (after_slot_num == before_slot_num) {
        mn_log_err("current slot is same as previous slot");
        return MN_ERROR_SAME_CARD;
    } else if ((aliyun_iot_smartcard_type_t) after_slot_num != switch_type) {
        mn_log_err("current slot %d is not specified slot %d", after_slot_num, switch_type);
        return MN_ERROR_GENERIC;
    }

    return 0;
}

static int sim_get_cur_slot_cb(aliyun_iot_smartcard_slot_num_t *slot_num)
{
    int ret;
    aliyun_iot_smartcard_carrier_type_t car_type;

    ret = aliyun_iot_smartcard_hal_get_carrier(&car_type);

    if (ret != 0) {
        mn_log_err("aliyun_iot_smartcard_hal_get_carrier fail!");
        return -1;
    }

    if (car_type == CHINA_MOBILE) {
        *slot_num = SIM1;
        g_card_num = SIM1;
        return 0;
    } else if (car_type == CHINA_UNICOM) {
        *slot_num = SIM2;
        g_card_num = SIM2;
        return 0;
    } else if (car_type == CHINA_TELECOM) {
        *slot_num = SIM3;
        g_card_num = SIM3;
        return 0;
    }

    return -1;
}

static int sim_is_switch_support_cb(bool *support)
{
    *support = true;
    return 0;
}

static int sim_get_slot_cnt_cb(uint8_t *cnt)
{
    *cnt = g_ali_card.slot_count;
    return 0;
}

static int _sim_parse_alibaba_iccid(char *str)
{
    int i = 0;
    int count = 0;
    char *number = str;

    if (str == NULL) {
        return -1;
    }

    if (strncmp(number, "8886", 4) != 0) {
        return -1;
    }

    number += 4;

    for (i = 0 ; i < 3 ; i++) {
        if (strncmp(number, "10", 2) == 0) {
            g_ali_card.cucc_slot = i;
            number += 2;
            count++;
        } else if (strncmp(number, "20", 2) == 0) {
            g_ali_card.cmcc_slot = i;
            number += 2;
            count++;
        } else if (strncmp(number, "30", 2) == 0) {
            g_ali_card.ctcc_slot = i;
            number += 2;
            count++;
        } else if (strncmp(number, "00", 2) == 0) {
            dummy_slot[i] = 1;
            number += 2;
        } else {
            return -1;
        }
    }

    g_ali_card.slot_count = count;

    if (strncmp(number, "1", 1) == 0) {
        g_ali_card.mfr = MFR_SIMHUB;
    } else if (strncmp(number, "2", 1) == 0) {
        g_ali_card.mfr = MFR_TIANYU;
    } else if (strncmp(number, "3", 1) == 0) {
        g_ali_card.mfr = MFR_HUAHONG;
    } else {
        return -1;
    }

    strncpy(g_ali_card.ali_iccid, str, sizeof(g_ali_card.ali_iccid) - 1);

    return 0;
}

typedef struct {
    int pos;
    char val;
} record_t;

static long _sim_power(int m, int n)
{
    int i = 0;
    long result = 1;

    for (i = 0; i < n; i++) {
        result = result * m;
    }

    return result;
}

static int _sim_convert_number_to_iccid(char *number, int number_len, char *iccid, int iccid_len)
{
    int i;
    int j;
    record_t record[4];
    int num_str_len = 0;
    /* char cnt in iccid */
    int char_cnt;
    int char_encode_start_pos;
    char decimal_str[4];
    int senary_val[4];
    int decimal_val;


    if (number == NULL) {
        return -1;
    }

    if (number_len != iccid_len) {
        return -1;
    }

    memset(iccid, '\0', iccid_len);
    memset((char *)record, '\0', sizeof(record));
    num_str_len = strlen(number);

    if (num_str_len == 19 || num_str_len == 20) {
        /* no need to convert */
        strncpy(iccid, number, num_str_len);
        return 0;
    } else if (num_str_len >= 21 && num_str_len <= 28) {
        char_cnt = (num_str_len - 19) / 2;
        char_encode_start_pos = num_str_len - char_cnt * 3;
    } else {
        return -1;
    }

    /* parse number into extra record */
    for (i = 0; i < char_cnt; i++) {
        strncpy(decimal_str, number + char_encode_start_pos + i * 3, 3);
        decimal_str[3] = '\0';
        decimal_val = atoi(decimal_str);

        for (j = 3; j >= 0; j--) {
            senary_val[3 - j] = decimal_val / _sim_power(6, j);
            decimal_val = decimal_val  % _sim_power(6, j);
        }

        record[i].pos = senary_val[0] * 6 + senary_val[1];

        if (record[i].pos < 1 || record[i].pos > 20) {
            return -1;
        }

        int char_idx;
        char_idx = senary_val[2] * 6 + senary_val[3];

        if (char_idx < 0 || char_idx > 25) {
            return -1;
        }

        record[i].val = 'A' + char_idx;
    }

    strncpy(iccid, number, char_encode_start_pos);

    /* loop to update iccid */
    for (i = 0; i < char_cnt; i++) {

        for (j = char_encode_start_pos; j >= record[i].pos - 1; j--) {
            iccid[j + 1] = iccid[j];
        }

        iccid[record[i].pos - 1] = record[i].val;
    }

    return 0;
}

__attribute__((weak)) int aliyun_iot_smartcard_hal_cscs(void)
{
    return 0;
}

static void _sim_remove_quote(char *str, int length)
{
    int i;
    int len;

    len = strlen(str);

    if (len > length || len < 3) {
        return;
    }

    if (str[0] == '"' && str[len - 1] == '"') {

        for (i = 0; i < len - 2; i++) {
            str[i] = str[i + 1];
        }

        str[len - 2] = '\0';
    }

    return;
}

/* if name or number are with quote, remove it */
static void _sim_shrink_cpbr_result(char *name, int name_length, char *number, int number_length)
{
    _sim_remove_quote(name, name_length);
    _sim_remove_quote(number, name_length);

    return;

}

/* called by sim_card_init_cb */
static int sim_alicard_detect()
{
    int i;
    char name[32];
    char number[32];
    char iccid[32];
    aliyun_iot_smartcard_carrier_type_t car_type;
    char car_name[12];

    int ret = -1;

    if (g_ali_card.simLoad) {
        return 0;
    }

    g_ali_card.cmcc_slot = -1;
    g_ali_card.cucc_slot = -1;
    g_ali_card.ctcc_slot = -1;
    g_ali_card.slot[0].slot = -1;
    g_ali_card.slot[1].slot = -1;
    g_ali_card.slot[2].slot = -1;

    /* set phonebook storage to sim */
    ret = aliyun_iot_smartcard_hal_cpbs();

    if (ret != 0) {
        mn_log_err("aliyun_iot_smartcard_hal_cpbs failed!");
        return ret;
    }

    /* set TE Character Set to GSM */
    ret = aliyun_iot_smartcard_hal_cscs();

    if (ret != 0) {
        mn_log_err("aliyun_iot_smartcard_hal_cscs failed!");
        return ret;
    }

    /* parse ali iccid */
    ret = aliyun_iot_smartcard_hal_cpbr(1, name, sizeof(name), number, sizeof(number));

    if (ret != 0) {
        mn_log_err("aliyun_iot_smartcard_hal_cpbr failed!");
        return MN_ERROR_CHECK;
    }

    _sim_shrink_cpbr_result(name, sizeof(name), number, sizeof(number));

    mn_log_info("get ali iccid name %s number %s !", name, number);

    if (strncmp(name, "ALIICCID", strlen("ALIICCID")) == 0) {
        if (_sim_parse_alibaba_iccid(number) != 0) {
            mn_log_err("parse_alibaba_iccid failed!");
            return MN_ERROR_CHECK;
        }
    } else {
        return MN_ERROR_CHECK;
    }

    /* parse sim1, sim2, sim3 */
    for (i = 0; i < MAX_SIM_SLOT_CNT; i++) {

        if (dummy_slot[i] == 1) {
            mn_log_info("slot %d is null", i);
            continue;
        }

        ret = aliyun_iot_smartcard_hal_cpbr(i + 2, name, sizeof(name), number, sizeof(number));

        if (ret != 0) {
            mn_log_err("aliyun_iot_smartcard_hal_cpbr failed!");
            return MN_ERROR_CHECK;
        }

        _sim_shrink_cpbr_result(name, sizeof(name), number, sizeof(number));

        ret = _sim_convert_number_to_iccid(number, sizeof(number), iccid, sizeof(iccid));

        if (ret != 0) {
            mn_log_err("number %s is illegal!", number);
            return MN_ERROR_CHECK;
        }

        char cpbr_text[32];

        snprintf(cpbr_text, sizeof(cpbr_text), "SIM%d", i + 1);

        if (strncmp(name, cpbr_text, strlen(cpbr_text)) == 0) {
            g_ali_card.slot[i].slot = i;
            strncpy(g_ali_card.slot[i].iccid , iccid, sizeof(g_ali_card.slot[i].iccid) - 1);

            if (mn_parse_carrier_from_iccid(g_ali_card.slot[i].iccid, &car_type, car_name) != 0) {
                mn_log_err("unknown %s iccid!", cpbr_text);
                return MN_ERROR_CHECK;
            }

            if (car_type == CHINA_MOBILE && g_ali_card.cmcc_slot != i) {
                mn_log_err("cmcc slot not match to aliiccid!");
                return MN_ERROR_CHECK;
            }

            if (car_type == CHINA_UNICOM && g_ali_card.cucc_slot != i) {
                mn_log_err("cucc slot not match to aliiccid!");
                return MN_ERROR_CHECK;
            }

            if (car_type == CHINA_TELECOM && g_ali_card.ctcc_slot != i) {
                mn_log_err("ctcc slot not match to aliiccid!");
                return MN_ERROR_CHECK;
            }

            g_ali_card.slot[i].carrier = car_type;

        } else {
            return MN_ERROR_CHECK;
        }

        mn_log_info("get sim iccid name %s number %s !", name, number);
    }

    g_ali_card.simLoad = true;

    return 0;

}

static int sim_card_init_cb()
{
    memset(&g_ali_card, 0 , sizeof(g_ali_card));

    return sim_alicard_detect();
}

static sim_callbacks_st alicard_callback = {
    sim_card_init_cb,
    sim_switch_to_next_card_cb,
    sim_get_cur_slot_cb,
    sim_is_switch_support_cb,
    sim_get_slot_cnt_cb
};

int sim_get_alicard_cb(sim_callbacks_st **cb)
{
    *cb = &alicard_callback;
    return 0;
}
