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

#include <alibabacloud/aliyuncvc/model/GetUserResult.h>
#include <json/json.h>

using namespace AlibabaCloud::Aliyuncvc;
using namespace AlibabaCloud::Aliyuncvc::Model;

GetUserResult::GetUserResult() :
	ServiceResult()
{}

GetUserResult::GetUserResult(const std::string &payload) :
	ServiceResult()
{
	parse(payload);
}

GetUserResult::~GetUserResult()
{}

void GetUserResult::parse(const std::string &payload)
{
	Json::Reader reader;
	Json::Value value;
	reader.parse(payload, value);
	setRequestId(value["RequestId"].asString());
	auto userInfoNode = value["UserInfo"];
	if(!userInfoNode["UserName"].isNull())
		userInfo_.userName = userInfoNode["UserName"].asString();
	if(!userInfoNode["CreateTime"].isNull())
		userInfo_.createTime = std::stol(userInfoNode["CreateTime"].asString());
	if(!userInfoNode["GroupId"].isNull())
		userInfo_.groupId = userInfoNode["GroupId"].asString();
	if(!userInfoNode["GroupName"].isNull())
		userInfo_.groupName = userInfoNode["GroupName"].asString();
	if(!userInfoNode["UserId"].isNull())
		userInfo_.userId = userInfoNode["UserId"].asString();
	if(!userInfoNode["UserTel"].isNull())
		userInfo_.userTel = userInfoNode["UserTel"].asString();
	if(!userInfoNode["UserEmail"].isNull())
		userInfo_.userEmail = userInfoNode["UserEmail"].asString();
	if(!userInfoNode["UserMobile"].isNull())
		userInfo_.userMobile = userInfoNode["UserMobile"].asString();
	if(!userInfoNode["UserAvatarUrl"].isNull())
		userInfo_.userAvatarUrl = userInfoNode["UserAvatarUrl"].asString();
	if(!userInfoNode["DepartId"].isNull())
		userInfo_.departId = userInfoNode["DepartId"].asString();
	if(!userInfoNode["DepartName"].isNull())
		userInfo_.departName = userInfoNode["DepartName"].asString();
	if(!userInfoNode["JobName"].isNull())
		userInfo_.jobName = userInfoNode["JobName"].asString();
	if(!value["ErrorCode"].isNull())
		errorCode_ = std::stoi(value["ErrorCode"].asString());
	if(!value["Message"].isNull())
		message_ = value["Message"].asString();
	if(!value["Success"].isNull())
		success_ = value["Success"].asString() == "true";

}

std::string GetUserResult::getMessage()const
{
	return message_;
}

GetUserResult::UserInfo GetUserResult::getUserInfo()const
{
	return userInfo_;
}

int GetUserResult::getErrorCode()const
{
	return errorCode_;
}

bool GetUserResult::getSuccess()const
{
	return success_;
}

