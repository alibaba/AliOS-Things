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

#include <alibabacloud/imm/model/GetVideoResult.h>
#include <json/json.h>

using namespace AlibabaCloud::Imm;
using namespace AlibabaCloud::Imm::Model;

GetVideoResult::GetVideoResult() :
	ServiceResult()
{}

GetVideoResult::GetVideoResult(const std::string &payload) :
	ServiceResult()
{
	parse(payload);
}

GetVideoResult::~GetVideoResult()
{}

void GetVideoResult::parse(const std::string &payload)
{
	Json::Reader reader;
	Json::Value value;
	reader.parse(payload, value);
	setRequestId(value["RequestId"].asString());
	auto allCelebrityNode = value["Celebrity"]["CelebrityItem"];
	for (auto valueCelebrityCelebrityItem : allCelebrityNode)
	{
		CelebrityItem celebrityObject;
		if(!valueCelebrityCelebrityItem["CelebrityName"].isNull())
			celebrityObject.celebrityName = valueCelebrityCelebrityItem["CelebrityName"].asString();
		if(!valueCelebrityCelebrityItem["CelebrityNum"].isNull())
			celebrityObject.celebrityNum = std::stoi(valueCelebrityCelebrityItem["CelebrityNum"].asString());
		if(!valueCelebrityCelebrityItem["CelebrityLibraryName"].isNull())
			celebrityObject.celebrityLibraryName = valueCelebrityCelebrityItem["CelebrityLibraryName"].asString();
		celebrity_.push_back(celebrityObject);
	}
	auto allVideoTagsNode = value["VideoTags"]["VideoTagsItem"];
	for (auto valueVideoTagsVideoTagsItem : allVideoTagsNode)
	{
		VideoTagsItem videoTagsObject;
		if(!valueVideoTagsVideoTagsItem["TagName"].isNull())
			videoTagsObject.tagName = valueVideoTagsVideoTagsItem["TagName"].asString();
		if(!valueVideoTagsVideoTagsItem["ParentTagName"].isNull())
			videoTagsObject.parentTagName = valueVideoTagsVideoTagsItem["ParentTagName"].asString();
		if(!valueVideoTagsVideoTagsItem["TagConfidence"].isNull())
			videoTagsObject.tagConfidence = std::stof(valueVideoTagsVideoTagsItem["TagConfidence"].asString());
		if(!valueVideoTagsVideoTagsItem["TagLevel"].isNull())
			videoTagsObject.tagLevel = std::stoi(valueVideoTagsVideoTagsItem["TagLevel"].asString());
		videoTags_.push_back(videoTagsObject);
	}
	auto allPersonsNode = value["Persons"]["PersonsItem"];
	for (auto valuePersonsPersonsItem : allPersonsNode)
	{
		PersonsItem personsObject;
		if(!valuePersonsPersonsItem["PersonId"].isNull())
			personsObject.personId = valuePersonsPersonsItem["PersonId"].asString();
		if(!valuePersonsPersonsItem["Age"].isNull())
			personsObject.age = std::stoi(valuePersonsPersonsItem["Age"].asString());
		if(!valuePersonsPersonsItem["AgeConfidence"].isNull())
			personsObject.ageConfidence = std::stof(valuePersonsPersonsItem["AgeConfidence"].asString());
		if(!valuePersonsPersonsItem["Gender"].isNull())
			personsObject.gender = valuePersonsPersonsItem["Gender"].asString();
		if(!valuePersonsPersonsItem["GenderConfidence"].isNull())
			personsObject.genderConfidence = std::stof(valuePersonsPersonsItem["GenderConfidence"].asString());
		persons_.push_back(personsObject);
	}
	if(!value["SetId"].isNull())
		setId_ = value["SetId"].asString();
	if(!value["VideoUri"].isNull())
		videoUri_ = value["VideoUri"].asString();
	if(!value["RemarksA"].isNull())
		remarksA_ = value["RemarksA"].asString();
	if(!value["RemarksB"].isNull())
		remarksB_ = value["RemarksB"].asString();
	if(!value["CreateTime"].isNull())
		createTime_ = value["CreateTime"].asString();
	if(!value["ModifyTime"].isNull())
		modifyTime_ = value["ModifyTime"].asString();
	if(!value["VideoWidth"].isNull())
		videoWidth_ = std::stoi(value["VideoWidth"].asString());
	if(!value["VideoHeight"].isNull())
		videoHeight_ = std::stoi(value["VideoHeight"].asString());
	if(!value["VideoFormat"].isNull())
		videoFormat_ = value["VideoFormat"].asString();
	if(!value["VideoDuration"].isNull())
		videoDuration_ = std::stof(value["VideoDuration"].asString());
	if(!value["FileSize"].isNull())
		fileSize_ = std::stoi(value["FileSize"].asString());
	if(!value["VideoFrames"].isNull())
		videoFrames_ = std::stoi(value["VideoFrames"].asString());
	if(!value["SourceType"].isNull())
		sourceType_ = value["SourceType"].asString();
	if(!value["SourceUri"].isNull())
		sourceUri_ = value["SourceUri"].asString();
	if(!value["SourcePosition"].isNull())
		sourcePosition_ = value["SourcePosition"].asString();
	if(!value["ProcessStatus"].isNull())
		processStatus_ = value["ProcessStatus"].asString();
	if(!value["ProcessModifyTime"].isNull())
		processModifyTime_ = value["ProcessModifyTime"].asString();
	if(!value["VideoTagsStatus"].isNull())
		videoTagsStatus_ = value["VideoTagsStatus"].asString();
	if(!value["VideoTagsModifyTime"].isNull())
		videoTagsModifyTime_ = value["VideoTagsModifyTime"].asString();
	if(!value["CelebrityStatus"].isNull())
		celebrityStatus_ = value["CelebrityStatus"].asString();
	if(!value["CelebrityModifyTime"].isNull())
		celebrityModifyTime_ = value["CelebrityModifyTime"].asString();
	if(!value["ProcessFailReason"].isNull())
		processFailReason_ = value["ProcessFailReason"].asString();
	if(!value["VideoTagsFailReason"].isNull())
		videoTagsFailReason_ = value["VideoTagsFailReason"].asString();
	if(!value["CelebrityFailReason"].isNull())
		celebrityFailReason_ = value["CelebrityFailReason"].asString();
	if(!value["RemarksC"].isNull())
		remarksC_ = value["RemarksC"].asString();
	if(!value["RemarksD"].isNull())
		remarksD_ = value["RemarksD"].asString();
	if(!value["ExternalId"].isNull())
		externalId_ = value["ExternalId"].asString();
	if(!value["VideoFacesStatus"].isNull())
		videoFacesStatus_ = value["VideoFacesStatus"].asString();
	if(!value["VideoFacesFailReason"].isNull())
		videoFacesFailReason_ = value["VideoFacesFailReason"].asString();
	if(!value["VideoFacesModifyTime"].isNull())
		videoFacesModifyTime_ = value["VideoFacesModifyTime"].asString();
	if(!value["VideoFrameTagsStatus"].isNull())
		videoFrameTagsStatus_ = value["VideoFrameTagsStatus"].asString();
	if(!value["VideoFrameTagsFailReason"].isNull())
		videoFrameTagsFailReason_ = value["VideoFrameTagsFailReason"].asString();
	if(!value["VideoFrameTagsModifyTime"].isNull())
		videoFrameTagsModifyTime_ = value["VideoFrameTagsModifyTime"].asString();
	if(!value["VideoSTTStatus"].isNull())
		videoSTTStatus_ = value["VideoSTTStatus"].asString();
	if(!value["VideoSTTFailReason"].isNull())
		videoSTTFailReason_ = value["VideoSTTFailReason"].asString();
	if(!value["VideoSTTModifyTime"].isNull())
		videoSTTModifyTime_ = value["VideoSTTModifyTime"].asString();
	if(!value["VideoOCRStatus"].isNull())
		videoOCRStatus_ = value["VideoOCRStatus"].asString();
	if(!value["VideoOCRFailReason"].isNull())
		videoOCRFailReason_ = value["VideoOCRFailReason"].asString();
	if(!value["VideoOCRModifyTime"].isNull())
		videoOCRModifyTime_ = value["VideoOCRModifyTime"].asString();
	if(!value["VideoInfo"].isNull())
		videoInfo_ = value["VideoInfo"].asString();

}

