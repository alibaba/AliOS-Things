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

#include <alibabacloud/live/model/DescribeCasterScenesResult.h>
#include <json/json.h>

using namespace AlibabaCloud::Live;
using namespace AlibabaCloud::Live::Model;

DescribeCasterScenesResult::DescribeCasterScenesResult() :
	ServiceResult()
{}

DescribeCasterScenesResult::DescribeCasterScenesResult(const std::string &payload) :
	ServiceResult()
{
	parse(payload);
}

DescribeCasterScenesResult::~DescribeCasterScenesResult()
{}

void DescribeCasterScenesResult::parse(const std::string &payload)
{
	Json::Reader reader;
	Json::Value value;
	reader.parse(payload, value);
	setRequestId(value["RequestId"].asString());
	auto allSceneListNode = value["SceneList"]["Scene"];
	for (auto valueSceneListScene : allSceneListNode)
	{
		Scene sceneListObject;
		if(!valueSceneListScene["SceneId"].isNull())
			sceneListObject.sceneId = valueSceneListScene["SceneId"].asString();
		if(!valueSceneListScene["SceneName"].isNull())
			sceneListObject.sceneName = valueSceneListScene["SceneName"].asString();
		if(!valueSceneListScene["OutputType"].isNull())
			sceneListObject.outputType = valueSceneListScene["OutputType"].asString();
		if(!valueSceneListScene["LayoutId"].isNull())
			sceneListObject.layoutId = valueSceneListScene["LayoutId"].asString();
		if(!valueSceneListScene["StreamUrl"].isNull())
			sceneListObject.streamUrl = valueSceneListScene["StreamUrl"].asString();
		if(!valueSceneListScene["Status"].isNull())
			sceneListObject.status = std::stoi(valueSceneListScene["Status"].asString());
		auto allStreamInfosNode = allSceneListNode["StreamInfos"]["StreamInfo"];
		for (auto allSceneListNodeStreamInfosStreamInfo : allStreamInfosNode)
		{
			Scene::StreamInfo streamInfosObject;
			if(!allSceneListNodeStreamInfosStreamInfo["TranscodeConfig"].isNull())
				streamInfosObject.transcodeConfig = allSceneListNodeStreamInfosStreamInfo["TranscodeConfig"].asString();
			if(!allSceneListNodeStreamInfosStreamInfo["VideoFormat"].isNull())
				streamInfosObject.videoFormat = allSceneListNodeStreamInfosStreamInfo["VideoFormat"].asString();
			if(!allSceneListNodeStreamInfosStreamInfo["OutputStreamUrl"].isNull())
				streamInfosObject.outputStreamUrl = allSceneListNodeStreamInfosStreamInfo["OutputStreamUrl"].asString();
			sceneListObject.streamInfos.push_back(streamInfosObject);
		}
		auto allComponentIds = value["ComponentIds"]["componentId"];
		for (auto value : allComponentIds)
			sceneListObject.componentIds.push_back(value.asString());
		sceneList_.push_back(sceneListObject);
	}
	if(!value["Total"].isNull())
		total_ = std::stoi(value["Total"].asString());

}

int DescribeCasterScenesResult::getTotal()const
{
	return total_;
}

std::vector<DescribeCasterScenesResult::Scene> DescribeCasterScenesResult::getSceneList()const
{
	return sceneList_;
}

