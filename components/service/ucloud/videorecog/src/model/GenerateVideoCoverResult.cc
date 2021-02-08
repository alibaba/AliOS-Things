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

#include <alibabacloud/videorecog/model/GenerateVideoCoverResult.h>
#include <json/json.h>

using namespace AlibabaCloud::Videorecog;
using namespace AlibabaCloud::Videorecog::Model;

GenerateVideoCoverResult::GenerateVideoCoverResult() :
	ServiceResult()
{}

GenerateVideoCoverResult::GenerateVideoCoverResult(const std::string &payload) :
	ServiceResult()
{
	parse(payload);
}

GenerateVideoCoverResult::~GenerateVideoCoverResult()
{}

void GenerateVideoCoverResult::parse(const std::string &payload)
{
	Json::Reader reader;
	Json::Value value;
	reader.parse(payload, value);
	setRequestId(value["RequestId"].asString());
	auto dataNode = value["Data"];
	auto allOutputsNode = dataNode["Outputs"]["Output"];
	for (auto dataNodeOutputsOutput : allOutputsNode)
	{
		Data::Output outputObject;
		if(!dataNodeOutputsOutput["ImageURL"].isNull())
			outputObject.imageURL = dataNodeOutputsOutput["ImageURL"].asString();
		if(!dataNodeOutputsOutput["Confidence"].isNull())
			outputObject.confidence = std::stof(dataNodeOutputsOutput["Confidence"].asString());
		data_.outputs.push_back(outputObject);
	}

}

GenerateVideoCoverResult::Data GenerateVideoCoverResult::getData()const
{
	return data_;
}

