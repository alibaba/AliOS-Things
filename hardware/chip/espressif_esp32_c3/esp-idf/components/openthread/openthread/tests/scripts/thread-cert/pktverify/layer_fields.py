#!/usr/bin/env python3
#
#  Copyright (c) 2019, The OpenThread Authors.
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
import datetime
import sys
import time
from typing import Any, Union

from pyshark.packet.fields import LayerFieldsContainer, LayerField
from pyshark.packet.packet import Packet as RawPacket

from pktverify.addrs import EthAddr, ExtAddr, Ipv6Addr
from pktverify.bytes import Bytes
from pktverify.consts import VALID_LAYER_NAMES
from pktverify.null_field import nullField


def _auto(v: Union[LayerFieldsContainer, LayerField]):
    """parse the layer field automatically according to its format"""
    assert not isinstance(v, LayerFieldsContainer) or len(v.fields) == 1 or v.get_default_value() is not None, v.fields
    dv = v.get_default_value()
    rv = v.raw_value

    if dv.startswith('0x'):
        return int(dv, 16)

    try:
        if dv == rv:
            return int(dv)
        elif int(dv) == int(rv, 16):
            return int(dv)
    except (ValueError, TypeError):
        pass

    if rv is None:
        try:
            return int(dv)
        except (ValueError, TypeError):
            pass

    if ':' in dv and '::' not in dv and dv.replace(':', '') == rv:  # '88:00', '8800'
        return int(rv, 16)

    # timestamp: 'Jan  1, 1970 08:00:00.000000000 CST', '0000000000000000'
    # convert to seconds from 1970, ignore the nanosecond for now since
    # there are integer seconds applied in the test cases
    try:
        time_str = datetime.datetime.strptime(dv, "%b  %d, %Y %H:%M:%S.%f000 %Z")
        time_in_sec = time.mktime(time_str.utctimetuple())
        return int(time_in_sec)
    except (ValueError, TypeError):
        pass

    try:
        int(rv, 16)
        return int(dv)
    except Exception:
        pass

    raise ValueError((v, v.get_default_value(), v.raw_value))


def _payload(v: Union[LayerFieldsContainer, LayerField]) -> bytearray:
    """parse the layer field as a bytearray"""
    assert not isinstance(v, LayerFieldsContainer) or len(v.fields) == 1
    hex_value = v.raw_value
    assert len(hex_value) % 2 == 0
    s = bytearray()
    for i in range(0, len(hex_value), 2):
        s.append(int(hex_value[i:i + 2], 16))

    return s


def _hex(v: Union[LayerFieldsContainer, LayerField]) -> int:
    """parse the layer field as a hex string"""
    # split v into octets and reverse the order
    assert not isinstance(v, LayerFieldsContainer) or len(v.fields) == 1
    return int(v.get_default_value(), 16)


def _raw_hex(v: Union[LayerFieldsContainer, LayerField]) -> int:
    """parse the layer field as a raw hex string"""
    # split v into octets and reverse the order
    assert not isinstance(v, LayerFieldsContainer) or len(v.fields) == 1
    iv = v.hex_value

    try:
        int(v.get_default_value())
        assert int(v.get_default_value()) == iv, (v.get_default_value(), v.raw_value)
    except ValueError:
        pass

    try:
        int(v.get_default_value(), 16)
        assert int(v.get_default_value(), 16) == iv, (v.get_default_value(), v.raw_value)
    except ValueError:
        pass

    return iv


def _raw_hex_rev(v: Union[LayerFieldsContainer, LayerField]) -> int:
    """parse the layer field as a reversed raw hex string"""
    # split v into octets and reverse the order
    assert not isinstance(v, LayerFieldsContainer) or len(v.fields) == 1
    rv = v.raw_value
    octets = [rv[i:i + 2] for i in range(0, len(rv), 2)]

    iv = int(''.join(reversed(octets)), 16)

    try:
        int(v.get_default_value())
        assert int(v.get_default_value()) == iv, (v.get_default_value(), v.raw_value)
    except ValueError:
        pass

    try:
        int(v.get_default_value(), 16)
        assert int(v.get_default_value(), 16) == iv, (v.get_default_value(), v.raw_value)
    except ValueError:
        pass

    return iv


