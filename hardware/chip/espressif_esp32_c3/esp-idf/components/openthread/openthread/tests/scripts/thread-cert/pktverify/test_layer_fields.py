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
# This is a test script for checking layer fields against a given test.pcap.
#

import logging
import unittest

from pktverify import layer_fields
from pktverify.addrs import EthAddr, ExtAddr, Ipv6Addr
from pktverify.bytes import Bytes
from pktverify.consts import REAL_LAYER_NAMES, VALID_LAYER_NAMES
from pktverify.layer_fields_container import LayerFieldsContainer
from pktverify.null_field import nullField
from pktverify.packet import Packet
from pktverify.pcap_reader import PcapReader


class TestLayerFields(unittest.TestCase):

    def test(self):
        logging.basicConfig(level=logging.DEBUG)
        pkts = PcapReader.read("test.pcap")

        for i, p in enumerate(pkts):

            logging.info("check packet #%d", i + 1)

            for layer_name in VALID_LAYER_NAMES:
                if layer_name == 'lowpan':  # we already checked 6lowpan
                    continue

                layer = getattr(p, layer_name)
                if hasattr(p._packet, layer_name):
                    if layer_name in REAL_LAYER_NAMES:
                        self.assertTrue(layer)
                    checker = getattr(self, '_test_' + layer_name, None)
                    if checker is None:
                        continue

                    try:
                        checker(p)
                    except Exception:
                        layer.show()
                        raise
                else:
                    if layer_name in REAL_LAYER_NAMES:
                        self.assertFalse(layer)

            for layer in p._packet.layers:
                self._check_missing_fields(p, layer.layer_name, layer)

    def _test_coap(self, p):
        coap = p.coap
        self.assertIsInstance(coap.version, int)
        self.assertIsInstance(coap.type, int)
        self.assertIsInstance(coap.token_len, int)
        self.assertIsInstance(coap.code, int)
        self.assertIsInstance(coap.mid, int)
        self.assertIsInstanceOrNull(coap.token, int)
        self.assertIsInstanceOrNull(coap.opt.uri_path_recon, str)
        self.assertIsInstanceOrNull(coap.payload, bytearray)

        print(p.coap.tlv.type, p.coap.tlv)
        assert isinstance(coap.tlv, LayerFieldsContainer), repr(coap.tlv)
        self.assertIsInstanceOrNull(coap.tlv.type, list)

    def _test_mle(self, p):
        mle = p.mle

        self._must_have_wpan_aux_sec(p)

        self.assertIsInstance(mle.cmd, int)
        self.assertIsInstanceOrNull(mle.tlv.mode.receiver_on_idle, int)
        self.assertIsInstanceOrNull(mle.tlv.mode.reserved1, int)
        self.assertIsInstanceOrNull(mle.tlv.mode.reserved2, int)
        self.assertIsInstanceOrNull(mle.tlv.mode.device_type_bit, int)
        self.assertIsInstanceOrNull(mle.tlv.mode.network_data, int)
        self.assertIsInstanceOrNull(mle.tlv.challenge, Bytes)
        self.assertIsInstanceOrNull(mle.tlv.scan_mask.r, int)
        self.assertIsInstanceOrNull(mle.tlv.scan_mask.e, int)
        self.assertIsInstanceOrNull(mle.tlv.version, int)
        self.assertIsInstanceOrNull(mle.tlv.source_addr, int)
        self.assertIsInstanceOrNull(mle.tlv.active_tstamp, int)
        self.assertIsInstanceOrNull(mle.tlv.leader_data.partition_id, int)
        self.assertIsInstanceOrNull(mle.tlv.leader_data.weighting, int)
        self.assertIsInstanceOrNull(mle.tlv.leader_data.data_version, int)
        self.assertIsInstanceOrNull(mle.tlv.leader_data.stable_data_version, int)
        self.assertIsInstanceOrNull(mle.tlv.leader_data.router_id, int)

    def _test_wpan(self, p):
        wpan = p.wpan
        self.assertIsInstance(wpan.fcf, int)
        self.assertIsInstance(wpan.fcs, int)
        self.assertIsInstance(wpan.security, int)
        self.assertIsInstance(wpan.pending, int)
        self.assertIsInstance(wpan.ack_request, int)
        self.assertIsInstance(wpan.pan_id_compression, int)
        self.assertIsInstance(wpan.seqno_suppression, int)
        self.assertIsInstance(wpan.ie_present, int)
        self.assertIsInstance(wpan.dst_addr_mode, int)
        self.assertIsInstance(wpan.version, int)
        self.assertIsInstance(wpan.src_addr_mode, int)

        self.assertIsInstance(wpan.seq_no, int)

        if not wpan.is_ack:
            self.assertIsInstanceOrNull(wpan.dst_pan, int)
            self.assertIsInstanceOrNull(wpan.dst16, int)
            self.assertIsInstanceOrNull(wpan.src16, int)
            self.assertIsInstanceOrNull(wpan.src64, ExtAddr)
            self.assertIsInstanceOrNull(wpan.dst64, ExtAddr)

        if wpan.aux_sec:
            self._must_have_wpan_aux_sec(p)

    def _must_have_wpan_aux_sec(self, p):
        wpan = p.wpan
        self.assertIsInstanceOrNull(wpan.aux_sec.sec_suite, int)
        self.assertIsInstanceOrNull(wpan.aux_sec.security_control_field, int)
        self.assertIsInstanceOrNull(wpan.aux_sec.sec_level, int)
        self.assertIsInstanceOrNull(wpan.aux_sec.key_id_mode, int)
        self.assertIsInstanceOrNull(wpan.aux_sec.frame_counter_suppression, int)
        self.assertIsInstanceOrNull(wpan.aux_sec.asn_in_nonce, int)
        self.assertIsInstanceOrNull(wpan.aux_sec.reserved, int)
        self.assertIsInstanceOrNull(wpan.aux_sec.frame_counter, int)
        self.assertIsInstanceOrNull(wpan.aux_sec.key_source, int)
        self.assertIsInstanceOrNull(wpan.aux_sec.key_index, int)

    def assertIsInstanceOrNull(self, field, type):
        if field is not nullField:
            self.assertIsInstance(field, type)

    def _test_thread_bl(self, p):
        thread_bl = p.thread_bl
        self.assertTrue(thread_bl)

        self.assertIsInstanceOrNull(thread_bl.tlv.target_eid, Ipv6Addr)
        self.assertIsInstanceOrNull(thread_bl.tlv.ml_eid, ExtAddr)
        self.assertIsInstanceOrNull(thread_bl.tlv.last_transaction_time, int)
        self.assertIsInstanceOrNull(p.thread_meshcop.tlv.net_name, list)

    def _test_thread_meshcop(self, p: Packet):
        thread_meshcop = p.thread_meshcop

        for layer in sorted(p.layers, key=lambda l: l.layer_name):
            if 'thread_meshcop.tlv.commissioner_sess_id' in layer._layer._all_fields:
                self.assertIsInstance(thread_meshcop.tlv.commissioner_sess_id, int)

            if 'thread_meshcop.tlv.net_name' in layer._layer._all_fields:
                self.assertIsInstance(thread_meshcop.tlv.net_name, list)

            if 'thread_meshcop.tlv.channel_page' in layer._layer._all_fields:
                self.assertIsInstance(thread_meshcop.tlv.channel_page, int)

            if 'thread_meshcop.tlv.channel' in layer._layer._all_fields:
                self.assertIsInstance(thread_meshcop.tlv.channel, list)

            if 'thread_meshcop.tlv.chan_mask_page' in layer._layer._all_fields:
                self.assertIsInstance(thread_meshcop.tlv.chan_mask_page, int)
            if 'thread_meshcop.tlv.chan_mask_len' in layer._layer._all_fields:
                self.assertIsInstance(thread_meshcop.tlv.chan_mask_len, int)
            if 'thread_meshcop.tlv.chan_mask_mask' in layer._layer._all_fields:
                self.assertIsInstance(thread_meshcop.tlv.chan_mask_mask, int)

            if 'thread_meshcop.tlv.panid' in layer._layer._all_fields:
                self.assertIsInstance(thread_meshcop.tlv.panid, int)

            if 'thread_meshcop.tlv.ml_prefix' in layer._layer._all_fields:
                self.assertIsInstance(thread_meshcop.tlv.ml_prefix, Bytes)

            if 'thread_meshcop.tlv.master_key' in layer._layer._all_fields:
                self.assertIsInstance(thread_meshcop.tlv.master_key, Bytes)

            if 'thread_meshcop.tlv.pskc' in layer._layer._all_fields:
                self.assertIsInstance(thread_meshcop.tlv.pskc, Bytes)

            if 'thread_meshcop.tlv.sec_policy_rot' in layer._layer._all_fields:
                self.assertIsInstance(thread_meshcop.tlv.sec_policy_rot, int)

            if 'thread_meshcop.tlv.sec_policy_o' in layer._layer._all_fields:
                self.assertIsInstance(thread_meshcop.tlv.sec_policy_o, int)

            if 'thread_meshcop.tlv.sec_policy_n' in layer._layer._all_fields:
                self.assertIsInstance(thread_meshcop.tlv.sec_policy_n, int)

            if 'thread_meshcop.tlv.sec_policy_r' in layer._layer._all_fields:
                self.assertIsInstance(thread_meshcop.tlv.sec_policy_r, int)

            if 'thread_meshcop.tlv.sec_policy_c' in layer._layer._all_fields:
                self.assertIsInstance(thread_meshcop.tlv.sec_policy_c, int)

            if 'thread_meshcop.tlv.sec_policy_b' in layer._layer._all_fields:
                self.assertIsInstance(thread_meshcop.tlv.sec_policy_b, int)

            if 'thread_meshcop.tlv.pan_id' in layer._layer._all_fields:
                self.assertIsInstance(thread_meshcop.tlv.pan_id, list)

            if 'thread_meshcop.tlv.xpan_id' in layer._layer._all_fields:
                self.assertIsInstance(thread_meshcop.tlv.xpan_id, Bytes)

            for field in layer._layer._all_fields:
                if field.startswith('thread_meshcop') and not layer_fields.is_layer_field(field) and field not in (
                        'thread_meshcop.tlv', 'thread_meshcop.tlv.type', 'thread_meshcop.tlv.len8'):
                    print('found %s = %s in layer %s' % (
                        field,
                        layer._layer.get_field(field),
                        layer.layer_name,
                    ))

    def _test_icmpv6(self, p):
        icmpv6 = p.icmpv6
        self.assertTrue(p.icmpv6)

        self.assertIsInstance(icmpv6.type, int)
        self.assertIsInstance(icmpv6.code, int)
        self.assertIsInstance(icmpv6.checksum, int)
        self.assertIsInstanceOrNull(icmpv6.reserved, int)
        self.assertIsInstanceOrNull(icmpv6.nd.na.flag.s, int)
        self.assertIsInstanceOrNull(icmpv6.nd.na.flag.o, int)
        self.assertIsInstanceOrNull(icmpv6.nd.na.flag.r, int)
        self.assertIsInstanceOrNull(icmpv6.nd.na.flag.rsv, int)
        self.assertIsInstanceOrNull(icmpv6.nd.ra.cur_hop_limit, int)
        self.assertIsInstanceOrNull(icmpv6.mldr.nb_mcast_records, int)
        self.assertIsInstanceOrNull(icmpv6.nd.ns.target_address, Ipv6Addr)
        self.assertIsInstanceOrNull(icmpv6.mldr.mar.multicast_address, list)

    def get_field(self, p: Packet, f):
        secs = f.split('.')
        assert len(secs) >= 2
        v = p
        for sec in secs:
            v = getattr(v, sec)

        return v

    def _test_6lowpan(self, p):
        lowpan = p.lowpan
        assert lowpan is getattr(p, '6lowpan')
        self.assertIsInstanceOrNull(lowpan.src, Ipv6Addr)
        self.assertIsInstanceOrNull(lowpan.dst, Ipv6Addr)
        self.assertIsInstanceOrNull(lowpan.udp.src, int)
        self.assertIsInstanceOrNull(lowpan.udp.dst, int)
        self.assertIsInstanceOrNull(lowpan.udp.checksum, int)
        self.assertIsInstanceOrNull(lowpan.frag.size, int)
        self.assertIsInstanceOrNull(lowpan.frag.tag, int)
        self.assertIsInstanceOrNull(lowpan.frag.offset, int)
        self.assertIsInstanceOrNull(lowpan.nhc.pattern, list)
        self.assertIsInstanceOrNull(lowpan.nhc.udp.checksum, int)
        self.assertIsInstanceOrNull(lowpan.nhc.udp.ports, int)

        self.assertIsInstanceOrNull(lowpan.pattern, list)
        self.assertIsInstanceOrNull(lowpan.iphc.tf, int)
        self.assertIsInstanceOrNull(lowpan.iphc.nh, int)
        self.assertIsInstanceOrNull(lowpan.iphc.hlim, int)
        self.assertIsInstanceOrNull(lowpan.iphc.cid, int)
        self.assertIsInstanceOrNull(lowpan.iphc.sac, int)
        self.assertIsInstanceOrNull(lowpan.iphc.sam, int)
        self.assertIsInstanceOrNull(lowpan.iphc.m, int)
        self.assertIsInstanceOrNull(lowpan.iphc.dac, int)
        self.assertIsInstanceOrNull(lowpan.iphc.dam, int)
        self.assertIsInstanceOrNull(lowpan.iphc.sctx.prefix, Bytes)
        self.assertIsInstanceOrNull(lowpan.iphc.dctx.prefix, Bytes)

    def _test_ip(self, p):
        pass

    def _test_ipv6(self, p):
        pass

    def _test_udp(self, p):
        pass

    def _test_eth(self, p):
        eth = p.eth
        self.assertIsInstance(eth.src, EthAddr)
        self.assertIsInstance(eth.dst, EthAddr)
        self.assertIsInstance(eth.type, int)

    def _check_missing_fields(self, p, layer_name, _layer):
        for f in sorted(_layer._all_fields.keys(), reverse=True):
            if f.startswith('_ws') or f.startswith('data'):
                continue

            logging.info('_check_missing_fields in layer %s: %s = %r' % (layer_name, f, _layer._all_fields[f]))
            if f in {
                    '', 'icmpv6.checksum.status', 'ip.ttl.lncb', 'wpan.aux_sec.key_source.bytes', 'wpan.src64.origin'
            }:
                # TODO: handle these fields
                continue

            v = _layer._all_fields[f]

            if layer_fields.is_layer_field_container(f):
                continue

            try:
                rv = self.get_field(p, f)
                self.assertIsNot(rv, nullField)

                parser = layer_fields._LAYER_FIELDS[f]
                if isinstance(parser, layer_fields._first):
                    parser = parser._sub_parse

                if parser in (layer_fields._raw_hex, layer_fields._hex, layer_fields._raw_hex_rev, layer_fields._dec,
                              layer_fields._auto):
                    self.assertIsInstance(rv, int)
                elif isinstance(parser, layer_fields._list):
                    self.assertIsInstance(rv, list)
                elif parser is layer_fields._ipv6_addr:
                    self.assertIsInstance(rv, Ipv6Addr)
                elif parser is layer_fields._eth_addr:
                    self.assertIsInstance(rv, EthAddr)
                elif parser is layer_fields._ext_addr:
                    self.assertIsInstance(rv, ExtAddr)
                elif parser is layer_fields._str:
                    self.assertIsInstance(rv, str)
                elif parser is layer_fields._bytes:
                    self.assertIsInstance(rv, Bytes)
                elif parser is layer_fields._payload:
                    self.assertIsInstance(rv, bytearray)
                elif parser is layer_fields._float:
                    self.assertIsInstance(rv, float)
                else:
                    raise NotImplementedError(parser)
            except Exception:
                logging.info('checking [%s] %s=%r, %r, %r (%d)' %
                             (layer_name, f, v, v.get_default_value(), v.raw_value, len(v.fields)))
                raise
