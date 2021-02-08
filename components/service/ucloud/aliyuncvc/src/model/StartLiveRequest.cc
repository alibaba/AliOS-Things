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

#include <alibabacloud/aliyuncvc/model/StartLiveRequest.h>

using AlibabaCloud::Aliyuncvc::Model::StartLiveRequest;

StartLiveRequest::StartLiveRequest() :
	RpcServiceRequest("aliyuncvc", "2019-10-30", "StartLive")
{
	setMethod(HttpRequest::Method::Post);
}

StartLiveRequest::~StartLiveRequest()
{}

std::string StartLiveRequest::getLiveUUID()const
{
	return liveUUID_;
}

void StartLiveRequest::setLiveUUID(const std::string& liveUUID)
{
	liveUUID_ = liveUUID;
	setBodyParameter("LiveUUID", liveUUID);
}

std::string StartLiveRequest::getPushInfo()const
{
	return pushInfo_;
}

void StartLiveRequest::setPushInfo(const std::string& pushInfo)
{
	pushInfo_ = pushInfo;
	setBodyParameter("PushInfo", pushInfo);
}

std::string StartLiveRequest::getUserId()const
{
	return userId_;
}

void StartLiveRequest::setUserId(const std::string& userId)
{
	userId_ = userId;
	setBodyParameter("UserId", userId);
}

std::string StartLiveRequest::getLayoutInfo()const
{
	return layoutInfo_;
}

void StartLiveRequest::setLayoutInfo(const std::string& layoutInfo)
{
	layoutInfo_ = layoutInfo;
	setBodyParameter("LayoutInfo", layoutInfo);
}