def _dec(v: Union[LayerFieldsContainer, LayerField]) -> int:
    """parse the layer field as a decimal"""
    assert not isinstance(v, LayerFieldsContainer) or len(v.fields) == 1
    return int(v.get_default_value())


def _float(v: Union[LayerFieldsContainer, LayerField]) -> float:
    """parse the layer field as a float"""
    assert not isinstance(v, LayerFieldsContainer) or len(v.fields) == 1
    return float(v.get_default_value())


def _str(v: Union[LayerFieldsContainer, LayerField]) -> str:
    """parse the layer field as a string"""
    assert not isinstance(v, LayerFieldsContainer) or len(v.fields) == 1
    return str(v.get_default_value())


def _bytes(v: Union[LayerFieldsContainer, LayerField]) -> Bytes:
    """parse the layer field as raw bytes"""
    assert not isinstance(v, LayerFieldsContainer) or len(v.fields) == 1
    return Bytes(v.raw_value)


def _ext_addr(v: Union[LayerFieldsContainer, LayerField]) -> ExtAddr:
    """parse the layer field as an extended address"""
    assert not isinstance(v, LayerFieldsContainer) or len(v.fields) == 1
    return ExtAddr(v.get_default_value())


def _ipv6_addr(v: Union[LayerFieldsContainer, LayerField]) -> Ipv6Addr:
    """parse the layer field as an IPv6 address"""
    assert not isinstance(v, LayerFieldsContainer) or len(v.fields) == 1
    return Ipv6Addr(v.get_default_value())


def _eth_addr(v: Union[LayerFieldsContainer, LayerField]) -> EthAddr:
    """parse the layer field as an Ethernet MAC address"""
    assert not isinstance(v, LayerFieldsContainer) or len(v.fields) == 1, v.fields
    return EthAddr(v.get_default_value())


def _routerid_set(v: Union[LayerFieldsContainer, LayerField]) -> set:
    """parse the layer field as a set of router ids

       Notes: the router ID mask in wireshark is a
              hexadecimal string separated by ':'
    """
    assert not isinstance(v, LayerFieldsContainer) or len(v.fields) == 1

    try:
        ridmask = str(v.get_default_value())
        assert isinstance(ridmask, str), ridmask
        ridmask_int = int(ridmask.replace(':', ''), base=16)
        rid_set = set()
        count = 0
        while ridmask_int:
            count += 1
            if ridmask_int & 1:
                rid_set.add(64 - count)
            ridmask_int = ridmask_int >> 1
    except ValueError:
        pass

    return rid_set


class _first(object):
    """parse the first layer field"""

    def __init__(self, sub_parse):
        self._sub_parse = sub_parse

    def __call__(self, v: Union[LayerFieldsContainer, LayerField]):
        return self._sub_parse(v.fields[0])


class _list(object):
    """parse all layer fields into a list"""

    def __init__(self, sub_parse):
        self._sub_parse = sub_parse

    def __call__(self, v: Union[LayerFieldsContainer, LayerField]):
        return [self._sub_parse(f) for f in v.fields]


