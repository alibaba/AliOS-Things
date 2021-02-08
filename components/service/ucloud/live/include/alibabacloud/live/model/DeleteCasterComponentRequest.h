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

#ifndef ALIBABACLOUD_LIVE_MODEL_DELETECASTERCOMPONENTREQUEST_H_
#define ALIBABACLOUD_LIVE_MODEL_DELETECASTERCOMPONENTREQUEST_H_

#include <string>
#include <vector>
#include <alibabacloud/core/RpcServiceRequest.h>
#include <alibabacloud/live/LiveExport.h>

namespace AlibabaCloud
{
	namespace Live
	{
		namespace Model
		{
			class ALIBABACLOUD_LIVE_EXPORT DeleteCasterComponentRequest : public RpcServiceRequest
			{

			public:
				DeleteCasterComponentRequest();
				~DeleteCasterComponentRequest();

				std::string getComponentId()const;
				void setComponentId(const std::string& componentId);
				std::string getCasterId()const;
				void setCasterId(const std::string& casterId);
				long getOwnerId()const;
				void setOwnerId(long ownerId);

            private:
				std::string componentId_;
				std::string casterId_;
				long ownerId_;

			};
		}
	}
}
#endif // !ALIBABACLOUD_LIVE_MODEL_DELETECASTERCOMPONENTREQUEST_H_