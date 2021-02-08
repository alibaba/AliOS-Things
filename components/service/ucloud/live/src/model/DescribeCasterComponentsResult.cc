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

#include <alibabacloud/live/model/DescribeCasterComponentsResult.h>
#include <json/json.h>

using namespace AlibabaCloud::Live;
using namespace AlibabaCloud::Live::Model;

DescribeCasterComponentsResult::DescribeCasterComponentsResult() :
	ServiceResult()
{}

DescribeCasterComponentsResult::DescribeCasterComponentsResult(const std::string &payload) :
	ServiceResult()
{
	parse(payload);
}

DescribeCasterComponentsResult::~DescribeCasterComponentsResult()
{}

void DescribeCasterComponentsResult::parse(const std::string &payload)
{
	Json::Reader reader;
	Json::Value value;
	reader.parse(payload, value);
	setRequestId(value["RequestId"].asString());
	auto allComponentsNode = value["Components"]["Component"];
	for (auto valueComponentsComponent : allComponentsNode)
	{
		Component componentsObject;
		if(!valueComponentsComponent["ComponentId"].isNull())
			componentsObject.componentId = valueComponentsComponent["ComponentId"].asString();
		if(!valueComponentsComponent["ComponentName"].isNull())
			componentsObject.componentName = valueComponentsComponent["ComponentName"].asString();
		if(!valueComponentsComponent["LocationId"].isNull())
			componentsObject.locationId = valueComponentsComponent["LocationId"].asString();
		if(!valueComponentsComponent["ComponentType"].isNull())
			componentsObject.componentType = valueComponentsComponent["ComponentType"].asString();
		if(!valueComponentsComponent["Effect"].isNull())
			componentsObject.effect = valueComponentsComponent["Effect"].asString();
		auto componentLayerNode = value["ComponentLayer"];
		if(!componentLayerNode["HeightNormalized"].isNull())
			componentsObject.componentLayer.heightNormalized = std::stof(componentLayerNode["HeightNormalized"].asString());
		if(!componentLayerNode["WidthNormalized"].isNull())
			componentsObject.componentLayer.widthNormalized = std::stof(componentLayerNode["WidthNormalized"].asString());
		if(!componentLayerNode["PositionRefer"].isNull())
			componentsObject.componentLayer.positionRefer = componentLayerNode["PositionRefer"].asString();
		if(!componentLayerNode["Transparency"].isNull())
			componentsObject.componentLayer.transparency = std::stoi(componentLayerNode["Transparency"].asString());
			auto allPositionNormalizeds = componentLayerNode["PositionNormalizeds"]["Position"];
			for (auto value : allPositionNormalizeds)
				componentsObject.componentLayer.positionNormalizeds.push_back(value.asString());
		auto textLayerContentNode = value["TextLayerContent"];
		if(!textLayerContentNode["Text"].isNull())
			componentsObject.textLayerContent.text = textLayerContentNode["Text"].asString();
		if(!textLayerContentNode["Color"].isNull())
			componentsObject.textLayerContent.color = textLayerContentNode["Color"].asString();
		if(!textLayerContentNode["FontName"].isNull())
			componentsObject.textLayerContent.fontName = textLayerContentNode["FontName"].asString();
		if(!textLayerContentNode["SizeNormalized"].isNull())
			componentsObject.textLayerContent.sizeNormalized = std::stof(textLayerContentNode["SizeNormalized"].asString());
		if(!textLayerContentNode["BorderWidthNormalized"].isNull())
			componentsObject.textLayerContent.borderWidthNormalized = std::stof(textLayerContentNode["BorderWidthNormalized"].asString());
		if(!textLayerContentNode["BorderColor"].isNull())
			componentsObject.textLayerContent.borderColor = textLayerContentNode["BorderColor"].asString();
		auto imageLayerContentNode = value["ImageLayerContent"];
		if(!imageLayerContentNode["MaterialId"].isNull())
			componentsObject.imageLayerContent.materialId = imageLayerContentNode["MaterialId"].asString();
		auto captionLayerContentNode = value["CaptionLayerContent"];
		if(!captionLayerContentNode["LocationId"].isNull())
			componentsObject.captionLayerContent.locationId = captionLayerContentNode["LocationId"].asString();
		if(!captionLayerContentNode["PtsOffset"].isNull())
			componentsObject.captionLayerContent.ptsOffset = std::stoi(captionLayerContentNode["PtsOffset"].asString());
		if(!captionLayerContentNode["WordsCount"].isNull())
			componentsObject.captionLayerContent.wordsCount = std::stoi(captionLayerContentNode["WordsCount"].asString());
		if(!captionLayerContentNode["Color"].isNull())
			componentsObject.captionLayerContent.color = captionLayerContentNode["Color"].asString();
		if(!captionLayerContentNode["FontName"].isNull())
			componentsObject.captionLayerContent.fontName = captionLayerContentNode["FontName"].asString();
		if(!captionLayerContentNode["SizeNormalized"].isNull())
			componentsObject.captionLayerContent.sizeNormalized = std::stof(captionLayerContentNode["SizeNormalized"].asString());
		if(!captionLayerContentNode["BorderWidthNormalized"].isNull())
			componentsObject.captionLayerContent.borderWidthNormalized = std::stof(captionLayerContentNode["BorderWidthNormalized"].asString());
		if(!captionLayerContentNode["BorderColor"].isNull())
			componentsObject.captionLayerContent.borderColor = captionLayerContentNode["BorderColor"].asString();
		if(!captionLayerContentNode["WordCountPerLine"].isNull())
			componentsObject.captionLayerContent.wordCountPerLine = std::stoi(captionLayerContentNode["WordCountPerLine"].asString());
		if(!captionLayerContentNode["WordSpaceNormalized"].isNull())
			componentsObject.captionLayerContent.wordSpaceNormalized = std::stof(captionLayerContentNode["WordSpaceNormalized"].asString());
		if(!captionLayerContentNode["LineSpaceNormalized"].isNull())
			componentsObject.captionLayerContent.lineSpaceNormalized = std::stof(captionLayerContentNode["LineSpaceNormalized"].asString());
		components_.push_back(componentsObject);
	}
	if(!value["Total"].isNull())
		total_ = std::stoi(value["Total"].asString());

}

std::vector<DescribeCasterComponentsResult::Component> DescribeCasterComponentsResult::getComponents()const
{
	return components_;
}

int DescribeCasterComponentsResult::getTotal()const
{
	return total_;
}

