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

#ifndef ALIBABACLOUD_MULTIMEDIAAI_MODEL_LISTFACEGROUPSRESULT_H_
#define ALIBABACLOUD_MULTIMEDIAAI_MODEL_LISTFACEGROUPSRESULT_H_

#include <string>
#include <vector>
#include <utility>
#include <alibabacloud/core/ServiceResult.h>
#include <alibabacloud/multimediaai/MultimediaaiExport.h>

namespace AlibabaCloud
{
	namespace Multimediaai
	{
		namespace Model
		{
			class ALIBABACLOUD_MULTIMEDIAAI_EXPORT ListFaceGroupsResult : public ServiceResult
			{
			public:
				struct FaceGroup
				{
					struct TemplatesItem
					{
						std::string id;
						std::string name;
					};
					long imageCount;
					std::string description;
					std::string faceGroupName;
					long personCount;
					std::vector<FaceGroup::TemplatesItem> templates;
					long faceGroupId;
				};


				ListFaceGroupsResult();
				explicit ListFaceGroupsResult(const std::string &payload);
				~ListFaceGroupsResult();
				long getTotalCount()const;
				int getPageSize()const;
				int getPageNumber()const;
				std::vector<FaceGroup> getFaceGroups()const;

			protected:
				void parse(const std::string &payload);
			private:
				long totalCount_;
				int pageSize_;
				int pageNumber_;
				std::vector<FaceGroup> faceGroups_;

			};
		}
	}
}
#endif // !ALIBABACLOUD_MULTIMEDIAAI_MODEL_LISTFACEGROUPSRESULT_H_