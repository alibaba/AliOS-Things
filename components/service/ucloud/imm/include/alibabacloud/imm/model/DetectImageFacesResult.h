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

#ifndef ALIBABACLOUD_IMM_MODEL_DETECTIMAGEFACESRESULT_H_
#define ALIBABACLOUD_IMM_MODEL_DETECTIMAGEFACESRESULT_H_

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
			class ALIBABACLOUD_IMM_EXPORT DetectImageFacesResult : public ServiceResult
			{
			public:
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
					std::string faceId;
					float attractiveConfidence;
					FaceAttributes faceAttributes;
					float genderConfidence;
					float emotionConfidence;
					std::string gender;
					EmotionDetails emotionDetails;
					float faceConfidence;
					float attractive;
					float faceQuality;
					std::string emotion;
					int age;
					float ageConfidence;
				};


				DetectImageFacesResult();
				explicit DetectImageFacesResult(const std::string &payload);
				~DetectImageFacesResult();
				std::string getImageUri()const;
				std::vector<FacesItem> getFaces()const;

			protected:
				void parse(const std::string &payload);
			private:
				std::string imageUri_;
				std::vector<FacesItem> faces_;

			};
		}
	}
}
#endif // !ALIBABACLOUD_IMM_MODEL_DETECTIMAGEFACESRESULT_H_