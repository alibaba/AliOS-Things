#!/usr/bin/env python3
#
#  Copyright (c) 2016, The OpenThread Authors.
#  All rights reserved.
#
#  Redistribution and use in source and binary forms, with or without
#  modification, are permitted provided that the following conditions are met:
#  1. Redistributions of source code must retain the above copyright
#     notice, this list of conditions and the following disclaimer.
#  2. Redistributions in binary form must reproduce the above copyright
#     notice, this list of conditions and the following disclaimer in the
#     documentation and/or other materials provided with the distribution.
#  3. Neither the name of the copyright holder nor the
#     names of its contributors may be used to endorse or promote products
#     derived from this software without specific prior written permission.
#
#  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
#  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
#  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
#  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
#  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
#  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
#  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
#  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
#  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
#  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
#  POSSIBILITY OF SUCH DAMAGE.
#
import os
from enum import Enum

import coap
import dtls
import ipv6
import lowpan
import mesh_cop
import message
import mle
import net_crypto
import network_data
import network_diag
import network_layer
import simulator
import sniffer
from tlvs_parsing import SubTlvsFactory

# This extended address will generate the MESH_LOCAL_PREFIX
EXTENDED_PANID = '000db80000000000'
MESH_LOCAL_PREFIX = 'fd00:db8::/64'
MESH_LOCAL_PREFIX_REGEX_PATTERN = '^fd00:0?db8:0{0,4}:0{0,4}'
ROUTING_LOCATOR = '64/:0:ff:fe00:/16'
ROUTING_LOCATOR_REGEX_PATTERN = r'.*:(0)?:0{0,2}ff:fe00:\w{1,4}$'
LINK_LOCAL = 'fe80:/112'
LINK_LOCAL_REGEX_PATTERN = '^fe80:.*'
ALOC_FLAG_REGEX_PATTERN = '.*:fc..$'
LINK_LOCAL_All_THREAD_NODES_MULTICAST_ADDRESS = 'ff32:40:fd00:db8:0:0:0:1'
REALM_LOCAL_All_THREAD_NODES_MULTICAST_ADDRESS = 'ff33:40:fd00:db8:0:0:0:1'
REALM_LOCAL_ALL_NODES_ADDRESS = 'ff03::1'
REALM_LOCAL_ALL_ROUTERS_ADDRESS = 'ff03::2'
LINK_LOCAL_ALL_NODES_ADDRESS = 'ff02::1'
LINK_LOCAL_ALL_ROUTERS_ADDRESS = 'ff02::2'

DOMAIN_PREFIX = 'fd00:7d03:7d03:7d03::/64'
DOMAIN_PREFIX_REGEX_PATTERN = '^fd00:7d03:7d03:7d03:'
DOMAIN_PREFIX_ALTER = 'fd00:7d04:7d04:7d04::/64'

PORT_OFFSET = int(os.getenv('PORT_OFFSET', '0'))
BACKBONE_PREFIX = f'{0x9100 + PORT_OFFSET:04x}::/64'
BACKBONE_PREFIX_REGEX_PATTERN = f'^{0x9100 + PORT_OFFSET:04x}:'
BACKBONE_DOCKER_NETWORK_NAME = f'backbone{PORT_OFFSET}'
BACKBONE_IFNAME = 'eth0'

OTBR_DOCKER_IMAGE = os.getenv('OTBR_DOCKER_IMAGE', 'otbr-ot12-backbone-ci')
OTBR_DOCKER_NAME_PREFIX = f'otbr_{PORT_OFFSET}_'

ALL_NETWORK_BBRS_ADDRESS = 'ff32:40:fd00:db8:0:0:0:3'

ALL_DOMAIN_BBRS_ADDRESS = 'ff32:40:fd00:7d03:7d03:7d03:0:3'
ALL_DOMAIN_BBRS_ADDRESS_ALTER = 'ff32:40:fd00:7d04:7d04:7d04:0:3'

ONLINK_GUA_PREFIX = '2021::/64'

# Any address starts with 'fd' are considered on-link address.
ONLINK_PREFIX_REGEX_PATTERN = '^fd'

