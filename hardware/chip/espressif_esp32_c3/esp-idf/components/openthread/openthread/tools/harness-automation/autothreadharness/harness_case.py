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
import json
import logging
import os
import subprocess
import re
import time
import unittest

from selenium import webdriver
from selenium.webdriver import ActionChains
from selenium.webdriver.support.ui import Select
from selenium.common.exceptions import UnexpectedAlertPresentException
from selenium.common.exceptions import NoSuchElementException
from functools import reduce

from autothreadharness import settings
from autothreadharness.exceptions import FailError, FatalError, GoldenDeviceNotEnoughError
from autothreadharness.harness_controller import HarnessController
from autothreadharness.helpers import HistoryHelper
from autothreadharness.open_thread_controller import OpenThreadController
from autothreadharness.pdu_controller_factory import PduControllerFactory
from autothreadharness.rf_shield_controller import get_rf_shield_controller

logger = logging.getLogger(__name__)

THREAD_CHANNEL_MAX = 26
"""Maximum channel number of thread protocol"""

THREAD_CHANNEL_MIN = 11
"""Minimum channel number of thread protocol"""

DEFAULT_TIMEOUT = 2700
"""Timeout for each test case in seconds"""


def wait_until(what, times=-1):
    """Wait until `what` return True

    Args:
        what (Callable[bool]): Call `wait()` again and again until it returns True
        times (int): Maximum times of trials before giving up

    Returns:
        True if success, False if times threshold reached

    """
    while times:
        logger.info('Waiting times left %d', times)
        try:
            if what() is True:
                return True
        except BaseException:
            logger.exception('Wait failed')
        else:
            logger.warning('Trial[%d] failed', times)
        times -= 1
        time.sleep(1)

    return False


