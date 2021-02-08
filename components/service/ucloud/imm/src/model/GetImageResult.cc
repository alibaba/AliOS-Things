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

#include <alibabacloud/imm/model/GetImageResult.h>
#include <json/json.h>

using namespace AlibabaCloud::Imm;
using namespace AlibabaCloud::Imm::Model;

GetImageResult::GetImageResult() :
	ServiceResult()
{}

GetImageResult::GetImageResult(const std::string &payload) :
	ServiceResult()
{
	parse(payload);
}

GetImageResult::~GetImageResult()
{}

void GetImageResult::parse(const std::string &payload)
{
	Json::Reader reader;
	Json::Value value;
	reader.parse(payload, value);
	setRequestId(value["RequestId"].asString());
	auto allCroppingSuggestionNode = value["CroppingSuggestion"]["CroppingSuggestionItem"];
	for (auto valueCroppingSuggestionCroppingSuggestionItem : allCroppingSuggestionNode)
	{
		CroppingSuggestionItem croppingSuggestionObject;
		if(!valueCroppingSuggestionCroppingSuggestionItem["AspectRatio"].isNull())
			croppingSuggestionObject.aspectRatio = valueCroppingSuggestionCroppingSuggestionItem["AspectRatio"].asString();
		if(!valueCroppingSuggestionCroppingSuggestionItem["Score"].isNull())
			croppingSuggestionObject.score = std::stof(valueCroppingSuggestionCroppingSuggestionItem["Score"].asString());
		auto croppingBoundaryNode = value["CroppingBoundary"];
		if(!croppingBoundaryNode["Width"].isNull())
			croppingSuggestionObject.croppingBoundary.width = std::stoi(croppingBoundaryNode["Width"].asString());
		if(!croppingBoundaryNode["Height"].isNull())
			croppingSuggestionObject.croppingBoundary.height = std::stoi(croppingBoundaryNode["Height"].asString());
		if(!croppingBoundaryNode["Left"].isNull())
			croppingSuggestionObject.croppingBoundary.left = std::stoi(croppingBoundaryNode["Left"].asString());
		if(!croppingBoundaryNode["Top"].isNull())
			croppingSuggestionObject.croppingBoundary.top = std::stoi(croppingBoundaryNode["Top"].asString());
		croppingSuggestion_.push_back(croppingSuggestionObject);
	}
	auto allFacesNode = value["Faces"]["FacesItem"];
	for (auto valueFacesFacesItem : allFacesNode)
	{
		FacesItem facesObject;
		if(!valueFacesFacesItem["FaceId"].isNull())
			facesObject.faceId = valueFacesFacesItem["FaceId"].asString();
		if(!valueFacesFacesItem["FaceConfidence"].isNull())
			facesObject.faceConfidence = std::stof(valueFacesFacesItem["FaceConfidence"].asString());
		if(!valueFacesFacesItem["Age"].isNull())
			facesObject.age = valueFacesFacesItem["Age"].asString();
		if(!valueFacesFacesItem["Gender"].isNull())
			facesObject.gender = valueFacesFacesItem["Gender"].asString();
		if(!valueFacesFacesItem["Emotion"].isNull())
			facesObject.emotion = valueFacesFacesItem["Emotion"].asString();
		if(!valueFacesFacesItem["Attractive"].isNull())
			facesObject.attractive = std::stof(valueFacesFacesItem["Attractive"].asString());
		if(!valueFacesFacesItem["GenderConfidence"].isNull())
			facesObject.genderConfidence = std::stof(valueFacesFacesItem["GenderConfidence"].asString());
		if(!valueFacesFacesItem["GroupId"].isNull())
			facesObject.groupId = valueFacesFacesItem["GroupId"].asString();
		if(!valueFacesFacesItem["FaceQuality"].isNull())
			facesObject.faceQuality = std::stof(valueFacesFacesItem["FaceQuality"].asString());
		if(!valueFacesFacesItem["EmotionConfidence"].isNull())
			facesObject.emotionConfidence = std::stof(valueFacesFacesItem["EmotionConfidence"].asString());
		auto faceAttributesNode = value["FaceAttributes"];
		if(!faceAttributesNode["Glasses"].isNull())
			facesObject.faceAttributes.glasses = faceAttributesNode["Glasses"].asString();
		if(!faceAttributesNode["Beard"].isNull())
			facesObject.faceAttributes.beard = faceAttributesNode["Beard"].asString();
		if(!faceAttributesNode["Race"].isNull())
			facesObject.faceAttributes.race = faceAttributesNode["Race"].asString();
		if(!faceAttributesNode["Mask"].isNull())
			facesObject.faceAttributes.mask = faceAttributesNode["Mask"].asString();
		if(!faceAttributesNode["GlassesConfidence"].isNull())
			facesObject.faceAttributes.glassesConfidence = std::stof(faceAttributesNode["GlassesConfidence"].asString());
		if(!faceAttributesNode["BeardConfidence"].isNull())
			facesObject.faceAttributes.beardConfidence = std::stof(faceAttributesNode["BeardConfidence"].asString());
		if(!faceAttributesNode["RaceConfidence"].isNull())
			facesObject.faceAttributes.raceConfidence = std::stof(faceAttributesNode["RaceConfidence"].asString());
		if(!faceAttributesNode["MaskConfidence"].isNull())
			facesObject.faceAttributes.maskConfidence = std::stof(faceAttributesNode["MaskConfidence"].asString());
		auto faceBoundaryNode = faceAttributesNode["FaceBoundary"];
		if(!faceBoundaryNode["Left"].isNull())
			facesObject.faceAttributes.faceBoundary.left = std::stoi(faceBoundaryNode["Left"].asString());
		if(!faceBoundaryNode["Top"].isNull())
			facesObject.faceAttributes.faceBoundary.top = std::stoi(faceBoundaryNode["Top"].asString());
		if(!faceBoundaryNode["Width"].isNull())
			facesObject.faceAttributes.faceBoundary.width = std::stoi(faceBoundaryNode["Width"].asString());
		if(!faceBoundaryNode["Height"].isNull())
			facesObject.faceAttributes.faceBoundary.height = std::stoi(faceBoundaryNode["Height"].asString());
		auto headPoseNode = faceAttributesNode["HeadPose"];
		if(!headPoseNode["Pitch"].isNull())
			facesObject.faceAttributes.headPose.pitch = std::stof(headPoseNode["Pitch"].asString());
		if(!headPoseNode["Roll"].isNull())
			facesObject.faceAttributes.headPose.roll = std::stof(headPoseNode["Roll"].asString());
		if(!headPoseNode["Yaw"].isNull())
			facesObject.faceAttributes.headPose.yaw = std::stof(headPoseNode["Yaw"].asString());
		auto emotionDetailsNode = value["EmotionDetails"];
		if(!emotionDetailsNode["HAPPY"].isNull())
			facesObject.emotionDetails.hAPPY = std::stof(emotionDetailsNode["HAPPY"].asString());
		if(!emotionDetailsNode["SAD"].isNull())
			facesObject.emotionDetails.sAD = std::stof(emotionDetailsNode["SAD"].asString());
		if(!emotionDetailsNode["ANGRY"].isNull())
			facesObject.emotionDetails.aNGRY = std::stof(emotionDetailsNode["ANGRY"].asString());
		if(!emotionDetailsNode["SURPRISED"].isNull())
			facesObject.emotionDetails.sURPRISED = std::stof(emotionDetailsNode["SURPRISED"].asString());
		if(!emotionDetailsNode["SCARED"].isNull())
			facesObject.emotionDetails.sCARED = std::stof(emotionDetailsNode["SCARED"].asString());
		if(!emotionDetailsNode["DISGUSTED"].isNull())
			facesObject.emotionDetails.dISGUSTED = std::stof(emotionDetailsNode["DISGUSTED"].asString());
		if(!emotionDetailsNode["CALM"].isNull())
			facesObject.emotionDetails.cALM = std::stof(emotionDetailsNode["CALM"].asString());
		faces_.push_back(facesObject);
	}
	auto allOCRNode = value["OCR"]["OCRItem"];
	for (auto valueOCROCRItem : allOCRNode)
	{
		OCRItem oCRObject;
		if(!valueOCROCRItem["OCRContents"].isNull())
			oCRObject.oCRContents = valueOCROCRItem["OCRContents"].asString();
		if(!valueOCROCRItem["OCRConfidence"].isNull())
			oCRObject.oCRConfidence = std::stof(valueOCROCRItem["OCRConfidence"].asString());
		auto oCRBoundaryNode = value["OCRBoundary"];
		if(!oCRBoundaryNode["Left"].isNull())
			oCRObject.oCRBoundary.left = std::stoi(oCRBoundaryNode["Left"].asString());
		if(!oCRBoundaryNode["Top"].isNull())
			oCRObject.oCRBoundary.top = std::stoi(oCRBoundaryNode["Top"].asString());
		if(!oCRBoundaryNode["Width"].isNull())
			oCRObject.oCRBoundary.width = std::stoi(oCRBoundaryNode["Width"].asString());
		if(!oCRBoundaryNode["Height"].isNull())
			oCRObject.oCRBoundary.height = std::stoi(oCRBoundaryNode["Height"].asString());
		oCR_.push_back(oCRObject);
	}
	auto allTagsNode = value["Tags"]["TagsItem"];
	for (auto valueTagsTagsItem : allTagsNode)
	{
		TagsItem tagsObject;
		if(!valueTagsTagsItem["TagName"].isNull())
			tagsObject.tagName = valueTagsTagsItem["TagName"].asString();
		if(!valueTagsTagsItem["TagConfidence"].isNull())
			tagsObject.tagConfidence = std::stof(valueTagsTagsItem["TagConfidence"].asString());
		if(!valueTagsTagsItem["TagLevel"].isNull())
			tagsObject.tagLevel = std::stoi(valueTagsTagsItem["TagLevel"].asString());
		if(!valueTagsTagsItem["ParentTagName"].isNull())
			tagsObject.parentTagName = valueTagsTagsItem["ParentTagName"].asString();
		tags_.push_back(tagsObject);
	}
	auto allCelebrityNode = value["Celebrity"]["CelebrityItem"];
	for (auto valueCelebrityCelebrityItem : allCelebrityNode)
	{
		CelebrityItem celebrityObject;
		if(!valueCelebrityCelebrityItem["CelebrityName"].isNull())
			celebrityObject.celebrityName = valueCelebrityCelebrityItem["CelebrityName"].asString();
		if(!valueCelebrityCelebrityItem["CelebrityGender"].isNull())
			celebrityObject.celebrityGender = valueCelebrityCelebrityItem["CelebrityGender"].asString();
		if(!valueCelebrityCelebrityItem["CelebrityConfidence"].isNull())
			celebrityObject.celebrityConfidence = std::stof(valueCelebrityCelebrityItem["CelebrityConfidence"].asString());
		if(!valueCelebrityCelebrityItem["CelebrityLibraryName"].isNull())
			celebrityObject.celebrityLibraryName = valueCelebrityCelebrityItem["CelebrityLibraryName"].asString();
		auto celebrityBoundaryNode = value["CelebrityBoundary"];
		if(!celebrityBoundaryNode["Left"].isNull())
			celebrityObject.celebrityBoundary.left = std::stoi(celebrityBoundaryNode["Left"].asString());
		if(!celebrityBoundaryNode["Top"].isNull())
			celebrityObject.celebrityBoundary.top = std::stoi(celebrityBoundaryNode["Top"].asString());
		if(!celebrityBoundaryNode["Width"].isNull())
			celebrityObject.celebrityBoundary.width = std::stoi(celebrityBoundaryNode["Width"].asString());
		if(!celebrityBoundaryNode["Height"].isNull())
			celebrityObject.celebrityBoundary.height = std::stoi(celebrityBoundaryNode["Height"].asString());
		celebrity_.push_back(celebrityObject);
	}
	auto imageQualityNode = value["ImageQuality"];
	if(!imageQualityNode["OverallScore"].isNull())
		imageQuality_.overallScore = std::stof(imageQualityNode["OverallScore"].asString());
	if(!imageQualityNode["ClarityScore"].isNull())
		imageQuality_.clarityScore = std::stof(imageQualityNode["ClarityScore"].asString());
	if(!imageQualityNode["Clarity"].isNull())
		imageQuality_.clarity = std::stof(imageQualityNode["Clarity"].asString());
	if(!imageQualityNode["ExposureScore"].isNull())
		imageQuality_.exposureScore = std::stof(imageQualityNode["ExposureScore"].asString());
	if(!imageQualityNode["Exposure"].isNull())
		imageQuality_.exposure = std::stof(imageQualityNode["Exposure"].asString());
	if(!imageQualityNode["ContrastScore"].isNull())
		imageQuality_.contrastScore = std::stof(imageQualityNode["ContrastScore"].asString());
	if(!imageQualityNode["Contrast"].isNull())
		imageQuality_.contrast = std::stof(imageQualityNode["Contrast"].asString());
	if(!imageQualityNode["ColorScore"].isNull())
		imageQuality_.colorScore = std::stof(imageQualityNode["ColorScore"].asString());
	if(!imageQualityNode["Color"].isNull())
		imageQuality_.color = std::stof(imageQualityNode["Color"].asString());
	if(!imageQualityNode["CompositionScore"].isNull())
		imageQuality_.compositionScore = std::stof(imageQualityNode["CompositionScore"].asString());
	auto addressNode = value["Address"];
	if(!addressNode["AddressLine"].isNull())
		address_.addressLine = addressNode["AddressLine"].asString();
	if(!addressNode["Country"].isNull())
		address_.country = addressNode["Country"].asString();
	if(!addressNode["Province"].isNull())
		address_.province = addressNode["Province"].asString();
	if(!addressNode["City"].isNull())
		address_.city = addressNode["City"].asString();
	if(!addressNode["District"].isNull())
		address_.district = addressNode["District"].asString();
	if(!addressNode["Township"].isNull())
		address_.township = addressNode["Township"].asString();
	if(!value["SetId"].isNull())
		setId_ = value["SetId"].asString();
	if(!value["ImageUri"].isNull())
		imageUri_ = value["ImageUri"].asString();
	if(!value["RemarksA"].isNull())
		remarksA_ = value["RemarksA"].asString();
	if(!value["RemarksB"].isNull())
		remarksB_ = value["RemarksB"].asString();
	if(!value["CreateTime"].isNull())
		createTime_ = value["CreateTime"].asString();
	if(!value["ModifyTime"].isNull())
		modifyTime_ = value["ModifyTime"].asString();
	if(!value["ImageWidth"].isNull())
		imageWidth_ = std::stoi(value["ImageWidth"].asString());
	if(!value["ImageHeight"].isNull())
		imageHeight_ = std::stoi(value["ImageHeight"].asString());
	if(!value["ImageFormat"].isNull())
		imageFormat_ = value["ImageFormat"].asString();
	if(!value["Exif"].isNull())
		exif_ = value["Exif"].asString();
	if(!value["FileSize"].isNull())
		fileSize_ = std::stoi(value["FileSize"].asString());
	if(!value["ImageTime"].isNull())
		imageTime_ = value["ImageTime"].asString();
	if(!value["Orientation"].isNull())
		orientation_ = value["Orientation"].asString();
	if(!value["SourceType"].isNull())
		sourceType_ = value["SourceType"].asString();
	if(!value["SourceUri"].isNull())
		sourceUri_ = value["SourceUri"].asString();
	if(!value["SourcePosition"].isNull())
		sourcePosition_ = value["SourcePosition"].asString();
	if(!value["FacesStatus"].isNull())
		facesStatus_ = value["FacesStatus"].asString();
	if(!value["FacesModifyTime"].isNull())
		facesModifyTime_ = value["FacesModifyTime"].asString();
	if(!value["Location"].isNull())
		location_ = value["Location"].asString();
	if(!value["OCRStatus"].isNull())
		oCRStatus_ = value["OCRStatus"].asString();
	if(!value["OCRModifyTime"].isNull())
		oCRModifyTime_ = value["OCRModifyTime"].asString();
	if(!value["OCRFailReason"].isNull())
		oCRFailReason_ = value["OCRFailReason"].asString();
	if(!value["FacesFailReason"].isNull())
		facesFailReason_ = value["FacesFailReason"].asString();
	if(!value["TagsFailReason"].isNull())
		tagsFailReason_ = value["TagsFailReason"].asString();
	if(!value["TagsModifyTime"].isNull())
		tagsModifyTime_ = value["TagsModifyTime"].asString();
	if(!value["CelebrityStatus"].isNull())
		celebrityStatus_ = value["CelebrityStatus"].asString();
	if(!value["CelebrityModifyTime"].isNull())
		celebrityModifyTime_ = value["CelebrityModifyTime"].asString();
	if(!value["CelebrityFailReason"].isNull())
		celebrityFailReason_ = value["CelebrityFailReason"].asString();
	if(!value["TagsStatus"].isNull())
		tagsStatus_ = value["TagsStatus"].asString();
	if(!value["RemarksC"].isNull())
		remarksC_ = value["RemarksC"].asString();
	if(!value["RemarksD"].isNull())
		remarksD_ = value["RemarksD"].asString();
	if(!value["ExternalId"].isNull())
		externalId_ = value["ExternalId"].asString();
	if(!value["AddressModifyTime"].isNull())
		addressModifyTime_ = value["AddressModifyTime"].asString();
	if(!value["AddressStatus"].isNull())
		addressStatus_ = value["AddressStatus"].asString();
	if(!value["AddressFailReason"].isNull())
		addressFailReason_ = value["AddressFailReason"].asString();
	if(!value["RemarksArrayA"].isNull())
		remarksArrayA_ = value["RemarksArrayA"].asString();
	if(!value["RemarksArrayB"].isNull())
		remarksArrayB_ = value["RemarksArrayB"].asString();
	if(!value["ImageQualityModifyTime"].isNull())
		imageQualityModifyTime_ = value["ImageQualityModifyTime"].asString();
	if(!value["ImageQualityFailReason"].isNull())
		imageQualityFailReason_ = value["ImageQualityFailReason"].asString();
	if(!value["ImageQualityStatus"].isNull())
		imageQualityStatus_ = value["ImageQualityStatus"].asString();
	if(!value["CroppingSuggestionStatus"].isNull())
		croppingSuggestionStatus_ = value["CroppingSuggestionStatus"].asString();
	if(!value["CroppingSuggestionFailReason"].isNull())
		croppingSuggestionFailReason_ = value["CroppingSuggestionFailReason"].asString();
	if(!value["CroppingSuggestionModifyTime"].isNull())
		croppingSuggestionModifyTime_ = value["CroppingSuggestionModifyTime"].asString();

}

