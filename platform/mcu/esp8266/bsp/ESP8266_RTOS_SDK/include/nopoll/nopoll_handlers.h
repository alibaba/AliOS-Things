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
#ifndef __NOPOLL_HANDLERS_H__
#define __NOPOLL_HANDLERS_H__

/** 
 * \defgroup nopoll_handlers noPoll Handlers: Handler definitions used by the library to du async notifications
 */

/** 
 * \addtogroup nopoll_handlers
 * @{
 */

/** 
 * @brief General async handler definition used to notify generic
 * events associated to a connection.
 *
 * Currently this handler is used by:
 * - \ref nopoll_ctx_set_on_accept
 * - \ref nopoll_ctx_set_on_open
 *
 * @param ctx The context where the wait is happening.
 *
 * @param conn The connection where the data or something meaningful
 * was detected.
 *
 * @param user_data Optional user data pointer passed in into the handler.
 *
 * @return The function returns a boolean value which is interpreted
 * in an especific form according to the event.
 */
typedef nopoll_bool (*noPollActionHandler) (noPollCtx * ctx, noPollConn * conn, noPollPtr user_data);

/** 
 * @brief Handler used to define the create function for an IO mechanism.
 *
 * @param ctx The context where the io mechanism will be created.
 */
typedef noPollPtr (*noPollIoMechCreate)  (noPollCtx * ctx);

/** 
 * @brief Handler used to define the IO wait set destroy function for
 * an IO mechanism.
 *
 * @param ctx The context where the io mechanism will be destroyed.
 *
 * @param io_object The io object to be destroyed as created by \ref
 * noPollIoMechCreate handler.
 */
typedef void (*noPollIoMechDestroy)  (noPollCtx * ctx, noPollPtr io_object);

/** 
 * @brief Handler used to define the IO wait set clear function for an
 * IO mechanism.
 *
 * @param ctx The context where the io mechanism will be cleared.
 *
 * @param io_object The io object to be created as created by \ref
 * noPollIoMechCreate handler.
 */
typedef void (*noPollIoMechClear)  (noPollCtx * ctx, noPollPtr io_object);


/** 
 * @brief Handler used to define the IO wait function for an IO
 * mechanism.
 *
 * @param ctx The context where the io mechanism was created.
 *
 * @param io_object The io object to be created as created by \ref
 * noPollIoMechCreate handler where the wait will be implemented.
 */
typedef int (*noPollIoMechWait)  (noPollCtx * ctx, noPollPtr io_object);


/** 
 * @brief Handler used to define the IO add to set function for an IO
 * mechanism.
 *
 * @param ctx The context where the io mechanism was created.
 *
 * @param conn The noPollConn to be added to the working set.
 *
 * @param io_object The io object to be created as created by \ref
 * noPollIoMechCreate handler where the wait will be implemented.
 */
typedef nopoll_bool (*noPollIoMechAddTo)  (int               fds, 
					   noPollCtx       * ctx,
					   noPollConn      * conn,
					   noPollPtr         io_object);


/** 
 * @brief Handler used to define the IO is set function for an IO
 * mechanism.
 *
 * @param ctx The context where the io mechanism was created.
 *
 * @param conn The noPollConn to be added to the working set.
 *
 * @param io_object The io object to be created as created by \ref
 * noPollIoMechCreate handler where the wait will be implemented.
 */
typedef nopoll_bool (*noPollIoMechIsSet)  (noPollCtx       * ctx,
					   int               fds, 
					   noPollPtr         io_object);

/** 
 * @brief Handler used to define the foreach function that is used by
 * \ref nopoll_ctx_foreach_conn
 *
 * @param ctx The context where the foreach operation is taking place.
 *
 * @param conn The connection notified
 *
 * @param user_data Optional user defined pointer received at \ref
 * nopoll_ctx_foreach_conn.
 *
 * @return nopoll_true to stop the foreach process, otherwise
 * nopoll_false to keep checking the next connection until all
 * connections are notified.
 */
typedef nopoll_bool (*noPollForeachConn)  (noPollCtx  * ctx,
					   noPollConn * conn,
					   noPollPtr    user_data);

/** 
 * @brief Handler definition used to describe read functions used by \ref noPollConn.
 *
 * @param conn The connection where the readOperation will take place.
 *
 * @param buffer The buffer where data read from socket will be placed.
 *
 * @param buffer_size The buffer size that is receiving the function.
 */
typedef int (*noPollRead) (noPollConn * conn,
			   char       * buffer,
			   int          buffer_size);

/** 
 * @brief Handler definition used to notify websocket messages
 * received.
 *
 * This handler will be called when a websocket message is
 * received. Keep in mind the reference received on this handler will
 * be finished when the handler ends. If you need to have a reference
 * to the message after handler execution, acquire a reference via
 * \ref nopoll_msg_ref.
 *
 * @param ctx The context where the messagewas received.
 *
 * @param conn The connection where the message was received.
 *
 * @param msg The websocket message was received.
 *
 * @param user_data An optional user defined pointer.
 */
