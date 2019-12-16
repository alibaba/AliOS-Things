/**
 * @file dbg.c  Debug printing
 *
 * Copyright (C) 2010 Creytiv.com
 */
#include <stdio.h>
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#ifdef HAVE_PTHREAD
#include <stdlib.h>
#include <posix/pthread.h>
#endif
#include <time.h>
#include <re_types.h>
#include <re_fmt.h>
#include <re_list.h>
#include <re_tmr.h>

#define DEBUG_MODULE "dbg"
#define DEBUG_LEVEL 0
#include <re_dbg.h>

/** Debug configuration */
static struct {
	uint64_t tick;         /**< Init ticks             */
	int level;             /**< Current debug level    */
	enum dbg_flags flags;  /**< Debug flags            */
	dbg_print_h *ph;       /**< Optional print handler */
	void *arg;             /**< Handler argument       */
	FILE *f;               /**< Logfile                */
#ifdef HAVE_PTHREAD
	pthread_mutex_t mutex; /**< Thread locking         */
#endif
} dbg = {
	0,
	DBG_INFO,
	DBG_ANSI,
	NULL,
	NULL,
	NULL,
#ifdef HAVE_PTHREAD
	PTHREAD_MUTEX_INITIALIZER,
#endif
};


#ifdef HAVE_PTHREAD
static inline void dbg_lock(void)
{
	pthread_mutex_lock(&dbg.mutex);
}


static inline void dbg_unlock(void)
{
	pthread_mutex_unlock(&dbg.mutex);
}
#else
#define dbg_lock()    /**< Stub */
#define dbg_unlock()  /**< Stub */
#endif


/**
 * Initialise debug printing
 *
 * @param level Debug level
 * @param flags Debug flags
 */
void dbg_init(int level, enum dbg_flags flags)
{
	dbg.tick  = tmr_jiffies();
	dbg.level = level;
	dbg.flags = flags;
}


/**
 * Close debugging
 */
void dbg_close(void)
{
	if (dbg.f) {
		(void)fclose(dbg.f);
		dbg.f = NULL;
	}
}


/**
 * Set debug logfile
 *
 * @param name Name of the logfile, NULL to close
 *
 * @return 0 if success, otherwise errorcode
 */
int dbg_logfile_set(const char *name)
{
	time_t t;

	dbg_close();

	if (!name)
		return 0;

	dbg.f = fopen(name, "a+");
	if (!dbg.f)
		return errno;

	(void)time(&t);
	(void)re_fprintf(dbg.f, "\n===== Log Started: %s", ctime(&t));
	(void)fflush(dbg.f);

	return 0;
}


/**
 * Set optional debug print handler
 *
 * @param ph  Print handler
 * @param arg Handler argument
 */
void dbg_handler_set(dbg_print_h *ph, void *arg)
{
	dbg.ph  = ph;
	dbg.arg = arg;
}


/* NOTE: This function should not allocate memory */
static void dbg_vprintf(int level, const char *fmt, va_list ap)
{
	if (level > dbg.level)
		return;

	/* Print handler? */
	if (dbg.ph)
		return;

	dbg_lock();

	if (dbg.flags & DBG_ANSI) {

		switch (level) {

		case DBG_WARNING:
			(void)re_fprintf(stderr, "\x1b[31m"); /* Red */
			break;

		case DBG_NOTICE:
			(void)re_fprintf(stderr, "\x1b[33m"); /* Yellow */
			break;

		case DBG_INFO:
			(void)re_fprintf(stderr, "\x1b[32m"); /* Green */
			break;

		default:
			break;
		}
	}

	if (dbg.flags & DBG_TIME) {
		const uint64_t ticks = tmr_jiffies();

		if (0 == dbg.tick)
			dbg.tick = tmr_jiffies();

		(void)re_fprintf(stderr, "[%09llu] ", ticks - dbg.tick);
	}

	(void)re_vfprintf(stderr, fmt, ap);

	if (dbg.flags & DBG_ANSI && level < DBG_DEBUG)
		(void)re_fprintf(stderr, "\x1b[;m");

	dbg_unlock();
}


/* Formatted output to print handler and/or logfile */
static void dbg_fmt_vprintf(int level, const char *fmt, va_list ap)
{
	char buf[256];
	int len;

	if (level > dbg.level)
		return;

	if (!dbg.ph && !dbg.f)
		return;

	dbg_lock();

	len = re_vsnprintf(buf, sizeof(buf), fmt, ap);
	if (len <= 0)
		goto out;

	/* Print handler? */
	if (dbg.ph) {
		dbg.ph(level, buf, len, dbg.arg);
	}

	/* Output to file */
	if (dbg.f) {
		if (fwrite(buf, 1, len, dbg.f) > 0)
			(void)fflush(dbg.f);
	}

 out:
	dbg_unlock();
}


/**
 * Print a formatted debug message
 *
 * @param level Debug level
 * @param fmt   Formatted string
 */
void dbg_printf(int level, const char *fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
	dbg_vprintf(level, fmt, ap);
	va_end(ap);

	va_start(ap, fmt);
	dbg_fmt_vprintf(level, fmt, ap);
	va_end(ap);
}


/**
 * Print a formatted debug message to /dev/null
 *
 * @param fmt   Formatted string
 */
void dbg_noprintf(const char *fmt, ...)
{
	(void)fmt;
}


/**
 * Print a formatted warning message
 *
 * @param fmt   Formatted string
 */
void dbg_warning(const char *fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
	dbg_vprintf(DBG_WARNING, fmt, ap);
	va_end(ap);

	va_start(ap, fmt);
	dbg_fmt_vprintf(DBG_WARNING, fmt, ap);
	va_end(ap);
}


/**
 * Print a formatted notice message
 *
 * @param fmt   Formatted string
 */
void dbg_notice(const char *fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
	dbg_vprintf(DBG_NOTICE, fmt, ap);
	va_end(ap);

	va_start(ap, fmt);
	dbg_fmt_vprintf(DBG_NOTICE, fmt, ap);
	va_end(ap);
}


/**
 * Print a formatted info message
 *
 * @param fmt   Formatted string
 */
void dbg_info(const char *fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
	dbg_vprintf(DBG_INFO, fmt, ap);
	va_end(ap);

	va_start(ap, fmt);
	dbg_fmt_vprintf(DBG_INFO, fmt, ap);
	va_end(ap);
}


/**
 * Get the name of the debug level
 *
 * @param level Debug level
 *
 * @return String with debug level name
 */
const char *dbg_level_str(int level)
{
	switch (level) {

	case DBG_EMERG:   return "EMERGENCY";
	case DBG_ALERT:   return "ALERT";
	case DBG_CRIT:    return "CRITICAL";
	case DBG_ERR:     return "ERROR";
	case DBG_WARNING: return "WARNING";
	case DBG_NOTICE:  return "NOTICE";
	case DBG_INFO:    return "INFO";
	case DBG_DEBUG:   return "DEBUG";
	default:          return "???";
	}
}
