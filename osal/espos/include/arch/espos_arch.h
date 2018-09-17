// Copyright 2015-2016 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef _ESPOS_ARCH_H_
#define _ESPOS_ARCH_H_

#if defined(ESPOS_FOR_ESP32)
#include "espos_esp32.h"
#elif defined(ESPOS_FOR_ESP8266)
#include "espos_esp8266.h"
#else
#error "no espressif platform"
#endif

#endif
