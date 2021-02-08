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

#include <alibabacloud/imm/model/FindImagesRequest.h>

using AlibabaCloud::Imm::Model::FindImagesRequest;

FindImagesRequest::FindImagesRequest() :
	RpcServiceRequest("imm", "2017-09-06", "FindImages")
{
	setMethod(HttpRequest::Method::Post);
}

FindImagesRequest::~FindImagesRequest()
{}

std::string FindImagesRequest::getRemarksArrayBIn()const
{
	return remarksArrayBIn_;
}

void FindImagesRequest::setRemarksArrayBIn(const std::string& remarksArrayBIn)
{
	remarksArrayBIn_ = remarksArrayBIn;
	setParameter("RemarksArrayBIn", remarksArrayBIn);
}

std::string FindImagesRequest::getProject()const
{
	return project_;
}

void FindImagesRequest::setProject(const std::string& project)
{
	project_ = project;
	setParameter("Project", project);
}

std::string FindImagesRequest::getExternalId()const
{
	return externalId_;
}

void FindImagesRequest::setExternalId(const std::string& externalId)
{
	externalId_ = externalId;
	setParameter("ExternalId", externalId);
}

std::string FindImagesRequest::getFacesModifyTimeRange()const
{
	return facesModifyTimeRange_;
}

void FindImagesRequest::setFacesModifyTimeRange(const std::string& facesModifyTimeRange)
{
	facesModifyTimeRange_ = facesModifyTimeRange;
	setParameter("FacesModifyTimeRange", facesModifyTimeRange);
}

std::string FindImagesRequest::getOCRContentsMatch()const
{
	return oCRContentsMatch_;
}

void FindImagesRequest::setOCRContentsMatch(const std::string& oCRContentsMatch)
{
	oCRContentsMatch_ = oCRContentsMatch;
	setParameter("OCRContentsMatch", oCRContentsMatch);
}

int FindImagesRequest::getLimit()const
{
	return limit_;
}

void FindImagesRequest::setLimit(int limit)
{
	limit_ = limit;
	setParameter("Limit", std::to_string(limit));
}

std::string FindImagesRequest::getRemarksDPrefix()const
{
	return remarksDPrefix_;
}

void FindImagesRequest::setRemarksDPrefix(const std::string& remarksDPrefix)
{
	remarksDPrefix_ = remarksDPrefix;
	setParameter("RemarksDPrefix", remarksDPrefix);
}

std::string FindImagesRequest::getSourceType()const
{
	return sourceType_;
}

void FindImagesRequest::setSourceType(const std::string& sourceType)
{
	sourceType_ = sourceType;
	setParameter("SourceType", sourceType);
}

std::string FindImagesRequest::getOrder()const
{
	return order_;
}

void FindImagesRequest::setOrder(const std::string& order)
{
	order_ = order;
	setParameter("Order", order);
}

std::string FindImagesRequest::getGroupId()const
{
	return groupId_;
}

void FindImagesRequest::setGroupId(const std::string& groupId)
{
	groupId_ = groupId;
	setParameter("GroupId", groupId);
}

std::string FindImagesRequest::getOrderBy()const
{
	return orderBy_;
}

void FindImagesRequest::setOrderBy(const std::string& orderBy)
{
	orderBy_ = orderBy;
	setParameter("OrderBy", orderBy);
}

std::string FindImagesRequest::getTagNames()const
{
	return tagNames_;
}

void FindImagesRequest::setTagNames(const std::string& tagNames)
{
	tagNames_ = tagNames;
	setParameter("TagNames", tagNames);
}

std::string FindImagesRequest::getMarker()const
{
	return marker_;
}

void FindImagesRequest::setMarker(const std::string& marker)
{
	marker_ = marker;
	setParameter("Marker", marker);
}

std::string FindImagesRequest::getRemarksCPrefix()const
{
	return remarksCPrefix_;
}

void FindImagesRequest::setRemarksCPrefix(const std::string& remarksCPrefix)
{
	remarksCPrefix_ = remarksCPrefix;
	setParameter("RemarksCPrefix", remarksCPrefix);
}

std::string FindImagesRequest::getModifyTimeRange()const
{
	return modifyTimeRange_;
}

void FindImagesRequest::setModifyTimeRange(const std::string& modifyTimeRange)
{
	modifyTimeRange_ = modifyTimeRange;
	setParameter("ModifyTimeRange", modifyTimeRange);
}

