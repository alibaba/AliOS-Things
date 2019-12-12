/**
 * @file mbuf.c  Memory buffers
 *
 * Copyright (C) 2010 Creytiv.com
 */
#include <string.h>
#include <re_types.h>
#include <re_fmt.h>
#include <re_mem.h>
#include <re_mbuf.h>


#define DEBUG_MODULE "mbuf"
#define DEBUG_LEVEL 4
#include <re_dbg.h>


enum {DEFAULT_SIZE=512};


static void mbuf_destructor(void *data)
{
	struct mbuf *mb = data;

	mem_deref(mb->buf);
}


/**
 * Allocate a new memory buffer
 *
 * @param size Initial buffer size
 *
 * @return New memory buffer, NULL if no memory
 */
struct mbuf *mbuf_alloc(size_t size)
{
	struct mbuf *mb;

	mb = mem_zalloc(sizeof(*mb), mbuf_destructor);
	if (!mb)
		return NULL;

	if (mbuf_resize(mb, size ? size : DEFAULT_SIZE))
		return mem_deref(mb);

	return mb;
}


/**
 * Allocate a new memory buffer with a reference to another mbuf
 *
 * @param mbr Memory buffer to reference
 *
 * @return New memory buffer, NULL if no memory
 */
struct mbuf *mbuf_alloc_ref(struct mbuf *mbr)
{
	struct mbuf *mb;

	if (!mbr)
		return NULL;

	mb = mem_zalloc(sizeof(*mb), mbuf_destructor);
	if (!mb)
		return NULL;

	mb->buf  = mem_ref(mbr->buf);
	mb->size = mbr->size;
	mb->pos  = mbr->pos;
	mb->end  = mbr->end;

	return mb;
}


/**
 * Initialize a memory buffer
 *
 * @param mb Memory buffer to initialize
 */
void mbuf_init(struct mbuf *mb)
{
	if (!mb)
		return;

	mb->buf  = NULL;
	mb->size = 0;
	mb->pos  = 0;
	mb->end  = 0;
}


/**
 * Reset a memory buffer
 *
 * @param mb Memory buffer to reset
 */
void mbuf_reset(struct mbuf *mb)
{
	if (!mb)
		return;

	mb->buf = mem_deref(mb->buf);
	mbuf_init(mb);
}


/**
 * Resize a memory buffer
 *
 * @param mb   Memory buffer to resize
 * @param size New buffer size
 *
 * @return 0 if success, otherwise errorcode
 */
int mbuf_resize(struct mbuf *mb, size_t size)
{
	uint8_t *buf;

	if (!mb)
		return EINVAL;

	buf = mb->buf ? mem_realloc(mb->buf, size) : mem_alloc(size, NULL);
	if (!buf)
		return ENOMEM;

	mb->buf  = buf;
	mb->size = size;

	return 0;
}


/**
 * Trim unused trailing bytes in a memory buffer, resize if necessary
 *
 * @param mb   Memory buffer to trim
 */
void mbuf_trim(struct mbuf *mb)
{
	int err;

	if (!mb || !mb->end || mb->end == mb->size)
		return;

	/* We shrink - this cannot fail */
	err = mbuf_resize(mb, mb->end);
	if (err) {
		DEBUG_WARNING("trim: resize failed (%m)\n", err);
	}
}


/**
 * Shift mbuf content position
 *
 * @param mb    Memory buffer to shift
 * @param shift Shift offset count
 *
 * @return 0 if success, otherwise errorcode
 */
int mbuf_shift(struct mbuf *mb, ssize_t shift)
{
	size_t rsize;
	uint8_t *p;

	if (!mb)
		return EINVAL;

	if (((ssize_t)mb->pos + shift) < 0 ||
	    ((ssize_t)mb->end + shift) < 0)
		return ERANGE;

	rsize = mb->end + shift;

	if (rsize > mb->size) {

		int err;

		err = mbuf_resize(mb, rsize);
		if (err)
			return err;
	}

	p = mbuf_buf(mb);

	memmove(p + shift, p, mbuf_get_left(mb));

	mb->pos += shift;
	mb->end += shift;

	return 0;
}


/**
 * Write a block of memory to a memory buffer
 *
 * @param mb   Memory buffer
 * @param buf  Memory block to write
 * @param size Number of bytes to write
 *
 * @return 0 if success, otherwise errorcode
 */
