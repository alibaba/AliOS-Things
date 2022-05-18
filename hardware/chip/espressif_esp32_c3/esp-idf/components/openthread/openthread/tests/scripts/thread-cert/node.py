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

import binascii
import ipaddress
import logging
import os
import re
import socket
import subprocess
import sys
import time
import traceback
import unittest
from ipaddress import IPv6Address, IPv6Network
from typing import Union, Dict, Optional, List, Any

import pexpect
import pexpect.popen_spawn

import config
import simulator
import thread_cert

PORT_OFFSET = int(os.getenv('PORT_OFFSET', "0"))


class OtbrDocker:
    RESET_DELAY = 3

    _socat_proc = None
    _ot_rcp_proc = None
    _docker_proc = None

    def __init__(self, nodeid: int, **kwargs):
        self.verbose = int(float(os.getenv('VERBOSE', 0)))
        try:
            self._docker_name = config.OTBR_DOCKER_NAME_PREFIX + str(nodeid)
            self._prepare_ot_rcp_sim(nodeid)
            self._launch_docker()
        except Exception:
            traceback.print_exc()
            self.destroy()
            raise

    def _prepare_ot_rcp_sim(self, nodeid: int):
        self._socat_proc = subprocess.Popen(['socat', '-d', '-d', 'pty,raw,echo=0', 'pty,raw,echo=0'],
                                            stderr=subprocess.PIPE,
                                            stdin=subprocess.DEVNULL,
                                            stdout=subprocess.DEVNULL)

        line = self._socat_proc.stderr.readline().decode('ascii').strip()
        self._rcp_device_pty = rcp_device_pty = line[line.index('PTY is /dev') + 7:]
        line = self._socat_proc.stderr.readline().decode('ascii').strip()
        self._rcp_device = rcp_device = line[line.index('PTY is /dev') + 7:]
        logging.info(f"socat running: device PTY: {rcp_device_pty}, device: {rcp_device}")

        ot_rcp_path = self._get_ot_rcp_path()
        self._ot_rcp_proc = subprocess.Popen(f"{ot_rcp_path} {nodeid} > {rcp_device_pty} < {rcp_device_pty}",
                                             shell=True,
                                             stdin=subprocess.DEVNULL,
                                             stdout=subprocess.DEVNULL,
                                             stderr=subprocess.DEVNULL)

    def _get_ot_rcp_path(self) -> str:
        srcdir = os.environ['top_builddir']
        path = '%s/examples/apps/ncp/ot-rcp' % srcdir
        logging.info("ot-rcp path: %s", path)
        return path

    def _launch_docker(self):
        subprocess.check_call(f"docker rm -f {self._docker_name} || true", shell=True)
        CI_ENV = os.getenv('CI_ENV', '').split()
        os.makedirs('/tmp/coverage/', exist_ok=True)
        self._docker_proc = subprocess.Popen(['docker', 'run'] + CI_ENV + [
            '--rm',
            '--name',
            self._docker_name,
            '--network',
            config.BACKBONE_DOCKER_NETWORK_NAME,
            '-i',
            '--sysctl',
            'net.ipv6.conf.all.disable_ipv6=0 net.ipv4.conf.all.forwarding=1 net.ipv6.conf.all.forwarding=1',
            '--privileged',
            '--cap-add=NET_ADMIN',
            '--volume',
            f'{self._rcp_device}:/dev/ttyUSB0',
            '-v',
            '/tmp/coverage/:/tmp/coverage/',
            config.OTBR_DOCKER_IMAGE,
            '-B',
            config.BACKBONE_IFNAME,
        ],
                                             stdin=subprocess.DEVNULL,
                                             stdout=sys.stdout,
                                             stderr=sys.stderr)

        launch_docker_deadline = time.time() + 300
        launch_ok = False

        while time.time() < launch_docker_deadline:
            try:
                subprocess.check_call(f'docker exec -i {self._docker_name} ot-ctl state', shell=True)
                launch_ok = True
                logging.info("OTBR Docker %s Is Ready!", self._docker_name)
                break
            except subprocess.CalledProcessError:
                time.sleep(5)
                continue

        assert launch_ok

        cmd = f'docker exec -i {self._docker_name} ot-ctl'
        self.pexpect = pexpect.popen_spawn.PopenSpawn(cmd, timeout=30)
        if self.verbose:
            self.pexpect.logfile_read = sys.stdout.buffer

        # Add delay to ensure that the process is ready to receive commands.
        timeout = 0.4
        while timeout > 0:
            self.pexpect.send('\r\n')
            try:
                self.pexpect.expect('> ', timeout=0.1)
                break
            except pexpect.TIMEOUT:
                timeout -= 0.1

    def __repr__(self):
        return f'OtbrDocker<{self.nodeid}>'

    def destroy(self):
        logging.info("Destroying %s", self)
        self._shutdown_docker()
        self._shutdown_ot_rcp()
        self._shutdown_socat()

    def _shutdown_docker(self):
        if self._docker_proc is not None:
            COVERAGE = int(os.getenv('COVERAGE', '0'))
            OTBR_COVERAGE = int(os.getenv('OTBR_COVERAGE', '0'))
            if COVERAGE or OTBR_COVERAGE:
                self.bash('service otbr-agent stop')

                test_name = os.getenv('TEST_NAME')
                cov_file_path = f'/tmp/coverage/coverage-{test_name}-{PORT_OFFSET}-{self.nodeid}.info'
                # Upload OTBR code coverage if OTBR_COVERAGE=1, otherwise OpenThread code coverage.
                if OTBR_COVERAGE:
                    codecov_cmd = f'lcov --directory . --capture --output-file {cov_file_path}'
                else:
                    codecov_cmd = ('lcov --directory build/otbr/third_party/openthread/repo --capture '
                                   f'--output-file {cov_file_path}')

                self.bash(codecov_cmd)

            subprocess.check_call(f"docker rm -f {self._docker_name}", shell=True)
            self._docker_proc.wait()
            del self._docker_proc

    def _shutdown_ot_rcp(self):
        if self._ot_rcp_proc is not None:
            self._ot_rcp_proc.kill()
            self._ot_rcp_proc.wait()
            del self._ot_rcp_proc

    def _shutdown_socat(self):
        if self._socat_proc is not None:
            self._socat_proc.stderr.close()
            self._socat_proc.kill()
            self._socat_proc.wait()
            del self._socat_proc

    def bash(self, cmd: str, encoding='ascii') -> List[str]:
        logging.info("%s $ %s", self, cmd)
        proc = subprocess.Popen(['docker', 'exec', '-i', self._docker_name, 'bash', '-c', cmd],
                                stdin=subprocess.DEVNULL,
                                stdout=subprocess.PIPE,
                                stderr=sys.stderr,
                                encoding=encoding)

        with proc:

            lines = []

            while True:
                line = proc.stdout.readline()

                if not line:
                    break

                lines.append(line)
                logging.info("%s $ %r", self, line.rstrip('\r\n'))

            proc.wait()

            if proc.returncode != 0:
                raise subprocess.CalledProcessError(proc.returncode, cmd, ''.join(lines))
            else:
                return lines

    def dns_dig(self, server: str, name: str, qtype: str):
        """
        Run dig command to query a DNS server.

        Args:
            server: the server address.
            name: the name to query.
            qtype: the query type (e.g. AAAA, PTR, TXT, SRV).

        Returns:
            The dig result similar as below:
            {
                "opcode": "QUERY",
                "status": "NOERROR",
                "id": "64144",
                "QUESTION": [
                    ('google.com.', 'IN', 'AAAA')
                ],
                "ANSWER": [
                    ('google.com.', 107,	'IN', 'AAAA', '2404:6800:4008:c00::71'),
                    ('google.com.', 107,	'IN', 'AAAA', '2404:6800:4008:c00::8a'),
                    ('google.com.', 107,	'IN', 'AAAA', '2404:6800:4008:c00::66'),
                    ('google.com.', 107,	'IN', 'AAAA', '2404:6800:4008:c00::8b'),
                ],
                "ADDITIONAL": [
                ],
            }
        """
        output = self.bash(f'dig -6 @{server} \'{name}\' {qtype}', encoding='raw_unicode_escape')

        section = None
        dig_result = {
            'QUESTION': [],
            'ANSWER': [],
            'ADDITIONAL': [],
        }

        for line in output:
            line = line.strip()

            if line.startswith(';; ->>HEADER<<- '):
                headers = line[len(';; ->>HEADER<<- '):].split(', ')
                for header in headers:
                    key, val = header.split(': ')
                    dig_result[key] = val

                continue

            if line == ';; QUESTION SECTION:':
                section = 'QUESTION'
                continue
            elif line == ';; ANSWER SECTION:':
                section = 'ANSWER'
                continue
            elif line == ';; ADDITIONAL SECTION:':
                section = 'ADDITIONAL'
                continue
            elif section and not line:
                section = None
                continue

            if section:
                assert line

                if section == 'QUESTION':
                    assert line.startswith(';')
                    line = line[1:]
                record = list(line.split())

                if section != 'QUESTION':
                    record[1] = int(record[1])
                    if record[3] == 'SRV':
                        record[4], record[5], record[6] = map(int, [record[4], record[5], record[6]])
                    elif record[3] == 'TXT':
                        record[4:] = [self.__parse_dns_dig_txt(line)]

                dig_result[section].append(tuple(record))

        return dig_result

    def __parse_dns_dig_txt(self, line: str):
        # Example TXT entry:
        # "xp=\\000\\013\\184\\000\\000\\000\\000\\000"
        txt = {}
        for entry in re.findall(r'"((?:[^\\]|\\.)*?)"', line):
            if entry == "":
                continue

            k, v = entry.split('=', 1)
            txt[k] = v

        return txt

    def _setup_sysctl(self):
        self.bash(f'sysctl net.ipv6.conf.{self.ETH_DEV}.accept_ra=2')
        self.bash(f'sysctl net.ipv6.conf.{self.ETH_DEV}.accept_ra_rt_info_max_plen=64')


