/*
 * ccsbcs.h
 *
 *  Created on: 2017-5-24
 *      Author: bo.wang
 */

#ifndef CCSBCS_H_
#define CCSBCS_H_
#include "integer.h"
#include "ffconf.h"

#if _CODE_PAGE != 936
WCHAR ff_convert (	/* Converted character, Returns zero on error */
    WCHAR	chr,	/* Character code to be converted */
    UINT	dir		/* 0: Unicode to OEM code, 1: OEM code to Unicode */
);

WCHAR ff_wtoupper (	/* Returns upper converted character */
    WCHAR chr		/* Unicode character to be upper converted (BMP only) */
);

#endif

#endif /* CCSBCS_H_ */
