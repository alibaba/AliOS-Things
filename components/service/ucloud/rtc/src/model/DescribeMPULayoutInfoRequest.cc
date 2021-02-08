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

#include <alibabacloud/rtc/model/DescribeMPULayoutInfoRequest.h>

using AlibabaCloud::Rtc::Model::DescribeMPULayoutInfoRequest;

DescribeMPULayoutInfoRequest::DescribeMPULayoutInfoRequest() :
	RpcServiceRequest("rtc", "2018-01-11", "DescribeMPULayoutInfo")
{
	setMethod(HttpRequest::Method::Post);
}

DescribeMPULayoutInfoRequest::~DescribeMPULayoutInfoRequest()
{}

long DescribeMPULayoutInfoRequest::getLayoutId()const
{
	return layoutId_;
}

void DescribeMPULayoutInfoRequest::setLayoutId(long layoutId)
{
	layoutId_ = layoutId;
	setParameter("LayoutId", std::to_string(layoutId));
}

std::string DescribeMPULayoutInfoRequest::getShowLog()const
{
	return showLog_;
}

void DescribeMPULayoutInfoRequest::setShowLog(const std::string& showLog)
{
	showLog_ = showLog;
	setParameter("ShowLog", showLog);
}

long DescribeMPULayoutInfoRequest::getOwnerId()const
{
	return ownerId_;
}

void DescribeMPULayoutInfoRequest::setOwnerId(long ownerId)
{
	ownerId_ = ownerId;
	setParameter("OwnerId", std::to_string(ownerId));
}

std::string DescribeMPULayoutInfoRequest::getAppId()const
{
	return appId_;
}

void DescribeMPULayoutInfoRequest::setAppId(const std::string& appId)
{
	appId_ = appId;
	setParameter("AppId", appId);
}

