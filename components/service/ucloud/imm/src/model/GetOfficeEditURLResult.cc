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

#include <alibabacloud/imm/model/GetOfficeEditURLResult.h>
#include <json/json.h>

using namespace AlibabaCloud::Imm;
using namespace AlibabaCloud::Imm::Model;

GetOfficeEditURLResult::GetOfficeEditURLResult() :
	ServiceResult()
{}

GetOfficeEditURLResult::GetOfficeEditURLResult(const std::string &payload) :
	ServiceResult()
{
	parse(payload);
}

GetOfficeEditURLResult::~GetOfficeEditURLResult()
{}

void GetOfficeEditURLResult::parse(const std::string &payload)
{
	Json::Reader reader;
	Json::Value value;
	reader.parse(payload, value);
	setRequestId(value["RequestId"].asString());
	if(!value["EditURL"].isNull())
		editURL_ = value["EditURL"].asString();
	if(!value["AccessToken"].isNull())
		accessToken_ = value["AccessToken"].asString();
	if(!value["RefreshToken"].isNull())
		refreshToken_ = value["RefreshToken"].asString();
	if(!value["AccessTokenExpiredTime"].isNull())
		accessTokenExpiredTime_ = value["AccessTokenExpiredTime"].asString();
	if(!value["RefreshTokenExpiredTime"].isNull())
		refreshTokenExpiredTime_ = value["RefreshTokenExpiredTime"].asString();

}

std::string GetOfficeEditURLResult::getRefreshToken()const
{
	return refreshToken_;
}

std::string GetOfficeEditURLResult::getEditURL()const
{
	return editURL_;
}

std::string GetOfficeEditURLResult::getAccessToken()const
{
	return accessToken_;
}

std::string GetOfficeEditURLResult::getRefreshTokenExpiredTime()const
{
	return refreshTokenExpiredTime_;
}

std::string GetOfficeEditURLResult::getAccessTokenExpiredTime()const
{
	return accessTokenExpiredTime_;
}

