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
 *   This file includes definitions for non-volatile storage of settings.
 */

#ifndef SETTINGS_HPP_
#define SETTINGS_HPP_

#include "openthread-core-config.h"

#include <openthread/platform/settings.h>

#include "common/clearable.hpp"
#include "common/encoding.hpp"
#include "common/equatable.hpp"
#include "common/locator.hpp"
#include "common/non_copyable.hpp"
#include "common/settings_driver.hpp"
#include "mac/mac_types.hpp"
#include "net/ip6_address.hpp"
#include "utils/flash.hpp"
#if OPENTHREAD_CONFIG_IP6_SLAAC_ENABLE
#include "utils/slaac_address.hpp"
#endif
#if OPENTHREAD_CONFIG_SRP_CLIENT_ENABLE
#include "crypto/ecdsa.hpp"
#endif

namespace ot {

namespace MeshCoP {
class Dataset;
}

class Settings;

/**
 * This class defines the base class used by `Settings` and `Settings::ChildInfoIterator`.
 *
 * This class provides structure definitions for different settings keys.
 *
 */
class SettingsBase : public InstanceLocator
{
protected:
    enum Action : uint8_t
    {
        kActionRead,
        kActionSave,
        kActionResave,
        kActionDelete,
#if OPENTHREAD_FTD
        kActionAdd,
        kActionRemove,
        kActionDeleteAll,
#endif
    };

public:
    /**
     * Rules for updating existing value structures.
     *
     * 1. Modifying existing key value fields in settings MUST only be
     *    done by appending new fields.  Existing fields MUST NOT be
     *    deleted or modified in any way.
     *
     * 2. To support backward compatibility (rolling back to an older
     *    software version), code reading and processing key values MUST
     *    process key values that have longer length.  Additionally, newer
     *    versions MUST update/maintain values in existing key value
     *    fields.
     *
     * 3. To support forward compatibility (rolling forward to a newer
     *    software version), code reading and processing key values MUST
     *    process key values that have shorter length.
     *
     * 4. New Key IDs may be defined in the future with the understanding
     *    that such key values are not backward compatible.
     *
     */

    /**
     * This enumeration defines the keys of settings.
     *
     */
    enum Key : uint16_t
    {
        kKeyActiveDataset     = OT_SETTINGS_KEY_ACTIVE_DATASET,
        kKeyPendingDataset    = OT_SETTINGS_KEY_PENDING_DATASET,
        kKeyNetworkInfo       = OT_SETTINGS_KEY_NETWORK_INFO,
        kKeyParentInfo        = OT_SETTINGS_KEY_PARENT_INFO,
        kKeyChildInfo         = OT_SETTINGS_KEY_CHILD_INFO,
        kKeyReserved          = OT_SETTINGS_KEY_RESERVED,
        kKeySlaacIidSecretKey = OT_SETTINGS_KEY_SLAAC_IID_SECRET_KEY,
        kKeyDadInfo           = OT_SETTINGS_KEY_DAD_INFO,
        kKeyOmrPrefix         = OT_SETTINGS_KEY_OMR_PREFIX,
        kKeyOnLinkPrefix      = OT_SETTINGS_KEY_ON_LINK_PREFIX,
        kKeySrpEcdsaKey       = OT_SETTINGS_KEY_SRP_ECDSA_KEY,
        kKeySrpClientInfo     = OT_SETTINGS_KEY_SRP_CLIENT_INFO,
        kKeySrpServerInfo     = OT_SETTINGS_KEY_SRP_SERVER_INFO,
    };

    static constexpr Key kLastKey = kKeySrpServerInfo; ///< The last (numerically) enumerator value in `Key`.

    /**
     * This structure represents the device's own network information for settings storage.
     *
     */
    OT_TOOL_PACKED_BEGIN
    class NetworkInfo : private Clearable<NetworkInfo>
    {
        friend class Settings;

    public:
        static constexpr Key kKey = kKeyNetworkInfo; ///< The associated key.

        /**
         * This method initializes the `NetworkInfo` object.
         *
         */
        void Init(void)
        {
            Clear();
            SetVersion(OT_THREAD_VERSION_1_1);
        }