class OtCli:
    RESET_DELAY = 0.1

    def __init__(self, nodeid, is_mtd=False, version=None, is_bbr=False, **kwargs):
        self.verbose = int(float(os.getenv('VERBOSE', 0)))
        self.node_type = os.getenv('NODE_TYPE', 'sim')
        self.env_version = os.getenv('THREAD_VERSION', '1.1')
        self.is_bbr = is_bbr
        self._initialized = False
        if os.getenv('COVERAGE', 0) and os.getenv('CC', 'gcc') == 'gcc':
            self._cmd_prefix = '/usr/bin/env GCOV_PREFIX=%s/ot-run/%s/ot-gcda.%d ' % (os.getenv(
                'top_srcdir', '.'), sys.argv[0], nodeid)
        else:
            self._cmd_prefix = ''

        if version is not None:
            self.version = version
        else:
            self.version = self.env_version

        mode = os.environ.get('USE_MTD') == '1' and is_mtd and 'mtd' or 'ftd'

        if self.node_type == 'soc':
            self.__init_soc(nodeid)
        elif self.node_type == 'ncp-sim':
            # TODO use mode after ncp-mtd is available.
            self.__init_ncp_sim(nodeid, 'ftd')
        else:
            self.__init_sim(nodeid, mode)

        if self.verbose:
            self.pexpect.logfile_read = sys.stdout.buffer

        self._initialized = True

    def __init_sim(self, nodeid, mode):
        """ Initialize a simulation node. """

        # Default command if no match below, will be overridden if below conditions are met.
        cmd = './ot-cli-%s' % (mode)

        # For Thread 1.2 MTD node, use ot-cli-mtd build regardless of OT_CLI_PATH
        if self.version == '1.2' and mode == 'mtd' and 'top_builddir' in os.environ:
            srcdir = os.environ['top_builddir']
            cmd = '%s/examples/apps/cli/ot-cli-%s %d' % (srcdir, mode, nodeid)

        # If Thread version of node matches the testing environment version.
        elif self.version == self.env_version:
            # Load Thread 1.2 BBR device when testing Thread 1.2 scenarios
            # which requires device with Backbone functionality.
            if self.version == '1.2' and self.is_bbr:
                if 'OT_CLI_PATH_1_2_BBR' in os.environ:
                    cmd = os.environ['OT_CLI_PATH_1_2_BBR']
                elif 'top_builddir_1_2_bbr' in os.environ:
                    srcdir = os.environ['top_builddir_1_2_bbr']
                    cmd = '%s/examples/apps/cli/ot-cli-%s' % (srcdir, mode)

            # Load Thread device of the testing environment version (may be 1.1 or 1.2)
            else:
                if 'OT_CLI_PATH' in os.environ:
                    cmd = os.environ['OT_CLI_PATH']
                elif 'top_builddir' in os.environ:
                    srcdir = os.environ['top_builddir']
                    cmd = '%s/examples/apps/cli/ot-cli-%s' % (srcdir, mode)

            if 'RADIO_DEVICE' in os.environ:
                cmd += ' --real-time-signal=+1 -v spinel+hdlc+uart://%s?forkpty-arg=%d' % (os.environ['RADIO_DEVICE'],
                                                                                           nodeid)
                self.is_posix = True
            else:
                cmd += ' %d' % nodeid

        # Load Thread 1.1 node when testing Thread 1.2 scenarios for interoperability
        elif self.version == '1.1':
            # Posix app
            if 'OT_CLI_PATH_1_1' in os.environ:
                cmd = os.environ['OT_CLI_PATH_1_1']
            elif 'top_builddir_1_1' in os.environ:
                srcdir = os.environ['top_builddir_1_1']
                cmd = '%s/examples/apps/cli/ot-cli-%s' % (srcdir, mode)

            if 'RADIO_DEVICE_1_1' in os.environ:
                cmd += ' --real-time-signal=+1 -v spinel+hdlc+uart://%s?forkpty-arg=%d' % (
                    os.environ['RADIO_DEVICE_1_1'], nodeid)
                self.is_posix = True
            else:
                cmd += ' %d' % nodeid

        print("%s" % cmd)

        self.pexpect = pexpect.popen_spawn.PopenSpawn(self._cmd_prefix + cmd, timeout=10)

        # Add delay to ensure that the process is ready to receive commands.
        timeout = 0.4
        while timeout > 0:
            self.pexpect.send('\r\n')
            try:
                self.pexpect.expect('> ', timeout=0.1)
                break
            except pexpect.TIMEOUT:
                timeout -= 0.1

    def __init_ncp_sim(self, nodeid, mode):
        """ Initialize an NCP simulation node. """

        # Default command if no match below, will be overridden if below conditions are met.
        cmd = 'spinel-cli.py -p ./ot-ncp-%s -n' % mode

        # If Thread version of node matches the testing environment version.
        if self.version == self.env_version:
            if 'RADIO_DEVICE' in os.environ:
                args = ' --real-time-signal=+1 spinel+hdlc+uart://%s?forkpty-arg=%d' % (os.environ['RADIO_DEVICE'],
                                                                                        nodeid)
                self.is_posix = True
            else:
                args = ''

            # Load Thread 1.2 BBR device when testing Thread 1.2 scenarios
            # which requires device with Backbone functionality.
            if self.version == '1.2' and self.is_bbr:
                if 'OT_NCP_PATH_1_2_BBR' in os.environ:
                    cmd = 'spinel-cli.py -p "%s%s" -n' % (
                        os.environ['OT_NCP_PATH_1_2_BBR'],
                        args,
                    )
                elif 'top_builddir_1_2_bbr' in os.environ:
                    srcdir = os.environ['top_builddir_1_2_bbr']
                    cmd = '%s/examples/apps/ncp/ot-ncp-%s' % (srcdir, mode)
                    cmd = 'spinel-cli.py -p "%s%s" -n' % (
                        cmd,
                        args,
                    )

            # Load Thread device of the testing environment version (may be 1.1 or 1.2).
            else:
                if 'OT_NCP_PATH' in os.environ:
                    cmd = 'spinel-cli.py -p "%s%s" -n' % (
                        os.environ['OT_NCP_PATH'],
                        args,
                    )
                elif 'top_builddir' in os.environ:
                    srcdir = os.environ['top_builddir']
                    cmd = '%s/examples/apps/ncp/ot-ncp-%s' % (srcdir, mode)
                    cmd = 'spinel-cli.py -p "%s%s" -n' % (
                        cmd,
                        args,
                    )

        # Load Thread 1.1 node when testing Thread 1.2 scenarios for interoperability.
        elif self.version == '1.1':
            if 'RADIO_DEVICE_1_1' in os.environ:
                args = ' --real-time-signal=+1 spinel+hdlc+uart://%s?forkpty-arg=%d' % (os.environ['RADIO_DEVICE_1_1'],
                                                                                        nodeid)
                self.is_posix = True
            else:
                args = ''

            if 'OT_NCP_PATH_1_1' in os.environ:
                cmd = 'spinel-cli.py -p "%s%s" -n' % (
                    os.environ['OT_NCP_PATH_1_1'],
                    args,
                )
            elif 'top_builddir_1_1' in os.environ:
                srcdir = os.environ['top_builddir_1_1']
                cmd = '%s/examples/apps/ncp/ot-ncp-%s' % (srcdir, mode)
                cmd = 'spinel-cli.py -p "%s%s" -n' % (
                    cmd,
                    args,
                )

        cmd += ' %d' % nodeid
        print("%s" % cmd)

        self.pexpect = pexpect.spawn(self._cmd_prefix + cmd, timeout=10)

        # Add delay to ensure that the process is ready to receive commands.
        time.sleep(0.2)
        self._expect('spinel-cli >')
        self.debug(int(os.getenv('DEBUG', '0')))

    def __init_soc(self, nodeid):
        """ Initialize a System-on-a-chip node connected via UART. """
        import fdpexpect

        serialPort = '/dev/ttyUSB%d' % ((nodeid - 1) * 2)
        self.pexpect = fdpexpect.fdspawn(os.open(serialPort, os.O_RDWR | os.O_NONBLOCK | os.O_NOCTTY))

    def destroy(self):
        if not self._initialized:
            return

        if (hasattr(self.pexpect, 'proc') and self.pexpect.proc.poll() is None or
                not hasattr(self.pexpect, 'proc') and self.pexpect.isalive()):
            print("%d: exit" % self.nodeid)
            self.pexpect.send('exit\n')
            self.pexpect.expect(pexpect.EOF)
            self.pexpect.wait()
            self._initialized = False