DEFAULT_NETWORK_KEY = bytearray([
    0x00,
    0x11,
    0x22,
    0x33,
    0x44,
    0x55,
    0x66,
    0x77,
    0x88,
    0x99,
    0xaa,
    0xbb,
    0xcc,
    0xdd,
    0xee,
    0xff,
])


class ADDRESS_TYPE(Enum):
    LINK_LOCAL = 'LINK_LOCAL'
    GLOBAL = 'GLOBAL'
    RLOC = 'RLOC'
    ALOC = 'ALOC'
    ML_EID = 'ML_EID'
    DUA = 'DUA'
    BACKBONE_GUA = 'BACKBONE_GUA'
    OMR = 'OMR'
    ONLINK_ULA = 'ONLINK_ULA'
    ONLINK_GUA = 'ONLINK_GUA'


RSSI = {
    'LINK_QULITY_0': -100,
    'LINK_QULITY_1': -95,
    'LINK_QULITY_2': -85,
    'LINK_QULITY_3': -65,
}

SNIFFER_ID = int(os.getenv('SNIFFER_ID', 34))
PANID = 0xface

MAX_NEIGHBOR_AGE = 100
INFINITE_COST_TIMEOUT = 90

MAX_ADVERTISEMENT_INTERVAL = 32
MLE_END_DEVICE_TIMEOUT = 100

AQ_TIMEOUT = 3
ADDRESS_QUERY_INITIAL_RETRY_DELAY = 15
DEFAULT_CHILD_TIMEOUT = 6
VIRTUAL_TIME = int(os.getenv('VIRTUAL_TIME', 0))
PARENT_AGGREGATIOIN_DELAY = 5
DUA_DAD_DELAY = 5
DEFAULT_BBR_REGISTRATION_JITTER = 2
DEFAULT_ROUTER_SELECTION_JITTER = 1

LEADER_NOTIFY_SED_BY_CHILD_UPDATE_REQUEST = True

THREAD_VERSION_1_1 = 2
THREAD_VERSION_1_2 = 3


def create_default_network_data_prefix_sub_tlvs_factories():
    return {
        network_data.TlvType.HAS_ROUTE:
            network_data.HasRouteFactory(routes_factory=network_data.RoutesFactory(
                route_factory=network_data.RouteFactory())),
        network_data.TlvType.BORDER_ROUTER:
            network_data.BorderRouterFactory(),
        network_data.TlvType.LOWPAN_ID:
            network_data.LowpanIdFactory(),
    }


def create_default_network_data_prefix_sub_tlvs_factory():
    return network_data.PrefixSubTlvsFactory(
        sub_tlvs_factories=create_default_network_data_prefix_sub_tlvs_factories())


def create_default_network_data_service_sub_tlvs_factories():
    return {network_data.TlvType.SERVER: network_data.ServerFactory()}


def create_default_network_data_service_sub_tlvs_factory():
    return network_data.ServiceSubTlvsFactory(
        sub_tlvs_factories=create_default_network_data_service_sub_tlvs_factories())


def create_default_network_data_commissioning_data_sub_tlvs_factories():
    return {
        mesh_cop.TlvType.CHANNEL: mesh_cop.ChannelFactory(),
        mesh_cop.TlvType.STEERING_DATA: mesh_cop.SteeringDataFactory(),
        mesh_cop.TlvType.BORDER_AGENT_LOCATOR: mesh_cop.BorderAgentLocatorFactory(),
        mesh_cop.TlvType.COMMISSIONER_SESSION_ID: mesh_cop.CommissionerSessionIdFactory(),
        mesh_cop.TlvType.COMMISSIONER_UDP_PORT: mesh_cop.CommissionerUdpPortFactory(),
    }


def create_default_network_data_commissioning_data_sub_tlvs_factory():
    return network_data.CommissioningDataSubTlvsFactory(
        sub_tlvs_factories=create_default_network_data_commissioning_data_sub_tlvs_factories())


