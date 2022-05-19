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
 *   This file includes implementation for SRP server.
 */

#include "srp_server.hpp"

#if OPENTHREAD_CONFIG_SRP_SERVER_ENABLE

#include "common/const_cast.hpp"
#include "common/instance.hpp"
#include "common/locator_getters.hpp"
#include "common/logging.hpp"
#include "common/new.hpp"
#include "common/random.hpp"
#include "net/dns_types.hpp"
#include "thread/thread_netif.hpp"

namespace ot {
namespace Srp {

static const char kDefaultDomain[]       = "default.service.arpa.";
static const char kServiceSubTypeLabel[] = "._sub.";

static Dns::UpdateHeader::Response ErrorToDnsResponseCode(Error aError)
{
    Dns::UpdateHeader::Response responseCode;

    switch (aError)
    {
    case kErrorNone:
        responseCode = Dns::UpdateHeader::kResponseSuccess;
        break;
    case kErrorNoBufs:
        responseCode = Dns::UpdateHeader::kResponseServerFailure;
        break;
    case kErrorParse:
        responseCode = Dns::UpdateHeader::kResponseFormatError;
        break;
    case kErrorDuplicated:
        responseCode = Dns::UpdateHeader::kResponseNameExists;
        break;
    default:
        responseCode = Dns::UpdateHeader::kResponseRefused;
        break;
    }

    return responseCode;
}

//---------------------------------------------------------------------------------------------------------------------
// Server

Server::Server(Instance &aInstance)
    : InstanceLocator(aInstance)
    , mSocket(aInstance)
    , mServiceUpdateHandler(nullptr)
    , mServiceUpdateHandlerContext(nullptr)
    , mLeaseTimer(aInstance, HandleLeaseTimer)
    , mOutstandingUpdatesTimer(aInstance, HandleOutstandingUpdatesTimer)
    , mServiceUpdateId(Random::NonCrypto::GetUint32())
    , mPort(kUdpPortMin)
    , mState(kStateDisabled)
    , mAddressMode(kDefaultAddressMode)
    , mAnycastSequenceNumber(0)
    , mHasRegisteredAnyService(false)
{
    IgnoreError(SetDomain(kDefaultDomain));
}

void Server::SetServiceHandler(otSrpServerServiceUpdateHandler aServiceHandler, void *aServiceHandlerContext)
{
    mServiceUpdateHandler        = aServiceHandler;
    mServiceUpdateHandlerContext = aServiceHandlerContext;
}

Error Server::SetAddressMode(AddressMode aMode)
{
    Error error = kErrorNone;

    VerifyOrExit(mState == kStateDisabled, error = kErrorInvalidState);
    VerifyOrExit(mAddressMode != aMode);
    otLogInfoSrp("[server] Address Mode: %s -> %s", AddressModeToString(mAddressMode), AddressModeToString(aMode));
    mAddressMode = aMode;

exit:
    return error;
}

Error Server::SetAnycastModeSequenceNumber(uint8_t aSequenceNumber)
{
    Error error = kErrorNone;

    VerifyOrExit(mState == kStateDisabled, error = kErrorInvalidState);
    mAnycastSequenceNumber = aSequenceNumber;

    otLogInfoSrp("[server] Set Anycast Address Mode Seq Number to %d", aSequenceNumber);

exit:
    return error;
}

void Server::SetEnabled(bool aEnabled)
{
    if (aEnabled)
    {
        VerifyOrExit(mState == kStateDisabled);
        mState = kStateStopped;

        // Request publishing of "DNS/SRP Address Service" entry in the
        // Thread Network Data based of `mAddressMode`. Then wait for
        // callback `HandleNetDataPublisherEntryChange()` from the
        // `Publisher` to start the SRP server.

        switch (mAddressMode)
        {
        case kAddressModeUnicast:
            SelectPort();
            Get<NetworkData::Publisher>().PublishDnsSrpServiceUnicast(mPort);
            break;

        case kAddressModeAnycast:
            mPort = kAnycastAddressModePort;
            Get<NetworkData::Publisher>().PublishDnsSrpServiceAnycast(mAnycastSequenceNumber);
            break;
        }
    }
    else
    {
        VerifyOrExit(mState != kStateDisabled);
        Get<NetworkData::Publisher>().UnpublishDnsSrpService();
        Stop();
        mState = kStateDisabled;
    }

exit:
    return;
}

Server::LeaseConfig::LeaseConfig(void)
{
    mMinLease    = kDefaultMinLease;
    mMaxLease    = kDefaultMaxLease;
    mMinKeyLease = kDefaultMinKeyLease;
    mMaxKeyLease = kDefaultMaxKeyLease;
}

bool Server::LeaseConfig::IsValid(void) const
{
    bool valid = false;

    // TODO: Support longer LEASE.
    // We use milliseconds timer for LEASE & KEY-LEASE, this is to avoid overflow.
    VerifyOrExit(mMaxKeyLease <= Time::MsecToSec(TimerMilli::kMaxDelay));
    VerifyOrExit(mMinLease <= mMaxLease);
    VerifyOrExit(mMinKeyLease <= mMaxKeyLease);
    VerifyOrExit(mMinLease <= mMinKeyLease);
    VerifyOrExit(mMaxLease <= mMaxKeyLease);

    valid = true;

exit:
    return valid;
}

uint32_t Server::LeaseConfig::GrantLease(uint32_t aLease) const
{
    OT_ASSERT(mMinLease <= mMaxLease);

    return (aLease == 0) ? 0 : OT_MAX(mMinLease, OT_MIN(mMaxLease, aLease));
}

uint32_t Server::LeaseConfig::GrantKeyLease(uint32_t aKeyLease) const
{
    OT_ASSERT(mMinKeyLease <= mMaxKeyLease);

    return (aKeyLease == 0) ? 0 : OT_MAX(mMinKeyLease, OT_MIN(mMaxKeyLease, aKeyLease));
}

Error Server::SetLeaseConfig(const LeaseConfig &aLeaseConfig)
{
    Error error = kErrorNone;

    VerifyOrExit(aLeaseConfig.IsValid(), error = kErrorInvalidArgs);
    mLeaseConfig = aLeaseConfig;

exit:
    return error;
}

Error Server::SetDomain(const char *aDomain)
{
    Error    error = kErrorNone;
    uint16_t length;

    VerifyOrExit(mState == kStateDisabled, error = kErrorInvalidState);

    length = StringLength(aDomain, Dns::Name::kMaxNameSize);
    VerifyOrExit((length > 0) && (length < Dns::Name::kMaxNameSize), error = kErrorInvalidArgs);

    if (aDomain[length - 1] == '.')
    {
        error = mDomain.Set(aDomain);
    }
    else
    {
        // Need to append dot at the end

        char buf[Dns::Name::kMaxNameSize];

        VerifyOrExit(length < Dns::Name::kMaxNameSize - 1, error = kErrorInvalidArgs);

        memcpy(buf, aDomain, length);
        buf[length]     = '.';
        buf[length + 1] = '\0';

        error = mDomain.Set(buf);
    }

exit:
    return error;
}

const Server::Host *Server::GetNextHost(const Server::Host *aHost)
{
    return (aHost == nullptr) ? mHosts.GetHead() : aHost->GetNext();
}

// This method adds a SRP service host and takes ownership of it.
// The caller MUST make sure that there is no existing host with the same hostname.
void Server::AddHost(Host &aHost)
{
    OT_ASSERT(mHosts.FindMatching(aHost.GetFullName()) == nullptr);
    IgnoreError(mHosts.Add(aHost));
}

void Server::RemoveHost(Host *aHost, bool aRetainName, bool aNotifyServiceHandler)
{
    VerifyOrExit(aHost != nullptr);

    aHost->mLease = 0;
    aHost->ClearResources();

    if (aRetainName)
    {
        otLogInfoSrp("[server] remove host '%s' (but retain its name)", aHost->GetFullName());
    }
    else
    {
        aHost->mKeyLease = 0;
        IgnoreError(mHosts.Remove(*aHost));
        otLogInfoSrp("[server] fully remove host '%s'", aHost->GetFullName());
    }

    if (aNotifyServiceHandler && mServiceUpdateHandler != nullptr)
    {
        mServiceUpdateHandler(AllocateId(), aHost, kDefaultEventsHandlerTimeout, mServiceUpdateHandlerContext);
        // We don't wait for the reply from the service update handler,
        // but always remove the host (and its services) regardless of
        // host/service update result. Because removing a host should fail
        // only when there is system failure of the platform mDNS implementation
        // and in which case the host is not expected to be still registered.
    }

    if (!aRetainName)
    {
        aHost->Free();
    }

exit:
    return;
}

bool Server::HasNameConflictsWith(Host &aHost) const
{
    bool        hasConflicts = false;
    const Host *existingHost = mHosts.FindMatching(aHost.GetFullName());

    if (existingHost != nullptr && *aHost.GetKey() != *existingHost->GetKey())
    {
        ExitNow(hasConflicts = true);
    }

    for (const Service::Description &desc : aHost.mServiceDescriptions)
    {
        // Check on all hosts for a matching service description with
        // the same instance name and if found, verify that it has the
        // same key.

        for (const Host &host : mHosts)
        {
            if (host.FindServiceDescription(desc.GetInstanceName()) != nullptr)
            {
                VerifyOrExit(*aHost.GetKey() == *host.GetKey(), hasConflicts = true);
            }
        }
    }

exit:
    return hasConflicts;
}

void Server::HandleServiceUpdateResult(ServiceUpdateId aId, Error aError)
{
    UpdateMetadata *update = mOutstandingUpdates.FindMatching(aId);

    if (update != nullptr)
    {
        HandleServiceUpdateResult(update, aError);
    }
    else
    {
        otLogInfoSrp("[server] delayed SRP host update result, the SRP update has been committed");
    }
}

void Server::HandleServiceUpdateResult(UpdateMetadata *aUpdate, Error aError)
{
    IgnoreError(mOutstandingUpdates.Remove(*aUpdate));
    CommitSrpUpdate(aError, aUpdate->GetDnsHeader(), aUpdate->GetHost(), aUpdate->GetMessageInfo());
    aUpdate->Free();

    if (mOutstandingUpdates.IsEmpty())
    {
        mOutstandingUpdatesTimer.Stop();
    }
    else
    {
        mOutstandingUpdatesTimer.StartAt(mOutstandingUpdates.GetTail()->GetExpireTime(), 0);
    }
}

void Server::CommitSrpUpdate(Error                    aError,
                             const Dns::UpdateHeader &aDnsHeader,
                             Host &                   aHost,
                             const Ip6::MessageInfo & aMessageInfo)
{
    Host *   existingHost;
    uint32_t hostLease;
    uint32_t hostKeyLease;
    uint32_t grantedLease;
    uint32_t grantedKeyLease;
    bool     shouldFreeHost = true;

    SuccessOrExit(aError);

    hostLease       = aHost.GetLease();
    hostKeyLease    = aHost.GetKeyLease();
    grantedLease    = mLeaseConfig.GrantLease(hostLease);
    grantedKeyLease = mLeaseConfig.GrantKeyLease(hostKeyLease);

    aHost.SetLease(grantedLease);
    aHost.SetKeyLease(grantedKeyLease);

    for (Service::Description &desc : aHost.mServiceDescriptions)
    {
        desc.mLease    = grantedLease;
        desc.mKeyLease = grantedKeyLease;
    }

    existingHost = mHosts.FindMatching(aHost.GetFullName());

    if (aHost.GetLease() == 0)
    {
        if (aHost.GetKeyLease() == 0)
        {
            otLogInfoSrp("[server] remove key of host %s", aHost.GetFullName());
            RemoveHost(existingHost, /* aRetainName */ false, /* aNotifyServiceHandler */ false);
        }
        else if (existingHost != nullptr)
        {
            existingHost->SetKeyLease(aHost.GetKeyLease());
            RemoveHost(existingHost, /* aRetainName */ true, /* aNotifyServiceHandler */ false);

            for (Service &service : existingHost->mServices)
            {
                existingHost->RemoveService(&service, /* aRetainName */ true, /* aNotifyServiceHandler */ false);
            }
        }
    }
    else if (existingHost != nullptr)
    {
        SuccessOrExit(aError = existingHost->MergeServicesAndResourcesFrom(aHost));
    }
    else
    {
        otLogInfoSrp("[server] add new host %s", aHost.GetFullName());

        for (Service &service : aHost.GetServices())
        {
            service.mIsCommitted = true;
            service.Log(Service::kAddNew);
        }

        AddHost(aHost);
        shouldFreeHost = false;

#if OPENTHREAD_CONFIG_SRP_SERVER_PORT_SWITCH_ENABLE
        if (!mHasRegisteredAnyService && (mAddressMode == kAddressModeUnicast))
        {
            Settings::SrpServerInfo info;

            mHasRegisteredAnyService = true;
            info.SetPort(GetSocket().mSockName.mPort);
            IgnoreError(Get<Settings>().Save(info));
        }
#endif
    }

    // Re-schedule the lease timer.
    HandleLeaseTimer();

exit:
    if (aError == kErrorNone && !(grantedLease == hostLease && grantedKeyLease == hostKeyLease))
    {
        SendResponse(aDnsHeader, grantedLease, grantedKeyLease, aMessageInfo);
    }
    else
    {
        SendResponse(aDnsHeader, ErrorToDnsResponseCode(aError), aMessageInfo);
    }

    if (shouldFreeHost)
    {
        aHost.Free();
    }
}

void Server::SelectPort(void)
{
    mPort = kUdpPortMin;

#if OPENTHREAD_CONFIG_SRP_SERVER_PORT_SWITCH_ENABLE
    {
        Settings::SrpServerInfo info;

        if (Get<Settings>().Read(info) == kErrorNone)
        {
            mPort = info.GetPort() + 1;
            if (mPort < kUdpPortMin || mPort > kUdpPortMax)
            {
                mPort = kUdpPortMin;
            }
        }
    }
#endif

    otLogInfoSrp("[server] selected port %u", mPort);
}

void Server::Start(void)
{
    VerifyOrExit(mState == kStateStopped);

    mState = kStateRunning;
    PrepareSocket();
    otLogInfoSrp("[server] start listening on port %u", mPort);

exit:
    return;
}

void Server::PrepareSocket(void)
{
    Error error = kErrorNone;

#if OPENTHREAD_CONFIG_DNSSD_SERVER_ENABLE
    Ip6::Udp::Socket &dnsSocket = Get<Dns::ServiceDiscovery::Server>().mSocket;

    if (dnsSocket.GetSockName().GetPort() == mPort)
    {
        // If the DNS-SD socket matches our port number, we use the
        // same socket so we close our own socket (in case it was
        // open). `GetSocket()` will now return the DNS-SD socket.

        IgnoreError(mSocket.Close());
        ExitNow();
    }
#endif

    VerifyOrExit(!mSocket.IsOpen());
    SuccessOrExit(error = mSocket.Open(HandleUdpReceive, this));
    error = mSocket.Bind(mPort, OT_NETIF_THREAD);

exit:
    if (error != kErrorNone)
    {
        otLogCritSrp("[server] failed to prepare socket: %s", ErrorToString(error));
        Stop();
    }
}

Ip6::Udp::Socket &Server::GetSocket(void)
{
    Ip6::Udp::Socket *socket = &mSocket;

#if OPENTHREAD_CONFIG_DNSSD_SERVER_ENABLE
    Ip6::Udp::Socket &dnsSocket = Get<Dns::ServiceDiscovery::Server>().mSocket;

    if (dnsSocket.GetSockName().GetPort() == mPort)
    {
        socket = &dnsSocket;
    }
#endif

    return *socket;
}

#if OPENTHREAD_CONFIG_DNSSD_SERVER_ENABLE

void Server::HandleDnssdServerStateChange(void)
{
    // This is called from` Dns::ServiceDiscovery::Server` to notify
    // that it has started or stopped. We check whether we need to
    // share the socket.

    if (mState == kStateRunning)
    {
        PrepareSocket();
    }
}

Error Server::HandleDnssdServerUdpReceive(Message &aMessage, const Ip6::MessageInfo &aMessageInfo)
{
    // This is called from` Dns::ServiceDiscovery::Server` when a UDP
    // message is received on its socket. We check whether we are
    // sharing socket and if so we process the received message. We
    // return `kErrorNone` to indicate that message was successfully
    // processed by `Srp::Server`, otherwise `kErrorDrop` is returned.

    Error error = kErrorDrop;

    VerifyOrExit((mState == kStateRunning) && !mSocket.IsOpen());

    error = ProcessMessage(aMessage, aMessageInfo);

exit:
    return error;
}

#endif // OPENTHREAD_CONFIG_DNSSD_SERVER_ENABLE

void Server::Stop(void)
{
    VerifyOrExit(mState == kStateRunning);

    mState = kStateStopped;

    while (!mHosts.IsEmpty())
    {
        RemoveHost(mHosts.GetHead(), /* aRetainName */ false, /* aNotifyServiceHandler */ true);
    }

    // TODO: We should cancel any outstanding service updates, but current
    // OTBR mDNS publisher cannot properly handle it.
    while (!mOutstandingUpdates.IsEmpty())
    {
        mOutstandingUpdates.Pop()->Free();
    }

    mLeaseTimer.Stop();
    mOutstandingUpdatesTimer.Stop();

    otLogInfoSrp("[server] stop listening on %u", mPort);
    IgnoreError(mSocket.Close());
    mHasRegisteredAnyService = false;

exit:
    return;
}

void Server::HandleNetDataPublisherEvent(NetworkData::Publisher::Event aEvent)
{
    switch (aEvent)
    {
    case NetworkData::Publisher::kEventEntryAdded:
        Start();
        break;

    case NetworkData::Publisher::kEventEntryRemoved:
        Stop();
        break;
    }
}

const Server::UpdateMetadata *Server::FindOutstandingUpdate(const Ip6::MessageInfo &aMessageInfo,
                                                            uint16_t                aDnsMessageId)
{
    const UpdateMetadata *ret = nullptr;

    for (const UpdateMetadata &update : mOutstandingUpdates)
    {
        if (aDnsMessageId == update.GetDnsHeader().GetMessageId() &&
            aMessageInfo.GetPeerAddr() == update.GetMessageInfo().GetPeerAddr() &&
            aMessageInfo.GetPeerPort() == update.GetMessageInfo().GetPeerPort())
        {
            ExitNow(ret = &update);
        }
    }

exit:
    return ret;
}

void Server::ProcessDnsUpdate(Message &                aMessage,
                              const Ip6::MessageInfo & aMessageInfo,
                              const Dns::UpdateHeader &aDnsHeader,
                              uint16_t                 aOffset)
{
    Error     error = kErrorNone;
    Dns::Zone zone;
    Host *    host = nullptr;

    otLogInfoSrp("[server] receive DNS update from %s", aMessageInfo.GetPeerAddr().ToString().AsCString());

    SuccessOrExit(error = ProcessZoneSection(aMessage, aDnsHeader, aOffset, zone));

    if (FindOutstandingUpdate(aMessageInfo, aDnsHeader.GetMessageId()) != nullptr)
    {
        otLogInfoSrp("[server] drop duplicated SRP update request: messageId=%hu", aDnsHeader.GetMessageId());

        // Silently drop duplicate requests.
        // This could rarely happen, because the outstanding SRP update timer should
        // be shorter than the SRP update retransmission timer.
        ExitNow(error = kErrorNone);
    }

    // Per 2.3.2 of SRP draft 6, no prerequisites should be included in a SRP update.
    VerifyOrExit(aDnsHeader.GetPrerequisiteRecordCount() == 0, error = kErrorFailed);

    host = Host::New(GetInstance());
    VerifyOrExit(host != nullptr, error = kErrorNoBufs);
    SuccessOrExit(error = ProcessUpdateSection(*host, aMessage, aDnsHeader, zone, aOffset));

    // Parse lease time and validate signature.
    SuccessOrExit(error = ProcessAdditionalSection(host, aMessage, aDnsHeader, aOffset));

    HandleUpdate(aDnsHeader, *host, aMessageInfo);

exit:
    if (error != kErrorNone)
    {
        if (host != nullptr)
        {
            host->Free();
        }

        SendResponse(aDnsHeader, ErrorToDnsResponseCode(error), aMessageInfo);
    }
}

Error Server::ProcessZoneSection(const Message &          aMessage,
                                 const Dns::UpdateHeader &aDnsHeader,
                                 uint16_t &               aOffset,
                                 Dns::Zone &              aZone) const
{
    Error     error = kErrorNone;
    char      name[Dns::Name::kMaxNameSize];
    Dns::Zone zone;

    VerifyOrExit(aDnsHeader.GetZoneRecordCount() == 1, error = kErrorParse);

    SuccessOrExit(error = Dns::Name::ReadName(aMessage, aOffset, name, sizeof(name)));
    // TODO: return `Dns::kResponseNotAuth` for not authorized zone names.
    VerifyOrExit(strcmp(name, GetDomain()) == 0, error = kErrorSecurity);
    SuccessOrExit(error = aMessage.Read(aOffset, zone));
    aOffset += sizeof(zone);

    VerifyOrExit(zone.GetType() == Dns::ResourceRecord::kTypeSoa, error = kErrorParse);
    aZone = zone;

exit:
    return error;
}

Error Server::ProcessUpdateSection(Host &                   aHost,
                                   const Message &          aMessage,
                                   const Dns::UpdateHeader &aDnsHeader,
                                   const Dns::Zone &        aZone,
                                   uint16_t &               aOffset) const
{
    Error error = kErrorNone;

    // Process Service Discovery, Host and Service Description Instructions with
    // 3 times iterations over all DNS update RRs. The order of those processes matters.

    // 0. Enumerate over all Service Discovery Instructions before processing any other records.
    // So that we will know whether a name is a hostname or service instance name when processing
    // a "Delete All RRsets from a name" record.
    error = ProcessServiceDiscoveryInstructions(aHost, aMessage, aDnsHeader, aZone, aOffset);
    SuccessOrExit(error);

    // 1. Enumerate over all RRs to build the Host Description Instruction.
    error = ProcessHostDescriptionInstruction(aHost, aMessage, aDnsHeader, aZone, aOffset);
    SuccessOrExit(error);

    // 2. Enumerate over all RRs to build the Service Description Instructions.
    error = ProcessServiceDescriptionInstructions(aHost, aMessage, aDnsHeader, aZone, aOffset);
    SuccessOrExit(error);

    // 3. Verify that there are no name conflicts.
    VerifyOrExit(!HasNameConflictsWith(aHost), error = kErrorDuplicated);

exit:
    return error;
}

Error Server::ProcessHostDescriptionInstruction(Host &                   aHost,
                                                const Message &          aMessage,
                                                const Dns::UpdateHeader &aDnsHeader,
                                                const Dns::Zone &        aZone,
                                                uint16_t                 aOffset) const
{
    Error error;

    OT_ASSERT(aHost.GetFullName() == nullptr);

    for (uint16_t numRecords = aDnsHeader.GetUpdateRecordCount(); numRecords > 0; numRecords--)
    {
        char                name[Dns::Name::kMaxNameSize];
        Dns::ResourceRecord record;

        SuccessOrExit(error = Dns::Name::ReadName(aMessage, aOffset, name, sizeof(name)));

        SuccessOrExit(error = aMessage.Read(aOffset, record));

        if (record.GetClass() == Dns::ResourceRecord::kClassAny)
        {
            // Delete All RRsets from a name.
            VerifyOrExit(IsValidDeleteAllRecord(record), error = kErrorFailed);

            // A "Delete All RRsets from a name" RR can only apply to a Service or Host Description.

            if (aHost.FindServiceDescription(name) == nullptr)
            {
                // If host name is already set to a different name, `SetFullName()`
                // will return `kErrorFailed`.
                SuccessOrExit(error = aHost.SetFullName(name));
                aHost.ClearResources();
            }
        }
        else if (record.GetType() == Dns::ResourceRecord::kTypeAaaa)
        {
            Dns::AaaaRecord aaaaRecord;

            VerifyOrExit(record.GetClass() == aZone.GetClass(), error = kErrorFailed);

            SuccessOrExit(error = aHost.SetFullName(name));

            SuccessOrExit(error = aMessage.Read(aOffset, aaaaRecord));
            VerifyOrExit(aaaaRecord.IsValid(), error = kErrorParse);

            // Tolerate kErrorDrop for AAAA Resources.
            VerifyOrExit(aHost.AddIp6Address(aaaaRecord.GetAddress()) != kErrorNoBufs, error = kErrorNoBufs);
        }
        else if (record.GetType() == Dns::ResourceRecord::kTypeKey)
        {
            // We currently support only ECDSA P-256.
            Dns::Ecdsa256KeyRecord key;

            VerifyOrExit(record.GetClass() == aZone.GetClass(), error = kErrorFailed);
            SuccessOrExit(error = aMessage.Read(aOffset, key));
            VerifyOrExit(key.IsValid(), error = kErrorParse);

            VerifyOrExit(aHost.GetKey() == nullptr || *aHost.GetKey() == key, error = kErrorSecurity);
            aHost.SetKey(key);
        }

        aOffset += record.GetSize();
    }

    // Verify that we have a complete Host Description Instruction.

    VerifyOrExit(aHost.GetFullName() != nullptr, error = kErrorFailed);
    VerifyOrExit(aHost.GetKey() != nullptr, error = kErrorFailed);

    // We check the number of host addresses after processing of the
    // Lease Option in the Addition Section and determining whether
    // the host is being removed or registered.

exit:
    return error;
}

Error Server::ProcessServiceDiscoveryInstructions(Host &                   aHost,
                                                  const Message &          aMessage,
                                                  const Dns::UpdateHeader &aDnsHeader,
                                                  const Dns::Zone &        aZone,
                                                  uint16_t                 aOffset) const
{
    Error error = kErrorNone;

    for (uint16_t numRecords = aDnsHeader.GetUpdateRecordCount(); numRecords > 0; numRecords--)
    {
        char           serviceName[Dns::Name::kMaxNameSize];
        char           instanceName[Dns::Name::kMaxNameSize];
        Dns::PtrRecord ptrRecord;
        const char *   subServiceName;
        Service *      service;
        bool           isSubType;

        SuccessOrExit(error = Dns::Name::ReadName(aMessage, aOffset, serviceName, sizeof(serviceName)));
        VerifyOrExit(Dns::Name::IsSubDomainOf(serviceName, GetDomain()), error = kErrorSecurity);

        error = Dns::ResourceRecord::ReadRecord(aMessage, aOffset, ptrRecord);

        if (error == kErrorNotFound)
        {
            // `ReadRecord()` updates `aOffset` to skip over a
            // non-matching record.
            error = kErrorNone;
            continue;
        }

        SuccessOrExit(error);

        SuccessOrExit(error = Dns::Name::ReadName(aMessage, aOffset, instanceName, sizeof(instanceName)));

        VerifyOrExit(ptrRecord.GetClass() == Dns::ResourceRecord::kClassNone ||
                         ptrRecord.GetClass() == aZone.GetClass(),
                     error = kErrorFailed);

        // Check if the `serviceName` is a subtype with the name
        // format: "<sub-label>._sub.<service-labels>.<domain>."

        subServiceName = StringFind(serviceName, kServiceSubTypeLabel);
        isSubType      = (subServiceName != nullptr);

        if (isSubType)
        {
            // Skip over the "._sub." label to get to the base
            // service name.
            subServiceName += sizeof(kServiceSubTypeLabel) - 1;
        }

        // Verify that instance name and service name are related.

        VerifyOrExit(StringEndsWith(instanceName, isSubType ? subServiceName : serviceName), error = kErrorFailed);

        // Ensure the same service does not exist already.
        VerifyOrExit(aHost.FindService(serviceName, instanceName) == nullptr, error = kErrorFailed);

        service = aHost.AddNewService(serviceName, instanceName, isSubType);
        VerifyOrExit(service != nullptr, error = kErrorNoBufs);

        // This RR is a "Delete an RR from an RRset" update when the CLASS is NONE.
        service->mIsDeleted = (ptrRecord.GetClass() == Dns::ResourceRecord::kClassNone);
    }

exit:
    return error;
}

Error Server::ProcessServiceDescriptionInstructions(Host &                   aHost,
                                                    const Message &          aMessage,
                                                    const Dns::UpdateHeader &aDnsHeader,
                                                    const Dns::Zone &        aZone,
                                                    uint16_t &               aOffset) const
{
    Error     error = kErrorNone;
    TimeMilli now   = TimerMilli::GetNow();

    for (uint16_t numRecords = aDnsHeader.GetUpdateRecordCount(); numRecords > 0; numRecords--)
    {
        Service::Description *desc;
        char                  name[Dns::Name::kMaxNameSize];
        Dns::ResourceRecord   record;

        SuccessOrExit(error = Dns::Name::ReadName(aMessage, aOffset, name, sizeof(name)));
        SuccessOrExit(error = aMessage.Read(aOffset, record));

        if (record.GetClass() == Dns::ResourceRecord::kClassAny)
        {
            // Delete All RRsets from a name.
            VerifyOrExit(IsValidDeleteAllRecord(record), error = kErrorFailed);

            desc = aHost.FindServiceDescription(name);

            if (desc != nullptr)
            {
                desc->ClearResources();
                desc->mTimeLastUpdate = now;
            }

            aOffset += record.GetSize();
            continue;
        }

        if (record.GetType() == Dns::ResourceRecord::kTypeSrv)
        {
            Dns::SrvRecord srvRecord;
            char           hostName[Dns::Name::kMaxNameSize];
            uint16_t       hostNameLength = sizeof(hostName);

            VerifyOrExit(record.GetClass() == aZone.GetClass(), error = kErrorFailed);
            SuccessOrExit(error = aMessage.Read(aOffset, srvRecord));
            aOffset += sizeof(srvRecord);

            SuccessOrExit(error = Dns::Name::ReadName(aMessage, aOffset, hostName, hostNameLength));
            VerifyOrExit(Dns::Name::IsSubDomainOf(name, GetDomain()), error = kErrorSecurity);
            VerifyOrExit(aHost.Matches(hostName), error = kErrorFailed);

            desc = aHost.FindServiceDescription(name);
            VerifyOrExit(desc != nullptr, error = kErrorFailed);

            // Make sure that this is the first SRV RR for this service description
            VerifyOrExit(desc->mPort == 0, error = kErrorFailed);
            desc->mPriority       = srvRecord.GetPriority();
            desc->mWeight         = srvRecord.GetWeight();
            desc->mPort           = srvRecord.GetPort();
            desc->mTimeLastUpdate = now;
        }
        else if (record.GetType() == Dns::ResourceRecord::kTypeTxt)
        {
            VerifyOrExit(record.GetClass() == aZone.GetClass(), error = kErrorFailed);

            desc = aHost.FindServiceDescription(name);
            VerifyOrExit(desc != nullptr, error = kErrorFailed);

            aOffset += sizeof(record);
            SuccessOrExit(error = desc->SetTxtDataFromMessage(aMessage, aOffset, record.GetLength()));
            aOffset += record.GetLength();
        }
        else
        {
            aOffset += record.GetSize();
        }
    }

    // Verify that all service descriptions on `aHost` are updated. Note
    // that `mTimeLastUpdate` on a new `Service::Description` is set to
    // `GetNow().GetDistantPast()`.

    for (Service::Description &desc : aHost.mServiceDescriptions)
    {
        VerifyOrExit(desc.mTimeLastUpdate == now, error = kErrorFailed);

        // Check that either both `mPort` and `mTxtData` are set
        // (i.e., we saw both SRV and TXT record) or both are default
        // (cleared) value (i.e., we saw neither of them).

        VerifyOrExit((desc.mPort == 0) == (desc.mTxtData == nullptr), error = kErrorFailed);
    }

exit:
    return error;
}

bool Server::IsValidDeleteAllRecord(const Dns::ResourceRecord &aRecord)
{
    return aRecord.GetClass() == Dns::ResourceRecord::kClassAny && aRecord.GetType() == Dns::ResourceRecord::kTypeAny &&
           aRecord.GetTtl() == 0 && aRecord.GetLength() == 0;
}

Error Server::ProcessAdditionalSection(Host *                   aHost,
                                       const Message &          aMessage,
                                       const Dns::UpdateHeader &aDnsHeader,
                                       uint16_t &               aOffset) const
{
    Error            error = kErrorNone;
    Dns::OptRecord   optRecord;
    Dns::LeaseOption leaseOption;
    Dns::SigRecord   sigRecord;
    char             name[2]; // The root domain name (".") is expected.
    uint16_t         sigOffset;
    uint16_t         sigRdataOffset;
    char             signerName[Dns::Name::kMaxNameSize];
    uint16_t         signatureLength;

    VerifyOrExit(aDnsHeader.GetAdditionalRecordCount() == 2, error = kErrorFailed);

    // EDNS(0) Update Lease Option.

    SuccessOrExit(error = Dns::Name::ReadName(aMessage, aOffset, name, sizeof(name)));
    SuccessOrExit(error = aMessage.Read(aOffset, optRecord));
    SuccessOrExit(error = aMessage.Read(aOffset + sizeof(optRecord), leaseOption));
    VerifyOrExit(leaseOption.IsValid(), error = kErrorFailed);
    VerifyOrExit(optRecord.GetSize() == sizeof(optRecord) + sizeof(leaseOption), error = kErrorParse);

    aOffset += optRecord.GetSize();

    aHost->SetLease(leaseOption.GetLeaseInterval());
    aHost->SetKeyLease(leaseOption.GetKeyLeaseInterval());

    if (aHost->GetLease() > 0)
    {
        uint8_t hostAddressesNum;

        aHost->GetAddresses(hostAddressesNum);

        // There MUST be at least one valid address if we have nonzero lease.
        VerifyOrExit(hostAddressesNum > 0, error = kErrorFailed);
    }

    // SIG(0).

    sigOffset = aOffset;
    SuccessOrExit(error = Dns::Name::ReadName(aMessage, aOffset, name, sizeof(name)));
    SuccessOrExit(error = aMessage.Read(aOffset, sigRecord));
    VerifyOrExit(sigRecord.IsValid(), error = kErrorParse);

    sigRdataOffset = aOffset + sizeof(Dns::ResourceRecord);
    aOffset += sizeof(sigRecord);

    // TODO: Verify that the signature doesn't expire. This is not
    // implemented because the end device may not be able to get
    // the synchronized date/time.

    SuccessOrExit(error = Dns::Name::ReadName(aMessage, aOffset, signerName, sizeof(signerName)));

    signatureLength = sigRecord.GetLength() - (aOffset - sigRdataOffset);
    aOffset += signatureLength;

    // Verify the signature. Currently supports only ECDSA.

    VerifyOrExit(sigRecord.GetAlgorithm() == Dns::KeyRecord::kAlgorithmEcdsaP256Sha256, error = kErrorFailed);
    VerifyOrExit(sigRecord.GetTypeCovered() == 0, error = kErrorFailed);
    VerifyOrExit(signatureLength == Crypto::Ecdsa::P256::Signature::kSize, error = kErrorParse);

    SuccessOrExit(error = VerifySignature(*aHost->GetKey(), aMessage, aDnsHeader, sigOffset, sigRdataOffset,
                                          sigRecord.GetLength(), signerName));

exit:
    return error;
}

Error Server::VerifySignature(const Dns::Ecdsa256KeyRecord &aKey,
                              const Message &               aMessage,
                              Dns::UpdateHeader             aDnsHeader,
                              uint16_t                      aSigOffset,
                              uint16_t                      aSigRdataOffset,
                              uint16_t                      aSigRdataLength,
                              const char *                  aSignerName) const
{
    Error                          error;
    uint16_t                       offset = aMessage.GetOffset();
    uint16_t                       signatureOffset;
    Crypto::Sha256                 sha256;
    Crypto::Sha256::Hash           hash;
    Crypto::Ecdsa::P256::Signature signature;
    Message *                      signerNameMessage = nullptr;

    VerifyOrExit(aSigRdataLength >= Crypto::Ecdsa::P256::Signature::kSize, error = kErrorInvalidArgs);

    sha256.Start();

    // SIG RDATA less signature.
    sha256.Update(aMessage, aSigRdataOffset, sizeof(Dns::SigRecord) - sizeof(Dns::ResourceRecord));

    // The uncompressed (canonical) form of the signer name should be used for signature
    // verification. See https://tools.ietf.org/html/rfc2931#section-3.1 for details.
    signerNameMessage = Get<Ip6::Udp>().NewMessage(0);
    VerifyOrExit(signerNameMessage != nullptr, error = kErrorNoBufs);
    SuccessOrExit(error = Dns::Name::AppendName(aSignerName, *signerNameMessage));
    sha256.Update(*signerNameMessage, signerNameMessage->GetOffset(), signerNameMessage->GetLength());

    // We need the DNS header before appending the SIG RR.
    aDnsHeader.SetAdditionalRecordCount(aDnsHeader.GetAdditionalRecordCount() - 1);
    sha256.Update(aDnsHeader);
    sha256.Update(aMessage, offset + sizeof(aDnsHeader), aSigOffset - offset - sizeof(aDnsHeader));

    sha256.Finish(hash);

    signatureOffset = aSigRdataOffset + aSigRdataLength - Crypto::Ecdsa::P256::Signature::kSize;
    SuccessOrExit(error = aMessage.Read(signatureOffset, signature));

    error = aKey.GetKey().Verify(hash, signature);

exit:
    FreeMessage(signerNameMessage);
    return error;
}

void Server::HandleUpdate(const Dns::UpdateHeader &aDnsHeader, Host &aHost, const Ip6::MessageInfo &aMessageInfo)
{
    Error error = kErrorNone;
    Host *existingHost;

    // Check whether the SRP update wants to remove `aHost`.

    VerifyOrExit(aHost.GetLease() == 0);

    aHost.ClearResources();

    existingHost = mHosts.FindMatching(aHost.GetFullName());
    VerifyOrExit(existingHost != nullptr);

    // The client may not include all services it has registered before
    // when removing a host. We copy and append any missing services to
    // `aHost` from the `existingHost` and mark them as deleted.

    for (Service &service : existingHost->mServices)
    {
        if (service.mIsDeleted)
        {
            continue;
        }

        if (aHost.FindService(service.GetServiceName(), service.GetInstanceName()) == nullptr)
        {
            Service *newService =
                aHost.AddNewService(service.GetServiceName(), service.GetInstanceName(), service.IsSubType());

            VerifyOrExit(newService != nullptr, error = kErrorNoBufs);
            newService->mDescription.mTimeLastUpdate = TimerMilli::GetNow();
            newService->mIsDeleted                   = true;
        }
    }

exit:
    if (error != kErrorNone)
    {
        CommitSrpUpdate(error, aDnsHeader, aHost, aMessageInfo);
    }
    else if (mServiceUpdateHandler != nullptr)
    {
        UpdateMetadata *update = UpdateMetadata::New(GetInstance(), aDnsHeader, &aHost, aMessageInfo);

        IgnoreError(mOutstandingUpdates.Add(*update));
        mOutstandingUpdatesTimer.StartAt(mOutstandingUpdates.GetTail()->GetExpireTime(), 0);

        mServiceUpdateHandler(update->GetId(), &aHost, kDefaultEventsHandlerTimeout, mServiceUpdateHandlerContext);
    }
    else
    {
        CommitSrpUpdate(kErrorNone, aDnsHeader, aHost, aMessageInfo);
    }
}

void Server::SendResponse(const Dns::UpdateHeader &   aHeader,
                          Dns::UpdateHeader::Response aResponseCode,
                          const Ip6::MessageInfo &    aMessageInfo)
{
    Error             error;
    Message *         response = nullptr;
    Dns::UpdateHeader header;

    response = GetSocket().NewMessage(0);
    VerifyOrExit(response != nullptr, error = kErrorNoBufs);

    header.SetMessageId(aHeader.GetMessageId());
    header.SetType(Dns::UpdateHeader::kTypeResponse);
    header.SetQueryType(aHeader.GetQueryType());
    header.SetResponseCode(aResponseCode);
    SuccessOrExit(error = response->Append(header));

    SuccessOrExit(error = GetSocket().SendTo(*response, aMessageInfo));

    if (aResponseCode != Dns::UpdateHeader::kResponseSuccess)
    {
        otLogInfoSrp("[server] send fail response: %d", aResponseCode);
    }
    else
    {
        otLogInfoSrp("[server] send success response");
    }

exit:
    if (error != kErrorNone)
    {
        otLogWarnSrp("[server] failed to send response: %s", ErrorToString(error));
        FreeMessage(response);
    }
}

void Server::SendResponse(const Dns::UpdateHeader &aHeader,
                          uint32_t                 aLease,
                          uint32_t                 aKeyLease,
                          const Ip6::MessageInfo & aMessageInfo)
{
    Error             error;
    Message *         response = nullptr;
    Dns::UpdateHeader header;
    Dns::OptRecord    optRecord;
    Dns::LeaseOption  leaseOption;

    response = GetSocket().NewMessage(0);
    VerifyOrExit(response != nullptr, error = kErrorNoBufs);

    header.SetMessageId(aHeader.GetMessageId());
    header.SetType(Dns::UpdateHeader::kTypeResponse);
    header.SetQueryType(aHeader.GetQueryType());
    header.SetResponseCode(Dns::UpdateHeader::kResponseSuccess);
    header.SetAdditionalRecordCount(1);
    SuccessOrExit(error = response->Append(header));

    // Append the root domain (".").
    SuccessOrExit(error = Dns::Name::AppendTerminator(*response));

    optRecord.Init();
    optRecord.SetUdpPayloadSize(kUdpPayloadSize);
    optRecord.SetDnsSecurityFlag();
    optRecord.SetLength(sizeof(Dns::LeaseOption));
    SuccessOrExit(error = response->Append(optRecord));

    leaseOption.Init();
    leaseOption.SetLeaseInterval(aLease);
    leaseOption.SetKeyLeaseInterval(aKeyLease);
    SuccessOrExit(error = response->Append(leaseOption));

    SuccessOrExit(error = GetSocket().SendTo(*response, aMessageInfo));

    otLogInfoSrp("[server] send response with granted lease: %u and key lease: %u", aLease, aKeyLease);

exit:
    if (error != kErrorNone)
    {
        otLogWarnSrp("[server] failed to send response: %s", ErrorToString(error));
        FreeMessage(response);
    }
}

void Server::HandleUdpReceive(void *aContext, otMessage *aMessage, const otMessageInfo *aMessageInfo)
{
    static_cast<Server *>(aContext)->HandleUdpReceive(*static_cast<Message *>(aMessage),
                                                      *static_cast<const Ip6::MessageInfo *>(aMessageInfo));
}

void Server::HandleUdpReceive(Message &aMessage, const Ip6::MessageInfo &aMessageInfo)
{
    Error error = ProcessMessage(aMessage, aMessageInfo);

    if (error != kErrorNone)
    {
        otLogInfoSrp("[server] failed to handle DNS message: %s", ErrorToString(error));
    }
}

Error Server::ProcessMessage(Message &aMessage, const Ip6::MessageInfo &aMessageInfo)
{
    Error             error;
    Dns::UpdateHeader dnsHeader;
    uint16_t          offset = aMessage.GetOffset();

    SuccessOrExit(error = aMessage.Read(offset, dnsHeader));
    offset += sizeof(dnsHeader);

    // Handles only queries.
    VerifyOrExit(dnsHeader.GetType() == Dns::UpdateHeader::Type::kTypeQuery, error = kErrorDrop);

    switch (dnsHeader.GetQueryType())
    {
    case Dns::UpdateHeader::kQueryTypeUpdate:
        ProcessDnsUpdate(aMessage, aMessageInfo, dnsHeader, offset);
        break;
    default:
        error = kErrorDrop;
        break;
    }

exit:
    return error;
}

void Server::HandleLeaseTimer(Timer &aTimer)
{
    aTimer.Get<Server>().HandleLeaseTimer();
}

void Server::HandleLeaseTimer(void)
{
    TimeMilli now                = TimerMilli::GetNow();
    TimeMilli earliestExpireTime = now.GetDistantFuture();
    Host *    nextHost;

    for (Host *host = mHosts.GetHead(); host != nullptr; host = nextHost)
    {
        nextHost = host->GetNext();

        if (host->GetKeyExpireTime() <= now)
        {
            otLogInfoSrp("[server] KEY LEASE of host %s expired", host->GetFullName());

            // Removes the whole host and all services if the KEY RR expired.
            RemoveHost(host, /* aRetainName */ false, /* aNotifyServiceHandler */ true);
        }
        else if (host->IsDeleted())
        {
            // The host has been deleted, but the hostname & service instance names retain.

            Service *next;

            earliestExpireTime = OT_MIN(earliestExpireTime, host->GetKeyExpireTime());

            // Check if any service instance name expired.
            for (Service *service = host->mServices.GetHead(); service != nullptr; service = next)
            {
                next = service->GetNext();

                OT_ASSERT(service->mIsDeleted);

                if (service->GetKeyExpireTime() <= now)
                {
                    service->Log(Service::kKeyLeaseExpired);
                    host->RemoveService(service, /* aRetainName */ false, /* aNotifyServiceHandler */ true);
                }
                else
                {
                    earliestExpireTime = OT_MIN(earliestExpireTime, service->GetKeyExpireTime());
                }
            }
        }
        else if (host->GetExpireTime() <= now)
        {
            otLogInfoSrp("[server] LEASE of host %s expired", host->GetFullName());

            // If the host expired, delete all resources of this host and its services.
            for (Service &service : host->mServices)
            {
                // Don't need to notify the service handler as `RemoveHost` at below will do.
                host->RemoveService(&service, /* aRetainName */ true, /* aNotifyServiceHandler */ false);
            }

            RemoveHost(host, /* aRetainName */ true, /* aNotifyServiceHandler */ true);

            earliestExpireTime = OT_MIN(earliestExpireTime, host->GetKeyExpireTime());
        }
        else
        {
            // The host doesn't expire, check if any service expired or is explicitly removed.

            Service *next;

            OT_ASSERT(!host->IsDeleted());

            earliestExpireTime = OT_MIN(earliestExpireTime, host->GetExpireTime());

            for (Service *service = host->mServices.GetHead(); service != nullptr; service = next)
            {
                next = service->GetNext();

                if (service->GetKeyExpireTime() <= now)
                {
                    service->Log(Service::kKeyLeaseExpired);
                    host->RemoveService(service, /* aRetainName */ false, /* aNotifyServiceHandler */ true);
                }
                else if (service->mIsDeleted)
                {
                    // The service has been deleted but the name retains.
                    earliestExpireTime = OT_MIN(earliestExpireTime, service->GetKeyExpireTime());
                }
                else if (service->GetExpireTime() <= now)
                {
                    service->Log(Service::kLeaseExpired);

                    // The service is expired, delete it.
                    host->RemoveService(service, /* aRetainName */ true, /* aNotifyServiceHandler */ true);
                    earliestExpireTime = OT_MIN(earliestExpireTime, service->GetKeyExpireTime());
                }
                else
                {
                    earliestExpireTime = OT_MIN(earliestExpireTime, service->GetExpireTime());
                }
            }
        }
    }

    if (earliestExpireTime != now.GetDistantFuture())
    {
        OT_ASSERT(earliestExpireTime >= now);
        if (!mLeaseTimer.IsRunning() || earliestExpireTime <= mLeaseTimer.GetFireTime())
        {
            otLogInfoSrp("[server] lease timer is scheduled for %u seconds", Time::MsecToSec(earliestExpireTime - now));
            mLeaseTimer.StartAt(earliestExpireTime, 0);
        }
    }
    else
    {
        otLogInfoSrp("[server] lease timer is stopped");
        mLeaseTimer.Stop();
    }
}

void Server::HandleOutstandingUpdatesTimer(Timer &aTimer)
{
    aTimer.Get<Server>().HandleOutstandingUpdatesTimer();
}

void Server::HandleOutstandingUpdatesTimer(void)
{
    otLogInfoSrp("[server] outstanding service update timeout");
    while (!mOutstandingUpdates.IsEmpty() && mOutstandingUpdates.GetTail()->GetExpireTime() <= TimerMilli::GetNow())
    {
        HandleServiceUpdateResult(mOutstandingUpdates.GetTail(), kErrorResponseTimeout);
    }
}

const char *Server::AddressModeToString(AddressMode aMode)
{
    static const char *const kAddressModeStrings[] = {
        "unicast", // (0) kAddressModeUnicast
        "anycast", // (1) kAddressModeAnycast
    };

    static_assert(kAddressModeUnicast == 0, "kAddressModeUnicast value is incorrect");
    static_assert(kAddressModeAnycast == 1, "kAddressModeAnycast value is incorrect");

    return kAddressModeStrings[aMode];
}

//---------------------------------------------------------------------------------------------------------------------
// Server::Service

Server::Service *Server::Service::New(const char *aServiceName, Description &aDescription, bool aIsSubType)
{
    void *   buf;
    Service *service = nullptr;

    buf = Instance::HeapCAlloc(1, sizeof(Service));
    VerifyOrExit(buf != nullptr);

    service = new (buf) Service(aDescription, aIsSubType);

    if (service->mServiceName.Set(aServiceName) != kErrorNone)
    {
        service->Free();
        service = nullptr;
    }

exit:
    return service;
}

void Server::Service::Free(void)
{
    Instance::HeapFree(this);
}

Server::Service::Service(Description &aDescription, bool aIsSubType)
    : mDescription(aDescription)
    , mNext(nullptr)
    , mTimeLastUpdate(TimerMilli::GetNow())
    , mIsDeleted(false)
    , mIsSubType(aIsSubType)
    , mIsCommitted(false)
{
}

Error Server::Service::GetServiceSubTypeLabel(char *aLabel, uint8_t aMaxSize) const
{
    Error       error       = kErrorNone;
    const char *serviceName = GetServiceName();
    const char *subServiceName;
    uint8_t     labelLength;

    memset(aLabel, 0, aMaxSize);

    VerifyOrExit(IsSubType(), error = kErrorInvalidArgs);

    subServiceName = StringFind(serviceName, kServiceSubTypeLabel);
    OT_ASSERT(subServiceName != nullptr);

    if (subServiceName - serviceName < aMaxSize)
    {
        labelLength = static_cast<uint8_t>(subServiceName - serviceName);
    }
    else
    {
        labelLength = aMaxSize - 1;
        error       = kErrorNoBufs;
    }

    memcpy(aLabel, serviceName, labelLength);

exit:
    return error;
}

TimeMilli Server::Service::GetExpireTime(void) const
{
    OT_ASSERT(!mIsDeleted);
    OT_ASSERT(!GetHost().IsDeleted());

    return mTimeLastUpdate + Time::SecToMsec(mDescription.mLease);
}

TimeMilli Server::Service::GetKeyExpireTime(void) const
{
    return mTimeLastUpdate + Time::SecToMsec(mDescription.mKeyLease);
}

bool Server::Service::MatchesFlags(Flags aFlags) const
{
    bool matches = false;

    if (IsSubType())
    {
        VerifyOrExit(aFlags & kFlagSubType);
    }
    else
    {
        VerifyOrExit(aFlags & kFlagBaseType);
    }

    if (IsDeleted())
    {
        VerifyOrExit(aFlags & kFlagDeleted);
    }
    else
    {
        VerifyOrExit(aFlags & kFlagActive);
    }

    matches = true;

exit:
    return matches;
}

#if (OPENTHREAD_CONFIG_LOG_LEVEL >= OT_LOG_LEVEL_INFO) && OPENTHREAD_CONFIG_LOG_SRP
void Server::Service::Log(Action aAction) const
{
    static const char *const kActionStrings[] = {
        "add new",                   // (0) kAddNew
        "update existing",           // (1) kUpdateExisting
        "remove but retain name of", // (2) kRemoveButRetainName
        "full remove",               // (3) kFullyRemove
        "LEASE expired for ",        // (4) kLeaseExpired
        "KEY LEASE expired for",     // (5) kKeyLeaseExpired
    };

    char subLabel[Dns::Name::kMaxLabelSize];

    static_assert(0 == kAddNew, "kAddNew value is incorrect");
    static_assert(1 == kUpdateExisting, "kUpdateExisting value is incorrect");
    static_assert(2 == kRemoveButRetainName, "kRemoveButRetainName value is incorrect");
    static_assert(3 == kFullyRemove, "kFullyRemove value is incorrect");
    static_assert(4 == kLeaseExpired, "kLeaseExpired value is incorrect");
    static_assert(5 == kKeyLeaseExpired, "kKeyLeaseExpired value is incorrect");

    // We only log if the `Service` is marked as committed. This
    // ensures that temporary `Service` entries associated with a
    // newly received SRP update message are not logged (e.g., when
    // associated `Host` is being freed).

    if (mIsCommitted)
    {
        IgnoreError(GetServiceSubTypeLabel(subLabel, sizeof(subLabel)));

        otLogInfoSrp("[server] %s service '%s'%s%s", kActionStrings[aAction], GetInstanceName(),
                     IsSubType() ? " subtype:" : "", subLabel);
    }
}
#else
void Server::Service::Log(Action) const
{
}
#endif // #if (OPENTHREAD_CONFIG_LOG_LEVEL >= OT_LOG_LEVEL_INFO) && OPENTHREAD_CONFIG_LOG_SRP

//---------------------------------------------------------------------------------------------------------------------
// Server::Service::Description

Server::Service::Description *Server::Service::Description::New(const char *aInstanceName, Host &aHost)
{
    void *       buf;
    Description *desc = nullptr;

    buf = Instance::HeapCAlloc(1, sizeof(Description));
    VerifyOrExit(buf != nullptr);

    desc = new (buf) Description(aHost);

    if (desc->mInstanceName.Set(aInstanceName) != kErrorNone)
    {
        desc->Free();
        desc = nullptr;
    }

exit:
    return desc;
}

void Server::Service::Description::Free(void)
{
    mInstanceName.Free();
    Instance::HeapFree(this);
}

Server::Service::Description::Description(Host &aHost)
    : mNext(nullptr)
    , mHost(aHost)
    , mPriority(0)
    , mWeight(0)
    , mPort(0)
    , mTxtLength(0)
    , mTxtData(nullptr)
    , mLease(0)
    , mKeyLease(0)
    , mTimeLastUpdate(TimerMilli::GetNow().GetDistantPast())
{
}

void Server::Service::Description::ClearResources(void)
{
    mPort = 0;
    Instance::HeapFree(mTxtData);
    mTxtData   = nullptr;
    mTxtLength = 0;
}

void Server::Service::Description::TakeResourcesFrom(Description &aDescription)
{
    // Take ownership and move the heap allocated `mTxtData` buffer
    // from `aDescription
    Instance::HeapFree(mTxtData);
    mTxtData                = aDescription.mTxtData;
    mTxtLength              = aDescription.mTxtLength;
    aDescription.mTxtData   = nullptr;
    aDescription.mTxtLength = 0;

    mPriority = aDescription.mPriority;
    mWeight   = aDescription.mWeight;
    mPort     = aDescription.mPort;

    mLease          = aDescription.mLease;
    mKeyLease       = aDescription.mKeyLease;
    mTimeLastUpdate = TimerMilli::GetNow();
}

Error Server::Service::Description::SetTxtDataFromMessage(const Message &aMessage, uint16_t aOffset, uint16_t aLength)
{
    Error    error = kErrorNone;
    uint8_t *txtData;

    txtData = static_cast<uint8_t *>(Instance::HeapCAlloc(1, aLength));
    VerifyOrExit(txtData != nullptr, error = kErrorNoBufs);

    VerifyOrExit(aMessage.ReadBytes(aOffset, txtData, aLength) == aLength, error = kErrorParse);
    VerifyOrExit(Dns::TxtRecord::VerifyTxtData(txtData, aLength, /* aAllowEmpty */ false), error = kErrorParse);

    Instance::HeapFree(mTxtData);
    mTxtData   = txtData;
    mTxtLength = aLength;

exit:
    if (error != kErrorNone)
    {
        Instance::HeapFree(txtData);
    }

    return error;
}

//---------------------------------------------------------------------------------------------------------------------
// Server::Host

Server::Host *Server::Host::New(Instance &aInstance)
{
    void *buf;
    Host *host = nullptr;

    buf = Instance::HeapCAlloc(1, sizeof(Host));
    VerifyOrExit(buf != nullptr);

    host = new (buf) Host(aInstance);

exit:
    return host;
}

void Server::Host::Free(void)
{
    FreeAllServices();
    mFullName.Free();
    Instance::HeapFree(this);
}

Server::Host::Host(Instance &aInstance)
    : InstanceLocator(aInstance)
    , mAddressesNum(0)
    , mNext(nullptr)
    , mLease(0)
    , mKeyLease(0)
    , mTimeLastUpdate(TimerMilli::GetNow())
{
    mKey.Clear();
}

Error Server::Host::SetFullName(const char *aFullName)
{
    // `mFullName` becomes immutable after it is set, so if it is
    // already set, we only accept a `aFullName` that matches the
    // current name.

    Error error;

    if (mFullName.IsNull())
    {
        error = mFullName.Set(aFullName);
    }
    else
    {
        error = Matches(aFullName) ? kErrorNone : kErrorFailed;
    }

    return error;
}

void Server::Host::SetKey(Dns::Ecdsa256KeyRecord &aKey)
{
    OT_ASSERT(aKey.IsValid());

    mKey = aKey;
}

TimeMilli Server::Host::GetExpireTime(void) const
{
    OT_ASSERT(!IsDeleted());

    return mTimeLastUpdate + Time::SecToMsec(mLease);
}

TimeMilli Server::Host::GetKeyExpireTime(void) const
{
    return mTimeLastUpdate + Time::SecToMsec(mKeyLease);
}

const Server::Service *Server::Host::FindNextService(const Service *aPrevService,
                                                     Service::Flags aFlags,
                                                     const char *   aServiceName,
                                                     const char *   aInstanceName) const
{
    const Service *service = (aPrevService == nullptr) ? GetServices().GetHead() : aPrevService->GetNext();

    for (; service != nullptr; service = service->GetNext())
    {
        if (!service->MatchesFlags(aFlags))
        {
            continue;
        }

        if ((aServiceName != nullptr) && !service->MatchesServiceName(aServiceName))
        {
            continue;
        }

        if ((aInstanceName != nullptr) && !service->MatchesInstanceName(aInstanceName))
        {
            continue;
        }

        break;
    }

    return service;
}

Server::Service *Server::Host::AddNewService(const char *aServiceName, const char *aInstanceName, bool aIsSubType)
{
    Service *             service = nullptr;
    Service::Description *desc;

    desc = FindServiceDescription(aInstanceName);

    if (desc == nullptr)
    {
        desc = Service::Description::New(aInstanceName, *this);
        VerifyOrExit(desc != nullptr);
        mServiceDescriptions.Push(*desc);
    }

    service = Service::New(aServiceName, *desc, aIsSubType);
    VerifyOrExit(service != nullptr);

    mServices.Push(*service);

exit:
    return service;
}

void Server::Host::RemoveService(Service *aService, bool aRetainName, bool aNotifyServiceHandler)
{
    Server &server = Get<Server>();

    VerifyOrExit(aService != nullptr);

    aService->mIsDeleted = true;

    aService->Log(aRetainName ? Service::kRemoveButRetainName : Service::kFullyRemove);

    if (aNotifyServiceHandler && server.mServiceUpdateHandler != nullptr)
    {
        server.mServiceUpdateHandler(server.AllocateId(), this, kDefaultEventsHandlerTimeout,
                                     server.mServiceUpdateHandlerContext);
        // We don't wait for the reply from the service update handler,
        // but always remove the service regardless of service update result.
        // Because removing a service should fail only when there is system
        // failure of the platform mDNS implementation and in which case the
        // service is not expected to be still registered.
    }

    if (!aRetainName)
    {
        IgnoreError(mServices.Remove(*aService));
        aService->Free();
        FreeUnusedServiceDescriptions();
    }

exit:
    return;
}

void Server::Host::FreeAllServices(void)
{
    while (!mServices.IsEmpty())
    {
        RemoveService(mServices.GetHead(), /* aRetainName */ false, /* aNotifyServiceHandler */ false);
    }
}

void Server::Host::FreeUnusedServiceDescriptions(void)
{
    Service::Description *desc;
    Service::Description *prev;
    Service::Description *next;

    for (prev = nullptr, desc = mServiceDescriptions.GetHead(); desc != nullptr; desc = next)
    {
        next = desc->GetNext();

        if (FindNextService(/* aPrevService */ nullptr, kFlagsAnyService, /* aServiceName */ nullptr,
                            desc->GetInstanceName()) == nullptr)
        {
            mServiceDescriptions.PopAfter(prev);
            desc->Free();

            // When the `desc` is removed from the list
            // we keep the `prev` pointer same as before.
        }
        else
        {
            prev = desc;
        }
    }
}

void Server::Host::ClearResources(void)
{
    mAddressesNum = 0;
}

Error Server::Host::MergeServicesAndResourcesFrom(Host &aHost)
{
    // This method merges services, service descriptions, and other
    // resources from another `aHost` into current host. It can
    // possibly take ownership of some items from `aHost`.

    Error error = kErrorNone;

    otLogInfoSrp("[server] update host %s", GetFullName());

    memcpy(mAddresses, aHost.mAddresses, aHost.mAddressesNum * sizeof(mAddresses[0]));
    mAddressesNum   = aHost.mAddressesNum;
    mKey            = aHost.mKey;
    mLease          = aHost.mLease;
    mKeyLease       = aHost.mKeyLease;
    mTimeLastUpdate = TimerMilli::GetNow();

    for (Service &service : aHost.mServices)
    {
        Service *existingService = FindService(service.GetServiceName(), service.GetInstanceName());
        Service *newService;

        if (service.mIsDeleted)
        {
            // `RemoveService()` does nothing if `exitsingService` is `nullptr`.
            RemoveService(existingService, /* aRetainName */ true, /* aNotifyServiceHandler */ false);
            continue;
        }

        // Add/Merge `service` into the existing service or a allocate a new one

        newService = (existingService != nullptr)
                         ? existingService
                         : AddNewService(service.GetServiceName(), service.GetInstanceName(), service.IsSubType());

        VerifyOrExit(newService != nullptr, error = kErrorNoBufs);

        newService->mIsDeleted      = false;
        newService->mIsCommitted    = true;
        newService->mTimeLastUpdate = TimerMilli::GetNow();

        if (!service.mIsSubType)
        {
            // (1) Service description is shared across a base type and all its subtypes.
            // (2) `TakeResourcesFrom()` releases resources pinned to its argument.
            // Therefore, make sure the function is called only for the base type.
            newService->mDescription.TakeResourcesFrom(service.mDescription);
        }

        newService->Log((existingService != nullptr) ? Service::kUpdateExisting : Service::kAddNew);
    }

exit:
    return error;
}

const Server::Service::Description *Server::Host::FindServiceDescription(const char *aInstanceName) const
{
    return mServiceDescriptions.FindMatching(aInstanceName);
}

Server::Service::Description *Server::Host::FindServiceDescription(const char *aInstanceName)
{
    return AsNonConst(AsConst(this)->FindServiceDescription(aInstanceName));
}

const Server::Service *Server::Host::FindService(const char *aServiceName, const char *aInstanceName) const
{
    return FindNextService(/* aPrevService */ nullptr, kFlagsAnyService, aServiceName, aInstanceName);
}

Server::Service *Server::Host::FindService(const char *aServiceName, const char *aInstanceName)
{
    return AsNonConst(AsConst(this)->FindService(aServiceName, aInstanceName));
}

Error Server::Host::AddIp6Address(const Ip6::Address &aIp6Address)
{
    Error error = kErrorNone;

    if (aIp6Address.IsMulticast() || aIp6Address.IsUnspecified() || aIp6Address.IsLoopback())
    {
        // We don't like those address because they cannot be used
        // for communication with exterior devices.
        ExitNow(error = kErrorDrop);
    }

    for (const Ip6::Address &addr : mAddresses)
    {
        if (aIp6Address == addr)
        {
            // Drop duplicate addresses.
            ExitNow(error = kErrorDrop);
        }
    }

    if (mAddressesNum >= kMaxAddressesNum)
    {
        otLogWarnSrp("[server] too many addresses for host %s", GetFullName());
        ExitNow(error = kErrorNoBufs);
    }

    mAddresses[mAddressesNum++] = aIp6Address;

exit:
    return error;
}

//---------------------------------------------------------------------------------------------------------------------
// Server::UpdateMetadata

Server::UpdateMetadata *Server::UpdateMetadata::New(Instance &               aInstance,
                                                    const Dns::UpdateHeader &aHeader,
                                                    Host *                   aHost,
                                                    const Ip6::MessageInfo & aMessageInfo)
{
    void *          buf;
    UpdateMetadata *update = nullptr;

    buf = Instance::HeapCAlloc(1, sizeof(UpdateMetadata));
    VerifyOrExit(buf != nullptr);

    update = new (buf) UpdateMetadata(aInstance, aHeader, aHost, aMessageInfo);

exit:
    return update;
}

void Server::UpdateMetadata::Free(void)
{
    Instance::HeapFree(this);
}

Server::UpdateMetadata::UpdateMetadata(Instance &               aInstance,
                                       const Dns::UpdateHeader &aHeader,
                                       Host *                   aHost,
                                       const Ip6::MessageInfo & aMessageInfo)
    : InstanceLocator(aInstance)
    , mExpireTime(TimerMilli::GetNow() + kDefaultEventsHandlerTimeout)
    , mDnsHeader(aHeader)
    , mId(Get<Server>().AllocateId())
    , mHost(aHost)
    , mMessageInfo(aMessageInfo)
    , mNext(nullptr)
{
}

} // namespace Srp
} // namespace ot

#endif // OPENTHREAD_CONFIG_SRP_SERVER_ENABLE
