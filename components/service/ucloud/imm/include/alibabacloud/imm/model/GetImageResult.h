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

#ifndef ALIBABACLOUD_IMM_MODEL_GETIMAGERESULT_H_
#define ALIBABACLOUD_IMM_MODEL_GETIMAGERESULT_H_

#include <string>
#include <vector>
#include <utility>
#include <alibabacloud/core/ServiceResult.h>
#include <alibabacloud/imm/ImmExport.h>

namespace AlibabaCloud
{
	namespace Imm
	{
		namespace Model
		{
			class ALIBABACLOUD_IMM_EXPORT GetImageResult : public ServiceResult
			{
			public:
				struct ImageQuality
				{
					float clarityScore;
					float compositionScore;
					float contrastScore;
					float color;
					float exposure;
					float overallScore;
					float clarity;
					float contrast;
					float exposureScore;
					float colorScore;
				};
				struct Address
				{
					std::string township;
					std::string addressLine;
					std::string country;
					std::string city;
					std::string district;
					std::string province;
				};
				struct CroppingSuggestionItem
				{
					struct CroppingBoundary
					{
						int left;
						int top;
						int height;
						int width;
					};
					std::string aspectRatio;
					float score;
					CroppingBoundary croppingBoundary;
				};
				struct FacesItem
				{
					struct FaceAttributes
					{
						struct FaceBoundary
						{
							int left;
							int top;
							int height;
							int width;
						};
						struct HeadPose
						{
							float pitch;
							float roll;
							float yaw;
						};
						FaceBoundary faceBoundary;
						std::string beard;
						float beardConfidence;
						HeadPose headPose;
						std::string race;
						float raceConfidence;
						std::string glasses;
						std::string mask;
						float maskConfidence;
						float glassesConfidence;
					};
					struct EmotionDetails
					{
						float cALM;
						float sCARED;
						float hAPPY;
						float sAD;
						float sURPRISED;
						float aNGRY;
						float dISGUSTED;
					};
					EmotionDetails emotionDetails;
					std::string faceId;
					float faceConfidence;
					float attractive;
					float faceQuality;
					FaceAttributes faceAttributes;
					float genderConfidence;
					float emotionConfidence;
					std::string gender;
					std::string emotion;
					std::string age;
					std::string groupId;
				};
				struct OCRItem
				{
					struct OCRBoundary
					{
						int left;
						int top;
						int height;
						int width;
					};
					OCRBoundary oCRBoundary;
					float oCRConfidence;
					std::string oCRContents;
				};
				struct TagsItem
				{
					std::string tagName;
					float tagConfidence;
					int tagLevel;
					std::string parentTagName;
				};
				struct CelebrityItem
				{
					struct CelebrityBoundary
					{
						int left;
						int top;
						int height;
						int width;
					};
					std::string celebrityName;
					CelebrityBoundary celebrityBoundary;
					std::string celebrityGender;
					std::string celebrityLibraryName;
					float celebrityConfidence;
				};


				GetImageResult();
				explicit GetImageResult(const std::string &payload);
				~GetImageResult();
				ImageQuality getImageQuality()const;
				Address getAddress()const;
				std::string getSourceType()const;
				std::string getSourceUri()const;
				std::string getFacesFailReason()const;
				std::string getCroppingSuggestionFailReason()const;
				std::string getAddressFailReason()const;
				std::string getImageFormat()const;
				std::string getRemarksArrayB()const;
				std::string getExif()const;
				int getImageWidth()const;
				std::vector<CelebrityItem> getCelebrity()const;
				std::string getSourcePosition()const;
				std::string getRemarksArrayA()const;
				std::vector<FacesItem> getFaces()const;
				std::vector<TagsItem> getTags()const;
				std::string getAddressStatus()const;
				std::string getImageQualityModifyTime()const;
				std::vector<CroppingSuggestionItem> getCroppingSuggestion()const;
				std::string getImageQualityFailReason()const;
				std::string getImageUri()const;
				std::string getOrientation()const;
				std::string getImageTime()const;
				std::string getCroppingSuggestionModifyTime()const;
				std::string getCelebrityModifyTime()const;
				int getImageHeight()const;
				std::string getTagsStatus()const;
				std::string getImageQualityStatus()const;
				std::string getCelebrityFailReason()const;
				std::string getSetId()const;
				std::string getCelebrityStatus()const;
				int getFileSize()const;
				std::string getModifyTime()const;
				std::string getCroppingSuggestionStatus()const;
				std::string getRemarksA()const;
				std::string getRemarksB()const;
				std::string getAddressModifyTime()const;
				std::string getTagsFailReason()const;
				std::string getFacesModifyTime()const;
				std::string getRemarksC()const;
				std::string getRemarksD()const;
				std::string getFacesStatus()const;
				std::string getCreateTime()const;
				std::string getTagsModifyTime()const;
				std::string getExternalId()const;
				std::string getOCRStatus()const;
				std::string getOCRModifyTime()const;
				std::string getOCRFailReason()const;
				std::vector<OCRItem> getOCR()const;
				std::string getLocation()const;

			protected:
				void parse(const std::string &payload);
			private:
				ImageQuality imageQuality_;
				Address address_;
				std::string sourceType_;
				std::string sourceUri_;
				std::string facesFailReason_;
				std::string croppingSuggestionFailReason_;
				std::string addressFailReason_;
				std::string imageFormat_;
				std::string remarksArrayB_;
				std::string exif_;
				int imageWidth_;
				std::vector<CelebrityItem> celebrity_;
				std::string sourcePosition_;
				std::string remarksArrayA_;
				std::vector<FacesItem> faces_;
				std::vector<TagsItem> tags_;
				std::string addressStatus_;
				std::string imageQualityModifyTime_;
				std::vector<CroppingSuggestionItem> croppingSuggestion_;
				std::string imageQualityFailReason_;
				std::string imageUri_;
				std::string orientation_;
				std::string imageTime_;
				std::string croppingSuggestionModifyTime_;
				std::string celebrityModifyTime_;
				int imageHeight_;
				std::string tagsStatus_;
				std::string imageQualityStatus_;
				std::string celebrityFailReason_;
				std::string setId_;
				std::string celebrityStatus_;
				int fileSize_;
				std::string modifyTime_;
				std::string croppingSuggestionStatus_;
				std::string remarksA_;
				std::string remarksB_;
				std::string addressModifyTime_;
				std::string tagsFailReason_;
				std::string facesModifyTime_;
				std::string remarksC_;
				std::string remarksD_;
				std::string facesStatus_;
				std::string createTime_;
				std::string tagsModifyTime_;
				std::string externalId_;
				std::string oCRStatus_;
				std::string oCRModifyTime_;
				std::string oCRFailReason_;
				std::vector<OCRItem> oCR_;
				std::string location_;

			};
		}
	}
}
#endif // !ALIBABACLOUD_IMM_MODEL_GETIMAGERESULT_H_