#!/bin/sh

# travis-log-failure.sh
#
# Copyright The Mbed TLS Contributors
# SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later
#
# This file is provided under the Apache License 2.0, or the
# GNU General Public License v2.0 or later.
#
# **********
# Apache License 2.0:
#
# Licensed under the Apache License, Version 2.0 (the "License"); you may
# not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
# http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
# WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#
# **********
#
# **********
# GNU General Public License v2.0 or later:
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License along
# with this program; if not, write to the Free Software Foundation, Inc.,
# 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
#
# **********
#
# Purpose
#
# List the server and client logs on failed ssl-opt.sh and compat.sh tests.
# This script is used to make the logs show up in the Travis test results.
#
# Some of the logs can be very long: this means usually a couple of megabytes
# but it can be much more. For example, the client log of test 273 in ssl-opt.sh
# is more than 630 Megabytes long.

if [ -d include/mbedtls ]; then :; else
    echo "$0: must be run from root" >&2
    exit 1
fi

FILES="o-srv-*.log o-cli-*.log c-srv-*.log c-cli-*.log o-pxy-*.log"
MAX_LOG_SIZE=1048576

for PATTERN in $FILES; do
    for LOG in $( ls tests/$PATTERN 2>/dev/null ); do
        echo
        echo "****** BEGIN file: $LOG ******"
        echo
        tail -c $MAX_LOG_SIZE $LOG
        echo "****** END file: $LOG ******"
        echo
        rm $LOG
    done
done
