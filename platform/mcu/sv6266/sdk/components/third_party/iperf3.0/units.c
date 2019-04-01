/*---------------------------------------------------------------
 * Copyright (c) 1999,2000,2001,2002,2003
 * The Board of Trustees of the University of Illinois
 * All Rights Reserved.
 *---------------------------------------------------------------
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software (Iperf) and associated
 * documentation files (the "Software"), to deal in the Software
 * without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute,
 * sublicense, and/or sell copies of the Software, and to permit
 * persons to whom the Software is furnished to do
 * so, subject to the following conditions:
 *
 *
 * Redistributions of source code must retain the above
 * copyright notice, this list of conditions and
 * the following disclaimers.
 *
 *
 * Redistributions in binary form must reproduce the above
 * copyright notice, this list of conditions and the following
 * disclaimers in the documentation and/or other materials
 * provided with the distribution.
 *
 *
 * Neither the names of the University of Illinois, NCSA,
 * nor the names of its contributors may be used to endorse
 * or promote products derived from this Software without
 * specific prior written permission.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE CONTIBUTORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 * ________________________________________________________________
 * National Laboratory for Applied Network Research
 * National Center for Supercomputing Applications
 * University of Illinois at Urbana-Champaign
 * http://www.ncsa.uiuc.edu
 * ________________________________________________________________
 *
 * stdio.c
 * by Mark Gates <mgates@nlanr.net>
 * and Ajay Tirumalla <tirumala@ncsa.uiuc.edu>
 * -------------------------------------------------------------------
 * input and output numbers, converting with kilo, mega, giga
 * ------------------------------------------------------------------- */
//#include <os_wrapper.h>

#include "iperf.h"

//#if (IPERF3_ENABLE == 1)

