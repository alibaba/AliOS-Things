/**
 * Copyright (C) 2016 The YunOS Project. All rights reserved.
 */

#ifndef _PROV_PRO_H_
#define _PROV_PRO_H_

#include <stdint.h>

/* prov error code */
#define PROV_ERROR_GENERIC           (0x10000001)
#define PROV_ERROR_BAD_ITEM_MAGIC    (0x10000002)
#define PROV_ERROR_BAD_ITEM_VER      (0x10000003)
#define PROV_ERROR_BAD_REC_NUM       (0x10000004)
#define PROV_ERROR_BAD_REC_MAGIC     (0x10000005)
#define PROV_ERROR_BAD_CHK_SUM       (0x10000007)
#define PROV_ERROR_BAD_IDX           (0x10000008)

//item ID2 name
#define ITEM_NAME_ID2 "item_id2"

/*
 * provision item to the board
 *
 * param: in: item_name: the name of item
 *        in: data: the data of item
 *        in: data_len: the length of the data
 * return: see prov error code
 */

uint32_t prov_item(char *item_name, uint8_t *data, uint32_t data_len);

#endif /* _PROV_PRO_H_ */

