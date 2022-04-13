# -*- coding:utf-8 -*-
#
# Copyright (C) 2009 The Android Open Source Project
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

import os
import sys
from time import time
import threading

# This will erase all content in the current line (wherever the cursor is).
# It does not move the cursor, so this is usually followed by \r to move to
# column 0.
CSI_ERASE_LINE = '\x1b[2K'


class Progress(object):
    def __init__(self, title, total=0, units='', print_newline=False,
                 always_print_percentage=False):
        self._title = title
        self._total = total
        self._done = 0
        self._lastp = -1
        self._start = time()
        self._show = False
        self._units = units
        self._print_newline = print_newline
        self._always_print_percentage = always_print_percentage
        self.mutex = threading.Lock()

    def update(self, inc=1, msg=''):
        self.mutex.acquire()
        self._update(inc, msg)
        self.mutex.release()

    def end(self):
        self.mutex.acquire()
        self._end()
        self.mutex.release()

    def _update(self, inc=1, msg=''):
        self._done += inc

        if not self._show:
            if 0.5 <= time() - self._start:
                self._show = True
            else:
                return

        if self._total <= 0:
            sys.stderr.write('%s\r%s: %d,' % (
                CSI_ERASE_LINE,
                self._title,
                self._done))
            sys.stderr.flush()
        else:
            p = (100 * self._done) / self._total

            if self._lastp != p or self._always_print_percentage:
                self._lastp = p
                sys.stderr.write('%s\r%s: %3d%% (%d%s/%d%s)%s%s%s' % (
                    CSI_ERASE_LINE,
                    self._title,
                    p,
                    self._done, self._units,
                    self._total, self._units,
                    ' ' if msg else '', msg,
                    "\n" if self._print_newline else ""))
                sys.stderr.flush()

    def _end(self):
        if not self._show:
            return

        if self._total <= 0:
            sys.stderr.write('%s\r%s: %d, done.\n' % (
                CSI_ERASE_LINE,
                self._title,
                self._done))
            sys.stderr.flush()
        else:
            p = (100 * self._done) / self._total
            sys.stderr.write('%s\r%s: %3d%% (%d%s/%d%s), done.\n' % (
                CSI_ERASE_LINE,
                self._title,
                p,
                self._done, self._units,
                self._total, self._units))
            sys.stderr.flush()
