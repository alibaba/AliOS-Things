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

#include <alibabacloud/rtc/model/AddRecordTemplateRequest.h>

using AlibabaCloud::Rtc::Model::AddRecordTemplateRequest;

AddRecordTemplateRequest::AddRecordTemplateRequest() :
	RpcServiceRequest("rtc", "2018-01-11", "AddRecordTemplate")
{
	setMethod(HttpRequest::Method::Post);
}

AddRecordTemplateRequest::~AddRecordTemplateRequest()
{}

std::vector<std::string> AddRecordTemplateRequest::getFormats()const
{
	return formats_;
}

void AddRecordTemplateRequest::setFormats(const std::vector<std::string>& formats)
{
	formats_ = formats;
	for(int dep1 = 0; dep1!= formats.size(); dep1++) {
		setParameter("Formats."+ std::to_string(dep1), formats.at(dep1));
	}
}

std::string AddRecordTemplateRequest::getOssFilePrefix()const
{
	return ossFilePrefix_;
}

void AddRecordTemplateRequest::setOssFilePrefix(const std::string& ossFilePrefix)
{
	ossFilePrefix_ = ossFilePrefix;
	setParameter("OssFilePrefix", ossFilePrefix);
}

int AddRecordTemplateRequest::getBackgroundColor()const
{
	return backgroundColor_;
}

void AddRecordTemplateRequest::setBackgroundColor(int backgroundColor)
{
	backgroundColor_ = backgroundColor;
	setParameter("BackgroundColor", std::to_string(backgroundColor));
}

std::string AddRecordTemplateRequest::getTaskProfile()const
{
	return taskProfile_;
}

void AddRecordTemplateRequest::setTaskProfile(const std::string& taskProfile)
{
	taskProfile_ = taskProfile;
	setParameter("TaskProfile", taskProfile);
}

std::vector<long> AddRecordTemplateRequest::getLayoutIds()const
{
	return layoutIds_;
}

void AddRecordTemplateRequest::setLayoutIds(const std::vector<long>& layoutIds)
{
	layoutIds_ = layoutIds;
	for(int dep1 = 0; dep1!= layoutIds.size(); dep1++) {
		setParameter("LayoutIds."+ std::to_string(dep1), std::to_string(layoutIds.at(dep1)));
	}
}

std::string AddRecordTemplateRequest::getShowLog()const
{
	return showLog_;
}

void AddRecordTemplateRequest::setShowLog(const std::string& showLog)
{
	showLog_ = showLog;
	setParameter("ShowLog", showLog);
}

std::string AddRecordTemplateRequest::getOssBucket()const
{
	return ossBucket_;
}

void AddRecordTemplateRequest::setOssBucket(const std::string& ossBucket)
{
	ossBucket_ = ossBucket;
	setParameter("OssBucket", ossBucket);
}

int AddRecordTemplateRequest::getDelayStopTime()const
{
	return delayStopTime_;
}

void AddRecordTemplateRequest::setDelayStopTime(int delayStopTime)
{
	delayStopTime_ = delayStopTime;
	setParameter("DelayStopTime", std::to_string(delayStopTime));
}

int AddRecordTemplateRequest::getFileSplitInterval()const
{
	return fileSplitInterval_;
}

void AddRecordTemplateRequest::setFileSplitInterval(int fileSplitInterval)
{
	fileSplitInterval_ = fileSplitInterval;
	setParameter("FileSplitInterval", std::to_string(fileSplitInterval));
}

std::string AddRecordTemplateRequest::getMnsQueue()const
{
	return mnsQueue_;
}

void AddRecordTemplateRequest::setMnsQueue(const std::string& mnsQueue)
{
	mnsQueue_ = mnsQueue;
	setParameter("MnsQueue", mnsQueue);
}

std::string AddRecordTemplateRequest::getHttpCallbackUrl()const
{
	return httpCallbackUrl_;
}

void AddRecordTemplateRequest::setHttpCallbackUrl(const std::string& httpCallbackUrl)
{
	httpCallbackUrl_ = httpCallbackUrl;
	setParameter("HttpCallbackUrl", httpCallbackUrl);
}

std::vector<AddRecordTemplateRequest::Watermarks> AddRecordTemplateRequest::getWatermarks()const
{
	return watermarks_;
}

void AddRecordTemplateRequest::setWatermarks(const std::vector<Watermarks>& watermarks)
{
	watermarks_ = watermarks;
	for(int dep1 = 0; dep1!= watermarks.size(); dep1++) {
		auto watermarksObj = watermarks.at(dep1);
		std::string watermarksObjStr = "Watermarks." + std::to_string(dep1 + 1);
		setParameter(watermarksObjStr + ".Url", watermarksObj.url);
		setParameter(watermarksObjStr + ".Alpha", std::to_string(watermarksObj.alpha));
		setParameter(watermarksObjStr + ".Display", std::to_string(watermarksObj.display));
		setParameter(watermarksObjStr + ".X", std::to_string(watermarksObj.x));
		setParameter(watermarksObjStr + ".Y", std::to_string(watermarksObj.y));
		setParameter(watermarksObjStr + ".Width", std::to_string(watermarksObj.width));
		setParameter(watermarksObjStr + ".Height", std::to_string(watermarksObj.height));
		setParameter(watermarksObjStr + ".ZOrder", std::to_string(watermarksObj.zOrder));
	}
}

long AddRecordTemplateRequest::getOwnerId()const
{
	return ownerId_;
}

void AddRecordTemplateRequest::setOwnerId(long ownerId)
{
	ownerId_ = ownerId;
	setParameter("OwnerId", std::to_string(ownerId));
}

std::string AddRecordTemplateRequest::getAppId()const
{
	return appId_;
}

void AddRecordTemplateRequest::setAppId(const std::string& appId)
{
	appId_ = appId;
	setParameter("AppId", appId);
}

std::vector<AddRecordTemplateRequest::Backgrounds> AddRecordTemplateRequest::getBackgrounds()const
{
	return backgrounds_;
}

void AddRecordTemplateRequest::setBackgrounds(const std::vector<Backgrounds>& backgrounds)
{
	backgrounds_ = backgrounds;
	for(int dep1 = 0; dep1!= backgrounds.size(); dep1++) {
		auto backgroundsObj = backgrounds.at(dep1);
		std::string backgroundsObjStr = "Backgrounds." + std::to_string(dep1 + 1);
		setParameter(backgroundsObjStr + ".Url", backgroundsObj.url);
		setParameter(backgroundsObjStr + ".Display", std::to_string(backgroundsObj.display));
		setParameter(backgroundsObjStr + ".X", std::to_string(backgroundsObj.x));
		setParameter(backgroundsObjStr + ".Y", std::to_string(backgroundsObj.y));
		setParameter(backgroundsObjStr + ".Width", std::to_string(backgroundsObj.width));
		setParameter(backgroundsObjStr + ".Height", std::to_string(backgroundsObj.height));
		setParameter(backgroundsObjStr + ".ZOrder", std::to_string(backgroundsObj.zOrder));
	}
}

std::string AddRecordTemplateRequest::getName()const
{
	return name_;
}

void AddRecordTemplateRequest::setName(const std::string& name)
{
	name_ = name;
	setParameter("Name", name);
}

int AddRecordTemplateRequest::getMediaEncode()const
{
	return mediaEncode_;
}

void AddRecordTemplateRequest::setMediaEncode(int mediaEncode)
{
	mediaEncode_ = mediaEncode;
	setParameter("MediaEncode", std::to_string(mediaEncode));
}

