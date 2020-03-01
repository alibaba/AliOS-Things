/**
 * @file re_mbuf.h  Interface to memory buffers
 *
 * Copyright (C) 2010 Creytiv.com
 */

#ifndef RE_MBUF_H
#define RE_MBUF_H

#include <stdint.h>
#include <stdarg.h>
#include <sys/types.h>
#include "re_common.h"

/**
 * @addtogroup aos_rtp_mbuf rtp_mbuf
 * @{
 */

#ifndef RELEASE
#define MBUF_DEBUG 1  /**< Mbuf debugging (0 or 1) */
#endif

#define MBUF_CHECK_POS(mb)
#define MBUF_CHECK_END(mb)

/** Defines a memory buffer */
struct mbuf {
	uint8_t *buf;   /**< Buffer memory      */
	size_t size;    /**< Size of buffer     */
	size_t pos;     /**< Position in buffer */
	size_t end;     /**< End of buffer      */
};

/**
 * Free a memory buffer
 *
 * @param data A pointer point to a buffer
 *
 */
void mbuf_free(void* data);

/**
 * Allocate a new memory buffer
 *
 * @param[in] size Initial buffer size
 *
 * @return   New memory buffer, NULL if no memory
 *
 */
struct mbuf *mbuf_alloc(size_t size);

/**
 * Allocate a new memory buffer with a reference to another mbuf
 *
 * @param[in] mbr Memory buffer to reference
 *
 * @return New memory buffer, NULL if no memory
 */
struct mbuf *mbuf_alloc_ref(struct mbuf *mbr);

/**
 * Initialize a memory buffer
 *
 * @param[in] mb Memory buffer to initialize
 */
void     mbuf_init(struct mbuf *mb);

/**
 * Reset a memory buffer
 *
 * @param mb Memory buffer to reset
 */
void     mbuf_reset(struct mbuf *mb);

/**
 * Resize a memory buffer
 *
 * @param[in] mb   Memory buffer to resize
 * @param[in] size New buffer size
 *
 * @return 0 if success, otherwise errorcode
 */
int      mbuf_resize(struct mbuf *mb, size_t size);

/**
 * Trim unused trailing bytes in a memory buffer, resize if necessary
 *
 * @param mb   Memory buffer to trim
 */
void     mbuf_trim(struct mbuf *mb);

/**
 * Shift mbuf content position
 *
 * @param mb    Memory buffer to shift
 * @param shift Shift offset count
 *
 * @return 0 if success, otherwise errorcode
 */
int      mbuf_shift(struct mbuf *mb, ssize_t shift);

/**
 * Write a block of memory to a memory buffer
 *
 * @param mb   Memory buffer
 * @param buf  Memory block to write
 * @param size Number of bytes to write
 *
 * @return 0 if success, otherwise errorcode
 */
int      mbuf_write_mem(struct mbuf *mb, const uint8_t *buf, size_t size);

/**
 * Write an 8-bit value to a memory buffer
 *
 * @param mb Memory buffer
 * @param v  8-bit value to write
 *
 * @return 0 if success, otherwise errorcode
 */
int      mbuf_write_u8(struct mbuf *mb, uint8_t v);

/**
 * Write a 16-bit value to a memory buffer
 *
 * @param mb Memory buffer
 * @param v  16-bit value to write
 *
 * @return 0 if success, otherwise errorcode
 */
int      mbuf_write_u16(struct mbuf *mb, uint16_t v);

/**
 * Write a 32-bit value to a memory buffer
 *
 * @param mb Memory buffer
 * @param v  32-bit value to write
 *
 * @return 0 if success, otherwise errorcode
 */
int      mbuf_write_u32(struct mbuf *mb, uint32_t v);

/**
 * Write a 64-bit value to a memory buffer
 *
 * @param mb Memory buffer
 * @param v  64-bit value to write
 *
 * @return 0 if success, otherwise errorcode
 */
int      mbuf_write_u64(struct mbuf *mb, uint64_t v);

/**
 * Write a null-terminated string to a memory buffer
 *
 * @param mb  Memory buffer
 * @param str Null terminated string to write
 *
 * @return 0 if success, otherwise errorcode
 */
int      mbuf_write_str(struct mbuf *mb, const char *str);

/**
 * Write a pointer-length string to a memory buffer
 *
 * @param mb  Memory buffer
 * @param pl  Pointer-length string
 *
 * @return 0 if success, otherwise errorcode
 */
int      mbuf_write_pl(struct mbuf *mb, const struct pl *pl);

/**
 * Read a block of memory from a memory buffer
 *
 * @param mb   Memory buffer
 * @param buf  Buffer to read data to
 * @param size Size of buffer
 *
 * @return 0 if success, otherwise errorcode
 */
int      mbuf_read_mem(struct mbuf *mb, uint8_t *buf, size_t size);