GetImageResult::ImageQuality GetImageResult::getImageQuality()const
{
	return imageQuality_;
}

GetImageResult::Address GetImageResult::getAddress()const
{
	return address_;
}

std::string GetImageResult::getSourceType()const
{
	return sourceType_;
}

std::string GetImageResult::getSourceUri()const
{
	return sourceUri_;
}

std::string GetImageResult::getFacesFailReason()const
{
	return facesFailReason_;
}

std::string GetImageResult::getCroppingSuggestionFailReason()const
{
	return croppingSuggestionFailReason_;
}

std::string GetImageResult::getAddressFailReason()const
{
	return addressFailReason_;
}

std::string GetImageResult::getImageFormat()const
{
	return imageFormat_;
}

std::string GetImageResult::getRemarksArrayB()const
{
	return remarksArrayB_;
}

std::string GetImageResult::getExif()const
{
	return exif_;
}

int GetImageResult::getImageWidth()const
{
	return imageWidth_;
}

std::vector<GetImageResult::CelebrityItem> GetImageResult::getCelebrity()const
{
	return celebrity_;
}

std::string GetImageResult::getSourcePosition()const
{
	return sourcePosition_;
}

std::string GetImageResult::getRemarksArrayA()const
{
	return remarksArrayA_;
}

std::vector<GetImageResult::FacesItem> GetImageResult::getFaces()const
{
	return faces_;
}

