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

#include <alibabacloud/imm/model/ListVideoAudiosResult.h>
#include <json/json.h>

using namespace AlibabaCloud::Imm;
using namespace AlibabaCloud::Imm::Model;

ListVideoAudiosResult::ListVideoAudiosResult() :
	ServiceResult()
{}

ListVideoAudiosResult::ListVideoAudiosResult(const std::string &payload) :
	ServiceResult()
{
	parse(payload);
}

ListVideoAudiosResult::~ListVideoAudiosResult()
{}

void ListVideoAudiosResult::parse(const std::string &payload)
{
	Json::Reader reader;
	Json::Value value;
	reader.parse(payload, value);
	setRequestId(value["RequestId"].asString());
	auto allAudiosNode = value["Audios"]["AudiosItem"];
	for (auto valueAudiosAudiosItem : allAudiosNode)
	{
		AudiosItem audiosObject;
		if(!valueAudiosAudiosItem["AudioDuration"].isNull())
			audiosObject.audioDuration = std::stof(valueAudiosAudiosItem["AudioDuration"].asString());
		if(!valueAudiosAudiosItem["AudioUri"].isNull())
			audiosObject.audioUri = valueAudiosAudiosItem["AudioUri"].asString();
		if(!valueAudiosAudiosItem["AudioRate"].isNull())
			audiosObject.audioRate = std::stoi(valueAudiosAudiosItem["AudioRate"].asString());
		if(!valueAudiosAudiosItem["SourceType"].isNull())
			audiosObject.sourceType = valueAudiosAudiosItem["SourceType"].asString();
		if(!valueAudiosAudiosItem["ModifyTime"].isNull())
			audiosObject.modifyTime = valueAudiosAudiosItem["ModifyTime"].asString();
		if(!valueAudiosAudiosItem["FileSize"].isNull())
			audiosObject.fileSize = std::stoi(valueAudiosAudiosItem["FileSize"].asString());
		if(!valueAudiosAudiosItem["SourceUri"].isNull())
			audiosObject.sourceUri = valueAudiosAudiosItem["SourceUri"].asString();
		if(!valueAudiosAudiosItem["CreateTime"].isNull())
			audiosObject.createTime = valueAudiosAudiosItem["CreateTime"].asString();
		if(!valueAudiosAudiosItem["RemarksA"].isNull())
			audiosObject.remarksA = valueAudiosAudiosItem["RemarksA"].asString();
		if(!valueAudiosAudiosItem["RemarksB"].isNull())
			audiosObject.remarksB = valueAudiosAudiosItem["RemarksB"].asString();
		if(!valueAudiosAudiosItem["AudioTextsStatus"].isNull())
			audiosObject.audioTextsStatus = valueAudiosAudiosItem["AudioTextsStatus"].asString();
		if(!valueAudiosAudiosItem["AudioTextsModifyTime"].isNull())
			audiosObject.audioTextsModifyTime = valueAudiosAudiosItem["AudioTextsModifyTime"].asString();
		if(!valueAudiosAudiosItem["ProcessModifyTime"].isNull())
			audiosObject.processModifyTime = valueAudiosAudiosItem["ProcessModifyTime"].asString();
		if(!valueAudiosAudiosItem["ProcessStatus"].isNull())
			audiosObject.processStatus = valueAudiosAudiosItem["ProcessStatus"].asString();
		if(!valueAudiosAudiosItem["SourcePosition"].isNull())
			audiosObject.sourcePosition = valueAudiosAudiosItem["SourcePosition"].asString();
		if(!valueAudiosAudiosItem["AudioFormat"].isNull())
			audiosObject.audioFormat = valueAudiosAudiosItem["AudioFormat"].asString();
		if(!valueAudiosAudiosItem["AudioTextsFailReason"].isNull())
			audiosObject.audioTextsFailReason = valueAudiosAudiosItem["AudioTextsFailReason"].asString();
		if(!valueAudiosAudiosItem["ProcessFailReason"].isNull())
			audiosObject.processFailReason = valueAudiosAudiosItem["ProcessFailReason"].asString();
		if(!valueAudiosAudiosItem["RemarksC"].isNull())
			audiosObject.remarksC = valueAudiosAudiosItem["RemarksC"].asString();
		if(!valueAudiosAudiosItem["RemarksD"].isNull())
			audiosObject.remarksD = valueAudiosAudiosItem["RemarksD"].asString();
		if(!valueAudiosAudiosItem["ExternalId"].isNull())
			audiosObject.externalId = valueAudiosAudiosItem["ExternalId"].asString();
		auto allAudioTextsNode = valueAudiosAudiosItem["AudioTexts"]["AudioTextsItem"];
		for (auto valueAudiosAudiosItemAudioTextsAudioTextsItem : allAudioTextsNode)
		{
			AudiosItem::AudioTextsItem audioTextsObject;
			if(!valueAudiosAudiosItemAudioTextsAudioTextsItem["Text"].isNull())
				audioTextsObject.text = valueAudiosAudiosItemAudioTextsAudioTextsItem["Text"].asString();
			if(!valueAudiosAudiosItemAudioTextsAudioTextsItem["BeginTime"].isNull())
				audioTextsObject.beginTime = std::stof(valueAudiosAudiosItemAudioTextsAudioTextsItem["BeginTime"].asString());
			if(!valueAudiosAudiosItemAudioTextsAudioTextsItem["EndTime"].isNull())
				audioTextsObject.endTime = std::stof(valueAudiosAudiosItemAudioTextsAudioTextsItem["EndTime"].asString());
			if(!valueAudiosAudiosItemAudioTextsAudioTextsItem["SilenceDuration"].isNull())
				audioTextsObject.silenceDuration = std::stof(valueAudiosAudiosItemAudioTextsAudioTextsItem["SilenceDuration"].asString());
			if(!valueAudiosAudiosItemAudioTextsAudioTextsItem["EmotionValue"].isNull())
				audioTextsObject.emotionValue = std::stof(valueAudiosAudiosItemAudioTextsAudioTextsItem["EmotionValue"].asString());
			if(!valueAudiosAudiosItemAudioTextsAudioTextsItem["ChannelId"].isNull())
				audioTextsObject.channelId = std::stoi(valueAudiosAudiosItemAudioTextsAudioTextsItem["ChannelId"].asString());
			if(!valueAudiosAudiosItemAudioTextsAudioTextsItem["SpeechRate"].isNull())
				audioTextsObject.speechRate = std::stoi(valueAudiosAudiosItemAudioTextsAudioTextsItem["SpeechRate"].asString());
			if(!valueAudiosAudiosItemAudioTextsAudioTextsItem["Confidence"].isNull())
				audioTextsObject.confidence = std::stof(valueAudiosAudiosItemAudioTextsAudioTextsItem["Confidence"].asString());
			if(!valueAudiosAudiosItemAudioTextsAudioTextsItem["Person"].isNull())
				audioTextsObject.person = valueAudiosAudiosItemAudioTextsAudioTextsItem["Person"].asString();
			if(!valueAudiosAudiosItemAudioTextsAudioTextsItem["Library"].isNull())
				audioTextsObject.library = valueAudiosAudiosItemAudioTextsAudioTextsItem["Library"].asString();
			audiosObject.audioTexts.push_back(audioTextsObject);
		}
		audios_.push_back(audiosObject);
	}
	if(!value["SetId"].isNull())
		setId_ = value["SetId"].asString();
	if(!value["VideoUri"].isNull())
		videoUri_ = value["VideoUri"].asString();
	if(!value["NextMarker"].isNull())
		nextMarker_ = value["NextMarker"].asString();

}

std::vector<ListVideoAudiosResult::AudiosItem> ListVideoAudiosResult::getAudios()const
{
	return audios_;
}

std::string ListVideoAudiosResult::getVideoUri()const
{
	return videoUri_;
}

std::string ListVideoAudiosResult::getSetId()const
{
	return setId_;
}

std::string ListVideoAudiosResult::getNextMarker()const
{
	return nextMarker_;
}

