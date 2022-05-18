#!/usr/bin/env python3
#
#  Copyright (c) 2017-2018, The OpenThread Authors.
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

import binascii

import ipaddress
import ipv6
import network_data
import network_layer
import common
import config
import mesh_cop
import mle

from enum import IntEnum


class CheckType(IntEnum):
    CONTAIN = 0
    NOT_CONTAIN = 1
    OPTIONAL = 2


class NetworkDataCheckType:
    PREFIX_CNT = 1
    PREFIX_CONTENT = 2


def check_address_query(command_msg, source_node, destination_address):
    """Verify source_node sent a properly formatted Address Query Request message to the destination_address.
    """
    command_msg.assertCoapMessageContainsTlv(network_layer.TargetEid)

    source_rloc = source_node.get_ip6_address(config.ADDRESS_TYPE.RLOC)
    assert (ipv6.ip_address(source_rloc) == command_msg.ipv6_packet.ipv6_header.source_address), (
        "Error: The IPv6 source address is not the RLOC of the originator. The source node's rloc is: " +
        str(ipv6.ip_address(source_rloc)) + ", but the source_address in command msg is: " +
        str(command_msg.ipv6_packet.ipv6_header.source_address))

    if isinstance(destination_address, bytearray):
        destination_address = bytes(destination_address)

    assert (ipv6.ip_address(destination_address) == command_msg.ipv6_packet.ipv6_header.destination_address
           ), "Error: The IPv6 destination address is not expected."


def check_address_notification(command_msg, source_node, destination_node):
    """Verify source_node sent a properly formatted Address Notification command message to destination_node.
    """
    command_msg.assertCoapMessageRequestUriPath('/a/an')
    command_msg.assertCoapMessageContainsTlv(network_layer.TargetEid)
    command_msg.assertCoapMessageContainsTlv(network_layer.Rloc16)
    command_msg.assertCoapMessageContainsTlv(network_layer.MlEid)

    source_rloc = source_node.get_ip6_address(config.ADDRESS_TYPE.RLOC)
    assert (ipv6.ip_address(source_rloc) == command_msg.ipv6_packet.ipv6_header.source_address
           ), "Error: The IPv6 source address is not the RLOC of the originator."

    destination_rloc = destination_node.get_ip6_address(config.ADDRESS_TYPE.RLOC)
    assert (ipv6.ip_address(destination_rloc) == command_msg.ipv6_packet.ipv6_header.destination_address
           ), "Error: The IPv6 destination address is not the RLOC of the destination."


def check_address_error_notification(command_msg, source_node, destination_address):
    """Verify source_node sent a properly formatted Address Error Notification command message to destination_address.
    """
    command_msg.assertCoapMessageRequestUriPath('/a/ae')
    command_msg.assertCoapMessageContainsTlv(network_layer.TargetEid)
    command_msg.assertCoapMessageContainsTlv(network_layer.MlEid)

    source_rloc = source_node.get_ip6_address(config.ADDRESS_TYPE.RLOC)
    assert (ipv6.ip_address(source_rloc) == command_msg.ipv6_packet.ipv6_header.source_address), (
        "Error: The IPv6 source address is not the RLOC of the originator. The source node's rloc is: " +
        str(ipv6.ip_address(source_rloc)) + ", but the source_address in command msg is: " +
        str(command_msg.ipv6_packet.ipv6_header.source_address))

    if isinstance(destination_address, bytearray):
        destination_address = bytes(destination_address)

    assert (ipv6.ip_address(destination_address) == command_msg.ipv6_packet.ipv6_header.destination_address), (
        "Error: The IPv6 destination address is not expected. The destination node's rloc is: " +
        str(ipv6.ip_address(destination_address)) + ", but the destination_address in command msg is: " +
        str(command_msg.ipv6_packet.ipv6_header.destination_address))


def check_address_solicit(command_msg, was_router):
    command_msg.assertCoapMessageRequestUriPath('/a/as')
    command_msg.assertCoapMessageContainsTlv(network_layer.MacExtendedAddress)
    command_msg.assertCoapMessageContainsTlv(network_layer.Status)
    if was_router:
        command_msg.assertCoapMessageContainsTlv(network_layer.Rloc16)
    else:
        command_msg.assertMleMessageDoesNotContainTlv(network_layer.Rloc16)