        /**
         * This method returns the Thread role.
         *
         * @returns The Thread role.
         *
         */
        uint8_t GetRole(void) const { return mRole; }

        /**
         * This method sets the Thread role.
         *
         * @param[in] aRole  The Thread Role.
         *
         */
        void SetRole(uint8_t aRole) { mRole = aRole; }

        /**
         * This method returns the Thread device mode.
         *
         * @returns the Thread device mode.
         *
         */
        uint8_t GetDeviceMode(void) const { return mDeviceMode; }

        /**
         * This method sets the Thread device mode.
         *
         * @param[in] aDeviceMode  The Thread device mode.
         *
         */
        void SetDeviceMode(uint8_t aDeviceMode) { mDeviceMode = aDeviceMode; }

        /**
         * This method returns the RLOC16.
         *
         * @returns The RLOC16.
         *
         */
        uint16_t GetRloc16(void) const { return Encoding::LittleEndian::HostSwap16(mRloc16); }

        /**
         * This method sets the RLOC16.
         *
         * @param[in] aRloc16  The RLOC16.
         *
         */
        void SetRloc16(uint16_t aRloc16) { mRloc16 = Encoding::LittleEndian::HostSwap16(aRloc16); }

        /**
         * This method returns the key sequence.
         *
         * @returns The key sequence.
         *
         */
        uint32_t GetKeySequence(void) const { return Encoding::LittleEndian::HostSwap32(mKeySequence); }

        /**
         * This method sets the key sequence.
         *
         * @param[in] aKeySequence  The key sequence.
         *
         */
        void SetKeySequence(uint32_t aKeySequence) { mKeySequence = Encoding::LittleEndian::HostSwap32(aKeySequence); }

        /**
         * This method returns the MLE frame counter.
         *
         * @returns The MLE frame counter.
         *
         */
        uint32_t GetMleFrameCounter(void) const { return Encoding::LittleEndian::HostSwap32(mMleFrameCounter); }

        /**
         * This method sets the MLE frame counter.
         *
         * @param[in] aMleFrameCounter  The MLE frame counter.
         *
         */
        void SetMleFrameCounter(uint32_t aMleFrameCounter)
        {
            mMleFrameCounter = Encoding::LittleEndian::HostSwap32(aMleFrameCounter);
        }

        /**
         * This method returns the MAC frame counter.
         *
         * @returns The MAC frame counter.
         *
         */
        uint32_t GetMacFrameCounter(void) const { return Encoding::LittleEndian::HostSwap32(mMacFrameCounter); }

        /**
         * This method sets the MAC frame counter.
         *
         * @param[in] aMacFrameCounter  The MAC frame counter.
         *
         */
        void SetMacFrameCounter(uint32_t aMacFrameCounter)
        {
            mMacFrameCounter = Encoding::LittleEndian::HostSwap32(aMacFrameCounter);
        }

        /**
         * This method returns the previous partition ID.
         *
         * @returns The previous partition ID.
         *
         */
        uint32_t GetPreviousPartitionId(void) const { return Encoding::LittleEndian::HostSwap32(mPreviousPartitionId); }

        /**
         * This method sets the previous partition id.
         *
         * @param[in] aPreviousPartitionId  The previous partition ID.
         *
         */
        void SetPreviousPartitionId(uint32_t aPreviousPartitionId)
        {
            mPreviousPartitionId = Encoding::LittleEndian::HostSwap32(aPreviousPartitionId);
        }

        /**
         * This method returns the extended address.
         *
         * @returns The extended address.
         *
         */
        const Mac::ExtAddress &GetExtAddress(void) const { return mExtAddress; }

        /**
         * This method sets the extended address.
         *
         * @param[in] aExtAddress  The extended address.
         *
         */
        void SetExtAddress(const Mac::ExtAddress &aExtAddress) { mExtAddress = aExtAddress; }

        /**
         * This method returns the Mesh Local Interface Identifier.
         *
         * @returns The Mesh Local Interface Identifier.
         *
         */
        const Ip6::InterfaceIdentifier &GetMeshLocalIid(void) const { return mMlIid; }

        /**
         * This method sets the Mesh Local Interface Identifier.
         *
         * @param[in] aMeshLocalIid  The Mesh Local Interface Identifier.
         *
         */
        void SetMeshLocalIid(const Ip6::InterfaceIdentifier &aMeshLocalIid) { mMlIid = aMeshLocalIid; }

