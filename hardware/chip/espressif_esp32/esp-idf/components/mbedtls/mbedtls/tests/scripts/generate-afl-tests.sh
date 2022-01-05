#!/bin/sh

# This script splits the data test files containing the test cases into
# individual files (one test case per file) suitable for use with afl
# (American Fuzzy Lop). http://lcamtuf.coredump.cx/afl/
#
# Usage: generate-afl-tests.sh <test data file path>
#  <test data file path> - should be the path to one of the test suite files
#                          such as 'test_suite_mpi.data'
#
# Copyright (C) 2016, Arm Limited, All Rights Reserved
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
# This file is part of Mbed TLS (https://tls.mbed.org)

# Abort on errors
set -e

if [ -z $1 ]
then
    echo " [!] No test file specified" >&2
    echo "Usage: $0 <test data file>" >&2
    exit 1
fi

SRC_FILEPATH=$(dirname $1)/$(basename $1)
TESTSUITE=$(basename $1 .data)

THIS_DIR=$(basename $PWD)

if [ -d ../library -a -d ../include -a -d ../tests -a $THIS_DIR == "tests" ];
then :;
else
    echo " [!] Must be run from mbed TLS tests directory" >&2
    exit 1
fi

DEST_TESTCASE_DIR=$TESTSUITE-afl-tests
DEST_OUTPUT_DIR=$TESTSUITE-afl-out

echo " [+] Creating output directories" >&2

if [ -e $DEST_OUTPUT_DIR/* ];
then :
    echo " [!] Test output files already exist." >&2
    exit 1
else
    mkdir -p $DEST_OUTPUT_DIR
fi

if [ -e $DEST_TESTCASE_DIR/* ];
then :
    echo " [!] Test output files already exist." >&2
else
    mkdir -p $DEST_TESTCASE_DIR
fi

echo " [+] Creating test cases" >&2
cd $DEST_TESTCASE_DIR

split -p '^\s*$' ../$SRC_FILEPATH

for f in *;
do
    # Strip out any blank lines (no trim on OS X)
    sed '/^\s*$/d' $f >testcase_$f
    rm $f
done

cd ..

echo " [+] Test cases in $DEST_TESTCASE_DIR" >&2

