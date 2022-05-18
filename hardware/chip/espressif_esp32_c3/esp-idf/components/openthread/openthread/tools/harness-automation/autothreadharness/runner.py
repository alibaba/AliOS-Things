#!/usr/bin/env python
#
# Copyright (c) 2016, The OpenThread Authors.
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
# 1. Redistributions of source code must retain the above copyright
#    notice, this list of conditions and the following disclaimer.
# 2. Redistributions in binary form must reproduce the above copyright
#    notice, this list of conditions and the following disclaimer in the
#    documentation and/or other materials provided with the distribution.
# 3. Neither the name of the copyright holder nor the
#    names of its contributors may be used to endorse or promote products
#    derived from this software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
# AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
# ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
# LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
# CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
# SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
# INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
# CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
# ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
# POSSIBILITY OF SUCH DAMAGE.
#

import ConfigParser
import argparse
import fnmatch
import logging
import json
import os
import sys
import time
import unittest
from collections import OrderedDict

from autothreadharness.harness_case import HarnessCase
from autothreadharness.open_thread_controller import OpenThreadController
from autothreadharness import settings

logging.basicConfig(level=logging.INFO)

logger = logging.getLogger()
"""Logger: The global logger"""

logger.setLevel(logging.INFO)

RESUME_SCRIPT_PATH = "%appdata%\\Microsoft\\Windows\\Start Menu\\Programs\\" "Startup\\continue_harness.bat"


class SimpleTestResult(unittest.TestResult):

    executions = 0

    def __init__(self, path, auto_reboot_args=None, keep_explorer=False, add_all_devices=False):
        """Record test results in json file

        Args:
            path (str): File path to record the results
            auto_reboot (bool): Whether reboot when harness die
        """
        super(SimpleTestResult, self).__init__()
        self.path = path
        self.auto_reboot_args = auto_reboot_args
        self.result = json.load(open(self.path, 'r'))
        self.log_handler = None
        self.started = None
        self.keep_explorer = keep_explorer
        self.add_all_devices = add_all_devices
        SimpleTestResult.executions += 1
        logger.info('Initial state is %s', json.dumps(self.result, indent=2))

    def startTest(self, test):
        logger.info(
            '\n========================================\n%s\n========================================',
            test.__class__.__name__,
        )

        test.add_all_devices = self.add_all_devices
        # create start up script if auto reboot enabled
        if self.auto_reboot_args:
            test.auto_reboot = True
            os.system('echo %s > "%s"' %
                      (' '.join(self.auto_reboot_args + ['-c', test.__class__.__name__]), RESUME_SCRIPT_PATH))

        # record start timestamp
        self.started = time.strftime('%Y-%m-%dT%H:%M:%S')

        os.system('mkdir %s' % test.result_dir)
        self.log_handler = logging.FileHandler('%s\\auto-%s.log' % (test.result_dir, time.strftime('%Y%m%d%H%M%S')))
        self.log_handler.setLevel(logging.DEBUG)
        self.log_handler.setFormatter(logging.Formatter('%(asctime)s %(levelname)s %(message)s'))
        logger.addHandler(self.log_handler)

    def add_result(self, test, passed, error=None):
        """Record test result into json file

        Args:
            test (TestCase): The test just run
            passed (bool): Whether the case is passed
        """
        fails = self.result.get(test.__class__.__name__, {}).get('fails', 0)
        if passed is False:
            fails += 1
        self.result[str(test.__class__.__name__)] = {
            'started': self.started,
            'stopped': time.strftime('%Y-%m-%dT%H:%M:%S'),
            'passed': passed,
            'fails': fails,
            'error': error,
            'executions': SimpleTestResult.executions,
        }
        if self.auto_reboot_args:
            os.system('del "%s"' % RESUME_SCRIPT_PATH)

        json.dump(OrderedDict(sorted(self.result.items(), key=lambda t: t[0])), open(self.path, 'w'), indent=2)

        # save logs
        logger.removeHandler(self.log_handler)
        self.log_handler.close()
        self.log_handler = None
        time.sleep(2)

        # close explorers
        if not self.keep_explorer:
            os.system('taskkill /f /im explorer.exe && start explorer.exe')

    def addSuccess(self, test):
        logger.info('case[%s] pass', test.__class__.__name__)
        super(SimpleTestResult, self).addSuccess(test)
        self.add_result(test, True)

    def addFailure(self, test, err):
        logger.warning('case[%s] fail', test.__class__.__name__)
        super(SimpleTestResult, self).addFailure(test, err)
        self.add_result(test, False)

    def addError(self, test, err):
        logger.error('case[%s] error', test.__class__.__name__, exc_info=err)

        if err and err[0] is SystemExit:
            if self.auto_reboot_args:
                logger.warning('rebooting..')
                os.system('shutdown /r /t 1')
            else:
                logger.warning('exiting..')
            sys.exit(1)

        super(SimpleTestResult, self).addError(test, err)
        self.add_result(test, None, str(err[1]))


