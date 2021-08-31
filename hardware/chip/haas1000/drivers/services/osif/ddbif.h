/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */
#ifndef __DDBIF_H__
#define __DDBIF_H__

#include "bluetooth.h"
#include "me_api.h"

void ddbif_list_saved_flash(void);

bt_status_t ddbif_close(void);

bt_status_t ddbif_add_record(btif_device_record_t *record);

bt_status_t ddbif_open(const bt_bdaddr_t *bdAddr);

bt_status_t ddbif_find_record(const bt_bdaddr_t *bdAddr, btif_device_record_t *record);

bt_status_t ddbif_delete_record(const bt_bdaddr_t *bdAddr);

bt_status_t ddbif_enum_device_records(I16 index, btif_device_record_t *record);

size_t ddbif_device_records_num(void);
#endif /*__DDBIF_H__*/