def create_default_network_data_tlvs_factories():
    return {
        network_data.TlvType.PREFIX:
            network_data.PrefixFactory(sub_tlvs_factory=create_default_network_data_prefix_sub_tlvs_factory()),
        network_data.TlvType.SERVICE:
            network_data.ServiceFactory(sub_tlvs_factory=create_default_network_data_service_sub_tlvs_factory()),
        network_data.TlvType.COMMISSIONING:
            network_data.CommissioningDataFactory(
                sub_tlvs_factory=create_default_network_data_commissioning_data_sub_tlvs_factory()),
    }


def create_default_network_data_tlvs_factory():
    return network_data.NetworkDataTlvsFactory(sub_tlvs_factories=create_default_network_data_tlvs_factories())


def create_default_mle_tlv_route64_factory():
    return mle.Route64Factory(link_quality_and_route_data_factory=mle.LinkQualityAndRouteDataFactory())


def create_default_mle_tlv_network_data_factory():
    return mle.NetworkDataFactory(network_data_tlvs_factory=create_default_network_data_tlvs_factory())


def create_default_mle_tlv_address_registration_factory():
    return mle.AddressRegistrationFactory(
        addr_compressed_factory=mle.AddressCompressedFactory(),
        addr_full_factory=mle.AddressFullFactory(),
    )


def create_default_mle_tlv_thread_discovery_factory():
    return mle.ThreadDiscoveryFactory(thread_discovery_tlvs_factory=create_default_thread_discovery_tlvs_factory())


def create_default_thread_discovery_tlvs_factory():
    return mesh_cop.ThreadDiscoveryTlvsFactory(sub_tlvs_factories=create_default_thread_discovery_sub_tlvs_factories())


def create_default_thread_discovery_sub_tlvs_factories():
    return {
        mesh_cop.TlvType.DISCOVERY_REQUEST: mesh_cop.DiscoveryRequestFactory(),
        mesh_cop.TlvType.DISCOVERY_RESPONSE: mesh_cop.DiscoveryResponseFactory(),
        mesh_cop.TlvType.EXTENDED_PANID: mesh_cop.ExtendedPanidFactory(),
        mesh_cop.TlvType.NETWORK_NAME: mesh_cop.NetworkNameFactory(),
        mesh_cop.TlvType.STEERING_DATA: mesh_cop.SteeringDataFactory(),
        mesh_cop.TlvType.JOINER_UDP_PORT: mesh_cop.JoinerUdpPortFactory(),
        mesh_cop.TlvType.COMMISSIONER_UDP_PORT: mesh_cop.CommissionerUdpPortFactory(),
    }