std::string GetVideoResult::getModifyTime()const
{
	return modifyTime_;
}

std::string GetVideoResult::getProcessStatus()const
{
	return processStatus_;
}

int GetVideoResult::getVideoWidth()const
{
	return videoWidth_;
}

std::string GetVideoResult::getSourceType()const
{
	return sourceType_;
}

std::string GetVideoResult::getSourceUri()const
{
	return sourceUri_;
}

std::string GetVideoResult::getVideoInfo()const
{
	return videoInfo_;
}

std::string GetVideoResult::getRemarksA()const
{
	return remarksA_;
}

std::string GetVideoResult::getVideoFrameTagsModifyTime()const
{
	return videoFrameTagsModifyTime_;
}

std::string GetVideoResult::getRemarksB()const
{
	return remarksB_;
}

std::string GetVideoResult::getVideoFacesFailReason()const
{
	return videoFacesFailReason_;
}

std::string GetVideoResult::getVideoFacesStatus()const
{
	return videoFacesStatus_;
}

std::string GetVideoResult::getRemarksC()const
{
	return remarksC_;
}

std::string GetVideoResult::getVideoOCRModifyTime()const
{
	return videoOCRModifyTime_;
}

std::string GetVideoResult::getRemarksD()const
{
	return remarksD_;
}

