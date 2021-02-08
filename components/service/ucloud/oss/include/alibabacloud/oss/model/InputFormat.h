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
	enum CompressionType
	{
		NONE = 0,
		GZIP
	};

    enum CSVHeader
    {
        None = 0, // there is no csv header
        Ignore,   // we should ignore csv header and should not use csv header in select sql
        Use       // we can use csv header in select sql
    };

    enum JsonType
    {
        DOCUMENT = 0,
        LINES
    };

    class SelectObjectRequest;
    class CreateSelectObjectMetaRequest;

	class ALIBABACLOUD_OSS_EXPORT InputFormat
	{
	public:
        void setCompressionType(CompressionType compressionType);

        void setLineRange(int64_t start, int64_t end);
        void setSplitRange(int64_t start, int64_t end);

		const std::string CompressionTypeInfo() const;

        

    protected:
        InputFormat();
        friend SelectObjectRequest;
        friend CreateSelectObjectMetaRequest;
        virtual int validate() const;
        virtual std::string toXML(int flag) const = 0;
        virtual std::string Type() const = 0;
        std::string RangeToString() const;
	private:
		CompressionType compressionType_;
        bool lineRangeIsSet_;
        int64_t lineRange_[2];
        bool splitRangeIsSet_;
        int64_t splitRange_[2];
	};

    class ALIBABACLOUD_OSS_EXPORT CSVInputFormat : public InputFormat
    {
    public:
        CSVInputFormat();
        CSVInputFormat(CSVHeader headerInfo,
            const std::string& recordDelimiter,
            const std::string& fieldDelimiter,
            const std::string& quoteChar,
            const std::string& commentChar);

        void setHeaderInfo(CSVHeader headerInfo);
        void setRecordDelimiter(const std::string& recordDelimiter);
        void setFieldDelimiter(const std::string& fieldDelimiter);
        void setQuoteChar(const std::string& quoteChar);
        void setCommentChar(const std::string& commentChar);

        CSVHeader HeaderInfo() const;
        const std::string& RecordDelimiter() const;
        const std::string& FieldDelimiter() const;
        const std::string& QuoteChar() const;
        const std::string& CommentChar() const;

    protected:
        std::string Type() const;
        std::string toXML(int flag) const;
        
    private:
        CSVHeader headerInfo_;
        std::string recordDelimiter_;
        std::string fieldDelimiter_;
        std::string quoteChar_;
        std::string commentChar_;
    };

    class ALIBABACLOUD_OSS_EXPORT JSONInputFormat : public InputFormat
    {
    public:
        JSONInputFormat();
        JSONInputFormat(JsonType jsonType);

        void setJsonType(JsonType jsonType);
        void setParseJsonNumberAsString(bool parseJsonNumberAsString);

        JsonType JsonInfo() const;
        bool ParseJsonNumberAsString() const;

    protected:
        std::string Type() const;
        std::string toXML(int flag) const;

    private:
        JsonType jsonType_;
        bool parseJsonNumberAsString_;
    };

}
}
