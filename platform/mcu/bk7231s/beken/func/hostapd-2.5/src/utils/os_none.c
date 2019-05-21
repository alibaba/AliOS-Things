/*
 * wpa_supplicant/hostapd / Empty OS specific functions
 * Copyright (c) 2005-2006, Jouni Malinen <j@w1.fi>
 *
 * This software may be distributed under the terms of the BSD license.
 * See README for more details.
 *
 * This file can be used as a starting point when adding a new OS target. The
 * functions here do not really work as-is since they are just empty or only
 * return an error value. os_internal.c can be used as another starting point
 * or reference since it has example implementation of many of these functions.
 */

#include "include.h"
#include "fake_clock_pub.h"

#include "includes.h"

#include "os.h"

void os_sleep(os_time_t sec, os_time_t usec)
{
}


int os_get_time(struct os_time *t)
{
	t->sec = fclk_get_second();
	t->usec = (fclk_get_tick() * FCLK_DURATION_MS * 1000) % 1000000;
	
	return 0;
}


int os_get_reltime(struct os_reltime *t)
{
	t->sec = fclk_get_second();
	t->usec = (fclk_get_tick() * FCLK_DURATION_MS * 1000) % 1000000;
	
	return 0;
}


int os_mktime(int year, int month, int day, int hour, int min, int sec,
	      os_time_t *t)
{
	return -1;
}

int os_gmtime(os_time_t t, struct os_tm *tm)
{
	return -1;
}


int os_daemonize(const char *pid_file)
{
	return -1;
}


void os_daemonize_terminate(const char *pid_file)
{
}


int os_get_random(unsigned char *buf, size_t len)
{
	srand(fclk_get_second());
	while(len--){
		*buf++ = rand()%255;
	}
	return 0;
}


unsigned long os_random(void)
{
	return rand();
}


char * os_rel2abs_path(const char *rel_path)
{
	return NULL; /* strdup(rel_path) can be used here */
}


int os_program_init(void)
{
	return 0;
}


void os_program_deinit(void)
{
}


int os_setenv(const char *name, const char *value, int overwrite)
{
	return -1;
}


int os_unsetenv(const char *name)
{
	return -1;
}


char * os_readfile(const char *name, size_t *len)
{
	return NULL;
}


int os_fdatasync(FILE *stream)
{
	return 0;
}


int os_exec(const char *program, const char *arg, int wait_completion)
{
	return -1;
}

// eof 

