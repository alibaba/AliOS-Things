/*
 * cc936.h
 *
 *  Created on: 2017-5-24
 *      Author: bo.wang
 */

#ifndef CC936_H_
#define CC936_H_
#include "integer.h"
#include "ffconf.h"

#if  _CODE_PAGE == 936
WCHAR ff_convert (	/* Converted code, 0 means conversion error */
    WCHAR	chr,	/* Character code to be converted */
    UINT	dir		/* 0: Unicode to OEM code, 1: OEM code to Unicode */
);

WCHAR ff_wtoupper (	/* Returns upper converted character */
    WCHAR chr		/* Unicode character to be upper converted (BMP only) */
);
#endif
#endif /* CC936_H_ */