def check_address_release(command_msg, destination_node):
    """Verify the message is a properly formatted address release destined to the given node.
    """
    command_msg.assertCoapMessageRequestUriPath('/a/ar')
    command_msg.assertCoapMessageContainsTlv(network_layer.Rloc16)
    command_msg.assertCoapMessageContainsTlv(network_layer.MacExtendedAddress)

    destination_rloc = destination_node.get_ip6_address(config.ADDRESS_TYPE.RLOC)
    assert (ipv6.ip_address(destination_rloc) == command_msg.ipv6_packet.ipv6_header.destination_address
           ), "Error: The destination is not RLOC address"


def check_tlv_request_tlv(command_msg, check_type, tlv_id):
    """Verify if TLV Request TLV contains specified TLV ID
    """
    tlv_request_tlv = command_msg.get_mle_message_tlv(mle.TlvRequest)

    if check_type == CheckType.CONTAIN:
        assert (tlv_request_tlv is not None), "Error: The msg doesn't contain TLV Request TLV"
        assert any(
            tlv_id == tlv
            for tlv in tlv_request_tlv.tlvs), "Error: The msg doesn't contain TLV Request TLV ID: {}".format(tlv_id)

    elif check_type == CheckType.NOT_CONTAIN:
        if tlv_request_tlv is not None:
            assert (any(tlv_id == tlv for tlv in tlv_request_tlv.tlvs) is
                    False), "Error: The msg contains TLV Request TLV ID: {}".format(tlv_id)

    elif check_type == CheckType.OPTIONAL:
        if tlv_request_tlv is not None:
            if any(tlv_id == tlv for tlv in tlv_request_tlv.tlvs):
                print("TLV Request TLV contains TLV ID: {}".format(tlv_id))
            else:
                print("TLV Request TLV doesn't contain TLV ID: {}".format(tlv_id))
        else:
            print("The msg doesn't contain TLV Request TLV")

    else:
        raise ValueError("Invalid check type")


def check_link_request(
    command_msg,
    source_address=CheckType.OPTIONAL,
    leader_data=CheckType.OPTIONAL,
    tlv_request_address16=CheckType.OPTIONAL,
    tlv_request_route64=CheckType.OPTIONAL,
    tlv_request_link_margin=CheckType.OPTIONAL,
):
    """Verify a properly formatted Link Request command message.
    """
    command_msg.assertMleMessageContainsTlv(mle.Challenge)
    command_msg.assertMleMessageContainsTlv(mle.Version)

    check_mle_optional_tlv(command_msg, source_address, mle.SourceAddress)
    check_mle_optional_tlv(command_msg, leader_data, mle.LeaderData)

    check_tlv_request_tlv(command_msg, tlv_request_address16, mle.TlvType.ADDRESS16)
    check_tlv_request_tlv(command_msg, tlv_request_route64, mle.TlvType.ROUTE64)
    check_tlv_request_tlv(command_msg, tlv_request_link_margin, mle.TlvType.LINK_MARGIN)


def check_link_accept(
    command_msg,
    destination_node,
    leader_data=CheckType.OPTIONAL,
    link_margin=CheckType.OPTIONAL,
    mle_frame_counter=CheckType.OPTIONAL,
    challenge=CheckType.OPTIONAL,
    address16=CheckType.OPTIONAL,
    route64=CheckType.OPTIONAL,
    tlv_request_link_margin=CheckType.OPTIONAL,
):
    """verify a properly formatted link accept command message.
    """
    command_msg.assertMleMessageContainsTlv(mle.LinkLayerFrameCounter)
    command_msg.assertMleMessageContainsTlv(mle.SourceAddress)
    command_msg.assertMleMessageContainsTlv(mle.Response)
    command_msg.assertMleMessageContainsTlv(mle.Version)

    check_mle_optional_tlv(command_msg, leader_data, mle.LeaderData)
    check_mle_optional_tlv(command_msg, link_margin, mle.LinkMargin)
    check_mle_optional_tlv(command_msg, mle_frame_counter, mle.MleFrameCounter)
    check_mle_optional_tlv(command_msg, challenge, mle.Challenge)
    check_mle_optional_tlv(command_msg, address16, mle.Address16)
    check_mle_optional_tlv(command_msg, route64, mle.Route64)

    check_tlv_request_tlv(command_msg, tlv_request_link_margin, mle.TlvType.LINK_MARGIN)

    destination_link_local = destination_node.get_ip6_address(config.ADDRESS_TYPE.LINK_LOCAL)
    assert (ipv6.ip_address(destination_link_local) == command_msg.ipv6_packet.ipv6_header.destination_address
           ), "Error: The destination is unexpected"


