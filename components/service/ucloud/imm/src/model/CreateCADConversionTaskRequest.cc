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

#include <alibabacloud/imm/model/CreateCADConversionTaskRequest.h>

using AlibabaCloud::Imm::Model::CreateCADConversionTaskRequest;

CreateCADConversionTaskRequest::CreateCADConversionTaskRequest() :
	RpcServiceRequest("imm", "2017-09-06", "CreateCADConversionTask")
{
	setMethod(HttpRequest::Method::Post);
}

CreateCADConversionTaskRequest::~CreateCADConversionTaskRequest()
{}

std::string CreateCADConversionTaskRequest::getSrcType()const
{
	return srcType_;
}

void CreateCADConversionTaskRequest::setSrcType(const std::string& srcType)
{
	srcType_ = srcType;
	setParameter("SrcType", srcType);
}

int CreateCADConversionTaskRequest::getBaseRow()const
{
	return baseRow_;
}

void CreateCADConversionTaskRequest::setBaseRow(int baseRow)
{
	baseRow_ = baseRow;
	setParameter("BaseRow", std::to_string(baseRow));
}

std::string CreateCADConversionTaskRequest::getProject()const
{
	return project_;
}

void CreateCADConversionTaskRequest::setProject(const std::string& project)
{
	project_ = project;
	setParameter("Project", project);
}

int CreateCADConversionTaskRequest::getZoomFactor()const
{
	return zoomFactor_;
}

void CreateCADConversionTaskRequest::setZoomFactor(int zoomFactor)
{
	zoomFactor_ = zoomFactor;
	setParameter("ZoomFactor", std::to_string(zoomFactor));
}

std::string CreateCADConversionTaskRequest::getAccessKeyId()const
{
	return accessKeyId_;
}

void CreateCADConversionTaskRequest::setAccessKeyId(const std::string& accessKeyId)
{
	accessKeyId_ = accessKeyId;
	setParameter("AccessKeyId", accessKeyId);
}

std::string CreateCADConversionTaskRequest::getNotifyEndpoint()const
{
	return notifyEndpoint_;
}

void CreateCADConversionTaskRequest::setNotifyEndpoint(const std::string& notifyEndpoint)
{
	notifyEndpoint_ = notifyEndpoint;
	setParameter("NotifyEndpoint", notifyEndpoint);
}

int CreateCADConversionTaskRequest::getBaseCol()const
{
	return baseCol_;
}

void CreateCADConversionTaskRequest::setBaseCol(int baseCol)
{
	baseCol_ = baseCol;
	setParameter("BaseCol", std::to_string(baseCol));
}

std::string CreateCADConversionTaskRequest::getNotifyTopicName()const
{
	return notifyTopicName_;
}

void CreateCADConversionTaskRequest::setNotifyTopicName(const std::string& notifyTopicName)
{
	notifyTopicName_ = notifyTopicName;
	setParameter("NotifyTopicName", notifyTopicName);
}

long CreateCADConversionTaskRequest::getUnitWidth()const
{
	return unitWidth_;
}

void CreateCADConversionTaskRequest::setUnitWidth(long unitWidth)
{
	unitWidth_ = unitWidth;
	setParameter("UnitWidth", std::to_string(unitWidth));
}

int CreateCADConversionTaskRequest::getZoomLevel()const
{
	return zoomLevel_;
}

void CreateCADConversionTaskRequest::setZoomLevel(int zoomLevel)
{
	zoomLevel_ = zoomLevel;
	setParameter("ZoomLevel", std::to_string(zoomLevel));
}

std::string CreateCADConversionTaskRequest::getModelId()const
{
	return modelId_;
}

void CreateCADConversionTaskRequest::setModelId(const std::string& modelId)
{
	modelId_ = modelId;
	setParameter("ModelId", modelId);
}

std::string CreateCADConversionTaskRequest::getTgtType()const
{
	return tgtType_;
}

void CreateCADConversionTaskRequest::setTgtType(const std::string& tgtType)
{
	tgtType_ = tgtType;
	setParameter("TgtType", tgtType);
}

long CreateCADConversionTaskRequest::getUnitHeight()const
{
	return unitHeight_;
}

void CreateCADConversionTaskRequest::setUnitHeight(long unitHeight)
{
	unitHeight_ = unitHeight;
	setParameter("UnitHeight", std::to_string(unitHeight));
}

std::string CreateCADConversionTaskRequest::getSrcUri()const
{
	return srcUri_;
}

void CreateCADConversionTaskRequest::setSrcUri(const std::string& srcUri)
{
	srcUri_ = srcUri;
	setParameter("SrcUri", srcUri);
}

bool CreateCADConversionTaskRequest::getThumbnails()const
{
	return thumbnails_;
}

void CreateCADConversionTaskRequest::setThumbnails(bool thumbnails)
{
	thumbnails_ = thumbnails;
	setParameter("Thumbnails", thumbnails ? "true" : "false");
}

std::string CreateCADConversionTaskRequest::getTgtUri()const
{
	return tgtUri_;
}

void CreateCADConversionTaskRequest::setTgtUri(const std::string& tgtUri)
{
	tgtUri_ = tgtUri;
	setParameter("TgtUri", tgtUri);
}