std::vector<GetImageResult::TagsItem> GetImageResult::getTags()const
{
	return tags_;
}

std::string GetImageResult::getAddressStatus()const
{
	return addressStatus_;
}

std::string GetImageResult::getImageQualityModifyTime()const
{
	return imageQualityModifyTime_;
}

std::vector<GetImageResult::CroppingSuggestionItem> GetImageResult::getCroppingSuggestion()const
{
	return croppingSuggestion_;
}

std::string GetImageResult::getImageQualityFailReason()const
{
	return imageQualityFailReason_;
}

std::string GetImageResult::getImageUri()const
{
	return imageUri_;
}

std::string GetImageResult::getOrientation()const
{
	return orientation_;
}

std::string GetImageResult::getImageTime()const
{
	return imageTime_;
}

std::string GetImageResult::getCroppingSuggestionModifyTime()const
{
	return croppingSuggestionModifyTime_;
}

std::string GetImageResult::getCelebrityModifyTime()const
{
	return celebrityModifyTime_;
}

int GetImageResult::getImageHeight()const
{
	return imageHeight_;
}

std::string GetImageResult::getTagsStatus()const
{
	return tagsStatus_;
}

std::string GetImageResult::getImageQualityStatus()const
{
	return imageQualityStatus_;
}

std::string GetImageResult::getCelebrityFailReason()const
{
	return celebrityFailReason_;
}

