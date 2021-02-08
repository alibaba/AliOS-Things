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

#include <alibabacloud/rtc/model/DescribeAppsRequest.h>

using AlibabaCloud::Rtc::Model::DescribeAppsRequest;

DescribeAppsRequest::DescribeAppsRequest() :
	RpcServiceRequest("rtc", "2018-01-11", "DescribeApps")
{
	setMethod(HttpRequest::Method::Post);
}

DescribeAppsRequest::~DescribeAppsRequest()
{}

int DescribeAppsRequest::getPageNum()const
{
	return pageNum_;
}

void DescribeAppsRequest::setPageNum(int pageNum)
{
	pageNum_ = pageNum;
	setParameter("PageNum", std::to_string(pageNum));
}

int DescribeAppsRequest::getPageSize()const
{
	return pageSize_;
}

void DescribeAppsRequest::setPageSize(int pageSize)
{
	pageSize_ = pageSize;
	setParameter("PageSize", std::to_string(pageSize));
}

std::string DescribeAppsRequest::getShowLog()const
{
	return showLog_;
}

void DescribeAppsRequest::setShowLog(const std::string& showLog)
{
	showLog_ = showLog;
	setParameter("ShowLog", showLog);
}

std::string DescribeAppsRequest::getOrder()const
{
	return order_;
}

void DescribeAppsRequest::setOrder(const std::string& order)
{
	order_ = order;
	setParameter("Order", order);
}

long DescribeAppsRequest::getOwnerId()const
{
	return ownerId_;
}

void DescribeAppsRequest::setOwnerId(long ownerId)
{
	ownerId_ = ownerId;
	setParameter("OwnerId", std::to_string(ownerId));
}

std::string DescribeAppsRequest::getAppId()const
{
	return appId_;
}

void DescribeAppsRequest::setAppId(const std::string& appId)
{
	appId_ = appId;
	setParameter("AppId", appId);
}

std::string DescribeAppsRequest::getStatus()const
{
	return status_;
}

void DescribeAppsRequest::setStatus(const std::string& status)
{
	status_ = status;
	setParameter("Status", status);
}

