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

#include <alibabacloud/aliyuncvc/model/ListDevicesResult.h>
#include <json/json.h>

using namespace AlibabaCloud::Aliyuncvc;
using namespace AlibabaCloud::Aliyuncvc::Model;

ListDevicesResult::ListDevicesResult() :
	ServiceResult()
{}

ListDevicesResult::ListDevicesResult(const std::string &payload) :
	ServiceResult()
{
	parse(payload);
}

ListDevicesResult::~ListDevicesResult()
{}

void ListDevicesResult::parse(const std::string &payload)
{
	Json::Reader reader;
	Json::Value value;
	reader.parse(payload, value);
	setRequestId(value["RequestId"].asString());
	auto dataNode = value["Data"];
	if(!dataNode["Total"].isNull())
		data_.total = std::stoi(dataNode["Total"].asString());
	if(!dataNode["PageSize"].isNull())
		data_.pageSize = std::stoi(dataNode["PageSize"].asString());
	if(!dataNode["PageNumber"].isNull())
		data_.pageNumber = std::stoi(dataNode["PageNumber"].asString());
	auto allDevicesNode = dataNode["Devices"]["Device"];
	for (auto dataNodeDevicesDevice : allDevicesNode)
	{
		Data::Device deviceObject;
		if(!dataNodeDevicesDevice["ActivationCode"].isNull())
			deviceObject.activationCode = dataNodeDevicesDevice["ActivationCode"].asString();
		if(!dataNodeDevicesDevice["ConferenceCode"].isNull())
			deviceObject.conferenceCode = dataNodeDevicesDevice["ConferenceCode"].asString();
		if(!dataNodeDevicesDevice["ConferenceName"].isNull())
			deviceObject.conferenceName = dataNodeDevicesDevice["ConferenceName"].asString();
		if(!dataNodeDevicesDevice["CreateTime"].isNull())
			deviceObject.createTime = dataNodeDevicesDevice["CreateTime"].asString();
		if(!dataNodeDevicesDevice["PictureUrl"].isNull())
			deviceObject.pictureUrl = dataNodeDevicesDevice["PictureUrl"].asString();
		if(!dataNodeDevicesDevice["SN"].isNull())
			deviceObject.sN = dataNodeDevicesDevice["SN"].asString();
		if(!dataNodeDevicesDevice["Status"].isNull())
			deviceObject.status = dataNodeDevicesDevice["Status"].asString();
		if(!dataNodeDevicesDevice["CastScreenCode"].isNull())
			deviceObject.castScreenCode = dataNodeDevicesDevice["CastScreenCode"].asString();
		data_.devices.push_back(deviceObject);
	}
	if(!value["ErrorCode"].isNull())
		errorCode_ = std::stoi(value["ErrorCode"].asString());
	if(!value["Message"].isNull())
		message_ = value["Message"].asString();
	if(!value["Success"].isNull())
		success_ = value["Success"].asString() == "true";

}

std::string ListDevicesResult::getMessage()const
{
	return message_;
}

ListDevicesResult::Data ListDevicesResult::getData()const
{
	return data_;
}

int ListDevicesResult::getErrorCode()const
{
	return errorCode_;
}

bool ListDevicesResult::getSuccess()const
{
	return success_;
}

