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

#include <alibabacloud/iot/model/BatchAddDeviceGroupRelationsResult.h>
#include <json/json.h>

using namespace AlibabaCloud::Iot;
using namespace AlibabaCloud::Iot::Model;

BatchAddDeviceGroupRelationsResult::BatchAddDeviceGroupRelationsResult() :
	ServiceResult()
{}

BatchAddDeviceGroupRelationsResult::BatchAddDeviceGroupRelationsResult(const std::string &payload) :
	ServiceResult()
{
	parse(payload);
}

BatchAddDeviceGroupRelationsResult::~BatchAddDeviceGroupRelationsResult()
{}

void BatchAddDeviceGroupRelationsResult::parse(const std::string &payload)
{
	Json::Reader reader;
	Json::Value value;
	reader.parse(payload, value);
	setRequestId(value["RequestId"].asString());
	if(!value["Success"].isNull())
		success_ = value["Success"].asString() == "true";
	if(!value["Code"].isNull())
		code_ = value["Code"].asString();
	if(!value["ValidDeviceCount"].isNull())
		validDeviceCount_ = std::stoi(value["ValidDeviceCount"].asString());
	if(!value["ExceedTenGroupDeviceCount"].isNull())
		exceedTenGroupDeviceCount_ = std::stoi(value["ExceedTenGroupDeviceCount"].asString());
	if(!value["AlreadyRelatedGroupDeviceCount"].isNull())
		alreadyRelatedGroupDeviceCount_ = std::stoi(value["AlreadyRelatedGroupDeviceCount"].asString());
	if(!value["SuccessAddedDeviceCount"].isNull())
		successAddedDeviceCount_ = std::stoi(value["SuccessAddedDeviceCount"].asString());
	if(!value["ErrorMessage"].isNull())
		errorMessage_ = value["ErrorMessage"].asString();

}

int BatchAddDeviceGroupRelationsResult::getValidDeviceCount()const
{
	return validDeviceCount_;
}

int BatchAddDeviceGroupRelationsResult::getSuccessAddedDeviceCount()const
{
	return successAddedDeviceCount_;
}

int BatchAddDeviceGroupRelationsResult::getExceedTenGroupDeviceCount()const
{
	return exceedTenGroupDeviceCount_;
}

int BatchAddDeviceGroupRelationsResult::getAlreadyRelatedGroupDeviceCount()const
{
	return alreadyRelatedGroupDeviceCount_;
}

std::string BatchAddDeviceGroupRelationsResult::getErrorMessage()const
{
	return errorMessage_;
}

std::string BatchAddDeviceGroupRelationsResult::getCode()const
{
	return code_;
}

bool BatchAddDeviceGroupRelationsResult::getSuccess()const
{
	return success_;
}

