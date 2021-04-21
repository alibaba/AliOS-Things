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

#include <alibabacloud/imagerecog/model/RecognizeImageColorResult.h>
#include <json/json.h>

using namespace AlibabaCloud::Imagerecog;
using namespace AlibabaCloud::Imagerecog::Model;

RecognizeImageColorResult::RecognizeImageColorResult() :
	ServiceResult()
{}

RecognizeImageColorResult::RecognizeImageColorResult(const std::string &payload) :
	ServiceResult()
{
	parse(payload);
}

RecognizeImageColorResult::~RecognizeImageColorResult()
{}

void RecognizeImageColorResult::parse(const std::string &payload)
{
	Json::Reader reader;
	Json::Value value;
	reader.parse(payload, value);
	setRequestId(value["RequestId"].asString());
	auto dataNode = value["Data"];
	auto allColorTemplateListNode = dataNode["ColorTemplateList"]["ColorTemplate"];
	for (auto dataNodeColorTemplateListColorTemplate : allColorTemplateListNode)
	{
		Data::ColorTemplate colorTemplateObject;
		if(!dataNodeColorTemplateListColorTemplate["Color"].isNull())
			colorTemplateObject.color = dataNodeColorTemplateListColorTemplate["Color"].asString();
		if(!dataNodeColorTemplateListColorTemplate["Label"].isNull())
			colorTemplateObject.label = dataNodeColorTemplateListColorTemplate["Label"].asString();
		if(!dataNodeColorTemplateListColorTemplate["Percentage"].isNull())
			colorTemplateObject.percentage = std::stof(dataNodeColorTemplateListColorTemplate["Percentage"].asString());
		data_.colorTemplateList.push_back(colorTemplateObject);
	}

}

RecognizeImageColorResult::Data RecognizeImageColorResult::getData()const
{
	return data_;
}