int GetVideoResult::getVideoHeight()const
{
	return videoHeight_;
}

std::vector<GetVideoResult::PersonsItem> GetVideoResult::getPersons()const
{
	return persons_;
}

std::vector<GetVideoResult::CelebrityItem> GetVideoResult::getCelebrity()const
{
	return celebrity_;
}

std::string GetVideoResult::getSourcePosition()const
{
	return sourcePosition_;
}

std::string GetVideoResult::getVideoOCRFailReason()const
{
	return videoOCRFailReason_;
}

std::string GetVideoResult::getVideoFrameTagsStatus()const
{
	return videoFrameTagsStatus_;
}

std::string GetVideoResult::getVideoTagsFailReason()const
{
	return videoTagsFailReason_;
}

std::string GetVideoResult::getVideoTagsModifyTime()const
{
	return videoTagsModifyTime_;
}

std::string GetVideoResult::getVideoOCRStatus()const
{
	return videoOCRStatus_;
}

int GetVideoResult::getVideoFrames()const
{
	return videoFrames_;
}

std::string GetVideoResult::getProcessModifyTime()const
{
	return processModifyTime_;
}

std::string GetVideoResult::getVideoSTTModifyTime()const
{
	return videoSTTModifyTime_;
}

std::string GetVideoResult::getProcessFailReason()const
{
	return processFailReason_;
}

std::string GetVideoResult::getCreateTime()const
{
	return createTime_;
}

std::string GetVideoResult::getExternalId()const
{
	return externalId_;
}

std::string GetVideoResult::getVideoSTTFailReason()const
{
	return videoSTTFailReason_;
}

std::string GetVideoResult::getVideoUri()const
{
	return videoUri_;
}

std::string GetVideoResult::getVideoFormat()const
{
	return videoFormat_;
}

std::string GetVideoResult::getVideoFrameTagsFailReason()const
{
	return videoFrameTagsFailReason_;
}

std::string GetVideoResult::getVideoSTTStatus()const
{
	return videoSTTStatus_;
}

std::string GetVideoResult::getVideoFacesModifyTime()const
{
	return videoFacesModifyTime_;
}

std::vector<GetVideoResult::VideoTagsItem> GetVideoResult::getVideoTags()const
{
	return videoTags_;
}

std::string GetVideoResult::getCelebrityModifyTime()const
{
	return celebrityModifyTime_;
}

float GetVideoResult::getVideoDuration()const
{
	return videoDuration_;
}

std::string GetVideoResult::getCelebrityFailReason()const
{
	return celebrityFailReason_;
}

std::string GetVideoResult::getSetId()const
{
	return setId_;
}

std::string GetVideoResult::getCelebrityStatus()const
{
	return celebrityStatus_;
}

std::string GetVideoResult::getVideoTagsStatus()const
{
	return videoTagsStatus_;
}

int GetVideoResult::getFileSize()const
{
	return fileSize_;
}

