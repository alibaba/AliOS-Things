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

#include <alibabacloud/oss/model/OutputFormat.h>
#include <sstream>
#include "../utils/Utils.h"

using namespace AlibabaCloud::OSS;

OutputFormat::OutputFormat():
	keepAllColumns_(false), outputRawData_(false),
	enablePayloadCrc_(true), outputHeader_(false)
{}

void OutputFormat::setEnablePayloadCrc(bool enablePayloadCrc)
{
	enablePayloadCrc_ = enablePayloadCrc;
}

void OutputFormat::setKeepAllColumns(bool keepAllColumns)
{
	keepAllColumns_ = keepAllColumns;
}

void OutputFormat::setOutputHeader(bool outputHeader)
{
	outputHeader_ = outputHeader;
}

void OutputFormat::setOutputRawData(bool outputRawData)
{
	outputRawData_ = outputRawData;
}

bool OutputFormat::OutputRawData() const
{
    return outputRawData_;
}

bool OutputFormat::KeepAllColumns() const
{
    return keepAllColumns_;
}

bool OutputFormat::EnablePayloadCrc() const
{
    return enablePayloadCrc_;
}

bool OutputFormat::OutputHeader() const
{
    return outputHeader_;
}

int OutputFormat::validate() const
{
    return 0;
}

////////////////////////////////////////////////////////////////////////////////////////

CSVOutputFormat::CSVOutputFormat()
    : CSVOutputFormat("\n", ",")
{}

CSVOutputFormat::CSVOutputFormat(
    const std::string& recordDelimiter, 
    const std::string& fieldDelimiter)
    : OutputFormat(), recordDelimiter_(recordDelimiter), fieldDelimiter_(fieldDelimiter)
{}

void CSVOutputFormat::setRecordDelimiter(const std::string& recordDelimiter) 
{
    recordDelimiter_ = recordDelimiter;
}

void CSVOutputFormat::setFieldDelimiter(const std::string& fieldDelimiter)
{
    fieldDelimiter_ = fieldDelimiter;
}

const std::string& CSVOutputFormat::FieldDelimiter() const
{
    return fieldDelimiter_;
}

const std::string& CSVOutputFormat::RecordDelimiter() const
{
    return recordDelimiter_;
}

std::string CSVOutputFormat::Type() const
{
    return "csv";
}

std::string CSVOutputFormat::toXML() const
{
    std::stringstream ss;
    ss << "<OutputSerialization>" << std::endl;
    ss << "<CSV>" << std::endl;
    ss << "<RecordDelimiter>" << Base64Encode(recordDelimiter_) << "</RecordDelimiter>" << std::endl;
    ss << "<FieldDelimiter>" << Base64Encode(fieldDelimiter_.empty() ? "" : std::string(1, fieldDelimiter_.front())) << "</FieldDelimiter>" << std::endl;
    ss << "</CSV>" << std::endl;
    ss << "<KeepAllColumns>" << (OutputFormat::KeepAllColumns() ? "true" : "false") << "</KeepAllColumns>" << std::endl;
    ss << "<OutputRawData>" << (OutputFormat::OutputRawData() ? "true" : "false") << "</OutputRawData>" << std::endl;
    ss << "<OutputHeader>" << (OutputFormat::OutputHeader() ? "true" : "false") << "</OutputHeader>" << std::endl;
    ss << "<EnablePayloadCrc>" << (OutputFormat::EnablePayloadCrc() ? "true" : "false") << "</EnablePayloadCrc>" << std::endl;
    ss << "</OutputSerialization>" << std::endl;
    return ss.str();
}

////////////////////////////////////////////////////////////////////////////////////////

JSONOutputFormat::JSONOutputFormat()
    :JSONOutputFormat("\n")
{}

JSONOutputFormat::JSONOutputFormat(const std::string& recordDelimiter)
    :OutputFormat(), recordDelimiter_(recordDelimiter)
{}

void JSONOutputFormat::setRecordDelimiter(const std::string& recordDelimiter)
{
    recordDelimiter_ = recordDelimiter;
}

const std::string& JSONOutputFormat::RecordDelimiter() const
{
    return recordDelimiter_;
}

std::string JSONOutputFormat::Type() const
{
    return "json";
}

std::string JSONOutputFormat::toXML() const
{
    std::stringstream ss;
    ss << "<OutputSerialization>" << std::endl;
    ss << "<JSON>" << std::endl;
    ss << "<RecordDelimiter>" << Base64Encode(recordDelimiter_) << "</RecordDelimiter>" << std::endl;
    ss << "</JSON>" << std::endl;
    ss << "<KeepAllColumns>" << (OutputFormat::KeepAllColumns() ? "true" : "false") << "</KeepAllColumns>" << std::endl;
    ss << "<OutputRawData>" << (OutputFormat::OutputRawData() ? "true" : "false") << "</OutputRawData>" << std::endl;
    ss << "<OutputHeader>" << (OutputFormat::OutputHeader() ? "true" : "false") << "</OutputHeader>" << std::endl;
    ss << "<EnablePayloadCrc>" << (OutputFormat::EnablePayloadCrc() ? "true" : "false") << "</EnablePayloadCrc>" << std::endl;
    ss << "</OutputSerialization>" << std::endl;
    return ss.str();
}
