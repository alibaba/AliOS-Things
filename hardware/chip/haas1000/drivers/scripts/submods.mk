# Copyright (c) 2021 Bestechnic (Shanghai) Co., Ltd. All rights reserved.
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
cur_makefile := $(lastword $(MAKEFILE_LIST))

$(cur_makefile): ;

get_subdirs = $(foreach m, $(1), $(if $(filter $m/% $m,$(2)),$m,))

get_submodgoals = $(foreach m, \
  $(patsubst $(1)/%,%,$(filter $(1)/%,$(2))), \
  $(addprefix $(1)/,$(firstword $(subst /,$(space),$m))))

get_multi_objs = $(addprefix $(obj)/, $($(subst $(obj)/,,$(1:.o=-objs))) \
  $($(subst $(obj)/,,$(1:.o=-objs))))

