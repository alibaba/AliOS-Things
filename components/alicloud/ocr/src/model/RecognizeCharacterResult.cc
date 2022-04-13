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

#include <alibabacloud/ocr/model/RecognizeCharacterResult.h>
#include <json/json.h>

using namespace AlibabaCloud::Ocr;
using namespace AlibabaCloud::Ocr::Model;

RecognizeCharacterResult::RecognizeCharacterResult() :
	ServiceResult()
{}

RecognizeCharacterResult::RecognizeCharacterResult(const std::string &payload) :
	ServiceResult()
{
	parse(payload);
}

RecognizeCharacterResult::~RecognizeCharacterResult()
{}

void RecognizeCharacterResult::parse(const std::string &payload)
{
	Json::Reader reader;
	Json::Value value;
	reader.parse(payload, value);
	setRequestId(value["RequestId"].asString());
	auto dataNode = value["Data"];
	auto allResultsNode = dataNode["Results"];
	for (auto dataNodeResultsResult : allResultsNode)
	{
		Data::Result resultObject;
		if(!dataNodeResultsResult["Probability"].isNull())
			resultObject.probability = std::stof(dataNodeResultsResult["Probability"].asString());
		if(!dataNodeResultsResult["Text"].isNull())
			resultObject.text = dataNodeResultsResult["Text"].asString();
		auto textRectanglesNode = dataNodeResultsResult["TextRectangles"];
		if(!textRectanglesNode["Angle"].isNull())
			resultObject.textRectangles.angle = std::stoi(textRectanglesNode["Angle"].asString());
		if(!textRectanglesNode["Left"].isNull())
			resultObject.textRectangles.left = std::stoi(textRectanglesNode["Left"].asString());
		if(!textRectanglesNode["Top"].isNull())
			resultObject.textRectangles.top = std::stoi(textRectanglesNode["Top"].asString());
		if(!textRectanglesNode["Width"].isNull())
			resultObject.textRectangles.width = std::stoi(textRectanglesNode["Width"].asString());
		if(!textRectanglesNode["Height"].isNull())
			resultObject.textRectangles.height = std::stoi(textRectanglesNode["Height"].asString());
		data_.results.push_back(resultObject);
	}

}

RecognizeCharacterResult::Data RecognizeCharacterResult::getData()const
{
	return data_;
}

