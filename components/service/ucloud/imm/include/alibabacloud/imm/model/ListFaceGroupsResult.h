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

#ifndef ALIBABACLOUD_IMM_MODEL_LISTFACEGROUPSRESULT_H_
#define ALIBABACLOUD_IMM_MODEL_LISTFACEGROUPSRESULT_H_

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
			class ALIBABACLOUD_IMM_EXPORT ListFaceGroupsResult : public ServiceResult
			{
			public:
				struct FaceGroupsItem
				{
					struct GroupCoverFace
					{
						struct FaceBoundary
						{
							int left;
							int top;
							int height;
							int width;
						};
						std::string faceId;
						FaceBoundary faceBoundary;
						std::string imageUri;
					};
					GroupCoverFace groupCoverFace;
					std::string groupName;
					std::string modifyTime;
					std::string createTime;
					std::string externalId;
					float maxAge;
					std::string gender;
					std::string remarksA;
					float averageAge;
					std::string remarksB;
					std::string groupId;
					std::string remarksArrayB;
					std::string remarksC;
					int imageCount;
					float minAge;
					std::string remarksD;
					std::string remarksArrayA;
					int faceCount;
				};


				ListFaceGroupsResult();
				explicit ListFaceGroupsResult(const std::string &payload);
				~ListFaceGroupsResult();
				std::string getNextMarker()const;
				std::vector<FaceGroupsItem> getFaceGroups()const;

			protected:
				void parse(const std::string &payload);
			private:
				std::string nextMarker_;
				std::vector<FaceGroupsItem> faceGroups_;

			};
		}
	}
}
#endif // !ALIBABACLOUD_IMM_MODEL_LISTFACEGROUPSRESULT_H_