_LAYER_FIELDS = {
    # WPAN
    'wpan.fcf': _raw_hex_rev,
    'wpan.cmd': _auto,
    'wpan.security': _auto,
    'wpan.frame_type': _auto,
    'wpan.pending': _auto,
    'wpan.ack_request': _auto,
    'wpan.pan_id_compression': _auto,
    'wpan.seqno_suppression': _auto,
    'wpan.ie_present': _auto,
    'wpan.dst_addr_mode': _auto,
    'wpan.version': _auto,
    'wpan.src_addr_mode': _auto,
    'wpan.dst_pan': _auto,
    'wpan.seq_no': _auto,
    'wpan.src16': _auto,
    'wpan.dst16': _auto,
    'wpan.src64': _ext_addr,
    'wpan.dst64': _ext_addr,
    'wpan.fcs': _raw_hex_rev,
    'wpan.fcs_ok': _auto,
    'wpan.frame_length': _dec,
    'wpan.key_number': _auto,
    'wpan.aux_sec.sec_suite': _auto,
    'wpan.aux_sec.security_control_field': _auto,
    'wpan.aux_sec.sec_level': _auto,
    'wpan.aux_sec.key_id_mode': _auto,
    'wpan.aux_sec.frame_counter_suppression': _auto,
    'wpan.aux_sec.asn_in_nonce': _auto,
    'wpan.aux_sec.reserved': _auto,
    'wpan.aux_sec.frame_counter': _auto,
    'wpan.aux_sec.key_source': _auto,
    'wpan.aux_sec.key_index': _auto,
    'wpan.aux_sec.hdr': _str,
    'wpan.mic': _auto,
    'wpan.channel': _auto,
    'wpan.header_ie.id': _list(_auto),
    'wpan.header_ie.csl.period': _auto,
    'wpan.payload_ie.vendor.oui': _auto,

    # MLE
    'mle.cmd': _auto,
    'mle.sec_suite': _hex,
    'mle.tlv.type': _list(_dec),
    'mle.tlv.len': _list(_dec),
    'mle.tlv.mode.receiver_on_idle': _auto,
    'mle.tlv.mode.reserved1': _auto,
    'mle.tlv.mode.reserved2': _auto,
    'mle.tlv.mode.device_type_bit': _auto,
    'mle.tlv.mode.network_data': _auto,
    'mle.tlv.challenge': _bytes,
    'mle.tlv.scan_mask.r': _auto,
    'mle.tlv.scan_mask.e': _auto,
    'mle.tlv.version': _auto,
    'mle.tlv.source_addr': _auto,
    'mle.tlv.active_tstamp': _auto,
    'mle.tlv.pending_tstamp': _auto,
    'mle.tlv.leader_data.partition_id': _auto,
    'mle.tlv.leader_data.weighting': _auto,
    'mle.tlv.leader_data.data_version': _auto,
    'mle.tlv.leader_data.stable_data_version': _auto,
    'mle.tlv.leader_data.router_id': _auto,
    'mle.tlv.route64.nbr_out': _list(_auto),
    'mle.tlv.route64.nbr_in': _list(_auto),
    'mle.tlv.route64.id_seq': _auto,
    'mle.tlv.route64.id_mask': _routerid_set,
    'mle.tlv.route64.cost': _list(_auto),
    'mle.tlv.response': _bytes,
    'mle.tlv.mle_frm_cntr': _auto,
    'mle.tlv.ll_frm_cntr': _auto,
    'mle.tlv.link_margin': _auto,
    'mle.tlv.conn.sed_dgram_cnt': _auto,
    'mle.tlv.conn.sed_buf_size': _auto,
    'mle.tlv.conn.lq3': _auto,
    'mle.tlv.conn.lq2': _auto,
    'mle.tlv.conn.lq1': _auto,
    'mle.tlv.conn.leader_cost': _auto,
    'mle.tlv.conn.id_seq': _auto,
    'mle.tlv.conn.flags.pp': _auto,
    'mle.tlv.conn.active_rtrs': _auto,
    'mle.tlv.timeout': _auto,
    'mle.tlv.addr16': _auto,
    'mle.tlv.channel': _auto,
    'mle.tlv.addr_reg_iid': _list(_auto),
    'mle.tlv.link_enh_ack_flags': _auto,
    'mle.tlv.link_forward_series': _list(_auto),
    'mle.tlv.link_requested_type_id_flags': _list(_hex),
    'mle.tlv.link_sub_tlv': _auto,
    'mle.tlv.link_status_sub_tlv': _auto,
    'mle.tlv.query_id': _auto,
    'mle.tlv.metric_type_id_flags.type': _list(_hex),
    'mle.tlv.metric_type_id_flags.metric': _list(_hex),
    'mle.tlv.metric_type_id_flags.l': _list(_hex),
    'mle.tlv.link_requested_type_id_flags': _bytes,

    # IP
    'ip.version': _auto,
    'ip.src': _str,
    'ip.src_host': _str,
    'ip.dst': _str,
    'ip.dst_host': _str,
    'ip.ttl': _auto,
    'ip.proto': _auto,
    'ip.len': _auto,
    'ip.id': _auto,
    'ip.host': _list(_str),
    'ip.hdr_len': _dec,
    'ip.frag_offset': _auto,
    'ip.flags.rb': _auto,
    'ip.flags.mf': _auto,
    'ip.flags.df': _auto,
    'ip.dsfield.ecn': _auto,
    'ip.dsfield.dscp': _auto,
    'ip.checksum.status': _auto,
    'ip.addr': _list(_str),
    'ip.options.routeralert': _bytes,
    'ip.opt.type.number': _auto,
    'ip.opt.type.copy': _auto,
    'ip.opt.type.class': _auto,
    'ip.opt.ra': _auto,
    'ip.opt.len': _auto,
    # UDP
    'udp.stream': _auto,
    'udp.srcport': _auto,
    'udp.dstport': _auto,
    'udp.length': _auto,
    'udp.port': _list(_dec),
    'udp.checksum.status': _auto,

    # IPv6
    'ipv6.version': _auto,
    'ipv6.src': _ipv6_addr,
    'ipv6.src_host': _ipv6_addr,
    'ipv6.dst': _ipv6_addr,
    'ipv6.dst_host': _ipv6_addr,
    'ipv6.addr': _list(_ipv6_addr),
    'ipv6.tclass.dscp': _auto,
    'ipv6.tclass.ecn': _auto,
    'ipv6.flow': _auto,
    'ipv6.hlim': _auto,
    'ipv6.nxt': _auto,
    'ipv6.hopopts.len': _auto,
    'ipv6.hopopts.nxt': _auto,
    'ipv6.hopopts.len_oct': _dec,
    'ipv6.host': _list(_ipv6_addr),
    'ipv6.plen': _auto,
    'ipv6.opt.type.rest': _list(_auto),
    'ipv6.opt.type.change': _list(_auto),
    'ipv6.opt.type.action': _list(_auto),
    'ipv6.opt.router_alert': _auto,
    'ipv6.opt.padn': _str,
    'ipv6.opt.length': _list(_auto),
    'ipv6.opt.mpl.seed_id': _bytes,
    'ipv6.opt.mpl.sequence': _auto,
    'ipv6.opt.mpl.flag.v': _auto,
    'ipv6.opt.mpl.flag.s': _auto,
    'ipv6.opt.mpl.flag.rsv': _auto,
    'ipv6.opt.mpl.flag.m': _auto,

    # Eth
    'eth.src': _eth_addr,
    'eth.src_resolved': _eth_addr,
    'eth.dst': _eth_addr,
    'eth.dst_resolved': _eth_addr,
    'eth.type': _auto,
    'eth.addr': _list(_eth_addr),
    'eth.addr_resolved': _list(_eth_addr),
    'eth.ig': _list(_auto),
    'eth.lg': _list(_auto),
    # 6LOWPAN
    '6lowpan.src': _ipv6_addr,
    '6lowpan.dst': _ipv6_addr,
    '6lowpan.udp.src': _auto,
    '6lowpan.udp.dst': _auto,
    '6lowpan.udp.checksum': _auto,
    '6lowpan.frag.offset': _auto,
    '6lowpan.frag.tag': _auto,
    '6lowpan.frag.size': _auto,
    '6lowpan.pattern': _list(_auto),
    '6lowpan.hops': _auto,
    '6lowpan.padding': _auto,
    '6lowpan.next': _auto,
    '6lowpan.flow': _auto,
    '6lowpan.ecn': _auto,
    '6lowpan.iphc.tf': _auto,
    '6lowpan.iphc.m': _auto,
    '6lowpan.iphc.nh': _auto,
    '6lowpan.iphc.hlim': _auto,
    '6lowpan.iphc.cid': _auto,
    '6lowpan.iphc.sac': _auto,
    '6lowpan.iphc.sam': _auto,
    '6lowpan.iphc.dac': _auto,
    '6lowpan.iphc.dam': _auto,
    '6lowpan.iphc.sci': _auto,
    '6lowpan.iphc.dci': _auto,
    '6lowpan.iphc.sctx.prefix': _ipv6_addr,
    '6lowpan.iphc.dctx.prefix': _ipv6_addr,
    '6lowpan.mesh.v': _auto,
    '6lowpan.nhc.pattern': _list(_auto),
    '6lowpan.nhc.udp.checksum': _auto,
    '6lowpan.nhc.udp.ports': _auto,
    '6lowpan.nhc.ext.nh': _auto,
    '6lowpan.nhc.ext.length': _auto,
    '6lowpan.nhc.ext.eid': _auto,
    '6lowpan.reassembled.length': _auto,
    '6lowpan.fragments': _str,
    '6lowpan.fragment.count': _auto,
    '6lowpan.mesh.orig16': _auto,
    '6lowpan.mesh.hops8': _auto,
    '6lowpan.mesh.hops': _auto,
    '6lowpan.mesh.f': _auto,
    '6lowpan.mesh.dest16': _auto,

    # ICMPv6
    'icmpv6.type': _first(_auto),
    'icmpv6.code': _first(_auto),
    'icmpv6.checksum': _first(_auto),
    'icmpv6.reserved': _raw_hex,
    'icmpv6.resptime': _float,
    'icmpv6.resp_to': _auto,
    'icmpv6.mldr.nb_mcast_records': _auto,
    'icmpv6.nd.ra.cur_hop_limit': _auto,
    'icmpv6.nd.ns.target_address': _ipv6_addr,
    'icmpv6.nd.na.target_address': _ipv6_addr,
    'icmpv6.nd.na.flag.s': _auto,
    'icmpv6.nd.na.flag.o': _auto,
    'icmpv6.nd.na.flag.r': _auto,
    'icmpv6.nd.na.flag.rsv': _auto,
    'icmpv6.mldr.mar.record_type': _list(_auto),
    'icmpv6.mldr.mar.aux_data_len': _list(_auto),
    'icmpv6.mldr.mar.nb_sources': _list(_auto),
    'icmpv6.mldr.mar.multicast_address': _list(_ipv6_addr),
    'icmpv6.opt.type': _list(_auto),
    'icmpv6.opt.nonce': _bytes,
    'icmpv6.opt.linkaddr': _eth_addr,
    'icmpv6.opt.src_linkaddr': _eth_addr,
    'icmpv6.opt.target_linkaddr': _eth_addr,
    'icmpv6.opt.route_lifetime': _auto,
    'icmpv6.opt.route_info.flag.route_preference': _auto,
    'icmpv6.opt.route_info.flag.reserved': _auto,
    'icmpv6.opt.prefix.valid_lifetime': _auto,
    'icmpv6.opt.prefix.preferred_lifetime': _auto,
    'icmpv6.opt.prefix.length': _list(_auto),
    'icmpv6.opt.prefix.flag.reserved': _auto,
    'icmpv6.opt.prefix.flag.r': _auto,
    'icmpv6.opt.prefix.flag.l': _auto,
    'icmpv6.opt.prefix.flag.a': _auto,
    'icmpv6.opt.length': _list(_auto),
    'icmpv6.opt.reserved': _str,
    'icmpv6.nd.ra.router_lifetime': _auto,
    'icmpv6.nd.ra.retrans_timer': _auto,
    'icmpv6.nd.ra.reachable_time': _auto,
    'icmpv6.nd.ra.flag.rsv': _auto,
    'icmpv6.nd.ra.flag.prf': _auto,
    'icmpv6.nd.ra.flag.p': _auto,
    'icmpv6.nd.ra.flag.o': _auto,
    'icmpv6.nd.ra.flag.m': _auto,
    'icmpv6.nd.ra.flag.h': _auto,
    'icmpv6.echo.sequence_number': _auto,
    'icmpv6.echo.identifier': _auto,
    'icmpv6.data.len': _auto,

    # COAP
    'coap.code': _auto,
    'coap.version': _auto,
    'coap.type': _auto,
    'coap.mid': _auto,
    'coap.token_len': _auto,
    'coap.token': _auto,
    'coap.opt.uri_path': _list(_str),
    'coap.opt.name': _list(_str),
    'coap.opt.length': _list(_auto),
    'coap.opt.uri_path_recon': _str,
    'coap.payload': _payload,
    'coap.payload_length': _auto,
    'coap.payload_desc': _str,
    'coap.opt.end_marker': _auto,
    'coap.opt.desc': _list(_str),
    'coap.opt.delta': _list(_auto),
    'coap.response_to': _auto,
    'coap.response_time': _float,
    # COAP TLVS
    'coap.tlv.type': _list(_auto),
    'coap.tlv.status': _auto,
    'coap.tlv.target_eid': _ipv6_addr,
    'coap.tlv.ml_eid': _ext_addr,
    'coap.tlv.last_transaction_time': _auto,
    'coap.tlv.rloc16': _auto,
    'coap.tlv.net_name': _str,
    'coap.tlv.ext_mac_addr': _ext_addr,
    'coap.tlv.router_mask_assigned': _auto,
    'coap.tlv.router_mask_id_seq': _auto,

    # dtls
    'dtls.handshake.type': _list(_auto),
    'dtls.handshake.cookie': _auto,
    'dtls.record.content_type': _list(_auto),
    'dtls.alert_message.desc': _auto,

    # thread beacon
    'thread_bcn.protocol': _auto,
    'thread_bcn.version': _auto,
    'thread_bcn.network_name': _str,
    'thread_bcn.epid': _ext_addr,

    # thread_address
    'thread_address.tlv.len': _list(_auto),
    'thread_address.tlv.type': _list(_auto),
    'thread_address.tlv.status': _auto,
    'thread_address.tlv.target_eid': _ipv6_addr,
    'thread_address.tlv.ext_mac_addr': _ext_addr,
    'thread_address.tlv.router_mask_id_seq': _auto,
    'thread_address.tlv.router_mask_assigned': _bytes,
    'thread_address.tlv.rloc16': _hex,
    'thread_address.tlv.target_eid': _ipv6_addr,
    'thread_address.tlv.ml_eid': _ext_addr,

    # thread bl
    'thread_bl.tlv.type': _list(_auto),
    'thread_bl.tlv.len': _list(_auto),
    'thread_bl.tlv.target_eid': _ipv6_addr,
    'thread_bl.tlv.ml_eid': _ext_addr,
    'thread_bl.tlv.last_transaction_time': _auto,
    'thread_bl.tlv.timeout': _auto,
    # THEAD NM
    'thread_nm.tlv.type': _list(_auto),
    'thread_nm.tlv.ml_eid': _ext_addr,
    'thread_nm.tlv.target_eid': _ipv6_addr,
    'thread_nm.tlv.status': _auto,
    'thread_nm.tlv.timeout': _auto,
    # thread_meshcop is not a real layer
    'thread_meshcop.len_size_mismatch': _str,
    'thread_meshcop.tlv.type': _list(_auto),
    'thread_meshcop.tlv.len8': _list(_auto),
    'thread_meshcop.tlv.net_name': _list(_str),  # from thread_bl
    'thread_meshcop.tlv.commissioner_id': _str,
    'thread_meshcop.tlv.commissioner_sess_id': _auto,  # from mle
    "thread_meshcop.tlv.channel_page": _auto,  # from ble
    "thread_meshcop.tlv.channel": _list(_auto),  # from ble
    "thread_meshcop.tlv.chan_mask": _str,  # from ble
    'thread_meshcop.tlv.chan_mask_page': _auto,
    'thread_meshcop.tlv.chan_mask_len': _auto,
    'thread_meshcop.tlv.chan_mask_mask': _bytes,
    'thread_meshcop.tlv.discovery_req_ver': _auto,
    'thread_meshcop.tlv.discovery_rsp_ver': _auto,
    'thread_meshcop.tlv.discovery_rsp_n': _auto,
    'thread_meshcop.tlv.energy_list': _list(_auto),
    'thread_meshcop.tlv.pan_id': _list(_auto),
    'thread_meshcop.tlv.xpan_id': _bytes,
    'thread_meshcop.tlv.ml_prefix': _bytes,
    'thread_meshcop.tlv.master_key': _bytes,
    'thread_meshcop.tlv.pskc': _bytes,
    'thread_meshcop.tlv.sec_policy_rot': _auto,
    'thread_meshcop.tlv.sec_policy_o': _auto,
    'thread_meshcop.tlv.sec_policy_n': _auto,
    'thread_meshcop.tlv.sec_policy_r': _auto,
    'thread_meshcop.tlv.sec_policy_c': _auto,
    'thread_meshcop.tlv.sec_policy_b': _auto,
    'thread_meshcop.tlv.state': _auto,
    'thread_meshcop.tlv.steering_data': _bytes,
    'thread_meshcop.tlv.unknown': _bytes,
    'thread_meshcop.tlv.udp_port': _list(_auto),
    'thread_meshcop.tlv.ba_locator': _auto,
    'thread_meshcop.tlv.jr_locator': _auto,
    'thread_meshcop.tlv.active_tstamp': _auto,
    'thread_meshcop.tlv.pending_tstamp': _auto,
    'thread_meshcop.tlv.delay_timer': _auto,
    'thread_meshcop.tlv.ipv6_addr': _list(_ipv6_addr),

    # THREAD NWD
    'thread_nwd.tlv.type': _list(_auto),
    'thread_nwd.tlv.len': _list(_auto),
    'thread_nwd.tlv.stable': _list(_auto),
    'thread_nwd.tlv.service.t': _auto,
    'thread_nwd.tlv.service.s_id': _auto,
    'thread_nwd.tlv.service.s_data_len': _auto,
    'thread_nwd.tlv.service.s_data.seqno': _auto,
    'thread_nwd.tlv.service.s_data.rrdelay': _auto,
    'thread_nwd.tlv.service.s_data.mlrtimeout': _auto,
    'thread_nwd.tlv.server_16': _list(_auto),
    'thread_nwd.tlv.border_router_16': _list(_auto),
    'thread_nwd.tlv.sub_tlvs': _list(_str),
    # TODO: support thread_nwd.tlv.prefix.length and thread_nwd.tlv.prefix.domain_id
    'thread_nwd.tlv.prefix': _list(_ipv6_addr),
    'thread_nwd.tlv.border_router.pref': _auto,
    'thread_nwd.tlv.border_router.flag.s': _list(_auto),
    'thread_nwd.tlv.border_router.flag.r': _list(_auto),
    'thread_nwd.tlv.border_router.flag.p': _list(_auto),
    'thread_nwd.tlv.border_router.flag.o': _list(_auto),
    'thread_nwd.tlv.border_router.flag.n': _list(_auto),
    'thread_nwd.tlv.border_router.flag.dp': _list(_auto),
    'thread_nwd.tlv.border_router.flag.d': _list(_auto),
    'thread_nwd.tlv.border_router.flag.c': _list(_auto),
    'thread_nwd.tlv.6co.flag.reserved': _auto,
    'thread_nwd.tlv.6co.flag.cid': _auto,
    'thread_nwd.tlv.6co.flag.c': _list(_auto),
    'thread_nwd.tlv.6co.context_length': _auto,

    # Thread Diagnostic
    'thread_diagnostic.tlv.type': _list(_auto),
    'thread_diagnostic.tlv.len8': _list(_auto),
    'thread_diagnostic.tlv.general': _list(_str)
}

