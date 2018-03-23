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

#ifndef __NOPOLL_WIN32_H__
#define __NOPOLL_WIN32_H__

#include <nopoll_ctx.h>

BEGIN_C_DECLS

int           nopoll_win32_init (noPollCtx * ctx);

int           nopoll_win32_nonblocking_enable (NOPOLL_SOCKET socket);

int           nopoll_win32_blocking_enable    (NOPOLL_SOCKET socket);

/* gettimeofday support on windows */
int nopoll_win32_gettimeofday (struct timeval *tv, noPollPtr notUsed);

BOOL APIENTRY DllMain                         (HINSTANCE hInst,
					            DWORD reason,
					            LPVOID reserved);

#if !defined(EINPROGRESS)
#define EINPROGRESS (WSAEINPROGRESS)
#endif

END_C_DECLS

#endif
