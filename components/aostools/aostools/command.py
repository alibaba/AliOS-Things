# -*- coding:utf-8 -*-
#
# Copyright (C) 2008 The Android Open Source Project
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
import optparse
import platform
import re
import sys


class Command(object):
    """Base class for any command line action in repo.
    """

    common = False
    manifest = None
    _optparse = None

    def WantPager(self, _opt):
        return False

    def ReadEnvironmentOptions(self, opts):
        """ Set options from environment variables. """

        env_options = self._RegisteredEnvironmentOptions()

        for env_key, opt_key in env_options.items():
            # Get the user-set option value if any
            opt_value = getattr(opts, opt_key)

            # If the value is set, it means the user has passed it as a command
            # line option, and we should use that.  Otherwise we can try to set it
            # with the value from the corresponding environment variable.
            if opt_value is not None:
                continue

            env_value = os.environ.get(env_key)
            if env_value is not None:
                setattr(opts, opt_key, env_value)

        return opts

    @property
    def OptionParser(self):
        if self._optparse is None:
            try:
                me = 'aos %s' % self.NAME
                usage = self.helpUsage.strip().replace('%prog', me)
            except AttributeError:
                usage = 'aos %s' % self.NAME
            self._optparse = optparse.OptionParser(usage=usage)
            self._Options(self._optparse)
        return self._optparse

    def _Options(self, p):
        """Initialize the option parser.
        """

    def _RegisteredEnvironmentOptions(self):
        """Get options that can be set from environment variables.

        Return a dictionary mapping environment variable name
        to option key name that it can override.

        Example: {'REPO_MY_OPTION': 'my_option'}

        Will allow the option with key value 'my_option' to be set
        from the value in the environment variable named 'REPO_MY_OPTION'.

        Note: This does not work properly for options that are explicitly
        set to None by the user, or options that are defined with a
        default value other than None.

        """
        return {}

    def Usage(self):
        """Display usage and terminate.
        """
        self.OptionParser.print_usage()
        sys.exit(1)

    def ValidateOptions(self, opt, args):
        """Validate the user options & arguments before executing.

        This is meant to help break the code up into logical steps.  Some tips:
        * Use self.OptionParser.error to display CLI related errors.
        * Adjust opt member defaults as makes sense.
        * Adjust the args list, but do so inplace so the caller sees updates.
        * Try to avoid updating self state.  Leave that to Execute.
        """

    def Execute(self, opt, args):
        """Perform the action, after option parsing is complete.
        """
        raise NotImplementedError
