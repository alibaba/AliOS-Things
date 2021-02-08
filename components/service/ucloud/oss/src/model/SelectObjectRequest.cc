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

#include <alibabacloud/oss/model/SelectObjectRequest.h>
#include <iostream>
#include <sstream>
#include <cstring>
#include "ModelError.h"
#include "../utils/Utils.h"
#include "../utils/LogUtils.h"
#include "../utils/Crc32.h"
#include "../utils/StreamBuf.h"

#define FRAME_HEADER_LEN   (12+8)

using namespace AlibabaCloud::OSS;


struct SelectObjectFrame {
    int frame_type;
    int init_crc32;
    int32_t header_len;
    int32_t tail_len;
    int32_t payload_remains;
    uint8_t tail[4];
    uint8_t header[FRAME_HEADER_LEN];
    uint8_t end_frame[256];
    uint32_t end_frame_size;
    uint32_t payload_crc32;
};


class SelectObjectStreamBuf : public StreamBufProxy
{
public:
    SelectObjectStreamBuf(std::iostream& stream, int initCrc32) :
        StreamBufProxy(stream), 
        lastStatus_(0)
    {
        // init frame
        frame_.init_crc32 = initCrc32;
        frame_.header_len = 0;
        frame_.tail_len = 0;
        frame_.payload_remains = 0;
        frame_.end_frame_size = 0;
    };

    int LastStatus()
    {
        return lastStatus_;
    }

protected:
    int selectObjectDepackFrame(const char *ptr, int len, int *frame_type, int *payload_len, char **payload_buf, SelectObjectFrame *frame)
    {
        int remain = len;
        //Version | Frame - Type | Payload Length | Header Checksum | Payload | Payload Checksum
        //<1 byte> <--3 bytes-->   <-- 4 bytes --> <------4 bytes--> <variable><----4bytes------>
        //Payload 
        //<offset | data>
        //<8 types><variable>

        // header
        if (frame->header_len < FRAME_HEADER_LEN) {
            int copy = FRAME_HEADER_LEN - frame->header_len;
            copy = ((remain > copy) ? copy : remain);
            memcpy(frame->header + frame->header_len, ptr, copy);
            frame->header_len += copy;
            ptr += copy;
            remain -= copy;

            // if deal with header done
            if (frame->header_len == FRAME_HEADER_LEN) {
                uint32_t payload_length;
                // calculation payload length
                payload_length = frame->header[4];
                payload_length = (payload_length << 8) | frame->header[5];
                payload_length = (payload_length << 8) | frame->header[6];
                payload_length = (payload_length << 8) | frame->header[7];
                frame->payload_remains = payload_length - 8;
                frame->payload_crc32 = CRC32::CalcCRC(frame->init_crc32, frame->header + 12, 8);
            }
        }

        // payload
        if (frame->payload_remains > 0) {
            int copy = (frame->payload_remains > remain) ? remain : frame->payload_remains;
            uint32_t type;
            type = frame->header[1];
            type = (type << 8) | frame->header[2];
            type = (type << 8) | frame->header[3];
            *frame_type = type;
            *payload_len = copy;
            *payload_buf = (char *)ptr;
            remain -= copy;
            frame->payload_remains -= copy;
            frame->payload_crc32 = CRC32::CalcCRC(frame->payload_crc32, ptr, copy);
            return len - remain;
        }

        // tail
        if (frame->tail_len < 4) {
            int copy = 4 - frame->tail_len;
            copy = (copy > remain ? remain : copy);
            memcpy(frame->tail + frame->tail_len, ptr, copy);
            frame->tail_len += copy;
            remain -= copy;
            *frame_type = 0;
        }

        return len - remain;
    }

    int selectObjectTransferContent(SelectObjectFrame *frame, const char *ptr, int wanted) {
        int remain = wanted;
        char *payload_buf;
        // the actual length of the write
        int result = 0;
        // deal with the whole buffer
        while (remain > 0) {
            int  frame_type = 0, payload_len = 0;
            int ret = selectObjectDepackFrame(ptr, remain, &frame_type, &payload_len, &payload_buf, frame);
            switch (frame_type)
            {
            case 0x800001:
                int temp;
                temp = static_cast<int>(StreamBufProxy::xsputn(payload_buf, payload_len));
                if (temp < 0) {
                    return temp;
                }
                result += temp;
                break;
            case 0x800004: //Continuous Frame
                break;
            case 0x800005: //Select object End Frame
            {
                int32_t copy = sizeof(frame->end_frame) - frame->end_frame_size;
                copy = (copy > payload_len) ? payload_len : copy;
                if (copy > 0) {
                    memcpy(frame->end_frame + frame->end_frame_size, ptr, copy);
                    frame->end_frame_size += copy;
                }
            }
            break;
            default:
                // get payload checksum
                if (frame->tail_len == 4) {
                    // compare check sum
                    uint32_t payload_crc32;
                    payload_crc32 = frame->tail[0];
                    payload_crc32 = (payload_crc32 << 8) | frame->tail[1];
                    payload_crc32 = (payload_crc32 << 8) | frame->tail[2];
                    payload_crc32 = (payload_crc32 << 8) | frame->tail[3];
                    if (payload_crc32 != 0 && payload_crc32 != frame->payload_crc32) {
                        // CRC32 Checksum failed
                        return -1;
                    }

                    // reset to get next frame
                    frame->header_len = 0;
                    frame->tail_len = 0;
                    frame->payload_remains = 0;
                    frame->end_frame_size = 0;
                }
                break;
            }
            ptr += ret;
            remain -= ret;
        }
        return result;
    }

