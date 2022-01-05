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

ifeq ($(WIN_PLAT),y)
SUBMODGOALS := $(sort $(foreach m, $(MAKECMDGOALS), \
  $(if $(filter-out ./,$(wildcard $(dir $(subst \,/,$m)) $(subst \,/,$m))),$m,)))
else
  SUBMODGOALS := $(sort $(foreach m, $(MAKECMDGOALS), \
  $(if $(filter-out ./,$(wildcard $(dir $m) $m)),$m,)))
endif

ifneq ($(SUBMODGOALS),)
MAKECMDGOALS := $(filter-out $(SUBMODGOALS),$(MAKECMDGOALS))
ifeq ($(WIN_PLAT),y)
SUBMODGOALS := $(subst \,/,$(SUBMODGOALS))
endif
SUBMODS := $(patsubst $(CURDIR)/%,%,$(SUBMODGOALS))
SUBMODS := $(patsubst %/,%,$(SUBMODS))
# Filter out subdirectories if their parent directories already in SUBMODS
SUBMODS := $(filter-out $(addsuffix /%,$(SUBMODS)),$(SUBMODS))
export SUBMODS
endif