int mbuf_write_mem(struct mbuf *mb, const uint8_t *buf, size_t size)
{
	size_t rsize;

	if (!mb || !buf)
		return EINVAL;

	rsize = mb->pos + size;

	if (rsize > mb->size) {
		const size_t dsize = mb->size ? (mb->size * 2)
			: DEFAULT_SIZE;
		int err;

		err = mbuf_resize(mb, MAX(rsize, dsize));
		if (err)
			return err;
	}

	memcpy(mb->buf + mb->pos, buf, size);

	mb->pos += size;
	mb->end  = MAX(mb->end, mb->pos);

	return 0;
}


/**
 * Write an 8-bit value to a memory buffer
 *
 * @param mb Memory buffer
 * @param v  8-bit value to write
 *
 * @return 0 if success, otherwise errorcode
 */
int mbuf_write_u8(struct mbuf *mb, uint8_t v)
{
	return mbuf_write_mem(mb, (uint8_t *)&v, sizeof(v));
}


/**
 * Write a 16-bit value to a memory buffer
 *
 * @param mb Memory buffer
 * @param v  16-bit value to write
 *
 * @return 0 if success, otherwise errorcode
 */
int mbuf_write_u16(struct mbuf *mb, uint16_t v)
{
	return mbuf_write_mem(mb, (uint8_t *)&v, sizeof(v));
}


/**
 * Write a 32-bit value to a memory buffer
 *
 * @param mb Memory buffer
 * @param v  32-bit value to write
 *
 * @return 0 if success, otherwise errorcode
 */
int mbuf_write_u32(struct mbuf *mb, uint32_t v)
{
	return mbuf_write_mem(mb, (uint8_t *)&v, sizeof(v));
}


/**
 * Write a 64-bit value to a memory buffer
 *
 * @param mb Memory buffer
 * @param v  64-bit value to write
 *
 * @return 0 if success, otherwise errorcode
 */
int mbuf_write_u64(struct mbuf *mb, uint64_t v)
{
	return mbuf_write_mem(mb, (uint8_t *)&v, sizeof(v));
}


/**
 * Write a null-terminated string to a memory buffer
 *
 * @param mb  Memory buffer
 * @param str Null terminated string to write
 *
 * @return 0 if success, otherwise errorcode
 */
int mbuf_write_str(struct mbuf *mb, const char *str)
{
	if (!str)
		return EINVAL;

	return mbuf_write_mem(mb, (const uint8_t *)str, strlen(str));
}


/**
 * Write a pointer-length string to a memory buffer
 *
 * @param mb  Memory buffer
 * @param pl  Pointer-length string
 *
 * @return 0 if success, otherwise errorcode
 */
int mbuf_write_pl(struct mbuf *mb, const struct pl *pl)
{
	if (!pl)
		return EINVAL;

	return mbuf_write_mem(mb, (const uint8_t *)pl->p, pl->l);
}


/**
 * Read a block of memory from a memory buffer
 *
 * @param mb   Memory buffer
 * @param buf  Buffer to read data to
 * @param size Size of buffer
 *
 * @return 0 if success, otherwise errorcode
 */
int mbuf_read_mem(struct mbuf *mb, uint8_t *buf, size_t size)
{
	if (!mb || !buf)
		return EINVAL;

	if (size > mbuf_get_left(mb)) {
		DEBUG_WARNING("tried to read beyond mbuf end (%u > %u)\n",
			      size, mbuf_get_left(mb));
		return EOVERFLOW;
	}

	memcpy(buf, mb->buf + mb->pos, size);

	mb->pos += size;

	return 0;
}


/**
 * Read an 8-bit value from a memory buffer
 *
 * @param mb Memory buffer
 *
 * @return 8-bit value
 */
uint8_t mbuf_read_u8(struct mbuf *mb)
{
	uint8_t v;

	return (0 == mbuf_read_mem(mb, &v, sizeof(v))) ? v : 0;
}


/**
 * Read a 16-bit value from a memory buffer
 *
 * @param mb Memory buffer
 *
 * @return 16-bit value
 */
uint16_t mbuf_read_u16(struct mbuf *mb)
{
	uint16_t v;

	return (0 == mbuf_read_mem(mb, (uint8_t *)&v, sizeof(v))) ? v : 0;
}


/**
 * Read a 32-bit value from a memory buffer
 *
 * @param mb Memory buffer
 *
 * @return 32-bit value
 */
uint32_t mbuf_read_u32(struct mbuf *mb)
{
	uint32_t v;

	return (0 == mbuf_read_mem(mb, (uint8_t *)&v, sizeof(v))) ? v : 0;
}


/**
 * Read a 64-bit value from a memory buffer
 *
 * @param mb Memory buffer
 *
 * @return 64-bit value
 */
uint64_t mbuf_read_u64(struct mbuf *mb)
{
	uint64_t v;

	return (0 == mbuf_read_mem(mb, (uint8_t *)&v, sizeof(v))) ? v : 0;
}


