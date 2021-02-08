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

#include <alibabacloud/live/model/AddCasterComponentRequest.h>

using AlibabaCloud::Live::Model::AddCasterComponentRequest;

AddCasterComponentRequest::AddCasterComponentRequest() :
	RpcServiceRequest("live", "2016-11-01", "AddCasterComponent")
{
	setMethod(HttpRequest::Method::Post);
}

AddCasterComponentRequest::~AddCasterComponentRequest()
{}

std::string AddCasterComponentRequest::getHtmlLayerContent()const
{
	return htmlLayerContent_;
}

void AddCasterComponentRequest::setHtmlLayerContent(const std::string& htmlLayerContent)
{
	htmlLayerContent_ = htmlLayerContent;
	setParameter("HtmlLayerContent", htmlLayerContent);
}

std::string AddCasterComponentRequest::getImageLayerContent()const
{
	return imageLayerContent_;
}

void AddCasterComponentRequest::setImageLayerContent(const std::string& imageLayerContent)
{
	imageLayerContent_ = imageLayerContent;
	setParameter("ImageLayerContent", imageLayerContent);
}

std::string AddCasterComponentRequest::getComponentName()const
{
	return componentName_;
}

void AddCasterComponentRequest::setComponentName(const std::string& componentName)
{
	componentName_ = componentName;
	setParameter("ComponentName", componentName);
}

std::string AddCasterComponentRequest::getLayerOrder()const
{
	return layerOrder_;
}

void AddCasterComponentRequest::setLayerOrder(const std::string& layerOrder)
{
	layerOrder_ = layerOrder;
	setParameter("LayerOrder", layerOrder);
}

std::string AddCasterComponentRequest::getCasterId()const
{
	return casterId_;
}

void AddCasterComponentRequest::setCasterId(const std::string& casterId)
{
	casterId_ = casterId;
	setParameter("CasterId", casterId);
}

std::string AddCasterComponentRequest::getComponentLayer()const
{
	return componentLayer_;
}

void AddCasterComponentRequest::setComponentLayer(const std::string& componentLayer)
{
	componentLayer_ = componentLayer;
	setParameter("ComponentLayer", componentLayer);
}

long AddCasterComponentRequest::getOwnerId()const
{
	return ownerId_;
}

void AddCasterComponentRequest::setOwnerId(long ownerId)
{
	ownerId_ = ownerId;
	setParameter("OwnerId", std::to_string(ownerId));
}

std::string AddCasterComponentRequest::getComponentType()const
{
	return componentType_;
}

void AddCasterComponentRequest::setComponentType(const std::string& componentType)
{
	componentType_ = componentType;
	setParameter("ComponentType", componentType);
}

std::string AddCasterComponentRequest::getLocationId()const
{
	return locationId_;
}

void AddCasterComponentRequest::setLocationId(const std::string& locationId)
{
	locationId_ = locationId;
	setParameter("LocationId", locationId);
}

std::string AddCasterComponentRequest::getEffect()const
{
	return effect_;
}

void AddCasterComponentRequest::setEffect(const std::string& effect)
{
	effect_ = effect;
	setParameter("Effect", effect);
}

std::string AddCasterComponentRequest::getCaptionLayerContent()const
{
	return captionLayerContent_;
}

void AddCasterComponentRequest::setCaptionLayerContent(const std::string& captionLayerContent)
{
	captionLayerContent_ = captionLayerContent;
	setParameter("CaptionLayerContent", captionLayerContent);
}

std::string AddCasterComponentRequest::getTextLayerContent()const
{
	return textLayerContent_;
}

void AddCasterComponentRequest::setTextLayerContent(const std::string& textLayerContent)
{
	textLayerContent_ = textLayerContent;
	setParameter("TextLayerContent", textLayerContent);
}

