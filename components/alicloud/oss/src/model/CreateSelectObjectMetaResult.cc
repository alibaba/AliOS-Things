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

#include <alibabacloud/oss/model/CreateSelectObjectMetaResult.h>
#include <iostream>
#include <sstream>
#include "../utils/Crc32.h"

#define FRAME_HEADER_LEN   (12+8)
#define PARSE_FOUR_BYTES(a, b, c, d) (((uint64_t)(a) << 24)|((uint64_t)(b) << 16)|((uint64_t)(c) << 8)|(d))
#define PARSE_EIGHT_BYTES(a, b, c, d, e, f, g, h) (((uint64_t)(a)<<56)|((uint64_t)(b)<<48)| ((uint64_t)(c)<<40)| ((uint64_t)(d)<<32)| ((uint64_t)(e)<<24)| ((uint64_t)(f)<<16)| ((uint64_t)(g)<<8)| (h))

using namespace AlibabaCloud::OSS;

CreateSelectObjectMetaResult::CreateSelectObjectMetaResult()
    :OssResult(),
    offset_(0), 
    totalScanned_(0), 
    status_(0), 
    splitsCount_(0), 
    rowsCount_(0), 
    colsCount_(0)
{
}

CreateSelectObjectMetaResult::CreateSelectObjectMetaResult( const std::string& bucket, const std::string& key,
    const std::string& requestId, const std::shared_ptr<std::iostream>& data) : 
    CreateSelectObjectMetaResult()
{
    bucket_ = bucket;
    key_ = key;
    requestId_ = requestId;
    *this = data;
}

CreateSelectObjectMetaResult& CreateSelectObjectMetaResult::operator=(const std::shared_ptr<std::iostream>& data)
{
    data->seekg(0, data->beg);
    uint8_t buffer[36];
    char messageBuffer[256];
    parseDone_ = true;
    while (data->good()) {
        // header 12 bytes
        data->read(reinterpret_cast<char*>(buffer), 12);
        if (!data->good()) {
            break;
        }
        // type 3 bytes
        int type = 0;
        type = (buffer[1] << 16) | (buffer[2] << 8) | buffer[3];
        // payload length 4 bytes
        int length = PARSE_FOUR_BYTES(buffer[4], buffer[5], buffer[6], buffer[7]);
        // header checksum

        // payload
        switch (type)
        {
        case 0x800006:
        case 0x800007:
        {
            uint32_t payloadCrc32 = 0;
            int messageLength = length - 32;
            data->read(reinterpret_cast<char*>(buffer), 32);
            payloadCrc32 = CRC32::CalcCRC(payloadCrc32, buffer, 32);
            // offset 8 bytes
            offset_ = PARSE_EIGHT_BYTES(buffer[0],buffer[1],buffer[2],buffer[3],
                    buffer[4],buffer[5],buffer[6],buffer[7]);
            // total scaned 8bytes
            totalScanned_ = PARSE_EIGHT_BYTES(buffer[8],buffer[9],buffer[10],buffer[11],
                    buffer[12],buffer[13],buffer[14],buffer[15]);
            // status 4 bytes
            status_ = PARSE_FOUR_BYTES(buffer[16], buffer[17], buffer[18], buffer[19]);
            // splitsCount 4 bytes
            splitsCount_ = PARSE_FOUR_BYTES(buffer[20], buffer[21], buffer[22], buffer[23]);
            // rowsCount 8 bytes
            rowsCount_ = PARSE_EIGHT_BYTES(buffer[24],buffer[25],buffer[26],buffer[27],
                    buffer[28],buffer[29],buffer[30],buffer[31]);

            if (type == 0x800006) {
                messageLength -= 4;
                // colsCount
                data->read(reinterpret_cast<char*>(buffer), 4);
                payloadCrc32 = CRC32::CalcCRC(payloadCrc32, buffer, 4);
                colsCount_ = PARSE_FOUR_BYTES(buffer[0],buffer[1],buffer[2],buffer[3]);
            }
            data->read(messageBuffer, messageLength);
            payloadCrc32 = CRC32::CalcCRC(payloadCrc32, messageBuffer, messageLength);
            errorMessage_ = std::string(messageBuffer);

            if (!data->good()) {
                parseDone_ = false;
                break;
            }

            // payload crc32 checksum
            uint32_t payloadChecksum = 0;
            data->read(reinterpret_cast<char*>(buffer), 4);
            payloadChecksum = PARSE_FOUR_BYTES(buffer[0],buffer[1],buffer[2],buffer[3]);
            if (payloadChecksum != 0 && payloadChecksum != payloadCrc32) {
                parseDone_ = false;
                return *this;
            }

        }
        break;
        default:
            data->seekg(length + 4, data->cur);
            break;
        }
    }
    return *this;
}
