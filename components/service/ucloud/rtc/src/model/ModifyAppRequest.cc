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

#include <alibabacloud/rtc/model/ModifyAppRequest.h>

using AlibabaCloud::Rtc::Model::ModifyAppRequest;

ModifyAppRequest::ModifyAppRequest() :
	RpcServiceRequest("rtc", "2018-01-11", "ModifyApp")
{
	setMethod(HttpRequest::Method::Post);
}

ModifyAppRequest::~ModifyAppRequest()
{}

std::string ModifyAppRequest::getAppName()const
{
	return appName_;
}

void ModifyAppRequest::setAppName(const std::string& appName)
{
	appName_ = appName;
	setParameter("AppName", appName);
}

std::string ModifyAppRequest::getShowLog()const
{
	return showLog_;
}

void ModifyAppRequest::setShowLog(const std::string& showLog)
{
	showLog_ = showLog;
	setParameter("ShowLog", showLog);
}

long ModifyAppRequest::getOwnerId()const
{
	return ownerId_;
}

void ModifyAppRequest::setOwnerId(long ownerId)
{
	ownerId_ = ownerId;
	setParameter("OwnerId", std::to_string(ownerId));
}

std::string ModifyAppRequest::getAppId()const
{
	return appId_;
}

void ModifyAppRequest::setAppId(const std::string& appId)
{
	appId_ = appId;
	setParameter("AppId", appId);
}