def check_icmp_path(sniffer, path, nodes, icmp_type=ipv6.ICMP_ECHO_REQUEST):
    """Verify icmp message is forwarded along the path.
    """
    len_path = len(path)

    # Verify icmp message is forwarded to the next node of the path.
    for i in range(0, len_path):
        node_msg = sniffer.get_messages_sent_by(path[i])
        node_icmp_msg = node_msg.get_icmp_message(icmp_type)

        if i < len_path - 1:
            next_node = nodes[path[i + 1]]
            next_node_rloc16 = next_node.get_addr16()
            assert (next_node_rloc16 == node_icmp_msg.mac_header.dest_address.rloc), "Error: The path is unexpected."
        else:
            return True

    return False


def check_id_set(command_msg, router_id):
    """Check the command_msg's Route64 tlv to verify router_id is an active router.
    """
    tlv = command_msg.assertMleMessageContainsTlv(mle.Route64)
    return (tlv.router_id_mask >> (63 - router_id)) & 1


def get_routing_cost(command_msg, router_id):
    """Check the command_msg's Route64 tlv to get the routing cost to router.
    """
    tlv = command_msg.assertMleMessageContainsTlv(mle.Route64)

    # Get router's mask pos
    # Turn the number into binary string. Need to consider the preceding 0
    # omitted during conversion.
    router_id_mask_str = bin(tlv.router_id_mask).replace('0b', '')
    prefix_len = 64 - len(router_id_mask_str)
    routing_entry_pos = 0

    for i in range(0, router_id - prefix_len):
        if router_id_mask_str[i] == '1':
            routing_entry_pos += 1

    assert router_id_mask_str[router_id - prefix_len] == '1', \
        (("Error: The router isn't in the topology. \n",
          "route64 tlv is: %s. \nrouter_id is: %s. \nrouting_entry_pos is: %s. \nrouter_id_mask_str is: %s.") %
         (tlv, router_id, routing_entry_pos, router_id_mask_str))

    return tlv.link_quality_and_route_data[routing_entry_pos].route


def check_mle_optional_tlv(command_msg, type, tlv):
    if type == CheckType.CONTAIN:
        command_msg.assertMleMessageContainsTlv(tlv)
    elif type == CheckType.NOT_CONTAIN:
        command_msg.assertMleMessageDoesNotContainTlv(tlv)
    elif type == CheckType.OPTIONAL:
        command_msg.assertMleMessageContainsOptionalTlv(tlv)
    else:
        raise ValueError("Invalid check type")


def check_mle_advertisement(command_msg):
    command_msg.assertSentWithHopLimit(255)
    command_msg.assertSentToDestinationAddress(config.LINK_LOCAL_ALL_NODES_ADDRESS)
    command_msg.assertMleMessageContainsTlv(mle.SourceAddress)
    command_msg.assertMleMessageContainsTlv(mle.LeaderData)
    command_msg.assertMleMessageContainsTlv(mle.Route64)


def check_parent_request(command_msg, is_first_request):
    """Verify a properly formatted Parent Request command message.
    """
    if command_msg.mle.aux_sec_hdr.key_id_mode != 0x2:
        raise ValueError("The Key Identifier Mode of the Security Control Field SHALL be set to 0x02")

    command_msg.assertSentWithHopLimit(255)
    command_msg.assertSentToDestinationAddress(config.LINK_LOCAL_ALL_ROUTERS_ADDRESS)
    command_msg.assertMleMessageContainsTlv(mle.Mode)
    command_msg.assertMleMessageContainsTlv(mle.Challenge)
    command_msg.assertMleMessageContainsTlv(mle.Version)
    scan_mask = command_msg.assertMleMessageContainsTlv(mle.ScanMask)
    if not scan_mask.router:
        raise ValueError("Parent request without R bit set")
    if is_first_request:
        if scan_mask.end_device:
            raise ValueError("First parent request with E bit set")
    elif not scan_mask.end_device:
        raise ValueError("Second parent request without E bit set")