class NodeImpl:
    is_host = False
    is_otbr = False

    def __init__(self, nodeid, name=None, simulator=None, **kwargs):
        self.nodeid = nodeid
        self.name = name or ('Node%d' % nodeid)
        self.is_posix = False

        self.simulator = simulator
        if self.simulator:
            self.simulator.add_node(self)

        super().__init__(nodeid, **kwargs)

        self.set_extpanid(config.EXTENDED_PANID)
        self.set_addr64('%016x' % (thread_cert.EXTENDED_ADDRESS_BASE + nodeid))

    def _expect(self, pattern, timeout=-1, *args, **kwargs):
        """ Process simulator events until expected the pattern. """
        if timeout == -1:
            timeout = self.pexpect.timeout

        assert timeout > 0

        while timeout > 0:
            try:
                return self.pexpect.expect(pattern, 0.1, *args, **kwargs)
            except pexpect.TIMEOUT:
                timeout -= 0.1
                self.simulator.go(0)
                if timeout <= 0:
                    raise

    def _expect_done(self, timeout=-1):
        self._expect('Done', timeout)

    def _prepare_pattern(self, pattern):
        """Build a new pexpect pattern matching line by line.

        Adds lookahead and lookbehind to make each pattern match a whole line,
        and add 'Done' as the first pattern.

        Args:
            pattern: a single regex or a list of regex.

        Returns:
            A list of regex.
        """
        EXPECT_LINE_FORMAT = r'(?<=[\r\n])%s(?=[\r\n])'

        if isinstance(pattern, list):
            pattern = [EXPECT_LINE_FORMAT % p for p in pattern]
        else:
            pattern = [EXPECT_LINE_FORMAT % pattern]

        return [EXPECT_LINE_FORMAT % 'Done'] + pattern

    def _expect_result(self, pattern, *args, **kwargs):
        """Expect a single matching result.

        The arguments are identical to pexpect.expect().

        Returns:
            The matched line.
        """
        results = self._expect_results(pattern, *args, **kwargs)
        assert len(results) == 1, results
        return results[0]

    def _expect_results(self, pattern, *args, **kwargs):
        """Expect multiple matching results.

        The arguments are identical to pexpect.expect().

        Returns:
            The matched lines.
        """
        results = []
        pattern = self._prepare_pattern(pattern)

        while self._expect(pattern, *args, **kwargs):
            results.append(self.pexpect.match.group(0).decode('utf8'))

        return results

    def _expect_command_output(self, cmd: str, ignore_logs=True):
        lines = []
        cmd_output_started = False

        while True:
            self._expect(r"[^\n]+\n")
            line = self.pexpect.match.group(0).decode('utf8').strip()

            if line.startswith('> '):
                line = line[2:]

            if line == '':
                continue

            if line == cmd:
                cmd_output_started = True
                continue

            if not cmd_output_started or (ignore_logs and self.__is_logging_line(line)):
                continue

            if line == 'Done':
                break
            elif line.startswith('Error '):
                raise Exception(line)
            else:
                lines.append(line)

        print(f'_expect_command_output({cmd!r}) returns {lines!r}')
        return lines

    def __is_logging_line(self, line: str) -> bool:
        return len(line) >= 6 and line[:6] in {'[DEBG]', '[INFO]', '[NOTE]', '[WARN]', '[CRIT]', '[NONE]'}

    def read_cert_messages_in_commissioning_log(self, timeout=-1):
        """Get the log of the traffic after DTLS handshake.
        """
        format_str = br"=+?\[\[THCI\].*?type=%s.*?\].*?=+?[\s\S]+?-{40,}"
        join_fin_req = format_str % br"JOIN_FIN\.req"
        join_fin_rsp = format_str % br"JOIN_FIN\.rsp"
        dummy_format_str = br"\[THCI\].*?type=%s.*?"
        join_ent_ntf = dummy_format_str % br"JOIN_ENT\.ntf"
        join_ent_rsp = dummy_format_str % br"JOIN_ENT\.rsp"
        pattern = (b"(" + join_fin_req + b")|(" + join_fin_rsp + b")|(" + join_ent_ntf + b")|(" + join_ent_rsp + b")")

        messages = []
        # There are at most 4 cert messages both for joiner and commissioner
        for _ in range(0, 4):
            try:
                self._expect(pattern, timeout=timeout)
                log = self.pexpect.match.group(0)
                messages.append(self._extract_cert_message(log))
            except BaseException:
                break
        return messages

    def _extract_cert_message(self, log):
        res = re.search(br"direction=\w+", log)
        assert res
        direction = res.group(0).split(b'=')[1].strip()

        res = re.search(br"type=\S+", log)
        assert res
        type = res.group(0).split(b'=')[1].strip()

        payload = bytearray([])
        payload_len = 0
        if type in [b"JOIN_FIN.req", b"JOIN_FIN.rsp"]:
            res = re.search(br"len=\d+", log)
            assert res
            payload_len = int(res.group(0).split(b'=')[1].strip())

            hex_pattern = br"\|(\s([0-9a-fA-F]{2}|\.\.))+?\s+?\|"
            while True:
                res = re.search(hex_pattern, log)
                if not res:
                    break
                data = [int(hex, 16) for hex in res.group(0)[1:-1].split(b' ') if hex and hex != b'..']
                payload += bytearray(data)
                log = log[res.end() - 1:]
        assert len(payload) == payload_len
        return (direction, type, payload)

    def send_command(self, cmd, go=True):
        print("%d: %s" % (self.nodeid, cmd))
        self.pexpect.send(cmd + '\n')
        if go:
            self.simulator.go(0, nodeid=self.nodeid)
        sys.stdout.flush()

    def get_commands(self):
        self.send_command('?')
        self._expect('Commands:')
        return self._expect_results(r'\S+')

    def set_mode(self, mode):
        cmd = 'mode %s' % mode
        self.send_command(cmd)
        self._expect_done()

    def debug(self, level):
        # `debug` command will not trigger interaction with simulator
        self.send_command('debug %d' % level, go=False)

    def start(self):
        self.interface_up()
        self.thread_start()

    def stop(self):
        self.thread_stop()
        self.interface_down()

    def set_log_level(self, level: int):
        self.send_command(f'log level {level}')
        self._expect_done()

    def interface_up(self):
        self.send_command('ifconfig up')
        self._expect_done()

    def interface_down(self):
        self.send_command('ifconfig down')
        self._expect_done()

    def thread_start(self):
        self.send_command('thread start')
        self._expect_done()

    def thread_stop(self):
        self.send_command('thread stop')
        self._expect_done()

    def commissioner_start(self):
        cmd = 'commissioner start'
        self.send_command(cmd)
        self._expect_done()

    def commissioner_stop(self):
        cmd = 'commissioner stop'
        self.send_command(cmd)
        self._expect_done()

    def commissioner_state(self):
        states = [r'disabled', r'petitioning', r'active']
        self.send_command('commissioner state')
        return self._expect_result(states)

    def commissioner_add_joiner(self, addr, psk):
        cmd = 'commissioner joiner add %s %s' % (addr, psk)
        self.send_command(cmd)
        self._expect_done()

    def commissioner_set_provisioning_url(self, provisioning_url=''):
        cmd = 'commissioner provisioningurl %s' % provisioning_url
        self.send_command(cmd)
        self._expect_done()

    def joiner_start(self, pskd='', provisioning_url=''):
        cmd = 'joiner start %s %s' % (pskd, provisioning_url)
        self.send_command(cmd)
        self._expect_done()

    def clear_allowlist(self):
        cmd = 'macfilter addr clear'
        self.send_command(cmd)
        self._expect_done()

    def enable_allowlist(self):
        cmd = 'macfilter addr allowlist'
        self.send_command(cmd)
        self._expect_done()

    def disable_allowlist(self):
        cmd = 'macfilter addr disable'
        self.send_command(cmd)
        self._expect_done()

    def add_allowlist(self, addr, rssi=None):
        cmd = 'macfilter addr add %s' % addr

        if rssi is not None:
            cmd += ' %s' % rssi

        self.send_command(cmd)
        self._expect_done()

    def get_bbr_registration_jitter(self):
        self.send_command('bbr jitter')
        return int(self._expect_result(r'\d+'))

    def set_bbr_registration_jitter(self, jitter):
        cmd = 'bbr jitter %d' % jitter
        self.send_command(cmd)
        self._expect_done()

    def srp_server_get_state(self):
        states = ['disabled', 'running', 'stopped']
        self.send_command('srp server state')
        return self._expect_result(states)

    def srp_server_get_addr_mode(self):
        modes = [r'unicast', r'anycast']
        self.send_command(f'srp server addrmode')
        return self._expect_result(modes)

    def srp_server_set_addr_mode(self, mode):
        self.send_command(f'srp server addrmode {mode}')
        self._expect_done()

    def srp_server_get_anycast_seq_num(self):
        self.send_command(f'srp server seqnum')
        return int(self._expect_result(r'\d+'))

    def srp_server_set_anycast_seq_num(self, seqnum):
        self.send_command(f'srp server seqnum {seqnum}')
        self._expect_done()

    def srp_server_set_enabled(self, enable):
        cmd = f'srp server {"enable" if enable else "disable"}'
        self.send_command(cmd)
        self._expect_done()

    def srp_server_set_lease_range(self, min_lease, max_lease, min_key_lease, max_key_lease):
        self.send_command(f'srp server lease {min_lease} {max_lease} {min_key_lease} {max_key_lease}')
        self._expect_done()

    def srp_server_get_hosts(self):
        """Returns the host list on the SRP server as a list of property
           dictionary.

           Example output:
           [{
               'fullname': 'my-host.default.service.arpa.',
               'name': 'my-host',
               'deleted': 'false',
               'addresses': ['2001::1', '2001::2']
           }]
        """

        cmd = 'srp server host'
        self.send_command(cmd)
        lines = self._expect_command_output(cmd)
        host_list = []
        while lines:
            host = {}

            host['fullname'] = lines.pop(0).strip()
            host['name'] = host['fullname'].split('.')[0]

            host['deleted'] = lines.pop(0).strip().split(':')[1].strip()
            if host['deleted'] == 'true':
                host_list.append(host)
                continue

            addresses = lines.pop(0).strip().split('[')[1].strip(' ]').split(',')
            map(str.strip, addresses)
            host['addresses'] = [addr for addr in addresses if addr]

            host_list.append(host)

        return host_list

    def srp_server_get_host(self, host_name):
        """Returns host on the SRP server that matches given host name.

           Example usage:
           self.srp_server_get_host("my-host")
        """

        for host in self.srp_server_get_hosts():
            if host_name == host['name']:
                return host

    def srp_server_get_services(self):
        """Returns the service list on the SRP server as a list of property
           dictionary.

           Example output:
           [{
               'fullname': 'my-service._ipps._tcp.default.service.arpa.',
               'instance': 'my-service',
               'name': '_ipps._tcp',
               'deleted': 'false',
               'port': '12345',
               'priority': '0',
               'weight': '0',
               'TXT': ['abc=010203'],
               'host_fullname': 'my-host.default.service.arpa.',
               'host': 'my-host',
               'addresses': ['2001::1', '2001::2']
           }]

           Note that the TXT data is output as a HEX string.
        """

        cmd = 'srp server service'
        self.send_command(cmd)
        lines = self._expect_command_output(cmd)
        service_list = []
        while lines:
            service = {}

            service['fullname'] = lines.pop(0).strip()
            name_labels = service['fullname'].split('.')
            service['instance'] = name_labels[0]
            service['name'] = '.'.join(name_labels[1:3])

            service['deleted'] = lines.pop(0).strip().split(':')[1].strip()
            if service['deleted'] == 'true':
                service_list.append(service)
                continue

            # 'subtypes', port', 'priority', 'weight'
            for i in range(0, 4):
                key_value = lines.pop(0).strip().split(':')
                service[key_value[0].strip()] = key_value[1].strip()

            txt_entries = lines.pop(0).strip().split('[')[1].strip(' ]').split(',')
            txt_entries = map(str.strip, txt_entries)
            service['TXT'] = [txt for txt in txt_entries if txt]

            service['host_fullname'] = lines.pop(0).strip().split(':')[1].strip()
            service['host'] = service['host_fullname'].split('.')[0]

            addresses = lines.pop(0).strip().split('[')[1].strip(' ]').split(',')
            addresses = map(str.strip, addresses)
            service['addresses'] = [addr for addr in addresses if addr]

            service_list.append(service)

        return service_list

    def srp_server_get_service(self, instance_name, service_name):
        """Returns service on the SRP server that matches given instance
           name and service name.

           Example usage:
           self.srp_server_get_service("my-service", "_ipps._tcp")
        """

        for service in self.srp_server_get_services():
            if (instance_name == service['instance'] and service_name == service['name']):
                return service

    def get_srp_server_port(self):
        """Returns the SRP server UDP port by parsing
           the SRP Server Data in Network Data.
        """

        for service in self.get_services():
            # TODO: for now, we are using 0xfd as the SRP service data.
            #       May use a dedicated bit flag for SRP server.
            if int(service[1], 16) == 0x5d:
                # The SRP server data contains IPv6 address (16 bytes)
                # followed by UDP port number.
                return int(service[2][2 * 16:], 16)

    def srp_client_start(self, server_address, server_port):
        self.send_command(f'srp client start {server_address} {server_port}')
        self._expect_done()

    def srp_client_stop(self):
        self.send_command(f'srp client stop')
        self._expect_done()

    def srp_client_get_state(self):
        cmd = 'srp client state'
        self.send_command(cmd)
        return self._expect_command_output(cmd)[0]

    def srp_client_get_auto_start_mode(self):
        cmd = 'srp client autostart'
        self.send_command(cmd)
        return self._expect_command_output(cmd)[0]

    def srp_client_enable_auto_start_mode(self):
        self.send_command(f'srp client autostart enable')
        self._expect_done()

    def srp_client_disable_auto_start_mode(self):
        self.send_command(f'srp client autostart able')
        self._expect_done()

    def srp_client_get_server_address(self):
        cmd = 'srp client server address'
        self.send_command(cmd)
        return self._expect_command_output(cmd)[0]

    def srp_client_get_server_port(self):
        cmd = 'srp client server port'
        self.send_command(cmd)
        return int(self._expect_command_output(cmd)[0])

    def srp_client_get_host_state(self):
        cmd = 'srp client host state'
        self.send_command(cmd)
        return self._expect_command_output(cmd)[0]

    def srp_client_set_host_name(self, name):
        self.send_command(f'srp client host name {name}')
        self._expect_done()

    def srp_client_get_host_name(self):
        self.send_command(f'srp client host name')
        self._expect_done()

    def srp_client_remove_host(self, remove_key=False, send_unreg_to_server=False):
        self.send_command(f'srp client host remove {int(remove_key)} {int(send_unreg_to_server)}')
        self._expect_done()

    def srp_client_clear_host(self):
        self.send_command(f'srp client host clear')
        self._expect_done()

    def srp_client_set_host_address(self, *addrs: str):
        self.send_command(f'srp client host address {" ".join(addrs)}')
        self._expect_done()

    def srp_client_get_host_address(self):
        self.send_command(f'srp client host address')
        self._expect_done()

    def srp_client_add_service(self, instance_name, service_name, port, priority=0, weight=0, txt_entries=[]):
        txt_record = "".join(self._encode_txt_entry(entry) for entry in txt_entries)
        self.send_command(
            f'srp client service add {instance_name} {service_name} {port} {priority} {weight} {txt_record}')
        self._expect_done()

    def srp_client_remove_service(self, instance_name, service_name):
        self.send_command(f'srp client service remove {instance_name} {service_name}')
        self._expect_done()

    def srp_client_clear_service(self, instance_name, service_name):
        self.send_command(f'srp client service clear {instance_name} {service_name}')
        self._expect_done()

    def srp_client_get_services(self):
        cmd = 'srp client service'
        self.send_command(cmd)
        service_lines = self._expect_command_output(cmd)
        return [self._parse_srp_client_service(line) for line in service_lines]

    def _encode_txt_entry(self, entry):
        """Encodes the TXT entry to the DNS-SD TXT record format as a HEX string.

           Example usage:
           self._encode_txt_entries(['abc'])     -> '03616263'
           self._encode_txt_entries(['def='])    -> '046465663d'
           self._encode_txt_entries(['xyz=XYZ']) -> '0778797a3d58595a'
        """
        return '{:02x}'.format(len(entry)) + "".join("{:02x}".format(ord(c)) for c in entry)

    def _parse_srp_client_service(self, line: str):
        """Parse one line of srp service list into a dictionary which
           maps string keys to string values.

           Example output for input
           'instance:\"%s\", name:\"%s\", state:%s, port:%d, priority:%d, weight:%d"'
           {
               'instance': 'my-service',
               'name': '_ipps._udp',
               'state': 'ToAdd',
               'port': '12345',
               'priority': '0',
               'weight': '0'
           }

           Note that value of 'port', 'priority' and 'weight' are represented
           as strings but not integers.
        """
        key_values = [word.strip().split(':') for word in line.split(', ')]
        keys = [key_value[0] for key_value in key_values]
        values = [key_value[1].strip('"') for key_value in key_values]
        return dict(zip(keys, values))

    def locate(self, anycast_addr):
        cmd = 'locate ' + anycast_addr
        self.send_command(cmd)
        self.simulator.go(5)
        return self._parse_locate_result(self._expect_command_output(cmd)[0])

    def _parse_locate_result(self, line: str):
        """Parse anycast locate result as list of ml-eid and rloc16.

           Example output for input
           'fd00:db8:0:0:acf9:9d0:7f3c:b06e 0xa800'

           [ 'fd00:db8:0:0:acf9:9d0:7f3c:b06e', '0xa800' ]
        """
        return line.split(' ')

    def enable_backbone_router(self):
        cmd = 'bbr enable'
        self.send_command(cmd)
        self._expect_done()

    def disable_backbone_router(self):
        cmd = 'bbr disable'
        self.send_command(cmd)
        self._expect_done()

    def register_backbone_router(self):
        cmd = 'bbr register'
        self.send_command(cmd)
        self._expect_done()

    def get_backbone_router_state(self):
        states = [r'Disabled', r'Primary', r'Secondary']
        self.send_command('bbr state')
        return self._expect_result(states)

    @property
    def is_primary_backbone_router(self) -> bool:
        return self.get_backbone_router_state() == 'Primary'

    def get_backbone_router(self):
        cmd = 'bbr config'
        self.send_command(cmd)
        self._expect(r'(.*)Done')
        g = self.pexpect.match.groups()
        output = g[0].decode("utf-8")
        lines = output.strip().split('\n')
        lines = [l.strip() for l in lines]
        ret = {}
        for l in lines:
            z = re.search(r'seqno:\s+([0-9]+)', l)
            if z:
                ret['seqno'] = int(z.groups()[0])

            z = re.search(r'delay:\s+([0-9]+)', l)
            if z:
                ret['delay'] = int(z.groups()[0])

            z = re.search(r'timeout:\s+([0-9]+)', l)
            if z:
                ret['timeout'] = int(z.groups()[0])

        return ret

    def set_backbone_router(self, seqno=None, reg_delay=None, mlr_timeout=None):
        cmd = 'bbr config'

        if seqno is not None:
            cmd += ' seqno %d' % seqno

        if reg_delay is not None:
            cmd += ' delay %d' % reg_delay

        if mlr_timeout is not None:
            cmd += ' timeout %d' % mlr_timeout

        self.send_command(cmd)
        self._expect_done()

    def set_domain_prefix(self, prefix, flags='prosD'):
        self.add_prefix(prefix, flags)
        self.register_netdata()

    def remove_domain_prefix(self, prefix):
        self.remove_prefix(prefix)
        self.register_netdata()

    def set_next_dua_response(self, status: Union[str, int], iid=None):
        # Convert 5.00 to COAP CODE 160
        if isinstance(status, str):
            assert '.' in status
            status = status.split('.')
            status = (int(status[0]) << 5) + int(status[1])

        cmd = 'bbr mgmt dua {}'.format(status)
        if iid is not None:
            cmd += ' ' + str(iid)
        self.send_command(cmd)
        self._expect_done()

    def set_dua_iid(self, iid: str):
        assert len(iid) == 16
        int(iid, 16)

        cmd = 'dua iid {}'.format(iid)
        self.send_command(cmd)
        self._expect_done()

    def clear_dua_iid(self):
        cmd = 'dua iid clear'
        self.send_command(cmd)
        self._expect_done()

    def multicast_listener_list(self) -> Dict[IPv6Address, int]:
        cmd = 'bbr mgmt mlr listener'
        self.send_command(cmd)

        table = {}
        for line in self._expect_results("\S+ \d+"):
            line = line.split()
            assert len(line) == 2, line
            ip = IPv6Address(line[0])
            timeout = int(line[1])
            assert ip not in table

            table[ip] = timeout

        return table

    def multicast_listener_clear(self):
        cmd = f'bbr mgmt mlr listener clear'
        self.send_command(cmd)
        self._expect_done()

    def multicast_listener_add(self, ip: Union[IPv6Address, str], timeout: int = 0):
        if not isinstance(ip, IPv6Address):
            ip = IPv6Address(ip)

        cmd = f'bbr mgmt mlr listener add {ip.compressed} {timeout}'
        self.send_command(cmd)
        self._expect(r"(Done|Error .*)")

    def set_next_mlr_response(self, status: int):
        cmd = 'bbr mgmt mlr response {}'.format(status)
        self.send_command(cmd)
        self._expect_done()

    def register_multicast_listener(self, *ipaddrs: Union[IPv6Address, str], timeout=None):
        assert len(ipaddrs) > 0, ipaddrs

        ipaddrs = map(str, ipaddrs)
        cmd = f'mlr reg {" ".join(ipaddrs)}'
        if timeout is not None:
            cmd += f' {int(timeout)}'
        self.send_command(cmd)
        self.simulator.go(3)
        lines = self._expect_command_output(cmd)
        m = re.match(r'status (\d+), (\d+) failed', lines[0])
        assert m is not None, lines
        status = int(m.group(1))
        failed_num = int(m.group(2))
        assert failed_num == len(lines) - 1
        failed_ips = list(map(IPv6Address, lines[1:]))
        print(f"register_multicast_listener {ipaddrs} => status: {status}, failed ips: {failed_ips}")
        return status, failed_ips

    def set_link_quality(self, addr, lqi):
        cmd = 'macfilter rss add-lqi %s %s' % (addr, lqi)
        self.send_command(cmd)
        self._expect_done()

    def set_outbound_link_quality(self, lqi):
        cmd = 'macfilter rss add-lqi * %s' % (lqi)
        self.send_command(cmd)
        self._expect_done()

    def remove_allowlist(self, addr):
        cmd = 'macfilter addr remove %s' % addr
        self.send_command(cmd)
        self._expect_done()

    def get_addr16(self):
        self.send_command('rloc16')
        rloc16 = self._expect_result(r'[0-9a-fA-F]{4}')
        return int(rloc16, 16)

    def get_router_id(self):
        rloc16 = self.get_addr16()
        return rloc16 >> 10

    def get_addr64(self):
        self.send_command('extaddr')
        return self._expect_result('[0-9a-fA-F]{16}')

    def set_addr64(self, addr64: str):
        # Make sure `addr64` is a hex string of length 16
        assert len(addr64) == 16
        int(addr64, 16)
        self.send_command('extaddr %s' % addr64)
        self._expect_done()

    def get_eui64(self):
        self.send_command('eui64')
        return self._expect_result('[0-9a-fA-F]{16}')

    def set_extpanid(self, extpanid):
        self.send_command('extpanid %s' % extpanid)
        self._expect_done()

    def get_joiner_id(self):
        self.send_command('joiner id')
        return self._expect_result('[0-9a-fA-F]{16}')

    def get_channel(self):
        self.send_command('channel')
        return int(self._expect_result(r'\d+'))

    def set_channel(self, channel):
        cmd = 'channel %d' % channel
        self.send_command(cmd)
        self._expect_done()

    def get_networkkey(self):
        self.send_command('networkkey')
        return self._expect_result('[0-9a-fA-F]{32}')

    def set_networkkey(self, networkkey):
        cmd = 'networkkey %s' % networkkey
        self.send_command(cmd)
        self._expect_done()

    def get_key_sequence_counter(self):
        self.send_command('keysequence counter')
        result = self._expect_result(r'\d+')
        return int(result)

    def set_key_sequence_counter(self, key_sequence_counter):
        cmd = 'keysequence counter %d' % key_sequence_counter
        self.send_command(cmd)
        self._expect_done()

    def set_key_switch_guardtime(self, key_switch_guardtime):
        cmd = 'keysequence guardtime %d' % key_switch_guardtime
        self.send_command(cmd)
        self._expect_done()

    def set_network_id_timeout(self, network_id_timeout):
        cmd = 'networkidtimeout %d' % network_id_timeout
        self.send_command(cmd)
        self._expect_done()

    def _escape_escapable(self, string):
        """Escape CLI escapable characters in the given string.

        Args:
            string (str): UTF-8 input string.

        Returns:
            [str]: The modified string with escaped characters.
        """
        escapable_chars = '\\ \t\r\n'
        for char in escapable_chars:
            string = string.replace(char, '\\%s' % char)
        return string

    def get_network_name(self):
        self.send_command('networkname')
        return self._expect_result([r'\S+'])

    def set_network_name(self, network_name):
        cmd = 'networkname %s' % self._escape_escapable(network_name)
        self.send_command(cmd)
        self._expect_done()

    def get_panid(self):
        self.send_command('panid')
        result = self._expect_result('0x[0-9a-fA-F]{4}')
        return int(result, 16)

    def set_panid(self, panid=config.PANID):
        cmd = 'panid %d' % panid
        self.send_command(cmd)
        self._expect_done()

    def set_parent_priority(self, priority):
        cmd = 'parentpriority %d' % priority
        self.send_command(cmd)
        self._expect_done()

    def get_partition_id(self):
        self.send_command('partitionid')
        return self._expect_result(r'\d+')

    def get_preferred_partition_id(self):
        self.send_command('partitionid preferred')
        return self._expect_result(r'\d+')

    def set_preferred_partition_id(self, partition_id):
        cmd = 'partitionid preferred %d' % partition_id
        self.send_command(cmd)
        self._expect_done()

    def get_pollperiod(self):
        self.send_command('pollperiod')
        return self._expect_result(r'\d+')

    def set_pollperiod(self, pollperiod):
        self.send_command('pollperiod %d' % pollperiod)
        self._expect_done()

    def get_csl_info(self):
        self.send_command('csl')
        self._expect_done()

    def set_csl_channel(self, csl_channel):
        self.send_command('csl channel %d' % csl_channel)
        self._expect_done()

    def set_csl_period(self, csl_period):
        self.send_command('csl period %d' % csl_period)
        self._expect_done()

    def set_csl_timeout(self, csl_timeout):
        self.send_command('csl timeout %d' % csl_timeout)
        self._expect_done()

    def send_mac_emptydata(self):
        self.send_command('mac send emptydata')
        self._expect_done()

    def send_mac_datarequest(self):
        self.send_command('mac send datarequest')
        self._expect_done()

    def set_router_upgrade_threshold(self, threshold):
        cmd = 'routerupgradethreshold %d' % threshold
        self.send_command(cmd)
        self._expect_done()

    def set_router_downgrade_threshold(self, threshold):
        cmd = 'routerdowngradethreshold %d' % threshold
        self.send_command(cmd)
        self._expect_done()

    def get_router_downgrade_threshold(self) -> int:
        self.send_command('routerdowngradethreshold')
        return int(self._expect_result(r'\d+'))

    def set_router_eligible(self, enable: bool):
        cmd = f'routereligible {"enable" if enable else "disable"}'
        self.send_command(cmd)
        self._expect_done()

    def get_router_eligible(self) -> bool:
        states = [r'Disabled', r'Enabled']
        self.send_command('routereligible')
        return self._expect_result(states) == 'Enabled'

    def prefer_router_id(self, router_id):
        cmd = 'preferrouterid %d' % router_id
        self.send_command(cmd)
        self._expect_done()

    def release_router_id(self, router_id):
        cmd = 'releaserouterid %d' % router_id
        self.send_command(cmd)
        self._expect_done()

    def get_state(self):
        states = [r'detached', r'child', r'router', r'leader', r'disabled']
        self.send_command('state')
        return self._expect_result(states)

    def set_state(self, state):
        cmd = 'state %s' % state
        self.send_command(cmd)
        self._expect_done()

    def get_timeout(self):
        self.send_command('childtimeout')
        return self._expect_result(r'\d+')

    def set_timeout(self, timeout):
        cmd = 'childtimeout %d' % timeout
        self.send_command(cmd)
        self._expect_done()

    def set_max_children(self, number):
        cmd = 'childmax %d' % number
        self.send_command(cmd)
        self._expect_done()

    def get_weight(self):
        self.send_command('leaderweight')
        return self._expect_result(r'\d+')

    def set_weight(self, weight):
        cmd = 'leaderweight %d' % weight
        self.send_command(cmd)
        self._expect_done()

    def add_ipaddr(self, ipaddr):
        cmd = 'ipaddr add %s' % ipaddr
        self.send_command(cmd)
        self._expect_done()

    def del_ipaddr(self, ipaddr):
        cmd = 'ipaddr del %s' % ipaddr
        self.send_command(cmd)
        self._expect_done()

    def add_ipmaddr(self, ipmaddr):
        cmd = 'ipmaddr add %s' % ipmaddr
        self.send_command(cmd)
        self._expect_done()

    def del_ipmaddr(self, ipmaddr):
        cmd = 'ipmaddr del %s' % ipmaddr
        self.send_command(cmd)
        self._expect_done()

    def get_addrs(self):
        self.send_command('ipaddr')

        return self._expect_results(r'\S+(:\S*)+')

    def get_mleid(self):
        self.send_command('ipaddr mleid')
        return self._expect_result(r'\S+(:\S*)+')

    def get_linklocal(self):
        self.send_command('ipaddr linklocal')
        return self._expect_result(r'\S+(:\S*)+')

    def get_rloc(self):
        self.send_command('ipaddr rloc')
        return self._expect_result(r'\S+(:\S*)+')

    def get_addr(self, prefix):
        network = ipaddress.ip_network(u'%s' % str(prefix))
        addrs = self.get_addrs()

        for addr in addrs:
            if isinstance(addr, bytearray):
                addr = bytes(addr)
            ipv6_address = ipaddress.ip_address(addr)
            if ipv6_address in network:
                return ipv6_address.exploded

        return None

    def has_ipaddr(self, address):
        ipaddr = ipaddress.ip_address(address)
        ipaddrs = self.get_addrs()
        for addr in ipaddrs:
            if isinstance(addr, bytearray):
                addr = bytes(addr)
            if ipaddress.ip_address(addr) == ipaddr:
                return True
        return False

    def get_ipmaddrs(self):
        self.send_command('ipmaddr')
        return self._expect_results(r'\S+(:\S*)+')

    def has_ipmaddr(self, address):
        ipmaddr = ipaddress.ip_address(address)
        ipmaddrs = self.get_ipmaddrs()
        for addr in ipmaddrs:
            if isinstance(addr, bytearray):
                addr = bytes(addr)
            if ipaddress.ip_address(addr) == ipmaddr:
                return True
        return False

    def get_addr_leader_aloc(self):
        addrs = self.get_addrs()
        for addr in addrs:
            segs = addr.split(':')
            if (segs[4] == '0' and segs[5] == 'ff' and segs[6] == 'fe00' and segs[7] == 'fc00'):
                return addr
        return None

    def get_mleid_iid(self):
        ml_eid = IPv6Address(self.get_mleid())
        return ml_eid.packed[8:].hex()

    def get_eidcaches(self):
        eidcaches = []
        self.send_command('eidcache')

        pattern = self._prepare_pattern(r'([a-fA-F0-9\:]+) ([a-fA-F0-9]+)')
        while self._expect(pattern):
            eid = self.pexpect.match.groups()[0].decode("utf-8")
            rloc = self.pexpect.match.groups()[1].decode("utf-8")
            eidcaches.append((eid, rloc))

        return eidcaches

    def add_service(self, enterpriseNumber, serviceData, serverData):
        cmd = 'service add %s %s %s' % (
            enterpriseNumber,
            serviceData,
            serverData,
        )
        self.send_command(cmd)
        self._expect_done()

    def remove_service(self, enterpriseNumber, serviceData):
        cmd = 'service remove %s %s' % (enterpriseNumber, serviceData)
        self.send_command(cmd)
        self._expect_done()

    def get_child_table(self) -> Dict[int, Dict[str, Any]]:
        """Get the table of attached children."""
        cmd = 'child table'
        self.send_command(cmd)
        output = self._expect_command_output(cmd)

        #
        # Example output:
        # | ID  | RLOC16 | Timeout    | Age        | LQ In | C_VN |R|D|N|Ver|CSL|QMsgCnt| Extended MAC     |
        # +-----+--------+------------+------------+-------+------+-+-+-+---+---+-------+------------------+
        # |   1 | 0xc801 |        240 |         24 |     3 |  131 |1|0|0|  3| 0 |     0 | 4ecede68435358ac |
        # |   2 | 0xc802 |        240 |          2 |     3 |  131 |0|0|0|  3| 1 |     0 | a672a601d2ce37d8 |
        # Done
        #

        headers = self.__split_table_row(output[0])

        table = {}
        for line in output[2:]:
            line = line.strip()
            if not line:
                continue

            fields = self.__split_table_row(line)
            col = lambda colname: self.__get_table_col(colname, headers, fields)

            id = int(col("ID"))
            r, d, n = int(col("R")), int(col("D")), int(col("N"))
            mode = f'{"r" if r else ""}{"d" if d else ""}{"n" if n else ""}'

            table[int(id)] = {
                'id': int(id),
                'rloc16': int(col('RLOC16'), 16),
                'timeout': int(col('Timeout')),
                'age': int(col('Age')),
                'lq_in': int(col('LQ In')),
                'c_vn': int(col('C_VN')),
                'mode': mode,
                'extaddr': col('Extended MAC'),
                'ver': int(col('Ver')),
                'csl': bool(int(col('CSL'))),
                'qmsgcnt': int(col('QMsgCnt')),
            }

        return table

    def __split_table_row(self, row: str) -> List[str]:
        if not (row.startswith('|') and row.endswith('|')):
            raise ValueError(row)

        fields = row.split('|')
        fields = [x.strip() for x in fields[1:-1]]
        return fields

    def __get_table_col(self, colname: str, headers: List[str], fields: List[str]) -> str:
        return fields[headers.index(colname)]

    def __getOmrAddress(self):
        prefixes = [prefix.split('::')[0] for prefix in self.get_prefixes()]
        omr_addrs = []
        for addr in self.get_addrs():
            for prefix in prefixes:
                if (addr.startswith(prefix)):
                    omr_addrs.append(addr)
                    break

        return omr_addrs

    def __getLinkLocalAddress(self):
        for ip6Addr in self.get_addrs():
            if re.match(config.LINK_LOCAL_REGEX_PATTERN, ip6Addr, re.I):
                return ip6Addr

        return None

    def __getGlobalAddress(self):
        global_address = []
        for ip6Addr in self.get_addrs():
            if ((not re.match(config.LINK_LOCAL_REGEX_PATTERN, ip6Addr, re.I)) and
                (not re.match(config.MESH_LOCAL_PREFIX_REGEX_PATTERN, ip6Addr, re.I)) and
                (not re.match(config.ROUTING_LOCATOR_REGEX_PATTERN, ip6Addr, re.I))):
                global_address.append(ip6Addr)

        return global_address

    def __getRloc(self):
        for ip6Addr in self.get_addrs():
            if (re.match(config.MESH_LOCAL_PREFIX_REGEX_PATTERN, ip6Addr, re.I) and
                    re.match(config.ROUTING_LOCATOR_REGEX_PATTERN, ip6Addr, re.I) and
                    not (re.match(config.ALOC_FLAG_REGEX_PATTERN, ip6Addr, re.I))):
                return ip6Addr
        return None

    def __getAloc(self):
        aloc = []
        for ip6Addr in self.get_addrs():
            if (re.match(config.MESH_LOCAL_PREFIX_REGEX_PATTERN, ip6Addr, re.I) and
                    re.match(config.ROUTING_LOCATOR_REGEX_PATTERN, ip6Addr, re.I) and
                    re.match(config.ALOC_FLAG_REGEX_PATTERN, ip6Addr, re.I)):
                aloc.append(ip6Addr)

        return aloc

    def __getMleid(self):
        for ip6Addr in self.get_addrs():
            if re.match(config.MESH_LOCAL_PREFIX_REGEX_PATTERN, ip6Addr,
                        re.I) and not (re.match(config.ROUTING_LOCATOR_REGEX_PATTERN, ip6Addr, re.I)):
                return ip6Addr

        return None

    def __getDua(self) -> Optional[str]:
        for ip6Addr in self.get_addrs():
            if re.match(config.DOMAIN_PREFIX_REGEX_PATTERN, ip6Addr, re.I):
                return ip6Addr

        return None

    def get_ip6_address_by_prefix(self, prefix: Union[str, IPv6Network]) -> List[IPv6Address]:
        """Get addresses matched with given prefix.

        Args:
            prefix: the prefix to match against.
                    Can be either a string or ipaddress.IPv6Network.

        Returns:
            The IPv6 address list.
        """
        if isinstance(prefix, str):
            prefix = IPv6Network(prefix)
        addrs = map(IPv6Address, self.get_addrs())

        return [addr for addr in addrs if addr in prefix]

    def get_ip6_address(self, address_type):
        """Get specific type of IPv6 address configured on thread device.

        Args:
            address_type: the config.ADDRESS_TYPE type of IPv6 address.

        Returns:
            IPv6 address string.
        """
        if address_type == config.ADDRESS_TYPE.LINK_LOCAL:
            return self.__getLinkLocalAddress()
        elif address_type == config.ADDRESS_TYPE.GLOBAL:
            return self.__getGlobalAddress()
        elif address_type == config.ADDRESS_TYPE.RLOC:
            return self.__getRloc()
        elif address_type == config.ADDRESS_TYPE.ALOC:
            return self.__getAloc()
        elif address_type == config.ADDRESS_TYPE.ML_EID:
            return self.__getMleid()
        elif address_type == config.ADDRESS_TYPE.DUA:
            return self.__getDua()
        elif address_type == config.ADDRESS_TYPE.BACKBONE_GUA:
            return self._getBackboneGua()
        elif address_type == config.ADDRESS_TYPE.OMR:
            return self.__getOmrAddress()
        else:
            return None

    def get_context_reuse_delay(self):
        self.send_command('contextreusedelay')
        return self._expect_result(r'\d+')

    def set_context_reuse_delay(self, delay):
        cmd = 'contextreusedelay %d' % delay
        self.send_command(cmd)
        self._expect_done()

    def add_prefix(self, prefix, flags='paosr', prf='med'):
        cmd = 'prefix add %s %s %s' % (prefix, flags, prf)
        self.send_command(cmd)
        self._expect_done()

    def remove_prefix(self, prefix):
        cmd = 'prefix remove %s' % prefix
        self.send_command(cmd)
        self._expect_done()

    def enable_br(self):
        self.send_command('br enable')
        self._expect_done()

    def disable_br(self):
        self.send_command('br disable')
        self._expect_done()

    def get_omr_prefix(self):
        cmd = 'br omrprefix'
        self.send_command(cmd)
        return self._expect_command_output(cmd)[0]

    def get_on_link_prefix(self):
        cmd = 'br onlinkprefix'
        self.send_command(cmd)
        return self._expect_command_output(cmd)[0]

    def get_prefixes(self):
        return self.get_netdata()['Prefixes']

    def get_routes(self):
        return self.get_netdata()['Routes']

    def get_services(self):
        netdata = self.netdata_show()
        services = []
        services_section = False

        for line in netdata:
            if line.startswith('Services:'):
                services_section = True
            elif services_section:
                services.append(line.strip().split(' '))
        return services

    def netdata_show(self):
        self.send_command('netdata show')
        return self._expect_command_output('netdata show')

    def get_netdata(self):
        raw_netdata = self.netdata_show()
        netdata = {'Prefixes': [], 'Routes': [], 'Services': []}
        key_list = ['Prefixes', 'Routes', 'Services']
        key = None

        for i in range(0, len(raw_netdata)):
            keys = list(filter(raw_netdata[i].startswith, key_list))
            if keys != []:
                key = keys[0]
            elif key is not None:
                netdata[key].append(raw_netdata[i])

        return netdata

    def add_route(self, prefix, stable=False, prf='med'):
        cmd = 'route add %s ' % prefix
        if stable:
            cmd += 's'
        cmd += ' %s' % prf
        self.send_command(cmd)
        self._expect_done()

    def remove_route(self, prefix):
        cmd = 'route remove %s' % prefix
        self.send_command(cmd)
        self._expect_done()

    def register_netdata(self):
        self.send_command('netdata register')
        self._expect_done()

    def netdata_publish_dnssrp_anycast(self, seqnum):
        self.send_command(f'netdata publish dnssrp anycast {seqnum}')
        self._expect_done()

    def netdata_publish_dnssrp_unicast(self, address, port):
        self.send_command(f'netdata publish dnssrp unicast {address} {port}')
        self._expect_done()

    def netdata_publish_dnssrp_unicast_mleid(self, port):
        self.send_command(f'netdata publish dnssrp unicast {port}')
        self._expect_done()

    def netdata_unpublish_dnssrp(self):
        self.send_command('netdata unpublish dnssrp')
        self._expect_done()

    def netdata_publish_prefix(self, prefix, flags='paosr', prf='med'):
        self.send_command(f'netdata publish prefix {prefix} {flags} {prf}')
        self._expect_done()

    def netdata_publish_route(self, prefix, flags='s', prf='med'):
        self.send_command(f'netdata publish route {prefix} {flags} {prf}')
        self._expect_done()

    def netdata_unpublish_prefix(self, prefix):
        self.send_command(f'netdata unpublish {prefix}')
        self._expect_done()

    def send_network_diag_get(self, addr, tlv_types):
        self.send_command('networkdiagnostic get %s %s' % (addr, ' '.join([str(t.value) for t in tlv_types])))

        if isinstance(self.simulator, simulator.VirtualTime):
            self.simulator.go(8)
            timeout = 1
        else:
            timeout = 8

        self._expect_done(timeout=timeout)

    def send_network_diag_reset(self, addr, tlv_types):
        self.send_command('networkdiagnostic reset %s %s' % (addr, ' '.join([str(t.value) for t in tlv_types])))

        if isinstance(self.simulator, simulator.VirtualTime):
            self.simulator.go(8)
            timeout = 1
        else:
            timeout = 8

        self._expect_done(timeout=timeout)

    def energy_scan(self, mask, count, period, scan_duration, ipaddr):
        cmd = 'commissioner energy %d %d %d %d %s' % (
            mask,
            count,
            period,
            scan_duration,
            ipaddr,
        )
        self.send_command(cmd)

        if isinstance(self.simulator, simulator.VirtualTime):
            self.simulator.go(8)
            timeout = 1
        else:
            timeout = 8

        self._expect('Energy:', timeout=timeout)

    def panid_query(self, panid, mask, ipaddr):
        cmd = 'commissioner panid %d %d %s' % (panid, mask, ipaddr)
        self.send_command(cmd)

        if isinstance(self.simulator, simulator.VirtualTime):
            self.simulator.go(8)
            timeout = 1
        else:
            timeout = 8

        self._expect('Conflict:', timeout=timeout)

    def scan(self, result=1):
        self.send_command('scan')

        if result == 1:
            return self._expect_results(
                r'\|\s(\S+)\s+\|\s(\S+)\s+\|\s([0-9a-fA-F]{4})\s\|\s([0-9a-fA-F]{16})\s\|\s(\d+)')

    def ping(self, ipaddr, num_responses=1, size=8, timeout=5, count=1, interval=1, hoplimit=64, interface=None):
        args = f'{ipaddr} {size} {count} {interval} {hoplimit} {timeout}'
        if interface is not None:
            args = f'-I {interface} {args}'
        cmd = f'ping {args}'

        self.send_command(cmd)

        wait_allowance = 3
        end = self.simulator.now() + timeout + wait_allowance

        responders = {}

        result = True
        # ncp-sim doesn't print Done
        done = (self.node_type == 'ncp-sim')
        while len(responders) < num_responses or not done:
            self.simulator.go(1)
            try:
                i = self._expect([r'from (\S+):', r'Done'], timeout=0.1)
            except (pexpect.TIMEOUT, socket.timeout):
                if self.simulator.now() < end:
                    continue
                result = False
                if isinstance(self.simulator, simulator.VirtualTime):
                    self.simulator.sync_devices()
                break
            else:
                if i == 0:
                    responders[self.pexpect.match.groups()[0]] = 1
                elif i == 1:
                    done = True
        return result

    def reset(self):
        self.send_command('reset')
        time.sleep(self.RESET_DELAY)

        if self.is_otbr:
            self.set_log_level(5)

    def set_router_selection_jitter(self, jitter):
        cmd = 'routerselectionjitter %d' % jitter
        self.send_command(cmd)
        self._expect_done()

    def set_active_dataset(
        self,
        timestamp,
        panid=None,
        channel=None,
        channel_mask=None,
        network_key=None,
        security_policy=[],
    ):
        self.send_command('dataset clear')
        self._expect_done()

        cmd = 'dataset activetimestamp %d' % timestamp
        self.send_command(cmd)
        self._expect_done()

        if panid is not None:
            cmd = 'dataset panid %d' % panid
            self.send_command(cmd)
            self._expect_done()

        if channel is not None:
            cmd = 'dataset channel %d' % channel
            self.send_command(cmd)
            self._expect_done()

        if channel_mask is not None:
            cmd = 'dataset channelmask %d' % channel_mask
            self.send_command(cmd)
            self._expect_done()

        if network_key is not None:
            cmd = 'dataset networkkey %s' % network_key
            self.send_command(cmd)
            self._expect_done()

        if security_policy and len(security_policy) == 2:
            cmd = 'dataset securitypolicy %s %s' % (
                str(security_policy[0]),
                security_policy[1],
            )
            self.send_command(cmd)
            self._expect_done()

        # Set the meshlocal prefix in config.py
        self.send_command('dataset meshlocalprefix %s' % config.MESH_LOCAL_PREFIX.split('/')[0])
        self._expect_done()

        self.send_command('dataset commit active')
        self._expect_done()

    def set_pending_dataset(self, pendingtimestamp, activetimestamp, panid=None, channel=None, delay=None):
        self.send_command('dataset clear')
        self._expect_done()

        cmd = 'dataset pendingtimestamp %d' % pendingtimestamp
        self.send_command(cmd)
        self._expect_done()

        cmd = 'dataset activetimestamp %d' % activetimestamp
        self.send_command(cmd)
        self._expect_done()

        if panid is not None:
            cmd = 'dataset panid %d' % panid
            self.send_command(cmd)
            self._expect_done()

        if channel is not None:
            cmd = 'dataset channel %d' % channel
            self.send_command(cmd)
            self._expect_done()

        if delay is not None:
            cmd = 'dataset delay %d' % delay
            self.send_command(cmd)
            self._expect_done()

        # Set the meshlocal prefix in config.py
        self.send_command('dataset meshlocalprefix %s' % config.MESH_LOCAL_PREFIX.split('/')[0])
        self._expect_done()

        self.send_command('dataset commit pending')
        self._expect_done()

    def start_dataset_updater(self, panid=None, channel=None):
        self.send_command('dataset clear')
        self._expect_done()

        if panid is not None:
            cmd = 'dataset panid %d' % panid
            self.send_command(cmd)
            self._expect_done()

        if channel is not None:
            cmd = 'dataset channel %d' % channel
            self.send_command(cmd)
            self._expect_done()

        self.send_command('dataset updater start')
        self._expect_done()

    def announce_begin(self, mask, count, period, ipaddr):
        cmd = 'commissioner announce %d %d %d %s' % (
            mask,
            count,
            period,
            ipaddr,
        )
        self.send_command(cmd)
        self._expect_done()

    def send_mgmt_active_set(
        self,
        active_timestamp=None,
        channel=None,
        channel_mask=None,
        extended_panid=None,
        panid=None,
        network_key=None,
        mesh_local=None,
        network_name=None,
        security_policy=None,
        binary=None,
    ):
        cmd = 'dataset mgmtsetcommand active '

        if active_timestamp is not None:
            cmd += 'activetimestamp %d ' % active_timestamp

        if channel is not None:
            cmd += 'channel %d ' % channel

        if channel_mask is not None:
            cmd += 'channelmask %d ' % channel_mask

        if extended_panid is not None:
            cmd += 'extpanid %s ' % extended_panid

        if panid is not None:
            cmd += 'panid %d ' % panid

        if network_key is not None:
            cmd += 'networkkey %s ' % network_key

        if mesh_local is not None:
            cmd += 'localprefix %s ' % mesh_local

        if network_name is not None:
            cmd += 'networkname %s ' % self._escape_escapable(network_name)

        if security_policy is not None:
            rotation, flags = security_policy
            cmd += 'securitypolicy %d %s ' % (rotation, flags)

        if binary is not None:
            cmd += '-x %s ' % binary

        self.send_command(cmd)
        self._expect_done()

    def send_mgmt_active_get(self, addr='', tlvs=[]):
        cmd = 'dataset mgmtgetcommand active'

        if addr != '':
            cmd += ' address '
            cmd += addr

        if len(tlvs) != 0:
            tlv_str = ''.join('%02x' % tlv for tlv in tlvs)
            cmd += ' -x '
            cmd += tlv_str

        self.send_command(cmd)
        self._expect_done()

    def send_mgmt_pending_get(self, addr='', tlvs=[]):
        cmd = 'dataset mgmtgetcommand pending'

        if addr != '':
            cmd += ' address '
            cmd += addr

        if len(tlvs) != 0:
            tlv_str = ''.join('%02x' % tlv for tlv in tlvs)
            cmd += ' -x '
            cmd += tlv_str

        self.send_command(cmd)
        self._expect_done()

    def send_mgmt_pending_set(
        self,
        pending_timestamp=None,
        active_timestamp=None,
        delay_timer=None,
        channel=None,
        panid=None,
        network_key=None,
        mesh_local=None,
        network_name=None,
    ):
        cmd = 'dataset mgmtsetcommand pending '
        if pending_timestamp is not None:
            cmd += 'pendingtimestamp %d ' % pending_timestamp

        if active_timestamp is not None:
            cmd += 'activetimestamp %d ' % active_timestamp

        if delay_timer is not None:
            cmd += 'delaytimer %d ' % delay_timer

        if channel is not None:
            cmd += 'channel %d ' % channel

        if panid is not None:
            cmd += 'panid %d ' % panid

        if network_key is not None:
            cmd += 'networkkey %s ' % network_key

        if mesh_local is not None:
            cmd += 'localprefix %s ' % mesh_local

        if network_name is not None:
            cmd += 'networkname %s ' % self._escape_escapable(network_name)

        self.send_command(cmd)
        self._expect_done()

    def coap_cancel(self):
        """
        Cancel a CoAP subscription.
        """
        cmd = 'coap cancel'
        self.send_command(cmd)
        self._expect_done()

    def coap_delete(self, ipaddr, uri, con=False, payload=None):
        """
        Send a DELETE request via CoAP.
        """
        return self._coap_rq('delete', ipaddr, uri, con, payload)

    def coap_get(self, ipaddr, uri, con=False, payload=None):
        """
        Send a GET request via CoAP.
        """
        return self._coap_rq('get', ipaddr, uri, con, payload)

    def coap_get_block(self, ipaddr, uri, size=16, count=0):
        """
        Send a GET request via CoAP.
        """
        return self._coap_rq_block('get', ipaddr, uri, size, count)

    def coap_observe(self, ipaddr, uri, con=False, payload=None):
        """
        Send a GET request via CoAP with Observe set.
        """
        return self._coap_rq('observe', ipaddr, uri, con, payload)

    def coap_post(self, ipaddr, uri, con=False, payload=None):
        """
        Send a POST request via CoAP.
        """
        return self._coap_rq('post', ipaddr, uri, con, payload)

    def coap_post_block(self, ipaddr, uri, size=16, count=0):
        """
        Send a POST request via CoAP.
        """
        return self._coap_rq_block('post', ipaddr, uri, size, count)

    def coap_put(self, ipaddr, uri, con=False, payload=None):
        """
        Send a PUT request via CoAP.
        """
        return self._coap_rq('put', ipaddr, uri, con, payload)

    def coap_put_block(self, ipaddr, uri, size=16, count=0):
        """
        Send a PUT request via CoAP.
        """
        return self._coap_rq_block('put', ipaddr, uri, size, count)

    def _coap_rq(self, method, ipaddr, uri, con=False, payload=None):
        """
        Issue a GET/POST/PUT/DELETE/GET OBSERVE request.
        """
        cmd = 'coap %s %s %s' % (method, ipaddr, uri)
        if con:
            cmd += ' con'
        else:
            cmd += ' non'

        if payload is not None:
            cmd += ' %s' % payload

        self.send_command(cmd)
        return self.coap_wait_response()

    def _coap_rq_block(self, method, ipaddr, uri, size=16, count=0):
        """
        Issue a GET/POST/PUT/DELETE/GET OBSERVE BLOCK request.
        """
        cmd = 'coap %s %s %s' % (method, ipaddr, uri)

        cmd += ' block-%d' % size

        if count != 0:
            cmd += ' %d' % count

        self.send_command(cmd)
        return self.coap_wait_response()

    def coap_wait_response(self):
        """
        Wait for a CoAP response, and return it.
        """
        if isinstance(self.simulator, simulator.VirtualTime):
            self.simulator.go(5)
            timeout = 1
        else:
            timeout = 5

        self._expect(r'coap response from ([\da-f:]+)(?: OBS=(\d+))?'
                     r'(?: with payload: ([\da-f]+))?\b',
                     timeout=timeout)
        (source, observe, payload) = self.pexpect.match.groups()
        source = source.decode('UTF-8')

        if observe is not None:
            observe = int(observe, base=10)

        if payload is not None:
            try:
                payload = binascii.a2b_hex(payload).decode('UTF-8')
            except UnicodeDecodeError:
                pass

        # Return the values received
        return dict(source=source, observe=observe, payload=payload)

    def coap_wait_request(self):
        """
        Wait for a CoAP request to be made.
        """
        if isinstance(self.simulator, simulator.VirtualTime):
            self.simulator.go(5)
            timeout = 1
        else:
            timeout = 5

        self._expect(r'coap request from ([\da-f:]+)(?: OBS=(\d+))?'
                     r'(?: with payload: ([\da-f]+))?\b',
                     timeout=timeout)
        (source, observe, payload) = self.pexpect.match.groups()
        source = source.decode('UTF-8')

        if observe is not None:
            observe = int(observe, base=10)

        if payload is not None:
            payload = binascii.a2b_hex(payload).decode('UTF-8')

        # Return the values received
        return dict(source=source, observe=observe, payload=payload)

    def coap_wait_subscribe(self):
        """
        Wait for a CoAP client to be subscribed.
        """
        if isinstance(self.simulator, simulator.VirtualTime):
            self.simulator.go(5)
            timeout = 1
        else:
            timeout = 5

        self._expect(r'Subscribing client\b', timeout=timeout)

    def coap_wait_ack(self):
        """
        Wait for a CoAP notification ACK.
        """
        if isinstance(self.simulator, simulator.VirtualTime):
            self.simulator.go(5)
            timeout = 1
        else:
            timeout = 5

        self._expect(r'Received ACK in reply to notification ' r'from ([\da-f:]+)\b', timeout=timeout)
        (source,) = self.pexpect.match.groups()
        source = source.decode('UTF-8')

        return source

    def coap_set_resource_path(self, path):
        """
        Set the path for the CoAP resource.
        """
        cmd = 'coap resource %s' % path
        self.send_command(cmd)
        self._expect_done()

    def coap_set_resource_path_block(self, path, count=0):
        """
        Set the path for the CoAP resource and how many blocks can be received from this resource.
        """
        cmd = 'coap resource %s %d' % (path, count)
        self.send_command(cmd)
        self._expect('Done')

    def coap_set_content(self, content):
        """
        Set the content of the CoAP resource.
        """
        cmd = 'coap set %s' % content
        self.send_command(cmd)
        self._expect_done()

    def coap_start(self):
        """
        Start the CoAP service.
        """
        cmd = 'coap start'
        self.send_command(cmd)
        self._expect_done()

    def coap_stop(self):
        """
        Stop the CoAP service.
        """
        cmd = 'coap stop'
        self.send_command(cmd)

        if isinstance(self.simulator, simulator.VirtualTime):
            self.simulator.go(5)
            timeout = 1
        else:
            timeout = 5

        self._expect_done(timeout=timeout)

    def coaps_start_psk(self, psk, pskIdentity):
        cmd = 'coaps psk %s %s' % (psk, pskIdentity)
        self.send_command(cmd)
        self._expect_done()

        cmd = 'coaps start'
        self.send_command(cmd)
        self._expect_done()

    def coaps_start_x509(self):
        cmd = 'coaps x509'
        self.send_command(cmd)
        self._expect_done()

        cmd = 'coaps start'
        self.send_command(cmd)
        self._expect_done()

    def coaps_set_resource_path(self, path):
        cmd = 'coaps resource %s' % path
        self.send_command(cmd)
        self._expect_done()

    def coaps_stop(self):
        cmd = 'coaps stop'
        self.send_command(cmd)

        if isinstance(self.simulator, simulator.VirtualTime):
            self.simulator.go(5)
            timeout = 1
        else:
            timeout = 5

        self._expect_done(timeout=timeout)

    def coaps_connect(self, ipaddr):
        cmd = 'coaps connect %s' % ipaddr
        self.send_command(cmd)

        if isinstance(self.simulator, simulator.VirtualTime):
            self.simulator.go(5)
            timeout = 1
        else:
            timeout = 5

        self._expect('coaps connected', timeout=timeout)

    def coaps_disconnect(self):
        cmd = 'coaps disconnect'
        self.send_command(cmd)
        self._expect_done()
        self.simulator.go(5)

    def coaps_get(self):
        cmd = 'coaps get test'
        self.send_command(cmd)

        if isinstance(self.simulator, simulator.VirtualTime):
            self.simulator.go(5)
            timeout = 1
        else:
            timeout = 5

        self._expect('coaps response', timeout=timeout)

    def commissioner_mgmtget(self, tlvs_binary=None):
        cmd = 'commissioner mgmtget'
        if tlvs_binary is not None:
            cmd += ' -x %s' % tlvs_binary
        self.send_command(cmd)
        self._expect_done()

    def commissioner_mgmtset(self, tlvs_binary):
        cmd = 'commissioner mgmtset -x %s' % tlvs_binary
        self.send_command(cmd)
        self._expect_done()

    def bytes_to_hex_str(self, src):
        return ''.join(format(x, '02x') for x in src)

    def commissioner_mgmtset_with_tlvs(self, tlvs):
        payload = bytearray()
        for tlv in tlvs:
            payload += tlv.to_hex()
        self.commissioner_mgmtset(self.bytes_to_hex_str(payload))

    def udp_start(self, local_ipaddr, local_port):
        cmd = 'udp open'
        self.send_command(cmd)
        self._expect_done()

        cmd = 'udp bind %s %s' % (local_ipaddr, local_port)
        self.send_command(cmd)
        self._expect_done()

    def udp_stop(self):
        cmd = 'udp close'
        self.send_command(cmd)
        self._expect_done()

    def udp_send(self, bytes, ipaddr, port, success=True):
        cmd = 'udp send %s %d -s %d ' % (ipaddr, port, bytes)
        self.send_command(cmd)
        if success:
            self._expect_done()
        else:
            self._expect('Error')

    def udp_check_rx(self, bytes_should_rx):
        self._expect('%d bytes' % bytes_should_rx)

    def set_routereligible(self, enable: bool):
        cmd = f'routereligible {"enable" if enable else "disable"}'
        self.send_command(cmd)
        self._expect_done()

    def router_list(self):
        cmd = 'router list'
        self.send_command(cmd)
        self._expect([r'(\d+)((\s\d+)*)'])

        g = self.pexpect.match.groups()
        router_list = g[0].decode('utf8') + ' ' + g[1].decode('utf8')
        router_list = [int(x) for x in router_list.split()]
        self._expect_done()
        return router_list

    def router_table(self):
        cmd = 'router table'
        self.send_command(cmd)

        self._expect(r'(.*)Done')
        g = self.pexpect.match.groups()
        output = g[0].decode('utf8')
        lines = output.strip().split('\n')
        lines = [l.strip() for l in lines]
        router_table = {}
        for i, line in enumerate(lines):
            if not line.startswith('|') or not line.endswith('|'):
                if i not in (0, 2):
                    # should not happen
                    print("unexpected line %d: %s" % (i, line))

                continue

            line = line[1:][:-1]
            line = [x.strip() for x in line.split('|')]
            if len(line) < 9:
                print("unexpected line %d: %s" % (i, line))
                continue

            try:
                int(line[0])
            except ValueError:
                if i != 1:
                    print("unexpected line %d: %s" % (i, line))
                continue

            id = int(line[0])
            rloc16 = int(line[1], 16)
            nexthop = int(line[2])
            pathcost = int(line[3])
            lqin = int(line[4])
            lqout = int(line[5])
            age = int(line[6])
            emac = str(line[7])
            link = int(line[8])

            router_table[id] = {
                'rloc16': rloc16,
                'nexthop': nexthop,
                'pathcost': pathcost,
                'lqin': lqin,
                'lqout': lqout,
                'age': age,
                'emac': emac,
                'link': link,
            }

        return router_table

    def link_metrics_query_single_probe(self, dst_addr: str, linkmetrics_flags: str):
        cmd = 'linkmetrics query %s single %s' % (dst_addr, linkmetrics_flags)
        self.send_command(cmd)
        self._expect_done()

    def link_metrics_query_forward_tracking_series(self, dst_addr: str, series_id: int):
        cmd = 'linkmetrics query %s forward %d' % (dst_addr, series_id)
        self.send_command(cmd)
        self._expect_done()

    def link_metrics_mgmt_req_enhanced_ack_based_probing(self,
                                                         dst_addr: str,
                                                         enable: bool,
                                                         metrics_flags: str,
                                                         ext_flags=''):
        cmd = "linkmetrics mgmt %s enhanced-ack" % (dst_addr)
        if enable:
            cmd = cmd + (" register %s %s" % (metrics_flags, ext_flags))
        else:
            cmd = cmd + " clear"
        self.send_command(cmd)
        self._expect_done()

    def link_metrics_mgmt_req_forward_tracking_series(self, dst_addr: str, series_id: int, series_flags: str,
                                                      metrics_flags: str):
        cmd = "linkmetrics mgmt %s forward %d %s %s" % (dst_addr, series_id, series_flags, metrics_flags)
        self.send_command(cmd)
        self._expect_done()

    def link_metrics_send_link_probe(self, dst_addr: str, series_id: int, length: int):
        cmd = "linkmetrics probe %s %d %d" % (dst_addr, series_id, length)
        self.send_command(cmd)
        self._expect_done()

    def send_address_notification(self, dst: str, target: str, mliid: str):
        cmd = f'fake /a/an {dst} {target} {mliid}'
        self.send_command(cmd)
        self._expect_done()

    def send_proactive_backbone_notification(self, target: str, mliid: str, ltt: int):
        cmd = f'fake /b/ba {target} {mliid} {ltt}'
        self.send_command(cmd)
        self._expect_done()

    def dns_get_config(self):
        """
        Returns the DNS config as a list of property dictionary (string key and string value).

        Example output:
        {
            'Server': '[fd00:0:0:0:0:0:0:1]:1234'
            'ResponseTimeout': '5000 ms'
            'MaxTxAttempts': '2'
            'RecursionDesired': 'no'
        }
        """
        cmd = f'dns config'
        self.send_command(cmd)
        output = self._expect_command_output(cmd)
        config = {}
        for line in output:
            k, v = line.split(': ')
            config[k] = v
        return config

    def dns_set_config(self, config):
        cmd = f'dns config {config}'
        self.send_command(cmd)
        self._expect_done()

    def dns_resolve(self, hostname, server=None, port=53):
        cmd = f'dns resolve {hostname}'
        if server is not None:
            cmd += f' {server} {port}'

        self.send_command(cmd)
        self.simulator.go(10)
        output = self._expect_command_output(cmd)
        dns_resp = output[0]
        # example output: "DNS response for host1.default.service.arpa. - fd00:db8:0:0:fd3d:d471:1e8c:b60 TTL:7190 "
        #                 " fd00:db8:0:0:0:ff:fe00:9000 TTL:7190"
        addrs = dns_resp.strip().split(' - ')[1].split(' ')
        ip = [item.strip() for item in addrs[::2]]
        ttl = [int(item.split('TTL:')[1]) for item in addrs[1::2]]

        return list(zip(ip, ttl))

    def dns_resolve_service(self, instance, service, server=None, port=53):
        """
        Resolves the service instance and returns the instance information as a dict.

        Example return value:
            {
                'port': 12345,
                'priority': 0,
                'weight': 0,
                'host': 'ins1._ipps._tcp.default.service.arpa.',
                'address': '2001::1',
                'txt_data': 'a=00, b=02bb',
                'srv_ttl': 7100,
                'txt_ttl': 7100,
                'aaaa_ttl': 7100,
            }
        """
        cmd = f'dns service {instance} {service}'
        if server is not None:
            cmd += f' {server} {port}'

        self.send_command(cmd)
        self.simulator.go(10)
        output = self._expect_command_output(cmd)

        # Example output:
        # DNS service resolution response for ins2 for service _ipps._tcp.default.service.arpa.
        # Port:22222, Priority:2, Weight:2, TTL:7155
        # Host:host2.default.service.arpa.
        # HostAddress:0:0:0:0:0:0:0:0 TTL:0
        # TXT:[a=00, b=02bb] TTL:7155
        # Done

        m = re.match(
            r'.*Port:(\d+), Priority:(\d+), Weight:(\d+), TTL:(\d+)\s+Host:(.*?)\s+HostAddress:(\S+) TTL:(\d+)\s+TXT:\[(.*?)\] TTL:(\d+)',
            '\r'.join(output))
        if m:
            port, priority, weight, srv_ttl, hostname, address, aaaa_ttl, txt_data, txt_ttl = m.groups()
            return {
                'port': int(port),
                'priority': int(priority),
                'weight': int(weight),
                'host': hostname,
                'address': address,
                'txt_data': txt_data,
                'srv_ttl': int(srv_ttl),
                'txt_ttl': int(txt_ttl),
                'aaaa_ttl': int(aaaa_ttl),
            }
        else:
            raise Exception('dns resolve service failed: %s.%s' % (instance, service))

    @staticmethod
    def __parse_hex_string(hexstr: str) -> bytes:
        assert (len(hexstr) % 2 == 0)
        return bytes(int(hexstr[i:i + 2], 16) for i in range(0, len(hexstr), 2))

    def dns_browse(self, service_name, server=None, port=53):
        """
        Browse the service and returns the instances.

        Example return value:
            {
                'ins1': {
                    'port': 12345,
                    'priority': 1,
                    'weight': 1,
                    'host': 'ins1._ipps._tcp.default.service.arpa.',
                    'address': '2001::1',
                    'txt_data': 'a=00, b=11cf',
                    'srv_ttl': 7100,
                    'txt_ttl': 7100,
                    'aaaa_ttl': 7100,
                },
                'ins2': {
                    'port': 12345,
                    'priority': 2,
                    'weight': 2,
                    'host': 'ins2._ipps._tcp.default.service.arpa.',
                    'address': '2001::2',
                    'txt_data': 'a=01, b=23dd',
                    'srv_ttl': 7100,
                    'txt_ttl': 7100,
                    'aaaa_ttl': 7100,
                }
            }
        """
        cmd = f'dns browse {service_name}'
        if server is not None:
            cmd += f' {server} {port}'

        self.send_command(cmd)
        self.simulator.go(10)
        output = '\n'.join(self._expect_command_output(cmd))

        # Example output:
        # ins2
        #     Port:22222, Priority:2, Weight:2, TTL:7175
        #     Host:host2.default.service.arpa.
        #     HostAddress:fd00:db8:0:0:3205:28dd:5b87:6a63 TTL:7175
        #     TXT:[a=00, b=11cf] TTL:7175
        # ins1
        #     Port:11111, Priority:1, Weight:1, TTL:7170
        #     Host:host1.default.service.arpa.
        #     HostAddress:fd00:db8:0:0:39f4:d9:eb4f:778 TTL:7170
        #     TXT:[a=01, b=23dd] TTL:7170
        # Done

        result = {}
        for ins, port, priority, weight, srv_ttl, hostname, address, aaaa_ttl, txt_data, txt_ttl in re.findall(
                r'(.*?)\s+Port:(\d+), Priority:(\d+), Weight:(\d+), TTL:(\d+)\s*Host:(\S+)\s+HostAddress:(\S+) TTL:(\d+)\s+TXT:\[(.*?)\] TTL:(\d+)',
                output):
            result[ins] = {
                'port': int(port),
                'priority': int(priority),
                'weight': int(weight),
                'host': hostname,
                'address': address,
                'txt_data': txt_data,
                'srv_ttl': int(srv_ttl),
                'txt_ttl': int(txt_ttl),
                'aaaa_ttl': int(aaaa_ttl),
            }

        return result

    def set_mliid(self, mliid: str):
        cmd = f'mliid {mliid}'
        self.send_command(cmd)
        self._expect_command_output(cmd)

    def history_netinfo(self, num_entries=0):
        """
        Get the `netinfo` history list, parse each entry and return
        a list of dictionary (string key and string value) entries.

        Example of return value:
        [
            {
                'age': '00:00:00.000 ago',
                'role': 'disabled',
                'mode': 'rdn',
                'rloc16': '0x7400',
                'partition-id': '1318093703'
            },
            {
                'age': '00:00:02.588 ago',
                'role': 'leader',
                'mode': 'rdn',
                'rloc16': '0x7400',
                'partition-id': '1318093703'
            }
        ]
        """
        cmd = f'history netinfo list {num_entries}'
        self.send_command(cmd)
        output = self._expect_command_output(cmd)
        netinfos = []
        for entry in output:
            netinfo = {}
            age, info = entry.split(' -> ')
            netinfo['age'] = age
            for item in info.split(' '):
                k, v = item.split(':')
                netinfo[k] = v
            netinfos.append(netinfo)
        return netinfos

    def history_rx(self, num_entries=0):
        """
        Get the IPv6 RX history list, parse each entry and return
        a list of dictionary (string key and string value) entries.

        Example of return value:
        [
            {
                'age': '00:00:01.999',
                'type': 'ICMP6(EchoReqst)',
                'len': '16',
                'sec': 'yes',
                'prio': 'norm',
                'rss': '-20',
                'from': '0xac00',
                'radio': '15.4',
                'src': '[fd00:db8:0:0:2cfa:fd61:58a9:f0aa]:0',
                'dst': '[fd00:db8:0:0:ed7e:2d04:e543:eba5]:0',
            }
        ]
        """
        cmd = f'history rx list {num_entries}'
        self.send_command(cmd)
        return self._parse_history_rx_tx_ouput(self._expect_command_output(cmd))

    def history_tx(self, num_entries=0):
        """
        Get the IPv6 TX history list, parse each entry and return
        a list of dictionary (string key and string value) entries.

        Example of return value:
        [
            {
                'age': '00:00:01.999',
                'type': 'ICMP6(EchoReply)',
                'len': '16',
                'sec': 'yes',
                'prio': 'norm',
                'to': '0xac00',
                'tx-success': 'yes',
                'radio': '15.4',
                'src': '[fd00:db8:0:0:ed7e:2d04:e543:eba5]:0',
                'dst': '[fd00:db8:0:0:2cfa:fd61:58a9:f0aa]:0',

            }
        ]
        """
        cmd = f'history tx list {num_entries}'
        self.send_command(cmd)
        return self._parse_history_rx_tx_ouput(self._expect_command_output(cmd))

    def _parse_history_rx_tx_ouput(self, lines):
        rxtx_list = []
        for line in lines:
            if line.strip().startswith('type:'):
                for item in line.strip().split(' '):
                    k, v = item.split(':')
                    entry[k] = v
            elif line.strip().startswith('src:'):
                entry['src'] = line[4:]
            elif line.strip().startswith('dst:'):
                entry['dst'] = line[4:]
                rxtx_list.append(entry)
            else:
                entry = {}
                entry['age'] = line

        return rxtx_list


