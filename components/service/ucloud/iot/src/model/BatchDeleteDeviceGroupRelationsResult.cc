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

#include <alibabacloud/iot/model/BatchDeleteDeviceGroupRelationsResult.h>
#include <json/json.h>

using namespace AlibabaCloud::Iot;
using namespace AlibabaCloud::Iot::Model;

BatchDeleteDeviceGroupRelationsResult::BatchDeleteDeviceGroupRelationsResult() :
	ServiceResult()
{}

BatchDeleteDeviceGroupRelationsResult::BatchDeleteDeviceGroupRelationsResult(const std::string &payload) :
	ServiceResult()
{
	parse(payload);
}

BatchDeleteDeviceGroupRelationsResult::~BatchDeleteDeviceGroupRelationsResult()
{}

void BatchDeleteDeviceGroupRelationsResult::parse(const std::string &payload)
{
	Json::Reader reader;
	Json::Value value;
	reader.parse(payload, value);
	setRequestId(value["RequestId"].asString());
	if(!value["Success"].isNull())
		success_ = value["Success"].asString() == "true";
	if(!value["Code"].isNull())
		code_ = value["Code"].asString();
	if(!value["ErrorMessage"].isNull())
		errorMessage_ = value["ErrorMessage"].asString();
	if(!value["ValidDeviceCount"].isNull())
		validDeviceCount_ = std::stoi(value["ValidDeviceCount"].asString());
	if(!value["AlreadyRelatedGroupDeviceCount"].isNull())
		alreadyRelatedGroupDeviceCount_ = std::stoi(value["AlreadyRelatedGroupDeviceCount"].asString());
	if(!value["SuccessDeviceCount"].isNull())
		successDeviceCount_ = std::stoi(value["SuccessDeviceCount"].asString());

}

int BatchDeleteDeviceGroupRelationsResult::getValidDeviceCount()const
{
	return validDeviceCount_;
}

int BatchDeleteDeviceGroupRelationsResult::getSuccessDeviceCount()const
{
	return successDeviceCount_;
}

std::string BatchDeleteDeviceGroupRelationsResult::getErrorMessage()const
{
	return errorMessage_;
}

int BatchDeleteDeviceGroupRelationsResult::getAlreadyRelatedGroupDeviceCount()const
{
	return alreadyRelatedGroupDeviceCount_;
}

std::string BatchDeleteDeviceGroupRelationsResult::getCode()const
{
	return code_;
}

bool BatchDeleteDeviceGroupRelationsResult::getSuccess()const
{
	return success_;
}

