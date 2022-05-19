/*
 *  Copyright (c) 2020, The OpenThread Authors.
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
 *  This file defines the OpenThread SRP (Service Registration Protocol) client APIs.
 */

#ifndef OPENTHREAD_SRP_CLIENT_H_
#define OPENTHREAD_SRP_CLIENT_H_

#include <openthread/dns.h>
#include <openthread/ip6.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup api-srp
 *
 * @brief
 *   This module includes functions that control SRP client behavior.
 *
 * @{
 *
 */

/**
 * This enumeration specifies an SRP client item (service or host info) state.
 *
 */
typedef enum
{
    OT_SRP_CLIENT_ITEM_STATE_TO_ADD,     ///< Item to be added/registered.
    OT_SRP_CLIENT_ITEM_STATE_ADDING,     ///< Item is being added/registered.
    OT_SRP_CLIENT_ITEM_STATE_TO_REFRESH, ///< Item to be refreshed (re-register to renew lease).
    OT_SRP_CLIENT_ITEM_STATE_REFRESHING, ///< Item is being refreshed.
    OT_SRP_CLIENT_ITEM_STATE_TO_REMOVE,  ///< Item to be removed.
    OT_SRP_CLIENT_ITEM_STATE_REMOVING,   ///< Item is being removed.
    OT_SRP_CLIENT_ITEM_STATE_REGISTERED, ///< Item is registered with server.
    OT_SRP_CLIENT_ITEM_STATE_REMOVED,    ///< Item is removed.
} otSrpClientItemState;

/**
 * This structure represents an SRP client host info.
 *
 */
typedef struct otSrpClientHostInfo
{
    const char *         mName;         ///< Host name (label) string (NULL if not yet set).
    const otIp6Address * mAddresses;    ///< Pointer to an array of host IPv6 addresses (NULL if not yet set).
    uint8_t              mNumAddresses; ///< Number of IPv6 addresses in `mAddresses` array.
    otSrpClientItemState mState;        ///< Host info state.
} otSrpClientHostInfo;

/**
 * This structure represents an SRP client service.
 *
 * The values in this structure, including the string buffers for the names and the TXT record entries, MUST persist
 * and stay constant after an instance of this structure is passed to OpenThread from `otSrpClientAddService()` or
 * `otSrpClientRemoveService()`.
 *
 */
typedef struct otSrpClientService
{
    const char *         mName;          ///< The service name labels (e.g., "_chip._udp", not the full domain name).
    const char *         mInstanceName;  ///< The service instance name label (not the full name).
    const char *const *  mSubTypeLabels; ///< Array of service sub-type labels (must end with `NULL` or can be `NULL`).
    const otDnsTxtEntry *mTxtEntries;    ///< Array of TXT entries (number of entries is given by `mNumTxtEntries`).
    uint16_t             mPort;          ///< The service port number.
    uint16_t             mPriority;      ///< The service priority.
    uint16_t             mWeight;        ///< The service weight.
    uint8_t              mNumTxtEntries; ///< Number of entries in the `mTxtEntries` array.

    /**
     * @note The following fields are used/managed by OT core only. Their values do not matter and are ignored when an
     * instance of `otSrpClientService` is passed in `otSrpClientAddService()` or `otSrpClientRemoveService()`. The
     * user should not modify these fields.
     *
     */

    otSrpClientItemState       mState; ///< Service state (managed by OT core).
    uint32_t                   mData;  ///< Internal data (used by OT core).
    struct otSrpClientService *mNext;  ///< Pointer to next entry in a linked-list (managed by OT core).
} otSrpClientService;

/**
 * This function pointer type defines the callback used by SRP client to notify user of changes/events/errors.
 *
 * This callback is invoked on a successful registration of an update (i.e., add/remove of host-info and/or some
 * service(s)) with the SRP server, or if there is a failure or error (e.g., server rejects a update request or client
 * times out waiting for response, etc).
 *
 * In case of a successful reregistration of an update, `aError` parameter would be `OT_ERROR_NONE` and the host info
 * and the full list of services is provided as input parameters to the callback. Note that host info and services each
 * track its own state in the corresponding `mState` member variable of the related data structure (the state
 * indicating whether the host-info/service is registered or removed or still being added/removed, etc).
 *
 * The list of removed services is passed as its own linked-list `aRemovedServices` in the callback. Note that when the
 * callback is invoked, the SRP client (OpenThread implementation) is done with the removed service instances listed in
 * `aRemovedServices` and no longer tracks/stores them (i.e., if from the callback we call `otSrpClientGetServices()`
 * the removed services will not be present in the returned list). Providing a separate list of removed services in
 * the callback helps indicate to user which items are now removed and allow user to re-claim/reuse the instances.
 *
 * If the server rejects an SRP update request, the DNS response code (RFC 2136) is mapped to the following errors:
 *
 *  - (0)  NOERROR   Success (no error condition)                    -> OT_ERROR_NONE
 *  - (1)  FORMERR   Server unable to interpret due to format error  -> OT_ERROR_PARSE
 *  - (2)  SERVFAIL  Server encountered an internal failure          -> OT_ERROR_FAILED
 *  - (3)  NXDOMAIN  Name that ought to exist, does not exist        -> OT_ERROR_NOT_FOUND
 *  - (4)  NOTIMP    Server does not support the query type (OpCode) -> OT_ERROR_NOT_IMPLEMENTED
 *  - (5)  REFUSED   Server refused for policy/security reasons      -> OT_ERROR_SECURITY
 *  - (6)  YXDOMAIN  Some name that ought not to exist, does exist   -> OT_ERROR_DUPLICATED
 *  - (7)  YXRRSET   Some RRset that ought not to exist, does exist  -> OT_ERROR_DUPLICATED
 *  - (8)  NXRRSET   Some RRset that ought to exist, does not exist  -> OT_ERROR_NOT_FOUND
 *  - (9)  NOTAUTH   Service is not authoritative for zone           -> OT_ERROR_SECURITY
 *  - (10) NOTZONE   A name is not in the zone                       -> OT_ERROR_PARSE
 *  - (20) BADNAME   Bad name                                        -> OT_ERROR_PARSE
 *  - (21) BADALG    Bad algorithm                                   -> OT_ERROR_SECURITY
 *  - (22) BADTRUN   Bad truncation                                  -> OT_ERROR_PARSE
 *  - Other response codes                                           -> OT_ERROR_FAILED
 *
 * The following errors are also possible:
 *
 *  - OT_ERROR_RESPONSE_TIMEOUT : Timed out waiting for response from server (client would continue to retry).
 *  - OT_ERROR_INVALID_ARGS     : The provided service structure is invalid (e.g., bad service name or `otDnsTxtEntry`).
 *  - OT_ERROR_NO_BUFS          : Insufficient buffer to prepare or send the update message.
 *
 * Note that in case of any failure, the client continues the operation, i.e. it prepares and (re)transmits the SRP
 * update message to the server, after some wait interval. The retry wait interval starts from the minimum value and
 * is increased by the growth factor every failure up to the max value (please see configuration parameter
 * `OPENTHREAD_CONFIG_SRP_CLIENT_MIN_RETRY_WAIT_INTERVAL` and the related ones for more details).
 *
 * @param[in] aError            The error (see above).
 * @param[in] aHostInfo         A pointer to host info.
 * @param[in] aService          The head of linked-list containing all services (excluding the ones removed). NULL if
 *                              the list is empty.
 * @param[in] aRemovedServices  The head of linked-list containing all removed services. NULL if the list is empty.
 * @param[in] aContext          A pointer to an arbitrary context (provided when callback was registered).
 *
 */
typedef void (*otSrpClientCallback)(otError                    aError,
                                    const otSrpClientHostInfo *aHostInfo,
                                    const otSrpClientService * aServices,
                                    const otSrpClientService * aRemovedServices,
                                    void *                     aContext);

/**
 * This function pointer type defines the callback used by SRP client to notify user when it is auto-started or stopped.
 *
 * This is only used when auto-start feature `OPENTHREAD_CONFIG_SRP_CLIENT_AUTO_START_API_ENABLE` is enabled.
 *
 * This callback is invoked when auto-start mode is enabled and the SRP client is either automatically started or
 * stopped.
 *
 * @param[in] aServerSockAddress   A non-NULL pointer indicates SRP server was started and pointer will give the
 *                                 selected server socket address. A NULL pointer indicates SRP server was stopped.
 * @param[in] aContext             A pointer to an arbitrary context (provided when callback was registered).
 *
 */
typedef void (*otSrpClientAutoStartCallback)(const otSockAddr *aServerSockAddr, void *aContext);

/**
 * This function starts the SRP client operation.
 *
 * SRP client will prepare and send "SRP Update" message to the SRP server once all the following conditions are met:
 *
 *  - The SRP client is started - `otSrpClientStart()` is called.
 *  - Host name is set - `otSrpClientSetHostName()` is called.
 *  - At least one host IPv6 address is set - `otSrpClientSetHostName()` is called.
 *  - At least one service is added - `otSrpClientAddService()` is called.
 *
 * It does not matter in which order these functions are called. When all conditions are met, the SRP client will
 * wait for a short delay before preparing an "SRP Update" message and sending it to server. This delay allows for user
 * to add multiple services and/or IPv6 addresses before the first SRP Update message is sent (ensuring a single SRP
 * Update is sent containing all the info). The config `OPENTHREAD_CONFIG_SRP_CLIENT_UPDATE_TX_DELAY` specifies the
 * delay interval.
 *
 * @param[in] aInstance        A pointer to the OpenThread instance.
 * @param[in] aServerSockAddr  The socket address (IPv6 address and port number) of the SRP server.
 *
 * @retval OT_ERROR_NONE       SRP client operation started successfully or it is already running with same server
 *                             socket address and callback.
 * @retval OT_ERROR_BUSY       SRP client is busy running with a different socket address.
 * @retval OT_ERROR_FAILED     Failed to open/connect the client's UDP socket.
 *
 */
otError otSrpClientStart(otInstance *aInstance, const otSockAddr *aServerSockAddr);

/**
 * This function stops the SRP client operation.
 *
 * This function stops any further interactions with the SRP server. Note that it does not remove or clear host info
 * and/or list of services. It marks all services to be added/removed again once the client is (re)started.
 *
 * @param[in] aInstance       A pointer to the OpenThread instance.
 *
 */
void otSrpClientStop(otInstance *aInstance);

/**
 * This function indicates whether the SRP client is running or not.
 *
 * @param[in] aInstance       A pointer to the OpenThread instance.
 *
 * @returns TRUE if the SRP client is running, FALSE otherwise.
 *
 */
bool otSrpClientIsRunning(otInstance *aInstance);

/**
 * This function gets the socket address (IPv6 address and port number) of the SRP server which is being used by SRP
 * client.
 *
 * If the client is not running, the address is unspecified (all zero) with zero port number.
 *
 * @param[in] aInstance       A pointer to the OpenThread instance.
 *
 * @returns A pointer to the SRP server's socket address (is always non-NULL).
 *
 */
const otSockAddr *otSrpClientGetServerAddress(otInstance *aInstance);

/**
 * This function sets the callback to notify caller of events/changes from SRP client.
 *
 * The SRP client allows a single callback to be registered. So consecutive calls to this function will overwrite any
 * previously set callback functions.
 *
 * @param[in] aInstance   A pointer to the OpenThread instance.
 * @param[in] aCallback   The callback to notify of events and changes. Can be NULL if not needed.
 * @param[in] aContext    An arbitrary context used with @p aCallback.
 *
 */
void otSrpClientSetCallback(otInstance *aInstance, otSrpClientCallback aCallback, void *aContext);

/**
 * This function enables the auto-start mode.
 *
 * This is only available when auto-start feature `OPENTHREAD_CONFIG_SRP_CLIENT_AUTO_START_API_ENABLE` is enabled.
 *
 * Config option `OPENTHREAD_CONFIG_SRP_CLIENT_AUTO_START_DEFAULT_MODE` specifies the default auto-start mode (whether
 * it is enabled or disabled at the start of OT stack).
 *
 * When auto-start is enabled, the SRP client will monitor the Thread Network Data for SRP Server Service entries
 * and automatically start and stop the client when an SRP server is detected.
 *
 * If multiple SRP servers are found, a random one will be selected. If the selected SRP server is no longer
 * detected (not longer present in the Thread Network Data), the SRP client will be stopped and then it may switch
 * to another SRP server (if available).
 *
 * When the SRP client is explicitly started through a successful call to `otSrpClientStart()`, the given SRP server
 * address in `otSrpClientStart()` will continue to be used regardless of the state of auto-start mode and whether the
 * same SRP server address is discovered or not in the Thread Network Data. In this case, only an explicit
 * `otSrpClientStop()` call will stop the client.
 *
 * @param[in] aInstance   A pointer to the OpenThread instance.
 * @param[in] aCallback   A callback to notify when client is auto-started/stopped. Can be NULL if not needed.
 * @param[in] aContext    A context to be passed when invoking @p aCallback.
 *
 */
void otSrpClientEnableAutoStartMode(otInstance *aInstance, otSrpClientAutoStartCallback aCallback, void *aContext);

/**
 * This function disables the auto-start mode.
 *
 * This is only available when auto-start feature `OPENTHREAD_CONFIG_SRP_CLIENT_AUTO_START_API_ENABLE` is enabled.
 *
 * Disabling the auto-start mode will not stop the client if it is already running but the client stops monitoring
 * the Thread Network Data to verify that the selected SRP server is still present in it.
 *
 * Note that a call to `otSrpClientStop()` will also disable the auto-start mode.
 *
 * @param[in] aInstance   A pointer to the OpenThread instance.
 *
 */
void otSrpClientDisableAutoStartMode(otInstance *aInstance);

/**
 * This function indicates the current state of auto-start mode (enabled or disabled).
 *
 * This is only available when auto-start feature `OPENTHREAD_CONFIG_SRP_CLIENT_AUTO_START_API_ENABLE` is enabled.
 *
 * @param[in] aInstance   A pointer to the OpenThread instance.
 *
 * @returns TRUE if the auto-start mode is enabled, FALSE otherwise.
 *
 */
bool otSrpClientIsAutoStartModeEnabled(otInstance *aInstance);

/**
 * This function gets the lease interval used in SRP update requests.
 *
 * Note that this is the lease duration requested by the SRP client. The server may choose to accept a different lease
 * interval.
 *
 * @param[in] aInstance        A pointer to the OpenThread instance.
 *
 * @returns The lease interval (in seconds).
 *
 */
uint32_t otSrpClientGetLeaseInterval(otInstance *aInstance);

/**
 * This function sets the lease interval used in SRP update requests.
 *
 * Changing the lease interval does not impact the accepted lease interval of already registered services/host-info.
 * It only affects any future SRP update messages (i.e., adding new services and/or refreshes of the existing services).
 *
 * @param[in] aInstance   A pointer to the OpenThread instance.
 * @param[in] aInterval   The lease interval (in seconds). If zero, the default value specified by
 *                        `OPENTHREAD_CONFIG_SRP_CLIENT_DEFAULT_LEASE` would be used.
 *
 */
void otSrpClientSetLeaseInterval(otInstance *aInstance, uint32_t aInterval);

/**
 * This function gets the key lease interval used in SRP update requests.
 *
 * Note that this is the lease duration requested by the SRP client. The server may choose to accept a different lease
 * interval.
 *
 * @param[in] aInstance        A pointer to the OpenThread instance.
 *
 * @returns The key lease interval (in seconds).
 *
 */
uint32_t otSrpClientGetKeyLeaseInterval(otInstance *aInstance);

/**
 * This function sets the key lease interval used in SRP update requests.
 *
 * Changing the lease interval does not impact the accepted lease interval of already registered services/host-info.
 * It only affects any future SRP update messages (i.e., adding new services and/or refreshes of existing services).
 *
 * @param[in] aInstance    A pointer to the OpenThread instance.
 * @param[in] aInterval    The key lease interval (in seconds). If zero, the default value specified by
 *                         `OPENTHREAD_CONFIG_SRP_CLIENT_DEFAULT_KEY_LEASE` would be used.
 *
 */
void otSrpClientSetKeyLeaseInterval(otInstance *aInstance, uint32_t aInterval);

/**
 * This function gets the host info.
 *
 * @param[in] aInstance        A pointer to the OpenThread instance.
 *
 * @returns A pointer to host info structure.
 *
 */
const otSrpClientHostInfo *otSrpClientGetHostInfo(otInstance *aInstance);

/**
 * This function sets the host name label.
 *
 * After a successful call to this function, `otSrpClientCallback` will be called to report the status of host info
 * registration with SRP server.
 *
 * The name string buffer pointed to by @p aName MUST persist and stay unchanged after returning from this function.
 * OpenThread will keep the pointer to the string.
 *
 * The host name can be set before client is started or after start but before host info is registered with server
 * (host info should be in either `STATE_TO_ADD` or `STATE_REMOVED`).
 *
 * @param[in] aInstance   A pointer to the OpenThread instance.
 * @param[in] aName       A pointer to host name label string (MUST NOT be NULL). Pointer to the string buffer MUST
 *                        persist and remain valid and constant after return from this function.
 *
 * @retval OT_ERROR_NONE            The host name label was set successfully.
 * @retval OT_ERROR_INVALID_ARGS    The @p aName is NULL.
 * @retval OT_ERROR_INVALID_STATE   The host name is already set and registered with the server.
 *
 */
otError otSrpClientSetHostName(otInstance *aInstance, const char *aName);

/**
 * This function sets/updates the list of host IPv6 address.
 *
 * Host IPv6 addresses can be set/changed before start or during operation of SRP client (e.g. to add/remove or change
 * a previously registered host address), except when the host info is being removed (client is busy handling a remove
 * request from an earlier call to `otSrpClientRemoveHostAndServices()` and host info still being in  either
 * `STATE_TO_REMOVE` or `STATE_REMOVING` states).
 *
 * The host IPv6 address array pointed to by @p aAddresses MUST persist and remain unchanged after returning from this
 * function (with `OT_ERROR_NONE`). OpenThread will save the pointer to the array.
 *
 * After a successful call to this function, `otSrpClientCallback` will be called to report the status of the address
 * registration with SRP server.
 *
 * @param[in] aInstance           A pointer to the OpenThread instance.
 * @param[in] aAddresses          A pointer to the an array containing the host IPv6 addresses.
 * @param[in] aNumAddresses       The number of addresses in the @p aAddresses array.
 *
 * @retval OT_ERROR_NONE            The host IPv6 address list change started successfully. The `otSrpClientCallback`
 *                                  will be called to report the status of registering addresses with server.
 * @retval OT_ERROR_INVALID_ARGS    The address list is invalid (e.g., must contain at least one address).
 * @retval OT_ERROR_INVALID_STATE   Host is being removed and therefore cannot change host address.
 *
 */
otError otSrpClientSetHostAddresses(otInstance *aInstance, const otIp6Address *aIp6Addresses, uint8_t aNumAddresses);

/**
 * This function adds a service to be registered with server.
 *
 * After a successful call to this function, `otSrpClientCallback` will be called to report the status of the service
 * addition/registration with SRP server.
 *
 * The `otSrpClientService` instance being pointed to by @p aService MUST persist and remain unchanged after returning
 * from this function (with `OT_ERROR_NONE`). OpenThread will save the pointer to the service instance.
 *
 * The `otSrpClientService` instance is not longer tracked by OpenThread and can be reclaimed only when
 *
 *  -  It is removed explicitly by a call to `otSrpClientRemoveService()` or removed along with other services by a
 *     call to `otSrpClientRemoveHostAndServices() and only after the `otSrpClientCallback` is called indicating the
 *     service was removed. Or,
 *  -  A call to `otSrpClientClearHostAndServices()` which removes the host and all related services immediately.
 *
 * @param[in] aInstance        A pointer to the OpenThread instance.
 * @param[in] aService         A pointer to a `otSrpClientService` instance to add.

 * @retval OT_ERROR_NONE          The addition of service started successfully. The `otSrpClientCallback` will be
 *                                called to report the status.
 * @retval OT_ERROR_ALREADY       A service with the same service and instance names is already in the list.
 * @retval OT_ERROR_INVALID_ARGS  The service structure is invalid (e.g., bad service name or `otDnsTxtEntry`).
 *
 */
otError otSrpClientAddService(otInstance *aInstance, otSrpClientService *aService);

/**
 * This function requests a service to be unregistered with server.
 *
 * After a successful call to this function, `otSrpClientCallback` will be called to report the status of remove
 * request with SRP server.

 * The `otSrpClientService` instance being pointed to by @p aService MUST persist and remain unchanged after returning
 * from this function (with `OT_ERROR_NONE`). OpenThread will keep the service instance during the remove process.
 * Only after the `otSrpClientCallback` is called indicating the service instance is removed from SRP client
 * service list and can be be freed/reused.
 *
 * @param[in] aInstance        A pointer to the OpenThread instance.
 * @param[in] aService         A pointer to a `otSrpClientService` instance to remove.
 *
 * @retval OT_ERROR_NONE       The removal of service started successfully. The `otSrpClientCallback` will be called to
 *                             report the status.
 * @retval OT_ERROR_NOT_FOUND  The service could not be found in the list.
 *
 */
otError otSrpClientRemoveService(otInstance *aInstance, otSrpClientService *aService);

/**
 * This function clears a service, immediately removing it from the client service list.
 *
 * Unlike `otSrpClientRemoveService()` which sends an update message to the server to remove the service, this function
 * clears the service from the client's service list without any interaction with the server. On a successful call to
 * this function, the `otSrpClientCallback` will NOT be called and the @p aService entry can be reclaimed and re-used
 * by the caller immediately.
 *
 * This function can be used along with a subsequent call to `otSrpClientAddService()` (potentially reusing the same @p
 * aService entry with the same service and instance names) to update some of the parameters in an existing service.
 *
 * @param[in] aInstance        A pointer to the OpenThread instance.
 * @param[in] aService         A pointer to a `otSrpClientService` instance to delete.
 *
 * @retval OT_ERROR_NONE       The @p aService is deleted successfully. It can be reclaimed and re-used immediately.
 * @retval OT_ERROR_NOT_FOUND  The service could not be found in the list.
 *
 */
otError otSrpClientClearService(otInstance *aInstance, otSrpClientService *aService);

/**
 * This function gets the list of services being managed by client.
 *
 * @param[in] aInstance        A pointer to the OpenThread instance.
 *
 * @returns A pointer to the head of linked-list of all services or NULL if the list is empty.
 *
 */
const otSrpClientService *otSrpClientGetServices(otInstance *aInstance);

/**
 * This function starts the remove process of the host info and all services.
 *
 * After returning from this function, `otSrpClientCallback` will be called to report the status of remove request with
 * SRP server.
 *
 * If the host info is to be permanently removed from server, @p aRemoveKeyLease should be set to `true` which removes
 * the key lease associated with host on server. Otherwise, the key lease record is kept as before, which ensures
 * that the server holds the host name in reserve for when the client is once again able to provide and register its
 * service(s).
 *
 * The @p aSendUnregToServer determines the behavior when the host info is not yet registered with the server. If
 * @p aSendUnregToServer is set to `false` (which is the default/expected value) then the SRP client will immediately
 * remove the host info and services without sending an update message to server (no need to update the server if
 * nothing is yet registered with it). If @p aSendUnregToServer is set to `true` then the SRP client will send an
 * update message to the server. Note that if the host info is registered then the value of @p aSendUnregToServer does
 * not matter and the SRP client will always send an update message to server requesting removal of all info.
 *
 * One situation where @p aSendUnregToServer can be useful is on a device reset/reboot, caller may want to remove any
 * previously registered services with the server. In this case, caller can `otSrpClientSetHostName()` and then request
 * `otSrpClientRemoveHostAndServices()` with `aSendUnregToServer` as `true`.
 *
 * @param[in] aInstance          A pointer to the OpenThread instance.
 * @param[in] aRemoveKeyLease    A boolean indicating whether or not the host key lease should also be removed.
 * @param[in] aSendUnregToServer A boolean indicating whether to send update to server when host info is not registered.
 *
 * @retval OT_ERROR_NONE       The removal of host info and services started successfully. The `otSrpClientCallback`
 *                             will be called to report the status.
 * @retval OT_ERROR_ALREADY    The host info is already removed.
 *
 */
otError otSrpClientRemoveHostAndServices(otInstance *aInstance, bool aRemoveKeyLease, bool aSendUnregToServer);

/**
 * This function clears all host info and all the services.
 *
 * Unlike `otSrpClientRemoveHostAndServices()` which sends an update message to the server to remove all the info, this
 * function clears all the info immediately without any interaction with the server.
 *
 * @param[in] aInstance        A pointer to the OpenThread instance.
 *
 */
void otSrpClientClearHostAndServices(otInstance *aInstance);

/**
 * This function gets the domain name being used by SRP client.
 *
 * This function requires `OPENTHREAD_CONFIG_SRP_CLIENT_DOMAIN_NAME_API_ENABLE` to be enabled.
 *
 * If domain name is not set, "default.service.arpa" will be used.
 *
 * @param[in] aInstance        A pointer to the OpenThread instance.
 *
 * @returns The domain name string.
 *
 */
const char *otSrpClientGetDomainName(otInstance *aInstance);

/**
 * This function sets the domain name to be used by SRP client.
 *
 * This function requires `OPENTHREAD_CONFIG_SRP_CLIENT_DOMAIN_NAME_API_ENABLE` to be enabled.
 *
 * If not set "default.service.arpa" will be used.
 *
 * The name string buffer pointed to by @p aName MUST persist and stay unchanged after returning from this function.
 * OpenThread will keep the pointer to the string.
 *
 * The domain name can be set before client is started or after start but before host info is registered with server
 * (host info should be in either `STATE_TO_ADD` or `STATE_TO_REMOVE`).
 *
 * @param[in] aInstance     A pointer to the OpenThread instance.
 * @param[in] aName         A pointer to the domain name string. If NULL sets it to default "default.service.arpa".
 *
 * @retval OT_ERROR_NONE            The domain name label was set successfully.
 * @retval OT_ERROR_INVALID_STATE   The host info is already registered with server.
 *
 */
otError otSrpClientSetDomainName(otInstance *aInstance, const char *aName);

/**
 * This function converts a `otSrpClientItemState` to a string.
 *
 * @param[in] aItemState  An item state.
 *
 * @returns A string representation of @p aItemState.
 *
 */
const char *otSrpClientItemStateToString(otSrpClientItemState aItemState);

/**
 * This function enables/disables "service key record inclusion" mode.
 *
 * When enabled, SRP client will include KEY record in Service Description Instructions in the SRP update messages
 * that it sends.
 *
 * This function is available when `OPENTHREAD_CONFIG_REFERENCE_DEVICE_ENABLE` configuration is enabled.
 *
 * @note KEY record is optional in Service Description Instruction (it is required and always included in the Host
 * Description Instruction). The default behavior of SRP client is to not include it. This function is intended to
 * override the default behavior for testing only.
 *
 * @param[in] aInstance  A pointer to the OpenThread instance.
 * @param[in] aEnabled   TRUE to enable, FALSE to disable the "service key record inclusion" mode.
 *
 */
void otSrpClientSetServiceKeyRecordEnabled(otInstance *aInstance, bool aEnabled);

/**
 * This method indicates whether the "service key record inclusion" mode is enabled or disabled.
 *
 * This function is available when `OPENTHREAD_CONFIG_REFERENCE_DEVICE_ENABLE` configuration is enabled.
 *
 * @param[in] aInstance     A pointer to the OpenThread instance.
 *
 * @returns TRUE if "service key record inclusion" mode is enabled, FALSE otherwise.
 *
 */
bool otSrpClientIsServiceKeyRecordEnabled(otInstance *aInstance);

/**
 * @}
 *
 */

#ifdef __cplusplus
} // extern "C"
#endif

#endif // OPENTHREAD_SRP_CLIENT_H_