std::string FindImagesRequest::getAddressLineContentsMatch()const
{
	return addressLineContentsMatch_;
}

void FindImagesRequest::setAddressLineContentsMatch(const std::string& addressLineContentsMatch)
{
	addressLineContentsMatch_ = addressLineContentsMatch;
	setParameter("AddressLineContentsMatch", addressLineContentsMatch);
}

std::string FindImagesRequest::getGender()const
{
	return gender_;
}

void FindImagesRequest::setGender(const std::string& gender)
{
	gender_ = gender;
	setParameter("Gender", gender);
}

std::string FindImagesRequest::getRemarksArrayAIn()const
{
	return remarksArrayAIn_;
}

void FindImagesRequest::setRemarksArrayAIn(const std::string& remarksArrayAIn)
{
	remarksArrayAIn_ = remarksArrayAIn;
	setParameter("RemarksArrayAIn", remarksArrayAIn);
}

std::string FindImagesRequest::getImageSizeRange()const
{
	return imageSizeRange_;
}

void FindImagesRequest::setImageSizeRange(const std::string& imageSizeRange)
{
	imageSizeRange_ = imageSizeRange;
	setParameter("ImageSizeRange", imageSizeRange);
}

std::string FindImagesRequest::getRemarksBPrefix()const
{
	return remarksBPrefix_;
}

void FindImagesRequest::setRemarksBPrefix(const std::string& remarksBPrefix)
{
	remarksBPrefix_ = remarksBPrefix;
	setParameter("RemarksBPrefix", remarksBPrefix);
}

std::string FindImagesRequest::getAccessKeyId()const
{
	return accessKeyId_;
}

void FindImagesRequest::setAccessKeyId(const std::string& accessKeyId)
{
	accessKeyId_ = accessKeyId;
	setParameter("AccessKeyId", accessKeyId);
}

std::string FindImagesRequest::getLocationBoundary()const
{
	return locationBoundary_;
}

void FindImagesRequest::setLocationBoundary(const std::string& locationBoundary)
{
	locationBoundary_ = locationBoundary;
	setParameter("LocationBoundary", locationBoundary);
}

std::string FindImagesRequest::getImageTimeRange()const
{
	return imageTimeRange_;
}

void FindImagesRequest::setImageTimeRange(const std::string& imageTimeRange)
{
	imageTimeRange_ = imageTimeRange;
	setParameter("ImageTimeRange", imageTimeRange);
}

std::string FindImagesRequest::getTagsModifyTimeRange()const
{
	return tagsModifyTimeRange_;
}

void FindImagesRequest::setTagsModifyTimeRange(const std::string& tagsModifyTimeRange)
{
	tagsModifyTimeRange_ = tagsModifyTimeRange;
	setParameter("TagsModifyTimeRange", tagsModifyTimeRange);
}

std::string FindImagesRequest::getAgeRange()const
{
	return ageRange_;
}

void FindImagesRequest::setAgeRange(const std::string& ageRange)
{
	ageRange_ = ageRange;
	setParameter("AgeRange", ageRange);
}

std::string FindImagesRequest::getRemarksAPrefix()const
{
	return remarksAPrefix_;
}

void FindImagesRequest::setRemarksAPrefix(const std::string& remarksAPrefix)
{
	remarksAPrefix_ = remarksAPrefix;
	setParameter("RemarksAPrefix", remarksAPrefix);
}

std::string FindImagesRequest::getSourceUriPrefix()const
{
	return sourceUriPrefix_;
}

void FindImagesRequest::setSourceUriPrefix(const std::string& sourceUriPrefix)
{
	sourceUriPrefix_ = sourceUriPrefix;
	setParameter("SourceUriPrefix", sourceUriPrefix);
}

std::string FindImagesRequest::getEmotion()const
{
	return emotion_;
}

void FindImagesRequest::setEmotion(const std::string& emotion)
{
	emotion_ = emotion;
	setParameter("Emotion", emotion);
}

std::string FindImagesRequest::getCreateTimeRange()const
{
	return createTimeRange_;
}

void FindImagesRequest::setCreateTimeRange(const std::string& createTimeRange)
{
	createTimeRange_ = createTimeRange;
	setParameter("CreateTimeRange", createTimeRange);
}

std::string FindImagesRequest::getSetId()const
{
	return setId_;
}

void FindImagesRequest::setSetId(const std::string& setId)
{
	setId_ = setId;
	setParameter("SetId", setId);
}

