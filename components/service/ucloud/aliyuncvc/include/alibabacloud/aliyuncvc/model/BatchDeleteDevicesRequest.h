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

#ifndef ALIBABACLOUD_ALIYUNCVC_MODEL_BATCHDELETEDEVICESREQUEST_H_
#define ALIBABACLOUD_ALIYUNCVC_MODEL_BATCHDELETEDEVICESREQUEST_H_

#include <string>
#include <vector>
#include <alibabacloud/core/RpcServiceRequest.h>
#include <alibabacloud/aliyuncvc/AliyuncvcExport.h>

namespace AlibabaCloud
{
	namespace Aliyuncvc
	{
		namespace Model
		{
			class ALIBABACLOUD_ALIYUNCVC_EXPORT BatchDeleteDevicesRequest : public RpcServiceRequest
			{

			public:
				BatchDeleteDevicesRequest();
				~BatchDeleteDevicesRequest();

				std::string getGroupId()const;
				void setGroupId(const std::string& groupId);
				std::string getSN()const;
				void setSN(const std::string& sN);

            private:
				std::string groupId_;
				std::string sN_;

			};
		}
	}
}
#endif // !ALIBABACLOUD_ALIYUNCVC_MODEL_BATCHDELETEDEVICESREQUEST_H_