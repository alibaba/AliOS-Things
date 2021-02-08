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

#include <alibabacloud/live/model/DescribeRoomListRequest.h>

using AlibabaCloud::Live::Model::DescribeRoomListRequest;

DescribeRoomListRequest::DescribeRoomListRequest() :
	RpcServiceRequest("live", "2016-11-01", "DescribeRoomList")
{
	setMethod(HttpRequest::Method::Post);
}

DescribeRoomListRequest::~DescribeRoomListRequest()
{}

std::string DescribeRoomListRequest::getStartTime()const
{
	return startTime_;
}

void DescribeRoomListRequest::setStartTime(const std::string& startTime)
{
	startTime_ = startTime;
	setParameter("StartTime", startTime);
}

std::string DescribeRoomListRequest::getAnchorId()const
{
	return anchorId_;
}

void DescribeRoomListRequest::setAnchorId(const std::string& anchorId)
{
	anchorId_ = anchorId;
	setParameter("AnchorId", anchorId);
}

int DescribeRoomListRequest::getPageNum()const
{
	return pageNum_;
}

void DescribeRoomListRequest::setPageNum(int pageNum)
{
	pageNum_ = pageNum;
	setParameter("PageNum", std::to_string(pageNum));
}

int DescribeRoomListRequest::getRoomStatus()const
{
	return roomStatus_;
}

void DescribeRoomListRequest::setRoomStatus(int roomStatus)
{
	roomStatus_ = roomStatus;
	setParameter("RoomStatus", std::to_string(roomStatus));
}

int DescribeRoomListRequest::getPageSize()const
{
	return pageSize_;
}

void DescribeRoomListRequest::setPageSize(int pageSize)
{
	pageSize_ = pageSize;
	setParameter("PageSize", std::to_string(pageSize));
}

std::string DescribeRoomListRequest::getOrder()const
{
	return order_;
}

void DescribeRoomListRequest::setOrder(const std::string& order)
{
	order_ = order;
	setParameter("Order", order);
}

std::string DescribeRoomListRequest::getEndTime()const
{
	return endTime_;
}

void DescribeRoomListRequest::setEndTime(const std::string& endTime)
{
	endTime_ = endTime;
	setParameter("EndTime", endTime);
}

long DescribeRoomListRequest::getOwnerId()const
{
	return ownerId_;
}

void DescribeRoomListRequest::setOwnerId(long ownerId)
{
	ownerId_ = ownerId;
	setParameter("OwnerId", std::to_string(ownerId));
}

std::string DescribeRoomListRequest::getRoomId()const
{
	return roomId_;
}

void DescribeRoomListRequest::setRoomId(const std::string& roomId)
{
	roomId_ = roomId;
	setParameter("RoomId", roomId);
}

std::string DescribeRoomListRequest::getAppId()const
{
	return appId_;
}

void DescribeRoomListRequest::setAppId(const std::string& appId)
{
	appId_ = appId;
	setParameter("AppId", appId);
}