def check_parent_response(command_msg, mle_frame_counter=CheckType.OPTIONAL):
    """Verify a properly formatted Parent Response command message.
    """
    command_msg.assertMleMessageContainsTlv(mle.Challenge)
    command_msg.assertMleMessageContainsTlv(mle.Connectivity)
    command_msg.assertMleMessageContainsTlv(mle.LeaderData)
    command_msg.assertMleMessageContainsTlv(mle.LinkLayerFrameCounter)
    command_msg.assertMleMessageContainsTlv(mle.LinkMargin)
    command_msg.assertMleMessageContainsTlv(mle.Response)
    command_msg.assertMleMessageContainsTlv(mle.SourceAddress)
    command_msg.assertMleMessageContainsTlv(mle.Version)

    check_mle_optional_tlv(command_msg, mle_frame_counter, mle.MleFrameCounter)


def check_child_id_request(
    command_msg,
    tlv_request=CheckType.OPTIONAL,
    mle_frame_counter=CheckType.OPTIONAL,
    address_registration=CheckType.OPTIONAL,
    active_timestamp=CheckType.OPTIONAL,
    pending_timestamp=CheckType.OPTIONAL,
    route64=CheckType.OPTIONAL,
):
    """Verify a properly formatted Child Id Request command message.
    """
    if command_msg.mle.aux_sec_hdr.key_id_mode != 0x2:
        raise ValueError("The Key Identifier Mode of the Security Control Field SHALL be set to 0x02")

    command_msg.assertMleMessageContainsTlv(mle.LinkLayerFrameCounter)
    command_msg.assertMleMessageContainsTlv(mle.Mode)
    command_msg.assertMleMessageContainsTlv(mle.Response)
    command_msg.assertMleMessageContainsTlv(mle.Timeout)
    command_msg.assertMleMessageContainsTlv(mle.Version)

    check_mle_optional_tlv(command_msg, tlv_request, mle.TlvRequest)
    check_mle_optional_tlv(command_msg, mle_frame_counter, mle.MleFrameCounter)
    check_mle_optional_tlv(command_msg, address_registration, mle.AddressRegistration)
    check_mle_optional_tlv(command_msg, active_timestamp, mle.ActiveTimestamp)
    check_mle_optional_tlv(command_msg, pending_timestamp, mle.PendingTimestamp)
    check_mle_optional_tlv(command_msg, route64, mle.Route64)

    check_tlv_request_tlv(command_msg, CheckType.CONTAIN, mle.TlvType.ADDRESS16)
    check_tlv_request_tlv(command_msg, CheckType.CONTAIN, mle.TlvType.NETWORK_DATA)


def check_child_id_response(
    command_msg,
    route64=CheckType.OPTIONAL,
    network_data=CheckType.OPTIONAL,
    address_registration=CheckType.OPTIONAL,
    active_timestamp=CheckType.OPTIONAL,
    pending_timestamp=CheckType.OPTIONAL,
    active_operational_dataset=CheckType.OPTIONAL,
    pending_operational_dataset=CheckType.OPTIONAL,
    network_data_check=None,
):
    """Verify a properly formatted Child Id Response command message.
    """
    command_msg.assertMleMessageContainsTlv(mle.SourceAddress)
    command_msg.assertMleMessageContainsTlv(mle.LeaderData)
    command_msg.assertMleMessageContainsTlv(mle.Address16)

    check_mle_optional_tlv(command_msg, route64, mle.Route64)
    check_mle_optional_tlv(command_msg, network_data, mle.NetworkData)
    check_mle_optional_tlv(command_msg, address_registration, mle.AddressRegistration)
    check_mle_optional_tlv(command_msg, active_timestamp, mle.ActiveTimestamp)
    check_mle_optional_tlv(command_msg, pending_timestamp, mle.PendingTimestamp)
    check_mle_optional_tlv(command_msg, active_operational_dataset, mle.ActiveOperationalDataset)
    check_mle_optional_tlv(command_msg, pending_operational_dataset, mle.PendingOperationalDataset)

    if network_data_check is not None:
        network_data_tlv = command_msg.assertMleMessageContainsTlv(mle.NetworkData)
        network_data_check.check(network_data_tlv)


