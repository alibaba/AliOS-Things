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

#include <alibabacloud/rtc/model/StartRecordTaskRequest.h>

using AlibabaCloud::Rtc::Model::StartRecordTaskRequest;

StartRecordTaskRequest::StartRecordTaskRequest() :
	RpcServiceRequest("rtc", "2018-01-11", "StartRecordTask")
{
	setMethod(HttpRequest::Method::Post);
}

StartRecordTaskRequest::~StartRecordTaskRequest()
{}

std::vector<StartRecordTaskRequest::UserPanes> StartRecordTaskRequest::getUserPanes()const
{
	return userPanes_;
}

void StartRecordTaskRequest::setUserPanes(const std::vector<UserPanes>& userPanes)
{
	userPanes_ = userPanes;
	for(int dep1 = 0; dep1!= userPanes.size(); dep1++) {
		auto userPanesObj = userPanes.at(dep1);
		std::string userPanesObjStr = "UserPanes." + std::to_string(dep1 + 1);
		setParameter(userPanesObjStr + ".PaneId", std::to_string(userPanesObj.paneId));
		setParameter(userPanesObjStr + ".UserId", userPanesObj.userId);
		setParameter(userPanesObjStr + ".SourceType", userPanesObj.sourceType);
		for(int dep2 = 0; dep2!= userPanesObj.images.size(); dep2++) {
			auto imagesObj = userPanesObj.images.at(dep2);
			std::string imagesObjStr = userPanesObjStr + "Images." + std::to_string(dep2 + 1);
			setParameter(imagesObjStr + ".Url", imagesObj.url);
			setParameter(imagesObjStr + ".Display", std::to_string(imagesObj.display));
			setParameter(imagesObjStr + ".X", std::to_string(imagesObj.x));
			setParameter(imagesObjStr + ".Y", std::to_string(imagesObj.y));
			setParameter(imagesObjStr + ".Width", std::to_string(imagesObj.width));
			setParameter(imagesObjStr + ".Height", std::to_string(imagesObj.height));
			setParameter(imagesObjStr + ".ZOrder", std::to_string(imagesObj.zOrder));
		}
		for(int dep2 = 0; dep2!= userPanesObj.texts.size(); dep2++) {
			auto textsObj = userPanesObj.texts.at(dep2);
			std::string textsObjStr = userPanesObjStr + "Texts." + std::to_string(dep2 + 1);
			setParameter(textsObjStr + ".Text", textsObj.text);
			setParameter(textsObjStr + ".X", std::to_string(textsObj.x));
			setParameter(textsObjStr + ".Y", std::to_string(textsObj.y));
			setParameter(textsObjStr + ".FontType", std::to_string(textsObj.fontType));
			setParameter(textsObjStr + ".FontSize", std::to_string(textsObj.fontSize));
			setParameter(textsObjStr + ".FontColor", std::to_string(textsObj.fontColor));
			setParameter(textsObjStr + ".ZOrder", std::to_string(textsObj.zOrder));
		}
	}
}

std::string StartRecordTaskRequest::getTaskId()const
{
	return taskId_;
}

void StartRecordTaskRequest::setTaskId(const std::string& taskId)
{
	taskId_ = taskId;
	setParameter("TaskId", taskId);
}

std::string StartRecordTaskRequest::getShowLog()const
{
	return showLog_;
}

void StartRecordTaskRequest::setShowLog(const std::string& showLog)
{
	showLog_ = showLog;
	setParameter("ShowLog", showLog);
}

long StartRecordTaskRequest::getOwnerId()const
{
	return ownerId_;
}

void StartRecordTaskRequest::setOwnerId(long ownerId)
{
	ownerId_ = ownerId;
	setParameter("OwnerId", std::to_string(ownerId));
}

std::string StartRecordTaskRequest::getTemplateId()const
{
	return templateId_;
}

void StartRecordTaskRequest::setTemplateId(const std::string& templateId)
{
	templateId_ = templateId;
	setParameter("TemplateId", templateId);
}

std::vector<std::string> StartRecordTaskRequest::getSubSpecUsers()const
{
	return subSpecUsers_;
}

void StartRecordTaskRequest::setSubSpecUsers(const std::vector<std::string>& subSpecUsers)
{
	subSpecUsers_ = subSpecUsers;
	for(int dep1 = 0; dep1!= subSpecUsers.size(); dep1++) {
		setParameter("SubSpecUsers."+ std::to_string(dep1), subSpecUsers.at(dep1));
	}
}

std::string StartRecordTaskRequest::getAppId()const
{
	return appId_;
}

void StartRecordTaskRequest::setAppId(const std::string& appId)
{
	appId_ = appId;
	setParameter("AppId", appId);
}

std::string StartRecordTaskRequest::getChannelId()const
{
	return channelId_;
}

void StartRecordTaskRequest::setChannelId(const std::string& channelId)
{
	channelId_ = channelId;
	setParameter("ChannelId", channelId);
}

