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

#ifndef ALIBABACLOUD_IMM_MODEL_GETCONTENTKEYREQUEST_H_
#define ALIBABACLOUD_IMM_MODEL_GETCONTENTKEYREQUEST_H_

#include <string>
#include <vector>
#include <alibabacloud/core/RpcServiceRequest.h>
#include <alibabacloud/imm/ImmExport.h>

namespace AlibabaCloud
{
	namespace Imm
	{
		namespace Model
		{
			class ALIBABACLOUD_IMM_EXPORT GetContentKeyRequest : public RpcServiceRequest
			{

			public:
				GetContentKeyRequest();
				~GetContentKeyRequest();

				std::string getProject()const;
				void setProject(const std::string& project);
				std::string getAccessKeyId()const;
				void setAccessKeyId(const std::string& accessKeyId);
				std::string getVersionId()const;
				void setVersionId(const std::string& versionId);
				std::string getDRMServerId()const;
				void setDRMServerId(const std::string& dRMServerId);
				std::string getKeyIds()const;
				void setKeyIds(const std::string& keyIds);

            private:
				std::string project_;
				std::string accessKeyId_;
				std::string versionId_;
				std::string dRMServerId_;
				std::string keyIds_;

			};
		}
	}
}
#endif // !ALIBABACLOUD_IMM_MODEL_GETCONTENTKEYREQUEST_H_