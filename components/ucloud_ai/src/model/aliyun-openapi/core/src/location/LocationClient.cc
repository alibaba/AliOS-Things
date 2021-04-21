/*
 * Copyright 1999-2019 Alibaba Cloud All rights reserved.
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

#include <alibabacloud/core/SimpleCredentialsProvider.h>
#include <alibabacloud/core/location/LocationClient.h>

using namespace AlibabaCloud;
using namespace AlibabaCloud::Location;

namespace {
const std::string SERVICE_NAME = "Location";
const std::string ENDPOINT = "location.aliyuncs.com";
} // namespace

LocationClient::LocationClient(const Credentials &credentials,
                               const ClientConfiguration &configuration)
    : RpcServiceClient(SERVICE_NAME,
                       std::make_shared<SimpleCredentialsProvider>(credentials),
                       configuration) {}

LocationClient::LocationClient(
    const std::shared_ptr<CredentialsProvider> &credentialsProvider,
    const ClientConfiguration &configuration)
    : RpcServiceClient(SERVICE_NAME, credentialsProvider, configuration) {}

LocationClient::LocationClient(const std::string &accessKeyId,
                               const std::string &accessKeySecret,
                               const ClientConfiguration &configuration)
    : RpcServiceClient(SERVICE_NAME,
                       std::make_shared<SimpleCredentialsProvider>(
                           accessKeyId, accessKeySecret),
                       configuration) {}

LocationClient::LocationClient(
    const std::shared_ptr<Location::LocationClient> &locationClient)
    : RpcServiceClient(SERVICE_NAME, locationClient->credentialsProvider(),
                       locationClient->configuration()) {}

LocationClient::~LocationClient() {}

LocationClient::DescribeEndpointsOutcome LocationClient::describeEndpoints(
    const Model::DescribeEndpointsRequest &request) const {
  auto outcome = makeRequest(ENDPOINT, request);

  if (outcome.isSuccess())
    return DescribeEndpointsOutcome(
        Model::DescribeEndpointsResult(outcome.result()));
  else
    return DescribeEndpointsOutcome(outcome.error());
}

void LocationClient::describeEndpointsAsync(
    const Model::DescribeEndpointsRequest &request,
    const DescribeEndpointsAsyncHandler &handler,
    const std::shared_ptr<const AsyncCallerContext> &context) const {
  std::async(std::launch::async, [this, request, handler, context]() {
    handler(this, request, describeEndpoints(request), context);
  });
}

LocationClient::DescribeEndpointsOutcomeCallable
LocationClient::describeEndpointsCallable(
    const Model::DescribeEndpointsRequest &request) const {
  auto task = std::make_shared<std::packaged_task<DescribeEndpointsOutcome()>>(
      [this, request]() { return this->describeEndpoints(request); });

  asyncExecute(new Runnable([task]() { (*task)(); }));
  return task->get_future();
}
