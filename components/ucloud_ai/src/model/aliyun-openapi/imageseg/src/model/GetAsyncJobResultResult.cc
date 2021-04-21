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

#include <alibabacloud/imageseg/model/GetAsyncJobResultResult.h>
#include <json/json.h>

using namespace AlibabaCloud::Imageseg;
using namespace AlibabaCloud::Imageseg::Model;

GetAsyncJobResultResult::GetAsyncJobResultResult() :
	ServiceResult()
{}

GetAsyncJobResultResult::GetAsyncJobResultResult(const std::string &payload) :
	ServiceResult()
{
	parse(payload);
}

GetAsyncJobResultResult::~GetAsyncJobResultResult()
{}

void GetAsyncJobResultResult::parse(const std::string &payload)
{
	Json::Reader reader;
	Json::Value value;
	reader.parse(payload, value);
	setRequestId(value["RequestId"].asString());
	auto dataNode = value["Data"];
	if(!dataNode["ErrorCode"].isNull())
		data_.errorCode = dataNode["ErrorCode"].asString();
	if(!dataNode["ErrorMessage"].isNull())
		data_.errorMessage = dataNode["ErrorMessage"].asString();
	if(!dataNode["JobId"].isNull())
		data_.jobId = dataNode["JobId"].asString();
	if(!dataNode["Result"].isNull())
		data_.result = dataNode["Result"].asString();
	if(!dataNode["Status"].isNull())
		data_.status = dataNode["Status"].asString();

}

GetAsyncJobResultResult::Data GetAsyncJobResultResult::getData()const
{
	return data_;
}

