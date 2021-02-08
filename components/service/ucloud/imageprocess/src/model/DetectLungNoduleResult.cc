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

#include <alibabacloud/imageprocess/model/DetectLungNoduleResult.h>
#include <json/json.h>

using namespace AlibabaCloud::Imageprocess;
using namespace AlibabaCloud::Imageprocess::Model;

DetectLungNoduleResult::DetectLungNoduleResult() :
	ServiceResult()
{}

DetectLungNoduleResult::DetectLungNoduleResult(const std::string &payload) :
	ServiceResult()
{
	parse(payload);
}

DetectLungNoduleResult::~DetectLungNoduleResult()
{}

void DetectLungNoduleResult::parse(const std::string &payload)
{
	Json::Reader reader;
	Json::Value value;
	reader.parse(payload, value);
	setRequestId(value["RequestId"].asString());
	auto dataNode = value["Data"];
	auto allSeriesNode = dataNode["Series"]["Serie"];
	for (auto dataNodeSeriesSerie : allSeriesNode)
	{
		Data::Serie serieObject;
		if(!dataNodeSeriesSerie["SeriesInstanceUid"].isNull())
			serieObject.seriesInstanceUid = dataNodeSeriesSerie["SeriesInstanceUid"].asString();
		if(!dataNodeSeriesSerie["Report"].isNull())
			serieObject.report = dataNodeSeriesSerie["Report"].asString();
		auto allElementsNode = dataNodeSeriesSerie["Elements"]["Element"];
		for (auto dataNodeSeriesSerieElementsElement : allElementsNode)
		{
			Data::Serie::Element elementsObject;
			if(!dataNodeSeriesSerieElementsElement["Category"].isNull())
				elementsObject.category = dataNodeSeriesSerieElementsElement["Category"].asString();
			if(!dataNodeSeriesSerieElementsElement["Confidence"].isNull())
				elementsObject.confidence = std::stof(dataNodeSeriesSerieElementsElement["Confidence"].asString());
			if(!dataNodeSeriesSerieElementsElement["Diameter"].isNull())
				elementsObject.diameter = std::stof(dataNodeSeriesSerieElementsElement["Diameter"].asString());
			if(!dataNodeSeriesSerieElementsElement["Lobe"].isNull())
				elementsObject.lobe = dataNodeSeriesSerieElementsElement["Lobe"].asString();
			if(!dataNodeSeriesSerieElementsElement["Lung"].isNull())
				elementsObject.lung = dataNodeSeriesSerieElementsElement["Lung"].asString();
			if(!dataNodeSeriesSerieElementsElement["X"].isNull())
				elementsObject.x = std::stof(dataNodeSeriesSerieElementsElement["X"].asString());
			if(!dataNodeSeriesSerieElementsElement["Z"].isNull())
				elementsObject.z = std::stof(dataNodeSeriesSerieElementsElement["Z"].asString());
			if(!dataNodeSeriesSerieElementsElement["Y"].isNull())
				elementsObject.y = std::stof(dataNodeSeriesSerieElementsElement["Y"].asString());
			if(!dataNodeSeriesSerieElementsElement["ImageX"].isNull())
				elementsObject.imageX = std::stof(dataNodeSeriesSerieElementsElement["ImageX"].asString());
			if(!dataNodeSeriesSerieElementsElement["ImageY"].isNull())
				elementsObject.imageY = std::stof(dataNodeSeriesSerieElementsElement["ImageY"].asString());
			if(!dataNodeSeriesSerieElementsElement["ImageZ"].isNull())
				elementsObject.imageZ = std::stof(dataNodeSeriesSerieElementsElement["ImageZ"].asString());
			if(!dataNodeSeriesSerieElementsElement["SOPInstanceUID"].isNull())
				elementsObject.sOPInstanceUID = dataNodeSeriesSerieElementsElement["SOPInstanceUID"].asString();
			if(!dataNodeSeriesSerieElementsElement["Volume"].isNull())
				elementsObject.volume = std::stof(dataNodeSeriesSerieElementsElement["Volume"].asString());
			if(!dataNodeSeriesSerieElementsElement["MeanValue"].isNull())
				elementsObject.meanValue = std::stof(dataNodeSeriesSerieElementsElement["MeanValue"].asString());
			serieObject.elements.push_back(elementsObject);
		}
		auto allOrigin = value["Origin"]["Origin"];
		for (auto value : allOrigin)
			serieObject.origin.push_back(value.asString());
		auto allSpacing = value["Spacing"]["Spacing"];
		for (auto value : allSpacing)
			serieObject.spacing.push_back(value.asString());
		data_.series.push_back(serieObject);
	}

}

DetectLungNoduleResult::Data DetectLungNoduleResult::getData()const
{
	return data_;
}

