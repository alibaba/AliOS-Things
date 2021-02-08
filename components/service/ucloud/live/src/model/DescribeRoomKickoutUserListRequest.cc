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

#include <alibabacloud/live/model/DescribeRoomKickoutUserListRequest.h>

using AlibabaCloud::Live::Model::DescribeRoomKickoutUserListRequest;

DescribeRoomKickoutUserListRequest::DescribeRoomKickoutUserListRequest() :
	RpcServiceRequest("live", "2016-11-01", "DescribeRoomKickoutUserList")
{
	setMethod(HttpRequest::Method::Post);
}

DescribeRoomKickoutUserListRequest::~DescribeRoomKickoutUserListRequest()
{}

int DescribeRoomKickoutUserListRequest::getPageNum()const
{
	return pageNum_;
}

void DescribeRoomKickoutUserListRequest::setPageNum(int pageNum)
{
	pageNum_ = pageNum;
	setParameter("PageNum", std::to_string(pageNum));
}

int DescribeRoomKickoutUserListRequest::getPageSize()const
{
	return pageSize_;
}

void DescribeRoomKickoutUserListRequest::setPageSize(int pageSize)
{
	pageSize_ = pageSize;
	setParameter("PageSize", std::to_string(pageSize));
}

std::string DescribeRoomKickoutUserListRequest::getOrder()const
{
	return order_;
}

void DescribeRoomKickoutUserListRequest::setOrder(const std::string& order)
{
	order_ = order;
	setParameter("Order", order);
}

long DescribeRoomKickoutUserListRequest::getOwnerId()const
{
	return ownerId_;
}

void DescribeRoomKickoutUserListRequest::setOwnerId(long ownerId)
{
	ownerId_ = ownerId;
	setParameter("OwnerId", std::to_string(ownerId));
}

std::string DescribeRoomKickoutUserListRequest::getRoomId()const
{
	return roomId_;
}

void DescribeRoomKickoutUserListRequest::setRoomId(const std::string& roomId)
{
	roomId_ = roomId;
	setParameter("RoomId", roomId);
}

std::string DescribeRoomKickoutUserListRequest::getAppId()const
{
	return appId_;
}

void DescribeRoomKickoutUserListRequest::setAppId(const std::string& appId)
{
	appId_ = appId;
	setParameter("AppId", appId);
}