        /**
         * This method returns the Thread version.
         *
         * @returns The Thread version.
         *
         */
        uint16_t GetVersion(void) const { return Encoding::LittleEndian::HostSwap16(mVersion); }

        /**
         * This method sets the Thread version.
         *
         * @param[in] aVersion  The Thread version.
         *
         */
        void SetVersion(uint16_t aVersion) { mVersion = Encoding::LittleEndian::HostSwap16(aVersion); }

    private:
        void Log(Action aAction) const;

        uint8_t                  mRole;                ///< Current Thread role.
        uint8_t                  mDeviceMode;          ///< Device mode setting.
        uint16_t                 mRloc16;              ///< RLOC16
        uint32_t                 mKeySequence;         ///< Key Sequence
        uint32_t                 mMleFrameCounter;     ///< MLE Frame Counter
        uint32_t                 mMacFrameCounter;     ///< MAC Frame Counter
        uint32_t                 mPreviousPartitionId; ///< PartitionId
        Mac::ExtAddress          mExtAddress;          ///< Extended Address
        Ip6::InterfaceIdentifier mMlIid;               ///< IID from ML-EID
        uint16_t                 mVersion;             ///< Version
    } OT_TOOL_PACKED_END;

    /**
     * This structure represents the parent information for settings storage.
     *
     */
    OT_TOOL_PACKED_BEGIN
    class ParentInfo : private Clearable<ParentInfo>
    {
        friend class Settings;

    public:
        static constexpr Key kKey = kKeyParentInfo; ///< The associated key.

        /**
         * This method initializes the `ParentInfo` object.
         *
         */
        void Init(void)
        {
            Clear();
            SetVersion(OT_THREAD_VERSION_1_1);
        }

        /**
         * This method returns the extended address.
         *
         * @returns The extended address.
         *
         */
        const Mac::ExtAddress &GetExtAddress(void) const { return mExtAddress; }

        /**
         * This method sets the extended address.
         *
         * @param[in] aExtAddress  The extended address.
         *
         */
        void SetExtAddress(const Mac::ExtAddress &aExtAddress) { mExtAddress = aExtAddress; }

        /**
         * This method returns the Thread version.
         *
         * @returns The Thread version.
         *
         */
        uint16_t GetVersion(void) const { return Encoding::LittleEndian::HostSwap16(mVersion); }

        /**
         * This method sets the Thread version.
         *
         * @param[in] aVersion  The Thread version.
         *
         */
        void SetVersion(uint16_t aVersion) { mVersion = Encoding::LittleEndian::HostSwap16(aVersion); }

    private:
        void Log(Action aAction) const;

        Mac::ExtAddress mExtAddress; ///< Extended Address
        uint16_t        mVersion;    ///< Version
    } OT_TOOL_PACKED_END;

#if OPENTHREAD_FTD
    /**
     * This structure represents the child information for settings storage.
     *
     */
    OT_TOOL_PACKED_BEGIN
    class ChildInfo
    {
        friend class Settings;

    public:
        static constexpr Key kKey = kKeyChildInfo; ///< The associated key.

        /**
         * This method clears the struct object (setting all the fields to zero).
         *
         */
        void Init(void)
        {
            memset(this, 0, sizeof(*this));
            SetVersion(OT_THREAD_VERSION_1_1);
        }

        /**
         * This method returns the extended address.
         *
         * @returns The extended address.
         *
         */
        const Mac::ExtAddress &GetExtAddress(void) const { return mExtAddress; }

        /**
         * This method sets the extended address.
         *
         * @param[in] aExtAddress  The extended address.
         *
         */
        void SetExtAddress(const Mac::ExtAddress &aExtAddress) { mExtAddress = aExtAddress; }

        /**
         * This method returns the child timeout.
         *
         * @returns The child timeout.
         *
         */
        uint32_t GetTimeout(void) const { return Encoding::LittleEndian::HostSwap32(mTimeout); }

        /**
         * This method sets the child timeout.
         *
         * @param[in] aTimeout  The child timeout.
         *
         */
        void SetTimeout(uint32_t aTimeout) { mTimeout = Encoding::LittleEndian::HostSwap32(aTimeout); }