_layer_containers = set()

for key in _LAYER_FIELDS:
    assert key.strip() == key and ' ' not in key, key
    secs = key.split('.')
    assert len(secs) >= 2
    assert secs[0] in VALID_LAYER_NAMES, secs[0]
    for i in range(len(secs) - 2):
        path = secs[0] + '.' + '.'.join(secs[1:i + 2])
        assert path not in _LAYER_FIELDS, '%s can not be both field and path' % path
        _layer_containers.add(path)


def is_layer_field(uri: str) -> bool:
    """
    Returns if the URI is a valid layer field.

    :param uri: The layer field URI.
    """
    return uri in _LAYER_FIELDS


def is_layer_field_container(uri: str) -> bool:
    """
    Returns if the URI is a valid layer field container.

    :param uri: The layer field container URI.
    """
    return uri in _layer_containers


def get_layer_field(packet: RawPacket, field_uri: str) -> Any:
    """
    Get a given layer field from the packet.

    :param packet: The packet.
    :param field_uri: The layer field URI.

    :return: The specified layer field.
    """
    assert isinstance(packet, RawPacket)
    secs = field_uri.split('.')
    layer_depth = 0
    layer_name = secs[0]
    if layer_name.endswith('inner'):
        layer_name = layer_name[:-len('inner')]
        field_uri = '.'.join([layer_name] + secs[1:])
        layer_depth = 1

    if is_layer_field(field_uri):
        candidate_layers = _get_candidate_layers(packet, layer_name)
        for layers in candidate_layers:
            if layer_depth >= len(layers):
                continue
            layer = layers[layer_depth]
            v = layer.get_field(field_uri)
            if v is not None:
                try:
                    v = _LAYER_FIELDS[field_uri](v)
                    print("[%s = %r] " % (field_uri, v), file=sys.stderr)
                    return v
                except Exception as ex:
                    raise ValueError('can not parse field %s = %r' % (field_uri,
                                                                      (v.get_default_value(), v.raw_value))) from ex

        print("[%s = %s] " % (field_uri, "null"), file=sys.stderr)
        return nullField

    elif is_layer_field_container(field_uri):
        from pktverify.layer_fields_container import LayerFieldsContainer
        return LayerFieldsContainer(packet, field_uri)
    else:
        raise NotImplementedError('Field %s is not valid, please add it to `_LAYER_FIELDS`' % field_uri)


