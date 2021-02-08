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

#include <alibabacloud/rtc/model/StartMPUTaskRequest.h>

using AlibabaCloud::Rtc::Model::StartMPUTaskRequest;

StartMPUTaskRequest::StartMPUTaskRequest() :
	RpcServiceRequest("rtc", "2018-01-11", "StartMPUTask")
{
	setMethod(HttpRequest::Method::Post);
}

StartMPUTaskRequest::~StartMPUTaskRequest()
{}

int StartMPUTaskRequest::getPayloadType()const
{
	return payloadType_;
}

void StartMPUTaskRequest::setPayloadType(int payloadType)
{
	payloadType_ = payloadType;
	setParameter("PayloadType", std::to_string(payloadType));
}

std::vector<StartMPUTaskRequest::UserPanes> StartMPUTaskRequest::getUserPanes()const
{
	return userPanes_;
}

void StartMPUTaskRequest::setUserPanes(const std::vector<UserPanes>& userPanes)
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

int StartMPUTaskRequest::getBackgroundColor()const
{
	return backgroundColor_;
}

void StartMPUTaskRequest::setBackgroundColor(int backgroundColor)
{
	backgroundColor_ = backgroundColor;
	setParameter("BackgroundColor", std::to_string(backgroundColor));
}

int StartMPUTaskRequest::getReportVad()const
{
	return reportVad_;
}

void StartMPUTaskRequest::setReportVad(int reportVad)
{
	reportVad_ = reportVad;
	setParameter("ReportVad", std::to_string(reportVad));
}

std::string StartMPUTaskRequest::getSourceType()const
{
	return sourceType_;
}

void StartMPUTaskRequest::setSourceType(const std::string& sourceType)
{
	sourceType_ = sourceType;
	setParameter("SourceType", sourceType);
}

std::string StartMPUTaskRequest::getTaskId()const
{
	return taskId_;
}

void StartMPUTaskRequest::setTaskId(const std::string& taskId)
{
	taskId_ = taskId;
	setParameter("TaskId", taskId);
}

std::vector<StartMPUTaskRequest::ClockWidgets> StartMPUTaskRequest::getClockWidgets()const
{
	return clockWidgets_;
}

void StartMPUTaskRequest::setClockWidgets(const std::vector<ClockWidgets>& clockWidgets)
{
	clockWidgets_ = clockWidgets;
	for(int dep1 = 0; dep1!= clockWidgets.size(); dep1++) {
		auto clockWidgetsObj = clockWidgets.at(dep1);
		std::string clockWidgetsObjStr = "ClockWidgets." + std::to_string(dep1 + 1);
		setParameter(clockWidgetsObjStr + ".X", std::to_string(clockWidgetsObj.x));
		setParameter(clockWidgetsObjStr + ".Y", std::to_string(clockWidgetsObj.y));
		setParameter(clockWidgetsObjStr + ".FontType", std::to_string(clockWidgetsObj.fontType));
		setParameter(clockWidgetsObjStr + ".FontSize", std::to_string(clockWidgetsObj.fontSize));
		setParameter(clockWidgetsObjStr + ".FontColor", std::to_string(clockWidgetsObj.fontColor));
		setParameter(clockWidgetsObjStr + ".ZOrder", std::to_string(clockWidgetsObj.zOrder));
	}
}

std::string StartMPUTaskRequest::getShowLog()const
{
	return showLog_;
}

void StartMPUTaskRequest::setShowLog(const std::string& showLog)
{
	showLog_ = showLog;
	setParameter("ShowLog", showLog);
}

long StartMPUTaskRequest::getVadInterval()const
{
	return vadInterval_;
}

void StartMPUTaskRequest::setVadInterval(long vadInterval)
{
	vadInterval_ = vadInterval;
	setParameter("VadInterval", std::to_string(vadInterval));
}

std::vector<StartMPUTaskRequest::Watermarks> StartMPUTaskRequest::getWatermarks()const
{
	return watermarks_;
}

void StartMPUTaskRequest::setWatermarks(const std::vector<Watermarks>& watermarks)
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

long StartMPUTaskRequest::getOwnerId()const
{
	return ownerId_;
}

