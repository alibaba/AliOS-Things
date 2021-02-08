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

#include <alibabacloud/imm/model/GetOfficePreviewURLResult.h>
#include <json/json.h>

using namespace AlibabaCloud::Imm;
using namespace AlibabaCloud::Imm::Model;

GetOfficePreviewURLResult::GetOfficePreviewURLResult() :
	ServiceResult()
{}

GetOfficePreviewURLResult::GetOfficePreviewURLResult(const std::string &payload) :
	ServiceResult()
{
	parse(payload);
}

GetOfficePreviewURLResult::~GetOfficePreviewURLResult()
{}

void GetOfficePreviewURLResult::parse(const std::string &payload)
{
	Json::Reader reader;
	Json::Value value;
	reader.parse(payload, value);
	setRequestId(value["RequestId"].asString());
	if(!value["PreviewURL"].isNull())
		previewURL_ = value["PreviewURL"].asString();
	if(!value["AccessToken"].isNull())
		accessToken_ = value["AccessToken"].asString();
	if(!value["RefreshToken"].isNull())
		refreshToken_ = value["RefreshToken"].asString();
	if(!value["AccessTokenExpiredTime"].isNull())
		accessTokenExpiredTime_ = value["AccessTokenExpiredTime"].asString();
	if(!value["RefreshTokenExpiredTime"].isNull())
		refreshTokenExpiredTime_ = value["RefreshTokenExpiredTime"].asString();

}

std::string GetOfficePreviewURLResult::getPreviewURL()const
{
	return previewURL_;
}

std::string GetOfficePreviewURLResult::getRefreshToken()const
{
	return refreshToken_;
}

std::string GetOfficePreviewURLResult::getAccessToken()const
{
	return accessToken_;
}

std::string GetOfficePreviewURLResult::getRefreshTokenExpiredTime()const
{
	return refreshTokenExpiredTime_;
}

std::string GetOfficePreviewURLResult::getAccessTokenExpiredTime()const
{
	return accessTokenExpiredTime_;
}

