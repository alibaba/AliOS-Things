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

#ifndef ALIBABACLOUD_LIVE_MODEL_COPYCASTERREQUEST_H_
#define ALIBABACLOUD_LIVE_MODEL_COPYCASTERREQUEST_H_

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
			class ALIBABACLOUD_LIVE_EXPORT CopyCasterRequest : public RpcServiceRequest
			{

			public:
				CopyCasterRequest();
				~CopyCasterRequest();

				std::string getClientToken()const;
				void setClientToken(const std::string& clientToken);
				std::string getCasterName()const;
				void setCasterName(const std::string& casterName);
				std::string getSrcCasterId()const;
				void setSrcCasterId(const std::string& srcCasterId);
				long getOwnerId()const;
				void setOwnerId(long ownerId);

            private:
				std::string clientToken_;
				std::string casterName_;
				std::string srcCasterId_;
				long ownerId_;

			};
		}
	}
}
#endif // !ALIBABACLOUD_LIVE_MODEL_COPYCASTERREQUEST_H_