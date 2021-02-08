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

#include <alibabacloud/imgsearch/model/ListImagesResult.h>
#include <json/json.h>

using namespace AlibabaCloud::Imgsearch;
using namespace AlibabaCloud::Imgsearch::Model;

ListImagesResult::ListImagesResult() :
	ServiceResult()
{}

ListImagesResult::ListImagesResult(const std::string &payload) :
	ServiceResult()
{
	parse(payload);
}

ListImagesResult::~ListImagesResult()
{}

void ListImagesResult::parse(const std::string &payload)
{
	Json::Reader reader;
	Json::Value value;
	reader.parse(payload, value);
	setRequestId(value["RequestId"].asString());
	auto dataNode = value["Data"];
	if(!dataNode["Token"].isNull())
		data_.token = dataNode["Token"].asString();
	if(!dataNode["TotalCount"].isNull())
		data_.totalCount = std::stoi(dataNode["TotalCount"].asString());
	auto allImageListNode = dataNode["ImageList"]["ImageListItem"];
	for (auto dataNodeImageListImageListItem : allImageListNode)
	{
		Data::ImageListItem imageListItemObject;
		if(!dataNodeImageListImageListItem["DataId"].isNull())
			imageListItemObject.dataId = dataNodeImageListImageListItem["DataId"].asString();
		if(!dataNodeImageListImageListItem["ExtraData"].isNull())
			imageListItemObject.extraData = dataNodeImageListImageListItem["ExtraData"].asString();
		if(!dataNodeImageListImageListItem["EntityId"].isNull())
			imageListItemObject.entityId = dataNodeImageListImageListItem["EntityId"].asString();
		if(!dataNodeImageListImageListItem["UpdatedAt"].isNull())
			imageListItemObject.updatedAt = std::stol(dataNodeImageListImageListItem["UpdatedAt"].asString());
		if(!dataNodeImageListImageListItem["CreatedAt"].isNull())
			imageListItemObject.createdAt = std::stol(dataNodeImageListImageListItem["CreatedAt"].asString());
		data_.imageList.push_back(imageListItemObject);
	}

}

ListImagesResult::Data ListImagesResult::getData()const
{
	return data_;
}