/**
 * Read a string from a memory buffer
 *
 * @param mb   Memory buffer
 * @param str  Buffer to read string to
 * @param size Size of buffer
 *
 * @return 0 if success, otherwise errorcode
 */
int mbuf_read_str(struct mbuf *mb, char *str, size_t size)
{
	if (!mb || !str)
		return EINVAL;

	while (size--) {
		const uint8_t c = mbuf_read_u8(mb);
		*str++ = c;
		if ('\0' == c)
			break;
	}

	return 0;
}


/**
 * Duplicate a null-terminated string from a memory buffer
 *
 * @param mb   Memory buffer
 * @param strp Pointer to destination string; allocated and set
 * @param len  Length of string
 *
 * @return 0 if success, otherwise errorcode
 */
int mbuf_strdup(struct mbuf *mb, char **strp, size_t len)
{
	char *str;
	int err;

	if (!mb || !strp)
		return EINVAL;

	str = mem_alloc(len + 1, NULL);
	if (!str)
		return ENOMEM;

	err = mbuf_read_mem(mb, (uint8_t *)str, len);
	if (err)
		goto out;

	str[len] = '\0';

 out:
	if (err)
		mem_deref(str);
	else
		*strp = str;

	return err;
}


static int vprintf_handler(const char *p, size_t size, void *arg)
{
	struct mbuf *mb = arg;

	return mbuf_write_mem(mb, (const uint8_t *)p, size);
}


/**
 * Print a formatted variable argument list to a memory buffer
 *
 * @param mb  Memory buffer
 * @param fmt Formatted string
 * @param ap  Variable argument list
 *
 * @return 0 if success, otherwise errorcode
 */
int mbuf_vprintf(struct mbuf *mb, const char *fmt, va_list ap)
{
	return re_vhprintf(fmt, ap, vprintf_handler, mb);
}


/**
 * Print a formatted string to a memory buffer
 *
 * @param mb  Memory buffer
 * @param fmt Formatted string
 *
 * @return 0 if success, otherwise errorcode
 */
int mbuf_printf(struct mbuf *mb, const char *fmt, ...)
{
	int err = 0;
	va_list ap;

	va_start(ap, fmt);
	err = re_vhprintf(fmt, ap, vprintf_handler, mb);
	va_end(ap);

	return err;
}


/**
 * Write a pointer-length string to a memory buffer, excluding a section
 *
 * @param mb   Memory buffer
 * @param pl   Pointer-length string
 * @param skip Part of pl to exclude
 *
 * @return 0 if success, otherwise errorcode
 *
 * @todo: create substf variante
 */
int mbuf_write_pl_skip(struct mbuf *mb, const struct pl *pl,
		       const struct pl *skip)
{
	struct pl r;
	int err;

	if (!pl || !skip)
		return EINVAL;

	if (pl->p > skip->p || (skip->p + skip->l) > (pl->p + pl->l))
		return ERANGE;

	r.p = pl->p;
	r.l = skip->p - pl->p;

	err = mbuf_write_mem(mb, (const uint8_t *)r.p, r.l);
	if (err)
		return err;

	r.p = skip->p + skip->l;
	r.l = pl->p + pl->l - r.p;

	return mbuf_write_mem(mb, (const uint8_t *)r.p, r.l);
}


/**
 * Write n bytes of value 'c' to a memory buffer
 *
 * @param mb   Memory buffer
 * @param c    Value to write
 * @param n    Number of bytes to write
 *
 * @return 0 if success, otherwise errorcode
 */
int mbuf_fill(struct mbuf *mb, uint8_t c, size_t n)
{
	size_t rsize;

	if (!mb || !n)
		return EINVAL;

	rsize = mb->pos + n;

	if (rsize > mb->size) {
		const size_t dsize = mb->size ? (mb->size * 2)
			: DEFAULT_SIZE;
		int err;

		err = mbuf_resize(mb, MAX(rsize, dsize));
		if (err)
			return err;
	}

	memset(mb->buf + mb->pos, c, n);

	mb->pos += n;
	mb->end  = MAX(mb->end, mb->pos);

	return 0;
}


/**
 * Debug the memory buffer
 *
 * @param pf Print handler
 * @param mb Memory buffer
 *
 * @return 0 if success, otherwise errorcode
 */
int mbuf_debug(struct re_printf *pf, const struct mbuf *mb)
{
	if (!mb)
		return 0;

	return re_hprintf(pf, "buf=%p pos=%zu end=%zu size=%zu",
			  mb->buf, mb->pos, mb->end, mb->size);
}
