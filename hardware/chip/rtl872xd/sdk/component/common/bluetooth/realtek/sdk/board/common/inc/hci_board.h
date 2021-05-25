/**
 * Copyright (c) 2017, Realsil Semiconductor Corporation. All rights reserved.
 *
 */
#ifndef _HCI_BOARD_H_
#define _HCI_BOARD_H_

#ifdef __cplusplus
extern "C" {
#endif
#include <stdio.h>
#include <string.h>

bool hci_board_init(void);
void bt_reset(void);
void bt_power_on(void);
void bt_power_off(void);


#ifdef __cplusplus
}
#endif

#endif /* _HCI_BOARD_H_ */
