/**
 * @file re_dbg.h  Interface to debugging module
 *
 * Copyright (C) 2010 Creytiv.com
 */

#ifndef RE_DBG_H
#define RE_DBG_H

#ifdef __cplusplus
extern "C" {
#endif

/** Debug levels */
enum {
	DBG_EMERG       = 0,       /**< System is unusable               */
	DBG_ALERT       = 1,       /**< Action must be taken immediately */
	DBG_CRIT        = 2,       /**< Critical conditions              */
	DBG_ERR         = 3,       /**< Error conditions                 */
	DBG_WARNING     = 4,       /**< Warning conditions               */
	DBG_NOTICE      = 5,       /**< Normal but significant condition */
	DBG_INFO        = 6,       /**< Informational                    */
	DBG_DEBUG       = 7        /**< Debug-level messages             */
};


/**
 * @def DEBUG_MODULE
 *
 * Module name defined by application
 */

/**
 * @def DEBUG_LEVEL
 *
 * Debug level defined by application
 */

#ifndef DEBUG_MODULE
# warning "DEBUG_MODULE is not defined"
#define DEBUG_MODULE "?"
#endif

#ifndef DEBUG_LEVEL
# warning "DEBUG_LEVEL is not defined"
#define DEBUG_LEVEL 7
#endif


/**
 * @def DEBUG_WARNING(...)
 *
 * Print warning message
 */

/**
 * @def DEBUG_NOTICE(...)
 *
 * Print notice message
 */

/**
 * @def DEBUG_INFO(...)
 *
 * Print info message
 */

/**
 * @def DEBUG_PRINTF(...)
 *
 * Print debug message
 */


/* Check for ISO C99 variable argument macros */
#if (defined(__STDC_VERSION__) && __STDC_VERSION__ >= 199901L)	\
	|| (__GNUC__ >= 3)

#if (DEBUG_LEVEL >= 4)
#define DEBUG_WARNING(...) \
	dbg_printf(DBG_WARNING, DEBUG_MODULE ": " __VA_ARGS__)
#else
#define DEBUG_WARNING(...)
#endif

#if (DEBUG_LEVEL >= 5)
#define DEBUG_NOTICE(...) \
	dbg_printf(DBG_NOTICE, DEBUG_MODULE ": " __VA_ARGS__)
#else
#define DEBUG_NOTICE(...)
#endif

#if (DEBUG_LEVEL >= 6)
#define DEBUG_INFO(...) \
	dbg_printf(DBG_INFO, DEBUG_MODULE ": " __VA_ARGS__)
#else
#define DEBUG_INFO(...)
#endif

#if (DEBUG_LEVEL >= 7)
#define DEBUG_PRINTF(...) \
	dbg_printf(DBG_DEBUG, DEBUG_MODULE ": " __VA_ARGS__)
#else
#define DEBUG_PRINTF(...)
#endif

/* GNU extensions for variable argument macros */
#elif defined(__GNUC__)

#if (DEBUG_LEVEL >= 4)
#define DEBUG_WARNING(a...) dbg_printf(DBG_WARNING, DEBUG_MODULE ": " a)
#else
#define DEBUG_WARNING(a...)
#endif

#if (DEBUG_LEVEL >= 5)
#define DEBUG_NOTICE(a...) dbg_printf(DBG_NOTICE, DEBUG_MODULE ": " a)
#else
#define DEBUG_NOTICE(a...)
#endif

#if (DEBUG_LEVEL >= 6)
#define DEBUG_INFO(a...) dbg_printf(DBG_INFO, DEBUG_MODULE ": " a)
#else
#define DEBUG_INFO(a...)
#endif

#if (DEBUG_LEVEL >= 7)
#define DEBUG_PRINTF(a...) dbg_printf(DBG_DEBUG, DEBUG_MODULE ": " a)
#else
#define DEBUG_PRINTF(a...)
#endif

/* No variable argument macros */
#else

#if (DEBUG_LEVEL >= 4)
#define DEBUG_WARNING dbg_warning
#else
#define DEBUG_WARNING dbg_noprintf
#endif

#if (DEBUG_LEVEL >= 5)
#define DEBUG_NOTICE dbg_notice
#else
#define DEBUG_NOTICE dbg_noprintf
#endif

#if (DEBUG_LEVEL >= 6)
#define DEBUG_INFO dbg_info
#else
#define DEBUG_INFO dbg_noprintf
#endif

#if (DEBUG_LEVEL >= 7)
#define DEBUG_PRINTF dbg_noprintf
#else
#define DEBUG_PRINTF dbg_noprintf
#endif

#endif


/** Debug flags */
enum dbg_flags {
	DBG_NONE = 0,                 /**< No debug flags         */
	DBG_TIME = 1<<0,              /**< Print timestamp flag   */
	DBG_ANSI = 1<<1,              /**< Print ANSI color codes */
	DBG_ALL  = DBG_TIME|DBG_ANSI  /**< All flags enabled      */
};


/**
 * Defines the debug print handler
 *
 * @param level Debug level
 * @param p     Debug string
 * @param len   String length
 * @param arg   Handler argument
 */
typedef void (dbg_print_h)(int level, const char *p, size_t len, void *arg);

void dbg_init(int level, enum dbg_flags flags);
void dbg_close(void);
int  dbg_logfile_set(const char *name);
void dbg_handler_set(dbg_print_h *ph, void *arg);
void dbg_printf(int level, const char *fmt, ...);
void dbg_noprintf(const char *fmt, ...);
void dbg_warning(const char *fmt, ...);
void dbg_notice(const char *fmt, ...);
void dbg_info(const char *fmt, ...);
const char *dbg_level_str(int level);

#ifdef __cplusplus
}
#endif
#endif /* RE_DBG_H */
