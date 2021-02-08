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
#include <alibabacloud/oss/Export.h>
#include <alibabacloud/oss/OssRequest.h>
#include <alibabacloud/oss/Types.h>

namespace AlibabaCloud
{
namespace OSS
{
    class SelectObjectRequest;
	class ALIBABACLOUD_OSS_EXPORT OutputFormat
	{
	public:
        virtual ~OutputFormat() {};
		void setKeepAllColumns(bool keepAllColumns);
		void setOutputRawData(bool outputRawData);
		void setEnablePayloadCrc(bool enablePayloadCrc);
		void setOutputHeader(bool outputHeader);

        bool OutputRawData() const;
        bool KeepAllColumns() const;
        bool EnablePayloadCrc() const;
        bool OutputHeader() const;
    protected:
        OutputFormat();
        friend SelectObjectRequest;
        virtual int validate() const;
        virtual std::string toXML() const = 0;
        virtual std::string Type() const = 0;
	private:
		bool keepAllColumns_;
		bool outputRawData_;
		bool enablePayloadCrc_;
		bool outputHeader_;
	};

    class ALIBABACLOUD_OSS_EXPORT CSVOutputFormat : public OutputFormat
    {
    public:
        CSVOutputFormat();
        CSVOutputFormat(
            const std::string& recordDelimiter,
            const std::string& fieldDelimiter);

        void setRecordDelimiter(const std::string& recordDelimiter);
        void setFieldDelimiter(const std::string& fieldDelimiter);

        const std::string& RecordDelimiter() const;
        const std::string& FieldDelimiter() const;
    protected:
        virtual std::string toXML() const;
        virtual std::string Type() const;
    private:
        std::string recordDelimiter_;
        std::string fieldDelimiter_;
    };

    class ALIBABACLOUD_OSS_EXPORT JSONOutputFormat : public OutputFormat
    {
    public:
        JSONOutputFormat();
        JSONOutputFormat(const std::string& recordDelimiter);

        void setRecordDelimiter(const std::string& recordDelimiter);
        const std::string& RecordDelimiter() const;
    protected:
        virtual std::string toXML() const;
        virtual std::string Type() const;
    private:
        std::string recordDelimiter_;
    };

}
}