        /**
         * This method returns the RLOC16.
         *
         * @returns The RLOC16.
         *
         */
        uint16_t GetRloc16(void) const { return Encoding::LittleEndian::HostSwap16(mRloc16); }

        /**
         * This method sets the RLOC16.
         *
         * @param[in] aRloc16  The RLOC16.
         *
         */
        void SetRloc16(uint16_t aRloc16) { mRloc16 = Encoding::LittleEndian::HostSwap16(aRloc16); }

        /**
         * This method returns the Thread device mode.
         *
         * @returns The Thread device mode.
         *
         */
        uint8_t GetMode(void) const { return mMode; }

        /**
         * This method sets the Thread device mode.
         *
         * @param[in] aRloc16  The Thread device mode.
         *
         */
        void SetMode(uint8_t aMode) { mMode = aMode; }

        /**
         * This method returns the Thread version.
         *
         * @returns The Thread version.
         *
         */
        uint16_t GetVersion(void) const { return Encoding::LittleEndian::HostSwap16(mVersion); }

        /**
         * This method sets the Thread version.
         *
         * @param[in] aVersion  The Thread version.
         *
         */
        void SetVersion(uint16_t aVersion) { mVersion = Encoding::LittleEndian::HostSwap16(aVersion); }

    private:
        void Log(Action aAction) const;

        Mac::ExtAddress mExtAddress; ///< Extended Address
        uint32_t        mTimeout;    ///< Timeout
        uint16_t        mRloc16;     ///< RLOC16
        uint8_t         mMode;       ///< The MLE device mode
        uint16_t        mVersion;    ///< Version
    } OT_TOOL_PACKED_END;
#endif // OPENTHREAD_FTD

#if OPENTHREAD_CONFIG_IP6_SLAAC_ENABLE
    /**
     * This class defines constants and types for SLAAC IID Secret key settings.
     *
     */
    class SlaacIidSecretKey
    {
    public:
        static constexpr Key kKey = kKeySlaacIidSecretKey; ///< The associated key.

        typedef Utils::Slaac::IidSecretKey ValueType; ///< The associated value type.

    private:
        SlaacIidSecretKey(void) = default;
    };
#endif

#if OPENTHREAD_CONFIG_DUA_ENABLE
    /**
     * This structure represents the duplicate address detection information for settings storage.
     *
     */
    OT_TOOL_PACKED_BEGIN
    class DadInfo : private Clearable<DadInfo>
    {
        friend class Settings;

    public:
        static constexpr Key kKey = kKeyDadInfo; ///< The associated key.

        /**
         * This method initializes the `DadInfo` object.
         *
         */
        void Init(void) { Clear(); }

        /**
         * This method returns the Dad Counter.
         *
         * @returns The Dad Counter value.
         *
         */
        uint8_t GetDadCounter(void) const { return mDadCounter; }

        /**
         * This method sets the Dad Counter.
         *
         * @param[in] aDadCounter The Dad Counter value.
         *
         */
        void SetDadCounter(uint8_t aDadCounter) { mDadCounter = aDadCounter; }

    private:
        void Log(Action aAction) const;

        uint8_t mDadCounter; ///< Dad Counter used to resolve address conflict in Thread 1.2 DUA feature.
    } OT_TOOL_PACKED_END;
#endif // OPENTHREAD_CONFIG_DUA_ENABLE

#if OPENTHREAD_CONFIG_BORDER_ROUTING_ENABLE
    /**
     * This class defines constants and types for OMR prefix settings.
     *
     */
    class OmrPrefix
    {
    public:
        static constexpr Key kKey = kKeyOmrPrefix; ///< The associated key.

        typedef Ip6::Prefix ValueType; ///< The associated value type.

    private:
        OmrPrefix(void) = default;
    };

    /**
     * This class defines constants and types for on-link prefix settings.
     *
     */
    class OnLinkPrefix
    {
    public:
        static constexpr Key kKey = kKeyOnLinkPrefix; ///< The associated key.

        typedef Ip6::Prefix ValueType; ///< The associated value type.

