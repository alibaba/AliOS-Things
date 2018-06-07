/* Copyright (C) 2016 RDA Technologies Limited and/or its affiliates("RDA").
* All rights reserved.
*
* This software is supplied "AS IS" without any warranties.
* RDA assumes no responsibility or liability for the use of the software,
* conveys no license or title under any patent, copyright, or mask work
* right to the product. RDA reserves the right to make changes in the
* software without notification.  RDA also make no representation or
* warranty that such application will be suitable for the specified use
* without further testing or modification.
*/

#ifndef _CRC32_CALC_H_
#define _CRC32_CALC_H_

#include <stdint.h>

uint32_t crc32_init(void);
uint32_t crc32_update(uint32_t crc, const void *p, unsigned len);
uint32_t crc32_calc(const void *p, unsigned len);

#endif
