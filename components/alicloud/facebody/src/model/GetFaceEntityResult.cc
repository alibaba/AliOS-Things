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

#include <alibabacloud/facebody/model/GetFaceEntityResult.h>
#include <json/json.h>

using namespace AlibabaCloud::Facebody;
using namespace AlibabaCloud::Facebody::Model;

GetFaceEntityResult::GetFaceEntityResult() :
	ServiceResult()
{}

GetFaceEntityResult::GetFaceEntityResult(const std::string &payload) :
	ServiceResult()
{
	parse(payload);
}

GetFaceEntityResult::~GetFaceEntityResult()
{}

void GetFaceEntityResult::parse(const std::string &payload)
{
	Json::Reader reader;
	Json::Value value;
	reader.parse(payload, value);
	setRequestId(value["RequestId"].asString());
	auto dataNode = value["Data"];
	if(!dataNode["DbName"].isNull())
		data_.dbName = dataNode["DbName"].asString();
	if(!dataNode["EntityId"].isNull())
		data_.entityId = dataNode["EntityId"].asString();
	if(!dataNode["Labels"].isNull())
		data_.labels = dataNode["Labels"].asString();
	auto allFacesNode = dataNode["Faces"]["Face"];
	for (auto dataNodeFacesFace : allFacesNode)
	{
		Data::Face faceObject;
		if(!dataNodeFacesFace["FaceId"].isNull())
			faceObject.faceId = dataNodeFacesFace["FaceId"].asString();
		data_.faces.push_back(faceObject);
	}

}

GetFaceEntityResult::Data GetFaceEntityResult::getData()const
{
	return data_;
}