    private:
        OnLinkPrefix(void) = default;
    };
#endif // OPENTHREAD_CONFIG_BORDER_ROUTING_ENABLE

#if OPENTHREAD_CONFIG_SRP_CLIENT_ENABLE
    /**
     * This class defines constants and types for SRP ECDSA key settings.
     *
     */
    class SrpEcdsaKey
    {
    public:
        static constexpr Key kKey = kKeySrpEcdsaKey; ///< The associated key.

        typedef Crypto::Ecdsa::P256::KeyPair ValueType; ///< The associated value type.

    private:
        SrpEcdsaKey(void) = default;
    };

#if OPENTHREAD_CONFIG_SRP_CLIENT_SAVE_SELECTED_SERVER_ENABLE
    /**
     * This structure represents the SRP client info (selected server address).
     *
     */
    OT_TOOL_PACKED_BEGIN
    class SrpClientInfo : private Clearable<SrpClientInfo>
    {
        friend class Settings;

    public:
        static constexpr Key kKey = kKeySrpClientInfo; ///< The associated key.

        /**
         * This method initializes the `SrpClientInfo` object.
         *
         */
        void Init(void) { Clear(); }

        /**
         * This method returns the server IPv6 address.
         *
         * @returns The server IPv6 address.
         *
         */
        const Ip6::Address &GetServerAddress(void) const { return mServerAddress; }

        /**
         * This method sets the server IPv6 address.
         *
         * @param[in] aAddress  The server IPv6 address.
         *
         */
        void SetServerAddress(const Ip6::Address &aAddress) { mServerAddress = aAddress; }

        /**
         * This method returns the server port number.
         *
         * @returns The server port number.
         *
         */
        uint16_t GetServerPort(void) const { return Encoding::LittleEndian::HostSwap16(mServerPort); }

        /**
         * This method sets the server port number.
         *
         * @param[in] aPort  The server port number.
         *
         */
        void SetServerPort(uint16_t aPort) { mServerPort = Encoding::LittleEndian::HostSwap16(aPort); }

    private:
        void Log(Action aAction) const;

        Ip6::Address mServerAddress;
        uint16_t     mServerPort; // (in little-endian encoding)
    } OT_TOOL_PACKED_END;
#endif // OPENTHREAD_CONFIG_SRP_CLIENT_SAVE_SELECTED_SERVER_ENABLE
#endif // OPENTHREAD_CONFIG_SRP_CLIENT_ENABLE

#if OPENTHREAD_CONFIG_SRP_SERVER_ENABLE && OPENTHREAD_CONFIG_SRP_SERVER_PORT_SWITCH_ENABLE
    /**
     * This structure represents the SRP server info.
     *
     */
    OT_TOOL_PACKED_BEGIN
    class SrpServerInfo : private Clearable<SrpServerInfo>
    {
        friend class Settings;

    public:
        static constexpr Key kKey = kKeySrpServerInfo; ///< The associated key.

        /**
         * This method initializes the `SrpServerInfo` object.
         *
         */
        void Init(void) { Clear(); }

        /**
         * This method returns the server port number.
         *
         * @returns The server port number.
         *
         */
        uint16_t GetPort(void) const { return Encoding::LittleEndian::HostSwap16(mPort); }

        /**
         * This method sets the server port number.
         *
         * @param[in] aPort  The server port number.
         *
         */
        void SetPort(uint16_t aPort) { mPort = Encoding::LittleEndian::HostSwap16(aPort); }

    private:
        void Log(Action aAction) const;

        uint16_t mPort; // (in little-endian encoding)
    } OT_TOOL_PACKED_END;
#endif // OPENTHREAD_CONFIG_SRP_SERVER_ENABLE && OPENTHREAD_CONFIG_SRP_SERVER_PORT_SWITCH_ENABLE

protected:
    explicit SettingsBase(Instance &aInstance)
        : InstanceLocator(aInstance)
    {
    }

#if OPENTHREAD_CONFIG_BORDER_ROUTING_ENABLE
    static void LogPrefix(Action aAction, Key aKey, const Ip6::Prefix &aPrefix);
#endif

#if OPENTHREAD_CONFIG_LOG_UTIL && (OPENTHREAD_CONFIG_LOG_LEVEL >= OT_LOG_LEVEL_WARN)
    static const char *KeyToString(Key aKey);
#endif
#if OPENTHREAD_CONFIG_LOG_UTIL && (OPENTHREAD_CONFIG_LOG_LEVEL >= OT_LOG_LEVEL_INFO)
    static const char *ActionToString(Action aAction);
#endif
};

/**
 * This class defines methods related to non-volatile storage of settings.
 *
 */
class Settings : public SettingsBase, private NonCopyable
{
    class ChildInfoIteratorBuilder;

public:
    /**
     * This constructor initializes a `Settings` object.
     *
     * @param[in]  aInstance     A reference to the OpenThread instance.
     *
     */
    explicit Settings(Instance &aInstance)
        : SettingsBase(aInstance)
    {
    }