def check_prefix(prefix):
    """Verify if a prefix contains 6loWPAN sub-TLV and border router sub-TLV
    """
    assert contains_tlv(prefix.sub_tlvs, network_data.BorderRouter), 'Prefix doesn\'t contain a border router sub-TLV!'
    assert contains_tlv(prefix.sub_tlvs, network_data.LowpanId), 'Prefix doesn\'t contain a LowpanId sub-TLV!'


def check_child_update_request_from_child(
        command_msg,
        source_address=CheckType.OPTIONAL,
        leader_data=CheckType.OPTIONAL,
        challenge=CheckType.OPTIONAL,
        time_out=CheckType.OPTIONAL,
        address_registration=CheckType.OPTIONAL,
        tlv_request_tlv=CheckType.OPTIONAL,
        active_timestamp=CheckType.OPTIONAL,
        CIDs=(),
):

    command_msg.assertMleMessageContainsTlv(mle.Mode)
    check_mle_optional_tlv(command_msg, source_address, mle.SourceAddress)
    check_mle_optional_tlv(command_msg, leader_data, mle.LeaderData)
    check_mle_optional_tlv(command_msg, challenge, mle.Challenge)
    check_mle_optional_tlv(command_msg, time_out, mle.Timeout)
    check_mle_optional_tlv(command_msg, address_registration, mle.AddressRegistration)
    check_mle_optional_tlv(command_msg, tlv_request_tlv, mle.TlvRequest)
    check_mle_optional_tlv(command_msg, active_timestamp, mle.ActiveTimestamp)

    if (address_registration == CheckType.CONTAIN) and len(CIDs) > 0:
        _check_address_registration(command_msg, CIDs)


def check_coap_optional_tlv(coap_msg, type, tlv):
    if type == CheckType.CONTAIN:
        coap_msg.assertCoapMessageContainsTlv(tlv)
    elif type == CheckType.NOT_CONTAIN:
        coap_msg.assertCoapMessageDoesNotContainTlv(tlv)
    elif type == CheckType.OPTIONAL:
        coap_msg.assertCoapMessageContainsOptionalTlv(tlv)
    else:
        raise ValueError("Invalid check type")


def check_router_id_cached(node, router_id, cached=True):
    """Verify if the node has cached any entries based on the router ID
    """
    eidcaches = node.get_eidcaches()
    if cached:
        assert any(router_id == (int(rloc, 16) >> 10) for (_, rloc) in eidcaches)
    else:
        assert (any(router_id == (int(rloc, 16) >> 10) for (_, rloc) in eidcaches) is False)


def contains_tlv(sub_tlvs, tlv_type):
    """Verify if a specific type of tlv is included in a sub-tlv list.
    """
    return any(isinstance(sub_tlv, tlv_type) for sub_tlv in sub_tlvs)


def contains_tlvs(sub_tlvs, tlv_types):
    """Verify if all types of tlv in a list are included in a sub-tlv list.
    """
    return all((any(isinstance(sub_tlv, tlv_type) for sub_tlv in sub_tlvs)) for tlv_type in tlv_types)


def check_secure_mle_key_id_mode(command_msg, key_id_mode):
    """Verify if the mle command message sets the right key id mode.
    """
    assert isinstance(command_msg.mle, mle.MleMessageSecured)
    assert command_msg.mle.aux_sec_hdr.key_id_mode == key_id_mode


def check_data_response(command_msg, network_data_check=None, active_timestamp=CheckType.OPTIONAL):
    """Verify a properly formatted Data Response command message.
    """
    check_secure_mle_key_id_mode(command_msg, 0x02)
    command_msg.assertMleMessageContainsTlv(mle.SourceAddress)
    command_msg.assertMleMessageContainsTlv(mle.LeaderData)
    check_mle_optional_tlv(command_msg, active_timestamp, mle.ActiveTimestamp)
    if network_data_check is not None:
        network_data_tlv = command_msg.assertMleMessageContainsTlv(mle.NetworkData)
        network_data_check.check(network_data_tlv)