    std::streamsize xsputn(const char *ptr, std::streamsize count)
    {
        int result = selectObjectTransferContent(&frame_, ptr, static_cast<int>(count));
        if (result < 0) {
            if (result == -1) {
                lastStatus_ = ARG_ERROR_SELECT_OBJECT_CHECK_SUM_FAILED;
            }
            return static_cast<std::streamsize>(result);
        }
        return count;
    }

private:
    SelectObjectFrame frame_;
    int lastStatus_;
};

/////////////////////////////////////////////////////////////

SelectObjectRequest::SelectObjectRequest(const std::string& bucket, const std::string& key) :
    GetObjectRequest(bucket, key),
    expressionType_(ExpressionType::SQL),
    skipPartialDataRecord_(false),
    maxSkippedRecordsAllowed_(0),
    inputFormat_(nullptr),
    outputFormat_(nullptr),
    streamBuffer_(nullptr),
    upperContent_(nullptr)
{
    setResponseStreamFactory(ResponseStreamFactory());

    // close CRC Checksum
    int flag = Flags();
    flag |= REQUEST_FLAG_CONTENTMD5;
    flag &= ~REQUEST_FLAG_CHECK_CRC64;
    setFlags(flag);
}

void SelectObjectRequest::setResponseStreamFactory(const IOStreamFactory& factory)
{
    upperResponseStreamFactory_ = factory;
    ServiceRequest::setResponseStreamFactory([=]() {
        streamBuffer_ = nullptr;
        auto content = upperResponseStreamFactory_();
        if (!outputFormat_->OutputRawData()) {
            int initCrc32 = 0;
#ifdef ENABLE_OSS_TEST
            if (!!(Flags() & 0x20000000)) {
                const char* TAG = "SelectObjectClient";
                OSS_LOG(LogLevel::LogDebug, TAG, "Payload checksum fail.");
                initCrc32 = 1;
            }
#endif // ENABLE_OSS_TEST
            streamBuffer_ = std::make_shared<SelectObjectStreamBuf>(*content, initCrc32);
        }
        upperContent_ = content;
        return content;
    });
}

uint64_t SelectObjectRequest::MaxSkippedRecordsAllowed() const
{
    return maxSkippedRecordsAllowed_;
}

void SelectObjectRequest::setSkippedRecords(bool skipPartialDataRecord, uint64_t maxSkippedRecords)
{
    skipPartialDataRecord_ = skipPartialDataRecord;
    maxSkippedRecordsAllowed_ = maxSkippedRecords;
}

void SelectObjectRequest::setExpression(const std::string& expression, ExpressionType type)
{
    expressionType_ = type;
    expression_ = expression;
}

void SelectObjectRequest::setInputFormat(InputFormat& inputFormat)
{
    inputFormat_ = &inputFormat;
}

void SelectObjectRequest::setOutputFormat(OutputFormat& OutputFormat)
{
    outputFormat_ = &OutputFormat;
}

int SelectObjectRequest::validate() const
{
    int ret = GetObjectRequest::validate();
    if (ret != 0) {
        return ret;
    }

    if (expressionType_ != ExpressionType::SQL) {
        return ARG_ERROR_SELECT_OBJECT_NOT_SQL_EXPRESSION;
    }
    if (inputFormat_ == nullptr || outputFormat_ == nullptr) {
        return ARG_ERROR_SELECT_OBJECT_NULL_POINT;
    }

    ret = inputFormat_->validate();
    if (ret != 0) {
        return ret;
    }
    ret = outputFormat_->validate();
    if (ret != 0) {
        return ret;
    }
    // check type
    if (inputFormat_->Type() != outputFormat_->Type()) {
        return ARG_ERROR_SELECT_OBJECT_PROCESS_NOT_SAME;
    }
    return 0;
}

std::string SelectObjectRequest::payload() const
{
    std::stringstream ss;
    ss << "<SelectRequest>" << std::endl;
    // Expression
    ss << "<Expression>" << Base64Encode(expression_) << "</Expression>" << std::endl;
    // input format
    ss << inputFormat_->toXML(1) << std::endl;
    // output format
    ss << outputFormat_->toXML() << std::endl;
    // options
    ss << "<Options>" << std::endl;
    ss << "<SkipPartialDataRecord>" << (skipPartialDataRecord_ ? "true" : "false") << "</SkipPartialDataRecord>" << std::endl;
    ss << "<MaxSkippedRecordsAllowed>" << std::to_string(MaxSkippedRecordsAllowed()) << "</MaxSkippedRecordsAllowed>" << std::endl;
    ss << "</Options>" << std::endl;
    ss << "</SelectRequest>" << std::endl;
    return ss.str();
}

int SelectObjectRequest::dispose() const
{
    int ret = 0;
    if (streamBuffer_ != nullptr) {
        auto buf = std::static_pointer_cast<SelectObjectStreamBuf>(streamBuffer_);
        ret = buf->LastStatus();
        streamBuffer_ = nullptr;
    }
    upperContent_ = nullptr;
    return ret;
}

ParameterCollection SelectObjectRequest::specialParameters() const
{
    auto parameters = GetObjectRequest::specialParameters();
    if (inputFormat_) {
        auto type = inputFormat_->Type();
        type.append("/select");
        parameters["x-oss-process"] = type;
    }
    return parameters;
}