    /**
     * This method initializes the platform settings (non-volatile) module.
     *
     * This should be called before any other method from this class.
     *
     */
    void Init(void);

    /**
     * This method de-initializes the platform settings (non-volatile) module.
     *
     * This method should be called when OpenThread instance is no longer in use.
     *
     */
    void Deinit(void);

    /**
     * This method removes all settings from the non-volatile store.
     *
     */
    void Wipe(void);

    /**
     * This method saves the Operational Dataset (active or pending).
     *
     * @param[in]   aIsActive   Indicates whether Dataset is active or pending.
     * @param[in]   aDataset    A reference to a `Dataset` object to be saved.
     *
     * @retval kErrorNone             Successfully saved the Dataset.
     * @retval kErrorNotImplemented   The platform does not implement settings functionality.
     *
     */
    Error SaveOperationalDataset(bool aIsActive, const MeshCoP::Dataset &aDataset);

    /**
     * This method reads the Operational Dataset (active or pending).
     *
     * @param[in]   aIsActive             Indicates whether Dataset is active or pending.
     * @param[out]  aDataset              A reference to a `Dataset` object to output the read content.
     *
     * @retval kErrorNone             Successfully read the Dataset.
     * @retval kErrorNotFound         No corresponding value in the setting store.
     * @retval kErrorNotImplemented   The platform does not implement settings functionality.
     *
     */
    Error ReadOperationalDataset(bool aIsActive, MeshCoP::Dataset &aDataset) const;

    /**
     * This method deletes the Operational Dataset (active/pending) from settings.
     *
     * @param[in]   aIsActive            Indicates whether Dataset is active or pending.
     *
     * @retval kErrorNone            Successfully deleted the Dataset.
     * @retval kErrorNotImplemented  The platform does not implement settings functionality.
     *
     */
    Error DeleteOperationalDataset(bool aIsActive);

    /**
     * This template method reads a specified settings entry.
     *
     * The template type `EntryType` specifies the entry's value data structure. It must provide the following:
     *
     *  - It must provide a constant `EntryType::kKey` to specify the associated entry settings key.
     *  - It must provide method `Init()` to initialize the `aEntry` object.
     *
     * This version of `Read<EntryType>` is intended for use with entries that define a data structure which represents
     * the entry's value, e.g., `NetworkInfo`, `ParentInfo`, `DadInfo`, etc.
     *
     * @tparam EntryType              The settings entry type.
     *
     * @param[out] aEntry             A reference to a entry data structure to output the read content.
     *
     * @retval kErrorNone             Successfully read the entry.
     * @retval kErrorNotFound         No corresponding value in the setting store.
     * @retval kErrorNotImplemented   The platform does not implement settings functionality.
     *
     */
    template <typename EntryType> Error Read(EntryType &aEntry) const
    {
        aEntry.Init();

        return ReadEntry(EntryType::kKey, &aEntry, sizeof(EntryType));
    }

    /**
     * This template method reads a specified settings entry.
     *
     * The template type `EntryType` provides information about the entry's value type. It must provide the following:
     *
     *  - It must provide a constant `EntryType::kKey` to specify the associated entry settings key.
     *  - It must provide a nested type `EntryType::ValueType` to specify the associated entry value type.
     *
     * This version of `Read<EntryType>` is intended for use with entries that have a simple entry value type (which can
     * be represented by an existing type), e.g., `OmrPrefx` (using `Ip6::Prefix` as the value type).
     *
     * @tparam EntryType              The settings entry type.
     *
     * @param[out] aValue             A reference to a value type object to output the read content.
     *
     * @retval kErrorNone             Successfully read the value.
     * @retval kErrorNotFound         No corresponding value in the setting store.
     * @retval kErrorNotImplemented   The platform does not implement settings functionality.
     *
     */
    template <typename EntryType> Error Read(typename EntryType::ValueType &aValue) const
    {
        return ReadEntry(EntryType::kKey, &aValue, sizeof(typename EntryType::ValueType));
    }

