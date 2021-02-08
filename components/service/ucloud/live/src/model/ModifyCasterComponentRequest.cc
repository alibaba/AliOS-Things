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

#include <alibabacloud/live/model/ModifyCasterComponentRequest.h>

using AlibabaCloud::Live::Model::ModifyCasterComponentRequest;

ModifyCasterComponentRequest::ModifyCasterComponentRequest() :
	RpcServiceRequest("live", "2016-11-01", "ModifyCasterComponent")
{
	setMethod(HttpRequest::Method::Post);
}

ModifyCasterComponentRequest::~ModifyCasterComponentRequest()
{}

std::string ModifyCasterComponentRequest::getImageLayerContent()const
{
	return imageLayerContent_;
}

void ModifyCasterComponentRequest::setImageLayerContent(const std::string& imageLayerContent)
{
	imageLayerContent_ = imageLayerContent;
	setParameter("ImageLayerContent", imageLayerContent);
}

std::string ModifyCasterComponentRequest::getComponentName()const
{
	return componentName_;
}

void ModifyCasterComponentRequest::setComponentName(const std::string& componentName)
{
	componentName_ = componentName;
	setParameter("ComponentName", componentName);
}

std::string ModifyCasterComponentRequest::getComponentId()const
{
	return componentId_;
}

void ModifyCasterComponentRequest::setComponentId(const std::string& componentId)
{
	componentId_ = componentId;
	setParameter("ComponentId", componentId);
}

std::string ModifyCasterComponentRequest::getCasterId()const
{
	return casterId_;
}

void ModifyCasterComponentRequest::setCasterId(const std::string& casterId)
{
	casterId_ = casterId;
	setParameter("CasterId", casterId);
}

std::string ModifyCasterComponentRequest::getComponentLayer()const
{
	return componentLayer_;
}

void ModifyCasterComponentRequest::setComponentLayer(const std::string& componentLayer)
{
	componentLayer_ = componentLayer;
	setParameter("ComponentLayer", componentLayer);
}

long ModifyCasterComponentRequest::getOwnerId()const
{
	return ownerId_;
}

void ModifyCasterComponentRequest::setOwnerId(long ownerId)
{
	ownerId_ = ownerId;
	setParameter("OwnerId", std::to_string(ownerId));
}

std::string ModifyCasterComponentRequest::getComponentType()const
{
	return componentType_;
}

void ModifyCasterComponentRequest::setComponentType(const std::string& componentType)
{
	componentType_ = componentType;
	setParameter("ComponentType", componentType);
}

std::string ModifyCasterComponentRequest::getEffect()const
{
	return effect_;
}

void ModifyCasterComponentRequest::setEffect(const std::string& effect)
{
	effect_ = effect;
	setParameter("Effect", effect);
}

std::string ModifyCasterComponentRequest::getCaptionLayerContent()const
{
	return captionLayerContent_;
}

void ModifyCasterComponentRequest::setCaptionLayerContent(const std::string& captionLayerContent)
{
	captionLayerContent_ = captionLayerContent;
	setParameter("CaptionLayerContent", captionLayerContent);
}

std::string ModifyCasterComponentRequest::getTextLayerContent()const
{
	return textLayerContent_;
}

void ModifyCasterComponentRequest::setTextLayerContent(const std::string& textLayerContent)
{
	textLayerContent_ = textLayerContent;
	setParameter("TextLayerContent", textLayerContent);
}

