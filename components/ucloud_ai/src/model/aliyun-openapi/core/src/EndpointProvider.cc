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

#include <algorithm>
#include <alibabacloud/core/EndpointProvider.h>
#include <condition_variable>
#include <iomanip>
#include <json/json.h>
#include <mutex>
#include <sstream>
#include <thread>
#ifdef USE_AOS_TIME_POSIX_API
#include <posix/timer.h>
#endif
#ifndef WIN32
#include "LocalEndpoints.h"
#include <alibabacloud/core/Utils.h>
#else
#include "LocalEndpointsForWindows.h"
#endif

namespace AlibabaCloud {

namespace {
#if defined(WIN32) && defined(_MSC_VER)
#define strcasecmp _stricmp
#define strncasecmp _strnicmp
#else
#include <strings.h>
#endif

std::mutex mutex;
std::condition_variable cv;
bool local_endpoints_loaded = false;
bool local_endpoints_loading = false;
typedef std::string productType;
typedef std::string regionType;
typedef std::string endpointType;
typedef std::string mappingType;
typedef std::vector<regionType> regionsType;
typedef std::map<productType, endpointType> regionalType;
typedef struct {
  regionsType regions;
  regionalType regional;
} productInfoType;

static std::vector<regionType> allRegions;
static std::vector<productType> allProductsInLocalEndpoints;
static std::map<productType, productInfoType> allLocalEndpoints;

static void LoadLocalEndpoints() {
  Json::Reader reader;
  Json::Value value;
  std::unique_lock<std::mutex> lock(mutex);

  if (local_endpoints_loaded) {
    return;
  }

#ifdef WIN32
  std::string LOCAL_ENDPOINTS_CONFIG = WIN_LOCAL_ENDPOINTS_CONFIG_1 +
                                       WIN_LOCAL_ENDPOINTS_CONFIG_2 +
                                       WIN_LOCAL_ENDPOINTS_CONFIG_3;
#endif

  if (!reader.parse(LOCAL_ENDPOINTS_CONFIG, value)) {
    return;
  }

  cv.wait(lock, [] {
    return !local_endpoints_loading;
  }); // continue if loading completed

  local_endpoints_loading = true;

  auto regions = value["regions"];
  for (const auto &region : regions) {
    allRegions.push_back(region.asString());
  }

  auto products = value["products"];
  for (const auto &product : products) {
    allProductsInLocalEndpoints.push_back(product.asString());
  }

  auto endpoints = value["endpoints"];
  for (auto &product : allProductsInLocalEndpoints) {
    auto endpoint_per_product = endpoints[product];
    productInfoType p;

    auto regions = endpoint_per_product["regions"];
    auto regional = endpoint_per_product["regional"];

    for (auto &r : regions) {
      const std::string region = r.asString();
      p.regions.push_back(region);
      p.regional[region] = endpoint_per_product["regional"][region].asString();
    }
    allLocalEndpoints[product] = p;
  }
  local_endpoints_loaded = true;
  local_endpoints_loading = false;

  lock.unlock();
  cv.notify_one();
}

} // namespace

EndpointProvider::EndpointProvider(
    const std::shared_ptr<Location::LocationClient> &locationClient,
    const std::string regionId, const std::string product,
    const std::string serviceCode, int durationSeconds)
    : LocationClient(locationClient), regionId_(regionId), product_(product),
      serviceCode_(serviceCode), durationSeconds_(durationSeconds),
      cachedMutex_(), cachedEndpoint_(), expiry_() {
  transform(product_.begin(), product_.end(), product_.begin(), ::tolower);
  loadLocalProductsInfo();
}

EndpointProvider::EndpointProvider(const Credentials &credentials,
                                   const ClientConfiguration &configuration,
                                   const std::string &regionId,
                                   const std::string &product,
                                   const std::string &serviceCode,
                                   int durationSeconds)
    : LocationClient(credentials, configuration), regionId_(regionId),
      product_(product), serviceCode_(serviceCode),
      durationSeconds_(durationSeconds), cachedMutex_(), cachedEndpoint_(),
      expiry_() {
  transform(product_.begin(), product_.end(), product_.begin(), ::tolower);
  loadLocalProductsInfo();
}

EndpointProvider::~EndpointProvider() {}

bool EndpointProvider::loadLocalProductsInfo() {
  LoadLocalEndpoints();
  return true;
}

std::string EndpointProvider::localEndpoint(const std::string regionId,
                                            const std::string product) {

  if (!local_endpoints_loaded) {
    // impossible
    return std::string();
  }

  std::vector<regionType>::iterator allRegionsit;
  allRegionsit = std::find(allRegions.begin(), allRegions.end(), regionId);
  if (allRegionsit == allRegions.end()) {
    return std::string();
  }

  std::vector<productType>::iterator allProductsInLocalEndpointsit;
  allProductsInLocalEndpointsit =
      std::find(allProductsInLocalEndpoints.begin(),
                allProductsInLocalEndpoints.end(), product);
  if (allProductsInLocalEndpointsit == allProductsInLocalEndpoints.end()) {
    return std::string();
  }

  std::vector<regionType> vec = allLocalEndpoints[product].regions;
  std::vector<regionType>::iterator it;
  it = std::find(vec.begin(), vec.end(), regionId);
  if (it == vec.end()) {
    return std::string();
  }
  return allLocalEndpoints[product].regional[regionId];
}

bool EndpointProvider::checkExpiry() const {
  auto now = std::chrono::system_clock::now();
  auto diff =
      std::chrono::duration_cast<std::chrono::seconds>(now - expiry_).count();
  return (diff > 0 - 60);
}

EndpointProvider::EndpointOutcome EndpointProvider::getEndpoint() {
  // 1st priority: user specified via configuration
  if (!configuration().endpoint().empty()) {
    return EndpointOutcome(configuration().endpoint());
  }

  // 2nd priority: local configuration
  std::string endpoint = localEndpoint(regionId_, product_);
  if (!endpoint.empty()) {
    return EndpointOutcome(endpoint);
  }

  // service code is mandatory for location service.
  if (serviceCode_.empty()) {
    return EndpointOutcome(
        Error("InvalidRegionId", "Product[" + product_ + "] at region[" +
                                     regionId_ + "] does not exist."));
  }

  // 3rd priority: request from location service
  EndpointOutcome outcome = loadRemoteEndpoint();
  if (outcome.isSuccess()) {
    return outcome;
  }

  if (outcome.error().errorCode() == "Illegal Parameter") {
    return EndpointOutcome(Error("InvalidProduct", "Prodcut[" + serviceCode_ +
                                                       "] does not exist."));
  }
  return outcome;
}

EndpointProvider::EndpointOutcome EndpointProvider::loadRemoteEndpoint() {
  if (checkExpiry()) {
    std::lock_guard<std::mutex> locker(cachedMutex_);
    if (checkExpiry()) {
      Location::Model::DescribeEndpointsRequest request;
      request.setId(regionId_);
      request.setServiceCode(serviceCode_);
      request.setType("openAPI");
      auto outcome = describeEndpoints(request);
      if (!outcome.isSuccess())
        return EndpointOutcome(outcome.error());

      auto all = outcome.result().endpoints();
      if (all.size() > 0)
        cachedEndpoint_ = all.front().endpoint;
#ifdef USE_AOS_TIME_POSIX_API
      struct timespec currentTime;
      time_t t;
      clock_gettime(CLOCK_REALTIME, &currentTime);
      t = currentTime.tv_nsec/1000000000 + currentTime.tv_sec + durationSeconds_;
#else
      std::time_t t = std::time(nullptr) + durationSeconds_;
#endif
      expiry_ = std::chrono::system_clock::from_time_t(t);
    }
  }
  return EndpointOutcome(cachedEndpoint_);
}

} // namespace AlibabaCloud