    /**
     * This template method saves a specified settings entry.
     *
     * The template type `EntryType` specifies the entry's value data structure. It must provide the following:
     *
     *  - It must provide a constant `EntryType::kKey` to specify the associated entry settings key.
     *
     * This version of `Save<EntryType>` is intended for use with entries that define a data structure which represents
     * the entry's value, e.g., `NetworkInfo`, `ParentInfo`, `DadInfo`, etc.
     *
     * @tparam EntryType              The settings entry type.
     *
     * @param[in] aEntry              The entry value to be saved.
     *
     * @retval kErrorNone             Successfully saved Network Info in settings.
     * @retval kErrorNotImplemented   The platform does not implement settings functionality.
     *
     */
    template <typename EntryType> Error Save(const EntryType &aEntry)
    {
        EntryType prev;

        return SaveEntry(EntryType::kKey, &aEntry, &prev, sizeof(EntryType));
    }

    /**
     * This template method saves a specified settings entry.
     *
     * The template type `EntryType` provides information about the entry's value type. It must provide the following:
     *
     *  - It must provide a constant `EntryType::kKey` to specify the associated entry settings key.
     *  - It must provide a nested type `EntryType::ValueType` to specify the associated entry value type.
     *
     * This version of `Save<EntryType>` is intended for use with entries that have a simple entry value type (which can
     * be represented by an existing type), e.g., `OmrPrefx` (using `Ip6::Prefix` as the value type).
     *
     * @tparam EntryType              The settings entry type.
     *
     * @param[in] aEntry              The entry value to be saved.
     *
     * @retval kErrorNone             Successfully saved Network Info in settings.
     * @retval kErrorNotImplemented   The platform does not implement settings functionality.
     *
     */
    template <typename EntryType> Error Save(const typename EntryType::ValueType &aValue)
    {
        typename EntryType::ValueType prev;

        return SaveEntry(EntryType::kKey, &aValue, &prev, sizeof(typename EntryType::ValueType));
    }

    /**
     * This template method deletes a specified setting entry.
     *
     * The template type `EntryType` provides information about the entry's key.
     *
     *  - It must provide a constant `EntryType::kKey` to specify the associated entry settings key.
     *
     * @tparam EntryType             The settings entry type.
     *
     * @retval kErrorNone            Successfully deleted the value.
     * @retval kErrorNotImplemented  The platform does not implement settings functionality.
     *
     */
    template <typename EntryType> Error Delete(void) { return DeleteEntry(EntryType::kKey); }

#if OPENTHREAD_FTD
    /**
     * This method adds a Child Info entry to settings.
     *
     * @note Child Info is a list-based settings property and can contain multiple entries.
     *
     * @param[in]   aChildInfo            A reference to a `ChildInfo` structure to be saved/added.
     *
     * @retval kErrorNone             Successfully saved the Child Info in settings.
     * @retval kErrorNotImplemented   The platform does not implement settings functionality.
     *
     */
    Error AddChildInfo(const ChildInfo &aChildInfo);

    /**
     * This method deletes all Child Info entries from the settings.
     *
     * @note Child Info is a list-based settings property and can contain multiple entries.
     *
     * @retval kErrorNone            Successfully deleted the value.
     * @retval kErrorNotImplemented  The platform does not implement settings functionality.
     *
     */
    Error DeleteAllChildInfo(void);

    /**
     * This method enables range-based `for` loop iteration over all child info entries in the `Settings`.
     *
     * This method should be used as follows:
     *
     *     for (const ChildInfo &childInfo : Get<Settings>().IterateChildInfo()) { ... }
     *
     *
     * @returns A ChildInfoIteratorBuilder instance.
     *
     */
    ChildInfoIteratorBuilder IterateChildInfo(void) { return ChildInfoIteratorBuilder(GetInstance()); }

