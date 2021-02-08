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

#include <alibabacloud/rtc/model/DescribeMPULayoutListRequest.h>

using AlibabaCloud::Rtc::Model::DescribeMPULayoutListRequest;

DescribeMPULayoutListRequest::DescribeMPULayoutListRequest() :
	RpcServiceRequest("rtc", "2018-01-11", "DescribeMPULayoutList")
{
	setMethod(HttpRequest::Method::Post);
}

DescribeMPULayoutListRequest::~DescribeMPULayoutListRequest()
{}

std::string DescribeMPULayoutListRequest::getShowLog()const
{
	return showLog_;
}

void DescribeMPULayoutListRequest::setShowLog(const std::string& showLog)
{
	showLog_ = showLog;
	setParameter("ShowLog", showLog);
}

long DescribeMPULayoutListRequest::getOwnerId()const
{
	return ownerId_;
}

void DescribeMPULayoutListRequest::setOwnerId(long ownerId)
{
	ownerId_ = ownerId;
	setParameter("OwnerId", std::to_string(ownerId));
}

std::string DescribeMPULayoutListRequest::getAppId()const
{
	return appId_;
}

void DescribeMPULayoutListRequest::setAppId(const std::string& appId)
{
	appId_ = appId;
	setParameter("AppId", appId);
}

