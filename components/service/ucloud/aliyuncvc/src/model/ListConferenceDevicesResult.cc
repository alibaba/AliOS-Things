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

#include <alibabacloud/aliyuncvc/model/ListConferenceDevicesResult.h>
#include <json/json.h>

using namespace AlibabaCloud::Aliyuncvc;
using namespace AlibabaCloud::Aliyuncvc::Model;

ListConferenceDevicesResult::ListConferenceDevicesResult() :
	ServiceResult()
{}

ListConferenceDevicesResult::ListConferenceDevicesResult(const std::string &payload) :
	ServiceResult()
{
	parse(payload);
}

ListConferenceDevicesResult::~ListConferenceDevicesResult()
{}

void ListConferenceDevicesResult::parse(const std::string &payload)
{
	Json::Reader reader;
	Json::Value value;
	reader.parse(payload, value);
	setRequestId(value["RequestId"].asString());
	auto conferencesDatasNode = value["ConferencesDatas"];
	if(!conferencesDatasNode["Total"].isNull())
		conferencesDatas_.total = std::stoi(conferencesDatasNode["Total"].asString());
	if(!conferencesDatasNode["PageSize"].isNull())
		conferencesDatas_.pageSize = std::stoi(conferencesDatasNode["PageSize"].asString());
	if(!conferencesDatasNode["PageNumber"].isNull())
		conferencesDatas_.pageNumber = std::stoi(conferencesDatasNode["PageNumber"].asString());
	auto allConferencesNode = conferencesDatasNode["Conferences"]["data"];
	for (auto conferencesDatasNodeConferencesdata : allConferencesNode)
	{
		ConferencesDatas::Data dataObject;
		if(!conferencesDatasNodeConferencesdata["ActivationCode"].isNull())
			dataObject.activationCode = conferencesDatasNodeConferencesdata["ActivationCode"].asString();
		if(!conferencesDatasNodeConferencesdata["ConferenceCode"].isNull())
			dataObject.conferenceCode = conferencesDatasNodeConferencesdata["ConferenceCode"].asString();
		if(!conferencesDatasNodeConferencesdata["ConferenceName"].isNull())
			dataObject.conferenceName = conferencesDatasNodeConferencesdata["ConferenceName"].asString();
		if(!conferencesDatasNodeConferencesdata["CreateTime"].isNull())
			dataObject.createTime = conferencesDatasNodeConferencesdata["CreateTime"].asString();
		if(!conferencesDatasNodeConferencesdata["DeviceModel"].isNull())
			dataObject.deviceModel = conferencesDatasNodeConferencesdata["DeviceModel"].asString();
		if(!conferencesDatasNodeConferencesdata["Manufacturer"].isNull())
			dataObject.manufacturer = conferencesDatasNodeConferencesdata["Manufacturer"].asString();
		if(!conferencesDatasNodeConferencesdata["PictureUrl"].isNull())
			dataObject.pictureUrl = conferencesDatasNodeConferencesdata["PictureUrl"].asString();
		if(!conferencesDatasNodeConferencesdata["SN"].isNull())
			dataObject.sN = conferencesDatasNodeConferencesdata["SN"].asString();
		if(!conferencesDatasNodeConferencesdata["Status"].isNull())
			dataObject.status = conferencesDatasNodeConferencesdata["Status"].asString();
		if(!conferencesDatasNodeConferencesdata["CastScreenCode"].isNull())
			dataObject.castScreenCode = conferencesDatasNodeConferencesdata["CastScreenCode"].asString();
		conferencesDatas_.conferences.push_back(dataObject);
	}
	if(!value["ErrorCode"].isNull())
		errorCode_ = std::stoi(value["ErrorCode"].asString());
	if(!value["Message"].isNull())
		message_ = value["Message"].asString();
	if(!value["Success"].isNull())
		success_ = value["Success"].asString() == "true";

}

ListConferenceDevicesResult::ConferencesDatas ListConferenceDevicesResult::getConferencesDatas()const
{
	return conferencesDatas_;
}

std::string ListConferenceDevicesResult::getMessage()const
{
	return message_;
}

int ListConferenceDevicesResult::getErrorCode()const
{
	return errorCode_;
}

bool ListConferenceDevicesResult::getSuccess()const
{
	return success_;
}