def create_default_mle_tlvs_factories():
    return {
        mle.TlvType.SOURCE_ADDRESS: mle.SourceAddressFactory(),
        mle.TlvType.MODE: mle.ModeFactory(),
        mle.TlvType.TIMEOUT: mle.TimeoutFactory(),
        mle.TlvType.CHALLENGE: mle.ChallengeFactory(),
        mle.TlvType.RESPONSE: mle.ResponseFactory(),
        mle.TlvType.LINK_LAYER_FRAME_COUNTER: mle.LinkLayerFrameCounterFactory(),
        mle.TlvType.MLE_FRAME_COUNTER: mle.MleFrameCounterFactory(),
        mle.TlvType.ROUTE64: create_default_mle_tlv_route64_factory(),
        mle.TlvType.ADDRESS16: mle.Address16Factory(),
        mle.TlvType.LEADER_DATA: mle.LeaderDataFactory(),
        mle.TlvType.NETWORK_DATA: create_default_mle_tlv_network_data_factory(),
        mle.TlvType.TLV_REQUEST: mle.TlvRequestFactory(),
        mle.TlvType.SCAN_MASK: mle.ScanMaskFactory(),
        mle.TlvType.CONNECTIVITY: mle.ConnectivityFactory(),
        mle.TlvType.LINK_MARGIN: mle.LinkMarginFactory(),
        mle.TlvType.STATUS: mle.StatusFactory(),
        mle.TlvType.VERSION: mle.VersionFactory(),
        mle.TlvType.ADDRESS_REGISTRATION: create_default_mle_tlv_address_registration_factory(),
        mle.TlvType.CHANNEL: mle.ChannelFactory(),
        mle.TlvType.PANID: mle.PanIdFactory(),
        mle.TlvType.ACTIVE_TIMESTAMP: mle.ActiveTimestampFactory(),
        mle.TlvType.PENDING_TIMESTAMP: mle.PendingTimestampFactory(),
        mle.TlvType.CSL_CHANNEL: mle.CslChannelFactory(),
        mle.TlvType.CSL_SYNCHRONIZED_TIMEOUT: mle.CslSynchronizedTimeoutFactory(),
        mle.TlvType.CSL_CLOCK_ACCURACY: mle.CslClockAccuracyFactory(),
        mle.TlvType.ACTIVE_OPERATIONAL_DATASET: mle.ActiveOperationalDatasetFactory(),
        mle.TlvType.PENDING_OPERATIONAL_DATASET: mle.PendingOperationalDatasetFactory(),
        mle.TlvType.TIME_REQUEST: mle.TimeRequestFactory(),
        mle.TlvType.TIME_PARAMETER: mle.TimeParameterFactory(),
        mle.TlvType.THREAD_DISCOVERY: create_default_mle_tlv_thread_discovery_factory(),
        mle.TlvType.LINK_METRICS_QUERY: mle.LinkMetricsQueryFactory(),
        mle.TlvType.LINK_METRICS_MANAGEMENT: mle.LinkMetricsManagementFactory(),
        mle.TlvType.LINK_METRICS_REPORT: mle.LinkMetricsReportFactory(),
        mle.TlvType.LINK_PROBE: mle.LinkProbeFactory(),
    }


def create_default_mle_crypto_engine(network_key):
    return net_crypto.CryptoEngine(crypto_material_creator=net_crypto.MleCryptoMaterialCreator(network_key))


def create_default_mle_message_factory(network_key):
    return mle.MleMessageFactory(
        aux_sec_hdr_factory=net_crypto.AuxiliarySecurityHeaderFactory(),
        mle_command_factory=mle.MleCommandFactory(tlvs_factories=create_default_mle_tlvs_factories()),
        crypto_engine=create_default_mle_crypto_engine(network_key),
    )


def create_deafult_network_tlvs_factories():
    return {
        network_layer.TlvType.TARGET_EID:
            network_layer.TargetEidFactory(),
        network_layer.TlvType.MAC_EXTENDED_ADDRESS:
            network_layer.MacExtendedAddressFactory(),
        network_layer.TlvType.RLOC16:
            network_layer.Rloc16Factory(),
        network_layer.TlvType.ML_EID:
            network_layer.MlEidFactory(),
        network_layer.TlvType.STATUS:
            network_layer.StatusFactory(),
        network_layer.TlvType.TIME_SINCE_LAST_TRANSACTION:
            network_layer.TimeSinceLastTransactionFactory(),
        network_layer.TlvType.ROUTER_MASK:
            network_layer.RouterMaskFactory(),
        network_layer.TlvType.ND_OPTION:
            network_layer.NdOptionFactory(),
        network_layer.TlvType.ND_DATA:
            network_layer.NdDataFactory(),
        network_layer.TlvType.THREAD_NETWORK_DATA:
            network_layer.ThreadNetworkDataFactory(create_default_network_data_tlvs_factory()),
        network_layer.TlvType.XTAL_ACCURACY:
            network_layer.XtalAccuracyFactory(),
        # Routing information are distributed in a Thread network by MLE Routing TLV
        # which is in fact MLE Route64 TLV. Thread specificaton v1.1. - Chapter 5.20
        network_layer.TlvType.MLE_ROUTING:
            create_default_mle_tlv_route64_factory(),
        network_layer.TlvType.IPv6_ADDRESSES:
            network_layer.IPv6AddressesFactory(),
    }


def create_default_network_tlvs_factory():
    return SubTlvsFactory(sub_tlvs_factories=create_deafult_network_tlvs_factories())