def check_child_update_request_from_parent(
    command_msg,
    leader_data=CheckType.OPTIONAL,
    network_data=CheckType.OPTIONAL,
    challenge=CheckType.OPTIONAL,
    tlv_request=CheckType.OPTIONAL,
    active_timestamp=CheckType.OPTIONAL,
):
    """Verify a properly formatted Child Update Request(from parent) command message.
    """
    check_secure_mle_key_id_mode(command_msg, 0x02)

    command_msg.assertMleMessageContainsTlv(mle.SourceAddress)
    check_mle_optional_tlv(command_msg, leader_data, mle.LeaderData)
    check_mle_optional_tlv(command_msg, network_data, mle.NetworkData)
    check_mle_optional_tlv(command_msg, challenge, mle.Challenge)
    check_mle_optional_tlv(command_msg, tlv_request, mle.TlvRequest)
    check_mle_optional_tlv(command_msg, active_timestamp, mle.ActiveTimestamp)


def check_child_update_response(
        command_msg,
        timeout=CheckType.OPTIONAL,
        address_registration=CheckType.OPTIONAL,
        address16=CheckType.OPTIONAL,
        leader_data=CheckType.OPTIONAL,
        network_data=CheckType.OPTIONAL,
        response=CheckType.OPTIONAL,
        link_layer_frame_counter=CheckType.OPTIONAL,
        mle_frame_counter=CheckType.OPTIONAL,
        CIDs=(),
):
    """Verify a properly formatted Child Update Response from parent
    """
    check_secure_mle_key_id_mode(command_msg, 0x02)

    command_msg.assertMleMessageContainsTlv(mle.SourceAddress)
    command_msg.assertMleMessageContainsTlv(mle.Mode)
    check_mle_optional_tlv(command_msg, timeout, mle.Timeout)
    check_mle_optional_tlv(command_msg, address_registration, mle.AddressRegistration)
    check_mle_optional_tlv(command_msg, address16, mle.Address16)
    check_mle_optional_tlv(command_msg, leader_data, mle.LeaderData)
    check_mle_optional_tlv(command_msg, network_data, mle.NetworkData)
    check_mle_optional_tlv(command_msg, response, mle.Response)
    check_mle_optional_tlv(command_msg, link_layer_frame_counter, mle.LinkLayerFrameCounter)
    check_mle_optional_tlv(command_msg, mle_frame_counter, mle.MleFrameCounter)

    if (address_registration == CheckType.CONTAIN) and len(CIDs) > 0:
        _check_address_registration(command_msg, CIDs)


def _check_address_registration(command_msg, CIDs=()):
    addresses = command_msg.assertMleMessageContainsTlv(mle.AddressRegistration).addresses
    for cid in CIDs:
        found = False
        for address in addresses:
            if isinstance(address, mle.AddressCompressed):
                if cid == address.cid:
                    found = True
                    break
        assert found, "AddressRegistration TLV doesn't have CID {} ".format(cid)


def get_sub_tlv(tlvs, tlv_type):
    for sub_tlv in tlvs:
        if isinstance(sub_tlv, tlv_type):
            return sub_tlv


def check_address_registration_tlv(
    command_msg,
    full_address,
):
    """Check whether or not a full IPv6 address in AddressRegistrationTlv.
    """
    found = False
    addr = ipaddress.ip_address(full_address)
    addresses = command_msg.assertMleMessageContainsTlv(mle.AddressRegistration).addresses

    for item in addresses:
        if isinstance(item, mle.AddressFull) and ipaddress.ip_address(item.ipv6_address) == addr:
            found = True
            break

    return found


def check_compressed_address_registration_tlv(command_msg, cid, iid, cid_present_once=False):
    '''Check whether or not a compressed IPv6 address in AddressRegistrationTlv.
    note: only compare the iid part of the address.

        Args:
            command_msg (MleMessage) : The Mle message to check.
            cid (int): The context id of the domain prefix.
            iid (string): The Interface Identifier.
            cid_present_once(boolean): True if cid entry should apprear only once in AR Tlv.
                                       False otherwise.
    '''
    found = False
    cid_cnt = 0

    addresses = command_msg.assertMleMessageContainsTlv(mle.AddressRegistration).addresses

    for item in addresses:
        if isinstance(item, mle.AddressCompressed):
            if cid == item.cid:
                cid_cnt = cid_cnt + 1
                if iid == item.iid.hex():
                    found = True
                    break
    assert found, 'Error: Expected (cid, iid):({},{}) Not Found'.format(cid, iid)

    assert cid_present_once == (cid_cnt == 1), 'Error: Expected cid present {} but present {}'.format(
        'once' if cid_present_once else '', cid_cnt)


