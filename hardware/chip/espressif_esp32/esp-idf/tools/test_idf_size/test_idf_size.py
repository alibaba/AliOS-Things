#!/usr/bin/env python
#
# Copyright 2018 Espressif Systems (Shanghai) PTE LTD
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

from __future__ import print_function
import sys
import collections

try:
    import idf_size
except ImportError:
    sys.path.append('..')
    import idf_size


if __name__ == "__main__":
    # Should deliver a RuntimeError as the 'test' header doesn't exist
    try:
        idf_size.scan_to_header([], 'test')
    except RuntimeError as e:
        assert "Didn't find line" in str(e)

    # Should deliver a RuntimeError as there's no content under the heading
    try:
        idf_size.load_memory_config(["Memory Configuration"])
        pass
    except RuntimeError as e:
        assert "End of file" in str(e)

    # This used to crash with a division by zero error but now it just prints nan% due to
    # zero lengths
    MemRegNames = collections.namedtuple('MemRegNames', ['iram_names', 'dram_names', 'diram_names', 'used_iram_names',
                                                         'used_dram_names', 'used_diram_names'])
    mem_reg = MemRegNames(set(), set(), set(), set(), set(), set())

    print(idf_size.get_summary('a.map', mem_reg, {"iram0_0_seg": {"origin":0,"length":0}, "dram0_0_seg":
          {"origin":0, "length":0}}, {}), end="")