class HarnessCase(unittest.TestCase):
    """This is the case class of all automation test cases.

    All test case classes MUST define properties `role`, `case` and `golden_devices_required`
    """

    channel = settings.THREAD_CHANNEL
    """int: Thread channel.

    Thread channel ranges from 11 to 26.
    """

    ROLE_LEADER = 1
    ROLE_ROUTER = 2
    ROLE_SED = 4
    ROLE_BORDER = 8
    ROLE_REED = 16
    ROLE_ED = 32
    ROLE_COMMISSIONER = 64
    ROLE_JOINER = 128
    ROLE_FED = 512
    ROLE_MED = 1024

    role = None
    """int: role id.

    1
        Leader
    2
        Router
    4
        Sleepy end device
    16
        Router eligible end device
    32
        End device
    64
        Commissioner
    128
        Joiner
    512
        Full end device
    1024
        Minimal end device
    """

    case = None
    """str: Case id, e.g. '6 5 1'.
    """

    golden_devices_required = 0
    """int: Golden devices needed to finish the test
    """

    child_timeout = settings.THREAD_CHILD_TIMEOUT
    """int: Child timeout in seconds
    """

    sed_polling_interval = settings.THREAD_SED_POLLING_INTERVAL
    """int: SED polling interval in seconds
    """

    auto_dut = settings.AUTO_DUT
    """bool: whether use harness auto dut feature"""

    timeout = hasattr(settings, 'TIMEOUT') and settings.TIMEOUT or DEFAULT_TIMEOUT
    """number: timeout in seconds to stop running this test case"""

    started = 0
    """number: test case started timestamp"""

    case_need_shield = False
    """bool: whether needs RF-box"""

    device_order = []
    """list: device drag order in TestHarness TestBed page"""

    def __init__(self, *args, **kwargs):
        self.dut = None
        self._browser = None
        self._hc = None
        self.result_dir = '%s\\%s' % (settings.OUTPUT_PATH, self.__class__.__name__)
        self.history = HistoryHelper()
        self.add_all_devices = False
        self.new_th = False

        harness_info = ConfigParser.ConfigParser()
        harness_info.read('%s\\info.ini' % settings.HARNESS_HOME)
        if harness_info.has_option('Thread_Harness_Info', 'Version') and harness_info.has_option(
                'Thread_Harness_Info', 'Mode'):
            harness_version = harness_info.get('Thread_Harness_Info', 'Version').rsplit(' ', 1)[1]
            harness_mode = harness_info.get('Thread_Harness_Info', 'Mode')

            if harness_mode == 'External' and harness_version > '1.4.0':
                self.new_th = True

            if harness_mode == 'Internal' and harness_version > '49.4':
                self.new_th = True

        super(HarnessCase, self).__init__(*args, **kwargs)

    def _init_devices(self):
        """Reboot all usb devices.

        Note:
            If PDU_CONTROLLER_TYPE is not valid, usb devices is not rebooted.
        """
        if not settings.PDU_CONTROLLER_TYPE:
            if settings.AUTO_DUT:
                return

            for device in settings.GOLDEN_DEVICES:
                port, _ = device
                try:
                    with OpenThreadController(port) as otc:
                        logger.info('Resetting %s', port)
                        otc.reset()
                except BaseException:
                    logger.exception('Failed to reset device %s', port)
                    self.history.mark_bad_golden_device(device)

            return

        tries = 3
        pdu_factory = PduControllerFactory()

        while True:
            try:
                pdu = pdu_factory.create_pdu_controller(settings.PDU_CONTROLLER_TYPE)
                pdu.open(**settings.PDU_CONTROLLER_OPEN_PARAMS)
            except EOFError:
                logger.warning('Failed to connect to telnet')
                tries = tries - 1
                if tries:
                    time.sleep(10)
                    continue
                else:
                    logger.error('Fatal error: cannot connect to apc')
                    raise
            else:
                pdu.reboot(**settings.PDU_CONTROLLER_REBOOT_PARAMS)
                pdu.close()
                break

        time.sleep(len(settings.GOLDEN_DEVICES))

    def _init_harness(self):
        """Restart harness backend service.

        Please start the harness controller before running the cases, otherwise, nothing happens
        """
        self._hc = HarnessController(self.result_dir)
        self._hc.stop()
        time.sleep(1)
        self._hc.start()
        time.sleep(2)

        harness_config = ConfigParser.ConfigParser()
        harness_config.read('%s\\Config\\Configuration.ini' % settings.HARNESS_HOME)
        if harness_config.has_option('THREAD_HARNESS_CONFIG', 'BrowserAutoNavigate') and harness_config.getboolean(
                'THREAD_HARNESS_CONFIG', 'BrowserAutoNavigate'):
            logger.error('BrowserAutoNavigate in Configuration.ini should be False')
            raise FailError('BrowserAutoNavigate in Configuration.ini should be False')
        if settings.MIXED_DEVICE_TYPE:
            if harness_config.has_option('THREAD_HARNESS_CONFIG',
                                         'EnableDeviceSelection') and not harness_config.getboolean(
                                             'THREAD_HARNESS_CONFIG', 'EnableDeviceSelection'):
                logger.error('EnableDeviceSelection in Configuration.ini should be True')
                raise FailError('EnableDeviceSelection in Configuration.ini should be True')

    def _destroy_harness(self):
        """Stop harness backend service

        Stop harness service.
        """
        self._hc.stop()
        time.sleep(2)

    def _init_dut(self):
        """Initialize the DUT.

        DUT will be restarted. and openthread will started.
        """
        if self.auto_dut:
            self.dut = None
            return

        dut_port = settings.DUT_DEVICE[0]
        dut = OpenThreadController(dut_port)
        self.dut = dut

    def _destroy_dut(self):
        self.dut = None

    def _init_browser(self):
        """Open harness web page.

        Open a quiet chrome which:
        1. disables extensions,
        2. ignore certificate errors and
        3. always allow notifications.
        """
        try:
            chrome_options = webdriver.ChromeOptions()
            chrome_options.add_argument('--disable-extensions')
            chrome_options.add_argument('--disable-infobars')
            chrome_options.add_argument('--ignore-certificate-errors')
            chrome_options.add_experimental_option('prefs',
                                                   {'profile.managed_default_content_settings.notifications': 1})

            browser = webdriver.Chrome(chrome_options=chrome_options)
            browser.set_page_load_timeout(20)
            browser.implicitly_wait(1)
            browser.get(settings.HARNESS_URL)
            browser.maximize_window()
            self._browser = browser
            if not wait_until(lambda: 'Thread' in browser.title, 30):
                self.assertIn('Thread', browser.title)
            return True
        except Exception as e:
            logger.info('Init chrome error: {0}'.format(type(e).__name__))
            return False

    def _destroy_browser(self):
        """Close the browser.
        """
        if self._browser:
            self._browser.close()
        self._browser = None

    def _init_rf_shield(self):
        if getattr(settings, 'SHIELD_CONTROLLER_TYPE', None) and getattr(settings, 'SHIELD_CONTROLLER_PARAMS', None):
            self.rf_shield = get_rf_shield_controller(shield_type=settings.SHIELD_CONTROLLER_TYPE,
                                                      params=settings.SHIELD_CONTROLLER_PARAMS)
        else:
            self.rf_shield = None

    def _destroy_rf_shield(self):
        self.rf_shield = None

    def setUp(self):
        """Prepare to run test case.

        Start harness service, init golden devices, reset DUT and open browser.
        """
        if self.__class__ is HarnessCase:
            return

        logger.info('Setting up')
        # clear files

        logger.info('Deleting all .pcapng')
        os.system('del /q "%s\\Captures\\*.pcapng"' % settings.HARNESS_HOME)
        logger.info('Empty files in Logs')
        os.system('del /q "%s\\Logs\\*.*"' % settings.HARNESS_HOME)

        # using temp files to fix excel downloading fail
        if self.new_th:
            logger.info('Empty files in Reports')
            os.system('del /q "%s\\Reports\\*.*"' % settings.HARNESS_HOME)
        else:
            logger.info('Empty files in temps')
            os.system('del /q "%s\\Thread_Harness\\temp\\*.*"' % settings.HARNESS_HOME)

        # create directory
        os.system('mkdir %s' % self.result_dir)
        self._init_harness()
        self._init_devices()
        self._init_dut()
        self._init_rf_shield()

    def tearDown(self):
        """Clean up after each case.

        Stop harness service, close browser and close DUT.
        """
        if self.__class__ is HarnessCase:
            return

        logger.info('Tearing down')
        self._destroy_harness()
        self._destroy_browser()
        self._destroy_dut()
        self._destroy_rf_shield()

    def _setup_page(self):
        """Do sniffer settings and general settings
        """
        if not self.started:
            self.started = time.time()

        # Detect Sniffer
        try:
            dialog = self._browser.find_element_by_id('capture-Setup-modal')
        except BaseException:
            logger.exception('Failed to get dialog.')
        else:
            if dialog and dialog.get_attribute('aria-hidden') == 'false':
                times = 100
                while times:
                    status = dialog.find_element_by_class_name('status-notify').text
                    if 'Searching' in status:
                        logger.info('Still detecting..')
                    elif 'Not' in status:
                        logger.warning('Sniffer device not verified!')
                        button = dialog.find_element_by_id('snifferAutoDetectBtn')
                        button.click()
                    elif 'Verified' in status:
                        logger.info('Verified!')
                        button = dialog.find_element_by_id('saveCaptureSettings')
                        button.click()
                        break
                    else:
                        logger.warning('Unexpected sniffer verification status')

                    times = times - 1
                    time.sleep(1)

                if not times:
                    raise Exception('Unable to detect sniffer device')

        time.sleep(1)

        try:
            skip_button = self._browser.find_element_by_id('SkipPrepareDevice')
            if skip_button.is_enabled():
                skip_button.click()
                time.sleep(1)
        except BaseException:
            logger.info('Still detecting sniffers')

        try:
            next_button = self._browser.find_element_by_id('nextButton')
        except BaseException:
            logger.exception('Failed to finish setup')
            return

        if not next_button.is_enabled():
            logger.info('Harness is still not ready')
            return

        # General Setup
        try:
            if self.child_timeout or self.sed_polling_interval:
                logger.info('finding general Setup button')
                button = self._browser.find_element_by_id('general-Setup')
                button.click()
                time.sleep(2)

                dialog = self._browser.find_element_by_id('general-Setup-modal')
                if dialog.get_attribute('aria-hidden') != 'false':
                    raise Exception('Missing General Setup dialog')

                field = dialog.find_element_by_id('inp_general_child_update_wait_time')
                field.clear()
                if self.child_timeout:
                    field.send_keys(str(self.child_timeout))

                field = dialog.find_element_by_id('inp_general_sed_polling_rate')
                field.clear()
                if self.sed_polling_interval:
                    field.send_keys(str(self.sed_polling_interval))

                button = dialog.find_element_by_id('saveGeneralSettings')
                button.click()
                time.sleep(1)

        except BaseException:
            logger.info('general setup exception')
            logger.exception('Failed to do general setup')
            return

        # Finish this page
        next_button.click()
        time.sleep(1)

    def _connect_devices(self):
        connect_all = self._browser.find_element_by_link_text('Connect All')
        connect_all.click()

    def _add_device(self, port, device_type_id):
        browser = self._browser
        test_bed = browser.find_element_by_id('test-bed')
        device = browser.find_element_by_id(device_type_id)
        # drag
        action_chains = ActionChains(browser)
        action_chains.click_and_hold(device)
        action_chains.move_to_element(test_bed).perform()
        time.sleep(1)

        # drop
        drop_hw = browser.find_element_by_class_name('drop-hw')
        action_chains = ActionChains(browser)
        action_chains.move_to_element(drop_hw)
        action_chains.release(drop_hw).perform()

        time.sleep(0.5)
        selected_hw = browser.find_element_by_class_name('selected-hw')
        form_inputs = selected_hw.find_elements_by_tag_name('input')
        form_port = form_inputs[0]
        form_port.clear()
        form_port.send_keys(port)

    def _test_bed(self):
        """Set up the test bed.

        Connect number of golden devices required by each case.
        """
        browser = self._browser
        test_bed = browser.find_element_by_id('test-bed')
        time.sleep(3)
        selected_hw_set = test_bed.find_elements_by_class_name('selected-hw')
        selected_hw_num = len(selected_hw_set)

        while selected_hw_num:
            remove_button = selected_hw_set[selected_hw_num - 1].find_element_by_class_name('removeSelectedDevice')
            remove_button.click()
            selected_hw_num = selected_hw_num - 1

        devices = [
            device for device in settings.GOLDEN_DEVICES if not self.history.is_bad_golden_device(device[0]) and
            not (settings.DUT_DEVICE and device[0] == settings.DUT_DEVICE[0])
        ]
        logger.info('Available golden devices: %s', json.dumps(devices, indent=2))

        shield_devices = [
            shield_device for shield_device in settings.SHIELD_GOLDEN_DEVICES
            if not self.history.is_bad_golden_device(shield_device[0]) and
            not (settings.DUT2_DEVICE and shield_device[0] == settings.DUT2_DEVICE[0])
        ]
        logger.info('Available shield golden devices: %s', json.dumps(shield_devices, indent=2))
        golden_devices_required = self.golden_devices_required

        dut_device = ()
        if settings.DUT_DEVICE:
            dut_device = settings.DUT_DEVICE
        """check if test case needs to use RF-shield box and its device order in Testbed page
        Two parameters case_need_shield & device_order should be set in the case script
        according to the requires: https://openthread.io/certification/test-cases#rf_shielding
        Example:
         In case script leader_9_2_9.py:
          case_need_shield = True
          device_order = [('Router_2', False), ('Commissioner', True), ('Router_1', False), ('DUT', True)]
         On the TestBed page of the Test Harness, the device sort order for Leader_9_2_9
           should be like:
             Router_2
             Commissioner
             Router_1
             DUT
           The ('Commissioner', True) and ('DUT', True) indicate Commissioner device and DUT2 device should
           be in the RF-box and choose from SHIELD_GOLDEN_DEVICES and DUT2_DEVICE. Otherwise ('DUT', False) means
           DUT device is not in RF-box and use DUT_DEVICE. The other roles devices with False should be selected
           from GOLDEN_DEVICES.

         In case script med_6_3_2.py:
         case_need_shield = True
         device_order = [] # or not defined
         means no device drag order. DUT2_DEVICE should be applied as DUT and the other golden devices
         are from GOLDEN_DEVICES.
        """
        if self.case_need_shield:
            if not settings.DUT2_DEVICE:
                logger.info('Must set DUT2_DEVICE')
                raise FailError('DUT2_DEVICE must be set in settings.py')
            if isinstance(self.device_order, list) and self.device_order:
                logger.info('case %s devices ordered by %s ', self.case, self.device_order)
            else:
                logger.info('case %s uses %s as DUT', self.case, settings.DUT2_DEVICE)

        # for test bed with multi-vendor devices
        if settings.MIXED_DEVICE_TYPE:
            topo_file = settings.HARNESS_HOME + "\\Thread_Harness\\TestScripts\\TopologyConfig.txt"
            try:
                f_topo = open(topo_file, 'r')
            except IOError:
                logger.info('%s can NOT be found', topo_file)
                raise GoldenDeviceNotEnoughError()
            topo_mixed_devices = []
            try:
                while True:
                    topo_line = f_topo.readline().strip()
                    if re.match(r'#.*', topo_line):
                        continue
                    match_line = re.match(r'(.*)-(.*)', topo_line, re.M | re.I)
                    if not match_line:
                        continue
                    case_id = match_line.group(1)

                    if re.sub(r'\.', ' ', case_id) == self.case:
                        logger.info('Get line by case %s: %s', case_id, topo_line)
                        topo_device_list = re.split(',', match_line.group(2))
                        for i in range(len(topo_device_list)):
                            topo_device = re.split(':', topo_device_list[i])
                            topo_mixed_devices.append(tuple(topo_device))
                        break
                    else:
                        continue
            except Exception as e:
                logger.info('Get devices from topology config file error: %s', e)
                raise GoldenDeviceNotEnoughError()
            logger.info('Golden devices in topology config file for case %s: %s', case_id, topo_mixed_devices)
            f_topo.close()
            golden_device_candidates = []
            missing_golden_devices = topo_mixed_devices[:]

            # mapping topology config devices with golden devices by device order
            if self.case_need_shield and self.device_order:
                matched_dut = False
                for device_order_item in self.device_order:
                    matched = False
                    for mixed_device_item in topo_mixed_devices:
                        # mapping device in device_order which needs to be shielded
                        if device_order_item[1]:
                            if 'DUT' in device_order_item[0]:
                                golden_device_candidates.append(settings.DUT2_DEVICE)
                                dut_device = settings.DUT2_DEVICE
                                matched_dut = True
                                matched = True
                                break
                            for device_item in shield_devices:
                                if (device_order_item[0] == mixed_device_item[0] and
                                        mixed_device_item[1] == device_item[1]):
                                    golden_device_candidates.append(device_item)
                                    shield_devices.remove(device_item)
                                    matched = True
                                    break
                        # mapping device in device_order which does not need to be shielded
                        else:
                            if 'DUT' in device_order_item[0]:
                                golden_device_candidates.append(settings.DUT_DEVICE)
                                matched_dut = True
                                matched = True
                                break
                            for device_item in devices:
                                if (device_order_item[0] == mixed_device_item[0] and
                                        mixed_device_item[1] == device_item[1]):
                                    golden_device_candidates.append(device_item)
                                    devices.remove(device_item)
                                    matched = True
                                    break
                    if not matched:
                        logger.info('Golden device not enough in : no %s', device_order_item)
                        raise GoldenDeviceNotEnoughError()
                if not matched_dut:
                    raise FailError('Failed to find DUT in device_order')
                devices = golden_device_candidates
                self.add_all_devices = True
            else:
                for mixed_device_item in topo_mixed_devices:
                    for device_item in devices:
                        if mixed_device_item[1] == device_item[1]:
                            golden_device_candidates.append(device_item)
                            devices.remove(device_item)
                            missing_golden_devices.remove(mixed_device_item)
                            break
                logger.info('Golden devices in topology config file mapped in settings : %s', golden_device_candidates)
                if len(topo_mixed_devices) != len(golden_device_candidates):
                    device_dict = dict()
                    for missing_device in missing_golden_devices:
                        if missing_device[1] in device_dict:
                            device_dict[missing_device[1]] += 1
                        else:
                            device_dict[missing_device[1]] = 1
                    logger.info('Missing Devices: %s', device_dict)
                    raise GoldenDeviceNotEnoughError()
                else:
                    devices = golden_device_candidates
                    golden_devices_required = len(devices)
                    logger.info('All case-needed golden devices: %s', json.dumps(devices, indent=2))
        # for test bed with single vendor devices
        else:
            golden_device_candidates = []
            if self.case_need_shield and self.device_order:
                matched_dut = False
                for device_order_item in self.device_order:
                    matched = False
                    # choose device which needs to be shielded
                    if device_order_item[1]:
                        if 'DUT' in device_order_item[0]:
                            golden_device_candidates.append(settings.DUT2_DEVICE)
                            dut_device = settings.DUT2_DEVICE
                            matched_dut = True
                            matched = True
                        else:
                            for device_item in shield_devices:
                                golden_device_candidates.append(device_item)
                                shield_devices.remove(device_item)
                                matched = True
                                break
                    # choose device which does not need to be shielded
                    else:
                        if 'DUT' in device_order_item[0]:
                            golden_device_candidates.append(settings.DUT_DEVICE)
                            matched_dut = True
                            matched = True
                        else:
                            for device_item in devices:
                                golden_device_candidates.append(device_item)
                                devices.remove(device_item)
                                matched = True
                                break
                    if not matched:
                        logger.info('Golden device not enough in : no %s', device_order_item)
                        raise GoldenDeviceNotEnoughError()
                if not matched_dut:
                    raise FailError('Failed to find DUT in device_order')
                devices = golden_device_candidates
                self.add_all_devices = True

        if self.auto_dut and not settings.DUT_DEVICE:
            if settings.MIXED_DEVICE_TYPE:
                logger.info('Must set DUT_DEVICE')
                raise FailError('DUT_DEVICE must be set for mixed testbed')
            golden_devices_required += 1

        if len(devices) < golden_devices_required:
            raise GoldenDeviceNotEnoughError()

        # add golden devices
        number_of_devices_to_add = len(devices) if self.add_all_devices else golden_devices_required
        for i in range(number_of_devices_to_add):
            self._add_device(*devices.pop())

        # add DUT
        if self.case_need_shield:
            if not self.device_order:
                self._add_device(*settings.DUT2_DEVICE)
        else:
            if settings.DUT_DEVICE:
                self._add_device(*settings.DUT_DEVICE)

        # enable AUTO DUT
        if self.auto_dut:
            checkbox_auto_dut = browser.find_element_by_id('EnableAutoDutSelection')
            if not checkbox_auto_dut.is_selected():
                checkbox_auto_dut.click()
                time.sleep(1)

            if settings.DUT_DEVICE:
                radio_auto_dut = browser.find_element_by_class_name('AutoDUT_RadBtns')
                if not radio_auto_dut.is_selected() and not self.device_order:
                    radio_auto_dut.click()

                if self.device_order:
                    selected_hw_set = test_bed.find_elements_by_class_name('selected-hw')
                    for selected_hw in selected_hw_set:
                        form_inputs = selected_hw.find_elements_by_tag_name('input')
                        form_port = form_inputs[0]
                        port = form_port.get_attribute('value').encode('utf8')
                        if port == dut_device[0]:
                            radio_auto_dut = selected_hw.find_element_by_class_name('AutoDUT_RadBtns')
                            if not radio_auto_dut.is_selected():
                                radio_auto_dut.click()

        while True:
            try:
                self._connect_devices()
                button_next = browser.find_element_by_id('nextBtn')
                if not wait_until(
                        lambda: 'disabled' not in button_next.get_attribute('class'),
                        times=(30 + 4 * number_of_devices_to_add),
                ):
                    bad_ones = []
                    selected_hw_set = test_bed.find_elements_by_class_name('selected-hw')
                    for selected_hw in selected_hw_set:
                        form_inputs = selected_hw.find_elements_by_tag_name('input')
                        form_port = form_inputs[0]
                        if form_port.is_enabled():
                            bad_ones.append(selected_hw)

                    for selected_hw in bad_ones:
                        form_inputs = selected_hw.find_elements_by_tag_name('input')
                        form_port = form_inputs[0]
                        port = form_port.get_attribute('value').encode('utf8')
                        if port == dut_device[0]:
                            if settings.PDU_CONTROLLER_TYPE is None:
                                # connection error cannot recover without power
                                # cycling
                                raise FatalError('Failed to connect to DUT')
                            else:
                                raise FailError('Failed to connect to DUT')

                        if settings.PDU_CONTROLLER_TYPE is None:
                            # port cannot recover without power cycling
                            self.history.mark_bad_golden_device(port)

                        # remove the bad one
                        selected_hw.find_element_by_class_name('removeSelectedDevice').click()
                        time.sleep(0.1)

                        if len(devices):
                            self._add_device(*devices.pop())
                        else:
                            devices = None

                    if devices is None:
                        logger.warning('Golden devices not enough')
                        raise GoldenDeviceNotEnoughError()
                    else:
                        logger.info('Try again with new golden devices')
                        continue

                if self.auto_dut and not settings.DUT_DEVICE:
                    radio_auto_dut = browser.find_element_by_class_name('AutoDUT_RadBtns')
                    if not radio_auto_dut.is_selected():
                        radio_auto_dut.click()

                    time.sleep(5)

                button_next.click()
                if not wait_until(lambda: self._browser.current_url.endswith('TestExecution.html'), 20):
                    raise Exception('Failed to load TestExecution page')
            except FailError:
                raise
            except BaseException:
                logger.exception('Unexpected error')
            else:
                break

    def _select_case(self, role, case):
        """Select the test case.
        """
        # select the case
        elem = Select(self._browser.find_element_by_id('select-dut'))
        elem.select_by_value(str(role))
        time.sleep(1)

        checkbox = None
        wait_until(lambda: self._browser.find_elements_by_css_selector('.tree-node .tree-title') and True)
        elems = self._browser.find_elements_by_css_selector('.tree-node .tree-title')
        finder = re.compile(r'.*\b' + case + r'\b')
        finder_dotted = re.compile(r'.*\b' + case.replace(' ', r'\.') + r'\b')
        for elem in elems:
            # elem.txt might be null when the required reference devices could
            # not be met (either due to the quantity or the type) for specific test.
            # perform() will throw exceptions if elem.text is null since Chrome
            # and chromedriver 80. If elem is not shown in current case list
            # window, move_to_element() will not work either.
            if not elem.text:
                continue
            # execute a javascript to scroll the window to the elem
            self._browser.execute_script('arguments[0].scrollIntoView();', elem)
            action_chains = ActionChains(self._browser)
            action_chains.move_to_element(elem)
            action_chains.perform()
            logger.debug(elem.text)
            if finder.match(elem.text) or finder_dotted.match(elem.text):
                parent = elem.find_element_by_xpath('..')
                checkbox = parent.find_element_by_class_name('tree-checkbox')
                break

        if not checkbox:
            time.sleep(5)
            raise Exception('Failed to find the case')

        self._browser.execute_script("$('.overview').css('left', '0')")
        checkbox.click()
        time.sleep(1)

        elem = self._browser.find_element_by_id('runTest')
        elem.click()
        if not wait_until(lambda: self._browser.find_element_by_id('stopTest') and True, 10):
            raise Exception('Failed to start test case')

    def _collect_result(self):
        """Collect test result.

        Copy PDF and pcap file to result directory
        """

        if self.new_th:
            os.system('copy "%s\\Reports\\*.*" "%s"' % (settings.HARNESS_HOME, self.result_dir))
        else:
            os.system('copy "%s\\Thread_Harness\\temp\\*.*" "%s"' % (settings.HARNESS_HOME, self.result_dir))

        os.system('copy "%s\\Captures\\*.pcapng" %s\\' % (settings.HARNESS_HOME, self.result_dir))

    def _wait_dialog(self):
        """Wait for dialogs and handle them until done.
        """
        logger.debug('waiting for dialog')
        done = False
        error = False

        logger.info('self timeout %d', self.timeout)
        while not done and self.timeout:
            try:
                dialog = self._browser.find_element_by_id('RemoteConfirm')
            except BaseException:
                logger.exception('Failed to get dialog.')
            else:
                if dialog and dialog.get_attribute('aria-hidden') == 'false':
                    title = dialog.find_element_by_class_name('modal-title').text
                    time.sleep(1)
                    logger.info('Handling dialog[%s]', title)

                    try:
                        done = self._handle_dialog(dialog, title)
                    except BaseException:
                        logger.exception('Error handling dialog: %s', title)
                        error = True

                    if done is None:
                        raise FailError('Unexpected dialog occurred')

                    dialog.find_element_by_id('ConfirmOk').click()

            time.sleep(1)

            try:
                stop_button = self._browser.find_element_by_id('stopTest')
                if done:
                    stop_button.click()
                    # wait for stop procedure end
                    time.sleep(10)
            except NoSuchElementException:
                logger.info('Test stopped')
                time.sleep(5)
                done = True

            self.timeout -= 1

            # check if already ended capture
            if self.timeout % 10 == 0:
                lines = self._hc.tail()
                if 'SUCCESS: The process "dumpcap.exe" with PID ' in lines:
                    logger.info('Tshark should be ended now, lets wait at most 30 seconds.')
                    if not wait_until(lambda: 'tshark.exe' not in subprocess.check_output('tasklist'), 30):
                        res = subprocess.check_output('taskkill /t /f /im tshark.exe',
                                                      stderr=subprocess.STDOUT,
                                                      shell=True)
                        logger.info(res)

        # Wait until case really stopped
        wait_until(lambda: self._browser.find_element_by_id('runTest') and True, 30)

        if error:
            raise FailError('Fail for previous exceptions')

    def _handle_dialog(self, dialog, title):
        """Handle a dialog.

        Returns:
            bool True if no more dialogs expected,
                 False if more dialogs needed, and
                 None if not handled
        """
        done = self.on_dialog(dialog, title)
        if isinstance(done, bool):
            return done

        if title.startswith('Start DUT'):
            body = dialog.find_element_by_id('cnfrmMsg').text
            if 'Sleepy End Device' in body:
                self.dut.mode = 's'
                self.dut.child_timeout = self.child_timeout
            elif 'End Device' in body:
                self.dut.mode = 'rn'
                self.dut.child_timeout = self.child_timeout
            else:
                self.dut.mode = 'rdn'

            if 'at channel' in body:
                self.channel = int(body.split(':')[1])

            self.dut.channel = self.channel
            self.dut.panid = settings.THREAD_PANID
            self.dut.networkname = settings.THREAD_NETWORKNAME
            self.dut.extpanid = settings.THREAD_EXTPANID
            self.dut.start()

        elif title.startswith('MAC Address Required') or title.startswith('DUT Random Extended MAC Address Required'):
            mac = self.dut.mac
            inp = dialog.find_element_by_id('cnfrmInpText')
            inp.clear()
            inp.send_keys('0x%s' % mac)

        elif title.startswith('LL64 Address'):
            ll64 = None
            for addr in self.dut.addrs:
                addr = addr.lower()
                if addr.startswith('fe80') and not re.match('.+ff:fe00:[0-9a-f]{0,4}$', addr):
                    ll64 = addr
                    break

            if not ll64:
                raise FailError('No link local address found')

            logger.info('Link local address is %s', ll64)
            inp = dialog.find_element_by_id('cnfrmInpText')
            inp.clear()
            inp.send_keys(ll64)

        elif title.startswith('Enter Channel'):
            self.dut.channel = self.channel
            inp = dialog.find_element_by_id('cnfrmInpText')
            inp.clear()
            inp.send_keys(str(self.dut.channel))

        elif title.startswith('User Action Needed'):
            body = dialog.find_element_by_id('cnfrmMsg').text
            if body.startswith('Power Down the DUT'):
                self.dut.stop()
            return True

        elif title.startswith('Short Address'):
            short_addr = '0x%s' % self.dut.short_addr
            inp = dialog.find_element_by_id('cnfrmInpText')
            inp.clear()
            inp.send_keys(short_addr)

        elif title.startswith('ML64 Address'):
            ml64 = None
            for addr in self.dut.addrs:
                if addr.startswith('fd') and not re.match('.+ff:fe00:[0-9a-f]{0,4}$', addr):
                    ml64 = addr
                    break

            if not ml64:
                raise Exception('No mesh local address found')

            logger.info('Mesh local address is %s', ml64)
            inp = dialog.find_element_by_id('cnfrmInpText')
            inp.clear()
            inp.send_keys(ml64)

        elif title.startswith('Shield Devices') or title.startswith('Shield DUT'):
            time.sleep(2)
            if self.rf_shield:
                logger.info('Shielding devices')
                with self.rf_shield:
                    self.rf_shield.shield()
            elif self.dut and settings.SHIELD_SIMULATION:
                self.dut.channel = (self.channel == THREAD_CHANNEL_MAX and THREAD_CHANNEL_MIN) or (self.channel + 1)
            else:
                input('Shield DUT and press enter to continue..')

        elif title.startswith('Unshield Devices') or title.startswith('Bring DUT back to network'):
            time.sleep(5)
            if self.rf_shield:
                logger.info('Unshielding devices')
                with self.rf_shield:
                    self.rf_shield.unshield()
            elif self.dut and settings.SHIELD_SIMULATION:
                self.dut.channel = self.channel
            else:
                input('Bring DUT and press enter to continue..')

        elif title.startswith('Configure Prefix on DUT'):
            body = dialog.find_element_by_id('cnfrmMsg').text
            body = body.split(': ')[1]
            params = reduce(
                lambda params, param: params.update(((param[0].strip(' '), param[1]),)) or params,
                [it.split('=') for it in body.split(', ')],
                {},
            )
            prefix = params['P_Prefix'].strip('\0\r\n\t ')
            flags = []
            if params.get('P_slaac_preferred', 0) == '1':
                flags.append('p')
            flags.append('ao')
            if params.get('P_stable', 0) == '1':
                flags.append('s')
            if params.get('P_default', 0) == '1':
                flags.append('r')
            prf = 'high'
            self.dut.add_prefix(prefix, ''.join(flags), prf)

        return False

    def test(self):
        """This method will only start test case in child class"""
        if self.__class__ is HarnessCase:
            logger.warning('Skip this harness itself')
            return

        logger.info('Testing role[%d] case[%s]', self.role, self.case)

        init_browser_times = 5
        while True:
            if self._init_browser():
                break
            elif init_browser_times > 0:
                init_browser_times -= 1
                self._destroy_browser()
            else:
                raise SystemExit()

        try:
            # prepare test case
            while True:
                url = self._browser.current_url
                if url.endswith('SetupPage.html'):
                    self._setup_page()
                elif url.endswith('TestBed.html'):
                    self._test_bed()
                elif url.endswith('TestExecution.html'):
                    logger.info('Ready to handle dialogs')
                    break
                time.sleep(2)
        except UnexpectedAlertPresentException:
            logger.exception('Failed to connect to harness server')
            raise SystemExit()
        except FatalError:
            logger.exception('Test stopped for fatal error')
            raise SystemExit()
        except FailError:
            logger.exception('Test failed')
            raise
        except BaseException:
            logger.exception('Something wrong')

        self._select_case(self.role, self.case)

        logger.info('start to wait test process end')
        self._wait_dialog()

        try:
            self._collect_result()
        except BaseException:
            logger.exception('Failed to collect results')
            raise

        # get case result
        status = self._browser.find_element_by_class_name('title-test').get_attribute('innerText')
        logger.info(status)
        success = 'Pass' in status
        self.assertTrue(success)
