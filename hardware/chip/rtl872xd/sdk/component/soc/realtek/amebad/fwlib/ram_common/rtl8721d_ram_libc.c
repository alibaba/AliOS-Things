/*
 *  Routines to access rom software library.
 *
 *  Copyright (c) 2019 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */
 
#include "diag.h"
#include "strproc.h"
#include <stdarg.h>

typedef union _va_int_  {
	va_list ap;
	const int *dp;
} va_int,*pva_int;

int _rtl_vsprintf(char *buf, size_t size, const char *fmt, const int *dp)
{
	char *p, *s, *buf_end = NULL;
	
	if(buf == NULL)
		return 0;
		
	s = buf;
	buf_end = size? (buf + size):(char*)~0;

	for(; *fmt != '\0'; ++fmt) {
		if(*fmt != '%') {
				*s++ = *fmt;

			if(s >= buf_end) {
				goto Exit;
			}

			continue;
		}

		if(*++fmt == 's') {
			for(p = (char *)*dp++; *p != '\0'; p++) {
					*s++ = *p;

				if(s >= buf_end) {
					goto Exit;
				}
			}
		}
		else {	/* Length of item is bounded */
			char tmp[20], *q = tmp;
			int alt = 0;
			int shift = 0;// = 12;
			const long *lpforchk = (const long *)dp;

			if((*lpforchk) < 0x10) {
				shift = 0;
			}
			else if(((*lpforchk) >= 0x10) && ((*lpforchk) < 0x100)) {
				shift = 4;
			}
			else if(((*lpforchk) >= 0x100) && ((*lpforchk) < 0x1000)) {
				shift = 8;
			}
			else if(((*lpforchk) >= 0x1000) && ((*lpforchk) < 0x10000)) {
				shift = 12;
			}
			else if(((*lpforchk) >= 0x10000) && ((*lpforchk) < 0x100000)) {
				shift = 16;
			}
			else if(((*lpforchk) >= 0x100000) && ((*lpforchk) < 0x1000000)) {
				shift = 20;
			}
			else if(((*lpforchk) >= 0x1000000) && ((*lpforchk) < 0x10000000)) {
				shift = 24;
			}
			else if((*lpforchk) >= 0x10000000) {
				shift = 28;
			}
			else {
				shift = 28;
			}

			if((*fmt  >= '0') && (*fmt  <= '9'))
			{
				int width;
				unsigned char fch = *fmt;

				for(width=0; (fch>='0') && (fch<='9'); fch=*++fmt)
				{	width = width * 10 + fch - '0';
				}

				shift=(width-1)*4;
			}


			/*
			 * Before each format q points to tmp buffer
			 * After each format q points past end of item
			 */

			if((*fmt == 'x')||(*fmt == 'X') || (*fmt == 'p') || (*fmt == 'P')) {
				/* With x86 gcc, sizeof(long) == sizeof(int) */
				const long *lp = (const long *)dp;
				long h = *lp++;
				int hex_count = 0;
				unsigned long h_back = h;
				int ncase = (*fmt & 0x20);
				dp = (const int *)lp;

				if((*fmt == 'p') || (*fmt == 'P'))
					alt=1;

				if(alt) {
					*q++ = '0';
					*q++ = 'X' | ncase;
				}

				while(h_back) {
					hex_count += (h_back & 0xF) ? 1 : 0;
					h_back  = h_back >> 4;
				}

				if(shift < (hex_count - 1)*4)
					shift = (hex_count - 1)*4;

				for(; shift >= 0; shift -= 4)
					*q++ = "0123456789ABCDEF"[(h >> shift) & 0xF] | ncase;
			}
			else if(*fmt == 'd') {
				int i = *dp++;
				char *r;
				int digit_space = 0;

				if(i < 0) {
					*q++ = '-';
					i = -i;
					digit_space++;
				}

				p = q;		/* save beginning of digits */

				do {
					*q++ = '0' + (i % 10);
					i /= 10;
					digit_space++;
				} while(i);


				for(; shift >= 0; shift -= 4) {

					if(digit_space-- > 0) {
						; //do nothing
					} else {
						*q++ = '0';
					}
				}

				/* reverse digits, stop in middle */
				r = q;		/* don't alter q */

				while(--r > p) {
					i = *r;
					*r = *p;
					*p++ = i;
				}
			}
			else if(*fmt == 'c')
				*q++ = *dp++;
			else {
				if(*fmt != '%')
					DiagPrintf("%s: format not support!\n", __func__);
				*q++ = *fmt;
			}

			/* now output the saved string */
			for(p = tmp; p < q; ++p) {
					*s++ = *p;

				if(s >= buf_end) {
					goto Exit;
				}
			}
		}
	}

Exit:

	if(buf)
		*s = '\0';

	return (s - buf);
}

