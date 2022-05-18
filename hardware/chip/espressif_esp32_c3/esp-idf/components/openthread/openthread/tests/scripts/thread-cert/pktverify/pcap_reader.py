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
#  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 'AS IS'
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
import os
import subprocess
from typing import Optional

import pyshark

from pktverify import consts, utils
from pktverify.packet import Packet
from pktverify.packet_filter import PacketFilter


class PcapReader(object):
    """
    Implements Pcap reading utilities.
    """

    @classmethod
    def read(cls,
             filename: str,
             override_prefs: Optional[dict] = None,
             tshark_path: Optional[str] = None) -> PacketFilter:
        """
        Read packets from a given Pcap file.

        :param filename: The Pcap file.
        :param override_prefs: Preferences settings for wireshark
        :param tshark_path: The optional path to the `tshark`.
        :return: A PacketFilter containing all packets of the Pcap file.
        """
        if override_prefs is None:
            override_prefs = consts.WIRESHARK_OVERRIDE_PREFS
        if tshark_path is None:
            tshark_path = utils.which_tshark()

        logging.info("Using tshark path: %s", tshark_path)
        subprocess.check_call(f"{tshark_path} -v", shell=True)
        os.system(f"ls -l {filename}")
        filecap = pyshark.FileCapture(filename,
                                      tshark_path=tshark_path,
                                      override_prefs=override_prefs,
                                      decode_as=consts.WIRESHARK_DECODE_AS_ENTRIES)
        filecap.load_packets()
        return PacketFilter(tuple(map(Packet, filecap._packets)))


if __name__ == '__main__':
    pkts = PcapReader.read("../../../../merged.pcap")
    print(len(pkts), 'packets loaded')
    for p in pkts:
        if not p.mle:
            break

        print("mle.cmd=%r" % p.mle.cmd)
