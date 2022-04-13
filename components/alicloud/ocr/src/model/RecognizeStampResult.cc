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

#include <alibabacloud/ocr/model/RecognizeStampResult.h>
#include <json/json.h>

using namespace AlibabaCloud::Ocr;
using namespace AlibabaCloud::Ocr::Model;

RecognizeStampResult::RecognizeStampResult() :
	ServiceResult()
{}

RecognizeStampResult::RecognizeStampResult(const std::string &payload) :
	ServiceResult()
{
	parse(payload);
}

RecognizeStampResult::~RecognizeStampResult()
{}

void RecognizeStampResult::parse(const std::string &payload)
{
	Json::Reader reader;
	Json::Value value;
	reader.parse(payload, value);
	setRequestId(value["RequestId"].asString());
	auto dataNode = value["Data"];
	auto allResultsNode = dataNode["Results"]["ResultsItem"];
	for (auto dataNodeResultsResultsItem : allResultsNode)
	{
		Data::ResultsItem resultsItemObject;
		auto allGeneralTextNode = dataNodeResultsResultsItem["GeneralText"]["GeneralTextItem"];
		for (auto dataNodeResultsResultsItemGeneralTextGeneralTextItem : allGeneralTextNode)
		{
			Data::ResultsItem::GeneralTextItem generalTextObject;
			if(!dataNodeResultsResultsItemGeneralTextGeneralTextItem["Content"].isNull())
				generalTextObject.content = dataNodeResultsResultsItemGeneralTextGeneralTextItem["Content"].asString();
			if(!dataNodeResultsResultsItemGeneralTextGeneralTextItem["Confidence"].isNull())
				generalTextObject.confidence = std::stof(dataNodeResultsResultsItemGeneralTextGeneralTextItem["Confidence"].asString());
			resultsItemObject.generalText.push_back(generalTextObject);
		}
		auto roiNode = value["Roi"];
		if(!roiNode["Left"].isNull())
			resultsItemObject.roi.left = std::stoi(roiNode["Left"].asString());
		if(!roiNode["Top"].isNull())
			resultsItemObject.roi.top = std::stoi(roiNode["Top"].asString());
		if(!roiNode["Width"].isNull())
			resultsItemObject.roi.width = std::stoi(roiNode["Width"].asString());
		if(!roiNode["Height"].isNull())
			resultsItemObject.roi.height = std::stoi(roiNode["Height"].asString());
		auto textNode = value["Text"];
		if(!textNode["Content"].isNull())
			resultsItemObject.text.content = textNode["Content"].asString();
		if(!textNode["Confidence"].isNull())
			resultsItemObject.text.confidence = std::stof(textNode["Confidence"].asString());
		data_.results.push_back(resultsItemObject);
	}

}

RecognizeStampResult::Data RecognizeStampResult::getData()const
{
	return data_;
}

