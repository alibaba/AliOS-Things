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

#include <alibabacloud/live/model/UpdateBoardCallbackRequest.h>

using AlibabaCloud::Live::Model::UpdateBoardCallbackRequest;

UpdateBoardCallbackRequest::UpdateBoardCallbackRequest() :
	RpcServiceRequest("live", "2016-11-01", "UpdateBoardCallback")
{
	setMethod(HttpRequest::Method::Post);
}

UpdateBoardCallbackRequest::~UpdateBoardCallbackRequest()
{}

std::string UpdateBoardCallbackRequest::getAuthKey()const
{
	return authKey_;
}

void UpdateBoardCallbackRequest::setAuthKey(const std::string& authKey)
{
	authKey_ = authKey;
	setParameter("AuthKey", authKey);
}

int UpdateBoardCallbackRequest::getCallbackEnable()const
{
	return callbackEnable_;
}

void UpdateBoardCallbackRequest::setCallbackEnable(int callbackEnable)
{
	callbackEnable_ = callbackEnable;
	setParameter("CallbackEnable", std::to_string(callbackEnable));
}

std::string UpdateBoardCallbackRequest::getCallbackEvents()const
{
	return callbackEvents_;
}

void UpdateBoardCallbackRequest::setCallbackEvents(const std::string& callbackEvents)
{
	callbackEvents_ = callbackEvents;
	setParameter("CallbackEvents", callbackEvents);
}

long UpdateBoardCallbackRequest::getOwnerId()const
{
	return ownerId_;
}

void UpdateBoardCallbackRequest::setOwnerId(long ownerId)
{
	ownerId_ = ownerId;
	setParameter("OwnerId", std::to_string(ownerId));
}

std::string UpdateBoardCallbackRequest::getCallbackUri()const
{
	return callbackUri_;
}

void UpdateBoardCallbackRequest::setCallbackUri(const std::string& callbackUri)
{
	callbackUri_ = callbackUri;
	setParameter("CallbackUri", callbackUri);
}

std::string UpdateBoardCallbackRequest::getAppId()const
{
	return appId_;
}

void UpdateBoardCallbackRequest::setAppId(const std::string& appId)
{
	appId_ = appId;
	setParameter("AppId", appId);
}

std::string UpdateBoardCallbackRequest::getAuthSwitch()const
{
	return authSwitch_;
}

void UpdateBoardCallbackRequest::setAuthSwitch(const std::string& authSwitch)
{
	authSwitch_ = authSwitch;
	setParameter("AuthSwitch", authSwitch);
}

