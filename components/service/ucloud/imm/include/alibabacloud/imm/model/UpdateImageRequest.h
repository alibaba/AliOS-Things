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

#ifndef ALIBABACLOUD_IMM_MODEL_UPDATEIMAGEREQUEST_H_
#define ALIBABACLOUD_IMM_MODEL_UPDATEIMAGEREQUEST_H_

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
			class ALIBABACLOUD_IMM_EXPORT UpdateImageRequest : public RpcServiceRequest
			{

			public:
				UpdateImageRequest();
				~UpdateImageRequest();

				std::string getProject()const;
				void setProject(const std::string& project);
				std::string getExternalId()const;
				void setExternalId(const std::string& externalId);
				std::string getAccessKeyId()const;
				void setAccessKeyId(const std::string& accessKeyId);
				std::string getSourceType()const;
				void setSourceType(const std::string& sourceType);
				std::string getRemarksB()const;
				void setRemarksB(const std::string& remarksB);
				std::string getRemarksA()const;
				void setRemarksA(const std::string& remarksA);
				std::string getImageUri()const;
				void setImageUri(const std::string& imageUri);
				std::string getRemarksArrayA()const;
				void setRemarksArrayA(const std::string& remarksArrayA);
				std::string getRemarksArrayB()const;
				void setRemarksArrayB(const std::string& remarksArrayB);
				std::string getSourceUri()const;
				void setSourceUri(const std::string& sourceUri);
				std::string getSourcePosition()const;
				void setSourcePosition(const std::string& sourcePosition);
				std::string getRemarksD()const;
				void setRemarksD(const std::string& remarksD);
				std::string getRemarksC()const;
				void setRemarksC(const std::string& remarksC);
				std::string getSetId()const;
				void setSetId(const std::string& setId);

            private:
				std::string project_;
				std::string externalId_;
				std::string accessKeyId_;
				std::string sourceType_;
				std::string remarksB_;
				std::string remarksA_;
				std::string imageUri_;
				std::string remarksArrayA_;
				std::string remarksArrayB_;
				std::string sourceUri_;
				std::string sourcePosition_;
				std::string remarksD_;
				std::string remarksC_;
				std::string setId_;

			};
		}
	}
}
#endif // !ALIBABACLOUD_IMM_MODEL_UPDATEIMAGEREQUEST_H_