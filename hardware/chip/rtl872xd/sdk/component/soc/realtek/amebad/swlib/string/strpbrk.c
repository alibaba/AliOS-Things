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

/**
 * strpbrk - Find the first occurrence of a set of characters
 * @cs: The string to be searched
 * @ct: The characters to search for
 */
LIBC_ROM_TEXT_SECTION
_LONG_CALL_ char *_strpbrk(const char *cs, const char *ct)
{
        const char *sc1, *sc2;

        for (sc1 = cs; *sc1 != '\0'; ++sc1) {
                for (sc2 = ct; *sc2 != '\0'; ++sc2) {
                        if (*sc1 == *sc2)
                                return (char *)sc1;
                }
        }
        return NULL;
}