/**
 * Read an 8-bit value from a memory buffer
 *
 * @param mb Memory buffer
 *
 * @return 8-bit value
 */
uint8_t  mbuf_read_u8(struct mbuf *mb);

/**
 * Read a 16-bit value from a memory buffer
 *
 * @param mb Memory buffer
 *
 * @return 16-bit value
 */
uint16_t mbuf_read_u16(struct mbuf *mb);

/**
 * Read a 32-bit value from a memory buffer
 *
 * @param mb Memory buffer
 *
 * @return 32-bit value
 */
uint32_t mbuf_read_u32(struct mbuf *mb);

/**
 * Read a 64-bit value from a memory buffer
 *
 * @param mb Memory buffer
 *
 * @return 64-bit value
 */
uint64_t mbuf_read_u64(struct mbuf *mb);

/**
 * Read a string from a memory buffer
 *
 * @param mb   Memory buffer
 * @param str  Buffer to read string to
 * @param size Size of buffer
 *
 * @return 0 if success, otherwise errorcode
 */
int      mbuf_read_str(struct mbuf *mb, char *str, size_t size);

/**
 * Duplicate a null-terminated string from a memory buffer
 *
 * @param mb   Memory buffer
 * @param strp Pointer to destination string; allocated and set
 * @param len  Length of string
 *
 * @return 0 if success, otherwise errorcode
 */
int      mbuf_strdup(struct mbuf *mb, char **strp, size_t len);

/**
 * Print a formatted variable argument list to a memory buffer
 *
 * @param mb  Memory buffer
 * @param fmt Formatted string
 * @param ap  Variable argument list
 *
 * @return 0 if success, otherwise errorcode
 */
int      mbuf_vprintf(struct mbuf *mb, const char *fmt, va_list ap);

/**
 * Print a formatted string to a memory buffer
 *
 * @param mb  Memory buffer
 * @param fmt Formatted string
 *
 * @return 0 if success, otherwise errorcode
 */
int      mbuf_printf(struct mbuf *mb, const char *fmt, ...);

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
int      mbuf_write_pl_skip(struct mbuf *mb, const struct pl *pl,
			    const struct pl *skip);

/**
 * Write n bytes of value 'c' to a memory buffer
 *
 * @param mb   Memory buffer
 * @param c    Value to write
 * @param n    Number of bytes to write
 *
 * @return 0 if success, otherwise errorcode
 */
int      mbuf_fill(struct mbuf *mb, uint8_t c, size_t n);
int      mbuf_debug(struct re_printf *pf, const struct mbuf *mb);


/**
 * Get the buffer from the current position
 *
 * @param mb Memory buffer
 *
 * @return Current buffer
 */
static inline uint8_t *mbuf_buf(const struct mbuf *mb)
{
	return mb ? mb->buf + mb->pos : (uint8_t *)NULL;
}


/**
 * Get number of bytes left in a memory buffer, from current position to end
 *
 * @param mb Memory buffer
 *
 * @return Number of bytes left
 */
static inline size_t mbuf_get_left(const struct mbuf *mb)
{
	return (mb && (mb->end > mb->pos)) ? (mb->end - mb->pos) : 0;
}


/**
 * Get available space in buffer (size - pos)
 *
 * @param mb Memory buffer
 *
 * @return Number of bytes available in buffer
 */
static inline size_t mbuf_get_space(const struct mbuf *mb)
{
	return (mb && (mb->size > mb->pos)) ? (mb->size - mb->pos) : 0;
}


/**
 * Set absolute position
 *
 * @param mb  Memory buffer
 * @param pos Position
 */
static inline void mbuf_set_pos(struct mbuf *mb, size_t pos)
{
	mb->pos = pos;
	MBUF_CHECK_POS(mb);
}


/**
 * Set absolute end
 *
 * @param mb  Memory buffer
 * @param end End position
 */
static inline void mbuf_set_end(struct mbuf *mb, size_t end)
{
	mb->end = end;
	MBUF_CHECK_END(mb);
}


/**
 * Advance position +/- N bytes
 *
 * @param mb  Memory buffer
 * @param n   Number of bytes to advance
 */
static inline void mbuf_advance(struct mbuf *mb, ssize_t n)
{
	mb->pos += n;
	MBUF_CHECK_POS(mb);
}


/**
 * Rewind position and end to the beginning of buffer
 *
 * @param mb  Memory buffer
 */
static inline void mbuf_rewind(struct mbuf *mb)
{
	mb->pos = mb->end = 0;
}


/**
 * Set position to the end of the buffer
 *
 * @param mb  Memory buffer
 */
static inline void mbuf_skip_to_end(struct mbuf *mb)
{
	mb->pos = mb->end;
}

/** @} */
#endif /* RE_MBUF_H */