def list_devices(names=None, continue_from=None, **kwargs):
    """List devices in settings file and print versions"""

    if not names:
        names = [device for device, _type in settings.GOLDEN_DEVICES if _type == 'OpenThread']

    if continue_from:
        continue_from = names.index(continue_from)
    else:
        continue_from = 0

    for port in names[continue_from:]:
        try:
            with OpenThreadController(port) as otc:
                print('%s: %s' % (port, otc.version))
        except BaseException:
            logger.exception('failed to get version of %s' % port)


def discover(
    names=None,
    pattern=['*.py'],
    skip='efp',
    dry_run=False,
    denylist=None,
    name_greps=None,
    manual_reset=False,
    delete_history=False,
    max_devices=0,
    continue_from=None,
    result_file='./result.json',
    auto_reboot=False,
    keep_explorer=False,
    add_all_devices=False,
):
    """Discover all test cases and skip those passed

    Args:
        pattern (str): Pattern to match case modules, refer python's unittest
                       documentation for more details
        skip (str): types cases to skip
    """
    if not os.path.exists(settings.OUTPUT_PATH):
        os.mkdir(settings.OUTPUT_PATH)

    if delete_history:
        os.system('del history.json')

    if denylist:
        try:
            excludes = [line.strip('\n') for line in open(denylist, 'r').readlines() if not line.startswith('#')]
        except BaseException:
            logger.exception('Failed to open test case denylist file')
            raise
    else:
        excludes = []

    log = None
    if os.path.isfile(result_file):
        try:
            log = json.load(open(result_file, 'r'))
        except BaseException:
            logger.exception('Failed to open result file')

    if not log:
        log = {}
        json.dump(log, open(result_file, 'w'), indent=2)

    new_th = False
    harness_info = ConfigParser.ConfigParser()
    harness_info.read('%s\\info.ini' % settings.HARNESS_HOME)
    if harness_info.has_option('Thread_Harness_Info', 'Version') and harness_info.has_option(
            'Thread_Harness_Info', 'Mode'):
        harness_version = harness_info.get('Thread_Harness_Info', 'Version').rsplit(' ', 1)[1]
        harness_mode = harness_info.get('Thread_Harness_Info', 'Mode')

        if harness_mode == 'External' and harness_version > '1.4.0':
            new_th = True

        if harness_mode == 'Internal' and harness_version > '49.4':
            new_th = True

    suite = unittest.TestSuite()
    if new_th:
        discovered = unittest.defaultTestLoader.discover('cases', pattern)
    else:
        discovered = unittest.defaultTestLoader.discover('cases_R140', pattern)

    if names and continue_from:
        names = names[names.index(continue_from):]

    for s1 in discovered:
        for s2 in s1:
            for case in s2:
                if case.__class__ is HarnessCase:
                    continue
                case_name = str(case.__class__.__name__)

                # grep name
                if name_greps and not any(fnmatch.fnmatch(case_name, name_grep) for name_grep in name_greps):
                    logger.info('case[%s] skipped by name greps', case_name)
                    continue

                # allowlist
                if len(names) and case_name not in names:
                    logger.info('case[%s] skipped', case_name)
                    continue

                # skip cases
                if case_name in log:
                    if ((log[case_name]['passed'] and ('p' in skip)) or
                        (log[case_name]['passed'] is False and ('f' in skip)) or (log[case_name]['passed'] is None and
                                                                                  ('e' in skip))):
                        logger.warning('case[%s] skipped for its status[%s]', case_name, log[case_name]['passed'])
                        continue

                # continue from
                if continue_from:
                    if continue_from != case_name:
                        logger.warning('case[%s] skipped for continue from[%s]', case_name, continue_from)
                        continue
                    else:
                        continue_from = None

                # denylist
                if case_name in excludes:
                    logger.warning('case[%s] skipped for denylist', case_name)
                    continue

                # max devices
                if max_devices and case.golden_devices_required > max_devices:
                    logger.warning('case[%s] skipped for exceeding max golden devices allowed[%d]', case_name,
                                   max_devices)
                    continue

                suite.addTest(case)
                logger.info('case[%s] added', case_name)

    if auto_reboot:
        argv = []
        argv.append('"%s"' % os.sep.join([os.getcwd(), 'start.bat']))
        argv.extend(['-p', pattern])
        argv.extend(['-k', skip])
        argv.extend(['-o', result_file])
        argv.append('-a')

        if manual_reset:
            argv.append('-m')

        if delete_history:
            argv.append('-d')

        auto_reboot_args = argv + names
    else:
        auto_reboot_args = None
        if os.path.isfile(RESUME_SCRIPT_PATH):
            os.system('del "%s"' % RESUME_SCRIPT_PATH)

    # manual reset
    if manual_reset:
        settings.PDU_CONTROLLER_TYPE = 'MANUAL_PDU_CONTROLLER'
        settings.PDU_CONTROLLER_OPEN_PARAMS = {}
        settings.PDU_CONTROLLER_REBOOT_PARAMS = {}

    result = SimpleTestResult(result_file, auto_reboot_args, keep_explorer, add_all_devices)
    for case in suite:
        logger.info(case.__class__.__name__)

    if dry_run:
        return

    suite.run(result)
    return result


