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

#include <alibabacloud/linkface/model/QueryFaceResult.h>
#include <json/json.h>

using namespace AlibabaCloud::LinkFace;
using namespace AlibabaCloud::LinkFace::Model;

QueryFaceResult::QueryFaceResult() :
	ServiceResult()
{}

QueryFaceResult::QueryFaceResult(const std::string &payload) :
	ServiceResult()
{
	parse(payload);
}

QueryFaceResult::~QueryFaceResult()
{}

void QueryFaceResult::parse(const std::string &payload)
{
	Json::Reader reader;
	Json::Value value;
	reader.parse(payload, value);
	setRequestId(value["RequestId"].asString());
	auto dataNode = value["Data"];
	auto allUserFaceMetasNode = dataNode["UserFaceMetas"]["UserFaceMetasItem"];
	for (auto dataNodeUserFaceMetasUserFaceMetasItem : allUserFaceMetasNode)
	{
		Data::UserFaceMetasItem userFaceMetasItemObject;
		if(!dataNodeUserFaceMetasUserFaceMetasItem["ClientTag"].isNull())
			userFaceMetasItemObject.clientTag = dataNodeUserFaceMetasUserFaceMetasItem["ClientTag"].asString();
		if(!dataNodeUserFaceMetasUserFaceMetasItem["Index"].isNull())
			userFaceMetasItemObject.index = std::stoi(dataNodeUserFaceMetasUserFaceMetasItem["Index"].asString());
		if(!dataNodeUserFaceMetasUserFaceMetasItem["FaceUrl"].isNull())
			userFaceMetasItemObject.faceUrl = dataNodeUserFaceMetasUserFaceMetasItem["FaceUrl"].asString();
		if(!dataNodeUserFaceMetasUserFaceMetasItem["UserInfo"].isNull())
			userFaceMetasItemObject.userInfo = dataNodeUserFaceMetasUserFaceMetasItem["UserInfo"].asString();
		data_.userFaceMetas.push_back(userFaceMetasItemObject);
	}
		auto allGroupIds = dataNode["GroupIds"]["GroupIds"];
		for (auto value : allGroupIds)
			data_.groupIds.push_back(value.asString());
	if(!value["Code"].isNull())
		code_ = std::stoi(value["Code"].asString());
	if(!value["Message"].isNull())
		message_ = value["Message"].asString();
	if(!value["Success"].isNull())
		success_ = value["Success"].asString() == "true";

}

std::string QueryFaceResult::getMessage()const
{
	return message_;
}

QueryFaceResult::Data QueryFaceResult::getData()const
{
	return data_;
}

int QueryFaceResult::getCode()const
{
	return code_;
}

bool QueryFaceResult::getSuccess()const
{
	return success_;
}

