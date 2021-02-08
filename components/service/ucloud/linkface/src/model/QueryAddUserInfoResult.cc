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

#include <alibabacloud/linkface/model/QueryAddUserInfoResult.h>
#include <json/json.h>

using namespace AlibabaCloud::LinkFace;
using namespace AlibabaCloud::LinkFace::Model;

QueryAddUserInfoResult::QueryAddUserInfoResult() :
	ServiceResult()
{}

QueryAddUserInfoResult::QueryAddUserInfoResult(const std::string &payload) :
	ServiceResult()
{
	parse(payload);
}

QueryAddUserInfoResult::~QueryAddUserInfoResult()
{}

void QueryAddUserInfoResult::parse(const std::string &payload)
{
	Json::Reader reader;
	Json::Value value;
	reader.parse(payload, value);
	setRequestId(value["RequestId"].asString());
	auto dataNode = value["Data"];
	auto allCurrentFaceInfosNode = dataNode["CurrentFaceInfos"]["CurrentFaceInfosItem"];
	for (auto dataNodeCurrentFaceInfosCurrentFaceInfosItem : allCurrentFaceInfosNode)
	{
		Data::CurrentFaceInfosItem currentFaceInfosItemObject;
		if(!dataNodeCurrentFaceInfosCurrentFaceInfosItem["UserId"].isNull())
			currentFaceInfosItemObject.userId = dataNodeCurrentFaceInfosCurrentFaceInfosItem["UserId"].asString();
		if(!dataNodeCurrentFaceInfosCurrentFaceInfosItem["ClientTag"].isNull())
			currentFaceInfosItemObject.clientTag = dataNodeCurrentFaceInfosCurrentFaceInfosItem["ClientTag"].asString();
		if(!dataNodeCurrentFaceInfosCurrentFaceInfosItem["Index"].isNull())
			currentFaceInfosItemObject.index = std::stoi(dataNodeCurrentFaceInfosCurrentFaceInfosItem["Index"].asString());
		data_.currentFaceInfos.push_back(currentFaceInfosItemObject);
	}
	auto allFailedFaceInfosNode = dataNode["FailedFaceInfos"]["FailedFaceInfosItem"];
	for (auto dataNodeFailedFaceInfosFailedFaceInfosItem : allFailedFaceInfosNode)
	{
		Data::FailedFaceInfosItem failedFaceInfosItemObject;
		if(!dataNodeFailedFaceInfosFailedFaceInfosItem["UserId"].isNull())
			failedFaceInfosItemObject.userId = dataNodeFailedFaceInfosFailedFaceInfosItem["UserId"].asString();
		if(!dataNodeFailedFaceInfosFailedFaceInfosItem["ClientTag"].isNull())
			failedFaceInfosItemObject.clientTag = dataNodeFailedFaceInfosFailedFaceInfosItem["ClientTag"].asString();
		if(!dataNodeFailedFaceInfosFailedFaceInfosItem["Index"].isNull())
			failedFaceInfosItemObject.index = std::stoi(dataNodeFailedFaceInfosFailedFaceInfosItem["Index"].asString());
		data_.failedFaceInfos.push_back(failedFaceInfosItemObject);
	}
	if(!value["Code"].isNull())
		code_ = std::stoi(value["Code"].asString());
	if(!value["Message"].isNull())
		message_ = value["Message"].asString();
	if(!value["Success"].isNull())
		success_ = value["Success"].asString() == "true";

}

std::string QueryAddUserInfoResult::getMessage()const
{
	return message_;
}

QueryAddUserInfoResult::Data QueryAddUserInfoResult::getData()const
{
	return data_;
}

int QueryAddUserInfoResult::getCode()const
{
	return code_;
}

bool QueryAddUserInfoResult::getSuccess()const
{
	return success_;
}

