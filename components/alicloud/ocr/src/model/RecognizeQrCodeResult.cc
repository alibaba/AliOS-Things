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

#include <alibabacloud/ocr/model/RecognizeQrCodeResult.h>
#include <json/json.h>

using namespace AlibabaCloud::Ocr;
using namespace AlibabaCloud::Ocr::Model;

RecognizeQrCodeResult::RecognizeQrCodeResult() :
	ServiceResult()
{}

RecognizeQrCodeResult::RecognizeQrCodeResult(const std::string &payload) :
	ServiceResult()
{
	parse(payload);
}

RecognizeQrCodeResult::~RecognizeQrCodeResult()
{}

void RecognizeQrCodeResult::parse(const std::string &payload)
{
	Json::Reader reader;
	Json::Value value;
	reader.parse(payload, value);
	setRequestId(value["RequestId"].asString());
	auto dataNode = value["Data"];
	auto allElementsNode = dataNode["Elements"]["Element"];
	for (auto dataNodeElementsElement : allElementsNode)
	{
		Data::Element elementObject;
		if(!dataNodeElementsElement["TaskId"].isNull())
			elementObject.taskId = dataNodeElementsElement["TaskId"].asString();
		if(!dataNodeElementsElement["ImageURL"].isNull())
			elementObject.imageURL = dataNodeElementsElement["ImageURL"].asString();
		auto allResultsNode = dataNodeElementsElement["Results"]["Result"];
		for (auto dataNodeElementsElementResultsResult : allResultsNode)
		{
			Data::Element::Result resultsObject;
			if(!dataNodeElementsElementResultsResult["Label"].isNull())
				resultsObject.label = dataNodeElementsElementResultsResult["Label"].asString();
			if(!dataNodeElementsElementResultsResult["Suggestion"].isNull())
				resultsObject.suggestion = dataNodeElementsElementResultsResult["Suggestion"].asString();
			if(!dataNodeElementsElementResultsResult["Rate"].isNull())
				resultsObject.rate = std::stof(dataNodeElementsElementResultsResult["Rate"].asString());
			auto allQrCodesData = value["QrCodesData"]["QrCodeData"];
			for (auto value : allQrCodesData)
				resultsObject.qrCodesData.push_back(value.asString());
			elementObject.results.push_back(resultsObject);
		}
		data_.elements.push_back(elementObject);
	}

}

RecognizeQrCodeResult::Data RecognizeQrCodeResult::getData()const
{
	return data_;
}