class Node(NodeImpl, OtCli):
    pass


class LinuxHost():
    PING_RESPONSE_PATTERN = re.compile(r'\d+ bytes from .*:.*')
    ETH_DEV = config.BACKBONE_IFNAME

    def enable_ether(self):
        """Enable the ethernet interface.
        """

        self.bash(f'ifconfig {self.ETH_DEV} up')

    def disable_ether(self):
        """Disable the ethernet interface.
        """

        self.bash(f'ifconfig {self.ETH_DEV} down')

    def get_ether_addrs(self):
        output = self.bash(f'ip -6 addr list dev {self.ETH_DEV}')

        addrs = []
        for line in output:
            # line example: "inet6 fe80::42:c0ff:fea8:903/64 scope link"
            line = line.strip().split()

            if line and line[0] == 'inet6':
                addr = line[1]
                if '/' in addr:
                    addr = addr.split('/')[0]
                addrs.append(addr)

        logging.debug('%s: get_ether_addrs: %r', self, addrs)
        return addrs

    def get_ether_mac(self):
        output = self.bash(f'ip addr list dev {self.ETH_DEV}')
        for line in output:
            # link/ether 02:42:ac:11:00:02 brd ff:ff:ff:ff:ff:ff link-netnsid 0
            line = line.strip().split()
            if line and line[0] == 'link/ether':
                return line[1]

        assert False, output

    def add_ipmaddr_ether(self, ip: str):
        cmd = f'python3 /app/third_party/openthread/repo/tests/scripts/thread-cert/mcast6.py {self.ETH_DEV} {ip} &'
        self.bash(cmd)

    def ping_ether(self, ipaddr, num_responses=1, size=None, timeout=5, ttl=None, interface='eth0') -> int:

        cmd = f'ping -6 {ipaddr} -I {interface} -c {num_responses} -W {timeout}'
        if size is not None:
            cmd += f' -s {size}'

        if ttl is not None:
            cmd += f' -t {ttl}'

        resp_count = 0

        try:
            for line in self.bash(cmd):
                if self.PING_RESPONSE_PATTERN.match(line):
                    resp_count += 1
        except subprocess.CalledProcessError:
            pass

        return resp_count

    def _getBackboneGua(self) -> Optional[str]:
        for addr in self.get_ether_addrs():
            if re.match(config.BACKBONE_PREFIX_REGEX_PATTERN, addr, re.I):
                return addr

        return None

    def _getInfraUla(self) -> Optional[str]:
        """ Returns the ULA addresses autoconfigured on the infra link.
        """
        addrs = []
        for addr in self.get_ether_addrs():
            if re.match(config.ONLINK_PREFIX_REGEX_PATTERN, addr, re.I):
                addrs.append(addr)

        return addrs

    def _getInfraGua(self) -> Optional[str]:
        """ Returns the GUA addresses autoconfigured on the infra link.
        """

        gua_prefix = config.ONLINK_GUA_PREFIX.split('::/')[0]
        return [addr for addr in self.get_ether_addrs() if addr.startswith(gua_prefix)]

    def ping(self, *args, **kwargs):
        backbone = kwargs.pop('backbone', False)
        if backbone:
            return self.ping_ether(*args, **kwargs)
        else:
            return super().ping(*args, **kwargs)

    def udp_send_host(self, ipaddr, port, data, hop_limit=None):
        if hop_limit is None:
            if ipaddress.ip_address(ipaddr).is_multicast:
                hop_limit = 10
            else:
                hop_limit = 64
        cmd = f'python3 /app/third_party/openthread/repo/tests/scripts/thread-cert/udp_send_host.py {ipaddr} {port} "{data}" {hop_limit}'
        self.bash(cmd)

    def add_ipmaddr(self, *args, **kwargs):
        backbone = kwargs.pop('backbone', False)
        if backbone:
            return self.add_ipmaddr_ether(*args, **kwargs)
        else:
            return super().add_ipmaddr(*args, **kwargs)

    def ip_neighbors_flush(self):
        # clear neigh cache on linux
        self.bash(f'ip -6 neigh list dev {self.ETH_DEV}')
        self.bash(f'ip -6 neigh flush nud all nud failed nud noarp dev {self.ETH_DEV}')
        self.bash('ip -6 neigh list nud all dev %s | cut -d " " -f1 | sudo xargs -I{} ip -6 neigh delete {} dev %s' %
                  (self.ETH_DEV, self.ETH_DEV))
        self.bash(f'ip -6 neigh list dev {self.ETH_DEV}')

    def browse_mdns_services(self, name, timeout=2):
        """ Browse mDNS services on the ethernet.

        :param name: the service type name in format of '<service-name>.<protocol>'.
        :param timeout: timeout value in seconds before returning.
        :return: A list of service instance names.
        """

        self.bash(f'dns-sd -Z {name} local. > /tmp/{name} 2>&1 &')
        time.sleep(timeout)
        self.bash('pkill dns-sd')

        instances = []
        for line in self.bash(f'cat /tmp/{name}', encoding='raw_unicode_escape'):
            elements = line.split()
            if len(elements) >= 3 and elements[0] == name and elements[1] == 'PTR':
                instances.append(elements[2][:-len('.' + name)])
        return instances

    def discover_mdns_service(self, instance, name, host_name, timeout=2):
        """ Discover/resolve the mDNS service on ethernet.

        :param instance: the service instance name.
        :param name: the service name in format of '<service-name>.<protocol>'.
        :param host_name: the host name this service points to. The domain
                          should not be included.
        :param timeout: timeout value in seconds before returning.
        :return: a dict of service properties or None.

        The return value is a dict with the same key/values of srp_server_get_service
        except that we don't have a `deleted` field here.
        """

        self.bash(f'dns-sd -Z {name} local. > /tmp/{name} 2>&1 &')
        time.sleep(timeout)

        full_service_name = f'{instance}.{name}'
        # When hostname is unspecified, extract hostname from browse result
        if host_name is None:
            for line in self.bash(f'cat /tmp/{name}', encoding='raw_unicode_escape'):
                elements = line.split()
                if len(elements) >= 6 and elements[0] == full_service_name and elements[1] == 'SRV':
                    host_name = elements[5].split('.')[0]
                    break

        assert (host_name is not None)
        self.bash(f'dns-sd -G v6 {host_name}.local. > /tmp/{host_name} 2>&1 &')
        time.sleep(timeout)

        self.bash('pkill dns-sd')
        addresses = []
        service = {}

        logging.debug(self.bash(f'cat /tmp/{host_name}', encoding='raw_unicode_escape'))
        logging.debug(self.bash(f'cat /tmp/{name}', encoding='raw_unicode_escape'))

        # example output in the host file:
        # Timestamp     A/R Flags if Hostname                               Address                                     TTL
        # 9:38:09.274  Add     23 48 my-host.local.                         2001:0000:0000:0000:0000:0000:0000:0002%<0>  120
        #
        for line in self.bash(f'cat /tmp/{host_name}', encoding='raw_unicode_escape'):
            elements = line.split()
            fullname = f'{host_name}.local.'
            if fullname not in elements:
                continue
            addresses.append(elements[elements.index(fullname) + 1].split('%')[0])

        logging.debug(f'addresses of {host_name}: {addresses}')

        # example output of in the service file:
        # _ipps._tcp                                      PTR     my-service._ipps._tcp
        # my-service._ipps._tcp                           SRV     0 0 12345 my-host.local. ; Replace with unicast FQDN of target host
        # my-service._ipps._tcp                           TXT     ""
        #
        is_txt = False
        txt = ''
        for line in self.bash(f'cat /tmp/{name}', encoding='raw_unicode_escape'):
            elements = line.split()
            if len(elements) >= 2 and elements[0] == full_service_name and elements[1] == 'TXT':
                is_txt = True
            if is_txt:
                txt += line.strip()
                if line.strip().endswith('"'):
                    is_txt = False
                    txt_dict = self.__parse_dns_sd_txt(txt)
                    logging.info(f'txt = {txt_dict}')
                    service['txt'] = txt_dict

            if not elements or elements[0] != full_service_name:
                continue
            if elements[1] == 'SRV':
                service['fullname'] = elements[0]
                service['instance'] = instance
                service['name'] = name
                service['priority'] = int(elements[2])
                service['weight'] = int(elements[3])
                service['port'] = int(elements[4])
                service['host_fullname'] = elements[5]
                assert (service['host_fullname'] == f'{host_name}.local.')
                service['host'] = host_name
                service['addresses'] = addresses
        return service if 'addresses' in service and service['addresses'] else None

    def start_radvd_service(self, prefix, slaac):
        self.bash("""cat >/etc/radvd.conf <<EOF
interface eth0
{
    AdvSendAdvert on;

    AdvReachableTime 200;
    AdvRetransTimer 200;
    AdvDefaultLifetime 1800;
    MinRtrAdvInterval 1200;
    MaxRtrAdvInterval 1800;
    AdvDefaultPreference low;

    prefix %s
    {
        AdvOnLink on;
        AdvAutonomous %s;
        AdvRouterAddr off;
        AdvPreferredLifetime 40;
        AdvValidLifetime 60;
    };
};
EOF
""" % (prefix, 'on' if slaac else 'off'))
        self.bash('service radvd start')
        self.bash('service radvd status')  # Make sure radvd service is running

    def stop_radvd_service(self):
        self.bash('service radvd stop')

    def kill_radvd_service(self):
        self.bash('pkill radvd')

    def __parse_dns_sd_txt(self, line: str):
        # Example TXT entry:
        # "xp=\\000\\013\\184\\000\\000\\000\\000\\000"
        txt = {}
        for entry in re.findall(r'"((?:[^\\]|\\.)*?)"', line):
            if '=' not in entry:
                continue

            k, v = entry.split('=', 1)
            txt[k] = v

        return txt


