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

#include <alibabacloud/facebody/model/RecognizePublicFaceResult.h>
#include <json/json.h>

using namespace AlibabaCloud::Facebody;
using namespace AlibabaCloud::Facebody::Model;

RecognizePublicFaceResult::RecognizePublicFaceResult() :
	ServiceResult()
{}

RecognizePublicFaceResult::RecognizePublicFaceResult(const std::string &payload) :
	ServiceResult()
{
	parse(payload);
}

RecognizePublicFaceResult::~RecognizePublicFaceResult()
{}

void RecognizePublicFaceResult::parse(const std::string &payload)
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
			auto allSubResultsNode = dataNodeElementsElementResultsResult["SubResults"]["SubResult"];
			for (auto dataNodeElementsElementResultsResultSubResultsSubResult : allSubResultsNode)
			{
				Data::Element::Result::SubResult subResultsObject;
				if(!dataNodeElementsElementResultsResultSubResultsSubResult["H"].isNull())
					subResultsObject.h = std::stof(dataNodeElementsElementResultsResultSubResultsSubResult["H"].asString());
				if(!dataNodeElementsElementResultsResultSubResultsSubResult["W"].isNull())
					subResultsObject.w = std::stof(dataNodeElementsElementResultsResultSubResultsSubResult["W"].asString());
				if(!dataNodeElementsElementResultsResultSubResultsSubResult["X"].isNull())
					subResultsObject.x = std::stof(dataNodeElementsElementResultsResultSubResultsSubResult["X"].asString());
				if(!dataNodeElementsElementResultsResultSubResultsSubResult["Y"].isNull())
					subResultsObject.y = std::stof(dataNodeElementsElementResultsResultSubResultsSubResult["Y"].asString());
				auto allFacesNode = dataNodeElementsElementResultsResultSubResultsSubResult["Faces"]["Face"];
				for (auto dataNodeElementsElementResultsResultSubResultsSubResultFacesFace : allFacesNode)
				{
					Data::Element::Result::SubResult::Face facesObject;
					if(!dataNodeElementsElementResultsResultSubResultsSubResultFacesFace["Id"].isNull())
						facesObject.id = dataNodeElementsElementResultsResultSubResultsSubResultFacesFace["Id"].asString();
					if(!dataNodeElementsElementResultsResultSubResultsSubResultFacesFace["Name"].isNull())
						facesObject.name = dataNodeElementsElementResultsResultSubResultsSubResultFacesFace["Name"].asString();
					if(!dataNodeElementsElementResultsResultSubResultsSubResultFacesFace["Rate"].isNull())
						facesObject.rate = std::stof(dataNodeElementsElementResultsResultSubResultsSubResultFacesFace["Rate"].asString());
					subResultsObject.faces.push_back(facesObject);
				}
				resultsObject.subResults.push_back(subResultsObject);
			}
			elementObject.results.push_back(resultsObject);
		}
		data_.elements.push_back(elementObject);
	}

}

RecognizePublicFaceResult::Data RecognizePublicFaceResult::getData()const
{
	return data_;
}

