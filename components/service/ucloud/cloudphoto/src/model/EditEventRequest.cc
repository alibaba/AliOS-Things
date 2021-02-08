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

#include <alibabacloud/cloudphoto/model/EditEventRequest.h>

using AlibabaCloud::CloudPhoto::Model::EditEventRequest;

EditEventRequest::EditEventRequest() :
	RpcServiceRequest("cloudphoto", "2017-07-11", "EditEvent")
{
	setMethod(HttpRequest::Method::Post);
}

EditEventRequest::~EditEventRequest()
{}

std::string EditEventRequest::getEventId()const
{
	return eventId_;
}

void EditEventRequest::setEventId(const std::string& eventId)
{
	eventId_ = eventId;
	setParameter("EventId", eventId);
}

std::string EditEventRequest::getStoreName()const
{
	return storeName_;
}

void EditEventRequest::setStoreName(const std::string& storeName)
{
	storeName_ = storeName;
	setParameter("StoreName", storeName);
}

std::string EditEventRequest::getRemark()const
{
	return remark_;
}

void EditEventRequest::setRemark(const std::string& remark)
{
	remark_ = remark;
	setParameter("Remark", remark);
}

std::string EditEventRequest::getTitle()const
{
	return title_;
}

void EditEventRequest::setTitle(const std::string& title)
{
	title_ = title;
	setParameter("Title", title);
}

long EditEventRequest::getEndAt()const
{
	return endAt_;
}

void EditEventRequest::setEndAt(long endAt)
{
	endAt_ = endAt;
	setParameter("EndAt", std::to_string(endAt));
}

std::string EditEventRequest::getIdentity()const
{
	return identity_;
}

void EditEventRequest::setIdentity(const std::string& identity)
{
	identity_ = identity;
	setParameter("Identity", identity);
}

std::string EditEventRequest::getWatermarkPhotoId()const
{
	return watermarkPhotoId_;
}

void EditEventRequest::setWatermarkPhotoId(const std::string& watermarkPhotoId)
{
	watermarkPhotoId_ = watermarkPhotoId;
	setParameter("WatermarkPhotoId", watermarkPhotoId);
}

std::string EditEventRequest::getSplashPhotoId()const
{
	return splashPhotoId_;
}

void EditEventRequest::setSplashPhotoId(const std::string& splashPhotoId)
{
	splashPhotoId_ = splashPhotoId;
	setParameter("SplashPhotoId", splashPhotoId);
}

std::string EditEventRequest::getLibraryId()const
{
	return libraryId_;
}

void EditEventRequest::setLibraryId(const std::string& libraryId)
{
	libraryId_ = libraryId;
	setParameter("LibraryId", libraryId);
}

std::string EditEventRequest::getWeixinTitle()const
{
	return weixinTitle_;
}

void EditEventRequest::setWeixinTitle(const std::string& weixinTitle)
{
	weixinTitle_ = weixinTitle;
	setParameter("WeixinTitle", weixinTitle);
}

std::string EditEventRequest::getBannerPhotoId()const
{
	return bannerPhotoId_;
}

void EditEventRequest::setBannerPhotoId(const std::string& bannerPhotoId)
{
	bannerPhotoId_ = bannerPhotoId;
	setParameter("BannerPhotoId", bannerPhotoId);
}

long EditEventRequest::getStartAt()const
{
	return startAt_;
}

void EditEventRequest::setStartAt(long startAt)
{
	startAt_ = startAt;
	setParameter("StartAt", std::to_string(startAt));
}

