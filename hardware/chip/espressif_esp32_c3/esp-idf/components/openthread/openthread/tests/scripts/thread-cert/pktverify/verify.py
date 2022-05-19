#!/usr/bin/env python3
#
#  Copyright (c) 2020, The OpenThread Authors.
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

import importlib.util
import inspect
import json
import logging
import os
import sys

THREAD_CERT_DIR = './tests/scripts/thread-cert'
sys.path.append(THREAD_CERT_DIR)

import thread_cert
from pktverify.packet_verifier import PacketVerifier

logging.basicConfig(level=logging.INFO,
                    format='File "%(pathname)s", line %(lineno)d, in %(funcName)s\n'
                    '%(asctime)s - %(levelname)s - %(message)s')


def main():
    json_file = sys.argv[1]
    with open(json_file, 'rt') as fp:
        test_info = json.load(fp)

    script = test_info['script']

    script = os.path.relpath(script, THREAD_CERT_DIR)

    module_name = os.path.splitext(script)[0].replace('/', '.')
    logging.info("Loading %s as module %s ...", script, module_name)

    spec = importlib.util.spec_from_file_location(module_name, os.path.join(THREAD_CERT_DIR, script))
    mod = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(mod)

    test_class = None

    for name, member in inspect.getmembers(mod):
        if isinstance(member, type) and issubclass(member, thread_cert.TestCase):
            assert test_class is None, (test_class, member)
            test_class = member

    assert test_class is not None, "can not find a test class in %s" % script

    test_instance = test_class()

    pv = PacketVerifier(json_file)
    pv.add_common_vars()
    test_instance.verify(pv)

    print("Packet verification passed: %s" % json_file, file=sys.stderr)


if __name__ == '__main__':
    main()
