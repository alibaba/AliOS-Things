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

#ifndef ALIBABACLOUD_MULTIMEDIAAI_MODEL_LISTFACEPERSONSRESULT_H_
#define ALIBABACLOUD_MULTIMEDIAAI_MODEL_LISTFACEPERSONSRESULT_H_

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
			class ALIBABACLOUD_MULTIMEDIAAI_EXPORT ListFacePersonsResult : public ServiceResult
			{
			public:
				struct FacePerson
				{
					long imageCount;
					std::string facePersonName;
					std::string imageUrl;
					long facePersonId;
				};


				ListFacePersonsResult();
				explicit ListFacePersonsResult(const std::string &payload);
				~ListFacePersonsResult();
				long getTotalCount()const;
				std::vector<FacePerson> getFacePersons()const;
				int getPageSize()const;
				int getPageNumber()const;

			protected:
				void parse(const std::string &payload);
			private:
				long totalCount_;
				std::vector<FacePerson> facePersons_;
				int pageSize_;
				int pageNumber_;

			};
		}
	}
}
#endif // !ALIBABACLOUD_MULTIMEDIAAI_MODEL_LISTFACEPERSONSRESULT_H_