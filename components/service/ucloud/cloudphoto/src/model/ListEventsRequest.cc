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

#include <alibabacloud/cloudphoto/model/ListEventsRequest.h>

using AlibabaCloud::CloudPhoto::Model::ListEventsRequest;

ListEventsRequest::ListEventsRequest() :
	RpcServiceRequest("cloudphoto", "2017-07-11", "ListEvents")
{
	setMethod(HttpRequest::Method::Post);
}

ListEventsRequest::~ListEventsRequest()
{}

std::string ListEventsRequest::getCursor()const
{
	return cursor_;
}

void ListEventsRequest::setCursor(const std::string& cursor)
{
	cursor_ = cursor;
	setParameter("Cursor", cursor);
}

std::string ListEventsRequest::getStoreName()const
{
	return storeName_;
}

void ListEventsRequest::setStoreName(const std::string& storeName)
{
	storeName_ = storeName;
	setParameter("StoreName", storeName);
}

int ListEventsRequest::getSize()const
{
	return size_;
}

void ListEventsRequest::setSize(int size)
{
	size_ = size;
	setParameter("Size", std::to_string(size));
}

std::string ListEventsRequest::getState()const
{
	return state_;
}

void ListEventsRequest::setState(const std::string& state)
{
	state_ = state;
	setParameter("State", state);
}

std::string ListEventsRequest::getDirection()const
{
	return direction_;
}

void ListEventsRequest::setDirection(const std::string& direction)
{
	direction_ = direction;
	setParameter("Direction", direction);
}

