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
#include "ameba_soc.h"
#include "rtl8721d_loguart.h"
#include "section_config.h"
#include <strproc.h>
#include "diag.h"

HAL_ROM_BSS_SECTION DIAG_PRINT_BUF_FUNC ConfigDebugBufferGet;
HAL_ROM_BSS_SECTION u32 ConfigDebugBuffer;
HAL_ROM_BSS_SECTION u32 ConfigDebugClose;

HAL_ROM_BSS_SECTION u32 ConfigDebug[LEVEL_NUMs];

HAL_ROM_TEXT_SECTION
int DiagVSprintf(char *buf, const char *fmt, const int *dp)
{
	char *p, *s;
	s = buf;

	for(; *fmt != '\0'; ++fmt) {
		if(*fmt != '%') {
			if(buf) {
				*s++ = *fmt;
			} else {
				DiagPutChar(*fmt);
			}

			continue;
		}

		if(*++fmt == 's') {
			for(p = (char *)*dp++; *p != '\0'; p++) {
				if(buf) {
					*s++ = *p;
				} else {
					DiagPutChar(*p);
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
#if 1   //wei patch for %02x

			if((*fmt  >= '0') && (*fmt  <= '9'))
			{
				int width;
				unsigned char fch = *fmt;

				for(width=0; (fch>='0') && (fch<='9'); fch=*++fmt)
				{	width = width * 10 + fch - '0';
				}

				shift=(width-1)*4;
			}

#endif

			/*
			 * Before each format q points to tmp buffer
			 * After each format q points past end of item
			 */

			if((*fmt == 'x')||(*fmt == 'X') || (*fmt == 'p') || (*fmt == 'P')) {
				/* With x86 gcc, sizeof(long) == sizeof(int) */
				const long *lp = (const long *)dp;
				unsigned long h = *lp++;
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

				//hback 是实际得到的数据，hex_count是统计数据的HEX字符个数
				while(h_back) {
					hex_count += 1;
					h_back  = h_back >> 4;
				}

				//这里修复 example：  字符有4个，但是用了%02x导致字符被截断的情况
				if(shift < (hex_count - 1)*4)
					shift = (hex_count - 1)*4;

				//printf("(%d,%d)", hex_count, shift);

				for(; shift >= 0; shift -= 4) {

					*q++ = "0123456789ABCDEF"[(h >> shift) & 0xF] | ncase;
				}

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

				//这里修复 example：用了%08d后，在数字前面没有0的情况
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
			else
				*q++ = *fmt;

			/* now output the saved string */
			for(p = tmp; p < q; ++p) {
				if(buf) {
					*s++ = *p;
				} else {
					DiagPutChar(*p);
				}

				if((*p) == '\n') {
					DiagPutChar('\r');
				}
			}
		}
	}

	if(buf)
		*s = '\0';

	return (s - buf);
}

HAL_ROM_TEXT_SECTION
_LONG_CALL_ u32 
DiagPrintf(
    IN  const char *fmt, ...
)
{
	log_buffer_t *buf = NULL;
	int ret = 0;
	
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

HAL_ROM_TEXT_SECTION
_LONG_CALL_ u32 
DiagPrintfD(
    IN  const char *fmt, ...
)
{
	//log_buffer_t *buf = NULL;
	
	if (ConfigDebugClose == 1)
		return 0;

	return DiagVSprintf(NULL, fmt, ((const int *)&fmt)+1);
}

HAL_ROM_TEXT_SECTION
u32 
DiagSPrintf(
    IN  u8 *buf,
    IN  const char *fmt, ...
)
{
	if (ConfigDebugClose == 1)
		return 0;
	
	return DiagVSprintf((char*)buf, fmt, ((const int *)&fmt)+1);	
}




HAL_ROM_TEXT_SECTION
int DiagSnPrintf(char *buf, size_t size, const char *fmt, ...)
{

	va_list     ap;
	char *p, *s, *buf_end = NULL;
	const int *dp = ((const int *)&fmt)+1;

	if(buf == NULL)
		return 0;


	va_start(ap, fmt);
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
			else
				*q++ = *fmt;

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

	va_end(ap);
	return(s-buf);

}

