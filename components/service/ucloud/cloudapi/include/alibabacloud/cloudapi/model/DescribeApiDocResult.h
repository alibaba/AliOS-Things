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

#ifndef ALIBABACLOUD_CLOUDAPI_MODEL_DESCRIBEAPIDOCRESULT_H_
#define ALIBABACLOUD_CLOUDAPI_MODEL_DESCRIBEAPIDOCRESULT_H_

#include <string>
#include <vector>
#include <utility>
#include <alibabacloud/core/ServiceResult.h>
#include <alibabacloud/cloudapi/CloudAPIExport.h>

namespace AlibabaCloud
{
	namespace CloudAPI
	{
		namespace Model
		{
			class ALIBABACLOUD_CLOUDAPI_EXPORT DescribeApiDocResult : public ServiceResult
			{
			public:
				struct RequestConfig
				{
					std::string requestPath;
					std::string requestMode;
					std::string requestProtocol;
					std::string requestHttpMethod;
					std::string postBodyDescription;
					std::string bodyFormat;
				};
				struct ErrorCodeSample
				{
					std::string description;
					std::string message;
					std::string code;
				};
				struct ResultDescription
				{
					std::string type;
					std::string description;
					std::string pid;
					std::string id;
					bool hasChild;
					std::string key;
					std::string name;
					bool mandatory;
				};
				struct RequestParameter
				{
					std::string arrayItemsType;
					std::string regularExpression;
					std::string parameterType;
					std::string jsonScheme;
					std::string description;
					std::string apiParameterName;
					std::string enumValue;
					long minLength;
					long maxValue;
					long maxLength;
					std::string demoValue;
					std::string defaultValue;
					std::string required;
					int docOrder;
					long minValue;
					std::string docShow;
					std::string location;
				};


				DescribeApiDocResult();
				explicit DescribeApiDocResult(const std::string &payload);
				~DescribeApiDocResult();
				std::string getGroupName()const;
				RequestConfig getRequestConfig()const;
				std::string getDescription()const;
				std::string getResultSample()const;
				bool getDisableInternet()const;
				std::string getStageName()const;
				std::string getApiName()const;
				bool getForceNonceCheck()const;
				std::string getResultType()const;
				std::string getFailResultSample()const;
				std::vector<ErrorCodeSample> getErrorCodeSamples()const;
				std::vector<RequestParameter> getRequestParameters()const;
				std::string getGroupId()const;
				std::string getDeployedTime()const;
				std::vector<ResultDescription> getResultDescriptions()const;
				std::string getVisibility()const;
				std::string getRegionId()const;
				std::string getAuthType()const;
				std::string getApiId()const;

			protected:
				void parse(const std::string &payload);
			private:
				std::string groupName_;
				RequestConfig requestConfig_;
				std::string description_;
				std::string resultSample_;
				bool disableInternet_;
				std::string stageName_;
				std::string apiName_;
				bool forceNonceCheck_;
				std::string resultType_;
				std::string failResultSample_;
				std::vector<ErrorCodeSample> errorCodeSamples_;
				std::vector<RequestParameter> requestParameters_;
				std::string groupId_;
				std::string deployedTime_;
				std::vector<ResultDescription> resultDescriptions_;
				std::string visibility_;
				std::string regionId_;
				std::string authType_;
				std::string apiId_;

			};
		}
	}
}
#endif // !ALIBABACLOUD_CLOUDAPI_MODEL_DESCRIBEAPIDOCRESULT_H_