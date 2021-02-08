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

#include <alibabacloud/rtc/model/CreateMPURuleRequest.h>

using AlibabaCloud::Rtc::Model::CreateMPURuleRequest;

CreateMPURuleRequest::CreateMPURuleRequest() :
	RpcServiceRequest("rtc", "2018-01-11", "CreateMPURule")
{
	setMethod(HttpRequest::Method::Post);
}

CreateMPURuleRequest::~CreateMPURuleRequest()
{}

int CreateMPURuleRequest::getBackgroundColor()const
{
	return backgroundColor_;
}

void CreateMPURuleRequest::setBackgroundColor(int backgroundColor)
{
	backgroundColor_ = backgroundColor;
	setParameter("BackgroundColor", std::to_string(backgroundColor));
}

int CreateMPURuleRequest::getCropMode()const
{
	return cropMode_;
}

void CreateMPURuleRequest::setCropMode(int cropMode)
{
	cropMode_ = cropMode;
	setParameter("CropMode", std::to_string(cropMode));
}

std::string CreateMPURuleRequest::getChannelPrefix()const
{
	return channelPrefix_;
}

void CreateMPURuleRequest::setChannelPrefix(const std::string& channelPrefix)
{
	channelPrefix_ = channelPrefix;
	setParameter("ChannelPrefix", channelPrefix);
}

std::string CreateMPURuleRequest::getTaskProfile()const
{
	return taskProfile_;
}

void CreateMPURuleRequest::setTaskProfile(const std::string& taskProfile)
{
	taskProfile_ = taskProfile;
	setParameter("TaskProfile", taskProfile);
}

std::vector<long> CreateMPURuleRequest::getLayoutIds()const
{
	return layoutIds_;
}

void CreateMPURuleRequest::setLayoutIds(const std::vector<long>& layoutIds)
{
	layoutIds_ = layoutIds;
	for(int dep1 = 0; dep1!= layoutIds.size(); dep1++) {
		setParameter("LayoutIds."+ std::to_string(dep1), std::to_string(layoutIds.at(dep1)));
	}
}

std::string CreateMPURuleRequest::getShowLog()const
{
	return showLog_;
}

void CreateMPURuleRequest::setShowLog(const std::string& showLog)
{
	showLog_ = showLog;
	setParameter("ShowLog", showLog);
}

std::string CreateMPURuleRequest::getPlayDomain()const
{
	return playDomain_;
}

void CreateMPURuleRequest::setPlayDomain(const std::string& playDomain)
{
	playDomain_ = playDomain;
	setParameter("PlayDomain", playDomain);
}

long CreateMPURuleRequest::getOwnerId()const
{
	return ownerId_;
}

void CreateMPURuleRequest::setOwnerId(long ownerId)
{
	ownerId_ = ownerId;
	setParameter("OwnerId", std::to_string(ownerId));
}

std::string CreateMPURuleRequest::getAppId()const
{
	return appId_;
}

void CreateMPURuleRequest::setAppId(const std::string& appId)
{
	appId_ = appId;
	setParameter("AppId", appId);
}

int CreateMPURuleRequest::getMediaEncode()const
{
	return mediaEncode_;
}

void CreateMPURuleRequest::setMediaEncode(int mediaEncode)
{
	mediaEncode_ = mediaEncode;
	setParameter("MediaEncode", std::to_string(mediaEncode));
}

std::string CreateMPURuleRequest::getCallBack()const
{
	return callBack_;
}

void CreateMPURuleRequest::setCallBack(const std::string& callBack)
{
	callBack_ = callBack;
	setParameter("CallBack", callBack);
}

