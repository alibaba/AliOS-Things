# Copyright 2019-2021 Google LLC
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

function(ruy_include_directories NAME DEPS)
  target_include_directories(${NAME}
      PUBLIC
      "${PROJECT_SOURCE_DIR}"
  )
  if (cpuinfo IN_LIST DEPS)
    target_include_directories(${NAME}
      PRIVATE
        "${PROJECT_SOURCE_DIR}/third_party/cpuinfo/include"
    )
  endif()
  if ((gtest IN_LIST DEPS) OR
      (gtest_main IN_LIST DEPS))
    target_include_directories(${NAME}
      PRIVATE
        "${PROJECT_SOURCE_DIR}/third_party/googletest/googletest"
    )
  endif()
endfunction()