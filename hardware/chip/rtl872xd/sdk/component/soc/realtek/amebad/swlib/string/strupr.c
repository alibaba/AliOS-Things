/*
 *  Routines to access hardware
 *
 *  Copyright (c) 2013 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */

#include "basic_types.h"
#include <stdarg.h>
#include <stddef.h>             /* Compiler defns such as size_t, NULL etc. */
#include "strproc.h"
#include "section_config.h"
#include "diag.h"
#include "ameba_soc.h"


LIBC_ROM_TEXT_SECTION
_LONG_CALL_ u8*
_strupr(
    IN  u8  *string
){
    u8  *pStr;
    const   u8  diff = 'a' - 'A';

    pStr = string;

    while(*pStr){
        if ((*pStr >= 'a') && (*pStr <= 'z')){
            *pStr -= diff;
        }
        pStr++;
    }

    return string;
}
