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
#ifndef __NOPOLL_CONN_OPTS_H__
#define __NOPOLL_CONN_OPTS_H__

#include <nopoll.h>

BEGIN_C_DECLS

noPollConnOpts * nopoll_conn_opts_new (void);

void nopoll_conn_opts_set_ssl_protocol (noPollConnOpts * opts, noPollSslProtocol ssl_protocol);

nopoll_bool  nopoll_conn_opts_set_ssl_certs    (noPollConnOpts * opts, 
						const char     * client_certificate,
						const char     * private_key,
						const char     * chain_certificate,
						const char     * ca_certificate);

void        nopoll_conn_opts_ssl_peer_verify (noPollConnOpts * opts, nopoll_bool verify);

void        nopoll_conn_opts_set_cookie (noPollConnOpts * opts, const char * cookie_content);

nopoll_bool nopoll_conn_opts_ref (noPollConnOpts * opts);

void        nopoll_conn_opts_unref (noPollConnOpts * opts);

void nopoll_conn_opts_set_reuse        (noPollConnOpts * opts, nopoll_bool reuse);

void nopoll_conn_opts_free (noPollConnOpts * opts);

/** internal API **/
void __nopoll_conn_opts_release_if_needed (noPollConnOpts * options);

END_C_DECLS

#endif
