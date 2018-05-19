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
#ifndef __NOPOLL_LOG_H__
#define __NOPOLL_LOG_H__

#include <nopoll_decl.h>
#include <nopoll_handlers.h>

BEGIN_C_DECLS

/** 
 * \addtogroup nopoll_log_module
 * @{
 */

nopoll_bool     nopoll_log_is_enabled (noPollCtx * ctx);

nopoll_bool     nopoll_log_color_is_enabled (noPollCtx * ctx);

void            nopoll_log_enable (noPollCtx * ctx, nopoll_bool value);
 
void            nopoll_log_color_enable (noPollCtx * ctx, nopoll_bool value);

void            nopoll_log_set_handler (noPollCtx * ctx, noPollLogHandler handler, noPollPtr user_data);

/* include this at this place to load GNU extensions */
#if defined(__GNUC__)
#  ifndef _GNU_SOURCE
#  define _GNU_SOURCE
#  endif
#  define __function_name__   __PRETTY_FUNCTION__
#  define __line__            __LINE__
#  define __file__            __FILE__
#elif defined(_MSC_VER)
#  define __function_name__   __FUNCDNAME__
#  define __line__            __LINE__
#  define __file__            __FILE__
#else
/* unknown compiler */
#define __function_name__ ""
#define __line__            0
#define __file__            ""
#endif


#if defined(SHOW_DEBUG_LOG)
# define nopoll_log(ctx,level,message, ...) do{__nopoll_log(ctx, __function_name__, __file__, __line__, level, message, ##__VA_ARGS__);}while(0)
#else
# if defined(NOPOLL_OS_WIN32) && !( defined (__GNUC__) || _MSC_VER >= 1400)
/* default case where '...' is not supported but log is still
 * disabled */
#   define nopoll_log __nopoll_log 
# else
#   define nopoll_log(ctx, level, message, ...) /* nothing */
# endif
#endif

/** 
 * @internal The following definition allows to find printf like wrong
 * argument passing to nopoll_log function. To activate the depuration
 * just add the following to local nopoll_config.h file:
 *
 * #define SHOW_FORMAT_BUGS (1)
 */
#if defined(SHOW_FORMAT_BUGS)
# undef  nopoll_log
# define nopoll_log(ctx,level,message, ...) do{printf (message, ##__VA_ARGS__);}while(0)
#endif

void __nopoll_log (noPollCtx * ctx, const char * function_name, const char * file, int line, noPollDebugLevel level, const char * message, ...);

/* @} */

END_C_DECLS

#endif

