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

#include <alibabacloud/iot/model/QuerySubscribeRelationResult.h>
#include <json/json.h>

using namespace AlibabaCloud::Iot;
using namespace AlibabaCloud::Iot::Model;

QuerySubscribeRelationResult::QuerySubscribeRelationResult() :
	ServiceResult()
{}

QuerySubscribeRelationResult::QuerySubscribeRelationResult(const std::string &payload) :
	ServiceResult()
{
	parse(payload);
}

QuerySubscribeRelationResult::~QuerySubscribeRelationResult()
{}

void QuerySubscribeRelationResult::parse(const std::string &payload)
{
	Json::Reader reader;
	Json::Value value;
	reader.parse(payload, value);
	setRequestId(value["RequestId"].asString());
	auto allConsumerGroupIds = value["ConsumerGroupIds"]["consumerGroupId"];
	for (const auto &item : allConsumerGroupIds)
		consumerGroupIds_.push_back(item.asString());
	if(!value["Success"].isNull())
		success_ = value["Success"].asString() == "true";
	if(!value["Code"].isNull())
		code_ = value["Code"].asString();
	if(!value["ErrorMessage"].isNull())
		errorMessage_ = value["ErrorMessage"].asString();
	if(!value["ProductKey"].isNull())
		productKey_ = value["ProductKey"].asString();
	if(!value["Type"].isNull())
		type_ = value["Type"].asString();
	if(!value["DeviceDataFlag"].isNull())
		deviceDataFlag_ = value["DeviceDataFlag"].asString() == "true";
	if(!value["DeviceLifeCycleFlag"].isNull())
		deviceLifeCycleFlag_ = value["DeviceLifeCycleFlag"].asString() == "true";
	if(!value["DeviceStatusChangeFlag"].isNull())
		deviceStatusChangeFlag_ = value["DeviceStatusChangeFlag"].asString() == "true";
	if(!value["DeviceTopoLifeCycleFlag"].isNull())
		deviceTopoLifeCycleFlag_ = value["DeviceTopoLifeCycleFlag"].asString() == "true";
	if(!value["FoundDeviceListFlag"].isNull())
		foundDeviceListFlag_ = value["FoundDeviceListFlag"].asString() == "true";
	if(!value["OtaEventFlag"].isNull())
		otaEventFlag_ = value["OtaEventFlag"].asString() == "true";
	if(!value["ThingHistoryFlag"].isNull())
		thingHistoryFlag_ = value["ThingHistoryFlag"].asString() == "true";
	if(!value["MnsConfiguration"].isNull())
		mnsConfiguration_ = value["MnsConfiguration"].asString();

}

bool QuerySubscribeRelationResult::getDeviceLifeCycleFlag()const
{
	return deviceLifeCycleFlag_;
}

bool QuerySubscribeRelationResult::getDeviceDataFlag()const
{
	return deviceDataFlag_;
}

bool QuerySubscribeRelationResult::getDeviceTopoLifeCycleFlag()const
{
	return deviceTopoLifeCycleFlag_;
}

bool QuerySubscribeRelationResult::getDeviceStatusChangeFlag()const
{
	return deviceStatusChangeFlag_;
}

std::vector<std::string> QuerySubscribeRelationResult::getConsumerGroupIds()const
{
	return consumerGroupIds_;
}

std::string QuerySubscribeRelationResult::getCode()const
{
	return code_;
}

bool QuerySubscribeRelationResult::getSuccess()const
{
	return success_;
}

bool QuerySubscribeRelationResult::getThingHistoryFlag()const
{
	return thingHistoryFlag_;
}

std::string QuerySubscribeRelationResult::getType()const
{
	return type_;
}

bool QuerySubscribeRelationResult::getFoundDeviceListFlag()const
{
	return foundDeviceListFlag_;
}

bool QuerySubscribeRelationResult::getOtaEventFlag()const
{
	return otaEventFlag_;
}

std::string QuerySubscribeRelationResult::getErrorMessage()const
{
	return errorMessage_;
}

std::string QuerySubscribeRelationResult::getProductKey()const
{
	return productKey_;
}

std::string QuerySubscribeRelationResult::getMnsConfiguration()const
{
	return mnsConfiguration_;
}

