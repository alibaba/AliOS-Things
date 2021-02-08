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

#ifndef ALIBABACLOUD_EAS_MODEL_LISTTASKSREQUEST_H_
#define ALIBABACLOUD_EAS_MODEL_LISTTASKSREQUEST_H_

#include <string>
#include <vector>
#include <alibabacloud/core/RoaServiceRequest.h>
#include <alibabacloud/eas/EasExport.h>

namespace AlibabaCloud
{
	namespace Eas
	{
		namespace Model
		{
			class ALIBABACLOUD_EAS_EXPORT ListTasksRequest : public RoaServiceRequest
			{

			public:
				ListTasksRequest();
				~ListTasksRequest();

				std::string getFilter()const;
				void setFilter(const std::string& filter);
				std::string getPageSize()const;
				void setPageSize(const std::string& pageSize);
				std::string getSort()const;
				void setSort(const std::string& sort);
				std::string getPageNum()const;
				void setPageNum(const std::string& pageNum);

            private:
				std::string filter_;
				std::string pageSize_;
				std::string sort_;
				std::string pageNum_;

			};
		}
	}
}
#endif // !ALIBABACLOUD_EAS_MODEL_LISTTASKSREQUEST_H_