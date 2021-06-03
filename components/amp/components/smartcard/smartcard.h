/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#ifndef __SMARTCARD_H__
#define __SMARTCARD_H__


typedef enum {
    SMARTCARD_CHANGE_TO_CM = 1,
	SMARTCARD_CHANGE_TO_CU,
	SMARTCARD_CHANGE_TO_CT,
	SMARTCARD_CHANGE_TO_NEXT,
} smartcard_change_operator_t;

/**
 * Smartcard init
 *
 * @return  0 : on success, -1 : if an error occurred with any step
 */
int smartcard_init(void);

/**
 * Smartcard deinit
 *
 * @return  0 : on success, -1 : if an error occurred with any step
 */
int smartcard_deinit(void);

/**
 * Smartcard change operator
 *
 * @param[in]  operator - operator type
 *
 * @return  0 : on success, -1 : if an error occurred with any step
 */
int smartcard_change_operator(smartcard_change_operator_t type);

#endif
