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

#ifndef ALIBABACLOUD_IVISION_MODEL_DESCRIBEFACEGROUPSRESULT_H_
#define ALIBABACLOUD_IVISION_MODEL_DESCRIBEFACEGROUPSRESULT_H_

#include <string>
#include <vector>
#include <utility>
#include <alibabacloud/core/ServiceResult.h>
#include <alibabacloud/ivision/IvisionExport.h>

namespace AlibabaCloud
{
	namespace Ivision
	{
		namespace Model
		{
			class ALIBABACLOUD_IVISION_EXPORT DescribeFaceGroupsResult : public ServiceResult
			{
			public:
				struct Group
				{
					std::string creationTime;
					std::string groupId;
					std::string name;
				};


				DescribeFaceGroupsResult();
				explicit DescribeFaceGroupsResult(const std::string &payload);
				~DescribeFaceGroupsResult();
				long getTotalNum()const;
				long getPageSize()const;
				long getCurrentPage()const;
				std::vector<Group> getGroups()const;
				std::string getNextPageToken()const;

			protected:
				void parse(const std::string &payload);
			private:
				long totalNum_;
				long pageSize_;
				long currentPage_;
				std::vector<Group> groups_;
				std::string nextPageToken_;

			};
		}
	}
}
#endif // !ALIBABACLOUD_IVISION_MODEL_DESCRIBEFACEGROUPSRESULT_H_