void StartMPUTaskRequest::setOwnerId(long ownerId)
{
	ownerId_ = ownerId;
	setParameter("OwnerId", std::to_string(ownerId));
}

int StartMPUTaskRequest::getMediaEncode()const
{
	return mediaEncode_;
}

void StartMPUTaskRequest::setMediaEncode(int mediaEncode)
{
	mediaEncode_ = mediaEncode;
	setParameter("MediaEncode", std::to_string(mediaEncode));
}

int StartMPUTaskRequest::getRtpExtInfo()const
{
	return rtpExtInfo_;
}

void StartMPUTaskRequest::setRtpExtInfo(int rtpExtInfo)
{
	rtpExtInfo_ = rtpExtInfo;
	setParameter("RtpExtInfo", std::to_string(rtpExtInfo));
}

int StartMPUTaskRequest::getCropMode()const
{
	return cropMode_;
}

void StartMPUTaskRequest::setCropMode(int cropMode)
{
	cropMode_ = cropMode;
	setParameter("CropMode", std::to_string(cropMode));
}

std::string StartMPUTaskRequest::getTaskProfile()const
{
	return taskProfile_;
}

void StartMPUTaskRequest::setTaskProfile(const std::string& taskProfile)
{
	taskProfile_ = taskProfile;
	setParameter("TaskProfile", taskProfile);
}

std::vector<long> StartMPUTaskRequest::getLayoutIds()const
{
	return layoutIds_;
}

void StartMPUTaskRequest::setLayoutIds(const std::vector<long>& layoutIds)
{
	layoutIds_ = layoutIds;
	for(int dep1 = 0; dep1!= layoutIds.size(); dep1++) {
		setParameter("LayoutIds."+ std::to_string(dep1), std::to_string(layoutIds.at(dep1)));
	}
}

std::string StartMPUTaskRequest::getStreamURL()const
{
	return streamURL_;
}

void StartMPUTaskRequest::setStreamURL(const std::string& streamURL)
{
	streamURL_ = streamURL;
	setParameter("StreamURL", streamURL);
}

int StartMPUTaskRequest::getStreamType()const
{
	return streamType_;
}

void StartMPUTaskRequest::setStreamType(int streamType)
{
	streamType_ = streamType;
	setParameter("StreamType", std::to_string(streamType));
}

std::vector<std::string> StartMPUTaskRequest::getSubSpecUsers()const
{
	return subSpecUsers_;
}

void StartMPUTaskRequest::setSubSpecUsers(const std::vector<std::string>& subSpecUsers)
{
	subSpecUsers_ = subSpecUsers;
	for(int dep1 = 0; dep1!= subSpecUsers.size(); dep1++) {
		setParameter("SubSpecUsers."+ std::to_string(dep1), subSpecUsers.at(dep1));
	}
}

std::string StartMPUTaskRequest::getAppId()const
{
	return appId_;
}

void StartMPUTaskRequest::setAppId(const std::string& appId)
{
	appId_ = appId;
	setParameter("AppId", appId);
}

std::vector<StartMPUTaskRequest::Backgrounds> StartMPUTaskRequest::getBackgrounds()const
{
	return backgrounds_;
}

void StartMPUTaskRequest::setBackgrounds(const std::vector<Backgrounds>& backgrounds)
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

long StartMPUTaskRequest::getTimeStampRef()const
{
	return timeStampRef_;
}

void StartMPUTaskRequest::setTimeStampRef(long timeStampRef)
{
	timeStampRef_ = timeStampRef;
	setParameter("TimeStampRef", std::to_string(timeStampRef));
}

int StartMPUTaskRequest::getMixMode()const
{
	return mixMode_;
}

void StartMPUTaskRequest::setMixMode(int mixMode)
{
	mixMode_ = mixMode;
	setParameter("MixMode", std::to_string(mixMode));
}

std::string StartMPUTaskRequest::getChannelId()const
{
	return channelId_;
}

void StartMPUTaskRequest::setChannelId(const std::string& channelId)
{
	channelId_ = channelId;
	setParameter("ChannelId", channelId);
}

