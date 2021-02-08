#!/bin/bash

curl -fsSLO --compressed "https://raw.githubusercontent.com/aliyun/aliyun-openapi-python-sdk/master/aliyun-python-sdk-core/aliyunsdkcore/data/endpoints.json"

DIR=`pwd`

node convert_endpoints.js > $DIR/../core/src/LocalEndpoints.h
