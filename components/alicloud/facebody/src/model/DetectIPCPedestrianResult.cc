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

#include <alibabacloud/facebody/model/DetectIPCPedestrianResult.h>
#include <json/json.h>

using namespace AlibabaCloud::Facebody;
using namespace AlibabaCloud::Facebody::Model;

DetectIPCPedestrianResult::DetectIPCPedestrianResult() :
	ServiceResult()
{}

DetectIPCPedestrianResult::DetectIPCPedestrianResult(const std::string &payload) :
	ServiceResult()
{
	parse(payload);
}

DetectIPCPedestrianResult::~DetectIPCPedestrianResult()
{}

void DetectIPCPedestrianResult::parse(const std::string &payload)
{
	Json::Reader reader;
	Json::Value value;
	reader.parse(payload, value);
	setRequestId(value["RequestId"].asString());
	auto dataNode = value["Data"];
	auto allImageInfoListNode = dataNode["ImageInfoList"]["ImageInfoListItem"];
	for (auto dataNodeImageInfoListImageInfoListItem : allImageInfoListNode)
	{
		Data::ImageInfoListItem imageInfoListItemObject;
		if(!dataNodeImageInfoListImageInfoListItem["ErrorCode"].isNull())
			imageInfoListItemObject.errorCode = dataNodeImageInfoListImageInfoListItem["ErrorCode"].asString();
		if(!dataNodeImageInfoListImageInfoListItem["ErrorMessage"].isNull())
			imageInfoListItemObject.errorMessage = dataNodeImageInfoListImageInfoListItem["ErrorMessage"].asString();
		if(!dataNodeImageInfoListImageInfoListItem["DataId"].isNull())
			imageInfoListItemObject.dataId = dataNodeImageInfoListImageInfoListItem["DataId"].asString();
		auto allElementsNode = dataNodeImageInfoListImageInfoListItem["Elements"]["Element"];
		for (auto dataNodeImageInfoListImageInfoListItemElementsElement : allElementsNode)
		{
			Data::ImageInfoListItem::Element elementsObject;
			if(!dataNodeImageInfoListImageInfoListItemElementsElement["Score"].isNull())
				elementsObject.score = std::stof(dataNodeImageInfoListImageInfoListItemElementsElement["Score"].asString());
			auto allBoxes = value["Boxes"]["Box"];
			for (auto value : allBoxes)
				elementsObject.boxes.push_back(value.asString());
			imageInfoListItemObject.elements.push_back(elementsObject);
		}
		data_.imageInfoList.push_back(imageInfoListItemObject);
	}

}

DetectIPCPedestrianResult::Data DetectIPCPedestrianResult::getData()const
{
	return data_;
}

