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

#include <alibabacloud/imm/model/ListVideosResult.h>
#include <json/json.h>

using namespace AlibabaCloud::Imm;
using namespace AlibabaCloud::Imm::Model;

ListVideosResult::ListVideosResult() :
	ServiceResult()
{}

ListVideosResult::ListVideosResult(const std::string &payload) :
	ServiceResult()
{
	parse(payload);
}

ListVideosResult::~ListVideosResult()
{}

void ListVideosResult::parse(const std::string &payload)
{
	Json::Reader reader;
	Json::Value value;
	reader.parse(payload, value);
	setRequestId(value["RequestId"].asString());
	auto allVideosNode = value["Videos"]["VideosItem"];
	for (auto valueVideosVideosItem : allVideosNode)
	{
		VideosItem videosObject;
		if(!valueVideosVideosItem["VideoUri"].isNull())
			videosObject.videoUri = valueVideosVideosItem["VideoUri"].asString();
		if(!valueVideosVideosItem["RemarksA"].isNull())
			videosObject.remarksA = valueVideosVideosItem["RemarksA"].asString();
		if(!valueVideosVideosItem["RemarksB"].isNull())
			videosObject.remarksB = valueVideosVideosItem["RemarksB"].asString();
		if(!valueVideosVideosItem["CreateTime"].isNull())
			videosObject.createTime = valueVideosVideosItem["CreateTime"].asString();
		if(!valueVideosVideosItem["ModifyTime"].isNull())
			videosObject.modifyTime = valueVideosVideosItem["ModifyTime"].asString();
		if(!valueVideosVideosItem["VideoWidth"].isNull())
			videosObject.videoWidth = std::stoi(valueVideosVideosItem["VideoWidth"].asString());
		if(!valueVideosVideosItem["VideoHeight"].isNull())
			videosObject.videoHeight = std::stoi(valueVideosVideosItem["VideoHeight"].asString());
		if(!valueVideosVideosItem["VideoFormat"].isNull())
			videosObject.videoFormat = valueVideosVideosItem["VideoFormat"].asString();
		if(!valueVideosVideosItem["VideoDuration"].isNull())
			videosObject.videoDuration = std::stof(valueVideosVideosItem["VideoDuration"].asString());
		if(!valueVideosVideosItem["FileSize"].isNull())
			videosObject.fileSize = std::stoi(valueVideosVideosItem["FileSize"].asString());
		if(!valueVideosVideosItem["VideoFrames"].isNull())
			videosObject.videoFrames = std::stoi(valueVideosVideosItem["VideoFrames"].asString());
		if(!valueVideosVideosItem["SourceType"].isNull())
			videosObject.sourceType = valueVideosVideosItem["SourceType"].asString();
		if(!valueVideosVideosItem["SourceUri"].isNull())
			videosObject.sourceUri = valueVideosVideosItem["SourceUri"].asString();
		if(!valueVideosVideosItem["SourcePosition"].isNull())
			videosObject.sourcePosition = valueVideosVideosItem["SourcePosition"].asString();
		if(!valueVideosVideosItem["ProcessStatus"].isNull())
			videosObject.processStatus = valueVideosVideosItem["ProcessStatus"].asString();
		if(!valueVideosVideosItem["ProcessModifyTime"].isNull())
			videosObject.processModifyTime = valueVideosVideosItem["ProcessModifyTime"].asString();
		if(!valueVideosVideosItem["VideoTagsStatus"].isNull())
			videosObject.videoTagsStatus = valueVideosVideosItem["VideoTagsStatus"].asString();
		if(!valueVideosVideosItem["VideoTagsModifyTime"].isNull())
			videosObject.videoTagsModifyTime = valueVideosVideosItem["VideoTagsModifyTime"].asString();
		if(!valueVideosVideosItem["CelebrityStatus"].isNull())
			videosObject.celebrityStatus = valueVideosVideosItem["CelebrityStatus"].asString();
		if(!valueVideosVideosItem["CelebrityModifyTime"].isNull())
			videosObject.celebrityModifyTime = valueVideosVideosItem["CelebrityModifyTime"].asString();
		if(!valueVideosVideosItem["CelebrityFailReason"].isNull())
			videosObject.celebrityFailReason = valueVideosVideosItem["CelebrityFailReason"].asString();
		if(!valueVideosVideosItem["VideoTagsFailReason"].isNull())
			videosObject.videoTagsFailReason = valueVideosVideosItem["VideoTagsFailReason"].asString();
		if(!valueVideosVideosItem["ProcessFailReason"].isNull())
			videosObject.processFailReason = valueVideosVideosItem["ProcessFailReason"].asString();
		if(!valueVideosVideosItem["RemarksC"].isNull())
			videosObject.remarksC = valueVideosVideosItem["RemarksC"].asString();
		if(!valueVideosVideosItem["RemarksD"].isNull())
			videosObject.remarksD = valueVideosVideosItem["RemarksD"].asString();
		if(!valueVideosVideosItem["ExternalId"].isNull())
			videosObject.externalId = valueVideosVideosItem["ExternalId"].asString();
		auto allVideoTagsNode = valueVideosVideosItem["VideoTags"]["VideoTagsItem"];
		for (auto valueVideosVideosItemVideoTagsVideoTagsItem : allVideoTagsNode)
		{
			VideosItem::VideoTagsItem videoTagsObject;
			if(!valueVideosVideosItemVideoTagsVideoTagsItem["TagName"].isNull())
				videoTagsObject.tagName = valueVideosVideosItemVideoTagsVideoTagsItem["TagName"].asString();
			if(!valueVideosVideosItemVideoTagsVideoTagsItem["ParentTagName"].isNull())
				videoTagsObject.parentTagName = valueVideosVideosItemVideoTagsVideoTagsItem["ParentTagName"].asString();
			if(!valueVideosVideosItemVideoTagsVideoTagsItem["TagConfidence"].isNull())
				videoTagsObject.tagConfidence = std::stof(valueVideosVideosItemVideoTagsVideoTagsItem["TagConfidence"].asString());
			if(!valueVideosVideosItemVideoTagsVideoTagsItem["TagLevel"].isNull())
				videoTagsObject.tagLevel = std::stoi(valueVideosVideosItemVideoTagsVideoTagsItem["TagLevel"].asString());
			videosObject.videoTags.push_back(videoTagsObject);
		}
		auto allCelebrityNode = valueVideosVideosItem["Celebrity"]["CelebrityItem"];
		for (auto valueVideosVideosItemCelebrityCelebrityItem : allCelebrityNode)
		{
			VideosItem::CelebrityItem celebrityObject;
			if(!valueVideosVideosItemCelebrityCelebrityItem["CelebrityName"].isNull())
				celebrityObject.celebrityName = valueVideosVideosItemCelebrityCelebrityItem["CelebrityName"].asString();
			if(!valueVideosVideosItemCelebrityCelebrityItem["CelebrityNum"].isNull())
				celebrityObject.celebrityNum = std::stoi(valueVideosVideosItemCelebrityCelebrityItem["CelebrityNum"].asString());
			if(!valueVideosVideosItemCelebrityCelebrityItem["CelebrityLibraryName"].isNull())
				celebrityObject.celebrityLibraryName = valueVideosVideosItemCelebrityCelebrityItem["CelebrityLibraryName"].asString();
			videosObject.celebrity.push_back(celebrityObject);
		}
		auto allPersonsNode = valueVideosVideosItem["Persons"]["PersonsItem"];
		for (auto valueVideosVideosItemPersonsPersonsItem : allPersonsNode)
		{
			VideosItem::PersonsItem personsObject;
			if(!valueVideosVideosItemPersonsPersonsItem["PersonId"].isNull())
				personsObject.personId = valueVideosVideosItemPersonsPersonsItem["PersonId"].asString();
			if(!valueVideosVideosItemPersonsPersonsItem["Age"].isNull())
				personsObject.age = std::stoi(valueVideosVideosItemPersonsPersonsItem["Age"].asString());
			if(!valueVideosVideosItemPersonsPersonsItem["AgeConfidence"].isNull())
				personsObject.ageConfidence = std::stof(valueVideosVideosItemPersonsPersonsItem["AgeConfidence"].asString());
			if(!valueVideosVideosItemPersonsPersonsItem["Gender"].isNull())
				personsObject.gender = valueVideosVideosItemPersonsPersonsItem["Gender"].asString();
			if(!valueVideosVideosItemPersonsPersonsItem["GenderConfidence"].isNull())
				personsObject.genderConfidence = std::stof(valueVideosVideosItemPersonsPersonsItem["GenderConfidence"].asString());
			videosObject.persons.push_back(personsObject);
		}
		videos_.push_back(videosObject);
	}
	if(!value["SetId"].isNull())
		setId_ = value["SetId"].asString();
	if(!value["NextMarker"].isNull())
		nextMarker_ = value["NextMarker"].asString();

}

std::string ListVideosResult::getSetId()const
{
	return setId_;
}

std::string ListVideosResult::getNextMarker()const
{
	return nextMarker_;
}

std::vector<ListVideosResult::VideosItem> ListVideosResult::getVideos()const
{
	return videos_;
}

