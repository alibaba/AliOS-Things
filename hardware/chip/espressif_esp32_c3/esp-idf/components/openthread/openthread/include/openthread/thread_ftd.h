/*
 *  Copyright (c) 2016-2017, The OpenThread Authors.
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
 *  This file defines the OpenThread Thread API (FTD only).
 */

#ifndef OPENTHREAD_THREAD_FTD_H_
#define OPENTHREAD_THREAD_FTD_H_

#include <openthread/link.h>
#include <openthread/message.h>
#include <openthread/thread.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup api-thread-router
 *
 * @{
 *
 */

/**
 * This structure holds diagnostic information for a Thread Child
 *
 */
typedef struct
{
    otExtAddress mExtAddress;           ///< IEEE 802.15.4 Extended Address
    uint32_t     mTimeout;              ///< Timeout
    uint32_t     mAge;                  ///< Time last heard
    uint16_t     mRloc16;               ///< RLOC16
    uint16_t     mChildId;              ///< Child ID
    uint8_t      mNetworkDataVersion;   ///< Network Data Version
    uint8_t      mLinkQualityIn;        ///< Link Quality In
    int8_t       mAverageRssi;          ///< Average RSSI
    int8_t       mLastRssi;             ///< Last observed RSSI
    uint16_t     mFrameErrorRate;       ///< Frame error rate (0xffff->100%). Requires error tracking feature.
    uint16_t     mMessageErrorRate;     ///< (IPv6) msg error rate (0xffff->100%). Requires error tracking feature.
    uint16_t     mQueuedMessageCnt;     ///< Number of queued messages for the child.
    uint8_t      mVersion;              ///< MLE version
    bool         mRxOnWhenIdle : 1;     ///< rx-on-when-idle
    bool         mFullThreadDevice : 1; ///< Full Thread Device
    bool         mFullNetworkData : 1;  ///< Full Network Data
    bool         mIsStateRestoring : 1; ///< Is in restoring state
    bool         mIsCslSynced : 1;      ///< Is child CSL synchronized
} otChildInfo;

#define OT_CHILD_IP6_ADDRESS_ITERATOR_INIT 0 ///< Initializer for otChildIP6AddressIterator

typedef uint16_t otChildIp6AddressIterator; ///< Used to iterate through IPv6 addresses of a Thread Child entry.

/**
 * This enumeration defines the EID cache entry state.
 *
 */
typedef enum otCacheEntryState
{
    OT_CACHE_ENTRY_STATE_CACHED      = 0, // Entry is cached and in-use.
    OT_CACHE_ENTRY_STATE_SNOOPED     = 1, // Entry is created by snoop optimization (inspection of received msg).
    OT_CACHE_ENTRY_STATE_QUERY       = 2, // Entry represents an ongoing query for the EID.
    OT_CACHE_ENTRY_STATE_RETRY_QUERY = 3, // Entry is in retry wait mode (a prior query did not get a response).
} otCacheEntryState;

/**
 * This structure represents an EID cache entry.
 *
 */
typedef struct otCacheEntryInfo
{
    otIp6Address      mTarget;             ///< Target EID
    otShortAddress    mRloc16;             ///< RLOC16
    otCacheEntryState mState;              ///< Entry state
    bool              mCanEvict : 1;       ///< Indicates whether the entry can be evicted.
    bool              mValidLastTrans : 1; ///< Indicates whether last transaction time and ML-EID are valid.
    uint32_t          mLastTransTime;      ///< Last transaction time (applicable in cached state).
    otIp6Address      mMeshLocalEid;       ///< Mesh Local EID (applicable if entry in cached state).
    uint16_t          mTimeout;            ///< Timeout in seconds (applicable if in snooped/query/retry-query states).
    uint16_t          mRetryDelay;         ///< Retry delay in seconds (applicable if in query-retry state).
} otCacheEntryInfo;

/**
 * This type represents an iterator used for iterating through the EID cache table entries.
 *
 * To initialize the iterator and start from the first entry in the cache table, set all its fields in the structure to
 * zero (e.g., `memset` the iterator to zero).
 *
 */
typedef struct otCacheEntryIterator
{
    const void *mData[2]; ///< Opaque data used by the core implementation. Should not be changed by user.
} otCacheEntryIterator;

/**
 * Get the maximum number of children currently allowed.
 *
 * @param[in]  aInstance A pointer to an OpenThread instance.
 *
 * @returns The maximum number of children currently allowed.
 *
 * @sa otThreadSetMaxAllowedChildren
 *
 */
uint16_t otThreadGetMaxAllowedChildren(otInstance *aInstance);

/**
 * Set the maximum number of children currently allowed.
 *
 * This parameter can only be set when Thread protocol operation has been stopped.
 *
 * @param[in]  aInstance     A pointer to an OpenThread instance.
 * @param[in]  aMaxChildren  The maximum allowed children.
 *
 * @retval  OT_ERROR_NONE           Successfully set the max.
 * @retval  OT_ERROR_INVALID_ARGS   If @p aMaxChildren is not in the range [1, OPENTHREAD_CONFIG_MLE_MAX_CHILDREN].
 * @retval  OT_ERROR_INVALID_STATE  If Thread isn't stopped.
 *
 * @sa otThreadGetMaxAllowedChildren
 *
 */
otError otThreadSetMaxAllowedChildren(otInstance *aInstance, uint16_t aMaxChildren);

/**
 * This method indicates whether or not the device is router-eligible.
 *
 * @param[in]  aInstance A pointer to an OpenThread instance.
 *
 * @retval TRUE   If device is router-eligible.
 * @retval FALSE  If device is not router-eligible.
 *
 */
bool otThreadIsRouterEligible(otInstance *aInstance);

/**
 * This function sets whether or not the device is router-eligible.
 *
 * If @p aEligible is false and the device is currently operating as a router, this call will cause the device to
 * detach and attempt to reattach as a child.
 *
 * @param[in]  aInstance  A pointer to an OpenThread instance.
 * @param[in]  aEligible  TRUE to configure the device as router-eligible, FALSE otherwise.
 *
 * @retval OT_ERROR_NONE         Successfully set the router-eligible configuration.
 * @retval OT_ERROR_NOT_CAPABLE  The device is not capable of becoming a router.
 *
 */
otError otThreadSetRouterEligible(otInstance *aInstance, bool aEligible);

/**
 * Set the preferred Router Id.
 *
 * Upon becoming a router/leader the node attempts to use this Router Id. If the preferred Router Id is not set or if
 * it can not be used, a randomly generated router id is picked. This property can be set only when the device role is
 * either detached or disabled.
 *
 * @note This API is reserved for testing and demo purposes only. Changing settings with
 * this API will render a production application non-compliant with the Thread Specification.
 *
 * @param[in]  aInstance    A pointer to an OpenThread instance.
 * @param[in]  aRouterId    The preferred Router Id.
 *
 * @retval OT_ERROR_NONE          Successfully set the preferred Router Id.
 * @retval OT_ERROR_INVALID_STATE Could not set (role is not detached or disabled)
 *
 */
otError otThreadSetPreferredRouterId(otInstance *aInstance, uint8_t aRouterId);

/**
 * Get the Thread Leader Weight used when operating in the Leader role.
 *
 * @param[in]  aInstance A pointer to an OpenThread instance.
 *
 * @returns The Thread Leader Weight value.
 *
 * @sa otThreadSetLeaderWeight
 *
 */
uint8_t otThreadGetLocalLeaderWeight(otInstance *aInstance);

/**
 * Set the Thread Leader Weight used when operating in the Leader role.
 *
 * @param[in]  aInstance A pointer to an OpenThread instance.
 * @param[in]  aWeight   The Thread Leader Weight value.
 *
 * @sa otThreadGetLeaderWeight
 *
 */
void otThreadSetLocalLeaderWeight(otInstance *aInstance, uint8_t aWeight);

/**
 * Get the preferred Thread Leader Partition Id used when operating in the Leader role.
 *
 * @param[in]  aInstance A pointer to an OpenThread instance.
 *
 * @returns The Thread Leader Partition Id value.
 *
 */
uint32_t otThreadGetPreferredLeaderPartitionId(otInstance *aInstance);

/**
 * Set the preferred Thread Leader Partition Id used when operating in the Leader role.
 *
 * @param[in]  aInstance     A pointer to an OpenThread instance.
 * @param[in]  aPartitionId  The Thread Leader Partition Id value.
 *
 */
void otThreadSetPreferredLeaderPartitionId(otInstance *aInstance, uint32_t aPartitionId);

/**
 * Get the Joiner UDP Port.
 *
 * @param[in] aInstance A pointer to an OpenThread instance.
 *
 * @returns The Joiner UDP Port number.
 *
 * @sa otThreadSetJoinerUdpPort
 *
 */
uint16_t otThreadGetJoinerUdpPort(otInstance *aInstance);

/**
 * Set the Joiner UDP Port.
 *
 * @param[in]  aInstance       A pointer to an OpenThread instance.
 * @param[in]  aJoinerUdpPort  The Joiner UDP Port number.
 *
 * @retval  OT_ERROR_NONE  Successfully set the Joiner UDP Port.
 *
 * @sa otThreadGetJoinerUdpPort
 *
 */
otError otThreadSetJoinerUdpPort(otInstance *aInstance, uint16_t aJoinerUdpPort);

/**
 * Set Steering data out of band.
 *
 * Configuration option `OPENTHREAD_CONFIG_MLE_STEERING_DATA_SET_OOB_ENABLE` should be set to enable setting of steering
 * data out of band.
 *
 * @param[in]  aInstance       A pointer to an OpenThread instance.
 * @param[in]  aExtAddress     Address used to update the steering data.
 *                             All zeros to clear the steering data (no steering data).
 *                             All 0xFFs to set steering data/bloom filter to accept/allow all.
 *                             A specific EUI64 which is then added to current steering data/bloom filter.
 *
 */
void otThreadSetSteeringData(otInstance *aInstance, const otExtAddress *aExtAddress);

/**
 * Get the CONTEXT_ID_REUSE_DELAY parameter used in the Leader role.
 *
 * @param[in]  aInstance A pointer to an OpenThread instance.
 *
 * @returns The CONTEXT_ID_REUSE_DELAY value.
 *
 * @sa otThreadSetContextIdReuseDelay
 *
 */
uint32_t otThreadGetContextIdReuseDelay(otInstance *aInstance);

/**
 * Set the CONTEXT_ID_REUSE_DELAY parameter used in the Leader role.
 *
 * @note This API is reserved for testing and demo purposes only. Changing settings with
 * this API will render a production application non-compliant with the Thread Specification.
 *
 * @param[in]  aInstance A pointer to an OpenThread instance.
 * @param[in]  aDelay    The CONTEXT_ID_REUSE_DELAY value.
 *
 * @sa otThreadGetContextIdReuseDelay
 *
 */
void otThreadSetContextIdReuseDelay(otInstance *aInstance, uint32_t aDelay);

/**
 * Get the NETWORK_ID_TIMEOUT parameter used in the Router role.
 *
 * @note This API is reserved for testing and demo purposes only. Changing settings with
 * this API will render a production application non-compliant with the Thread Specification.
 *
 * @param[in]  aInstance A pointer to an OpenThread instance.
 *
 * @returns The NETWORK_ID_TIMEOUT value.
 *
 * @sa otThreadSetNetworkIdTimeout
 *
 */
uint8_t otThreadGetNetworkIdTimeout(otInstance *aInstance);

/**
 * Set the NETWORK_ID_TIMEOUT parameter used in the Leader role.
 *
 * @param[in]  aInstance A pointer to an OpenThread instance.
 * @param[in]  aTimeout  The NETWORK_ID_TIMEOUT value.
 *
 * @sa otThreadGetNetworkIdTimeout
 *
 */
void otThreadSetNetworkIdTimeout(otInstance *aInstance, uint8_t aTimeout);

/**
 * Get the ROUTER_UPGRADE_THRESHOLD parameter used in the REED role.
 *
 * @param[in]  aInstance A pointer to an OpenThread instance.
 *
 * @returns The ROUTER_UPGRADE_THRESHOLD value.
 *
 * @sa otThreadSetRouterUpgradeThreshold
 *
 */
uint8_t otThreadGetRouterUpgradeThreshold(otInstance *aInstance);

/**
 * Set the ROUTER_UPGRADE_THRESHOLD parameter used in the Leader role.
 *
 * @note This API is reserved for testing and demo purposes only. Changing settings with
 * this API will render a production application non-compliant with the Thread Specification.
 *
 * @param[in]  aInstance   A pointer to an OpenThread instance.
 * @param[in]  aThreshold  The ROUTER_UPGRADE_THRESHOLD value.
 *
 * @sa otThreadGetRouterUpgradeThreshold
 *
 */
void otThreadSetRouterUpgradeThreshold(otInstance *aInstance, uint8_t aThreshold);

/**
 * Release a Router ID that has been allocated by the device in the Leader role.
 *
 * @note This API is reserved for testing and demo purposes only. Changing settings with
 * this API will render a production application non-compliant with the Thread Specification.
 *
 * @param[in]  aInstance  A pointer to an OpenThread instance.
 * @param[in]  aRouterId  The Router ID to release. Valid range is [0, 62].
 *
 * @retval OT_ERROR_NONE           Successfully released the router id.
 * @retval OT_ERROR_INVALID_ARGS   @p aRouterId is not in the range [0, 62].
 * @retval OT_ERROR_INVALID_STATE  The device is not currently operating as a leader.
 * @retval OT_ERROR_NOT_FOUND      The router id is not currently allocated.
 *
 */
otError otThreadReleaseRouterId(otInstance *aInstance, uint8_t aRouterId);

/**
 * Attempt to become a router.
 *
 * @note This API is reserved for testing and demo purposes only. Changing settings with
 * this API will render a production application non-compliant with the Thread Specification.
 *
 * @param[in]  aInstance A pointer to an OpenThread instance.
 *
 * @retval OT_ERROR_NONE           Successfully begin attempt to become a router.
 * @retval OT_ERROR_INVALID_STATE  Thread is disabled.
 */
otError otThreadBecomeRouter(otInstance *aInstance);

/**
 * Become a leader and start a new partition.
 *
 * @note This API is reserved for testing and demo purposes only. Changing settings with
 * this API will render a production application non-compliant with the Thread Specification.
 *
 * @param[in]  aInstance A pointer to an OpenThread instance.
 *
 * @retval OT_ERROR_NONE           Successfully became a leader and started a new partition.
 * @retval OT_ERROR_INVALID_STATE  Thread is disabled.
 */
otError otThreadBecomeLeader(otInstance *aInstance);

/**
 * Get the ROUTER_DOWNGRADE_THRESHOLD parameter used in the Router role.
 *
 * @param[in]  aInstance  A pointer to an OpenThread instance.
 *
 * @returns The ROUTER_DOWNGRADE_THRESHOLD value.
 *
 * @sa otThreadSetRouterDowngradeThreshold
 *
 */
uint8_t otThreadGetRouterDowngradeThreshold(otInstance *aInstance);

/**
 * Set the ROUTER_DOWNGRADE_THRESHOLD parameter used in the Leader role.
 *
 * @note This API is reserved for testing and demo purposes only. Changing settings with
 * this API will render a production application non-compliant with the Thread Specification.
 *
 * @param[in]  aInstance   A pointer to an OpenThread instance.
 * @param[in]  aThreshold  The ROUTER_DOWNGRADE_THRESHOLD value.
 *
 * @sa otThreadGetRouterDowngradeThreshold
 *
 */
void otThreadSetRouterDowngradeThreshold(otInstance *aInstance, uint8_t aThreshold);

/**
 * Get the ROUTER_SELECTION_JITTER parameter used in the REED/Router role.
 *
 * @param[in]  aInstance   A pointer to an OpenThread instance.
 *
 * @returns The ROUTER_SELECTION_JITTER value.
 *
 * @sa otThreadSetRouterSelectionJitter
 *
 */
uint8_t otThreadGetRouterSelectionJitter(otInstance *aInstance);

/**
 * Set the ROUTER_SELECTION_JITTER parameter used in the REED/Router role.
 *
 * @note This API is reserved for testing and demo purposes only. Changing settings with
 * this API will render a production application non-compliant with the Thread Specification.
 *
 * @param[in]  aInstance      A pointer to an OpenThread instance.
 * @param[in]  aRouterJitter  The ROUTER_SELECTION_JITTER value.
 *
 * @sa otThreadGetRouterSelectionJitter
 *
 */
void otThreadSetRouterSelectionJitter(otInstance *aInstance, uint8_t aRouterJitter);

/**
 * The function retains diagnostic information for an attached Child by its Child ID or RLOC16.
 *
 * @param[in]   aInstance   A pointer to an OpenThread instance.
 * @param[in]   aChildId    The Child ID or RLOC16 for the attached child.
 * @param[out]  aChildInfo  A pointer to where the child information is placed.
 *
 * @retval OT_ERROR_NONE          @p aChildInfo was successfully updated with the info for the given ID.
 * @retval OT_ERROR_NOT_FOUND     No valid child with this Child ID.
 * @retval OT_ERROR_INVALID_ARGS  If @p aChildInfo is NULL.
 *
 */
otError otThreadGetChildInfoById(otInstance *aInstance, uint16_t aChildId, otChildInfo *aChildInfo);

/**
 * The function retains diagnostic information for an attached Child by the internal table index.
 *
 * @param[in]   aInstance    A pointer to an OpenThread instance.
 * @param[in]   aChildIndex  The table index.
 * @param[out]  aChildInfo   A pointer to where the child information is placed.
 *
 * @retval OT_ERROR_NONE             @p aChildInfo was successfully updated with the info for the given index.
 * @retval OT_ERROR_NOT_FOUND        No valid child at this index.
 * @retval OT_ERROR_INVALID_ARGS     Either @p aChildInfo is NULL, or @p aChildIndex is out of range (higher
 *                                   than max table index).
 *
 * @sa otGetMaxAllowedChildren
 *
 */
otError otThreadGetChildInfoByIndex(otInstance *aInstance, uint16_t aChildIndex, otChildInfo *aChildInfo);

/**
 * This function gets the next IPv6 address (using an iterator) for a given child.
 *
 * @param[in]     aInstance    A pointer to an OpenThread instance.
 * @param[in]     aChildIndex  The child index.
 * @param[inout]  aIterator    A pointer to the iterator. On success the iterator will be updated to point to next
 *                             entry in the list. To get the first IPv6 address the iterator should be set to
 *                             OT_CHILD_IP6_ADDRESS_ITERATOR_INIT.
 * @param[out]    aAddress     A pointer to an IPv6 address where the child's next address is placed (on success).
 *
 * @retval OT_ERROR_NONE          Successfully found the next IPv6 address (@p aAddress was successfully updated).
 * @retval OT_ERROR_NOT_FOUND     The child has no subsequent IPv6 address entry.
 * @retval OT_ERROR_INVALID_ARGS  @p aIterator or @p aAddress are NULL, or child at @p aChildIndex is not valid.
 *
 * @sa otThreadGetChildInfoByIndex
 *
 */
otError otThreadGetChildNextIp6Address(otInstance *               aInstance,
                                       uint16_t                   aChildIndex,
                                       otChildIp6AddressIterator *aIterator,
                                       otIp6Address *             aAddress);

/**
 * Get the current Router ID Sequence.
 *
 * @param[in]  aInstance A pointer to an OpenThread instance.
 *
 * @returns The Router ID Sequence.
 *
 */
uint8_t otThreadGetRouterIdSequence(otInstance *aInstance);

/**
 * The function returns the maximum allowed router ID
 *
 * @param[in]   aInstance    A pointer to an OpenThread instance.
 *
 * @returns The maximum allowed router ID.
 *
 */
uint8_t otThreadGetMaxRouterId(otInstance *aInstance);

/**
 * The function retains diagnostic information for a given Thread Router.
 *
 * @param[in]   aInstance    A pointer to an OpenThread instance.
 * @param[in]   aRouterId    The router ID or RLOC16 for a given router.
 * @param[out]  aRouterInfo  A pointer to where the router information is placed.
 *
 * @retval OT_ERROR_NONE          Successfully retrieved the router info for given id.
 * @retval OT_ERROR_NOT_FOUND     No router entry with the given id.
 * @retval OT_ERROR_INVALID_ARGS  @p aRouterInfo is NULL.
 *
 */
otError otThreadGetRouterInfo(otInstance *aInstance, uint16_t aRouterId, otRouterInfo *aRouterInfo);

/**
 * This function gets the next EID cache entry (using an iterator).
 *
 * @param[in]    aInstance   A pointer to an OpenThread instance.
 * @param[out]   aEntryInfo  A pointer to where the EID cache entry information is placed.
 * @param[inout] aIterator   A pointer to an iterator. It will be updated to point to next entry on success. To get the
 *                           first entry, initialize the iterator by setting all its fields to zero (e.g., `memset` the
 *                           the iterator structure to zero).
 *
 * @retval OT_ERROR_NONE          Successfully populated @p aEntryInfo for next EID cache entry.
 * @retval OT_ERROR_NOT_FOUND     No more entries in the address cache table.
 *
 */
otError otThreadGetNextCacheEntry(otInstance *aInstance, otCacheEntryInfo *aEntryInfo, otCacheEntryIterator *aIterator);

/**
 * Get the Thread PSKc
 *
 * @param[in]   aInstance   A pointer to an OpenThread instance.
 * @param[out]  aPskc       A pointer to an `otPskc` to return the retrieved Thread PSKc.
 *
 * @sa otThreadSetPskc
 *
 */
void otThreadGetPskc(otInstance *aInstance, otPskc *aPskc);

/**
 * Get Key Reference to Thread PSKc stored
 *
 * This function requires the build-time feature `OPENTHREAD_CONFIG_PLATFORM_KEY_REFERENCES_ENABLE` to be enabled.
 *
 * @param[in]   aInstance   A pointer to an OpenThread instance.
 *
 * @returns Key Reference to PSKc
 *
 * @sa otThreadSetPskcRef
 *
 */
otPskcRef otThreadGetPskcRef(otInstance *aInstance);

/**
 * Set the Thread PSKc
 *
 * This function will only succeed when Thread protocols are disabled.  A successful
 * call to this function will also invalidate the Active and Pending Operational Datasets in
 * non-volatile memory.
 *
 * @param[in]  aInstance   A pointer to an OpenThread instance.
 * @param[in]  aPskc       A pointer to the new Thread PSKc.
 *
 * @retval OT_ERROR_NONE           Successfully set the Thread PSKc.
 * @retval OT_ERROR_INVALID_STATE  Thread protocols are enabled.
 *
 * @sa otThreadGetPskc
 *
 */
otError otThreadSetPskc(otInstance *aInstance, const otPskc *aPskc);

/**
 * Set the Thread PSKc
 *
 * This function requires the build-time feature `OPENTHREAD_CONFIG_PLATFORM_KEY_REFERENCES_ENABLE` to be enabled.
 *
 * This function will only succeed when Thread protocols are disabled.  A successful
 * call to this function will also invalidate the Active and Pending Operational Datasets in
 * non-volatile memory.
 *
 * @param[in]  aInstance   A pointer to an OpenThread instance.
 * @param[in]  aPskcRef    Key Reference to the new Thread PSKc.
 *
 * @retval OT_ERROR_NONE           Successfully set the Thread PSKc.
 * @retval OT_ERROR_INVALID_STATE  Thread protocols are enabled.
 *
 * @sa otThreadGetPskcRef
 *
 */
otError otThreadSetPskcRef(otInstance *aInstance, otPskcRef aKeyRef);

/**
 * Get the assigned parent priority.
 *
 * @param[in]   aInstance   A pointer to an OpenThread instance.
 *
 * @returns The assigned parent priority value, -2 means not assigned.
 *
 * @sa otThreadSetParentPriority
 *
 */
int8_t otThreadGetParentPriority(otInstance *aInstance);

/**
 * Set the parent priority.
 *
 * @note This API is reserved for testing and demo purposes only. Changing settings with
 * this API will render a production application non-compliant with the Thread Specification.
 *
 * @param[in]  aInstance        A pointer to an OpenThread instance.
 * @param[in]  aParentPriority  The parent priority value.
 *
 * @retval OT_ERROR_NONE           Successfully set the parent priority.
 * @retval OT_ERROR_INVALID_ARGS   If the parent priority value is not among 1, 0, -1 and -2.
 *
 * @sa otThreadGetParentPriority
 *
 */
otError otThreadSetParentPriority(otInstance *aInstance, int8_t aParentPriority);

/**
 * This function gets the maximum number of IP addresses that each MTD child may register with this device as parent.
 *
 * @param[in]  aInstance    A pointer to an OpenThread instance.
 *
 * @returns The maximum number of IP addresses that each MTD child may register with this device as parent.
 *
 * @sa otThreadSetMaxChildIpAddresses
 *
 */
uint8_t otThreadGetMaxChildIpAddresses(otInstance *aInstance);

/**
 * This function sets/restores the maximum number of IP addresses that each MTD child may register with this
 * device as parent.
 *
 * @note This API requires `OPENTHREAD_CONFIG_REFERENCE_DEVICE_ENABLE`, and is only used by Thread Test Harness
 *       to limit the address registrations of the reference parent in order to test the MTD DUT reaction.
 *
 * @param[in]  aInstance        A pointer to an OpenThread instance.
 * @param[in]  aMaxIpAddresses  The maximum number of IP addresses that each MTD child may register with this
 *                              device as parent. 0 to clear the setting and restore the default.
 *
 * @retval OT_ERROR_NONE           Successfully set/cleared the number.
 * @retval OT_ERROR_INVALID_ARGS   If exceeds the allowed maximum number.
 *
 * @sa otThreadGetMaxChildIpAddresses
 *
 */
otError otThreadSetMaxChildIpAddresses(otInstance *aInstance, uint8_t aMaxIpAddresses);

/**
 * This enumeration defines the constants used in `otNeighborTableCallback` to indicate changes in neighbor table.
 *
 */
typedef enum
{
    OT_NEIGHBOR_TABLE_EVENT_CHILD_ADDED,        ///< A child is being added.
    OT_NEIGHBOR_TABLE_EVENT_CHILD_REMOVED,      ///< A child is being removed.
    OT_NEIGHBOR_TABLE_EVENT_CHILD_MODE_CHANGED, ///< An existing child's mode is changed.
    OT_NEIGHBOR_TABLE_EVENT_ROUTER_ADDED,       ///< A router is being added.
    OT_NEIGHBOR_TABLE_EVENT_ROUTER_REMOVED,     ///< A router is being removed.
} otNeighborTableEvent;

/**
 * This type represent a neighbor table entry info (child or router) and is used as a parameter in the neighbor table
 * callback `otNeighborTableCallback`.
 *
 */
typedef struct
{
    otInstance *mInstance; ///< The OpenThread instance.
    union
    {
        otChildInfo    mChild;  ///< The child neighbor info.
        otNeighborInfo mRouter; ///< The router neighbor info.
    } mInfo;
} otNeighborTableEntryInfo;

/**
 * This function pointer is called to notify that there is a change in the neighbor table.
 *
 * @param[in]  aEvent      A event flag.
 * @param[in]  aEntryInfo  A pointer to table entry info.
 *
 */
typedef void (*otNeighborTableCallback)(otNeighborTableEvent aEvent, const otNeighborTableEntryInfo *aEntryInfo);

/**
 * This function registers a neighbor table callback function.
 *
 * The provided callback (if non-NULL) will be invoked when there is a change in the neighbor table (e.g., a child or a
 * router neighbor entry is being added/removed or an existing child's mode is changed).
 *
 * Subsequent calls to this method will overwrite the previous callback.  Note that this callback in invoked while the
 * neighbor/child table is being updated and always before the `otStateChangedCallback`.
 *
 * @param[in] aInstance  A pointer to an OpenThread instance.
 * @param[in] aCallback  A pointer to callback handler function.
 *
 */
void otThreadRegisterNeighborTableCallback(otInstance *aInstance, otNeighborTableCallback aCallback);

/**
 * @}
 *
 */

#ifdef __cplusplus
} // extern "C"
#endif

#endif // OPENTHREAD_THREAD_FTD_H_
