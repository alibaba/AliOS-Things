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

#include <alibabacloud/rtc/model/DeleteMPULayoutRequest.h>

using AlibabaCloud::Rtc::Model::DeleteMPULayoutRequest;

DeleteMPULayoutRequest::DeleteMPULayoutRequest() :
	RpcServiceRequest("rtc", "2018-01-11", "DeleteMPULayout")
{
	setMethod(HttpRequest::Method::Post);
}

DeleteMPULayoutRequest::~DeleteMPULayoutRequest()
{}

long DeleteMPULayoutRequest::getLayoutId()const
{
	return layoutId_;
}

void DeleteMPULayoutRequest::setLayoutId(long layoutId)
{
	layoutId_ = layoutId;
	setParameter("LayoutId", std::to_string(layoutId));
}

std::string DeleteMPULayoutRequest::getShowLog()const
{
	return showLog_;
}

void DeleteMPULayoutRequest::setShowLog(const std::string& showLog)
{
	showLog_ = showLog;
	setParameter("ShowLog", showLog);
}

long DeleteMPULayoutRequest::getOwnerId()const
{
	return ownerId_;
}

void DeleteMPULayoutRequest::setOwnerId(long ownerId)
{
	ownerId_ = ownerId;
	setParameter("OwnerId", std::to_string(ownerId));
}

std::string DeleteMPULayoutRequest::getAppId()const
{
	return appId_;
}

void DeleteMPULayoutRequest::setAppId(const std::string& appId)
{
	appId_ = appId;
	setParameter("AppId", appId);
}

