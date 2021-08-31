#!/bin/bash
# Copyright 2021 Google LLC
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      https://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

this_script_dir="$(dirname "$0")"

root_dir="$(git -C "${this_script_dir}" rev-parse --show-toplevel)"

build_files="$(find "${root_dir}" -type f -name BUILD)"

if ! command -v python3 &> /dev/null; then
  python_command=python
else
  python_command=python3
fi

for build_file in ${build_files}; do
    package_dir="$(dirname "${build_file}")"
    if [[ "${package_dir}" == "${root_dir}" ]]; then
      # The root CMakeLists.txt is not generated.
      continue
    fi
    "${python_command}" "${this_script_dir}/bazel_to_cmake.py" "${root_dir}" "${package_dir}" > "${package_dir}/CMakeLists.txt"
done