def create_default_mesh_cop_tlvs_factories():
    return {
        mesh_cop.TlvType.CHANNEL: mesh_cop.ChannelFactory(),
        mesh_cop.TlvType.PAN_ID: mesh_cop.PanidFactory(),
        mesh_cop.TlvType.EXTENDED_PANID: mesh_cop.ExtendedPanidFactory(),
        mesh_cop.TlvType.NETWORK_NAME: mesh_cop.NetworkNameFactory(),
        mesh_cop.TlvType.PSKC: mesh_cop.PSKcFactory(),
        mesh_cop.TlvType.NETWORK_KEY: mesh_cop.NetworkKeyFactory(),
        mesh_cop.TlvType.NETWORK_KEY_SEQUENCE_COUNTER: mesh_cop.NetworkKeySequenceCounterFactory(),
        mesh_cop.TlvType.NETWORK_MESH_LOCAL_PREFIX: mesh_cop.NetworkMeshLocalPrefixFactory(),
        mesh_cop.TlvType.STEERING_DATA: mesh_cop.SteeringDataFactory(),
        mesh_cop.TlvType.BORDER_AGENT_LOCATOR: mesh_cop.BorderAgentLocatorFactory(),
        mesh_cop.TlvType.COMMISSIONER_ID: mesh_cop.CommissionerIdFactory(),
        mesh_cop.TlvType.COMMISSIONER_SESSION_ID: mesh_cop.CommissionerSessionIdFactory(),
        mesh_cop.TlvType.SECURITY_POLICY: mesh_cop.SecurityPolicyFactory(),
        mesh_cop.TlvType.GET: mesh_cop.GetFactory(),
        mesh_cop.TlvType.ACTIVE_TIMESTAMP: mesh_cop.ActiveTimestampFactory(),
        mesh_cop.TlvType.COMMISSIONER_UDP_PORT: mesh_cop.CommissionerUdpPortFactory(),
        mesh_cop.TlvType.STATE: mesh_cop.StateFactory(),
        mesh_cop.TlvType.JOINER_DTLS_ENCAPSULATION: mesh_cop.JoinerDtlsEncapsulationFactory(),
        mesh_cop.TlvType.JOINER_UDP_PORT: mesh_cop.JoinerUdpPortFactory(),
        mesh_cop.TlvType.JOINER_IID: mesh_cop.JoinerIIDFactory(),
        mesh_cop.TlvType.JOINER_ROUTER_LOCATOR: mesh_cop.JoinerRouterLocatorFactory(),
        mesh_cop.TlvType.JOINER_ROUTER_KEK: mesh_cop.JoinerRouterKEKFactory(),
        mesh_cop.TlvType.PROVISIONING_URL: mesh_cop.ProvisioningUrlFactory(),
        mesh_cop.TlvType.VENDOR_NAME: mesh_cop.VendorNameFactory(),
        mesh_cop.TlvType.VENDOR_MODEL: mesh_cop.VendorModelFactory(),
        mesh_cop.TlvType.VENDOR_SW_VERSION: mesh_cop.VendorSWVersionFactory(),
        mesh_cop.TlvType.VENDOR_DATA: mesh_cop.VendorDataFactory(),
        mesh_cop.TlvType.VENDOR_STACK_VERSION: mesh_cop.VendorStackVersionFactory(),
        mesh_cop.TlvType.UDP_ENCAPSULATION: mesh_cop.UdpEncapsulationFactory(),
        mesh_cop.TlvType.IPV6_ADDRESS: mesh_cop.Ipv6AddressFactory(),
        mesh_cop.TlvType.PENDING_TIMESTAMP: mesh_cop.PendingTimestampFactory(),
        mesh_cop.TlvType.DELAY_TIMER: mesh_cop.DelayTimerFactory(),
        mesh_cop.TlvType.CHANNEL_MASK: mesh_cop.ChannelMaskFactory(),
        mesh_cop.TlvType.COUNT: mesh_cop.CountFactory(),
        mesh_cop.TlvType.PERIOD: mesh_cop.PeriodFactory(),
        mesh_cop.TlvType.SCAN_DURATION: mesh_cop.ScanDurationFactory(),
        mesh_cop.TlvType.ENERGY_LIST: mesh_cop.EnergyListFactory(),
    }


