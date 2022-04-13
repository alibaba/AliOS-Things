/*
 * Copyright 2009-2017 Alibaba Cloud All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#pragma once
#include <stdint.h>
#include <cstddef>
namespace AlibabaCloud
{
namespace OSS
{
    class CRC64
    {
    public:
        static uint64_t CalcCRC(uint64_t crc, void *buf, size_t len);
        static uint64_t CombineCRC(uint64_t crc1, uint64_t crc2, uintmax_t len2);
        static uint64_t CalcCRC(uint64_t crc, void *buf, size_t len, bool little);
    };
}
}

