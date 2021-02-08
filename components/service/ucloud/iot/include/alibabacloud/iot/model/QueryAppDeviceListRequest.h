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

#ifndef ALIBABACLOUD_IOT_MODEL_QUERYAPPDEVICELISTREQUEST_H_
#define ALIBABACLOUD_IOT_MODEL_QUERYAPPDEVICELISTREQUEST_H_

#include <string>
#include <vector>
#include <alibabacloud/core/RpcServiceRequest.h>
#include <alibabacloud/iot/IotExport.h>

namespace AlibabaCloud
{
	namespace Iot
	{
		namespace Model
		{
			class ALIBABACLOUD_IOT_EXPORT QueryAppDeviceListRequest : public RpcServiceRequest
			{
			public:
				struct TagList
				{
					std::string tagName;
					std::string tagValue;
				};

			public:
				QueryAppDeviceListRequest();
				~QueryAppDeviceListRequest();

				int getCurrentPage()const;
				void setCurrentPage(int currentPage);
				std::vector<TagList> getTagList()const;
				void setTagList(const std::vector<TagList>& tagList);
				std::vector<std::string> getProductKeyList()const;
				void setProductKeyList(const std::vector<std::string>& productKeyList);
				std::vector<std::string> getCategoryKeyList()const;
				void setCategoryKeyList(const std::vector<std::string>& categoryKeyList);
				std::string getIotInstanceId()const;
				void setIotInstanceId(const std::string& iotInstanceId);
				int getPageSize()const;
				void setPageSize(int pageSize);
				std::string getAppKey()const;
				void setAppKey(const std::string& appKey);

            private:
				int currentPage_;
				std::vector<TagList> tagList_;
				std::vector<std::string> productKeyList_;
				std::vector<std::string> categoryKeyList_;
				std::string iotInstanceId_;
				int pageSize_;
				std::string appKey_;

			};
		}
	}
}
#endif // !ALIBABACLOUD_IOT_MODEL_QUERYAPPDEVICELISTREQUEST_H_