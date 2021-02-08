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

#include <alibabacloud/cloudphoto/model/ListTimeLinesRequest.h>

using AlibabaCloud::CloudPhoto::Model::ListTimeLinesRequest;

ListTimeLinesRequest::ListTimeLinesRequest() :
	RpcServiceRequest("cloudphoto", "2017-07-11", "ListTimeLines")
{
	setMethod(HttpRequest::Method::Post);
}

ListTimeLinesRequest::~ListTimeLinesRequest()
{}

long ListTimeLinesRequest::getCursor()const
{
	return cursor_;
}

void ListTimeLinesRequest::setCursor(long cursor)
{
	cursor_ = cursor;
	setParameter("Cursor", std::to_string(cursor));
}

std::string ListTimeLinesRequest::getLibraryId()const
{
	return libraryId_;
}

void ListTimeLinesRequest::setLibraryId(const std::string& libraryId)
{
	libraryId_ = libraryId;
	setParameter("LibraryId", libraryId);
}

std::string ListTimeLinesRequest::getStoreName()const
{
	return storeName_;
}

void ListTimeLinesRequest::setStoreName(const std::string& storeName)
{
	storeName_ = storeName;
	setParameter("StoreName", storeName);
}

std::string ListTimeLinesRequest::getFilterBy()const
{
	return filterBy_;
}

void ListTimeLinesRequest::setFilterBy(const std::string& filterBy)
{
	filterBy_ = filterBy;
	setParameter("FilterBy", filterBy);
}

int ListTimeLinesRequest::getPhotoSize()const
{
	return photoSize_;
}

void ListTimeLinesRequest::setPhotoSize(int photoSize)
{
	photoSize_ = photoSize;
	setParameter("PhotoSize", std::to_string(photoSize));
}

int ListTimeLinesRequest::getTimeLineCount()const
{
	return timeLineCount_;
}

void ListTimeLinesRequest::setTimeLineCount(int timeLineCount)
{
	timeLineCount_ = timeLineCount;
	setParameter("TimeLineCount", std::to_string(timeLineCount));
}

std::string ListTimeLinesRequest::getTimeLineUnit()const
{
	return timeLineUnit_;
}

void ListTimeLinesRequest::setTimeLineUnit(const std::string& timeLineUnit)
{
	timeLineUnit_ = timeLineUnit;
	setParameter("TimeLineUnit", timeLineUnit);
}

std::string ListTimeLinesRequest::getDirection()const
{
	return direction_;
}

void ListTimeLinesRequest::setDirection(const std::string& direction)
{
	direction_ = direction;
	setParameter("Direction", direction);
}

std::string ListTimeLinesRequest::getOrder()const
{
	return order_;
}

void ListTimeLinesRequest::setOrder(const std::string& order)
{
	order_ = order;
	setParameter("Order", order);
}