def main():
    parser = argparse.ArgumentParser(description='Thread harness test case runner')
    parser.add_argument('--auto-reboot',
                        '-a',
                        action='store_true',
                        default=False,
                        help='restart system when harness service die')
    parser.add_argument('names',
                        metavar='NAME',
                        type=str,
                        nargs='*',
                        default=None,
                        help='test case name, omit to test all')
    parser.add_argument('--denylist',
                        '-b',
                        metavar='DENYLIST_FILE',
                        type=str,
                        help='file to list test cases to skip',
                        default=None)
    parser.add_argument('--continue-from', '-c', type=str, default=None, help='first case to test')
    parser.add_argument('--delete-history', '-d', action='store_true', default=False, help='clear history on startup')
    parser.add_argument('--keep-explorer',
                        '-e',
                        action='store_true',
                        default=False,
                        help='do not restart explorer.exe at the end')
    parser.add_argument('--name-greps', '-g', action='append', default=None, help='grep case by names')
    parser.add_argument('--list-file', '-i', type=str, default=None, help='file to list cases names to test')
    parser.add_argument(
        '--skip',
        '-k',
        metavar='SKIP',
        type=str,
        help='type of results to skip. e for error, f for fail, p for pass.',
        default='',
    )
    parser.add_argument('--list-devices', '-l', action='store_true', default=False, help='list devices')
    parser.add_argument('--manual-reset', '-m', action='store_true', default=False, help='reset devices manually')
    parser.add_argument('--dry-run', '-n', action='store_true', default=False, help='just show what to run')
    parser.add_argument(
        '--result-file',
        '-o',
        type=str,
        default=settings.OUTPUT_PATH + '\\result.json',
        help='file to store and read current status',
    )
    parser.add_argument('--pattern',
                        '-p',
                        metavar='PATTERN',
                        type=str,
                        help='file name pattern, default to "*.py"',
                        default='*.py')
    parser.add_argument('--rerun-fails', '-r', type=int, default=0, help='number of times to rerun failed test cases')
    parser.add_argument('--add-all-devices',
                        '-t',
                        action='store_true',
                        default=False,
                        help='add all devices to the test bed')
    parser.add_argument('--max-devices', '-u', type=int, default=0, help='max golden devices allowed')

    args = vars(parser.parse_args())

    if args['list_file']:
        try:
            names = [line.strip('\n') for line in open(args['list_file'], 'r').readlines() if not line.startswith('#')]
        except BaseException:
            logger.exception('Failed to open test case list file')
            raise
        else:
            args['names'] = args['names'] + names

    args.pop('list_file')

    if args.pop('list_devices', False):
        list_devices(**args)
        return

    rerun_fails = args.pop('rerun_fails')
    result = discover(**args)

    if rerun_fails > 0:
        for i in range(rerun_fails):
            failed_names = {name for name in result.result if result.result[name]['passed'] is False}
            if not failed_names:
                break
            logger.info('Rerunning failed test cases')
            logger.info('Rerun #{}:'.format(i + 1))
            result = discover(
                names=failed_names,
                pattern=args['pattern'],
                skip='',
                result_file=args['result_file'],
                auto_reboot=args['auto_reboot'],
                keep_explorer=args['keep_explorer'],
                add_all_devices=args['add_all_devices'],
            )


if __name__ == '__main__':
    main()