std::string GetImageResult::getSetId()const
{
	return setId_;
}

std::string GetImageResult::getCelebrityStatus()const
{
	return celebrityStatus_;
}

int GetImageResult::getFileSize()const
{
	return fileSize_;
}

std::string GetImageResult::getModifyTime()const
{
	return modifyTime_;
}

std::string GetImageResult::getCroppingSuggestionStatus()const
{
	return croppingSuggestionStatus_;
}

std::string GetImageResult::getRemarksA()const
{
	return remarksA_;
}

std::string GetImageResult::getRemarksB()const
{
	return remarksB_;
}

std::string GetImageResult::getAddressModifyTime()const
{
	return addressModifyTime_;
}

std::string GetImageResult::getTagsFailReason()const
{
	return tagsFailReason_;
}

std::string GetImageResult::getFacesModifyTime()const
{
	return facesModifyTime_;
}

std::string GetImageResult::getRemarksC()const
{
	return remarksC_;
}

std::string GetImageResult::getRemarksD()const
{
	return remarksD_;
}

std::string GetImageResult::getFacesStatus()const
{
	return facesStatus_;
}

std::string GetImageResult::getCreateTime()const
{
	return createTime_;
}

std::string GetImageResult::getTagsModifyTime()const
{
	return tagsModifyTime_;
}

std::string GetImageResult::getExternalId()const
{
	return externalId_;
}

std::string GetImageResult::getOCRStatus()const
{
	return oCRStatus_;
}

std::string GetImageResult::getOCRModifyTime()const
{
	return oCRModifyTime_;
}

std::string GetImageResult::getOCRFailReason()const
{
	return oCRFailReason_;
}

std::vector<GetImageResult::OCRItem> GetImageResult::getOCR()const
{
	return oCR_;
}

std::string GetImageResult::getLocation()const
{
	return location_;
}

