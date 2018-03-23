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
#ifndef __NOPOLL_LISTENER_H__
#define __NOPOLL_LISTENER_H__

#include <nopoll.h>

BEGIN_C_DECLS

NOPOLL_SOCKET     nopoll_listener_sock_listen      (noPollCtx   * ctx,
						    const char  * host,
						    const char  * port);

noPollConn      * nopoll_listener_new (noPollCtx  * ctx,
				       const char * host,
				       const char * port);

noPollConn      * nopoll_listener_new_opts (noPollCtx      * ctx,
					    noPollConnOpts * opts,
					    const char     * host,
					    const char     * port);

noPollConn      * nopoll_listener_tls_new (noPollCtx  * ctx,
					   const char * host,
					   const char * port);

noPollConn      * nopoll_listener_tls_new_opts (noPollCtx      * ctx,
						noPollConnOpts * opts,
						const char     * host,
						const char     * port);

nopoll_bool       nopoll_listener_set_certificate (noPollConn * listener,
						   const char * certificate,
						   const char * private_key,
						   const char * chain_file);

noPollConn      * nopoll_listener_from_socket (noPollCtx      * ctx,
					       NOPOLL_SOCKET    session);

NOPOLL_SOCKET     nopoll_listener_accept (NOPOLL_SOCKET server_socket);

END_C_DECLS

#endif