def assert_contains_tlv(tlvs, check_type, tlv_type):
    """Assert a tlv list contains specific tlv and return the first qualified.
    """
    tlvs = [tlv for tlv in tlvs if isinstance(tlv, tlv_type)]
    if check_type is CheckType.CONTAIN:
        assert tlvs
        return tlvs[0]
    elif check_type is CheckType.NOT_CONTAIN:
        assert not tlvs
        return None
    elif check_type is CheckType.OPTIONAL:
        return None
    else:
        raise ValueError("Invalid check type: {}".format(check_type))


def check_discovery_request(command_msg, thread_version: str = None):
    """Verify a properly formatted Thread Discovery Request command message.
    """
    assert not isinstance(command_msg.mle, mle.MleMessageSecured)
    tlvs = command_msg.assertMleMessageContainsTlv(mle.ThreadDiscovery).tlvs
    request = assert_contains_tlv(tlvs, CheckType.CONTAIN, mesh_cop.DiscoveryRequest)
    assert not thread_version or thread_version in ['1.1', '1.2']
    if thread_version == '1.1':
        assert request.version == config.THREAD_VERSION_1_1
    elif thread_version == '1.2':
        assert request.version == config.THREAD_VERSION_1_2


def check_discovery_response(command_msg,
                             request_src_addr,
                             steering_data=CheckType.OPTIONAL,
                             thread_version: str = None):
    """Verify a properly formatted Thread Discovery Response command message.
    """
    assert not isinstance(command_msg.mle, mle.MleMessageSecured)
    assert (command_msg.mac_header.src_address.type == common.MacAddressType.LONG)
    assert command_msg.mac_header.dest_address == request_src_addr

    tlvs = command_msg.assertMleMessageContainsTlv(mle.ThreadDiscovery).tlvs
    response = assert_contains_tlv(tlvs, CheckType.CONTAIN, mesh_cop.DiscoveryResponse)
    assert not thread_version or thread_version in ['1.1', '1.2']
    if thread_version == '1.1':
        assert response.version == config.THREAD_VERSION_1_1
    elif thread_version == '1.2':
        assert response.version == config.THREAD_VERSION_1_2
    assert_contains_tlv(tlvs, CheckType.CONTAIN, mesh_cop.ExtendedPanid)
    assert_contains_tlv(tlvs, CheckType.CONTAIN, mesh_cop.NetworkName)
    assert_contains_tlv(tlvs, steering_data, mesh_cop.SteeringData)
    assert_contains_tlv(tlvs, steering_data, mesh_cop.JoinerUdpPort)

    check_type = (CheckType.CONTAIN if response.native_flag else CheckType.OPTIONAL)
    assert_contains_tlv(tlvs, check_type, mesh_cop.CommissionerUdpPort)


def get_joiner_udp_port_in_discovery_response(command_msg):
    """Get the udp port specified in a DISCOVERY RESPONSE message
    """
    tlvs = command_msg.assertMleMessageContainsTlv(mle.ThreadDiscovery).tlvs
    udp_port_tlv = assert_contains_tlv(tlvs, CheckType.CONTAIN, mesh_cop.JoinerUdpPort)
    return udp_port_tlv.udp_port


def check_joiner_commissioning_messages(commissioning_messages, url=''):
    """Verify COAP messages sent by joiner while commissioning process.
    """
    print(commissioning_messages)
    assert len(commissioning_messages) >= 4
    join_fin_req = commissioning_messages[0]
    assert join_fin_req.type == mesh_cop.MeshCopMessageType.JOIN_FIN_REQ
    if url:
        provisioning_url = assert_contains_tlv(join_fin_req.tlvs, CheckType.CONTAIN, mesh_cop.ProvisioningUrl)
        assert url == provisioning_url.url
    else:
        assert_contains_tlv(join_fin_req.tlvs, CheckType.NOT_CONTAIN, mesh_cop.ProvisioningUrl)

    join_ent_rsp = commissioning_messages[3]
    assert join_ent_rsp.type == mesh_cop.MeshCopMessageType.JOIN_ENT_RSP