def create_default_mesh_cop_tlvs_factory():
    return SubTlvsFactory(sub_tlvs_factories=create_default_mesh_cop_tlvs_factories())


def create_default_network_diag_tlv_factories():
    return {
        network_diag.TlvType.EXT_ADDRESS: network_layer.MacExtendedAddressFactory(),
        network_diag.TlvType.ADDRESS16: mle.Address16Factory(),
        network_diag.TlvType.MODE: mle.ModeFactory(),
        network_diag.TlvType.POLLING_PERIOD: mle.TimeoutFactory(),
        network_diag.TlvType.CONNECTIVITY: mle.ConnectivityFactory(),
        network_diag.TlvType.ROUTE64: create_default_mle_tlv_route64_factory(),
        network_diag.TlvType.LEADER_DATA: mle.LeaderDataFactory(),
        network_diag.TlvType.NETWORK_DATA: create_default_mle_tlv_network_data_factory(),
        network_diag.TlvType.IPV6_ADDRESS_LIST: network_diag.Ipv6AddressListFactory(),
        network_diag.TlvType.MAC_COUNTERS: network_diag.MacCountersFactory(),
        network_diag.TlvType.BATTERY_LEVEL: network_diag.BatteryLevelFactory(),
        network_diag.TlvType.SUPPLY_VOLTAGE: network_diag.SupplyVoltageFactory(),
        network_diag.TlvType.CHILD_TABLE: network_diag.ChildTableFactory(),
        network_diag.TlvType.CHANNEL_PAGES: network_diag.ChannelPagesFactory(),
        network_diag.TlvType.TYPE_LIST: network_diag.TypeListFactory(),
        network_diag.TlvType.MAX_CHILD_TIMEOUT: network_diag.MaxChildTimeoutFactory()
    }


def create_default_network_diag_tlvs_factory():
    return SubTlvsFactory(sub_tlvs_factories=create_default_network_diag_tlv_factories())


def create_default_uri_path_based_payload_factories():
    network_layer_tlvs_factory = create_default_network_tlvs_factory()
    mesh_cop_tlvs_factory = create_default_mesh_cop_tlvs_factory()
    network_diag_tlvs_factory = create_default_network_diag_tlvs_factory()

    return {
        '/a/as': network_layer_tlvs_factory,
        '/a/aq': network_layer_tlvs_factory,
        '/a/ar': network_layer_tlvs_factory,
        '/a/ae': network_layer_tlvs_factory,
        '/a/an': network_layer_tlvs_factory,
        '/a/sd': network_layer_tlvs_factory,
        '/c/lp': mesh_cop_tlvs_factory,
        '/c/cs': mesh_cop_tlvs_factory,
        '/d/da': network_diag_tlvs_factory,
        '/d/dg': network_diag_tlvs_factory,
        '/d/dq': network_diag_tlvs_factory,
        '/d/dr': network_diag_tlvs_factory,
        '/n/mr': network_layer_tlvs_factory,
        '/n/dr': network_layer_tlvs_factory,
        '/n/dn': network_layer_tlvs_factory,
    }


def create_default_coap_message_factory():
    return coap.CoapMessageFactory(
        options_factory=coap.CoapOptionsFactory(),
        uri_path_based_payload_factories=create_default_uri_path_based_payload_factories(),
        message_id_to_uri_path_binder=coap.CoapMessageIdToUriPathBinder(),
    )


def create_default_ipv6_hop_by_hop_options_factories():
    return {109: ipv6.MPLOptionFactory()}


def create_default_ipv6_hop_by_hop_options_factory():
    return ipv6.HopByHopOptionsFactory(options_factories=create_default_ipv6_hop_by_hop_options_factories())