int _rtl_vsnprintf(char *buf, size_t size, const char *fmt, va_list ap)
{
	int ret;
	va_int m;
	m.ap = ap;
	
	ret = _rtl_vsprintf(buf, size, fmt, m.dp);
	return ret;
}

int _rtl_snprintf(char* str, size_t size, const char* fmt, ...)
{
	int ret;	
	ret = _rtl_vsprintf(str, size, fmt, ((const int *)&fmt)+1);
	return ret;
}

int _rtl_sprintf(char* str, const char* fmt, ...)
{
	const char* fmt1;

	fmt1 = fmt;
	
	for(; *fmt1 != '\0'; ++fmt1) {
		if(*fmt1 != '%')
			continue;
		else
			fmt1 ++;
		
		while(isdigit(*fmt1)){
			fmt1 ++;
		}
		
		if((*fmt1  == 's') || (*fmt1 == 'x') || (*fmt1 == 'X') || (*fmt1 == 'p') || (*fmt1 == 'P') || (*fmt1 == 'd') || (*fmt1 == 'c') || (*fmt1 == '%'))
			continue;
		else {
			DiagPrintf("%s: format not support!\n", __func__);
			break;
		}
	}

	if (ConfigDebugClose == 1)
		return 0;
	
	return DiagVSprintf((char*)str, fmt, ((const int *)&fmt)+1);
}

int _rtl_printf(const char* fmt, ...)
{
	u32 ret;
	const char* fmt1;
	log_buffer_t *buf = NULL;

	fmt1 = fmt;
	
	for(; *fmt1 != '\0'; ++fmt1) {
		if(*fmt1 == '"') {
			do {
				fmt1 ++;
			} while(*fmt1 != '"');
			fmt1 ++;
		}
		
		if(*fmt1 != '%')
			continue;
		else
			fmt1 ++;
		
		while(isdigit(*fmt1)){
			fmt1 ++;
		}
		
		if((*fmt1  == 's') || (*fmt1 == 'x') || (*fmt1 == 'X') || (*fmt1 == 'p') || (*fmt1 == 'P') || (*fmt1 == 'd') || (*fmt1 == 'c') || (*fmt1 == '%'))
			continue;
		else {
			DiagPrintf("%s: format not support!\n", __func__);
			break;
		}
	}
	
	if (ConfigDebugClose == 1)
		return 0;

	if (ConfigDebugBuffer == 1 && ConfigDebugBufferGet != NULL) {
		buf = (log_buffer_t *)ConfigDebugBufferGet(fmt);
	}

	if (buf != NULL) {
		return DiagVSprintf(buf->buffer, fmt, ((const int *)&fmt)+1);
	} else {
		ret = DiagVSprintf(NULL, fmt, ((const int *)&fmt)+1);

		return ret;
	}
}

/*
 * Fast implementation of tolower() for internal usage. Do not use in your
 * code.
 */
static inline char _tolower(const char c)
{
    return c | 0x20;
}

int _rtl_sscanf(const char *buf, const char *fmt, ...)
{
	int ret;
	char nxt_fmt = -1;
	va_list args;
	const char* fmt1;

	fmt1 = fmt;
	
	for(; *fmt1 != '\0'; ++fmt1) {
		if(*fmt1 != '%')
			continue;
		else
			fmt1 ++;

		if(*fmt1 == '*')
			continue;
		
		while(isdigit(*fmt1)){
			fmt1 ++;
		}

		if(*fmt1 == 'h' || _tolower(*fmt1) == 'l' || _tolower(*fmt1) == 'z') {
			nxt_fmt = *fmt1++;
			if(nxt_fmt == *fmt1) {
				if((nxt_fmt == 'h') || (nxt_fmt == 'l'))
					fmt1++;
			}
		}

		if(!*fmt1) {
			break;
		}

		if(*fmt1 == 'n')
			continue;
		
		if((*fmt1  == 'c') || (*fmt1 == 's') || (*fmt1 == 'o') || (*fmt1 == 'x') || (*fmt1 == 'X') || (*fmt1 == 'i') || (*fmt1 == 'd') || (*fmt1 == 'u') || (*fmt1 == '%'))
			continue;
		else {
			DiagPrintf("%s: format not support!\n", __func__);
			break;
		}
	}

	va_start (args, fmt);
	ret = _vsscanf(buf, fmt, args);
	va_end (args);
	return ret;
}