def check_commissioner_commissioning_messages(commissioning_messages, state=mesh_cop.MeshCopState.ACCEPT):
    """Verify COAP messages sent by commissioner while commissioning process.
    """
    assert len(commissioning_messages) >= 2
    join_fin_rsq = commissioning_messages[1]
    assert join_fin_rsq.type == mesh_cop.MeshCopMessageType.JOIN_FIN_RSP
    rsq_state = assert_contains_tlv(join_fin_rsq.tlvs, CheckType.CONTAIN, mesh_cop.State)
    assert rsq_state.state == state


def check_joiner_router_commissioning_messages(commissioning_messages):
    """Verify COAP messages sent by joiner router while commissioning process.
    """
    if len(commissioning_messages) >= 4:
        join_ent_ntf = commissioning_messages[2]
    else:
        join_ent_ntf = commissioning_messages[0]
    assert join_ent_ntf.type == mesh_cop.MeshCopMessageType.JOIN_ENT_NTF
    return None


def check_payload_same(tp1, tp2):
    """Verfiy two payloads are totally the same.
       A payload is a tuple of tlvs.
    """
    assert len(tp1) == len(tp2)
    for tlv in tp2:
        peer_tlv = get_sub_tlv(tp1, type(tlv))
        assert (peer_tlv is not None and
                peer_tlv == tlv), 'peer_tlv:{}, tlv:{} type:{}'.format(peer_tlv, tlv, type(tlv))


def check_coap_message(msg, payloads, dest_addrs=None):
    if dest_addrs is not None:
        found = False
        for dest in dest_addrs:
            if msg.ipv6_packet.ipv6_header.destination_address == dest:
                found = True
                break
        assert found, 'Destination address incorrect'
    check_payload_same(msg.coap.payload, payloads)


class SinglePrefixCheck:

    def __init__(self, prefix=None, border_router_16=None):
        self._prefix = prefix
        self._border_router_16 = border_router_16

    def check(self, prefix_tlv):
        border_router_tlv = assert_contains_tlv(prefix_tlv.sub_tlvs, CheckType.CONTAIN, network_data.BorderRouter)
        assert_contains_tlv(prefix_tlv.sub_tlvs, CheckType.CONTAIN, network_data.LowpanId)
        result = True
        if self._prefix is not None:
            result &= self._prefix == binascii.hexlify(prefix_tlv.prefix)
        if self._border_router_16 is not None:
            result &= (self._border_router_16 == border_router_tlv.border_router_16)
        return result


class PrefixesCheck:

    def __init__(self, prefix_cnt=0, prefix_check_list=()):
        self._prefix_cnt = prefix_cnt
        self._prefix_check_list = prefix_check_list

    def check(self, prefix_tlvs):
        # if prefix_cnt is given, then check count only
        if self._prefix_cnt > 0:
            assert (len(prefix_tlvs) >= self._prefix_cnt), 'prefix count is less than expected'
        else:
            for prefix_check in self._prefix_check_list:
                found = False
                for prefix_tlv in prefix_tlvs:
                    if prefix_check.check(prefix_tlv):
                        found = True
                        break
                assert found, 'Some prefix is absent: {}'.format(prefix_check)


class CommissioningDataCheck:

    def __init__(self, stable=None, sub_tlv_type_list=()):
        self._stable = stable
        self._sub_tlv_type_list = sub_tlv_type_list

    def check(self, commissioning_data_tlv):
        if self._stable is not None:
            assert (self._stable == commissioning_data_tlv.stable), 'Commissioning Data stable flag is not correct'
        assert contains_tlvs(commissioning_data_tlv.sub_tlvs,
                             self._sub_tlv_type_list), 'Some sub tlvs are missing in Commissioning Data'


class NetworkDataCheck:

    def __init__(self, prefixes_check=None, commissioning_data_check=None):
        self._prefixes_check = prefixes_check
        self._commissioning_data_check = commissioning_data_check

    def check(self, network_data_tlv):
        if self._prefixes_check is not None:
            prefix_tlvs = [tlv for tlv in network_data_tlv.tlvs if isinstance(tlv, network_data.Prefix)]
            self._prefixes_check.check(prefix_tlvs)
        if self._commissioning_data_check is not None:
            commissioning_data_tlv = assert_contains_tlv(
                network_data_tlv.tlvs,
                CheckType.CONTAIN,
                network_data.CommissioningData,
            )
            self._commissioning_data_check.check(commissioning_data_tlv)
