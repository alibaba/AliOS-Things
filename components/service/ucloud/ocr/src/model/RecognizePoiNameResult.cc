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

#include <alibabacloud/ocr/model/RecognizePoiNameResult.h>
#include <json/json.h>

using namespace AlibabaCloud::Ocr;
using namespace AlibabaCloud::Ocr::Model;

RecognizePoiNameResult::RecognizePoiNameResult() :
	ServiceResult()
{}

RecognizePoiNameResult::RecognizePoiNameResult(const std::string &payload) :
	ServiceResult()
{
	parse(payload);
}

RecognizePoiNameResult::~RecognizePoiNameResult()
{}

void RecognizePoiNameResult::parse(const std::string &payload)
{
	Json::Reader reader;
	Json::Value value;
	reader.parse(payload, value);
	setRequestId(value["RequestId"].asString());
	auto dataNode = value["Data"];
	auto allSignboardsNode = dataNode["Signboards"]["SignboardsItem"];
	for (auto dataNodeSignboardsSignboardsItem : allSignboardsNode)
	{
		Data::SignboardsItem signboardsItemObject;
		auto allTextsNode = dataNodeSignboardsSignboardsItem["Texts"]["TextsItem"];
		for (auto dataNodeSignboardsSignboardsItemTextsTextsItem : allTextsNode)
		{
			Data::SignboardsItem::TextsItem textsObject;
			if(!dataNodeSignboardsSignboardsItemTextsTextsItem["Label"].isNull())
				textsObject.label = dataNodeSignboardsSignboardsItemTextsTextsItem["Label"].asString();
			if(!dataNodeSignboardsSignboardsItemTextsTextsItem["Score"].isNull())
				textsObject.score = std::stof(dataNodeSignboardsSignboardsItemTextsTextsItem["Score"].asString());
			if(!dataNodeSignboardsSignboardsItemTextsTextsItem["Tag"].isNull())
				textsObject.tag = dataNodeSignboardsSignboardsItemTextsTextsItem["Tag"].asString();
			if(!dataNodeSignboardsSignboardsItemTextsTextsItem["Type"].isNull())
				textsObject.type = dataNodeSignboardsSignboardsItemTextsTextsItem["Type"].asString();
			auto allPoints = value["Points"]["Points"];
			for (auto value : allPoints)
				textsObject.points.push_back(value.asString());
			signboardsItemObject.texts.push_back(textsObject);
		}
		data_.signboards.push_back(signboardsItemObject);
	}
	auto summaryNode = dataNode["Summary"];
	if(!summaryNode["Brand"].isNull())
		data_.summary.brand = summaryNode["Brand"].asString();
	if(!summaryNode["Score"].isNull())
		data_.summary.score = std::stof(summaryNode["Score"].asString());

}

RecognizePoiNameResult::Data RecognizePoiNameResult::getData()const
{
	return data_;
}

