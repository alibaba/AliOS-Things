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

#include <alibabacloud/facebody/model/ListFaceEntitiesResult.h>
#include <json/json.h>

using namespace AlibabaCloud::Facebody;
using namespace AlibabaCloud::Facebody::Model;

ListFaceEntitiesResult::ListFaceEntitiesResult() :
	ServiceResult()
{}

ListFaceEntitiesResult::ListFaceEntitiesResult(const std::string &payload) :
	ServiceResult()
{
	parse(payload);
}

ListFaceEntitiesResult::~ListFaceEntitiesResult()
{}

void ListFaceEntitiesResult::parse(const std::string &payload)
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
	auto allEntitiesNode = dataNode["Entities"]["Entity"];
	for (auto dataNodeEntitiesEntity : allEntitiesNode)
	{
		Data::Entity entityObject;
		if(!dataNodeEntitiesEntity["DbName"].isNull())
			entityObject.dbName = dataNodeEntitiesEntity["DbName"].asString();
		if(!dataNodeEntitiesEntity["EntityId"].isNull())
			entityObject.entityId = dataNodeEntitiesEntity["EntityId"].asString();
		if(!dataNodeEntitiesEntity["FaceCount"].isNull())
			entityObject.faceCount = std::stoi(dataNodeEntitiesEntity["FaceCount"].asString());
		if(!dataNodeEntitiesEntity["Labels"].isNull())
			entityObject.labels = dataNodeEntitiesEntity["Labels"].asString();
		if(!dataNodeEntitiesEntity["CreatedAt"].isNull())
			entityObject.createdAt = std::stol(dataNodeEntitiesEntity["CreatedAt"].asString());
		if(!dataNodeEntitiesEntity["UpdatedAt"].isNull())
			entityObject.updatedAt = std::stol(dataNodeEntitiesEntity["UpdatedAt"].asString());
		data_.entities.push_back(entityObject);
	}

}

ListFaceEntitiesResult::Data ListFaceEntitiesResult::getData()const
{
	return data_;
}

