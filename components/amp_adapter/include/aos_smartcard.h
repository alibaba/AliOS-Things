/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#ifndef _AOS_SMARTCARD_H_
#define _AOS_SMARTCARD_H_


typedef enum {
    AOS_SMARTCARD_ACT_NULL = 0,
    AOS_SMARTCARD_ACT_2G,
    AOS_SMARTCARD_ACT_3G,
    AOS_SMARTCARD_ACT_4G
} aos_smartcard_act_t;

/**
 * Configure smartcard
 *
 * @param[in]  module_type - module type buffer
 *             size - module type buffer size
 *             reset_while_switch - flag to enable/disable reset while switch
 *             card_switch_time - card switch time
 *             module_startup_time - module startup time
 *
 * @return  0 : on success, -1 : if an error occurred with any step
 */
int aos_smartcard_configure(char *module_type, int size, int *reset_while_switch, unsigned short *card_switch_time, unsigned short *module_startup_time);

/**
 * Get imei
 *
 * @param[in]  imei - imei buffer, size - imei buffer size
 *
 * @return  0 : on success, -1 : if an error occurred with any step
 */
int aos_smartcard_get_imei(char *imei, int size);

/**
 * Get iccid
 *
 * @param[in]  iccid - iccid buffer, size - iccid buffer size
 *
 * @return  0 : on success, -1 : if an error occurred with any step
 */
int aos_smartcard_get_iccid(char *iccid, int size);

/**
 * Get rssi
 *
 * @param[in]  rssi - rssi buffer
 *
 * @return  0 : on success, -1 : if an error occurred with any step
 */
int aos_smartcard_get_rssi(int *rssi);

/**
 * Get act
 *
 * @param[in]  act - act buffer
 *
 * @return  0 : on success, -1 : if an error occurred with any step
 */
int aos_smartcard_get_act(aos_smartcard_act_t *act);

/**
 * Reset module
 *
 * @return  0 : on success, -1 : if an error occurred with any step
 */
int aos_smartcard_reset_module(void);

/**
 * Update phonebook recordings by slot
 *
 * @param[in]  slot - phone number slot, number - phone number
 *
 * @return  0 : on success, -1 : if an error occurred with any step
 */
int aos_smartcard_cpbw(int slot, char *number);

/**
 * Set phonebook storage location to sim card
 *
 * @return  0 : on success, -1 : if an error occurred with any step
 */
int aos_smartcard_cpbs(void);

/**
 * Read phonebook by slot
 *
 * @param[in]  slot - phone number slot
 *             name - name buffer
 *             name_size - name buffer size
 *             number - number buffer
 *             number_size - number buffer size
 *
 * @return  0 : on success, -1 : if an error occurred with any step
 */
int aos_smartcard_cpbr(int slot, char *name, int name_size, char *number, int number_size);


#endif

