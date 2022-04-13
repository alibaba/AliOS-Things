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

#include <alibabacloud/facebody/model/BodyPostureResult.h>
#include <json/json.h>

using namespace AlibabaCloud::Facebody;
using namespace AlibabaCloud::Facebody::Model;

BodyPostureResult::BodyPostureResult() :
	ServiceResult()
{}

BodyPostureResult::BodyPostureResult(const std::string &payload) :
	ServiceResult()
{
	parse(payload);
}

BodyPostureResult::~BodyPostureResult()
{}

void BodyPostureResult::parse(const std::string &payload)
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
		if(!dataNodeOutputsOutput["HumanCount"].isNull())
			outputObject.humanCount = std::stoi(dataNodeOutputsOutput["HumanCount"].asString());
		auto allResultsNode = dataNodeOutputsOutput["Results"]["Result"];
		for (auto dataNodeOutputsOutputResultsResult : allResultsNode)
		{
			Data::Output::Result resultsObject;
			auto allBodiesNode = dataNodeOutputsOutputResultsResult["Bodies"]["Body"];
			for (auto dataNodeOutputsOutputResultsResultBodiesBody : allBodiesNode)
			{
				Data::Output::Result::Body bodiesObject;
				if(!dataNodeOutputsOutputResultsResultBodiesBody["Confident"].isNull())
					bodiesObject.confident = std::stof(dataNodeOutputsOutputResultsResultBodiesBody["Confident"].asString());
				if(!dataNodeOutputsOutputResultsResultBodiesBody["Label"].isNull())
					bodiesObject.label = dataNodeOutputsOutputResultsResultBodiesBody["Label"].asString();
				auto allPositionsNode = dataNodeOutputsOutputResultsResultBodiesBody["Positions"]["Position"];
				for (auto dataNodeOutputsOutputResultsResultBodiesBodyPositionsPosition : allPositionsNode)
				{
					Data::Output::Result::Body::Position positionsObject;
					auto allPoints = value["Points"]["Point"];
					for (auto value : allPoints)
						positionsObject.points.push_back(value.asString());
					bodiesObject.positions.push_back(positionsObject);
				}
				resultsObject.bodies.push_back(bodiesObject);
			}
			outputObject.results.push_back(resultsObject);
		}
		data_.outputs.push_back(outputObject);
	}
	auto metaObjectNode = dataNode["MetaObject"];
	if(!metaObjectNode["Height"].isNull())
		data_.metaObject.height = std::stoi(metaObjectNode["Height"].asString());
	if(!metaObjectNode["Width"].isNull())
		data_.metaObject.width = std::stoi(metaObjectNode["Width"].asString());

}

BodyPostureResult::Data BodyPostureResult::getData()const
{
	return data_;
}

