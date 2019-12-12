/**
 * @file hexdump.c  Hexadecimal dumping
 *
 * Copyright (C) 2010 Creytiv.com
 */
#include <ctype.h>
#include <re_types.h>
#include <re_fmt.h>


/**
 * Hexadecimal dump of binary buffer. Similar output to HEXDUMP(1)
 *
 * @param f   File stream for output (e.g. stderr, stdout)
 * @param p   Pointer to data
 * @param len Number of bytes
 */
void hexdump(FILE *f, const void *p, size_t len)
{
	const uint8_t *buf = p;
	uint32_t j;
	size_t i;

	if (!f || !buf)
		return;

	for (i=0; i < len; i += 16) {

		(void)re_fprintf(f, "%08x ", i);

		for (j=0; j<16; j++) {
			const size_t pos = i+j;
			if (pos < len)
				(void)re_fprintf(f, " %02x", buf[pos]);
			else
				(void)re_fprintf(f, "   ");

			if (j == 7)
				(void)re_fprintf(f, "  ");
		}

		(void)re_fprintf(f, "  |");

		for (j=0; j<16; j++) {
			const size_t pos = i+j;
			uint8_t v;
			if (pos >= len)
				break;
			v = buf[pos];
			(void)re_fprintf(f, "%c", isprint(v) ? v : '.');
			if (j == 7)
				(void)re_fprintf(f, " ");
		}

		(void)re_fprintf(f, "|\n");
	}
}
