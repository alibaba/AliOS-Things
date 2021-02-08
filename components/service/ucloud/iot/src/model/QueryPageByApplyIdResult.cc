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

#include <alibabacloud/iot/model/QueryPageByApplyIdResult.h>
#include <json/json.h>

using namespace AlibabaCloud::Iot;
using namespace AlibabaCloud::Iot::Model;

QueryPageByApplyIdResult::QueryPageByApplyIdResult() :
	ServiceResult()
{}

QueryPageByApplyIdResult::QueryPageByApplyIdResult(const std::string &payload) :
	ServiceResult()
{
	parse(payload);
}

QueryPageByApplyIdResult::~QueryPageByApplyIdResult()
{}

void QueryPageByApplyIdResult::parse(const std::string &payload)
{
	Json::Reader reader;
	Json::Value value;
	reader.parse(payload, value);
	setRequestId(value["RequestId"].asString());
	auto allApplyDeviceListNode = value["ApplyDeviceList"]["ApplyDeviceInfo"];
	for (auto valueApplyDeviceListApplyDeviceInfo : allApplyDeviceListNode)
	{
		ApplyDeviceInfo applyDeviceListObject;
		if(!valueApplyDeviceListApplyDeviceInfo["DeviceId"].isNull())
			applyDeviceListObject.deviceId = valueApplyDeviceListApplyDeviceInfo["DeviceId"].asString();
		if(!valueApplyDeviceListApplyDeviceInfo["DeviceName"].isNull())
			applyDeviceListObject.deviceName = valueApplyDeviceListApplyDeviceInfo["DeviceName"].asString();
		if(!valueApplyDeviceListApplyDeviceInfo["DeviceSecret"].isNull())
			applyDeviceListObject.deviceSecret = valueApplyDeviceListApplyDeviceInfo["DeviceSecret"].asString();
		if(!valueApplyDeviceListApplyDeviceInfo["IotId"].isNull())
			applyDeviceListObject.iotId = valueApplyDeviceListApplyDeviceInfo["IotId"].asString();
		applyDeviceList_.push_back(applyDeviceListObject);
	}
	if(!value["Success"].isNull())
		success_ = value["Success"].asString() == "true";
	if(!value["Code"].isNull())
		code_ = value["Code"].asString();
	if(!value["ErrorMessage"].isNull())
		errorMessage_ = value["ErrorMessage"].asString();
	if(!value["PageSize"].isNull())
		pageSize_ = std::stoi(value["PageSize"].asString());
	if(!value["Page"].isNull())
		page_ = std::stoi(value["Page"].asString());
	if(!value["PageCount"].isNull())
		pageCount_ = std::stoi(value["PageCount"].asString());
	if(!value["Total"].isNull())
		total_ = std::stoi(value["Total"].asString());

}

int QueryPageByApplyIdResult::getPageSize()const
{
	return pageSize_;
}

int QueryPageByApplyIdResult::getPageCount()const
{
	return pageCount_;
}

int QueryPageByApplyIdResult::getTotal()const
{
	return total_;
}

int QueryPageByApplyIdResult::getPage()const
{
	return page_;
}

std::vector<QueryPageByApplyIdResult::ApplyDeviceInfo> QueryPageByApplyIdResult::getApplyDeviceList()const
{
	return applyDeviceList_;
}

std::string QueryPageByApplyIdResult::getErrorMessage()const
{
	return errorMessage_;
}

std::string QueryPageByApplyIdResult::getCode()const
{
	return code_;
}

bool QueryPageByApplyIdResult::getSuccess()const
{
	return success_;
}

