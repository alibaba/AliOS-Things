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
#include <alibabacloud/oss/OssRequest.h>
#include <alibabacloud/oss/model/GetObjectRequest.h>
#include <alibabacloud/oss/Types.h>
#include <alibabacloud/oss/model/InputFormat.h>
#include <alibabacloud/oss/model/OutputFormat.h>
#include <alibabacloud/oss/model/CreateSelectObjectMetaRequest.h>

namespace AlibabaCloud
{
namespace OSS
{
	enum ExpressionType
	{
		SQL,
	};
    class OssClientImpl;
	class ALIBABACLOUD_OSS_EXPORT SelectObjectRequest : public GetObjectRequest
	{
	public:
		SelectObjectRequest(const std::string& bucket, const std::string& key);

		void setExpression(const std::string& expression, ExpressionType type = SQL);
        void setSkippedRecords(bool skipPartialDataRecord, uint64_t maxSkippedRecords);
        void setInputFormat(InputFormat& inputFormat);
        void setOutputFormat(OutputFormat& OutputFormat);

        uint64_t MaxSkippedRecordsAllowed() const;
        void setResponseStreamFactory(const IOStreamFactory& factory);

	protected:
        friend class OssClientImpl;
		virtual std::string payload() const;
		virtual int validate() const;
        virtual ParameterCollection specialParameters() const;
        int dispose() const;
	private:
		ExpressionType expressionType_;
		std::string expression_;
		bool skipPartialDataRecord_;
		uint64_t maxSkippedRecordsAllowed_;
		InputFormat *inputFormat_;
		OutputFormat *outputFormat_;

        mutable std::shared_ptr<std::streambuf> streamBuffer_;
        mutable std::shared_ptr<std::iostream> upperContent_;
        IOStreamFactory upperResponseStreamFactory_;
	};

}
}