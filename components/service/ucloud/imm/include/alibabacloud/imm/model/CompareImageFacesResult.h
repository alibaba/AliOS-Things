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

#ifndef ALIBABACLOUD_IMM_MODEL_COMPAREIMAGEFACESRESULT_H_
#define ALIBABACLOUD_IMM_MODEL_COMPAREIMAGEFACESRESULT_H_

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
			class ALIBABACLOUD_IMM_EXPORT CompareImageFacesResult : public ServiceResult
			{
			public:
				struct FaceA
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
						FaceBoundary faceBoundary;
					};
					std::string faceId;
					FaceAttributes faceAttributes;
				};
				struct FaceB
				{
					struct FaceAttributes1
					{
						struct FaceBoundary2
						{
							int left;
							int top;
							int height;
							int width;
						};
						FaceBoundary2 faceBoundary2;
					};
					std::string faceId;
					FaceAttributes1 faceAttributes1;
				};


				CompareImageFacesResult();
				explicit CompareImageFacesResult(const std::string &payload);
				~CompareImageFacesResult();
				float getSimilarity()const;
				FaceA getFaceA()const;
				std::string getSetId()const;
				FaceB getFaceB()const;

			protected:
				void parse(const std::string &payload);
			private:
				float similarity_;
				FaceA faceA_;
				std::string setId_;
				FaceB faceB_;

			};
		}
	}
}
#endif // !ALIBABACLOUD_IMM_MODEL_COMPAREIMAGEFACESRESULT_H_