def create_default_based_on_src_dst_ports_udp_payload_factory(network_key):
    mle_message_factory = create_default_mle_message_factory(network_key)
    coap_message_factory = create_default_coap_message_factory()
    dtls_message_factory = create_default_dtls_message_factory()

    return ipv6.UdpBasedOnSrcDstPortsPayloadFactory(
        src_dst_port_based_payload_factories={
            19788: mle_message_factory,
            61631: coap_message_factory,
            1000: dtls_message_factory,
            5683: coap_message_factory,
            5684: dtls_message_factory,
        })


def create_default_dtls_message_factory():
    return dtls.MessageFactory()


def create_default_ipv6_icmp_body_factories():
    return {
        ipv6.ICMP_DESTINATION_UNREACHABLE: ipv6.ICMPv6DestinationUnreachableFactory(),
        ipv6.ICMP_ECHO_REQUEST: ipv6.ICMPv6EchoBodyFactory(),
        ipv6.ICMP_ECHO_RESPONSE: ipv6.ICMPv6EchoBodyFactory(),
        'default': ipv6.BytesPayloadFactory(),
    }


def create_default_ipv6_upper_layer_factories(network_key):
    return {
        ipv6.IPV6_NEXT_HEADER_UDP:
            ipv6.UDPDatagramFactory(
                udp_header_factory=ipv6.UDPHeaderFactory(),
                udp_payload_factory=create_default_based_on_src_dst_ports_udp_payload_factory(network_key),
            ),
        ipv6.IPV6_NEXT_HEADER_ICMP:
            ipv6.ICMPv6Factory(body_factories=create_default_ipv6_icmp_body_factories()),
    }


def create_default_lowpan_extension_headers_factories():
    return {
        ipv6.IPV6_NEXT_HEADER_HOP_BY_HOP:
            lowpan.LowpanHopByHopFactory(hop_by_hop_options_factory=create_default_ipv6_hop_by_hop_options_factory())
    }


def create_default_ipv6_extension_headers_factories():
    return {
        ipv6.IPV6_NEXT_HEADER_HOP_BY_HOP:
            ipv6.HopByHopFactory(hop_by_hop_options_factory=create_default_ipv6_hop_by_hop_options_factory())
    }


def create_default_ipv6_packet_factory(network_key):
    return ipv6.IPv6PacketFactory(
        ehf=create_default_ipv6_extension_headers_factories(),
        ulpf=create_default_ipv6_upper_layer_factories(network_key),
    )


def create_default_lowpan_decompressor(context_manager):
    return lowpan.LowpanDecompressor(
        lowpan_ip_header_factory=lowpan.LowpanIpv6HeaderFactory(context_manager=context_manager),
        lowpan_extension_headers_factory=lowpan.LowpanExtensionHeadersFactory(
            ext_headers_factories=create_default_lowpan_extension_headers_factories()),
        lowpan_udp_header_factory=lowpan.LowpanUdpHeaderFactory(),
    )


def create_default_thread_context_manager():
    context_manager = lowpan.ContextManager()
    context_manager[0] = lowpan.Context(MESH_LOCAL_PREFIX)

    return context_manager


def create_default_lowpan_parser(context_manager, network_key=DEFAULT_NETWORK_KEY):
    return lowpan.LowpanParser(
        lowpan_mesh_header_factory=lowpan.LowpanMeshHeaderFactory(),
        lowpan_decompressor=create_default_lowpan_decompressor(context_manager),
        lowpan_fragements_buffers_manager=lowpan.LowpanFragmentsBuffersManager(),
        ipv6_packet_factory=create_default_ipv6_packet_factory(network_key),
    )


def create_default_thread_message_factory(network_key=DEFAULT_NETWORK_KEY):
    context_manager = create_default_thread_context_manager()
    lowpan_parser = create_default_lowpan_parser(context_manager, network_key)

    return message.MessageFactory(lowpan_parser=lowpan_parser)


def create_default_thread_sniffer(use_message_factory=True):
    return sniffer.Sniffer(create_default_thread_message_factory() if use_message_factory else None)


def create_default_simulator(use_message_factory=True):
    if VIRTUAL_TIME:
        return simulator.VirtualTime(use_message_factory=use_message_factory)
    return simulator.RealTime(use_message_factory=use_message_factory)
