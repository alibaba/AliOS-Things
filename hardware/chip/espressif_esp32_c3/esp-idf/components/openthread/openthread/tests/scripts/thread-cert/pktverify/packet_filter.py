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

import logging
import sys
from operator import attrgetter
from typing import Optional, Callable, Tuple, Union

from pktverify import consts, errors
from pktverify.addrs import EthAddr, ExtAddr, Ipv6Addr
from pktverify.bytes import Bytes
from pktverify.packet import Packet
from pktverify.utils import make_filter_func

WPAN, ETH = 0, 1


class _SavedIndex(object):
    __slots__ = ('_pkts', '_saved_index')

    def __init__(self, pkts):
        self._pkts = pkts
        self._saved_index = pkts.index

    def __enter__(self):
        pass

    def __exit__(self, exc_type, exc_val, exc_tb):
        self._pkts.index = self._saved_index


def _always_true(p):
    return True


class PacketFilter(object):
    """
    Represents a range of packets that are filtered by given filter
    """

    def __init__(self,
                 pkts,
                 start=(0, 0),
                 stop=None,
                 *,
                 index=None,
                 filter_func: Optional[Callable] = None,
                 parent: Optional['PacketFilter'] = None):
        if stop is None:
            stop = (len(pkts), len(pkts))

        self._pkts = pkts
        self._start_index = start
        self._stop_index = stop
        self._index = index if index is not None else self._start_index
        self._last_index = -1
        self._filter_func = filter_func or _always_true
        self._parent = parent
        self._check_type_ok()

    def _check_type_ok(self):
        assert self._last_index == -1 or 0 <= self._last_index < len(self._pkts)

        assert isinstance(self._start_index, tuple) and len(self._start_index) == 2, self._start_index
        assert isinstance(self._stop_index, tuple) and len(self._stop_index) == 2, self._stop_index
        assert isinstance(self._index, tuple) and len(self._index) == 2, self._index
        self._check_idx_range_ok((0, 0), self._start_index)
        self._check_idx_range_ok(self._start_index, self._index)
        self._check_idx_range_ok(self._index, self._stop_index)
        self._check_idx_range_ok(self._stop_index, (len(self._pkts), len(self._pkts)))

    def _check_idx_range_ok(self, start, stop):
        assert start[0] <= stop[0], (start, stop)
        assert start[1] <= stop[1], (start, stop)

    @property
    def index(self) -> Tuple[int, int]:
        """
        :return: the current index (which is a tuple)
        """
        return self._index

    @index.setter
    def index(self, index: Tuple[int, int]):
        """
        Set the current index
        :param index: the index tuple to set
        """
        assert isinstance(index, tuple) and len(index) == 2, index
        self._check_type_ok()
        self._index = index
        self._check_type_ok()

    def __len__(self):
        """
        :return: length of packets
        """
        return len(self._pkts)

    def save_index(self):
        """
        Save the current index to be restored.

        :return: a context that saves the current index when entering, and restores when exiting
        """
        return _SavedIndex(self)

    @property
    def start_index(self) -> Tuple[int, int]:
        """
        :return: the start index tuple
        """
        return self._start_index

    @property
    def stop_index(self) -> Tuple[int, int]:
        """
        :return: the stop index tuple
        """
        return self._stop_index

    def filter(self, func, cascade=True, **vars) -> 'PacketFilter':
        """
        Create a new PacketFilter based on this packet filter with given filter func

        :param func: a callable that returns a bool (e.x. lambda p: xxx) or a filter string
        :param cascade: True if calling next in the new filter will also set index for this filter, False otherwise
        :param vars: variables for filter string
        :return: a new PacketFilter
        """
        print('\n>>> filtering in range %s~%s%s:' %
              (self._index, self._stop_index, "<end>" if self._stop_index == len(self._pkts) else "<stop>"),
              file=sys.stderr)

        func = make_filter_func(func, **vars)
        self._check_type_ok()
        return PacketFilter(self._pkts,
                            self._index,
                            self._stop_index,
                            filter_func=lambda p: self._filter_func(p) and func(p),
                            parent=self if cascade else None)

    def filter_if(self, cond: bool, *args, **kwargs) -> 'PacketFilter':
        """
        Create a filter using given arguments if `cond` is true.

        :param cond: the condition to be checked
        :param args: arguments for filter func
        :param kwargs: arguments for filter func
        :return: a sub filter using given arguments if cond is true, or self otherwise
        """
        if cond:
            return self.filter(*args, **kwargs)
        else:
            return self

    @property
    def last_index(self) -> Tuple[int, int]:
        return self._last_index

    def last(self) -> Packet:
        """
        :return: the last packet found
        """
        if self._last_index >= 0:
            return self._pkts[self._last_index]
        else:
            raise errors.PacketNotFound(self.index, self._stop_index)

    def next(self) -> Optional[Packet]:
        """
        Find the next packet starting from the current index to the stop index that matches the current filter.

        :return: the next matching packet, or None if packet not found
        """
        self._check_type_ok()
        idx = min(self._index)
        stop_idx = max(self._stop_index)

        while idx < stop_idx:
            p = self._pkts[idx]

            sys.stderr.write('#%d %s' % (idx + 1, '\n' if idx % 40 == 39 else ''))
            if self._filter_func(p):
                if p.wpan and not (self._index[0] <= idx < self._stop_index[0]):  # wpan matched but not in range
                    pass
                elif p.eth and not (self._index[1] <= idx < self._stop_index[1]):  # eth matched but not in range
                    pass
                else:
                    self._on_found_next(idx, p)
                    print("\n>>> found packet at #%d!" % (idx + 1,), file=sys.stderr)
                    return p

            idx += 1

        return None

    def must_next(self) -> Packet:
        """
        Call .next(), raise error if packet is not found.

        :return: the next matching packet
        """
        p = self.next()
        if p is not None:
            return p
        else:
            raise errors.PacketNotFound(self.index, self._stop_index)

    def must_not_next(self) -> None:
        """
        Call .next(), raise error if packet is found
        """
        p = self.next()
        if p is None:
            return
        else:
            logging.error("Found unexpected packet at #%s", self.index)
            p.show()
            p.debug_fields()
            raise errors.UnexpectedPacketFound(self.index, p)

    def _on_found_next(self, idx: int, p: Packet):
        assert self._pkts[idx] is p
        assert idx >= min(self._index)
        assert not p.wpan or idx >= self._index[0]
        assert not p.eth or idx >= self._index[1], (self._index, idx)

        if p.wpan:
            wpan_idx = idx + 1
            eth_idx = max(self._index[1],
                          self._find_prev_packet(idx + 1, p.sniff_timestamp - consts.AUTO_SEEK_BACK_MAX_DURATION, ETH))
        else:
            eth_idx = idx + 1
            wpan_idx = max(
                self._index[0],
                self._find_prev_packet(idx + 1, p.sniff_timestamp - consts.AUTO_SEEK_BACK_MAX_DURATION, WPAN))

        # make sure index never go back
        assert wpan_idx >= self._index[0]
        assert eth_idx >= self._index[1]

        print('\n>>>_on_found_next %d %s => %s' % (idx, self._index, (wpan_idx, eth_idx)), file=sys.stderr)
        self._set_found_index(idx, (wpan_idx, eth_idx))

    def _find_prev_packet(self, idx, min_sniff_timestamp, pkttype):
        assert pkttype in (WPAN, ETH)

        prev_idx = idx
        while idx > 0 and self._pkts[idx - 1].sniff_timestamp >= min_sniff_timestamp:
            idx -= 1
            if pkttype == WPAN and self._pkts[idx].wpan:
                prev_idx = idx
            elif pkttype == ETH and self._pkts[idx].eth:
                prev_idx = idx

        return prev_idx

    def __iter__(self):
        for pkt in self._pkts:
            yield pkt

    def range(self, start, stop=None, cascade=True) -> 'PacketFilter':
        """
        Create a new PacketFilter using the specified start and stop index tuples

        :param start: the new start index tuple
        :param stop: the new stop index tuple
        :param cascade: True if calling next in the new filter will also set index for this filter, False otherwise
        :return: a new PacketFilter with new start and stop range
        """

        if stop is None:
            stop = self._stop_index

        assert self._start_index <= start <= self._stop_index
        assert self._start_index <= stop <= self._stop_index
        return PacketFilter(self._pkts, start, stop, filter_func=self._filter_func, parent=self if cascade else None)

    def copy(self) -> 'PacketFilter':
        """
        :return: a copy of the current PacketFilter
        """
        return PacketFilter(self._pkts, self._index, self._stop_index, filter_func=self._filter_func, parent=None)

    def __getitem__(self, index: int) -> Packet:
        """
        :param index: the packet index (not tuple!)
        :return: the packet at the specified index
        """
        assert isinstance(index, int), index
        return self._pkts[index]

    def seek_back(self, max_duration: float, *, eth=False, wpan=False) -> 'PacketFilter':
        """
        Move the current index back in time within the specified max duration. Either eth or wpan must be True.

        :param max_duration: the max duration to move back
        :param eth: True if eth index can be moved back
        :param wpan: True if wpan index can be moved back
        :return: self
        """
        assert eth or wpan, "must have eth or wpan"

        wpan_idx = self._index[0]
        if wpan and wpan_idx < len(self._pkts):
            wpan_idx = self._find_prev_packet(wpan_idx, self._pkts[wpan_idx].sniff_timestamp - max_duration, WPAN)
            wpan_idx = max(self._start_index[0], wpan_idx)

        eth_idx = self._index[1]
        if eth and eth_idx < len(self._pkts):
            eth_idx = self._find_prev_packet(eth_idx, self._pkts[eth_idx].sniff_timestamp - max_duration, ETH)
            eth_idx = max(self._start_index[1], eth_idx)

        print("\n>>> back %s wpan=%s, eth=%s: index %s => %s" % (max_duration, wpan, eth, self._index,
                                                                 (wpan_idx, eth_idx)),
              file=sys.stderr)
        self._index = (wpan_idx, eth_idx)
        self._check_type_ok()
        return self

    def _set_found_index(self, last_index: Tuple[int, int], index: Tuple[int, int]):
        self._last_index = last_index
        self._index = index
        self._check_type_ok()

        if self._parent is not None:
            self._parent._set_found_index(last_index, index)

    def filter_mle_advertisement(self, role: str, **kwargs):
        assert role in ('Leader', 'Router', 'REED'), role

        tlv_set = {consts.LEADER_DATA_TLV, consts.SOURCE_ADDRESS_TLV}

        if role != 'REED':
            tlv_set.add(consts.ROUTE64_TLV)

        return self.filter_LLANMA(). \
            filter_mle_cmd(consts.MLE_ADVERTISEMENT). \
            filter(lambda p: tlv_set ==
                             set(p.mle.tlv.type) and \
                             p.ipv6.hlim == 255, **kwargs
                   )

    def filter_coap(self, **kwargs):
        """
        Create a new PacketFilter to filter COAP packets.

        :param kwargs: Extra arguments for `filter`.
        :return: The new PacketFilter to filter COAP packets.
        """
        return self.filter(attrgetter('coap'), **kwargs)

    def filter_coap_request(self, uri_path, port=None, confirmable=None, **kwargs):
        """
        Create a new PacketFilter to filter COAP Request packets.

        :param uri_path:  The COAP URI path to filter.
        :param port: The UDP port to filter if specified.
        :param kwargs: Extra arguments for `filter`.
        :return: The new PacketFilter to filter COAP Request packets.
        """
        assert isinstance(uri_path, str), uri_path
        assert port is None or isinstance(port, int), port
        return self.filter(
            lambda p: (p.coap.is_post and p.coap.opt.uri_path_recon == uri_path and
                       (confirmable is None or p.coap.type ==
                        (0 if confirmable else 1)) and (port is None or p.udp.dstport == port)), **kwargs)

    def filter_coap_ack(self, uri_path, port=None, **kwargs):
        """
        Create a new PacketFilter for filter COAP ACK packets.

        :param uri_path:  The COAP URI path to filter.
        :param port: The UDP port to filter if specified.
        :param kwargs: Extra arguments for `filter`.
        :return: The new PacketFilter to filter COAP ACK packets.
        """
        assert isinstance(uri_path, str), uri_path
        assert port is None or isinstance(port, int), port
        return self.filter(
            lambda p: (p.coap.is_ack and p.coap.opt.uri_path_recon == uri_path and
                       (port is None or p.udp.dstport == port)), **kwargs)

    def filter_backbone_answer(self,
                               target: str,
                               *,
                               eth_src: Optional[EthAddr] = None,
                               port: int = None,
                               confirmable: bool = None,
                               mliid=None):
        filter_eth = self.filter_eth_src(eth_src) if eth_src else self.filter_eth()
        f = filter_eth.filter_coap_request('/b/ba', port=port,
                                           confirmable=confirmable).filter('thread_bl.tlv.target_eid == {target}',
                                                                           target=target)
        if mliid is not None:
            f = f.filter('thread_bl.tlv.ml_eid == {mliid}', mliid=mliid)

        return f

    def filter_backbone_query(self, target: str, *, eth_src: EthAddr, port: int = None) -> 'PacketFilter':
        return self.filter_eth_src(eth_src).filter_coap_request('/b/bq', port=port, confirmable=False).filter(
            'thread_bl.tlv.target_eid == {target}', target=target)

    def filter_wpan(self, **kwargs):
        """
        Create a new PacketFilter for filter WPAN packets.

        :param kwargs: Extra arguments for `filter`.
        :return: The new PacketFilter to filter WPAN packets.
        """
        return self.filter(attrgetter('wpan'), **kwargs)

    def filter_wpan_ack(self, **kwargs):
        """
        Create a new PacketFilter for filter WPAN ACK packets.

        :param kwargs: Extra arguments for `filter`.
        :return: The new PacketFilter to filter WPAN packets.
        """
        return self.filter(lambda p: p.wpan.frame_type == consts.MAC_FRAME_TYPE_ACK, **kwargs)

    def filter_wpan_beacon(self, **kwargs):
        """
        Create a new PacketFilter for filter WPAN beacon.

        :param kwargs: Extra arguments for `filter`.
        :return: The new PacketFilter to filter WPAN packets.
        """
        return self.filter(lambda p: p.wpan.frame_type == consts.MAC_FRAME_TYPE_BEACON, **kwargs)

    def filter_wpan_data(self, **kwargs):
        """
        Create a new PacketFilter for filter WPAN data packets.

        :param kwargs: Extra arguments for `filter`.
        :return: The new PacketFilter to filter WPAN packets.
        """
        return self.filter(lambda p: p.wpan.frame_type == consts.MAC_FRAME_TYPE_DATA, **kwargs)

    def filter_wpan_seq(self, seq, **kwargs):
        """
        Create a new PacketFilter for filter WPAN packets of a sequence number.

        :param seq: The sequence number to filter.
        :param kwargs: Extra arguments for `filter`.
        :return: The new PacketFilter to filter WPAN packets.
        """
        return self.filter(lambda p: p.wpan.seq_no == seq, **kwargs)

    def filter_wpan_version(self, version: int, **kwargs):
        """
        Create a new PacketFilter for filter WPAN packets of a given version.

        :param version: The version to filter.
        :param kwargs: Extra arguments for `filter`.
        :return: The new PacketFilter to filter WPAN packets.
        """
        return self.filter(lambda p: p.wpan.version == version, **kwargs)

    def filter_wpan_channel(self, channel: int, **kwargs):
        """
        Create a new PacketFilter for filter WPAN packets of a given channel.

        :param channel: The channel to filter.
        :param kwargs: Extra arguments for `filter`.
        :return: The new PacketFilter to filter WPAN packets.
        """
        return self.filter(lambda p: p.wpan.channel == channel, **kwargs)

    def filter_wpan_src16(self, addr, **kwargs):
        return self.filter(lambda p: p.wpan.src16 == addr, **kwargs)

    def filter_wpan_dst16(self, addr, **kwargs):
        return self.filter(lambda p: p.wpan.dst16 == addr, **kwargs)

    def filter_wpan_src16_dst16(self, src_addr, dst_addr, **kwargs):
        return self.filter(lambda p: p.wpan.src16 == src_addr and p.wpan.dst16 == dst_addr, **kwargs)

    def filter_wpan_src64(self, addr, **kwargs):
        assert isinstance(addr, (str, ExtAddr)), addr
        return self.filter(lambda p: p.wpan.src64 == addr, **kwargs)

    def filter_wpan_dst64(self, addr, **kwargs):
        assert isinstance(addr, (str, ExtAddr)), addr
        return self.filter(lambda p: p.wpan.dst64 == addr, **kwargs)

    def filter_dst16(self, rloc16: int, **kwargs):
        return self.filter(lambda p: p.lowpan.mesh.dest16 == rloc16 or p.wpan.dst16 == rloc16, **kwargs)

    def filter_wpan_ie_present(self, **kwargs):
        return self.filter(lambda p: p.wpan.ie_present == 1)

    def filter_wpan_ie_not_present(self, **kwargs):
        return self.filter(lambda p: p.wpan.ie_present == 0)

    def filter_ping_request(self, identifier=None, **kwargs):
        return self.filter(
            lambda p: p.icmpv6.is_ping_request and (identifier is None or p.icmpv6.echo.identifier == identifier),
            **kwargs)

    def filter_ping_reply(self, **kwargs):
        identifier = kwargs.pop('identifier', None)
        return self.filter(
            lambda p: (p.icmpv6.is_ping_reply and (identifier is None or p.icmpv6.echo.identifier == identifier)),
            **kwargs)

    def filter_eth(self, **kwargs):
        return self.filter(attrgetter('eth'), **kwargs)

    def filter_eth_src(self, addr, **kwargs):
        assert isinstance(addr, (str, EthAddr))
        return self.filter(lambda p: p.eth.src == addr, **kwargs)

    def filter_ipv6_dst(self, addr, **kwargs):
        assert isinstance(addr, (str, Ipv6Addr))
        return self.filter(lambda p: p.ipv6.dst == addr, **kwargs)

    def filter_ipv6_2dsts(self, addr1, addr2, **kwargs):
        assert isinstance(addr1, (str, Ipv6Addr))
        assert isinstance(addr2, (str, Ipv6Addr))
        return self.filter(lambda p: p.ipv6.dst == addr1 or p.ipv6.dst == addr2, **kwargs)

    def filter_ipv6_src_dst(self, src_addr, dst_addr, **kwargs):
        assert isinstance(src_addr, (str, Ipv6Addr))
        assert isinstance(dst_addr, (str, Ipv6Addr))
        return self.filter(lambda p: p.ipv6.src == src_addr and p.ipv6.dst == dst_addr, **kwargs)

    def filter_LLATNMA(self, **kwargs):
        return self.filter(lambda p: p.ipv6.dst == consts.LINK_LOCAL_All_THREAD_NODES_MULTICAST_ADDRESS, **kwargs)

    def filter_RLANMA(self, **kwargs):
        return self.filter(lambda p: p.ipv6.dst == consts.REALM_LOCAL_ALL_NODES_ADDRESS, **kwargs)

    def filter_RLARMA(self, **kwargs):
        return self.filter(lambda p: p.ipv6.dst == consts.REALM_LOCAL_ALL_ROUTERS_ADDRESS, **kwargs)

    def filter_RLATNMA(self, **kwargs):
        return self.filter(lambda p: p.ipv6.dst == consts.REALM_LOCAL_All_THREAD_NODES_MULTICAST_ADDRESS, **kwargs)

    def filter_LLANMA(self, **kwargs):
        return self.filter(lambda p: p.ipv6.dst == consts.LINK_LOCAL_ALL_NODES_MULTICAST_ADDRESS, **kwargs)

    def filter_LLABMA(self, **kwargs):
        return self.filter(lambda p: p.ipv6.dst == consts.LINK_LOCAL_ALL_BBRS_MULTICAST_ADDRESS, **kwargs)

    def filter_LLARMA(self, **kwargs):
        return self.filter(lambda p: p.ipv6.dst == consts.LINK_LOCAL_ALL_ROUTERS_MULTICAST_ADDRESS, **kwargs)

    def filter_AMPLFMA(self, mpl_seed_id: Union[int, Ipv6Addr] = None, **kwargs):
        f = self.filter(lambda p: p.ipv6.dst == consts.ALL_MPL_FORWARDERS_MA, **kwargs)
        if mpl_seed_id is not None:
            if isinstance(mpl_seed_id, int):
                mpl_seed_id = Bytes([mpl_seed_id >> 8, mpl_seed_id & 0xFF])
                f = f.filter(lambda p: p.ipv6.opt.mpl.seed_id == mpl_seed_id)
            else:
                rloc = mpl_seed_id
                rloc16 = bytes(rloc[-2:])
                f = f.filter(lambda p: (p.ipv6.src == rloc and p.ipv6.opt.mpl.flag.s == 0) or
                             (p.ipv6.src != rloc and p.ipv6.opt.mpl.flag.s == 1 and p.ipv6.opt.mpl.seed_id == rloc16))
        return f

    def filter_mle(self, **kwargs):
        return self.filter(attrgetter('mle'), **kwargs)

    def filter_wpan_cmd(self, cmd, **kwargs):
        assert isinstance(cmd, int), cmd
        return self.filter(lambda p: p.wpan.cmd == cmd, **kwargs)

    def filter_mle_cmd(self, cmd, **kwargs):
        assert isinstance(cmd, int), cmd
        return self.filter(lambda p: p.mle.cmd == cmd, **kwargs)

    def filter_mle_cmd2(self, cmd1, cmd2, **kwargs):
        assert isinstance(cmd1, int), cmd1
        assert isinstance(cmd2, int), cmd2
        return self.filter(lambda p: p.mle.cmd == cmd1 or p.mle.cmd == cmd2, **kwargs)

    def filter_mle_has_tlv(self, *tlv_types, **kwargs):
        return self.filter(lambda p: set(tlv_types) <= set(p.mle.tlv.type), **kwargs)

    def filter_icmpv6(self, **kwargs):
        return self.filter(attrgetter('icmpv6'), **kwargs)

    def filter_icmpv6_nd_ns(self, target_address: Ipv6Addr):
        return self.filter(lambda p:
                           (p.icmpv6.is_neighbor_solicitation and p.icmpv6.nd.ns.target_address == target_address))

    def filter_icmpv6_nd_na(self, target_address: Ipv6Addr):
        return self.filter(lambda p:
                           (p.icmpv6.is_neighbor_advertisement and p.icmpv6.nd.na.target_address == target_address))

    def filter_icmpv6_nd_ra(self):
        return self.filter(lambda p: p.icmpv6.is_router_advertisement)

    def filter_has_bbr_dataset(self):
        return self.filter("""
                thread_nwd.tlv.server.has('16')
                and thread_nwd.tlv.service.s_data.seqno is not null
                and thread_nwd.tlv.service.s_data.rrdelay is not null
                and thread_nwd.tlv.service.s_data.mlrtimeout is not null
            """)