def check_layer_field_exists(packet, field_uri):
    """
    Check if a given layer field URI exists in the packet.

    :param packet: The packet to check.
    :param field_uri: The layer field URI.
    :return: Whether the layer field URI exists in the packet.
    """
    assert isinstance(packet, RawPacket)
    secs = field_uri.split('.')
    layer_name = secs[0]

    if not is_layer_field(field_uri) and not is_layer_field_container(field_uri):
        raise NotImplementedError('%s is neither a field or field container' % field_uri)

    candidate_layers = _get_candidate_layers(packet, layer_name)
    for layers in candidate_layers:
        for layer in layers:
            for k, v in layer._all_fields.items():
                if k == field_uri or k.startswith(field_uri + '.'):
                    return True

    return False


def _get_candidate_layers(packet, layer_name):
    if layer_name == 'thread_meshcop':
        candidate_layer_names = ['thread_meshcop', 'mle', 'coap', 'thread_bl', 'thread_nm']
    elif layer_name == 'thread_nwd':
        candidate_layer_names = ['mle', 'thread_address', 'thread_diagnostic']
    elif layer_name == 'wpan':
        candidate_layer_names = ['wpan', 'mle']
    elif layer_name == 'ip':
        candidate_layer_names = ['ip', 'ipv6']
    elif layer_name == 'thread_bcn':
        candidate_layer_names = ['thread_bcn']
    else:
        candidate_layer_names = [layer_name]

    layers = []
    for ln in candidate_layer_names:
        if hasattr(packet, ln):
            layers.append(packet.get_multiple_layers(ln))

    return layers
