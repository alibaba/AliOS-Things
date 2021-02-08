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

#include <alibabacloud/cloudphoto/model/CreateEventRequest.h>

using AlibabaCloud::CloudPhoto::Model::CreateEventRequest;

CreateEventRequest::CreateEventRequest() :
	RpcServiceRequest("cloudphoto", "2017-07-11", "CreateEvent")
{
	setMethod(HttpRequest::Method::Post);
}

CreateEventRequest::~CreateEventRequest()
{}

std::string CreateEventRequest::getStoreName()const
{
	return storeName_;
}

void CreateEventRequest::setStoreName(const std::string& storeName)
{
	storeName_ = storeName;
	setParameter("StoreName", storeName);
}

std::string CreateEventRequest::getRemark()const
{
	return remark_;
}

void CreateEventRequest::setRemark(const std::string& remark)
{
	remark_ = remark;
	setParameter("Remark", remark);
}

std::string CreateEventRequest::getTitle()const
{
	return title_;
}

void CreateEventRequest::setTitle(const std::string& title)
{
	title_ = title;
	setParameter("Title", title);
}

long CreateEventRequest::getEndAt()const
{
	return endAt_;
}

void CreateEventRequest::setEndAt(long endAt)
{
	endAt_ = endAt;
	setParameter("EndAt", std::to_string(endAt));
}

std::string CreateEventRequest::getIdentity()const
{
	return identity_;
}

void CreateEventRequest::setIdentity(const std::string& identity)
{
	identity_ = identity;
	setParameter("Identity", identity);
}

std::string CreateEventRequest::getWatermarkPhotoId()const
{
	return watermarkPhotoId_;
}

void CreateEventRequest::setWatermarkPhotoId(const std::string& watermarkPhotoId)
{
	watermarkPhotoId_ = watermarkPhotoId;
	setParameter("WatermarkPhotoId", watermarkPhotoId);
}

std::string CreateEventRequest::getSplashPhotoId()const
{
	return splashPhotoId_;
}

void CreateEventRequest::setSplashPhotoId(const std::string& splashPhotoId)
{
	splashPhotoId_ = splashPhotoId;
	setParameter("SplashPhotoId", splashPhotoId);
}

std::string CreateEventRequest::getLibraryId()const
{
	return libraryId_;
}

void CreateEventRequest::setLibraryId(const std::string& libraryId)
{
	libraryId_ = libraryId;
	setParameter("LibraryId", libraryId);
}

std::string CreateEventRequest::getWeixinTitle()const
{
	return weixinTitle_;
}

void CreateEventRequest::setWeixinTitle(const std::string& weixinTitle)
{
	weixinTitle_ = weixinTitle;
	setParameter("WeixinTitle", weixinTitle);
}

std::string CreateEventRequest::getBannerPhotoId()const
{
	return bannerPhotoId_;
}

void CreateEventRequest::setBannerPhotoId(const std::string& bannerPhotoId)
{
	bannerPhotoId_ = bannerPhotoId;
	setParameter("BannerPhotoId", bannerPhotoId);
}

long CreateEventRequest::getStartAt()const
{
	return startAt_;
}

void CreateEventRequest::setStartAt(long startAt)
{
	startAt_ = startAt;
	setParameter("StartAt", std::to_string(startAt));
}

