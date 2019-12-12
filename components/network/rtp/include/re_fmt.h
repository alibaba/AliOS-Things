/**
 * @file re_fmt.h  Interface to formatted text functions
 *
 * Copyright (C) 2010 Creytiv.com
 */


#include <stdarg.h>
#include <stdio.h>
#include <stdint.h>

struct mbuf;


/** Defines a pointer-length string type */
struct pl {
	const char *p;  /**< Pointer to string */
	size_t l;       /**< Length of string  */
};

/** Initialise a pointer-length object from a constant string */
#define PL(s) {(s), sizeof((s))-1}

/** Pointer-length Initializer */
#define PL_INIT {NULL, 0}

extern const struct pl pl_null;

void     pl_set_str(struct pl *pl, const char *str);
void     pl_set_mbuf(struct pl *pl, const struct mbuf *mb);
uint32_t pl_u32(const struct pl *pl);
uint32_t pl_x32(const struct pl *pl);
uint64_t pl_u64(const struct pl *pl);
uint64_t pl_x64(const struct pl *pl);
double   pl_float(const struct pl *pl);
bool     pl_isset(const struct pl *pl);
int      pl_strcpy(const struct pl *pl, char *str, size_t size);
int      pl_strdup(char **dst, const struct pl *src);
int      pl_dup(struct pl *dst, const struct pl *src);
int      pl_strcmp(const struct pl *pl, const char *str);
int      pl_strcasecmp(const struct pl *pl, const char *str);
int      pl_cmp(const struct pl *pl1, const struct pl *pl2);
int      pl_casecmp(const struct pl *pl1, const struct pl *pl2);
const char *pl_strchr(const struct pl *pl, char c);

/** Advance pl position/length by +/- N bytes */
static inline void pl_advance(struct pl *pl, ssize_t n)
{
	pl->p += n;
	pl->l -= n;
}


/* Formatted printing */

/**
 * Defines the re_vhprintf() print handler
 *
 * @param p    String to print
 * @param size Size of string to print
 * @param arg  Handler argument
 *
 * @return 0 for success, otherwise errorcode
 */
typedef int(re_vprintf_h)(const char *p, size_t size, void *arg);

/** Defines a print backend */
struct re_printf {
	re_vprintf_h *vph; /**< Print handler   */
	void *arg;         /**< Handler agument */
};

/**
 * Defines the %H print handler
 *
 * @param pf  Print backend
 * @param arg Handler argument
 *
 * @return 0 for success, otherwise errorcode
 */
typedef int(re_printf_h)(struct re_printf *pf, void *arg);

int re_vhprintf(const char *fmt, va_list ap, re_vprintf_h *vph, void *arg);
int re_vfprintf(FILE *stream, const char *fmt, va_list ap);
int re_vprintf(const char *fmt, va_list ap);
int re_vsnprintf(char *str, size_t size, const char *fmt, va_list ap);
int re_vsdprintf(char **strp, const char *fmt, va_list ap);

int re_hprintf(struct re_printf *pf, const char *fmt, ...);
int re_fprintf(FILE *stream, const char *fmt, ...);
int re_printf(const char *fmt, ...);
int re_snprintf(char *str, size_t size, const char *fmt, ...);
int re_sdprintf(char **strp, const char *fmt, ...);


/* Regular expressions */
int re_regex(const char *ptr, size_t len, const char *expr, ...);


/* Character functions */
uint8_t ch_hex(char ch);


/* String functions */
int  str_hex(uint8_t *hex, size_t len, const char *str);
void str_ncpy(char *dst, const char *src, size_t n);
int  str_dup(char **dst, const char *src);
int  str_cmp(const char *s1, const char *s2);
int  str_casecmp(const char *s1, const char *s2);
size_t str_len(const char *s);
const char *str_error(int errnum, char *buf, size_t sz);


/**
 * Check if string is set
 *
 * @param s Zero-terminated string
 *
 * @return true if set, false if not set
 */
static inline bool str_isset(const char *s)
{
	return s && s[0] != '\0';
}


/* time */
int  fmt_gmtime(struct re_printf *pf, void *ts);
int  fmt_human_time(struct re_printf *pf, const uint32_t *seconds);


void hexdump(FILE *f, const void *p, size_t len);


/* param */
typedef void (fmt_param_h)(const struct pl *name, const struct pl *val,
			   void *arg);

bool fmt_param_exists(const struct pl *pl, const char *pname);
bool fmt_param_get(const struct pl *pl, const char *pname, struct pl *val);
void fmt_param_apply(const struct pl *pl, fmt_param_h *ph, void *arg);


/* unicode */
int utf8_encode(struct re_printf *pf, const char *str);
int utf8_decode(struct re_printf *pf, const struct pl *pl);
size_t utf8_byteseq(char u[4], unsigned cp);