class OtbrNode(LinuxHost, NodeImpl, OtbrDocker):
    is_otbr = True
    is_bbr = True  # OTBR is also BBR
    node_type = 'otbr-docker'

    def __repr__(self):
        return f'Otbr<{self.nodeid}>'

    def get_addrs(self) -> List[str]:
        return super().get_addrs() + self.get_ether_addrs()

    def start(self):
        self._setup_sysctl()
        self.set_log_level(5)
        super().start()


class HostNode(LinuxHost, OtbrDocker):
    is_host = True

    def __init__(self, nodeid, name=None, **kwargs):
        self.nodeid = nodeid
        self.name = name or ('Host%d' % nodeid)
        super().__init__(nodeid, **kwargs)

    def start(self, start_radvd=True, prefix=config.DOMAIN_PREFIX, slaac=False):
        self._setup_sysctl()
        if start_radvd:
            self.start_radvd_service(prefix, slaac)
        else:
            self.stop_radvd_service()

    def stop(self):
        self.stop_radvd_service()

    def get_addrs(self) -> List[str]:
        return self.get_ether_addrs()

    def __repr__(self):
        return f'Host<{self.nodeid}>'

    def get_matched_ula_addresses(self, prefix):
        """Get the IPv6 addresses that matches given prefix.
        """

        addrs = []
        for addr in self.get_ip6_address(config.ADDRESS_TYPE.ONLINK_ULA):
            if IPv6Address(addr) in IPv6Network(prefix):
                addrs.append(addr)

        return addrs

    def get_ip6_address(self, address_type: config.ADDRESS_TYPE):
        """Get specific type of IPv6 address configured on thread device.

        Args:
            address_type: the config.ADDRESS_TYPE type of IPv6 address.

        Returns:
            IPv6 address string.
        """

        if address_type == config.ADDRESS_TYPE.BACKBONE_GUA:
            return self._getBackboneGua()
        elif address_type == config.ADDRESS_TYPE.ONLINK_ULA:
            return self._getInfraUla()
        elif address_type == config.ADDRESS_TYPE.ONLINK_GUA:
            return self._getInfraGua()
        else:
            raise ValueError(f'unsupported address type: {address_type}')


if __name__ == '__main__':
    unittest.main()