typedef void (*noPollOnMessageHandler) (noPollCtx  * ctx,
					noPollConn * conn,
					noPollMsg  * msg,
					noPollPtr    user_data);

/** 
 * @brief Handler definition used by \ref nopoll_conn_set_on_close.
 *
 * Handler definition for the function that is called when the
 * connection is closed but just before shutting down the socket
 * associated to the connection.
 *
 * @param ctx The context where the operation will take place.
 *
 * @param conn The connection where the operation will take place.
 *
 * @param user_data The reference that was configured to be passed in
 * into the handler.
 */
typedef void (*noPollOnCloseHandler)    (noPollCtx  * ctx,
					 noPollConn * conn, 
					 noPollPtr    user_data);

/** 
 * @brief Mutex creation handler used by the library.
 *
 * @return A reference to the mutex created (already initialized).
 */
typedef noPollPtr (*noPollMutexCreate) (void);

/** 
 * @brief Mutex destroy handler used by the library.
 *
 * @param The mutex to destroy.
 */
typedef void (*noPollMutexDestroy) (noPollPtr mutex);

/** 
 * @brief Mutex lock handler used by the library.
 *
 * @param The mutex where to implement the lock operation.
 */
typedef void (*noPollMutexLock) (noPollPtr mutex);

/** 
 * @brief Mutex unlock handler used by the library.
 *
 * @param The mutex where to implement the unlock operation.
 */
typedef void (*noPollMutexUnlock) (noPollPtr mutex);

/** 
 * @brief Handler used by nopoll_log_set_handler to receive all log
 * notifications produced by the library on this function.
 *
 * @param ctx The context where the operation is happening.
 *
 * @param level The log level 
 *
 * @param log_msg The actual log message reported.
 *
 * @param user_data A reference to user defined pointer passed in into  the function.
 */
typedef void (*noPollLogHandler) (noPollCtx * ctx, noPollDebugLevel level, const char * log_msg, noPollPtr user_data);

/** 
 * @brief An optional handler that allows user land code to define how
 * is SSL_CTX (SSL context) created and which are the settings it
 * should have before taking place SSL/TLS handshake.
 *
 * NOTE: that the function should return one context for every
 * connection created. Do not reuse unless you know what you are
 * doing.
 *
 * A very bare implementation for this context creation will be:
 *
 * \code 
 * SSL_CTX * my_ssl_ctx_creator (noPollCtx * ctx, noPollConn * conn, noPollConnOpts * opts, nopoll_bool is_client, noPollPtr user_data)
 * {
 *        // very basic context creation using default settings provided by OpenSSL
 *        return SSL_CTX_new (is_client ? TLSv1_client_method () : TLSv1_server_method ()); 
 * }
 * \endcode
 *
 * @param ctx The context where the operation is taking place.
 *
 * @param conn The connection that is being requested for a new context (SSL_CTX). Use is_client to know if this is a connecting client or a listener connection.
 *
 * @param opts Optional reference to the connection object created for this connection.
 *
 * @param is_client nopoll_true to signal that this is a request for a context for a client connection. Otherwise, it is for a listener connection.
 *
 * @param user_data User defined pointer that received on this function as defined at \ref nopoll_ctx_set_ssl_context_creator.
 *
 * @return The function must return a valid SSL_CTX object (see OpenSSL documentation to know more about this) or NULL if it fails.
 */
typedef noPollPtr (*noPollSslContextCreator) (noPollCtx       * ctx, 
					      noPollConn      * conn, 
					      noPollConnOpts  * opts, 
					      nopoll_bool       is_client, 
					      noPollPtr         user_data);

/** 
 * @brief Optional user defined handler that allows to execute SSL
 * post checks code before proceed.
 *
 * This handler is configured at \ref nopoll_ctx_set_post_ssl_check
 * and allows to implement custom actions while additional
 * verifications about certificate received, validation based on
 * certain attributes, etc.
 *
 * Note that when this handler is called, the SSL handshake has
 * finished without error. In case of SSL handshake failure, this
 * handler is not executed.
 *
 * @param ctx The context where the operation happens.
 *
 * @param conn The connection where the operation takes place and for which the post SSL check is being done.
 *
 * @param SSL_CTX The OpenSSL SSL_CTX object created for this connection.
 *
 * @param SSL The OpenSSL SSL object created for this connection.
 *
 * @param user_data User defined data that is received on this handler as configured at \ref nopoll_ctx_set_post_ssl_check
 */
typedef nopoll_bool (*noPollSslPostCheck) (noPollCtx      * ctx,
					   noPollConn     * conn,
					   noPollPtr        SSL_CTX,
					   noPollPtr        SSL,
					   noPollPtr        user_data);


#endif

/* @} */
