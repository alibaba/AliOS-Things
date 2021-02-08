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

#include <alibabacloud/iot/model/DeleteTopicRouteTableResult.h>
#include <json/json.h>

using namespace AlibabaCloud::Iot;
using namespace AlibabaCloud::Iot::Model;

DeleteTopicRouteTableResult::DeleteTopicRouteTableResult() :
	ServiceResult()
{}

DeleteTopicRouteTableResult::DeleteTopicRouteTableResult(const std::string &payload) :
	ServiceResult()
{
	parse(payload);
}

DeleteTopicRouteTableResult::~DeleteTopicRouteTableResult()
{}

void DeleteTopicRouteTableResult::parse(const std::string &payload)
{
	Json::Reader reader;
	Json::Value value;
	reader.parse(payload, value);
	setRequestId(value["RequestId"].asString());
	auto allFailureTopics = value["FailureTopics"]["Topic"];
	for (const auto &item : allFailureTopics)
		failureTopics_.push_back(item.asString());
	if(!value["Success"].isNull())
		success_ = value["Success"].asString() == "true";
	if(!value["Code"].isNull())
		code_ = value["Code"].asString();
	if(!value["IsAllSucceed"].isNull())
		isAllSucceed_ = value["IsAllSucceed"].asString() == "true";
	if(!value["ErrorMessage"].isNull())
		errorMessage_ = value["ErrorMessage"].asString();

}

std::vector<std::string> DeleteTopicRouteTableResult::getFailureTopics()const
{
	return failureTopics_;
}

std::string DeleteTopicRouteTableResult::getErrorMessage()const
{
	return errorMessage_;
}

std::string DeleteTopicRouteTableResult::getCode()const
{
	return code_;
}

bool DeleteTopicRouteTableResult::getSuccess()const
{
	return success_;
}

bool DeleteTopicRouteTableResult::getIsAllSucceed()const
{
	return isAllSucceed_;
}

