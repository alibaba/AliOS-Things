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

#include <alibabacloud/aliyuncvc/model/ListUsersResult.h>
#include <json/json.h>

using namespace AlibabaCloud::Aliyuncvc;
using namespace AlibabaCloud::Aliyuncvc::Model;

ListUsersResult::ListUsersResult() :
	ServiceResult()
{}

ListUsersResult::ListUsersResult(const std::string &payload) :
	ServiceResult()
{
	parse(payload);
}

ListUsersResult::~ListUsersResult()
{}

void ListUsersResult::parse(const std::string &payload)
{
	Json::Reader reader;
	Json::Value value;
	reader.parse(payload, value);
	setRequestId(value["RequestId"].asString());
	auto dataNode = value["Data"];
	if(!dataNode["TotalCount"].isNull())
		data_.totalCount = std::stoi(dataNode["TotalCount"].asString());
	if(!dataNode["PageSize"].isNull())
		data_.pageSize = std::stoi(dataNode["PageSize"].asString());
	if(!dataNode["PageNumber"].isNull())
		data_.pageNumber = std::stoi(dataNode["PageNumber"].asString());
	auto allUserInfosNode = dataNode["UserInfos"]["UserInfo"];
	for (auto dataNodeUserInfosUserInfo : allUserInfosNode)
	{
		Data::UserInfo userInfoObject;
		if(!dataNodeUserInfosUserInfo["CreateTime"].isNull())
			userInfoObject.createTime = std::stol(dataNodeUserInfosUserInfo["CreateTime"].asString());
		if(!dataNodeUserInfosUserInfo["GroupName"].isNull())
			userInfoObject.groupName = dataNodeUserInfosUserInfo["GroupName"].asString();
		if(!dataNodeUserInfosUserInfo["UserName"].isNull())
			userInfoObject.userName = dataNodeUserInfosUserInfo["UserName"].asString();
		if(!dataNodeUserInfosUserInfo["GroupId"].isNull())
			userInfoObject.groupId = dataNodeUserInfosUserInfo["GroupId"].asString();
		if(!dataNodeUserInfosUserInfo["DepartName"].isNull())
			userInfoObject.departName = dataNodeUserInfosUserInfo["DepartName"].asString();
		if(!dataNodeUserInfosUserInfo["DepartId"].isNull())
			userInfoObject.departId = dataNodeUserInfosUserInfo["DepartId"].asString();
		if(!dataNodeUserInfosUserInfo["UserEmail"].isNull())
			userInfoObject.userEmail = dataNodeUserInfosUserInfo["UserEmail"].asString();
		if(!dataNodeUserInfosUserInfo["UserTel"].isNull())
			userInfoObject.userTel = dataNodeUserInfosUserInfo["UserTel"].asString();
		if(!dataNodeUserInfosUserInfo["UserMobile"].isNull())
			userInfoObject.userMobile = dataNodeUserInfosUserInfo["UserMobile"].asString();
		if(!dataNodeUserInfosUserInfo["UserAvatarUrl"].isNull())
			userInfoObject.userAvatarUrl = dataNodeUserInfosUserInfo["UserAvatarUrl"].asString();
		if(!dataNodeUserInfosUserInfo["JobName"].isNull())
			userInfoObject.jobName = dataNodeUserInfosUserInfo["JobName"].asString();
		if(!dataNodeUserInfosUserInfo["UserId"].isNull())
			userInfoObject.userId = dataNodeUserInfosUserInfo["UserId"].asString();
		data_.userInfos.push_back(userInfoObject);
	}
	if(!value["ErrorCode"].isNull())
		errorCode_ = std::stoi(value["ErrorCode"].asString());
	if(!value["Message"].isNull())
		message_ = value["Message"].asString();
	if(!value["Success"].isNull())
		success_ = value["Success"].asString() == "true";

}

std::string ListUsersResult::getMessage()const
{
	return message_;
}

ListUsersResult::Data ListUsersResult::getData()const
{
	return data_;
}

int ListUsersResult::getErrorCode()const
{
	return errorCode_;
}

bool ListUsersResult::getSuccess()const
{
	return success_;
}

