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

import collections
import io
import logging
import os
import sys
import time

import pcap
import threading
import traceback

try:
    import Queue
except ImportError:
    import queue as Queue

import message
import sniffer_transport


class Sniffer:
    """ Class representing the Sniffing node, whose main task is listening
        and logging message exchange performed by other nodes.
    """

    logger = logging.getLogger("sniffer.Sniffer")

    RECV_BUFFER_SIZE = 4096

    def __init__(self, message_factory):
        """
        Args:
            message_factory (MessageFactory): Class producing messages from data bytes.
        """

        self._message_factory = message_factory

        self._pcap = pcap.PcapCodec(os.getenv('TEST_NAME', 'current'))
        if __name__ == '__main__':
            sys.stdout.buffer.write(self._pcap.encode_header())
            sys.stdout.buffer.flush()

        # Create transport
        transport_factory = sniffer_transport.SnifferTransportFactory()
        self._transport = transport_factory.create_transport()

        self._thread = None
        self._thread_alive = threading.Event()
        self._thread_alive.clear()

        self._buckets = collections.defaultdict(Queue.Queue)

    def _sniffer_main_loop(self):
        """ Sniffer main loop. """

        self.logger.debug("Sniffer started.")

        while self._thread_alive.is_set():
            data, nodeid = self._transport.recv(self.RECV_BUFFER_SIZE)

            pkt = self._pcap.append(data)
            if __name__ == '__main__':
                try:
                    sys.stdout.buffer.write(pkt)
                    sys.stdout.flush()
                except BrokenPipeError:
                    self._thread_alive.clear()
                    break

            # Ignore any exceptions
            if self._message_factory is not None:
                try:
                    messages = self._message_factory.create(io.BytesIO(data))
                    self.logger.debug("Received messages: {}".format(messages))
                    for msg in messages:
                        self._buckets[nodeid].put(msg)

                except Exception as e:
                    # Just print the exception to the console
                    self.logger.error("EXCEPTION: %s" % e)
                    traceback.print_exc()

        self.logger.debug("Sniffer stopped.")

    def start(self):
        """ Start sniffing. """

        self._thread = threading.Thread(target=self._sniffer_main_loop)
        self._thread.daemon = True

        self._transport.open()

        self._thread_alive.set()
        self._thread.start()

    def stop(self):
        """ Stop sniffing. """

        self._thread_alive.clear()

        self._transport.close()

        self._thread.join(timeout=1)
        self._thread = None

    def set_lowpan_context(self, cid, prefix):
        self._message_factory.set_lowpan_context(cid, prefix)

    def get_messages_sent_by(self, nodeid):
        """ Get sniffed messages.

        Note! This method flushes the message queue so calling this
        method again will return only the newly logged messages.

        Args:
            nodeid (int): node id

        Returns:
            MessagesSet: a set with received messages.
        """
        bucket = self._buckets[nodeid]
        messages = []

        while not bucket.empty():
            messages.append(bucket.get_nowait())

        return message.MessagesSet(messages)


def run_sniffer():
    sniffer = Sniffer(None)
    sniffer.start()
    while sniffer._thread_alive.is_set():
        try:
            time.sleep(1)
        except KeyboardInterrupt:
            break

    sniffer.stop()


if __name__ == '__main__':
    run_sniffer()
