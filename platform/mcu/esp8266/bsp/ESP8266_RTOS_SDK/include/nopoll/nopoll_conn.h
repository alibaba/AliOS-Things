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
#ifndef __NOPOLL_CONN_H__
#define __NOPOLL_CONN_H__

#include <nopoll.h>

BEGIN_C_DECLS

noPollConn * nopoll_conn_new (noPollCtx  * ctx,
			      const char * host_ip, 
			      const char * host_port, 
			      const char * host_name,
			      const char * get_url, 
			      const char * protocols,
			      const char * origin);

noPollConn * nopoll_conn_new_opts (noPollCtx       * ctx,
				   noPollConnOpts  * opts,
				   const char      * host_ip, 
				   const char      * host_port, 
				   const char      * host_name,
				   const char      * get_url, 
				   const char      * protocols,
				   const char      * origin);

noPollConn * nopoll_conn_tls_new (noPollCtx  * ctx,
				  noPollConnOpts * options,
				  const char * host_ip, 
				  const char * host_port, 
				  const char * host_name,
				  const char * get_url, 
				  const char * protocols,
				  const char * origin);

noPollConn   * nopoll_conn_accept (noPollCtx * ctx, noPollConn * listener);

noPollConn   * nopoll_conn_accept_socket (noPollCtx * ctx, noPollConn * listener, NOPOLL_SOCKET session);

nopoll_bool    nopoll_conn_accept_complete (noPollCtx      * ctx, 
					    noPollConn     * listener, 
					    noPollConn     * conn, 
					    NOPOLL_SOCKET    session,
					    nopoll_bool      tls_on);

nopoll_bool    nopoll_conn_ref (noPollConn * conn);

int            nopoll_conn_ref_count (noPollConn * conn);

void           nopoll_conn_unref (noPollConn * conn);

nopoll_bool    nopoll_conn_is_ok (noPollConn * conn);

nopoll_bool    nopoll_conn_is_ready (noPollConn * conn);

nopoll_bool    nopoll_conn_is_tls_on (noPollConn * conn);

NOPOLL_SOCKET nopoll_conn_socket (noPollConn * conn);

void           nopoll_conn_set_socket (noPollConn * conn, NOPOLL_SOCKET _socket);

int           nopoll_conn_get_id (noPollConn * conn);

noPollCtx   * nopoll_conn_ctx    (noPollConn * conn);

noPollRole    nopoll_conn_role   (noPollConn * conn);

const char  * nopoll_conn_host   (noPollConn * conn);

const char  * nopoll_conn_port   (noPollConn * conn);

const char  * nopoll_conn_get_origin (noPollConn * conn);

const char  * nopoll_conn_get_host_header (noPollConn * conn);

const char  * nopoll_conn_get_cookie (noPollConn * conn);

const char  * nopoll_conn_get_accepted_protocol (noPollConn * conn);

const char  * nopoll_conn_get_requested_protocol (noPollConn * conn);

void          nopoll_conn_set_accepted_protocol (noPollConn * conn, const char * protocol);

int           nopoll_conn_get_close_status (noPollConn * conn);

const char *  nopoll_conn_get_close_reason (noPollConn * conn);

void          nopoll_conn_shutdown (noPollConn * conn);

void          nopoll_conn_close  (noPollConn  * conn);

void          nopoll_conn_close_ext  (noPollConn  * conn, int status, const char * reason, int reason_size);

void          nopoll_conn_set_hook (noPollConn * conn, noPollPtr ptr);

noPollPtr     nopoll_conn_get_hook (noPollConn * conn);

nopoll_bool   nopoll_conn_set_sock_block         (NOPOLL_SOCKET socket,
						  nopoll_bool   enable);

noPollMsg   * nopoll_conn_get_msg (noPollConn * conn);

int           nopoll_conn_send_text (noPollConn * conn, const char * content, long length);

int           nopoll_conn_send_text_fragment (noPollConn * conn, const char * content, long length);

int           nopoll_conn_send_binary (noPollConn * conn, const char * content, long length);

int           nopoll_conn_send_binary_fragment (noPollConn * conn, const char * content, long length);

int           nopoll_conn_complete_pending_write (noPollConn * conn);

int           nopoll_conn_pending_write_bytes    (noPollConn * conn);

int           nopoll_conn_flush_writes           (noPollConn * conn, long timeout, int previous_result);

int           nopoll_conn_read (noPollConn * conn, char * buffer, int bytes, nopoll_bool block, long int timeout);

noPollConn  * nopoll_conn_get_listener (noPollConn * conn);

nopoll_bool      nopoll_conn_send_ping (noPollConn * conn);

nopoll_bool      nopoll_conn_send_pong (noPollConn * conn);

void          nopoll_conn_set_on_msg (noPollConn              * conn,
				      noPollOnMessageHandler    on_msg,
				      noPollPtr                 user_data);

void          nopoll_conn_set_on_ready (noPollConn            * conn,
					noPollActionHandler     on_ready,
					noPollPtr               user_data);

void          nopoll_conn_set_on_close (noPollConn            * conn,
					noPollOnCloseHandler    on_close,
					noPollPtr               user_data);

int nopoll_conn_send_frame (noPollConn * conn, nopoll_bool fin, nopoll_bool masked,
			    noPollOpCode op_code, long length, noPollPtr content,
			    long sleep_in_header);

int           __nopoll_conn_send_common (noPollConn * conn, 
					 const char * content, 
					 long         length, 
					 nopoll_bool  has_fin, 
					 long         sleep_in_header,
					 noPollOpCode frame_type);

nopoll_bool      nopoll_conn_wait_until_connection_ready (noPollConn * conn,
							  int          timeout);

/** internal api **/
void nopoll_conn_complete_handshake (noPollConn * conn);

int nopoll_conn_default_receive (noPollConn * conn, char * buffer, int buffer_size);

int nopoll_conn_default_send (noPollConn * conn, char * buffer, int buffer_size);

void nopoll_conn_mask_content (noPollCtx * ctx, char * payload, int payload_size, char * mask, int desp);

END_C_DECLS

#endif
