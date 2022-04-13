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

#include <alibabacloud/facebody/model/CompareFaceResult.h>
#include <json/json.h>

using namespace AlibabaCloud::Facebody;
using namespace AlibabaCloud::Facebody::Model;

CompareFaceResult::CompareFaceResult() :
	ServiceResult()
{}

CompareFaceResult::CompareFaceResult(const std::string &payload) :
	ServiceResult()
{
	parse(payload);
}

CompareFaceResult::~CompareFaceResult()
{}

void CompareFaceResult::parse(const std::string &payload)
{
	Json::Reader reader;
	Json::Value value;
	reader.parse(payload, value);
	setRequestId(value["RequestId"].asString());
	auto dataNode = value["Data"];
	if(!dataNode["Confidence"].isNull())
		data_.confidence = std::stof(dataNode["Confidence"].asString());
		auto allThresholds = dataNode["Thresholds"];
		for (auto value : allThresholds)
			data_.thresholds.push_back(value.asDouble());
		auto allRectAList = dataNode["RectAList"];
		for (auto value : allRectAList)
			data_.rectAList.push_back(value.asDouble());
		auto allRectBList = dataNode["RectBList"];
		for (auto value : allRectBList)
			data_.rectBList.push_back(value.asDouble());

}

CompareFaceResult::Data CompareFaceResult::getData()const
{
	return data_;
}

