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

#include <alibabacloud/oss/model/InputFormat.h>
#include <sstream>
#include "../utils/Utils.h"
#include "ModelError.h"

using namespace AlibabaCloud::OSS;

static inline std::string CSVHeader_Str(CSVHeader num)
{
    static const std::string strings[] = {
        "None\0", "Ignore\0", "Use\0"
    };
    return strings[num];
}

static inline std::string CompressionType_Str(CompressionType num)
{
    static const std::string strings[] = {
        "NONE\0",
        "GZIP\0"
    };
    return strings[num];
}

static inline std::string JsonType_Str(JsonType num)
{
    static const std::string strings[] = {
        "DOCUMENT\0",
        "LINES\0"
    };
    return strings[num];
}

static std::string Range_Str(const std::string rangePrefix,
    bool isSet, const int64_t range[2])
{
    int64_t start = 0, end = 0;
    start = range[0];
    end = range[1];

    if (!isSet ||
        (start < 0 && end < 0) ||
        (start > 0 && end > 0 && start > end)) {
        return "";
    }

    std::ostringstream ostr;
    if (start < 0) {
        ostr << rangePrefix << "-" << end;
    }
    else if (end < 0) {
        ostr << rangePrefix << start << "-";
    }
    else {
        ostr << rangePrefix << start << "-" << end;
    }
    std::string str = ostr.str();
    return str;
}


InputFormat::InputFormat() :
	compressionType_(CompressionType::NONE),lineRangeIsSet_(false), splitRangeIsSet_(false)
{
}

void InputFormat::setCompressionType(CompressionType compressionType)
{
	compressionType_ = compressionType;
}

const std::string InputFormat::CompressionTypeInfo() const
{
	std::string str = CompressionType_Str(compressionType_);
	return str;
}

void InputFormat::setLineRange(int64_t start, int64_t end)
{
    lineRange_[0] = start;
    lineRange_[1] = end;
    lineRangeIsSet_ = true;
    splitRangeIsSet_ = false;
}

void InputFormat::setSplitRange(int64_t start, int64_t end) 
{
    splitRange_[0] = start;
    splitRange_[1] = end;
    splitRangeIsSet_ = true;
    lineRangeIsSet_ = false;
}

int InputFormat::validate() const
{
    if (lineRangeIsSet_ && splitRangeIsSet_) {
        return ARG_ERROR_SELECT_OBJECT_RANGE_INVALID;
    }

    if (lineRangeIsSet_ &&
        (lineRange_[0] < 0 || lineRange_[1] < -1 || (lineRange_[1] > -1 && lineRange_[1] < lineRange_[0]))) {
        return ARG_ERROR_SELECT_OBJECT_LINE_RANGE_INVALID;
    }
    if (splitRangeIsSet_ &&
        (splitRange_[0] < 0 || splitRange_[1] < -1 || (splitRange_[1] > -1 && splitRange_[1] < splitRange_[0]))) {
        return ARG_ERROR_SELECT_OBJECT_SPLIT_RANGE_INVALID;
    }

    return 0;
}

std::string InputFormat::RangeToString() const
{
    if (lineRangeIsSet_ && splitRangeIsSet_) {
        return "";
    }
    if (lineRangeIsSet_) {
        return Range_Str("line-range=", lineRangeIsSet_, lineRange_);
    }
    if (splitRangeIsSet_) {
        return Range_Str("split-range=", splitRangeIsSet_, splitRange_);
    }
    return "";
}

/////////////////////////////////////////////////////////////////////////////////////////

CSVInputFormat::CSVInputFormat()
    : CSVInputFormat(CSVHeader::None, "\n", ",", "\"", "#")
{}

CSVInputFormat::CSVInputFormat(CSVHeader headerInfo,
    const std::string& recordDelimiter,
    const std::string& fieldDelimiter,
    const std::string& quoteChar,
    const std::string& commentChar):
    InputFormat(),
    headerInfo_(headerInfo),
    recordDelimiter_(recordDelimiter),
    fieldDelimiter_(fieldDelimiter),
    quoteChar_(quoteChar),
    commentChar_(commentChar)
{}

void CSVInputFormat::setHeaderInfo(CSVHeader headerInfo)
{
    headerInfo_ = headerInfo;
}

