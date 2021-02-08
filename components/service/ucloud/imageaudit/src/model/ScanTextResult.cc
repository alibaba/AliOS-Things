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

#include <alibabacloud/imageaudit/model/ScanTextResult.h>
#include <json/json.h>

using namespace AlibabaCloud::Imageaudit;
using namespace AlibabaCloud::Imageaudit::Model;

ScanTextResult::ScanTextResult() :
	ServiceResult()
{}

ScanTextResult::ScanTextResult(const std::string &payload) :
	ServiceResult()
{
	parse(payload);
}

ScanTextResult::~ScanTextResult()
{}

void ScanTextResult::parse(const std::string &payload)
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
			auto allDetailsNode = dataNodeElementsElementResultsResult["Details"]["Detail"];
			for (auto dataNodeElementsElementResultsResultDetailsDetail : allDetailsNode)
			{
				Data::Element::Result::Detail detailsObject;
				if(!dataNodeElementsElementResultsResultDetailsDetail["Label"].isNull())
					detailsObject.label = dataNodeElementsElementResultsResultDetailsDetail["Label"].asString();
				auto allContextsNode = dataNodeElementsElementResultsResultDetailsDetail["Contexts"]["Context"];
				for (auto dataNodeElementsElementResultsResultDetailsDetailContextsContext : allContextsNode)
				{
					Data::Element::Result::Detail::Context contextsObject;
					if(!dataNodeElementsElementResultsResultDetailsDetailContextsContext["Context"].isNull())
						contextsObject.context = dataNodeElementsElementResultsResultDetailsDetailContextsContext["Context"].asString();
					detailsObject.contexts.push_back(contextsObject);
				}
				resultsObject.details.push_back(detailsObject);
			}
			elementObject.results.push_back(resultsObject);
		}
		data_.elements.push_back(elementObject);
	}

}

ScanTextResult::Data ScanTextResult::getData()const
{
	return data_;
}

