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

#include <alibabacloud/rtc/model/DescribeChannelParticipantsRequest.h>

using AlibabaCloud::Rtc::Model::DescribeChannelParticipantsRequest;

DescribeChannelParticipantsRequest::DescribeChannelParticipantsRequest() :
	RpcServiceRequest("rtc", "2018-01-11", "DescribeChannelParticipants")
{
	setMethod(HttpRequest::Method::Post);
}

DescribeChannelParticipantsRequest::~DescribeChannelParticipantsRequest()
{}

int DescribeChannelParticipantsRequest::getPageNum()const
{
	return pageNum_;
}

void DescribeChannelParticipantsRequest::setPageNum(int pageNum)
{
	pageNum_ = pageNum;
	setParameter("PageNum", std::to_string(pageNum));
}

int DescribeChannelParticipantsRequest::getPageSize()const
{
	return pageSize_;
}

void DescribeChannelParticipantsRequest::setPageSize(int pageSize)
{
	pageSize_ = pageSize;
	setParameter("PageSize", std::to_string(pageSize));
}

std::string DescribeChannelParticipantsRequest::getShowLog()const
{
	return showLog_;
}

void DescribeChannelParticipantsRequest::setShowLog(const std::string& showLog)
{
	showLog_ = showLog;
	setParameter("ShowLog", showLog);
}

std::string DescribeChannelParticipantsRequest::getOrder()const
{
	return order_;
}

void DescribeChannelParticipantsRequest::setOrder(const std::string& order)
{
	order_ = order;
	setParameter("Order", order);
}

long DescribeChannelParticipantsRequest::getOwnerId()const
{
	return ownerId_;
}

void DescribeChannelParticipantsRequest::setOwnerId(long ownerId)
{
	ownerId_ = ownerId;
	setParameter("OwnerId", std::to_string(ownerId));
}

std::string DescribeChannelParticipantsRequest::getAppId()const
{
	return appId_;
}

void DescribeChannelParticipantsRequest::setAppId(const std::string& appId)
{
	appId_ = appId;
	setParameter("AppId", appId);
}

std::string DescribeChannelParticipantsRequest::getChannelId()const
{
	return channelId_;
}

void DescribeChannelParticipantsRequest::setChannelId(const std::string& channelId)
{
	channelId_ = channelId;
	setParameter("ChannelId", channelId);
}

