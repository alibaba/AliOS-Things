/*
 *  Copyright (c) 2021, The OpenThread Authors.
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions are met:
 *  1. Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *  2. Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *  3. Neither the name of the copyright holder nor the
 *     names of its contributors may be used to endorse or promote products
 *     derived from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 *  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 *  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 *  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 *  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 *  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 *  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 *  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 *  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *  POSSIBILITY OF SUCH DAMAGE.
 */

/**
 * @file
 * @brief
 *  This file defines the OpenThread TCP API.
 *
 */

#ifndef OPENTHREAD_TCP_H_
#define OPENTHREAD_TCP_H_

#include <openthread/ip6.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup api-tcp
 *
 * @brief
 *   This module includes functions that control TCP communication.
 *
 * @{
 *
 */

/**
 * A linked buffer structure for use with TCP.
 *
 * A single otLinkedBuffer structure references an array of bytes in memory,
 * via mData and mLength. The mNext field is used to form a chain of
 * otLinkedBuffer structures.
 *
 */
typedef struct otLinkedBuffer
{
    struct otLinkedBuffer *mNext;   ///< Pointer to the next linked buffer in the chain, or NULL if it is the end.
    const uint8_t *        mData;   ///< Pointer to data referenced by this linked buffer.
    uint16_t               mLength; ///< Length of this linked buffer (number of bytes).
} otLinkedBuffer;

struct otTcpEndpoint;
typedef struct otTcpEndpoint otTcpEndpoint;

/**
 * This callback informs the application that the TCP 3-way handshake is
 * complete and that the connection is now established.
 *
 * @param[in]  aEndpoint  The TCP endpoint whose connection is now established.
 *
 */
typedef void (*otTcpEstablished)(otTcpEndpoint *aEndpoint);

/**
 * This callback informs the application that data in the provided
 * @p aData have been acknowledged by the connection peer and that @p aData and
 * the data it contains can be reclaimed by the application.
 *
 * The @p aData are guaranteed to be identical to those passed in to TCP via
 * otTcpSendByReference(), including any extensions effected via
 * otTcpSendByExtension().
 *
 * @param[in]  aEndpoint  The TCP endpoint for the connection.
 * @param[in]  aData      A pointer to the otLinkedBuffer that can be reclaimed.
 *
 */
typedef void (*otTcpSendDone)(otTcpEndpoint *aEndpoint, otLinkedBuffer *aData);

/**
 * This callback informs the application that the first @p aNumBytes in the
 * send buffer have been acknowledged by the connection peer and that their
 * underlying memory can be reclaimed by the application.
 *
 * This callback is not necessary for correct TCP operation. Most applications
 * can just rely on the otTcpSendDone() callback. If an application wants
 * fine-grained feedback as memory in the send buffer becomes available again
 * (instead of waiting for an entire linked buffer's worth of data becomes
 * available) or some indication as to whether the connection is making forward
 * progress, it can register this callback.
 *
 * @param[in]  aEndpoint  The TCP endpoint for the connection.
 * @param[in]  aNumBytes  The number of bytes newly acknowledged by the connection peer.
 *
 */
typedef void (*otTcpBytesAcked)(otTcpEndpoint *aEndpoint, size_t aNumBytes);

/**
 * This callback informs the application that if data is added to the send
 * buffer, some of it will be transmitted immediately without delay, as opposed
 * to being queued for transmission once the peer ACKs some data.
 *
 * After a call to otTcpSendByReference() or otTcpSendByExtension(), the
 * otTcpSendReady() callback is guaranteed to be called, either immediately (if
 * the connection is already ready) or sometime in the future (once the
 * connection becomes ready for more data).
 *
 * This callback is not necessary for correct TCP operation. If more data is
 * added to the send buffer than can be transmitted without delay, it will
 * simply be queued for transmission at a later time. This callback should be
 * used only in cases where some assurance is desired that data added to the
 * send buffer will be sent soon (e.g., TCP won't wait for the recipient to
 * ACK some other data first before sending this data out). For example, you
 * may use this callback if you'd rather have your data be dropped than develop
 * a backlog of data in your send buffer. But for most applications, where this
 * isn't a concern, it's expected that one would not use this callback at all.
 *
 * @param[in]  aEndpoint  The TCP endpoint for the connection.
 *
 */
typedef void (*otTcpSendReady)(otTcpEndpoint *aEndpoint);

/**
 * This callback indicates the number of bytes available for consumption from
 * the receive buffer.
 *
 * It is called whenever bytes are added to the receive buffer and when the
 * end of stream is reached. If the end of the stream has been reached (i.e.,
 * if no more data will become available to read because the connection peer
 * has closed their end of the connection for writing), then @p aEndOfStream is
 * true. Finally, @p aBytesRemaining indicates how much capacity is left in the
 * receive buffer to hold additional data that arrives.
 *
 * @param[in]  aEndpoint        The TCP endpoint for the connection.
 * @param[in]  aBytesAvailable  The number of bytes in the connection's receive buffer.
 * @param[in]  aEndOfStream     Indicates if additional data, beyond what is already in the connection's receive buffer,
 *                              can be received.
 * @param[in]  aBytesRemaining  The number of additional bytes that can be received before the receive buffer becomes
 *                              full.
 *
 */
typedef void (*otTcpReceiveAvailable)(otTcpEndpoint *aEndpoint,
                                      size_t         aBytesAvailable,
                                      bool           aEndOfStream,
                                      size_t         aBytesRemaining);

typedef enum otTcpDisconnectedReason
{
    OT_TCP_DISCONNECTED_REASON_NORMAL,
    OT_TCP_DISCONNECTED_REASON_REFUSED,
    OT_TCP_DISCONNECTED_REASON_RESET,
    OT_TCP_DISCONNECTED_REASON_TIME_WAIT,
    OT_TCP_DISCONNECTED_REASON_TIMED_OUT,
} otTcpDisconnectedReason;

/**
 * This callback indicates that the connection was broken and should no longer
 * be used, or that a connection has entered the TIME-WAIT state.
 *
 * It can occur if a connection establishment attempt (initiated by calling
 * otTcpConnect()) fails, or any point thereafter (e.g., if the connection
 * times out or an RST segment is received from the connection peer). Once this
 * callback fires, all resources that the application provided for this
 * connection (i.e., any `otLinkedBuffers` and memory they reference, but not
 * the TCP endpoint itself or space for the receive buffers) can be reclaimed.
 * In the case of a connection entering the TIME-WAIT state, this callback is
 * called twice, once upon entry into the TIME-WAIT state (with
 * OT_TCP_DISCONNECTED_REASON_TIME_WAIT, and again when the TIME-WAIT state
 * expires (with OT_TCP_DISCONNECTED_REASON_NORMAL).
 *
 * @param[in]  aEndpoint  The TCP endpoint whose connection has been lost.
 * @param[in]  aReason    The reason why the connection was lost.
 *
 */
typedef void (*otTcpDisconnected)(otTcpEndpoint *aEndpoint, otTcpDisconnectedReason aReason);

/**
 * This structure represents a TCP endpoint.
 *
 * An TCP endpoint acts an endpoint of TCP connection. It can be used to
 * initiate TCP connections, and, once a TCP connection is established, send
 * data to and receive data from the connection peer.
 *
 * The application should not inspect the fields of this structure directly; it
 * should only interact with it via the TCP API functions whose signatures are
 * provided in this file.
 *
 */
struct otTcpEndpoint
{
    struct otTcpEndpoint *mNext;     ///< A pointer to the next TCP endpoint (internal use only)
    otInstance *          mInstance; ///< A pointer to the OpenThread instance associated with this TCP endpoint
    void *                mContext;  ///< A pointer to application-specific context

    otTcpEstablished      mEstablishedCallback;      ///< "Established" callback function
    otTcpSendDone         mSendDoneCallback;         ///< "Send done" callback function
    otTcpSendReady        mSendReadyCallback;        ///< "Send ready" callback function
    otTcpReceiveAvailable mReceiveAvailableCallback; ///< "Receive available" callback function
    otTcpDisconnected     mDisconnectedCallback;     ///< "Disconnected" callback function

    uint32_t mTimers[4];

    /* Other implementation-defined fields go here. */
};

/**
 * This structure contains arguments to the otTcpEndpointInitialize() function.
 *
 */
typedef struct otTcpEndpointInitializeArgs
{
    void *mContext; ///< Pointer to application-specific context

    otTcpEstablished      mEstablishedCallback;      ///< "Established" callback function
    otTcpSendDone         mSendDoneCallback;         ///< "Send done" callback function
    otTcpBytesAcked       mBytesAckedCallback;       ///< "Bytes acked" callback
    otTcpSendReady        mSendReadyCallback;        ///< "Send ready" callback function
    otTcpReceiveAvailable mReceiveAvailableCallback; ///< "Receive available" callback function
    otTcpDisconnected     mDisconnectedCallback;     ///< "Disconnected" callback function

    void * mReceiveBuffer;     ///< Pointer to memory provided to the system for the TCP receive buffer
    size_t mReceiveBufferSize; ///< Size of memory provided to the system for the TCP receive buffer
} otTcpEndpointInitializeArgs;

/**
 * @def OT_TCP_RECEIVE_BUFFER_SIZE_FEW_HOPS
 *
 * Recommended buffer size for TCP connections that traverse about 3 wireless
 * hops or fewer.
 *
 * On platforms where memory is particularly constrained and in situations
 * where high bandwidth is not necessary, it may be desirable to manually
 * select a smaller buffer size.
 *
 */
#define OT_TCP_RECEIVE_BUFFER_SIZE_FEW_HOPS 2599

/**
 * @def OT_TCP_RECEIVE_BUFFER_SIZE_MANY_HOPS
 *
 * Recommended buffer size for TCP connections that traverse many wireless
 * hops.
 *
 * If the TCP connection traverses a very large number of hops (more than 6 or
 * so), then it may be advisable to select a large buffer size manually.
 *
 */
#define OT_TCP_RECEIVE_BUFFER_SIZE_MANY_HOPS 4158

/**
 * Initializes a TCP endpoint.
 *
 * Calling this function causes OpenThread to keep track of the TCP endpoint
 * and store and retrieve TCP data inside the @p aEndpoint. The application
 * should refrain from directly accessing or modifying the fields in
 * @p aEndpoint. If the application needs to reclaim the memory backing
 * @p aEndpoint, it should call otTcpEndpointDeinitialize().
 *
 * @param[in]  aInstance  A pointer to an OpenThread instance.
 * @param[in]  aEndpoint  A pointer to a TCP endpoint structure.
 * @param[in]  aArgs      A pointer to a structure of arguments.
 *
 * @retval OT_ERROR_NONE    Successfully opened the TCP endpoint.
 * @retval OT_ERROR_FAILED  Failed to open the TCP endpoint.
 *
 */
otError otTcpEndpointInitialize(otInstance *aInstance, otTcpEndpoint *aEndpoint, otTcpEndpointInitializeArgs *aArgs);

/**
 * Obtains the otInstance that was associated with @p aEndpoint upon
 * initialization.
 *
 * @param[in]  aEndpoint  The TCP endpoint whose instance to obtain.
 *
 * @returns  The otInstance pointer associated with @p aEndpoint.
 *
 */
otInstance *otTcpEndpointGetInstance(otTcpEndpoint *aEndpoint);

/**
 * Obtains the context pointer that was associated with @p aEndpoint upon
 * initialization.
 *
 * @param[in]  aEndpoint  The TCP endpoint whose context to obtain.
 *
 * @returns  The context pointer associated with @p aEndpoint.
 *
 */
void *otTcpEndpointGetContext(otTcpEndpoint *aEndpoint);

/**
 * Obtains a pointer to a TCP endpoint's local host and port.
 *
 * The contents of the host and port may be stale if this socket is not in a
 * connected state and has not been bound after it was last disconnected.
 *
 * @param[in]  aEndpoint  The TCP endpoint whose local host and port to obtain.
 *
 * @returns  The local host and port of @p aEndpoint.
 *
 */
const otSockAddr *otTcpGetLocalAddress(const otTcpEndpoint *aEndpoint);

/**
 * Obtains a pointer to a TCP endpoint's peer's host and port.
 *
 * The contents of the host and port may be stale if this socket is not in a
 * connected state.
 *
 * @param[in]  aEndpoint  The TCP endpoint whose peer's host and port to obtain.
 *
 * @returns  The host and port of the connection peer of @p aEndpoint.
 *
 */
const otSockAddr *otTcpGetPeerAddress(const otTcpEndpoint *aEndpoint);

/**
 * Binds the TCP endpoint to an IP address and port.
 *
 * @param[in]  aEndpoint   A pointer to the TCP endpoint structure to bind.
 * @param[in]  aSockName   The address and port to which to bind this TCP endpoint.
 *
 * @retval OT_ERROR_NONE    Successfully bound the TCP endpoint.
 * @retval OT_ERROR_FAILED  Failed to bind the TCP endpoint.
 *
 */
otError otTcpBind(otTcpEndpoint *aEndpoint, const otSockAddr *aSockName);

/**
 * This enumeration defines flags passed to otTcpConnect().
 *
 */
enum
{
    OT_TCP_CONNECT_NO_FAST_OPEN = 1 << 0,
};

/**
 * Records the remote host and port for this connection.
 *
 * By default TCP Fast Open is used. This means that this function merely
 * records the remote host and port, and that the TCP connection establishment
 * handshake only happens on the first call to otTcpSendByReference(). TCP Fast
 * Open can be explicitly disabled using @p aFlags, in which case the TCP
 * connection establishment handshake is initiated immediately.
 *
 * @param[in]  aEndpoint  A pointer to the TCP endpoint structure to connect.
 * @param[in]  aSockName  The IP address and port of the host to which to connect.
 * @param[in]  aFlags     Flags specifying options for this operation (see enumeration above).
 *
 * @retval OT_ERROR_NONE    Successfully completed the operation.
 * @retval OT_ERROR_FAILED  Failed to complete the operation.
 *
 */
otError otTcpConnect(otTcpEndpoint *aEndpoint, const otSockAddr *aSockName, uint32_t aFlags);

/**
 * This enumeration defines flags passed to @p otTcpSendByReference.
 *
 */
enum
{
    OT_TCP_SEND_MORE_TO_COME = 1 << 0,
};

/**
 * Adds data referenced by the linked buffer pointed to by @p aBuffer to the
 * send buffer.
 *
 * Upon a successful call to this function, the linked buffer and data it
 * references are owned by the TCP stack; they should not be modified by the
 * application until a "send done" callback returns ownership of those objects
 * to the application. It is acceptable to call this function to add another
 * linked buffer to the send queue, even if the "send done" callback for a
 * previous invocation of this function has not yet fired.
 *
 * Note that @p aBuffer should not be chained; its mNext field should be
 * NULL. If additional data will be added right after this call, then the
 * OT_TCP_SEND_MORE_TO_COME flag should be used as a hint to the TCP
 * implementation.
 *
 * @param[in]  aEndpoint  A pointer to the TCP endpoint structure representing the TCP endpoint on which to send data.
 * @param[in]  aBuffer    A pointer to the linked buffer chain referencing data to add to the send buffer.
 * @param[in]  aFlags     Flags specifying options for this operation (see enumeration above).
 *
 * @retval OT_ERROR_NONE    Successfully added data to the send buffer.
 * @retval OT_ERROR_FAILED  Failed to add data to the send buffer.
 *
 */
otError otTcpSendByReference(otTcpEndpoint *aEndpoint, otLinkedBuffer *aBuffer, uint32_t aFlags);

/**
 * Adds data to the send buffer by extending the length of the final
 * otLinkedBuffer in the send buffer by the specified amount.
 *
 * If the send buffer is empty, then the operation fails.
 *
 * @param[in]  aEndpoint  A pointer to the TCP endpoint structure representing the TCP endpoint on which to send data.
 * @param[in]  aNumBytes  The number of bytes by which to extend the length of the final linked buffer.
 * @param[in]  aFlags     Flags specifying options for this operation (see enumeration above).
 *
 * @retval OT_ERROR_NONE    Successfully added data to the send buffer.
 * @retval OT_ERROR_FAILED  Failed to add data to the send buffer.
 *
 */
otError otTcpSendByExtension(otTcpEndpoint *aEndpoint, size_t aNumBytes, uint32_t aFlags);

/**
 * Provides the application with a linked buffer chain referencing data
 * currently in the TCP receive buffer.
 *
 * The linked buffer chain is valid until the "receive ready" callback is next
 * invoked, or until the next call to otTcpReceiveContiguify() or
 * otTcpCommitReceive().
 *
 * @param[in]   aEndpoint  A pointer to the TCP endpoint structure representing the TCP endpoint on which to receive
 *                         data.
 * @param[out]  aBuffer    A pointer to the linked buffer chain referencing data currently in the receive buffer.
 *
 * @retval OT_ERROR_NONE    Successfully completed the operation.
 * @retval OT_ERROR_FAILED  Failed to complete the operation.
 *
 */
otError otTcpReceiveByReference(const otTcpEndpoint *aEndpoint, const otLinkedBuffer **aBuffer);

/**
 * Reorganizes the receive buffer to be entirely contiguous in memory.
 *
 * This is optional; an application can simply traverse the linked buffer
 * chain obtained by calling @p otTcpReceiveByReference. Some
 * applications may wish to call this function to make the receive buffer
 * contiguous to simplify their data processing, but this comes at the expense
 * of CPU time to reorganize the data in the receive buffer.
 *
 * @param[in]   aEndpoint  A pointer to the TCP endpoint whose receive buffer to reorganize.
 *
 * @retval OT_ERROR_NONE    Successfully completed the operation.
 * @retval OT_ERROR_FAILED  Failed to complete the operation.
 *
 */
otError otTcpReceiveContiguify(otTcpEndpoint *aEndpoint);

/**
 * Informs the TCP stack that the application has finished processing
 * @p aNumBytes bytes of data at the start of the receive buffer and that the
 * TCP stack need not continue maintaining those bytes in the receive buffer.
 *
 * @param[in]  aEndpoint  A pointer to the TCP endpoint structure representing the TCP endpoint on which to receive
 *                        data.
 * @param[in]  aNumBytes  The number of bytes consumed.
 * @param[in]  aFlags     Flags specifying options for this operation (none yet).
 *
 * @retval OT_ERROR_NONE    Successfully completed the receive operation.
 * @retval OT_ERROR_FAILED  Failed to complete the receive operation.
 *
 */
otError otTcpCommitReceive(otTcpEndpoint *aEndpoint, size_t aNumBytes, uint32_t aFlags);

/**
 * Informs the connection peer that this TCP endpoint will not send more data.
 *
 * This should be used when the application has no more data to send to the
 * connection peer. For this connection, future reads on the connection peer
 * will result in the "end of stream" condition, and future writes on this
 * connection endpoint will fail.
 *
 * The "end of stream" condition only applies after any data previously
 * provided to the TCP stack to send out has been received by the connection
 * peer.
 *
 * @param[in]  aEndpoint  A pointer to the TCP endpoint structure representing the TCP endpoint to shut down.
 *
 * @retval OT_ERROR_NONE    Successfully queued the "end of stream" condition for transmission.
 * @retval OT_ERROR_FAILED  Failed to queue the "end of stream" condition for transmission.
 *
 */
otError otTcpSendEndOfStream(otTcpEndpoint *aEndpoint);

/**
 * Forcibly ends the TCP connection associated with this TCP endpoint.
 *
 * This immediately makes the TCP endpoint free for use for another connection
 * and empties the send and receive buffers, transferring ownership of any data
 * provided by the application in otTcpSendByReference() calls back to
 * the application. The TCP endpoint's callbacks and memory for the receive
 * buffer remain associated with the TCP endpoint.
 *
 * @param[in]  aEndpoint  A pointer to the TCP endpoint structure representing the TCP endpoint to abort.
 *
 * @retval OT_ERROR_NONE    Successfully aborted the TCP endpoint's connection.
 * @retval OT_ERROR_FAILED  Failed to abort the TCP endpoint's connection.
 *
 */
otError otTcpAbort(otTcpEndpoint *aEndpoint);

/**
 * Deinitializes this TCP endpoint.
 *
 * This means that OpenThread no longer keeps track of this TCP endpoint and
 * deallocates all resources it has internally allocated for this TCP endpoint.
 * The application can reuse the memory backing the TCP endpoint as it sees fit.
 *
 * If it corresponds to a live TCP connection, the connection is terminated
 * unceremoniously (as in otTcpAbort()). All resources the application has
 * provided for this TCP endpoint (linked buffers for the send buffer, memory
 * for the receive buffer, the @p aEndpoint structure itself, etc.) are
 * immediately returned to the application.
 *
 * @param[in]  aEndpoint  A pointer to the TCP endpoint structure to deinitialize.
 *
 * @retval OT_ERROR_NONE    Successfully deinitialized the TCP endpoint.
 * @retval OT_ERROR_FAILED  Failed to deinitialize the TCP endpoint.
 *
 */
otError otTcpEndpointDeinitialize(otTcpEndpoint *aEndpoint);

struct otTcpListener;
typedef struct otTcpListener otTcpListener;

/**
 * This enumeration defines incoming connection actions.
 *
 * This is used in otTcpAcceptReady() callback.
 *
 */
typedef enum otTcpIncomingConnectionAction
{
    OT_TCP_INCOMING_CONNECTION_ACTION_ACCEPT, ///< Accept the incoming connection.
    OT_TCP_INCOMING_CONNECTION_ACTION_DEFER,  ///< Defer (silently ignore) the incoming connection.
    OT_TCP_INCOMING_CONNECTION_ACTION_REFUSE, ///< Refuse the incoming connection.
} otTcpIncomingConnectionAction;

/**
 * This callback indicates that an incoming connection that matches this TCP
 * listener has arrived.
 *
 * The typical response is for the application to accept the incoming
 * connection. It does so by populating @p aAcceptInto with a pointer to the
 * otTcpEndpoint into which to accept the incoming connection. This
 * otTcpEndpoint must already be initialized using otTcpEndpointInitialize().
 * Then, the application returns OT_TCP_INCOMING_CONNECTION_ACTION_ACCEPT.
 *
 * Alternatively, the application can decline to accept the incoming
 * connection. There are two ways for the application to do this. First, if the
 * application returns OT_TCP_INCOMING_CONNECTION_ACTION_DEFER, then OpenThread
 * silently ignores the connection establishment request; the connection peer
 * will likely retransmit the request, at which point the callback will be
 * called again. This is valuable if resources are not presently available to
 * accept the connection, but they may be available when the connection peer
 * retransmits its connection establishment attempt. Second, if the application
 * returns OT_TCP_INCOMING_CONNECTION_ACTION_REFUSE, then OpenThread sends a
 * "connection refused" message to the host that attempted to establish a
 * connection. If the application declines the incoming connection, it is not
 * required to populate @p aAcceptInto.
 *
 * @param[in]   aListener    The TCP listener that matches the incoming connection.
 * @param[in]   aPeer        The host and port from which the incoming connection originates.
 * @param[out]  aAcceptInto  The TCP endpoint into which to accept the incoming connection.
 *
 * @returns  Description of how to handle the incoming connection.
 *
 */
typedef otTcpIncomingConnectionAction (*otTcpAcceptReady)(otTcpListener *   aListener,
                                                          const otSockAddr *aPeer,
                                                          otTcpEndpoint **  aAcceptInto);

/**
 * This callback indicates that the TCP connection is now ready for two-way
 * communication.
 *
 * In the case of TCP Fast Open, this may be before the TCP
 * connection handshake has actually completed. The application is provided
 * with the context pointers both for the TCP listener that accepted the
 * connection and the TCP endpoint into which it was accepted. The provided
 * context is the one associated with the TCP listener.
 *
 * @param[in]  aListener  The TCP listener that matches the incoming connection.
 * @param[in]  aEndpoint  The TCP endpoint into which the incoming connection was accepted.
 * @param[in]  aPeer      the host and port from which the incoming connection originated.
 *
 */
typedef void (*otTcpAcceptDone)(otTcpListener *aListener, otTcpEndpoint *aEndpoint, const otSockAddr *aPeer);

/**
 * This structure represents a TCP listener.
 *
 * A TCP listener is used to listen for and accept incoming TCP connections.
 *
 * The application should not inspect the fields of this structure directly; it
 * should only interact with it via the TCP API functions whose signatures are
 * provided in this file.
 *
 */
struct otTcpListener
{
    struct otTcpListener *mNext;     ///< A pointer to the next TCP listener (internal use only)
    otInstance *          mInstance; ///< A pointer to the OpenThread instance associated with this TCP listener
    void *                mContext;  ///< A pointer to application-specific context

    otTcpAcceptReady mAcceptReadyCallback; ///< "Accept ready" callback function
    otTcpAcceptDone  mAcceptDoneCallback;  ///< "Accept done" callback function

    /* Other implementation-defined fields go here. */
};

/**
 * This structure contains arguments to the otTcpListenerInitialize() function.
 *
 */
typedef struct otTcpListenerInitializeArgs
{
    void *mContext; ///< Pointer to application-specific context

    otTcpAcceptReady mAcceptReadyCallback; ///< "Accept ready" callback function
    otTcpAcceptDone  mAcceptDoneCallback;  ///< "Accept done" callback function
} otTcpListenerInitializeArgs;

/**
 * Initializes a TCP listener.
 *
 * Calling this function causes OpenThread to keep track of the TCP listener
 * and store and retrieve TCP data inside @p aListener. The application should
 * refrain from directly accessing or modifying the fields in @p aListener. If
 * the application needs to reclaim the memory backing @p aListener, it should
 * call otTcpListenerDeinitialize().
 *
 * @param[in]  aInstance  A pointer to an OpenThread instance.
 * @param[in]  aListener  A pointer to a TCP listener structure.
 * @param[in]  aArgs      A pointer to a structure of arguments.
 *
 * @retval OT_ERROR_NONE    Successfully opened the TCP listener.
 * @retval OT_ERROR_FAILED  Failed to open the TCP listener.
 *
 */
otError otTcpListenerInitialize(otInstance *aInstance, otTcpListener *aListener, otTcpListenerInitializeArgs *aArgs);

/**
 * Obtains the otInstance that was associated with @p aListener upon
 * initialization.
 *
 * @param[in]  aListener  The TCP listener whose instance to obtain.
 *
 * @returns  The otInstance pointer associated with @p aListener.
 *
 */
otInstance *otTcpListenerGetInstance(otTcpListener *aListener);

/**
 * Obtains the context pointer that was associated with @p aListener upon
 * initialization.
 *
 * @param[in]  aListener  The TCP listener whose context to obtain.
 *
 * @returns  The context pointer associated with @p aListener.
 *
 */
void *otTcpListenerGetContext(otTcpListener *aListener);

/**
 * Causes incoming TCP connections that match the specified IP address and port
 * to trigger this TCP listener's callbacks.
 *
 * @param[in]  aListener  A pointer to the TCP listener structure that should begin listening.
 * @param[in]  aSockName  The address and port on which to listen for incoming connections.
 *
 * @retval OT_ERROR_NONE    Successfully initiated listening on the TCP listener.
 * @retval OT_ERROR_FAILED  Failed to initiate listening on the TCP listener.
 *
 */
otError otTcpListen(otTcpListener *aListener, const otSockAddr *aSockName);

/**
 * Causes this TCP listener to stop listening for incoming connections.
 *
 * @param[in]  aListener  A pointer to the TCP listener structure that should stop listening.
 *
 * @retval OT_ERROR_NONE    Successfully stopped listening on the TCP listener.
 * @retval OT_ERROR_FAILED  Failed to stop listening on the TCP listener.
 *
 */
otError otTcpStopListening(otTcpListener *aListener);

/**
 * Deinitializes this TCP listener.
 *
 * This means that OpenThread no longer keeps track of this TCP listener and
 * deallocates all resources it has internally allocated for this TCP listener.
 * The application can reuse the memory backing the TCP listener as it sees
 * fit.
 *
 * If the TCP listener is currently listening, it stops listening.
 *
 * @param[in]  aListener  A pointer to the TCP listener structure to deinitialize.
 *
 * @retval OT_ERROR_NONE    Successfully deinitialized the TCP listener.
 * @retval OT_ERROR_FAILED  Failed to deinitialize the TCP listener.
 *
 */
otError otTcpListenerDeinitialize(otTcpListener *aListener);

/**
 * @}
 *
 */

#ifdef __cplusplus
} // extern "C"
#endif

#endif // OPENTHREAD_TCP_H_