#ifdef __cplusplus
extern    "C"
{
#endif

/* -------------------------------------------------------------------
 * constants for byte_printf
 * ------------------------------------------------------------------- */

/* used as indices into conversion_bytes[], label_byte[], and label_bit[] */
    enum
    {
	UNIT_CONV,
	KILO_CONV,
	MEGA_CONV,
	GIGA_CONV
    };

/* factor to multiply the number by */
    const s32 conversion_bytes[] =
    {
	1,			/* unit */
	1024,		/* kilo */
	1024 * 1024,	/* mega */
	1024 * 1024 * 1024/* giga */
    };

/* factor to multiply the number by for bits*/
    const s32 conversion_bits[] =
    {
	1,			/* unit */
	1000,		/* kilo */
	1000 * 1000,	/* mega */
	1000 * 1000 * 1000/* giga */
    };

/* labels for Byte formats [KMG] */
    const char *label_byte[] =
    {
	"Byte",
	"KByte",
	"MByte",
	"GByte"
    };

/* labels for bit formats [kmg] */
    const char *label_bit[] =
    {
	"bit",
	"Kbit",
	"Mbit",
	"Gbit"
    };

/* -------------------------------------------------------------------
 * unit_snprintf
 *
 * Given a number in bytes and a format, converts the number and
 * prints it out with a bits or bytes label.
 *   B, K, M, G, A for Byte, Kbyte, Mbyte, Gbyte, adaptive byte
 *   b, k, m, g, a for bit,  Kbit,  Mbit,  Gbit,  adaptive bit
 * adaptive picks the "best" one based on the number.
 * s should be at least 11 chars long
 * (4 digits + space + 5 chars max + null)
 * ------------------------------------------------------------------- */

u32 unit_atoi_rate(const char *s)
{
    u32  n;
    char      suffix = '\0';

    sscanf(s, "%d%c", (int *)&n, &suffix);

/* convert according to [Gg Mm Kk] */
    switch    (suffix)
    {
        case 'g': case 'G':
            n *= 1024*1024*1024;
            break;
        case 'm': case 'M':
            n *= 1024*1024;
            break;
        case 'k': case 'K':
            n *= 1024;
            break;
        default:
            break;
    }
    return n;
}               /* end unit_atof_rate */

#ifdef snprintf
#undef snprintf
#endif

#ifdef IPERF_SUPPORT_DOUBLE
void      unit_snprintf(char *s, int inLen,
			              double inNum, char inFormat)
{
	int       conv;
	const char *suffix;
	const char *format;

	/* convert to bits for [bkmga] */
    //if (('a' <= inFormat) && (inFormat <= 'z'))
	if (!ISUPPER((int) inFormat))
	{
	    inNum *= 8;
	}
	switch    (TOUPPER((u8)inFormat))
	{
    	case 'B':
    	    conv = UNIT_CONV;
    	    break;
    	case 'K':
    	    conv = KILO_CONV;
    	    break;
    	case 'M':
    	    conv = MEGA_CONV;
    	    break;
    	case 'G':
    	    conv = GIGA_CONV;
    	    break;

    	default:
    	case 'A':
    	    {
    		double    tmpNum = inNum;
    		conv = UNIT_CONV;

    		if (ISUPPER((int) inFormat))
    		{
    		    while (tmpNum >= 1024.0 && conv <= GIGA_CONV)
    		    {
    			    tmpNum /= 1024.0;
    			    conv++;
    		    }
    		} else
    		{
    		    while (tmpNum >= 1000.0 && conv <= GIGA_CONV)
    		    {
    			    tmpNum /= 1000.0;
    			    conv++;
    		    }
    		}
    		break;
	    }
	}

    //if (('a' <= inFormat) && (inFormat <= 'z'))
	if (!ISUPPER((int) inFormat))
	{
	    inNum /= conversion_bits[conv];
	    suffix = label_bit[conv];
	} else
	{
	    inNum /= conversion_bytes[conv];
	    suffix = label_byte[conv];
	}

	/* print such that we always fit in 4 places */
	if (inNum < 9.995)
	{			/* 9.995 would be rounded to 10.0 */
	    format = "%4.2f %s";/* #.## */
	} else if (inNum < 99.95)
	{			/* 99.95 would be rounded to 100 */
	    format = "%4.1f %s";/* ##.# */
	} else if (inNum < 999.5)
	{			/* 999.5 would be rounded to 1000 */
	    format = "%4.0f %s";/* ### */
	} else
	{			/* 1000-1024 fits in 4 places If not using
				 * Adaptive sizes then this code will not
				 * control spaces */
	    format = "%4.0f %s";/* #### */
	}
	snprintf(s, inLen, format, inNum, suffix);
}				/* end unit_snprintf */

#else

void      unit_snprintf(char *s, int inLen,
			              iperf_size_t inNum, char inFormat)
{
	int       conv;
	int       frac = 0;
	char      str_num[20];
	const char *suffix;
	const char *format;

	/* convert to bits for [bkmga] */
    //if (('a' <= inFormat) && (inFormat <= 'z'))
	if (!ISUPPER((int) inFormat))
	{
	    inNum *= 8;
	}
	switch    (TOUPPER((u8)inFormat))
	{
    	case 'B':
    	    conv = UNIT_CONV;
    	    break;
    	case 'K':
    	    conv = KILO_CONV;
    	    break;
    	case 'M':
    	    conv = MEGA_CONV;
    	    break;
    	case 'G':
    	    conv = GIGA_CONV;
    	    break;

    	case 'A':
    	default:
    	    {
    		iperf_size_t    tmpNum = inNum;
    		conv = UNIT_CONV;

    		if (ISUPPER((int) inFormat)){
    		    while (tmpNum >= 1024 && conv <= GIGA_CONV)
    		    {
    			    tmpNum /= 1024;
    			    conv++;
    		    }
    		}else{
    		    while (tmpNum >= 1000 && conv <= GIGA_CONV)
    		    {
    			    tmpNum /= 1000;
    			    conv++;
    		    }
    		}
	        }
   		    break;
	}

	if (!ISUPPER((int) inFormat))
	{
        frac = inNum % conversion_bits[conv];
        if (conv > 0)
            frac = frac /conversion_bits[conv-1]; 
	    inNum /= conversion_bits[conv];
	    suffix = label_bit[conv];        
	} else{
        frac = inNum % conversion_bytes[conv];
        if (conv > 0)
            frac = frac /conversion_bytes[conv-1]; 
	    inNum /= conversion_bytes[conv];
	    suffix = label_byte[conv];
	}

	/* print such that we always fit in 4 places */
	if (inNum < 10){
        frac /= 10;
	    sprintf(str_num, "%1d.%02d", (int)inNum, frac);  /* #.##*/
	} else if (inNum < 100){
        frac /= 100;
	    sprintf(str_num, "%2d.%01d", (int)inNum, frac); /* ##.#*/
	} else if (inNum < 1000){
	    sprintf(str_num, "%3d.", (int)inNum); /* ###*/
	} else{
	    sprintf(str_num, "%d", (int)inNum);  /* #### ???*/
	}
    format = "%s %s";       
	sprintf(s, format, str_num, suffix);
}

#endif

#ifdef __cplusplus
}				/* end extern "C" */

#endif
//#endif
