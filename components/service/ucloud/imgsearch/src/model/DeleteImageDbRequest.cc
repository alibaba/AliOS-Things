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

#include <alibabacloud/imgsearch/model/DeleteImageDbRequest.h>

using AlibabaCloud::Imgsearch::Model::DeleteImageDbRequest;

DeleteImageDbRequest::DeleteImageDbRequest() :
	RpcServiceRequest("imgsearch", "2020-03-20", "DeleteImageDb")
{
	setMethod(HttpRequest::Method::Post);
}

DeleteImageDbRequest::~DeleteImageDbRequest()
{}

std::string DeleteImageDbRequest::getName()const
{
	return name_;
}

void DeleteImageDbRequest::setName(const std::string& name)
{
	name_ = name;
	setBodyParameter("Name", name);
}

