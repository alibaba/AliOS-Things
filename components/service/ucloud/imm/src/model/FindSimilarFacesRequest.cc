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

#include <alibabacloud/imm/model/FindSimilarFacesRequest.h>

using AlibabaCloud::Imm::Model::FindSimilarFacesRequest;

FindSimilarFacesRequest::FindSimilarFacesRequest() :
	RpcServiceRequest("imm", "2017-09-06", "FindSimilarFaces")
{
	setMethod(HttpRequest::Method::Post);
}

FindSimilarFacesRequest::~FindSimilarFacesRequest()
{}

std::string FindSimilarFacesRequest::getProject()const
{
	return project_;
}

void FindSimilarFacesRequest::setProject(const std::string& project)
{
	project_ = project;
	setParameter("Project", project);
}

std::string FindSimilarFacesRequest::getAccessKeyId()const
{
	return accessKeyId_;
}

void FindSimilarFacesRequest::setAccessKeyId(const std::string& accessKeyId)
{
	accessKeyId_ = accessKeyId;
	setParameter("AccessKeyId", accessKeyId);
}

float FindSimilarFacesRequest::getMinSimilarity()const
{
	return minSimilarity_;
}

void FindSimilarFacesRequest::setMinSimilarity(float minSimilarity)
{
	minSimilarity_ = minSimilarity;
	setParameter("MinSimilarity", std::to_string(minSimilarity));
}

std::string FindSimilarFacesRequest::getResponseFormat()const
{
	return responseFormat_;
}

void FindSimilarFacesRequest::setResponseFormat(const std::string& responseFormat)
{
	responseFormat_ = responseFormat;
	setParameter("ResponseFormat", responseFormat);
}

int FindSimilarFacesRequest::getLimit()const
{
	return limit_;
}

void FindSimilarFacesRequest::setLimit(int limit)
{
	limit_ = limit;
	setParameter("Limit", std::to_string(limit));
}

std::string FindSimilarFacesRequest::getFaceId()const
{
	return faceId_;
}

void FindSimilarFacesRequest::setFaceId(const std::string& faceId)
{
	faceId_ = faceId;
	setParameter("FaceId", faceId);
}

std::string FindSimilarFacesRequest::getImageUri()const
{
	return imageUri_;
}

void FindSimilarFacesRequest::setImageUri(const std::string& imageUri)
{
	imageUri_ = imageUri;
	setParameter("ImageUri", imageUri);
}

std::string FindSimilarFacesRequest::getSetId()const
{
	return setId_;
}

void FindSimilarFacesRequest::setSetId(const std::string& setId)
{
	setId_ = setId;
	setParameter("SetId", setId);
}

