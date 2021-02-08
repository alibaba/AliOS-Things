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

#include <alibabacloud/rtc/model/DescribeMPULayoutInfoListRequest.h>

using AlibabaCloud::Rtc::Model::DescribeMPULayoutInfoListRequest;

DescribeMPULayoutInfoListRequest::DescribeMPULayoutInfoListRequest() :
	RpcServiceRequest("rtc", "2018-01-11", "DescribeMPULayoutInfoList")
{
	setMethod(HttpRequest::Method::Post);
}

DescribeMPULayoutInfoListRequest::~DescribeMPULayoutInfoListRequest()
{}

long DescribeMPULayoutInfoListRequest::getPageNum()const
{
	return pageNum_;
}

void DescribeMPULayoutInfoListRequest::setPageNum(long pageNum)
{
	pageNum_ = pageNum;
	setParameter("PageNum", std::to_string(pageNum));
}

long DescribeMPULayoutInfoListRequest::getLayoutId()const
{
	return layoutId_;
}

void DescribeMPULayoutInfoListRequest::setLayoutId(long layoutId)
{
	layoutId_ = layoutId;
	setParameter("LayoutId", std::to_string(layoutId));
}

long DescribeMPULayoutInfoListRequest::getPageSize()const
{
	return pageSize_;
}

void DescribeMPULayoutInfoListRequest::setPageSize(long pageSize)
{
	pageSize_ = pageSize;
	setParameter("PageSize", std::to_string(pageSize));
}

std::string DescribeMPULayoutInfoListRequest::getShowLog()const
{
	return showLog_;
}

void DescribeMPULayoutInfoListRequest::setShowLog(const std::string& showLog)
{
	showLog_ = showLog;
	setParameter("ShowLog", showLog);
}

long DescribeMPULayoutInfoListRequest::getOwnerId()const
{
	return ownerId_;
}

void DescribeMPULayoutInfoListRequest::setOwnerId(long ownerId)
{
	ownerId_ = ownerId;
	setParameter("OwnerId", std::to_string(ownerId));
}

std::string DescribeMPULayoutInfoListRequest::getAppId()const
{
	return appId_;
}

void DescribeMPULayoutInfoListRequest::setAppId(const std::string& appId)
{
	appId_ = appId;
	setParameter("AppId", appId);
}

std::string DescribeMPULayoutInfoListRequest::getName()const
{
	return name_;
}

void DescribeMPULayoutInfoListRequest::setName(const std::string& name)
{
	name_ = name;
	setParameter("Name", name);
}

