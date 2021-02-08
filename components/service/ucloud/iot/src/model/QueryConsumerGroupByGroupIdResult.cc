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

#include <alibabacloud/iot/model/QueryConsumerGroupByGroupIdResult.h>
#include <json/json.h>

using namespace AlibabaCloud::Iot;
using namespace AlibabaCloud::Iot::Model;

QueryConsumerGroupByGroupIdResult::QueryConsumerGroupByGroupIdResult() :
	ServiceResult()
{}

QueryConsumerGroupByGroupIdResult::QueryConsumerGroupByGroupIdResult(const std::string &payload) :
	ServiceResult()
{
	parse(payload);
}

QueryConsumerGroupByGroupIdResult::~QueryConsumerGroupByGroupIdResult()
{}

void QueryConsumerGroupByGroupIdResult::parse(const std::string &payload)
{
	Json::Reader reader;
	Json::Value value;
	reader.parse(payload, value);
	setRequestId(value["RequestId"].asString());
	auto dataNode = value["Data"];
	if(!dataNode["IotId"].isNull())
		data_.iotId = dataNode["IotId"].asString();
	if(!dataNode["GroupId"].isNull())
		data_.groupId = dataNode["GroupId"].asString();
	if(!dataNode["GroupName"].isNull())
		data_.groupName = dataNode["GroupName"].asString();
	if(!dataNode["Creator"].isNull())
		data_.creator = dataNode["Creator"].asString();
	if(!dataNode["CreateTime"].isNull())
		data_.createTime = dataNode["CreateTime"].asString();
	if(!dataNode["AuthType"].isNull())
		data_.authType = dataNode["AuthType"].asString();
	if(!value["Success"].isNull())
		success_ = value["Success"].asString() == "true";
	if(!value["ErrorMessage"].isNull())
		errorMessage_ = value["ErrorMessage"].asString();
	if(!value["Code"].isNull())
		code_ = value["Code"].asString();

}

QueryConsumerGroupByGroupIdResult::Data QueryConsumerGroupByGroupIdResult::getData()const
{
	return data_;
}

std::string QueryConsumerGroupByGroupIdResult::getErrorMessage()const
{
	return errorMessage_;
}

std::string QueryConsumerGroupByGroupIdResult::getCode()const
{
	return code_;
}

bool QueryConsumerGroupByGroupIdResult::getSuccess()const
{
	return success_;
}

