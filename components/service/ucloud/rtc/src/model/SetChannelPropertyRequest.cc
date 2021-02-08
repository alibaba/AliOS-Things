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

#include <alibabacloud/rtc/model/SetChannelPropertyRequest.h>

using AlibabaCloud::Rtc::Model::SetChannelPropertyRequest;

SetChannelPropertyRequest::SetChannelPropertyRequest() :
	RpcServiceRequest("rtc", "2018-01-11", "SetChannelProperty")
{
	setMethod(HttpRequest::Method::Post);
}

SetChannelPropertyRequest::~SetChannelPropertyRequest()
{}

int SetChannelPropertyRequest::getStartTime()const
{
	return startTime_;
}

void SetChannelPropertyRequest::setStartTime(int startTime)
{
	startTime_ = startTime;
	setParameter("StartTime", std::to_string(startTime));
}

int SetChannelPropertyRequest::getMaxUserNum()const
{
	return maxUserNum_;
}

void SetChannelPropertyRequest::setMaxUserNum(int maxUserNum)
{
	maxUserNum_ = maxUserNum;
	setParameter("MaxUserNum", std::to_string(maxUserNum));
}

int SetChannelPropertyRequest::getDuration()const
{
	return duration_;
}

void SetChannelPropertyRequest::setDuration(int duration)
{
	duration_ = duration;
	setParameter("Duration", std::to_string(duration));
}

std::string SetChannelPropertyRequest::getShowLog()const
{
	return showLog_;
}

void SetChannelPropertyRequest::setShowLog(const std::string& showLog)
{
	showLog_ = showLog;
	setParameter("ShowLog", showLog);
}

std::string SetChannelPropertyRequest::getTopics()const
{
	return topics_;
}

void SetChannelPropertyRequest::setTopics(const std::string& topics)
{
	topics_ = topics;
	setParameter("Topics", topics);
}

long SetChannelPropertyRequest::getOwnerId()const
{
	return ownerId_;
}

void SetChannelPropertyRequest::setOwnerId(long ownerId)
{
	ownerId_ = ownerId;
	setParameter("OwnerId", std::to_string(ownerId));
}

std::string SetChannelPropertyRequest::getPriority()const
{
	return priority_;
}

void SetChannelPropertyRequest::setPriority(const std::string& priority)
{
	priority_ = priority;
	setParameter("Priority", priority);
}

std::string SetChannelPropertyRequest::getAppId()const
{
	return appId_;
}

void SetChannelPropertyRequest::setAppId(const std::string& appId)
{
	appId_ = appId;
	setParameter("AppId", appId);
}

std::string SetChannelPropertyRequest::getChannelId()const
{
	return channelId_;
}

void SetChannelPropertyRequest::setChannelId(const std::string& channelId)
{
	channelId_ = channelId;
	setParameter("ChannelId", channelId);
}

