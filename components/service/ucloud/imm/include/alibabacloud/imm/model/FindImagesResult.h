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

#ifndef ALIBABACLOUD_IMM_MODEL_FINDIMAGESRESULT_H_
#define ALIBABACLOUD_IMM_MODEL_FINDIMAGESRESULT_H_

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
			class ALIBABACLOUD_IMM_EXPORT FindImagesResult : public ServiceResult
			{
			public:
				struct ImagesItem
				{
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
							float raceConfidence;
							std::string race;
							std::string glasses;
							float maskConfidence;
							std::string mask;
							float glassesConfidence;
						};
						EmotionDetails emotionDetails;
						float faceConfidence;
						std::string faceId;
						float attractive;
						float faceQuality;
						FaceAttributes faceAttributes;
						float genderConfidence;
						float emotionConfidence;
						std::string gender;
						std::string emotion;
						int age;
						std::string groupId;
					};
					struct TagsItem
					{
						float tagConfidence;
						std::string tagName;
						int tagLevel;
						std::string parentTagName;
					};
					struct OCRItem
					{
						struct OCRBoundary
						{
							int left;
							int left1;
							int height;
							int width;
						};
						OCRBoundary oCRBoundary;
						float oCRConfidence;
						std::string oCRContents;
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
					ImageQuality imageQuality;
					Address address;
					std::string sourceType;
					std::string sourceUri;
					std::string facesFailReason;
					std::string croppingSuggestionFailReason;
					std::string addressFailReason;
					std::string imageFormat;
					std::string remarksArrayB;
					std::string exif;
					int imageWidth;
					std::vector<ImagesItem::CelebrityItem> celebrity;
					std::string sourcePosition;
					std::string remarksArrayA;
					std::vector<ImagesItem::FacesItem> faces;
					std::vector<ImagesItem::TagsItem> tags;
					std::string addressStatus;
					std::string imageQualityModifyTime;
					std::vector<ImagesItem::CroppingSuggestionItem> croppingSuggestion;
					std::string imageQualityFailReason;
					std::string imageUri;
					std::string orientation;
					std::string imageTime;
					std::string croppingSuggestionModifyTime;
					std::string celebrityModifyTime;
					int imageHeight;
					std::string tagsStatus;
					std::string imageQualityStatus;
					std::string celebrityFailReason;
					std::string celebrityStatus;
					int fileSize;
					std::string modifyTime;
					std::string croppingSuggestionStatus;
					std::string remarksA;
					std::string remarksB;
					std::string addressModifyTime;
					std::string tagsFailReason;
					std::string facesModifyTime;
					std::string remarksC;
					std::string remarksD;
					std::string facesStatus;
					std::string createTime;
					std::string tagsModifyTime;
					std::string externalId;
					std::string oCRStatus;
					std::string oCRModifyTime;
					std::string oCRFailReason;
					std::vector<ImagesItem::OCRItem> oCR;
					std::string location;
				};


				FindImagesResult();
				explicit FindImagesResult(const std::string &payload);
				~FindImagesResult();
				std::vector<ImagesItem> getImages()const;
				std::string getSetId()const;
				std::string getNextMarker()const;

			protected:
				void parse(const std::string &payload);
			private:
				std::vector<ImagesItem> images_;
				std::string setId_;
				std::string nextMarker_;

			};
		}
	}
}
#endif // !ALIBABACLOUD_IMM_MODEL_FINDIMAGESRESULT_H_