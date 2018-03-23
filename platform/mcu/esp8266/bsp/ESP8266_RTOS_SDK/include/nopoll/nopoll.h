/*
 *  LibNoPoll: A websocket library
 *  Copyright (C) 2013 Advanced Software Production Line, S.L.
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public License
 *  as published by the Free Software Foundation; either version 2.1
 *  of the License, or (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this program; if not, write to the Free
 *  Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
 *  02111-1307 USA
 *  
 *  You may find a copy of the license under this software is released
 *  at COPYING file. This is LGPL software: you are welcome to develop
 *  proprietary applications using this library without any royalty or
 *  fee but returning back any change, improvement or addition in the
 *  form of source code, project image, documentation patches, etc.
 *
 *  For commercial support on build Websocket enabled solutions
 *  contact us:
 *          
 *      Postal address:
 *         Advanced Software Production Line, S.L.
 *         Edificio Alius A, Oficina 102,
 *         C/ Antonio Suarez Nº 10,
 *         Alcalá de Henares 28802 Madrid
 *         Spain
 *
 *      Email address:
 *         info@aspl.es - http://www.aspl.es/nopoll
 */
#ifndef __NOPOLL_H__
#define __NOPOLL_H__

#include <nopoll_decl.h>
#include <nopoll_handlers.h>

BEGIN_C_DECLS

#if defined(NOPOLL_OS_WIN32)
#include <nopoll_win32.h>
#endif

#include <nopoll_ctx.h>
#include <nopoll_io.h>
#include <nopoll_conn_opts.h>
#include <nopoll_conn.h>
#include <nopoll_msg.h>
#include <nopoll_log.h>
#include <nopoll_listener.h>
#include <nopoll_io.h>
#include <nopoll_loop.h>

/** 
 * \addtogroup nopoll_module
 * @{
 */

nopoll_bool nopoll_cmp (const char * string1, const char * string2);

nopoll_bool nopoll_ncmp (const char * string1, const char * string2, int bytes);

char      * nopoll_strdup_printf   (const char * chunk, ...);

char      * nopoll_strdup_printfv  (const char * chunk, va_list args);

void        nopoll_trim  (char * chunk, int * trimmed);

void        nopoll_sleep (long microseconds);

void        nopoll_thread_handlers (noPollMutexCreate  mutex_create,
				    noPollMutexDestroy mutex_destroy,
				    noPollMutexLock    mutex_lock,
				    noPollMutexUnlock  mutex_unlock);

noPollPtr   nopoll_mutex_create (void);

void        nopoll_mutex_lock    (noPollPtr mutex);

void        nopoll_mutex_unlock  (noPollPtr mutex);

void        nopoll_mutex_destroy (noPollPtr mutex);

nopoll_bool nopoll_base64_encode (const char * content, 
				  int          length, 
				  char       * output, 
				  int        * output_size);

nopoll_bool nopoll_base64_decode (const char * content, 
				  int          length, 
				  char       * output, 
				  int        * output_size);

int         nopoll_timeval_substract  (struct timeval * a, 
				       struct timeval * b,
				       struct timeval * result);

char      * nopoll_strdup (const char * buffer);

nopoll_bool nopoll_nonce (char * buffer, int nonce_size);

void        nopoll_cleanup_library (void);

int    nopoll_get_bit (char byte, int position);

void   nopoll_set_bit     (char * buffer, int position);

void   nopoll_show_byte (noPollCtx * ctx, char byte, const char * label);

char * nopoll_int2bin (int a, char *buffer, int buf_size);

void   nopoll_int2bin_print (noPollCtx * ctx, int value);

int    nopoll_get_8bit  (const char * buffer);

int    nopoll_get_16bit (const char * buffer);

void   nopoll_set_16bit (int value, char * buffer);

void   nopoll_set_32bit (int value, char * buffer);

int    nopoll_get_32bit (const char * buffer);

/* @} */

END_C_DECLS

#endif
