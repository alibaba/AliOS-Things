/*
 *  Copyright (c) 2016, The OpenThread Authors.
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
 *  This file defines the OpenThread Operational Dataset API (for both FTD and MTD).
 */

#ifndef OPENTHREAD_DATASET_H_
#define OPENTHREAD_DATASET_H_

#include <openthread/instance.h>
#include <openthread/ip6.h>
#include <openthread/platform/crypto.h>
#include <openthread/platform/radio.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup api-operational-dataset
 *
 * @{
 *
 */

#define OT_NETWORK_KEY_SIZE 16 ///< Size of the Thread Network Key (bytes)

/**
 * @struct otNetworkKey
 *
 * This structure represents a Thread Network Key.
 *
 */
OT_TOOL_PACKED_BEGIN
struct otNetworkKey
{
    uint8_t m8[OT_NETWORK_KEY_SIZE]; ///< Byte values
} OT_TOOL_PACKED_END;

/**
 * This structure represents a Thread Network Key.
 *
 */
typedef struct otNetworkKey otNetworkKey;

/**
 * This datatype represents KeyRef to NetworkKey.
 *
 */
typedef otCryptoKeyRef otNetworkKeyRef; ///< Reference to Key

#define OT_NETWORK_NAME_MAX_SIZE 16 ///< Maximum size of the Thread Network Name field (bytes)

/**
 * This structure represents a Network Name.
 *
 * The `otNetworkName` is a null terminated C string (i.e., `m8` char array MUST end with null char `\0`).
 *
 */
typedef struct otNetworkName
{
    char m8[OT_NETWORK_NAME_MAX_SIZE + 1]; ///< Byte values. The `+ 1` is for null char.
} otNetworkName;

#define OT_EXT_PAN_ID_SIZE 8 ///< Size of a Thread PAN ID (bytes)

/**
 * This structure represents an Extended PAN ID.
 *
 */
OT_TOOL_PACKED_BEGIN
struct otExtendedPanId
{
    uint8_t m8[OT_EXT_PAN_ID_SIZE]; ///< Byte values
} OT_TOOL_PACKED_END;

/**
 * This structure represents an Extended PAN ID.
 *
 */
typedef struct otExtendedPanId otExtendedPanId;

#define OT_MESH_LOCAL_PREFIX_SIZE OT_IP6_PREFIX_SIZE ///< Size of the Mesh Local Prefix (bytes)

/**
 * This structure represents a Mesh Local Prefix.
 *
 */
typedef otIp6NetworkPrefix otMeshLocalPrefix;

#define OT_PSKC_MAX_SIZE 16 ///< Maximum size of the PSKc (bytes)

/**
 * This structure represents PSKc.
 *
 */
OT_TOOL_PACKED_BEGIN
struct otPskc
{
    uint8_t m8[OT_PSKC_MAX_SIZE]; ///< Byte values
} OT_TOOL_PACKED_END;

/**
 * This structure represents a PSKc.
 *
 */
typedef struct otPskc otPskc;

/**
 * This datatype represents KeyRef to PSKc.
 *
 */
typedef otCryptoKeyRef otPskcRef; ///< Reference to Key

/**
 * This structure represent Security Policy.
 *
 */
typedef struct otSecurityPolicy
{
    uint16_t mRotationTime; ///< The value for thrKeyRotation in units of hours.

    bool    mObtainNetworkKeyEnabled : 1;        ///< Obtaining the Network Key for out-of-band commissioning is enabled
    bool    mNativeCommissioningEnabled : 1;     ///< Native Commissioning using PSKc is allowed
    bool    mRoutersEnabled : 1;                 ///< Thread 1.0/1.1.x Routers are enabled
    bool    mExternalCommissioningEnabled : 1;   ///< External Commissioner authentication is allowed
    bool    mBeaconsEnabled : 1;                 ///< Thread 1.0/1.1.x Beacons are enabled
    bool    mCommercialCommissioningEnabled : 1; ///< Commercial Commissioning is enabled
    bool    mAutonomousEnrollmentEnabled : 1;    ///< Autonomous Enrollment is enabled
    bool    mNetworkKeyProvisioningEnabled : 1;  ///< Network Key Provisioning is enabled
    bool    mTobleLinkEnabled : 1;               ///< ToBLE link is enabled
    bool    mNonCcmRoutersEnabled : 1;           ///< Non-CCM Routers enabled
    uint8_t mVersionThresholdForRouting : 3;     ///< Version-threshold for Routing
} otSecurityPolicy;

/**
 * This type represents Channel Mask.
 *
 */
typedef uint32_t otChannelMask;

#define OT_CHANNEL_1_MASK (1 << 1)   ///< Channel 1
#define OT_CHANNEL_2_MASK (1 << 2)   ///< Channel 2
#define OT_CHANNEL_3_MASK (1 << 3)   ///< Channel 3
#define OT_CHANNEL_4_MASK (1 << 4)   ///< Channel 4
#define OT_CHANNEL_5_MASK (1 << 5)   ///< Channel 5
#define OT_CHANNEL_6_MASK (1 << 6)   ///< Channel 6
#define OT_CHANNEL_7_MASK (1 << 7)   ///< Channel 7
#define OT_CHANNEL_8_MASK (1 << 8)   ///< Channel 8
#define OT_CHANNEL_9_MASK (1 << 9)   ///< Channel 9
#define OT_CHANNEL_10_MASK (1 << 10) ///< Channel 10
#define OT_CHANNEL_11_MASK (1 << 11) ///< Channel 11
#define OT_CHANNEL_12_MASK (1 << 12) ///< Channel 12
#define OT_CHANNEL_13_MASK (1 << 13) ///< Channel 13
#define OT_CHANNEL_14_MASK (1 << 14) ///< Channel 14
#define OT_CHANNEL_15_MASK (1 << 15) ///< Channel 15
#define OT_CHANNEL_16_MASK (1 << 16) ///< Channel 16
#define OT_CHANNEL_17_MASK (1 << 17) ///< Channel 17
#define OT_CHANNEL_18_MASK (1 << 18) ///< Channel 18
#define OT_CHANNEL_19_MASK (1 << 19) ///< Channel 19
#define OT_CHANNEL_20_MASK (1 << 20) ///< Channel 20
#define OT_CHANNEL_21_MASK (1 << 21) ///< Channel 21
#define OT_CHANNEL_22_MASK (1 << 22) ///< Channel 22
#define OT_CHANNEL_23_MASK (1 << 23) ///< Channel 23
#define OT_CHANNEL_24_MASK (1 << 24) ///< Channel 24
#define OT_CHANNEL_25_MASK (1 << 25) ///< Channel 25
#define OT_CHANNEL_26_MASK (1 << 26) ///< Channel 26

/**
 * This structure represents presence of different components in Active or Pending Operational Dataset.
 *
 */
typedef struct otOperationalDatasetComponents
{
    bool mIsActiveTimestampPresent : 1;  ///< TRUE if Active Timestamp is present, FALSE otherwise.
    bool mIsPendingTimestampPresent : 1; ///< TRUE if Pending Timestamp is present, FALSE otherwise.
    bool mIsNetworkKeyPresent : 1;       ///< TRUE if Network Key is present, FALSE otherwise.
    bool mIsNetworkNamePresent : 1;      ///< TRUE if Network Name is present, FALSE otherwise.
    bool mIsExtendedPanIdPresent : 1;    ///< TRUE if Extended PAN ID is present, FALSE otherwise.
    bool mIsMeshLocalPrefixPresent : 1;  ///< TRUE if Mesh Local Prefix is present, FALSE otherwise.
    bool mIsDelayPresent : 1;            ///< TRUE if Delay Timer is present, FALSE otherwise.
    bool mIsPanIdPresent : 1;            ///< TRUE if PAN ID is present, FALSE otherwise.
    bool mIsChannelPresent : 1;          ///< TRUE if Channel is present, FALSE otherwise.
    bool mIsPskcPresent : 1;             ///< TRUE if PSKc is present, FALSE otherwise.
    bool mIsSecurityPolicyPresent : 1;   ///< TRUE if Security Policy is present, FALSE otherwise.
    bool mIsChannelMaskPresent : 1;      ///< TRUE if Channel Mask is present, FALSE otherwise.
} otOperationalDatasetComponents;

/**
 * This structure represents an Active or Pending Operational Dataset.
 *
 * Components in Dataset are optional. `mComponets` structure specifies which components are present in the Dataset.
 *
 */
typedef struct otOperationalDataset
{
    uint64_t                       mActiveTimestamp;  ///< Active Timestamp
    uint64_t                       mPendingTimestamp; ///< Pending Timestamp
    otNetworkKey                   mNetworkKey;       ///< Network Key
    otNetworkName                  mNetworkName;      ///< Network Name
    otExtendedPanId                mExtendedPanId;    ///< Extended PAN ID
    otMeshLocalPrefix              mMeshLocalPrefix;  ///< Mesh Local Prefix
    uint32_t                       mDelay;            ///< Delay Timer
    otPanId                        mPanId;            ///< PAN ID
    uint16_t                       mChannel;          ///< Channel
    otPskc                         mPskc;             ///< PSKc
    otSecurityPolicy               mSecurityPolicy;   ///< Security Policy
    otChannelMask                  mChannelMask;      ///< Channel Mask
    otOperationalDatasetComponents mComponents;       ///< Specifies which components are set in the Dataset.
} otOperationalDataset;

/**
 * Maximum length of Operational Dataset in bytes.
 *
 */
#define OT_OPERATIONAL_DATASET_MAX_LENGTH 254

/**
 * This structure represents an Active or Pending Operational Dataset.
 *
 * The Operational Dataset is TLV encoded as specified by Thread.
 *
 */
typedef struct otOperationalDatasetTlvs
{
    uint8_t mTlvs[OT_OPERATIONAL_DATASET_MAX_LENGTH]; ///< Operational Dataset TLVs.
    uint8_t mLength;                                  ///< Size of Operational Dataset in bytes.
} otOperationalDatasetTlvs;

/**
 * This enumeration represents meshcop TLV types.
 *
 */
typedef enum otMeshcopTlvType
{
    OT_MESHCOP_TLV_CHANNEL                  = 0,   ///< meshcop Channel TLV
    OT_MESHCOP_TLV_PANID                    = 1,   ///< meshcop Pan Id TLV
    OT_MESHCOP_TLV_EXTPANID                 = 2,   ///< meshcop Extended Pan Id TLV
    OT_MESHCOP_TLV_NETWORKNAME              = 3,   ///< meshcop Network Name TLV
    OT_MESHCOP_TLV_PSKC                     = 4,   ///< meshcop PSKc TLV
    OT_MESHCOP_TLV_NETWORKKEY               = 5,   ///< meshcop Network Key TLV
    OT_MESHCOP_TLV_NETWORK_KEY_SEQUENCE     = 6,   ///< meshcop Network Key Sequence TLV
    OT_MESHCOP_TLV_MESHLOCALPREFIX          = 7,   ///< meshcop Mesh Local Prefix TLV
    OT_MESHCOP_TLV_STEERING_DATA            = 8,   ///< meshcop Steering Data TLV
    OT_MESHCOP_TLV_BORDER_AGENT_RLOC        = 9,   ///< meshcop Border Agent Locator TLV
    OT_MESHCOP_TLV_COMMISSIONER_ID          = 10,  ///< meshcop Commissioner ID TLV
    OT_MESHCOP_TLV_COMM_SESSION_ID          = 11,  ///< meshcop Commissioner Session ID TLV
    OT_MESHCOP_TLV_SECURITYPOLICY           = 12,  ///< meshcop Security Policy TLV
    OT_MESHCOP_TLV_GET                      = 13,  ///< meshcop Get TLV
    OT_MESHCOP_TLV_ACTIVETIMESTAMP          = 14,  ///< meshcop Active Timestamp TLV
    OT_MESHCOP_TLV_COMMISSIONER_UDP_PORT    = 15,  ///< meshcop Commissioner UDP Port TLV
    OT_MESHCOP_TLV_STATE                    = 16,  ///< meshcop State TLV
    OT_MESHCOP_TLV_JOINER_DTLS              = 17,  ///< meshcop Joiner DTLS Encapsulation TLV
    OT_MESHCOP_TLV_JOINER_UDP_PORT          = 18,  ///< meshcop Joiner UDP Port TLV
    OT_MESHCOP_TLV_JOINER_IID               = 19,  ///< meshcop Joiner IID TLV
    OT_MESHCOP_TLV_JOINER_RLOC              = 20,  ///< meshcop Joiner Router Locator TLV
    OT_MESHCOP_TLV_JOINER_ROUTER_KEK        = 21,  ///< meshcop Joiner Router KEK TLV
    OT_MESHCOP_TLV_PROVISIONING_URL         = 32,  ///< meshcop Provisioning URL TLV
    OT_MESHCOP_TLV_VENDOR_NAME_TLV          = 33,  ///< meshcop Vendor Name TLV
    OT_MESHCOP_TLV_VENDOR_MODEL_TLV         = 34,  ///< meshcop Vendor Model TLV
    OT_MESHCOP_TLV_VENDOR_SW_VERSION_TLV    = 35,  ///< meshcop Vendor SW Version TLV
    OT_MESHCOP_TLV_VENDOR_DATA_TLV          = 36,  ///< meshcop Vendor Data TLV
    OT_MESHCOP_TLV_VENDOR_STACK_VERSION_TLV = 37,  ///< meshcop Vendor Stack Version TLV
    OT_MESHCOP_TLV_UDP_ENCAPSULATION_TLV    = 48,  ///< meshcop UDP encapsulation TLV
    OT_MESHCOP_TLV_IPV6_ADDRESS_TLV         = 49,  ///< meshcop IPv6 address TLV
    OT_MESHCOP_TLV_PENDINGTIMESTAMP         = 51,  ///< meshcop Pending Timestamp TLV
    OT_MESHCOP_TLV_DELAYTIMER               = 52,  ///< meshcop Delay Timer TLV
    OT_MESHCOP_TLV_CHANNELMASK              = 53,  ///< meshcop Channel Mask TLV
    OT_MESHCOP_TLV_COUNT                    = 54,  ///< meshcop Count TLV
    OT_MESHCOP_TLV_PERIOD                   = 55,  ///< meshcop Period TLV
    OT_MESHCOP_TLV_SCAN_DURATION            = 56,  ///< meshcop Scan Duration TLV
    OT_MESHCOP_TLV_ENERGY_LIST              = 57,  ///< meshcop Energy List TLV
    OT_MESHCOP_TLV_DISCOVERYREQUEST         = 128, ///< meshcop Discovery Request TLV
    OT_MESHCOP_TLV_DISCOVERYRESPONSE        = 129, ///< meshcop Discovery Response TLV
    OT_MESHCOP_TLV_JOINERADVERTISEMENT      = 241, ///< meshcop Joiner Advertisement TLV
} otMeshcopTlvType;

/**
 * This function pointer is called when a response to a MGMT_SET request is received or times out.
 *
 * @param[in]  aResult   A result of the operation.
 * @param[in]  aContext  A pointer to application-specific context.
 *
 * @retval  OT_ERROR_NONE              The request was accepted by the leader.
 * @retval  OT_ERROR_REJECTED          The request was rejected by the leader.
 * @retval  OT_ERROR_PARSE             An error occurred during parsing the response.
 * @retval  OT_ERROR_ABORT             The request was reset by peer.
 * @retval  OT_ERROR_RESPONSE_TIMEOUT  No response or acknowledgment received during timeout period.
 *
 */
typedef void (*otDatasetMgmtSetCallback)(otError aResult, void *aContext);

/**
 * This function indicates whether a valid network is present in the Active Operational Dataset or not.
 *
 * @param[in]  aInstance A pointer to an OpenThread instance.
 *
 * @returns TRUE if a valid network is present in the Active Operational Dataset, FALSE otherwise.
 *
 */
bool otDatasetIsCommissioned(otInstance *aInstance);

/**
 * This function gets the Active Operational Dataset.
 *
 * @param[in]   aInstance A pointer to an OpenThread instance.
 * @param[out]  aDataset  A pointer to where the Active Operational Dataset will be placed.
 *
 * @retval OT_ERROR_NONE          Successfully retrieved the Active Operational Dataset.
 * @retval OT_ERROR_NOT_FOUND     No corresponding value in the setting store.
 *
 */
otError otDatasetGetActive(otInstance *aInstance, otOperationalDataset *aDataset);

/**
 * This function gets the Active Operational Dataset.
 *
 * @param[in]   aInstance A pointer to an OpenThread instance.
 * @param[out]  aDataset  A pointer to where the Active Operational Dataset will be placed.
 *
 * @retval OT_ERROR_NONE          Successfully retrieved the Active Operational Dataset.
 * @retval OT_ERROR_NOT_FOUND     No corresponding value in the setting store.
 *
 */
otError otDatasetGetActiveTlvs(otInstance *aInstance, otOperationalDatasetTlvs *aDataset);

/**
 * This function sets the Active Operational Dataset.
 *
 * If the dataset does not include an Active Timestamp, the dataset is only partially complete.
 *
 * If Thread is enabled on a device that has a partially complete Active Dataset, the device will attempt to attach to
 * an existing Thread network using any existing information in the dataset. Only the Thread Network Key is needed to
 * attach to a network.
 *
 * If channel is not included in the dataset, the device will send MLE Announce messages across different channels to
 * find neighbors on other channels.
 *
 * If the device successfully attaches to a Thread network, the device will then retrieve the full Active Dataset from
 * its Parent. Note that a router-capable device will not transition to the Router or Leader roles until it has a
 * complete Active Dataset.
 *
 * @param[in]  aInstance A pointer to an OpenThread instance.
 * @param[in]  aDataset  A pointer to the Active Operational Dataset.
 *
 * @retval OT_ERROR_NONE             Successfully set the Active Operational Dataset.
 * @retval OT_ERROR_NO_BUFS          Insufficient buffer space to set the Active Operational Dataset.
 * @retval OT_ERROR_NOT_IMPLEMENTED  The platform does not implement settings functionality.
 *
 */
otError otDatasetSetActive(otInstance *aInstance, const otOperationalDataset *aDataset);

/**
 * This function sets the Active Operational Dataset.
 *
 * If the dataset does not include an Active Timestamp, the dataset is only partially complete.
 *
 * If Thread is enabled on a device that has a partially complete Active Dataset, the device will attempt to attach to
 * an existing Thread network using any existing information in the dataset. Only the Thread Network Key is needed to
 * attach to a network.
 *
 * If channel is not included in the dataset, the device will send MLE Announce messages across different channels to
 * find neighbors on other channels.
 *
 * If the device successfully attaches to a Thread network, the device will then retrieve the full Active Dataset from
 * its Parent. Note that a router-capable device will not transition to the Router or Leader roles until it has a
 * complete Active Dataset.
 *
 * @param[in]  aInstance A pointer to an OpenThread instance.
 * @param[in]  aDataset  A pointer to the Active Operational Dataset.
 *
 * @retval OT_ERROR_NONE             Successfully set the Active Operational Dataset.
 * @retval OT_ERROR_NO_BUFS          Insufficient buffer space to set the Active Operational Dataset.
 * @retval OT_ERROR_NOT_IMPLEMENTED  The platform does not implement settings functionality.
 *
 */
otError otDatasetSetActiveTlvs(otInstance *aInstance, const otOperationalDatasetTlvs *aDataset);

/**
 * This function gets the Pending Operational Dataset.
 *
 * @param[in]   aInstance A pointer to an OpenThread instance.
 * @param[out]  aDataset  A pointer to where the Pending Operational Dataset will be placed.
 *
 * @retval OT_ERROR_NONE          Successfully retrieved the Pending Operational Dataset.
 * @retval OT_ERROR_NOT_FOUND     No corresponding value in the setting store.
 *
 */
otError otDatasetGetPending(otInstance *aInstance, otOperationalDataset *aDataset);

/**
 * This function gets the Pending Operational Dataset.
 *
 * @param[in]   aInstance A pointer to an OpenThread instance.
 * @param[out]  aDataset  A pointer to where the Pending Operational Dataset will be placed.
 *
 * @retval OT_ERROR_NONE          Successfully retrieved the Pending Operational Dataset.
 * @retval OT_ERROR_NOT_FOUND     No corresponding value in the setting store.
 *
 */
otError otDatasetGetPendingTlvs(otInstance *aInstance, otOperationalDatasetTlvs *aDataset);

/**
 * This function sets the Pending Operational Dataset.
 *
 * @param[in]  aInstance A pointer to an OpenThread instance.
 * @param[in]  aDataset  A pointer to the Pending Operational Dataset.
 *
 * @retval OT_ERROR_NONE             Successfully set the Pending Operational Dataset.
 * @retval OT_ERROR_NO_BUFS          Insufficient buffer space to set the Pending Operational Dataset.
 * @retval OT_ERROR_NOT_IMPLEMENTED  The platform does not implement settings functionality.
 *
 */
otError otDatasetSetPending(otInstance *aInstance, const otOperationalDataset *aDataset);

/**
 * This function sets the Pending Operational Dataset.
 *
 * @param[in]  aInstance A pointer to an OpenThread instance.
 * @param[in]  aDataset  A pointer to the Pending Operational Dataset.
 *
 * @retval OT_ERROR_NONE             Successfully set the Pending Operational Dataset.
 * @retval OT_ERROR_NO_BUFS          Insufficient buffer space to set the Pending Operational Dataset.
 * @retval OT_ERROR_NOT_IMPLEMENTED  The platform does not implement settings functionality.
 *
 */
otError otDatasetSetPendingTlvs(otInstance *aInstance, const otOperationalDatasetTlvs *aDataset);

/**
 * This function sends MGMT_ACTIVE_GET.
 *
 * @param[in]  aInstance           A pointer to an OpenThread instance.
 * @param[in]  aDatasetComponents  A pointer to a Dataset Components structure specifying which components to request.
 * @param[in]  aTlvTypes           A pointer to array containing additional raw TLV types to be requested.
 * @param[in]  aLength             The length of @p aTlvTypes.
 * @param[in]  aAddress            A pointer to the IPv6 destination, if it is NULL, will use Leader ALOC as default.
 *
 * @retval OT_ERROR_NONE          Successfully send the meshcop dataset command.
 * @retval OT_ERROR_NO_BUFS       Insufficient buffer space to send.
 *
 */
otError otDatasetSendMgmtActiveGet(otInstance *                          aInstance,
                                   const otOperationalDatasetComponents *aDatasetComponents,
                                   const uint8_t *                       aTlvTypes,
                                   uint8_t                               aLength,
                                   const otIp6Address *                  aAddress);

/**
 * This function sends MGMT_ACTIVE_SET.
 *
 * @param[in]  aInstance  A pointer to an OpenThread instance.
 * @param[in]  aDataset   A pointer to operational dataset.
 * @param[in]  aTlvs      A pointer to TLVs.
 * @param[in]  aLength    The length of TLVs.
 * @param[in]  aCallback  A pointer to a function that is called on response reception or timeout.
 * @param[in]  aContext   A pointer to application-specific context for @p aCallback.
 *
 * @retval OT_ERROR_NONE          Successfully send the meshcop dataset command.
 * @retval OT_ERROR_NO_BUFS       Insufficient buffer space to send.
 * @retval OT_ERROR_BUSY          A previous request is ongoing.
 *
 */
otError otDatasetSendMgmtActiveSet(otInstance *                aInstance,
                                   const otOperationalDataset *aDataset,
                                   const uint8_t *             aTlvs,
                                   uint8_t                     aLength,
                                   otDatasetMgmtSetCallback    aCallback,
                                   void *                      aContext);

/**
 * This function sends MGMT_PENDING_GET.
 *
 * @param[in]  aInstance           A pointer to an OpenThread instance.
 * @param[in]  aDatasetComponents  A pointer to a Dataset Components structure specifying which components to request.
 * @param[in]  aTlvTypes           A pointer to array containing additional raw TLV types to be requested.
 * @param[in]  aLength             The length of @p aTlvTypes.
 * @param[in]  aAddress            A pointer to the IPv6 destination, if it is NULL, will use Leader ALOC as default.
 *
 * @retval OT_ERROR_NONE          Successfully send the meshcop dataset command.
 * @retval OT_ERROR_NO_BUFS       Insufficient buffer space to send.
 *
 */
otError otDatasetSendMgmtPendingGet(otInstance *                          aInstance,
                                    const otOperationalDatasetComponents *aDatasetComponents,
                                    const uint8_t *                       aTlvTypes,
                                    uint8_t                               aLength,
                                    const otIp6Address *                  aAddress);

/**
 * This function sends MGMT_PENDING_SET.
 *
 * @param[in]  aInstance  A pointer to an OpenThread instance.
 * @param[in]  aDataset   A pointer to operational dataset.
 * @param[in]  aTlvs      A pointer to TLVs.
 * @param[in]  aLength    The length of TLVs.
 * @param[in]  aCallback  A pointer to a function that is called on response reception or timeout.
 * @param[in]  aContext   A pointer to application-specific context for @p aCallback.
 *
 * @retval OT_ERROR_NONE          Successfully send the meshcop dataset command.
 * @retval OT_ERROR_NO_BUFS       Insufficient buffer space to send.
 * @retval OT_ERROR_BUSY          A previous request is ongoing.
 *
 */
otError otDatasetSendMgmtPendingSet(otInstance *                aInstance,
                                    const otOperationalDataset *aDataset,
                                    const uint8_t *             aTlvs,
                                    uint8_t                     aLength,
                                    otDatasetMgmtSetCallback    aCallback,
                                    void *                      aContext);

/**
 * This function generates PSKc from a given pass-phrase, network name, and extended PAN ID.
 *
 * PSKc is used to establish the Commissioner Session.
 *
 * @param[in]  aPassPhrase   The commissioning pass-phrase.
 * @param[in]  aNetworkName  The network name for PSKc computation.
 * @param[in]  aExtPanId     The extended PAN ID for PSKc computation.
 * @param[out] aPskc         A pointer to variable to output the generated PSKc.
 *
 * @retval OT_ERROR_NONE          Successfully generate PSKc.
 * @retval OT_ERROR_INVALID_ARGS  If any of the input arguments is invalid.
 *
 */
otError otDatasetGeneratePskc(const char *           aPassPhrase,
                              const otNetworkName *  aNetworkName,
                              const otExtendedPanId *aExtPanId,
                              otPskc *               aPskc);

/**
 * This function sets an `otNetworkName` instance from a given null terminated C string.
 *
 * This function also validates that the given @p aNameString follows UTF-8 encoding and its length is not longer than
 * `OT_NETWORK_NAME_MAX_SIZE`.
 *
 * @param[out] aNetworkName        A pointer to the `otNetworkName` to set.
 * @param[in]  aNameString         A name C string.
 *
 * @retval OT_ERROR_NONE           Successfully set @p aNetworkName from @p aNameString.
 * @retval OT_ERROR_INVALID_ARGS   @p aNameStrng is invalid (too long or does not follow UTF-8 encoding).
 *
 */
otError otNetworkNameFromString(otNetworkName *aNetworkName, const char *aNameString);

/**
 * This function parses an Operational Dataset from a `otOperationalDatasetTlvs`.
 *
 * @param[in]  aDatasetTlvs  A pointer to dataset TLVs.
 * @param[out] aDataset      A pointer to where the dataset will be placed.
 *
 * @retval OT_ERROR_NONE          Successfully set @p aDataset from @p aDatasetTlvs.
 * @retval OT_ERROR_INVALID_ARGS  @p aDatasetTlvs is invalid.
 *
 */
otError otDatasetParseTlvs(const otOperationalDatasetTlvs *aDatasetTlvs, otOperationalDataset *aDataset);

/**
 * @}
 *
 */

#ifdef __cplusplus
} // extern "C"
#endif

#endif // OPENTHREAD_DATASET_H_
