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

#include <alibabacloud/live/model/StartCasterResult.h>
#include <json/json.h>

using namespace AlibabaCloud::Live;
using namespace AlibabaCloud::Live::Model;

StartCasterResult::StartCasterResult() :
	ServiceResult()
{}

StartCasterResult::StartCasterResult(const std::string &payload) :
	ServiceResult()
{
	parse(payload);
}

StartCasterResult::~StartCasterResult()
{}

void StartCasterResult::parse(const std::string &payload)
{
	Json::Reader reader;
	Json::Value value;
	reader.parse(payload, value);
	setRequestId(value["RequestId"].asString());
	auto allPvwSceneInfosNode = value["PvwSceneInfos"]["SceneInfo"];
	for (auto valuePvwSceneInfosSceneInfo : allPvwSceneInfosNode)
	{
		SceneInfo pvwSceneInfosObject;
		if(!valuePvwSceneInfosSceneInfo["SceneId"].isNull())
			pvwSceneInfosObject.sceneId = valuePvwSceneInfosSceneInfo["SceneId"].asString();
		if(!valuePvwSceneInfosSceneInfo["StreamUrl"].isNull())
			pvwSceneInfosObject.streamUrl = valuePvwSceneInfosSceneInfo["StreamUrl"].asString();
		pvwSceneInfos_.push_back(pvwSceneInfosObject);
	}
	auto allPgmSceneInfosNode = value["PgmSceneInfos"]["SceneInfo"];
	for (auto valuePgmSceneInfosSceneInfo : allPgmSceneInfosNode)
	{
		SceneInfo1 pgmSceneInfosObject;
		if(!valuePgmSceneInfosSceneInfo["SceneId"].isNull())
			pgmSceneInfosObject.sceneId = valuePgmSceneInfosSceneInfo["SceneId"].asString();
		if(!valuePgmSceneInfosSceneInfo["StreamUrl"].isNull())
			pgmSceneInfosObject.streamUrl = valuePgmSceneInfosSceneInfo["StreamUrl"].asString();
		auto allStreamInfosNode = allPgmSceneInfosNode["StreamInfos"]["StreamInfo"];
		for (auto allPgmSceneInfosNodeStreamInfosStreamInfo : allStreamInfosNode)
		{
			SceneInfo1::StreamInfo streamInfosObject;
			if(!allPgmSceneInfosNodeStreamInfosStreamInfo["TranscodeConfig"].isNull())
				streamInfosObject.transcodeConfig = allPgmSceneInfosNodeStreamInfosStreamInfo["TranscodeConfig"].asString();
			if(!allPgmSceneInfosNodeStreamInfosStreamInfo["VideoFormat"].isNull())
				streamInfosObject.videoFormat = allPgmSceneInfosNodeStreamInfosStreamInfo["VideoFormat"].asString();
			if(!allPgmSceneInfosNodeStreamInfosStreamInfo["OutputStreamUrl"].isNull())
				streamInfosObject.outputStreamUrl = allPgmSceneInfosNodeStreamInfosStreamInfo["OutputStreamUrl"].asString();
			pgmSceneInfosObject.streamInfos.push_back(streamInfosObject);
		}
		pgmSceneInfos_.push_back(pgmSceneInfosObject);
	}

}

std::vector<StartCasterResult::SceneInfo> StartCasterResult::getPvwSceneInfos()const
{
	return pvwSceneInfos_;
}

std::vector<StartCasterResult::SceneInfo1> StartCasterResult::getPgmSceneInfos()const
{
	return pgmSceneInfos_;
}

