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

#include <alibabacloud/iot/model/QueryProductTopicResult.h>
#include <json/json.h>

using namespace AlibabaCloud::Iot;
using namespace AlibabaCloud::Iot::Model;

QueryProductTopicResult::QueryProductTopicResult() :
	ServiceResult()
{}

QueryProductTopicResult::QueryProductTopicResult(const std::string &payload) :
	ServiceResult()
{
	parse(payload);
}

QueryProductTopicResult::~QueryProductTopicResult()
{}

void QueryProductTopicResult::parse(const std::string &payload)
{
	Json::Reader reader;
	Json::Value value;
	reader.parse(payload, value);
	setRequestId(value["RequestId"].asString());
	auto allDataNode = value["Data"]["ProductTopicInfo"];
	for (auto valueDataProductTopicInfo : allDataNode)
	{
		ProductTopicInfo dataObject;
		if(!valueDataProductTopicInfo["ProductKey"].isNull())
			dataObject.productKey = valueDataProductTopicInfo["ProductKey"].asString();
		if(!valueDataProductTopicInfo["TopicShortName"].isNull())
			dataObject.topicShortName = valueDataProductTopicInfo["TopicShortName"].asString();
		if(!valueDataProductTopicInfo["Operation"].isNull())
			dataObject.operation = valueDataProductTopicInfo["Operation"].asString();
		if(!valueDataProductTopicInfo["Desc"].isNull())
			dataObject.desc = valueDataProductTopicInfo["Desc"].asString();
		if(!valueDataProductTopicInfo["Id"].isNull())
			dataObject.id = valueDataProductTopicInfo["Id"].asString();
		data_.push_back(dataObject);
	}
	if(!value["Success"].isNull())
		success_ = value["Success"].asString() == "true";
	if(!value["Code"].isNull())
		code_ = value["Code"].asString();
	if(!value["ErrorMessage"].isNull())
		errorMessage_ = value["ErrorMessage"].asString();

}

std::vector<QueryProductTopicResult::ProductTopicInfo> QueryProductTopicResult::getData()const
{
	return data_;
}

std::string QueryProductTopicResult::getErrorMessage()const
{
	return errorMessage_;
}

std::string QueryProductTopicResult::getCode()const
{
	return code_;
}

bool QueryProductTopicResult::getSuccess()const
{
	return success_;
}