void CSVInputFormat::setCommentChar(const std::string& commentChar)
{
    commentChar_ = commentChar;
}

void CSVInputFormat::setQuoteChar(const std::string& quoteChar)
{
    quoteChar_ = quoteChar;
}

void CSVInputFormat::setFieldDelimiter(const std::string& fieldDelimiter)
{
    fieldDelimiter_ = fieldDelimiter;
}

void CSVInputFormat::setRecordDelimiter(const std::string& recordDelimiter)
{
    recordDelimiter_ = recordDelimiter;
}

CSVHeader CSVInputFormat::HeaderInfo() const
{
    return headerInfo_;
}

const std::string& CSVInputFormat::RecordDelimiter() const
{
    return recordDelimiter_;
}

const std::string& CSVInputFormat::FieldDelimiter() const
{
    return fieldDelimiter_;
}

const std::string& CSVInputFormat::QuoteChar() const
{
    return quoteChar_;
}

const std::string& CSVInputFormat::CommentChar() const
{
    return commentChar_;
}

std::string CSVInputFormat::Type() const
{
    return "csv";
}

std::string CSVInputFormat::toXML(int flag) const
{
    std::stringstream ss;
    ss << "<InputSerialization>" << std::endl;
    ss << "<CompressionType>" << InputFormat::CompressionTypeInfo() << "</CompressionType>" << std::endl;
    ss << "<CSV>" << std::endl;
    ss << "<RecordDelimiter>" << Base64Encode(recordDelimiter_) << "</RecordDelimiter>" << std::endl;
    ss << "<FieldDelimiter>" << Base64Encode(fieldDelimiter_.empty() ? "" : std::string(1, fieldDelimiter_.front())) << "</FieldDelimiter>" << std::endl;
    ss << "<QuoteCharacter>" << Base64Encode(quoteChar_.empty() ? "" : std::string(1, quoteChar_.front())) << "</QuoteCharacter>" << std::endl;
    if (flag == 1) {
        ss << "<FileHeaderInfo>" << CSVHeader_Str(headerInfo_) << "</FileHeaderInfo>" << std::endl;
        ss << "<CommentCharacter>" << Base64Encode(commentChar_.empty() ? "" : std::string(1, commentChar_.front())) << "</CommentCharacter>" << std::endl;
        ss << "<Range>" << InputFormat::RangeToString() << "</Range>" << std::endl;
    }
    ss << "</CSV>" << std::endl;
    ss << "</InputSerialization>" << std::endl;
    return ss.str();
}

////////////////////////////////////////////////////////////////////////////////////////
JSONInputFormat::JSONInputFormat()
    : InputFormat()
{}

JSONInputFormat::JSONInputFormat(JsonType jsonType)
    : InputFormat(), jsonType_(jsonType)
{}

void JSONInputFormat::setJsonType(JsonType jsonType)
{
    jsonType_ = jsonType;
}

void JSONInputFormat::setParseJsonNumberAsString(bool parseJsonNumberAsString)
{
    parseJsonNumberAsString_ = parseJsonNumberAsString;
}

JsonType JSONInputFormat::JsonInfo() const
{
    return jsonType_;
}

bool JSONInputFormat::ParseJsonNumberAsString() const
{
    return parseJsonNumberAsString_;
}

std::string JSONInputFormat::Type() const
{
    return "json";
}

std::string JSONInputFormat::toXML(int flag) const
{
    std::stringstream ss;
    ss << "<InputSerialization>" << std::endl;
    ss << "<CompressionType>" << InputFormat::CompressionTypeInfo() << "</CompressionType>" << std::endl;
    ss << "<JSON>" << std::endl;
    if (flag == 1) {
        ss << "<Type>" << JsonType_Str(jsonType_) << "</Type>" << std::endl;
        ss << "<ParseJsonNumberAsString>" << (parseJsonNumberAsString_ ? "true" : "false") << "</ParseJsonNumberAsString>" << std::endl;
        ss << "<Range>" << InputFormat::RangeToString() << "</Range>" << std::endl;
    }
    else {
        ss << "<Type>" << "LINES" << "</Type>" << std::endl;
    }
    
    ss << "</JSON>" << std::endl;
    ss << "</InputSerialization>" << std::endl;
    return ss.str();
}