    /**
     * This class defines an iterator to access all Child Info entries in the settings.
     *
     */
    class ChildInfoIterator : public SettingsBase, public Unequatable<ChildInfoIterator>
    {
        friend class ChildInfoIteratorBuilder;

    public:
        /**
         * This constructor initializes a `ChildInfoInterator` object.
         *
         * @param[in]  aInstance  A reference to the OpenThread instance.
         *
         */
        explicit ChildInfoIterator(Instance &aInstance);

        /**
         * This method indicates whether there are no more Child Info entries in the list (iterator has reached end of
         * the list), or the current entry is valid.
         *
         * @retval TRUE   There are no more entries in the list (reached end of the list).
         * @retval FALSE  The current entry is valid.
         *
         */
        bool IsDone(void) const { return mIsDone; }

        /**
         * This method overloads operator `++` (pre-increment) to advance the iterator to move to the next Child Info
         * entry in the list (if any).
         *
         */
        void operator++(void) { Advance(); }

        /**
         * This method overloads operator `++` (post-increment) to advance the iterator to move to the next Child Info
         * entry in the list (if any).
         *
         */
        void operator++(int) { Advance(); }

        /**
         * This method gets the Child Info corresponding to the current iterator entry in the list.
         *
         * @note This method should be used only if `IsDone()` is returning FALSE indicating that the iterator is
         * pointing to a valid entry.
         *
         * @returns A reference to `ChildInfo` structure corresponding to current iterator entry.
         *
         */
        const ChildInfo &GetChildInfo(void) const { return mChildInfo; }

        /**
         * This method deletes the current Child Info entry.
         *
         * @retval kErrorNone            The entry was deleted successfully.
         * @retval kErrorInvalidState    The entry is not valid (iterator has reached end of list).
         * @retval kErrorNotImplemented  The platform does not implement settings functionality.
         *
         */
        Error Delete(void);

        /**
         * This method overloads the `*` dereference operator and gets a reference to `ChildInfo` entry to which the
         * iterator is currently pointing.
         *
         * @note This method should be used only if `IsDone()` is returning FALSE indicating that the iterator is
         * pointing to a valid entry.
         *
         *
         * @returns A reference to the `ChildInfo` entry currently pointed by the iterator.
         *
         */
        const ChildInfo &operator*(void)const { return mChildInfo; }

        /**
         * This method overloads operator `==` to evaluate whether or not two iterator instances are equal.
         *
         * @param[in]  aOther  The other iterator to compare with.
         *
         * @retval TRUE   If the two iterator objects are equal
         * @retval FALSE  If the two iterator objects are not equal.
         *
         */
        bool operator==(const ChildInfoIterator &aOther) const
        {
            return (mIsDone && aOther.mIsDone) || (!mIsDone && !aOther.mIsDone && (mIndex == aOther.mIndex));
        }

    private:
        enum IteratorType : uint8_t
        {
            kEndIterator,
        };

        ChildInfoIterator(Instance &aInstance, IteratorType)
            : SettingsBase(aInstance)
            , mIndex(0)
            , mIsDone(true)
        {
        }

        void Advance(void);
        void Read(void);

        ChildInfo mChildInfo;
        uint16_t  mIndex;
        bool      mIsDone;
    };
#endif // OPENTHREAD_FTD

private:
#if OPENTHREAD_FTD
    class ChildInfoIteratorBuilder : public InstanceLocator
    {
    public:
        explicit ChildInfoIteratorBuilder(Instance &aInstance)
            : InstanceLocator(aInstance)
        {
        }

        ChildInfoIterator begin(void) { return ChildInfoIterator(GetInstance()); }
        ChildInfoIterator end(void) { return ChildInfoIterator(GetInstance(), ChildInfoIterator::kEndIterator); }
    };
#endif

    Error ReadEntry(Key aKey, void *aValue, uint16_t aMaxLength) const;
    Error SaveEntry(Key aKey, const void *aValue, void *aPrev, uint16_t aLength);
    Error DeleteEntry(Key aKey);

    static void Log(Action aAction, Error aError, Key aKey, const void *aValue = nullptr);

    static const uint16_t kCriticalKeys[];
};

} // namespace ot

#endif // SETTINGS_HPP_
