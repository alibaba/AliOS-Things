/* Copyright (C) 2017 RDA Technologies Limited and/or its affiliates("RDA").
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

#ifndef _FOTA_CRC_H_
#define _FOTA_CRC_H_

#include <stdint.h>

// =============================================================================
// fotaCalcCrc
// -----------------------------------------------------------------------------
/// CRC implemented inside FOTA
// =============================================================================
uint32_t fotaCalcCrc(const void *p, unsigned length);

// =============================================================================
// fotaCalcFileCrc
// -----------------------------------------------------------------------------
/// Calc CRC from a file implemented inside FOTA
// =============================================================================
bool fotaCalcFileCrc(const char *fn, uint32_t offset, uint32_t length, uint32_t *pcrc);

#endif
