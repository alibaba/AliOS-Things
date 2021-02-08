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

#ifndef ALIBABACLOUD_IMM_MODEL_LISTVIDEOFRAMESRESULT_H_
#define ALIBABACLOUD_IMM_MODEL_LISTVIDEOFRAMESRESULT_H_

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
			class ALIBABACLOUD_IMM_EXPORT ListVideoFramesResult : public ServiceResult
			{
			public:
				struct FramesItem
				{
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
					std::string modifyTime;
					std::string sourceType;
					std::string sourceUri;
					std::string facesFailReason;
					std::string remarksA;
					std::string remarksB;
					std::string imageFormat;
					std::string tagsFailReason;
					std::string facesModifyTime;
					std::string exif;
					std::string remarksC;
					std::string remarksD;
					int imageWidth;
					std::vector<FramesItem::CelebrityItem> celebrity;
					std::string sourcePosition;
					std::vector<FramesItem::FacesItem> faces;
					std::vector<FramesItem::TagsItem> tags;
					std::string facesStatus;
					std::string createTime;
					std::string tagsModifyTime;
					std::string externalId;
					std::string imageUri;
					std::string orientation;
					std::string oCRStatus;
					std::string oCRModifyTime;
					std::string imageTime;
					std::string celebrityModifyTime;
					int imageHeight;
					std::string tagsStatus;
					std::string oCRFailReason;
					std::string celebrityFailReason;
					std::string celebrityStatus;
					std::vector<FramesItem::OCRItem> oCR;
					int fileSize;
					std::string location;
				};


				ListVideoFramesResult();
				explicit ListVideoFramesResult(const std::string &payload);
				~ListVideoFramesResult();
				std::vector<FramesItem> getFrames()const;
				std::string getVideoUri()const;
				std::string getSetId()const;
				std::string getNextMarker()const;

			protected:
				void parse(const std::string &payload);
			private:
				std::vector<FramesItem> frames_;
				std::string videoUri_;
				std::string setId_;
				std::string nextMarker_;

			};
		}
	}
}
#endif // !ALIBABACLOUD_IMM_MODEL_LISTVIDEOFRAMESRESULT_H_